/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Ciaran McCreesh
 *
 * This file is part of the Paludis package manager. Paludis is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * Paludis is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GPU_GUARD_UTILS_WRAPPED_FORWARD_ITERATOR_HH
#define GPU_GUARD_UTILS_WRAPPED_FORWARD_ITERATOR_HH 1

#include <utils/wrapped_forward_iterator.hh>

#include <iterator>
#include <tr1/type_traits>
#include <tr1/functional>

namespace gpu
{
    /**
     * A WrappedForwardIterator is a generic wrapper around a forward iterator,
     * hiding the underlying base iterator.
     */
    template <typename Tag_, typename Value_>
    class WrappedForwardIterator
    {
        private:
            struct Base;
            template <typename T_> struct BaseImpl;

            Base * _base;

        public:
            ///\name Basic operations
            ///\{

            WrappedForwardIterator();
            ~WrappedForwardIterator();
            WrappedForwardIterator(const WrappedForwardIterator &);

            template <typename T_>
            WrappedForwardIterator(const T_ &);

            WrappedForwardIterator & operator= (const WrappedForwardIterator &);

            ///\}

            ///\name Standard library typedefs
            ///\{

            typedef typename std::tr1::remove_reference<Value_>::type & value_type;
            typedef typename std::tr1::remove_reference<Value_>::type & reference;
            typedef typename std::tr1::remove_reference<Value_>::type * pointer;
            typedef std::ptrdiff_t difference_type;
            typedef std::forward_iterator_tag iterator_category;

            ///\}

            ///\name Increment
            ///\{

            WrappedForwardIterator & operator++ ();
            WrappedForwardIterator operator++ (int);

            ///\}

            ///\name Dereference
            ///\{

            pointer operator-> () const;
            reference operator* () const;

            ///\}

            ///\name Equality
            ///\{

            bool operator== (const WrappedForwardIterator &) const;
            bool operator!= (const WrappedForwardIterator &) const;

            ///\}

            ///\name Underlying iterator
            ///\{

            template <typename T_> T_ & underlying_iterator();
            template <typename T_> const T_ & underlying_iterator() const;

            ///\}
    };
}

#endif
