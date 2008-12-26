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

#ifndef GPU_GUARD_UTILS_SEQUENCE_IMPL_HH
#define GPU_GUARD_UTILS_SEQUENCE_IMPL_HH 1

#include <utils/sequence.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/wrapped_forward_iterator-impl.hh>

#include <list>

namespace gpu
{
    template <typename T_>
    struct Implementation<Sequence<T_> >
    {
        std::list<T_> list;
    };

    template <typename T_>
    Sequence<T_>::Sequence() :
        PrivateImplementationPattern<Sequence<T_> >(new Implementation<Sequence<T_> >)
    {
    }

    template <typename T_>
    Sequence<T_>::~Sequence()
    {
    }

    template <typename T_>
    typename Sequence<T_>::Iterator
    Sequence<T_>::begin() const
    {
        return Iterator(this->_imp->list.begin());
    }

    template <typename T_>
    typename Sequence<T_>::Iterator
    Sequence<T_>::end() const
    {
        return Iterator(this->_imp->list.end());
    }

    template <typename T_>
    void
    Sequence<T_>::append(const T_ & t)
    {
        this->_imp->list.push_back(t);
    }
}

#endif
