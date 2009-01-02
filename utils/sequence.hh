/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008, 2009 Danny van Dyk <danny.dyk@tu-dortmund.de>
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

#ifndef GPU_GUARD_UTILS_SEQUENCE_HH
#define GPU_GUARD_UTILS_SEQUENCE_HH 1

#include <utils/private_implementation_pattern.hh>
#include <utils/wrapped_forward_iterator.hh>

namespace gpu
{
    template <typename T_>
    class Sequence :
        public PrivateImplementationPattern<Sequence<T_> >
    {
        public:
            /// \name Basic operations
            /// \{

            /// Constructor.
            Sequence();

            /// Destructor.
            ~Sequence();

            /// \}

            /// \name Iteration
            /// \{

            struct IteratorTag;
            typedef WrappedForwardIterator<IteratorTag, T_> Iterator;

            Iterator begin() const;

            Iterator end() const;

            /// \}

            void append(const T_ &);

            bool empty() const;

            const T_ & first() const;

            T_ & first();

            const T_ & last() const;

            T_ & last();

            unsigned size() const;
    };
}

#endif
