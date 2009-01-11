/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008, 2009 Danny van Dyk <danny.dyk@tu-dortmund.de>
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

#include <elf/file.hh>
#include <elf/string_table.hh>
#include <elf/symbol_table.hh>
#include <r6xx/alu_generate.hh>
#include <r6xx/cf_generate.hh>
#include <r6xx/assembler.hh>
#include <r6xx/error.hh>
#include <r6xx/section.hh>
#include <r6xx/symbol.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/sequence-impl.hh>
#include <utils/wrapped_forward_iterator-impl.hh>

#include <algorithm>
#include <list>

#include <elf.h>

namespace gpu
{
    template <>
    struct Implementation<r6xx::Assembler>
    {
        Sequence<r6xx::SectionPtr> sections;

        Sequence<r6xx::Symbol> symbols;

        elf::StringTable strtab;

        elf::SymbolTable symtab;

        Implementation() :
            symtab(strtab)
        {
        }
    };

    namespace r6xx
    {
        Assembler::Assembler(const Sequence<std::tr1::shared_ptr<AssemblyEntity> > & entities) :
            PrivateImplementationPattern<r6xx::Assembler>(new Implementation<r6xx::Assembler>)
        {
            _imp->sections = SectionConverter::convert(entities);

            _imp->symbols = SymbolScanner::scan(_imp->sections);
        }

        Assembler::~Assembler()
        {
        }

        Assembler::SectionIterator
        Assembler::begin_sections() const
        {
            return SectionIterator(_imp->sections.begin());
        }

        Assembler::SectionIterator
        Assembler::end_sections() const
        {
            return SectionIterator(_imp->sections.end());
        }

        Assembler::SymbolIterator
        Assembler::begin_symbols() const
        {
            return SymbolIterator(_imp->symbols.begin());
        }

        Assembler::SymbolIterator
        Assembler::end_symbols() const
        {
            return SymbolIterator(_imp->symbols.end());
        }

        void
        Assembler::write(const std::string & filename) const
        {
            elf::File file(elf::File::Parameters()
                    .data(ELFDATA2LSB)
                    .machine(0xA600)
                    .type(ET_REL));

            // generate symbols
            for (Sequence<Symbol>::Iterator s(_imp->symbols.begin()), s_end(_imp->symbols.end()) ;
                    s != s_end ; ++s)
            {
                if ('.' == s->name[0])
                    continue;

                std::string section;
                switch (s->section)
                {
                    case sid_alu:
                        section = ".alu";
                        break;

                    case sid_cf:
                        section = ".cf";
                        break;

                    case sid_tex:
                        section = ".tex";
                }

                elf::Symbol symbol(s->name, section);
                symbol.value = s->offset;
                symbol.size = s->size;

                _imp->symtab.append(symbol);
            }

            // generate and emit CF instructions
            file.append(cf::Generator::generate(_imp->sections, _imp->symtab, _imp->symbols));

            file.append(alu::Generator::generate(_imp->sections, _imp->symbols));

            elf::Section tex_section(elf::Section::Parameters()
                    .alignment(0x4)
                    .flags(SHF_ALLOC)
                    .name(".tex")
                    .type(SHT_PROGBITS));
            file.append(tex_section);

            // string table
            elf::Section strtab_section(elf::Section::Parameters()
                    .alignment(0x4)
                    .flags(SHF_STRINGS)
                    .name(".strtab")
                    .type(SHT_STRTAB));
            file.append(strtab_section);

            // symbol table
            elf::Section symtab_section(elf::Section::Parameters()
                    .alignment(0x4)
                    .flags(0)
                    .link(file.index(strtab_section))
                    .name(".symtab")
                    .type(SHT_SYMTAB));
            file.append(symtab_section);

            // link relocation sections
            for (elf::File::Iterator s(file.begin()), s_end(file.end()) ; s != s_end ; ++s)
            {
                if ((s->name() == ".cf.rel")
                       || (s->name() == ".alu.rel"))
                {
                    s->link(file.index(symtab_section));
                }
            }

            // emit symbols
            _imp->symtab.write(file.section_table(), symtab_section.data());

            // emit strings
            _imp->strtab.write(strtab_section.data());

            file.write(filename);
        }
    }
}
