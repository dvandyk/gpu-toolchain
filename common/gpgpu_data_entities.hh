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

#ifndef GPU_GUARD_COMMON_GPGPU_DATA_ENTITIES_HH
#define GPU_GUARD_COMMON_GPGPU_DATA_ENTITIES_HH 1

#include <common/assembly_entities-fwd.hh>
#include <common/gpgpu_data_entities-fwd.hh>
#include <utils/sequence.hh>
#include <utils/visitor.hh>

#include <string>

namespace gpu
{
    namespace common
    {
        typedef ConstVisitorTag<common::Buffer, common::Counter> DataEntities;

        typedef ConstVisitor<DataEntities> DataEntityVisitor;

        struct DataEntity :
            ConstVisitable<DataEntities>
        {
            virtual ~DataEntity() = 0;
        };

        struct Buffer :
            public DataEntity
        {
            std::string name;

            Buffer(const std::string & name);

            ~Buffer();

            void accept(DataEntityVisitor & v) const;
        };

        struct Counter :
            public DataEntity
        {
            std::string name;

            Counter(const std::string & name);

            ~Counter();

            void accept(DataEntityVisitor & v) const;
        };

        struct DataEntityConverter
        {
            static DataEntityPtr convert(const AssemblyEntityPtr &);
        };
    }
}

#endif
