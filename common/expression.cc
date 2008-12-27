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
#include <utils/text_manipulation.hh>
#include <utils/private_implementation_pattern-impl.hh>

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

    bool is_value(const std::string & input)
    {
        return is_dec_value(input) || is_hex_value(input);
    }

    bool is_operator(const std::string & input)
    {
        static std::string allowed_chars("+-");

        if (1 != input.size())
            return false;

        return (std::string::npos != allowed_chars.find(input[0]));
    }
}

namespace gpu
{
    ExpressionPtr
    ExpressionParser::parse(const std::string & expression)
    {
        Expression * result(0);
        std::string stripped(strip_whitespaces(expression));

        if (stripped.empty())
            return ExpressionPtr(result); // ToDo: Throw!

        std::string operand(split_first_word(" \t", stripped));
        stripped = strip_whitespaces(stripped);

        if (is_value(operand))
        {
            result = new Value(destringify<unsigned>(operand));
        }
        else if (! is_operator(operand))
        {
            result = new Variable(operand);
        }
        else
        {
            return ExpressionPtr(result); // ToDo: Throw!
        }

        while (! stripped.empty())
        {
            std::string op(split_first_word(" \t", stripped));
            stripped = strip_whitespaces(stripped);

            if (op.empty())
                return ExpressionPtr(result); // ToDo: Throw!

            if (! is_operator(op))
                return ExpressionPtr(result); // ToDo: Throw!

            operand = split_first_word(" \t", stripped);
            stripped = strip_whitespaces(stripped);

            if (operand.empty())
                return ExpressionPtr(result); // ToDo: Throw!

            Expression * operand_expression(0);

            if (is_value(operand))
            {
                operand_expression = new Value(destringify<unsigned>(operand));
            }
            else if (! is_operator(operand))
            {
                operand_expression = new Variable(operand);
            }
            else
            {
                return ExpressionPtr(result); // ToDo: Throw!
            }

            if ('+' == op[0])
            {
                result = new Sum(ExpressionPtr(result), ExpressionPtr(operand_expression));
            }
            else if ('-' == op[0])
            {
                result = new Difference(ExpressionPtr(result), ExpressionPtr(operand_expression));
            }
            else
            {
                return ExpressionPtr(result); // ToDo: Throw!
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
}

