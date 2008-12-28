/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 Danny van Dyk <danny.dyk@tu-dortmund.de>
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

#ifndef GPU_GUARD_UTILS_ENUMERATION_HH
#define GPU_GUARD_UTILS_ENUMERATION_HH 1

#include <utils/exception.hh>

namespace gpu
{
    class EnumerationValueOutOfBoundsError :
        public Exception
    {
        public:
            EnumerationValueOutOfBoundsError(unsigned value, unsigned width);
    };

    template <unsigned width_>
    class Enumeration
    {
        private:
            unsigned _value;

        public:
            explicit Enumeration(unsigned value) :
                _value(value)
            {
                if (value >= (1 << width_))
                    throw EnumerationValueOutOfBoundsError(_value, width_);
            }

            ~Enumeration()
            {
            }

            Enumeration<width_> & operator= (const Enumeration<width_> & other)
            {
                _value = other._value;

                return *this;
            }

            operator unsigned () const
            {
                return _value;
            }
    };
}

#endif
