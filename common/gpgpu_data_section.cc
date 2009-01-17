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

#include <common/gpgpu_data_entities.hh>
#include <common/gpgpu_data_section.hh>
#include <elf/section.hh>
#include <utils/sequence-impl.hh>

#include <string>

#include <elf.h>

namespace gpu
{
    namespace common
    {
        namespace internal
        {
            struct DataSymbolScanner :
                public common::DataEntityVisitor
            {
                unsigned offset;

                Sequence<elf::Symbol> symbols;

                DataSymbolScanner(const Sequence<common::DataEntityPtr> & data_entities) :
                    offset(0)
                {
                    for (Sequence<DataEntityPtr>::Iterator i(data_entities.begin()), i_end(data_entities.end()) ;
                            i != i_end ; ++i, ++offset)
                    {
                        (*i)->accept(*this);
                    }
                }

                void add_symbol(const std::string & name, unsigned type)
                {
                    elf::Symbol symbol(name);
                    symbol.section = ".gpgpu.data";
                    symbol.type = type;
                    symbol.value = offset;

                    symbols.append(symbol);
                }

                // common::DataEntityVisitor
                void visit(const common::Buffer & b)
                {
                    add_symbol(b.name, STT_OBJECT);
                }

                void visit(const common::Counter & c)
                {
                    add_symbol(c.name, STT_OBJECT);
                }
            };
        }

        GPGPUDataSection::GPGPUDataSection()
        {
        }

        GPGPUDataSection::~GPGPUDataSection()
        {
        }

        void
        GPGPUDataSection::append(const AssemblyEntityPtr & entity)
        {
            DataEntityPtr data_entity(DataEntityConverter::convert(entity));

            if (data_entity.get())
                entities.append(data_entity);
        }

        std::string
        GPGPUDataSection::name() const
        {
            return ".gpgpu.data";
        }

        Sequence<elf::Section>
        GPGPUDataSection::sections(const elf::SymbolTable &, const Sequence<elf::Symbol> &) const
        {
            Sequence<elf::Section> result;
            elf::Section data_section(elf::Section::Parameters()
                    .alignment(0)
                    .flags(0)
                    .name(".gpgpu.data")
                    .type(SHT_HIUSER - 1));

            result.append(data_section);

            return result;
        }

        Sequence<elf::Symbol>
        GPGPUDataSection::symbols() const
        {
            internal::DataSymbolScanner s(entities);

            return s.symbols;
        }
    }
}
