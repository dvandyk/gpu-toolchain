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

#include <common/expression.hh>
#include <utils/destringify.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/stringify.hh>
#include <utils/text_manipulation.hh>

#include <string>
#include <vector>

namespace gpu
{
    Expression::~Expression()
    {
    }

    template <>
    Visits<Difference>::~Visits()
    {
    }

    template <>
    struct Implementation<Difference>
    {
        ExpressionPtr lhs;

        ExpressionPtr rhs;

        Implementation(const ExpressionPtr & lhs, const ExpressionPtr & rhs) :
            lhs(lhs),
            rhs(rhs)
        {
        }
    };

    Difference::Difference(const ExpressionPtr & lhs, const ExpressionPtr & rhs) :
        PrivateImplementationPattern<Difference>(new Implementation<Difference>(lhs, rhs))
    {
    }

    Difference::~Difference()
    {
    }

    void
    Difference::accept(ExpressionVisitor & visitor)
    {
        static_cast<Visits<Difference> *>(&visitor)->visit(*this);
    }

    ExpressionPtr
    Difference::left_hand_side() const
    {
        return _imp->lhs;
    }

    ExpressionPtr
    Difference::right_hand_side() const
    {
        return _imp->rhs;
    }

    template <>
    Visits<Sum>::~Visits()
    {
    }

    template <>
    struct Implementation<Sum>
    {
        ExpressionPtr lhs;

        ExpressionPtr rhs;

        Implementation(const ExpressionPtr & lhs, const ExpressionPtr & rhs) :
            lhs(lhs),
            rhs(rhs)
        {
        }
    };

    Sum::Sum(const ExpressionPtr & lhs, const ExpressionPtr & rhs) :
        PrivateImplementationPattern<Sum>(new Implementation<Sum>(lhs, rhs))
    {
    }

    Sum::~Sum()
    {
    }

    void
    Sum::accept(ExpressionVisitor & visitor)
    {
        static_cast<Visits<Sum> *>(&visitor)->visit(*this);
    }

    ExpressionPtr
    Sum::left_hand_side() const
    {
        return _imp->lhs;
    }

    ExpressionPtr
    Sum::right_hand_side() const
    {
        return _imp->rhs;
    }

    template <>
    Visits<Value>::~Visits()
    {
    }

    template <>
    struct Implementation<Value>
    {
        unsigned value;

        Implementation(unsigned value) :
            value(value)
        {
        }
    };

    Value::Value(unsigned value) :
        PrivateImplementationPattern<Value>(new Implementation<Value>(value))
    {
    }

    Value::~Value()
    {
    }

    void
    Value::accept(ExpressionVisitor & visitor)
    {
        static_cast<Visits<Value> *>(&visitor)->visit(*this);
    }

    ExpressionPtr
    Value::left_hand_side() const
    {
        return ExpressionPtr();
    }

    ExpressionPtr
    Value::right_hand_side() const
    {
        return ExpressionPtr();
    }

    unsigned
    Value::value() const
    {
        return _imp->value;
    }

    template <>
    Visits<Variable>::~Visits()
    {
    }

    template <>
    struct Implementation<Variable>
    {
        std::string variable;

        Implementation(const std::string & variable):
            variable(variable)
        {
        }
    };

    Variable::Variable(const std::string & variable) :
        PrivateImplementationPattern<Variable>(new Implementation<Variable>(variable))
    {
    }

    Variable::~Variable()
    {
    }

    void
    Variable::accept(ExpressionVisitor & visitor)
    {
        static_cast<Visits<Variable> *>(&visitor)->visit(*this);
    }

    ExpressionPtr
    Variable::left_hand_side() const
    {
        return ExpressionPtr();
    }

    ExpressionPtr
    Variable::right_hand_side() const
    {
        return ExpressionPtr();
    }

    std::string
    Variable::variable() const
    {
        return _imp->variable;
    }
}

namespace
{
    bool is_dec_value(const std::string & input)
    {
        static std::string allowed_chars("1234567890");
        static std::string allowed_first_chars("+-" + allowed_chars);

        if (std::string::npos == allowed_first_chars.find(input[0]))
            return false;

        if (std::string::npos != input.substr(1).find_first_not_of(allowed_chars))
            return false;

        return true;
    }

    bool is_hex_value(const std::string & input)
    {
        static std::string allowed_chars("1234567890abcdefgABCDEF");

        if ("0x" != input.substr(0, 2))
            return false;

        if (std::string::npos != input.substr(2).find_first_not_of(allowed_chars))
            return false;

        return true;
    }

    long dehexify(const std::string & s)
    {
        long result;
        std::stringstream ss(s);
        ss.setf(std::ios::hex, std::ios::basefield);

        ss >> result;

        return result;
    }
}

namespace gpu
{
    ExpressionPtr
    ExpressionParser::parse(const std::string & expression)
    {
        const static std::string operators("+-");

        Expression * result(0);
        std::string input(strip_whitespaces(expression));

        if (input.empty())
            return ExpressionPtr(); // ToDo: Throw!

        std::string::size_type pos(input.find_first_of(operators));
        std::string first(input.substr(0, pos));
        input.erase(0, pos);

        if (is_dec_value(first))
        {
            result = new Value(destringify<long>(first));
        }
        else if (is_hex_value(first))
        {
            result = new Value(dehexify(first));
        }
        else
        {
            result = new Variable(strip_whitespaces(first));
        }

        while (! input.empty())
        {
            char op(input[0]);

            if (std::string::npos == operators.find(op))
                return ExpressionPtr(); // ToDo: Throw!

            input.erase(0, 1);
            pos = input.find_first_of(operators);
            std::string operand(input.substr(0, pos));
            input.erase(0, pos);

            Expression * rhs(0);
            if (is_dec_value(operand))
            {
                rhs = new Value(destringify<long>(operand));
            }
            else if (is_hex_value(operand))
            {
                rhs = new Value(dehexify(operand));
            }
            else
            {
                rhs = new Variable(strip_whitespaces(operand));
            }

            if ('+' == op)
            {
                result = new Sum(ExpressionPtr(result), ExpressionPtr(rhs));
            }
            else if ('-' == op)
            {
                result = new Difference(ExpressionPtr(result), ExpressionPtr(rhs));
            }
            else
            {
                return ExpressionPtr(); // ToDo: Throw!
            }
        }

        return ExpressionPtr(result);
    }

    template <>
    struct Implementation<ExpressionEvaluator> :
        public ExpressionVisitor
    {
        ExpressionEvaluator::LookupFunction lookup;

        std::vector<unsigned> stack;

        Implementation(const ExpressionEvaluator::LookupFunction & lookup) :
            lookup(lookup),
            stack(0)
        {
        }

        virtual ~Implementation()
        {
        }

        void clear()
        {
            stack.clear();
        }

        virtual void visit(Difference & d)
        {
            d.left_hand_side()->accept(*this);
            d.right_hand_side()->accept(*this);

            unsigned subtrahend(stack.back());
            stack.pop_back();

            unsigned minuend(stack.back());

            stack.back() = minuend - subtrahend;
        }

        virtual void visit(Sum & s)
        {
            s.left_hand_side()->accept(*this);
            s.right_hand_side()->accept(*this);

            unsigned addend(stack.back());
            stack.pop_back();

            unsigned augent(stack.back());

            stack.back() = augent + addend;
        }

        virtual void visit(Value & v)
        {
            stack.push_back(v.value());
        }

        virtual void visit(Variable & v)
        {
            stack.push_back(lookup(v.variable()));
        }
    };

    ExpressionEvaluator::ExpressionEvaluator(const ExpressionEvaluator::LookupFunction & lookup) :
        PrivateImplementationPattern<ExpressionEvaluator>(new Implementation<ExpressionEvaluator>(lookup))
    {
    }

    ExpressionEvaluator::~ExpressionEvaluator()
    {
    }

    unsigned
    ExpressionEvaluator::evaluate(const ExpressionPtr & expression)
    {
        _imp->clear();
        expression->accept(*_imp);

        return _imp->stack.front();
    }

    template <>
    struct Implementation<ExpressionPrinter>
    {
        std::string output;
    };

    ExpressionPrinter::ExpressionPrinter() :
        PrivateImplementationPattern<ExpressionPrinter>(new Implementation<ExpressionPrinter>)
    {
    }

    ExpressionPrinter::~ExpressionPrinter()
    {
    }

    std::string
    ExpressionPrinter::print(const ExpressionPtr & e)
    {
        _imp->output = "";

        if (0 != e.get())
            e->accept(*this);

        return _imp->output;
    }

    void
    ExpressionPrinter::visit(Difference & d)
    {
        _imp->output += "-(";

        d.left_hand_side()->accept(*this);

        _imp->output += ",";

        d.right_hand_side()->accept(*this);

        _imp->output += ")";
    }

    void
    ExpressionPrinter::visit(Sum & s)
    {
        _imp->output += "+(";

        s.left_hand_side()->accept(*this);

        _imp->output += ",";

        s.right_hand_side()->accept(*this);

        _imp->output += ")";
    }

    void
    ExpressionPrinter::visit(Value & v)
    {
        _imp->output += stringify(v.value());
    }

    void
    ExpressionPrinter::visit(Variable & v)
    {
        _imp->output += v.variable();
    }
}

