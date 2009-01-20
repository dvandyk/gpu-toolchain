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

#ifndef GPU_GUARD_ELF_SEGMENT_HH
#define GPU_GUARD_ELF_SEGMENT_HH 1

#include <utils/private_implementation_pattern.hh>

#include <string>

namespace gpu
{
    namespace elf
    {
        class File;

        class Segment :
            public PrivateImplementationPattern<elf::Segment>
        {
            public:
                class Parameters
                {
                    protected:
                        unsigned _alignment;

                        unsigned _address;

                        unsigned _flags;

                        unsigned _offset;

                        unsigned _size;

                        unsigned _type;

                    public:
                        friend class File;
                        friend class Segment;

                        Parameters & alignment(unsigned);

                        Parameters & address(unsigned);

                        Parameters & flags(unsigned);

                        Parameters & offset(unsigned);

                        Parameters & size(unsigned);

                        Parameters & type(unsigned);
                };

                Segment(const Parameters & parameters);

                ~Segment();

                const Parameters & parameters() const;
        };
    }
}

#endif
