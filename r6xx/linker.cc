/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Danny van Dyk <danny.dyk@tu-dortmund.de>
 *
 * This file is part of the GPU Toolchain. GPU Toolchain is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * GPU Toolchain is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <elf/function.hh>
#include <elf/relocation_table.hh>
#include <r6xx/linker.hh>
#include <utils/exception.hh>
#include <utils/sequence-impl.hh>
#include <utils/tuple.hh>

#include <elf.h>

#include <iostream> // remove!
#include <list>
#include <set>
#include <string>

namespace gpu
{
    template class Sequence<elf::File>;

    namespace r6xx
    {
        namespace internal
        {
            Sequence<elf::Relocation> find_relocations(const elf::FunctionPtr & f)
            {
                elf::File::Iterator i(f->file.find(".cf.rel"));

                if (f->file.end() == i)
                    return Sequence<elf::Relocation>();

                const unsigned start(f->offset), stop(f->offset + f->size);
                Sequence<elf::Relocation> result;
                elf::SymbolTable symtab(f->file.symbol_table());
                elf::RelocationTable reltab(symtab);

                reltab.read(i->data());

                for (elf::RelocationTable::Iterator j(reltab.begin()), j_end(reltab.end()) ;
                        j != j_end ; ++j)
                {
                    if ((j->offset < start) || (stop <= j->offset))
                        continue;

                    result.append(*j);
                }

                return result;
            }
        }

        Sequence<elf::Segment>
        Linker::link(const Sequence<elf::File> & files)
        {

            std::list<std::pair<elf::FunctionPtr, Sequence<elf::Relocation> > > functions;
            std::set<std::string> needed_functions;
            std::set<std::string> handled_functions;
            needed_functions.insert("main");

            // TODO: Sequence<elf::ObjectPtr> objects;
            std::set<std::string> needed_objects;
            std::set<std::string> handled_objects;

            for (std::set<std::string>::iterator n(needed_functions.begin()) ;
                    n != needed_functions.end() ; n = needed_functions.begin())
            {
                elf::FunctionPtr f(elf::FunctionFinder::find(*n, files));
                if (0 == f.get())
                    throw InternalError("?", "Function '" + *n + "' is needed, but there is no such symbol");

                Sequence<elf::Relocation> relocations;
                if (".cf" == f->section.name())
                {
                    relocations = internal::find_relocations(f);
                    elf::SymbolTable symtab(f->file.symbol_table());

                    for (Sequence<elf::Relocation>::Iterator r(relocations.begin()), r_end(relocations.end()) ;
                            r != r_end ; ++r)
                    {
                        unsigned index(symtab[r->symbol]);
                        elf::Symbol s(symtab[index]);

                        if (STT_FUNC == s.type)
                        {
                            if (handled_functions.end() == handled_functions.find(s.name))
                            {
                                needed_functions.insert(s.name);
                            }
                        }
                        else if (STT_OBJECT == s.type)
                        {
                            if (handled_objects.end() == handled_objects.find(s.name))
                                needed_objects.insert(s.name);
                        }
                    }
                }

                functions.push_back(std::make_pair(f, relocations));
                handled_functions.insert(*n);
                needed_functions.erase(n);
            }

            unsigned offset(0);
            elf::Section cf_section(elf::Section::Parameters()
                    .alignment(0x8)
                    .flags(SHF_ALLOC | SHF_EXECINSTR)
                    .name(".cf")
                    .type(SHT_PROGBITS));
            Sequence<elf::Relocation> cf_relocations;

            for (std::list<std::pair<elf::FunctionPtr, Sequence<elf::Relocation> > >::const_iterator i(functions.begin()), i_end(functions.end()) ;
                    i != i_end ; ++i)
            {
                if (".cf" != i->first->section.name())
                    continue;

                std::cout << "Copying function '" << i->first->name << "'" << std::endl;

                // copy to cf_section
                cf_section.data().resize(offset + i->first->size);
                cf_section.data().write(offset, reinterpret_cast<const char *>(i->first->section.data().buffer()), i->first->size);

                for (Sequence<elf::Relocation>::Iterator j(i->second.begin()), j_end(i->second.end()) ;
                        j != j_end ; ++j)
                {
                    std::cout << "Relocation to symbol '" << j->symbol << "'" << std::endl;
                    std::cout << "  offset = " << j->offset << std::endl;

                    elf::Relocation r(*j);
                    r.offset -= i->first->offset; // make relative to the function
                    r.offset += offset; // make relative to the new section/segment

                    cf_relocations.append(r);
                }
            }

            // TODO: Objects

            elf::Segment segment(elf::Segment::Parameters()
                    .alignment(4));
            segment.append(cf_section);

            Sequence<elf::Segment> result;
            result.append(segment);

            return result;
        }
    }
}
