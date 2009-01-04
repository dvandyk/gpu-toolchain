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

#include <elf/data.hh>
#include <utils/exception.hh>
#include <utils/private_implementation_pattern-impl.hh>

#include <algorithm>
#include <vector>

namespace gpu
{
    template <>
    struct Implementation<elf::Data>
    {
        std::vector<char> data;

        Implementation(unsigned size) :
            data(size, 0)
        {
        }
    };

    namespace elf
    {
        Data::Data(unsigned size) :
            PrivateImplementationPattern<elf::Data>(new Implementation<elf::Data>(size))
        {
        }

        Data::~Data()
        {
        }

        const void *
        Data::buffer() const
        {
            return &_imp->data[0];
        }

        void
        Data::read(unsigned offset, char * data, unsigned size) const
        {
            if (offset + size > _imp->data.size())
                throw InternalError("elf", "out-of-bounds read");

            std::copy(_imp->data.begin() + offset, _imp->data.begin() + offset + size, data);
        }

        void
        Data::resize(unsigned size)
        {
            _imp->data.resize(size, 0);
        }

        unsigned
        Data::size() const
        {
            return _imp->data.size();
        }

        void
        Data::write(unsigned offset, const char * data, unsigned size)
        {
            if (offset + size > _imp->data.size())
                throw InternalError("elf", "out-of-bounds read");

            std::copy(data, data + size, _imp->data.begin() + offset);
        }
    }
}
