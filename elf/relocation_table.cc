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
#include <utils/wrapped_forward_iterator-impl.hh>

#include <algorithm>
#include <cstring>
#include <map>
#include <vector>

#include <elf.h>

namespace gpu
{
    template class WrappedForwardIterator<elf::RelocationTable::IteratorTag, elf::Relocation>;

    template <>
    struct Implementation<elf::RelocationTable>
    {
        std::vector<elf::Relocation> entries;

        elf::SymbolTable symtab;

        Implementation(const elf::SymbolTable & symtab) :
            symtab(symtab)
        {
        }
    };

    namespace elf
    {
        Relocation::Relocation(unsigned offset, const std::string & symbol, unsigned type, unsigned addend) :
            addend(addend),
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
        RelocationTable::append(const Relocation & entry)
        {
            _imp->entries.push_back(entry);
        }

        RelocationTable::Iterator
        RelocationTable::begin() const
        {
            return Iterator(_imp->entries.begin());
        }

        RelocationTable::Iterator
        RelocationTable::end() const
        {
            return Iterator(_imp->entries.end());
        }

        void
        RelocationTable::read(const Data & data)
        {
            const Elf32_Rela * r(reinterpret_cast<const Elf32_Rela *>(data.buffer()));
            const Elf32_Rela * r_end(r + data.size() / sizeof(Elf32_Rela));

            for ( ; r != r_end ; ++r)
            {
                unsigned symbol_index(ELF32_R_SYM(r->r_info));
                Relocation entry(r->r_offset, _imp->symtab[symbol_index].name, ELF32_R_TYPE(r->r_info), r->r_addend);

                _imp->entries.push_back(entry);
            }
        }

        void
        RelocationTable::write(Data data)
        {
            unsigned size(_imp->entries.size() * sizeof(Elf32_Rela));
            std::vector<Elf32_Rela> relocations;

            for (std::vector<Relocation>::const_iterator i(_imp->entries.begin()), i_end(_imp->entries.end()) ;
                    i != i_end ; ++i)
            {
                unsigned symbol_index(_imp->symtab[i->symbol]);
                if (0 == symbol_index)
                    throw InternalError("elf", "Relocation against unknown symbol '" + i->symbol + "'");

                Elf32_Rela r;
                r.r_addend = i->addend;
                r.r_info = ELF32_R_INFO(symbol_index, i->type);
                r.r_offset = i->offset;

                relocations.push_back(r);
            }

            data.resize(size);
            data.write(0, reinterpret_cast<char *>(&relocations[0]), size);
        }
    }
}
