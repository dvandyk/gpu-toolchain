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

#include <utils/exception.hh>
#include <utils/number.hh>
#include <utils/private_implementation_pattern-impl.hh>

#include <ostream>
#include <iostream> // <<-- Remove!

#include <gmpxx.h>

namespace gpu
{
    enum NumberValueType
    {
        nvt_float,
        nvt_signed,
        nvt_unsigned
    };

    template <>
    struct Implementation<Number>
    {
        NumberValueType type;

        mpf_class value;

        Implementation(float f) :
            type(nvt_float),
            value(f)
        {
        }

        Implementation(double d) :
            type(nvt_float),
            value(d)
        {
        }

        Implementation(signed si) :
            type(nvt_signed),
            value(si)
        {
        }

        Implementation(long sl) :
            type(nvt_signed),
            value(sl)
        {
        }

        Implementation(unsigned ui) :
            type(nvt_unsigned),
            value(ui)
        {
        }

        Implementation(unsigned long ul) :
            type(nvt_unsigned),
            value(ul)
        {
        }

        Implementation(const std::string & s)
        {
            value = mpf_class(s);
            if (std::string::npos != s.find('.'))
            {
                type = nvt_float;
            }
            else if ('-' == s[0])
            {
                type = nvt_signed;
            }
            else
            {
                type = nvt_unsigned;
            }
        }

        void adjust_type(NumberValueType other)
        {
            if (type == other)
                return;

            if ((nvt_float == type) || (nvt_float == other))
            {
                type = nvt_float;
                return;
            }

            if ((nvt_signed == type) || (nvt_signed == other))
            {
                type = nvt_signed;
                return;
            }
        }
    };

    Number::Number(float f) :
        PrivateImplementationPattern<Number>(new Implementation<Number>(f))
    {
    }

    Number::Number(double d) :
        PrivateImplementationPattern<Number>(new Implementation<Number>(d))
    {
    }

    Number::Number(int si) :
        PrivateImplementationPattern<Number>(new Implementation<Number>(si))
    {
    }

    Number::Number(long sl) :
        PrivateImplementationPattern<Number>(new Implementation<Number>(sl))
    {
    }

    Number::Number(unsigned ui) :
        PrivateImplementationPattern<Number>(new Implementation<Number>(ui))
    {
    }

    Number::Number(unsigned long ul) :
        PrivateImplementationPattern<Number>(new Implementation<Number>(ul))
    {
    }

    Number::Number(const std::string & s) :
        PrivateImplementationPattern<Number>(new Implementation<Number>(s))
    {
    }

    Number::Number(const Number & other) :
        PrivateImplementationPattern<Number>(new Implementation<Number>(*other._imp))
    {
    }

    Number::~Number()
    {
    }

    Number &
    Number::operator= (const Number & rhs)
    {
        if (&rhs == this)
            return *this;

        _imp = std::tr1::shared_ptr<Implementation<Number> >(new Implementation<Number>(*rhs._imp));

        return *this;
    }

    Number &
    Number::operator+= (const Number & rhs)
    {
        _imp->value += rhs._imp->value;
        _imp->adjust_type(rhs._imp->type);

        return *this;
    }

    Number &
    Number::operator-= (const Number & rhs)
    {
        _imp->value -= rhs._imp->value;
        _imp->adjust_type(rhs._imp->type);

        return *this;
    }

    Number &
    Number::operator*= (const Number & rhs)
    {
        _imp->value *= rhs._imp->value;
        _imp->adjust_type(rhs._imp->type);

        return *this;
    }

    Number &
    Number::operator/= (const Number & rhs)
    {
        _imp->value /= rhs._imp->value;
        _imp->adjust_type(rhs._imp->type);

        return *this;
    }

    std::tr1::shared_ptr<Implementation<Number> >
    Number::_implementation() const
    {
        return _imp;
    }

    Number operator+ (const Number & lhs, const Number & rhs)
    {
        Number result(lhs);
        result += rhs;

        return result;
    }

    Number operator- (const Number & lhs, const Number & rhs)
    {
        Number result(lhs);
        result -= rhs;

        return result;
    }

    Number operator* (const Number & lhs, const Number & rhs)
    {
        Number result(lhs);
        result *= rhs;

        return result;
    }

    Number operator/ (const Number & lhs, const Number & rhs)
    {
        Number result(lhs);
        result /= rhs;

        return result;
    }

    bool operator== (const Number & lhs, const Number & rhs)
    {
        return lhs._implementation()->value == rhs._implementation()->value;
    }

    bool operator!= (const Number & lhs, const Number & rhs)
    {
        return lhs._implementation()->value != rhs._implementation()->value;
    }

    bool operator< (const Number & lhs, const Number & rhs)
    {
        return lhs._implementation()->value < rhs._implementation()->value;
    }

    bool operator> (const Number & lhs, const Number & rhs)
    {
        return lhs._implementation()->value > rhs._implementation()->value;
    }

    template <>
    float interpret_number_as<float>(const Number & n)
    {
        switch (n._implementation()->type)
        {
            case nvt_float:
            case nvt_signed:
            case nvt_unsigned:
                return n._implementation()->value.get_d();

            default:
                throw InternalError("utils", "Trying to interpret incompatible Number as 'float'");
        };
    }

    template <>
    signed interpret_number_as<signed>(const Number & n)
    {
        switch (n._implementation()->type)
        {
            case nvt_signed:
                return n._implementation()->value.get_si();

            default:
                throw InternalError("utils", "Trying to interpret incompatible Number as 'signed'");
        };
    }

    template <>
    unsigned interpret_number_as<unsigned>(const Number & n)
    {
        switch (n._implementation()->type)
        {
            case nvt_unsigned:
                return n._implementation()->value.get_ui();

            default:
                std::cout << "Gna: " << n._implementation()->type << std::endl;
                throw InternalError("utils", "Trying to interpret incompatible Number as 'unsigned'");
        };
    }

    std::ostream & operator<< (std::ostream & lhs, const Number & rhs)
    {
        lhs << rhs._implementation()->value;

        switch (rhs._implementation()->type)
        {
            case nvt_float:
                lhs << 'f';
                break;

            case nvt_unsigned:
                lhs << 'u';
                break;
        }

        return lhs;
    }
}
