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
#include <utils/sequence-impl.hh>
#include <utils/tuple.hh>

#include <list>

namespace gpu
{
    template <> struct Implementation<elf::FunctionFinder>
    {
        std::list<Tuple<elf::File, elf::SymbolTable> > data;
    };

    namespace elf
    {
        Function::Function(const std::string & name, const File & file, const Section & section, unsigned offset, unsigned size) :
            file(file),
            name(name),
            offset(offset),
            section(section),
            size(size)
        {
        }

        Function::~Function()
        {
        }

        FunctionPtr
        FunctionFinder::find(const std::string & name, const Sequence<File> & files)
        {
            for (Sequence<File>::Iterator f(files.begin()), f_end(files.end()) ;
                    f != f_end ; ++f)
            {
                SymbolTable symtab(f->symbol_table());

                for (SymbolTable::Iterator s(symtab.begin()), s_end(symtab.end()) ;
                        s != s_end ; ++s)
                {
                    if (name != s->name)
                        continue;

                    File::Iterator section(f->find(s->section));

                    return FunctionPtr(new Function(name, *f, *section, s->value, s->size));
                }
            }

            return FunctionPtr();
        }
    }
}
