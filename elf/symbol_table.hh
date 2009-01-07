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

#ifndef GPU_GUARD_ELF_SYMBOL_TABLE_HH
#define GPU_GUARD_ELF_SYMBOL_TABLE_HH 1

#include <elf/data.hh>
#include <elf/section.hh>
#include <elf/string_table.hh>
#include <utils/private_implementation_pattern.hh>

#include <string>

namespace gpu
{
    namespace elf
    {
        struct Symbol
        {
            unsigned bind;

            std::string name;

            std::string section;

            unsigned size;

            unsigned type;

            unsigned value;

            Symbol(const std::string & name, const std::string & section);

            bool operator== (const Symbol &) const;
        };

        class SymbolTable :
            public PrivateImplementationPattern<elf::SymbolTable>
        {
            public:
                SymbolTable(const StringTable & strtab);

                ~SymbolTable();

                unsigned operator[] (const std::string & name);

                void append(const Symbol & symbol);

                void write(const SectionTable & section_table, Data data);
        };
    }
}

#endif
