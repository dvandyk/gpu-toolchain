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

#include <elf/relocation_table.hh>
#include <utils/exception.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/tuple.hh>

#include <algorithm>
#include <cstring>
#include <map>
#include <vector>

#include <elf.h>

namespace gpu
{
    template <>
    struct Implementation<elf::RelocationTable>
    {
        std::vector<Elf32_Rel> entries;

        elf::SymbolTable symtab;

        Implementation(const elf::SymbolTable & symtab) :
            symtab(symtab)
        {
        }
    };

    namespace elf
    {
        Relocation::Relocation(unsigned offset, const std::string & symbol, unsigned type) :
            offset(offset),
            symbol(symbol),
            type(type)
        {
        }

        RelocationTable::RelocationTable(const SymbolTable & symtab) :
            PrivateImplementationPattern<elf::RelocationTable>(new Implementation<elf::RelocationTable>(symtab))
        {
        }

        RelocationTable::~RelocationTable()
        {
        }

        void
        RelocationTable::append(const Relocation & r)
        {
            unsigned index(_imp->entries.size());

            Elf32_Rel relocation;
            relocation.r_offset = r.offset;
            relocation.r_info = ELF32_R_INFO(_imp->symtab[r.symbol], r.type);

            _imp->entries.push_back(relocation);
        }

        void
        RelocationTable::write(Data data)
        {
            unsigned size(_imp->entries.size() * sizeof(Elf32_Rel));

            data.resize(size);
            data.write(0, reinterpret_cast<char *>(&_imp->entries[0]), size);
        }
    }
}
