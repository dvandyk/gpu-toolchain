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

#ifndef GPU_GUARD_UTILS_TUPLE_HH
#define GPU_GUARD_UTILS_TUPLE_HH 1

namespace gpu
{
    template <typename T1_, typename T2_,
                typename T3_ = void,
                typename T4_ = void>
    struct Tuple
    {
        T1_ first;

        T2_ second;

        T3_ third;

        T4_ fourth;

        Tuple(const T1_ & t1, const T2_ & t2, const T3_ & t3, const T4_ & t4) :
            first(t1),
            second(t2),
            third(t3),
            fourth(t4)
        {
        }
    };

    template <typename T1_, typename T2_, typename T3_> struct Tuple<T1_, T2_, T3_, void>
    {
        T1_ first;

        T2_ second;

        T3_ third;

        Tuple(const T1_ & t1, const T2_ & t2, const T3_ & t3) :
            first(t1),
            second(t2),
            third(t3)
        {
        }
    };

    template <typename T1_, typename T2_> struct Tuple<T1_, T2_, void, void>
    {
        T1_ first;

        T2_ second;

        Tuple(const T1_ & t1, const T2_ & t2) :
            first(t1),
            second(t2)
        {
        }
    };

    template <typename Tuple_, typename T_, unsigned> class TupleComparator;

    template <typename Tuple_, typename T_> class TupleComparator<Tuple_, T_, 1>
    {
        private:
            const T_ reference;

        public:
            TupleComparator(const T_ & reference) :
                reference(reference)
            {
            }

            ~TupleComparator()
            {
            }

            bool operator() (const Tuple_ & tuple)
            {
                return reference == tuple.first;
            }
    };
}

#endif
