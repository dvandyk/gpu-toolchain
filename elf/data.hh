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

#ifndef GPU_GUARD_ELF_DATA_HH
#define GPU_GUARD_ELF_DATA_HH 1

#include <utils/private_implementation_pattern.hh>

namespace gpu
{
    namespace elf
    {
        class Data :
            public PrivateImplementationPattern<elf::Data>
        {
            public:
                Data(unsigned size = 0);

                ~Data();

                const void * buffer() const;

                void read(unsigned offset, char * data, unsigned size) const;

                void resize(unsigned size);

                unsigned size() const;

                void write(unsigned offset, const char * data, unsigned size);
        };
    }
}

#endif
