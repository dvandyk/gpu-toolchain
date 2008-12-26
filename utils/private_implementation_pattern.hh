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

#ifndef UTILS_GUARD_PRIVATE_IMPLEMENTATION_PATTERN_HH
#define UTILS_GUARD_PRIVATE_IMPLEMENTATION_PATTERN_HH 1

#include <tr1/memory>

namespace gpu
{
    /**
     * Private implementation data, to be specialised for any class that uses
     * PrivateImplementationPattern.
     */
    template <typename T_> struct Implementation;

    /**
     * PrivateImplementationPattern is a utility base class that can be used to
     * easily create classes with private implementation.
     */

    template <typename T_> class PrivateImplementationPattern
    {
        protected:
            /// Our implementation.
            std::tr1::shared_ptr<Implementation<T_> > _imp;

        public:
            explicit PrivateImplementationPattern(Implementation<T_> * imp);

            ~PrivateImplementationPattern();
    };
}

#endif
