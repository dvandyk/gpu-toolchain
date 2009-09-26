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

#ifndef GPU_TOOLCHAIN_GUARD_UTILS_NUMBER_HH
#define GPU_TOOLCHAIN_GUARD_UTILS_NUMBER_HH 1

#include <utils/private_implementation_pattern.hh>

namespace gpu
{
    class Number;

    template <typename T_> T_ interpret_number_as(const Number &);

    template <> float interpret_number_as<float>(const Number &);

    template <> signed interpret_number_as<signed>(const Number &);

    template <> unsigned interpret_number_as<unsigned>(const Number &);

    class Number :
        public PrivateImplementationPattern<Number>
    {
        private:
            std::tr1::shared_ptr<Implementation<Number> > _implementation() const;

        public:
            friend float interpret_number_as<float>(const Number &);
            friend signed interpret_number_as<signed>(const Number &);
            friend unsigned interpret_number_as<unsigned>(const Number &);
            friend bool operator== (const Number & lhs, const Number & rhs);
            friend bool operator!= (const Number & lhs, const Number & rhs);
            friend bool operator< (const Number & lhs, const Number & rhs);
            friend bool operator> (const Number & lhs, const Number & rhs);
            friend std::ostream & operator<< (std::ostream & lhs, const Number & rhs);

            explicit Number(float);

            explicit Number(double);

            explicit Number(signed);

            explicit Number(long);

            explicit Number(unsigned);

            explicit Number(unsigned long);

            explicit Number(const std::string &);

            Number(const Number &);

            ~Number();

            Number & operator= (const Number &);

            Number & operator+= (const Number &);

            Number & operator-= (const Number &);

            Number & operator*= (const Number &);

            Number & operator/= (const Number &);
    };

    Number operator+ (const Number & lhs, const Number & rhs);

    Number operator- (const Number & lhs, const Number & rhs);

    Number operator* (const Number & lhs, const Number & rhs);

    Number operator/ (const Number & lhs, const Number & rhs);

    bool operator== (const Number & lhs, const Number & rhs);

    bool operator!= (const Number & lhs, const Number & rhs);

    bool operator< (const Number & lhs, const Number & rhs);

    bool operator> (const Number & lhs, const Number & rhs);

    std::ostream & operator<< (std::ostream & lhs, const Number & rhs);
}

#endif
