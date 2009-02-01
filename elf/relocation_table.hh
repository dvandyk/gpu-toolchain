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

#ifndef GPU_GUARD_ELF_RELOCATION_TABLE_HH
#define GPU_GUARD_ELF_RELOCATION_TABLE_HH 1

#include <elf/data.hh>
#include <elf/symbol_table.hh>
#include <utils/private_implementation_pattern.hh>
#include <utils/wrapped_forward_iterator.hh>

#include <string>

namespace gpu
{
    namespace elf
    {
        struct Relocation
        {
            unsigned addend;

            unsigned offset;

            std::string symbol;

            unsigned type;

            Relocation(unsigned offset, const std::string & symbol, unsigned type, unsigned addend);
        };

        class RelocationTable :
            public PrivateImplementationPattern<elf::RelocationTable>
        {
            public:
                RelocationTable(const elf::SymbolTable & symtab);

                ~RelocationTable();

                struct IteratorTag;
                typedef WrappedForwardIterator<IteratorTag, elf::Relocation> Iterator;

                void append(const Relocation & relocation);

                Iterator begin() const;

                Iterator end() const;

                void write(Data data);
        };
    }
}

#endif
