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

#ifndef GPU_GUARD_UTILS_VISITOR_HH
#define GPU_GUARD_UTILS_VISITOR_HH 1

namespace gpu
{
    namespace internal
    {
        template <unsigned dummy_>
        struct NoType
        {
        };
    }

    template <typename Type_>
    struct ConstVisits
    {
        virtual ~ConstVisits();

        virtual void visit(const Type_ &) = 0;
    };

    template <unsigned dummy_>
    struct ConstVisits<internal::NoType<dummy_> >
    {
    };

    template <typename T1_, typename T2_,
             typename T3_ = internal::NoType<3>,
             typename T4_ = internal::NoType<4>,
             typename T5_ = internal::NoType<5>,
             typename T6_ = internal::NoType<6>,
             typename T7_ = internal::NoType<7>,
             typename T8_ = internal::NoType<8>,
             typename T9_ = internal::NoType<9>,
             typename T10_ = internal::NoType<10> >
    struct ConstVisitorTag
    {
        typedef T1_ ConstVisitableType1;

        typedef T2_ ConstVisitableType2;

        typedef T3_ ConstVisitableType3;

        typedef T4_ ConstVisitableType4;

        typedef T5_ ConstVisitableType5;

        typedef T6_ ConstVisitableType6;

        typedef T7_ ConstVisitableType7;

        typedef T8_ ConstVisitableType8;

        typedef T9_ ConstVisitableType9;

        typedef T10_ ConstVisitableType10;
    };

    template <typename Tag_> class ConstVisitor :
        public ConstVisits<typename Tag_::ConstVisitableType1>,
        public ConstVisits<typename Tag_::ConstVisitableType2>,
        public ConstVisits<typename Tag_::ConstVisitableType3>,
        public ConstVisits<typename Tag_::ConstVisitableType4>,
        public ConstVisits<typename Tag_::ConstVisitableType5>,
        public ConstVisits<typename Tag_::ConstVisitableType6>,
        public ConstVisits<typename Tag_::ConstVisitableType7>,
        public ConstVisits<typename Tag_::ConstVisitableType8>,
        public ConstVisits<typename Tag_::ConstVisitableType9>
    {
    };

    template <typename Tag_> class ConstVisitable
    {
        public:
            virtual void accept(ConstVisitor<Tag_> &) const = 0;
    };

    template <typename Type_>
    struct Visits
    {
        virtual ~Visits();

        virtual void visit(Type_ &) = 0;
    };

    template <unsigned dummy_>
    struct Visits<internal::NoType<dummy_> >
    {
    };

    template <typename T1_, typename T2_,
             typename T3_ = internal::NoType<3>,
             typename T4_ = internal::NoType<4>,
             typename T5_ = internal::NoType<5>,
             typename T6_ = internal::NoType<6>,
             typename T7_ = internal::NoType<7>,
             typename T8_ = internal::NoType<8>,
             typename T9_ = internal::NoType<9> >
    struct VisitorTag
    {
        typedef T1_ VisitableType1;

        typedef T2_ VisitableType2;

        typedef T3_ VisitableType3;

        typedef T4_ VisitableType4;

        typedef T5_ VisitableType5;

        typedef T6_ VisitableType6;

        typedef T7_ VisitableType7;

        typedef T8_ VisitableType8;

        typedef T9_ VisitableType9;
    };

    template <typename Tag_> class Visitor :
        public Visits<typename Tag_::VisitableType1>,
        public Visits<typename Tag_::VisitableType2>,
        public Visits<typename Tag_::VisitableType3>,
        public Visits<typename Tag_::VisitableType4>,
        public Visits<typename Tag_::VisitableType5>,
        public Visits<typename Tag_::VisitableType6>,
        public Visits<typename Tag_::VisitableType7>,
        public Visits<typename Tag_::VisitableType8>,
        public Visits<typename Tag_::VisitableType9>
    {
    };

    template <typename Tag_> class Visitable
    {
        public:
            virtual void accept(Visitor<Tag_> &) = 0;
    };
}

#endif
