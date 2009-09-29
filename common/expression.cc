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

#include <common/expression.hh>
#include <utils/destringify.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/sequence-impl.hh>
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
    Visits<Call>::~Visits()
    {
    }

    template <>
    struct Implementation<Call>
    {
        std::string function;

        Sequence<ExpressionPtr> parameters;

        Implementation(const std::string & function, Sequence<ExpressionPtr> parameters) :
            function(function),
            parameters(parameters)
        {
        }
    };

    Call::Call(const std::string & function, Sequence<ExpressionPtr> parameters) :
        PrivateImplementationPattern<Call>(new Implementation<Call>(function, parameters))
    {
    }

    Call::~Call()
    {
    }

    void
    Call::accept(ExpressionVisitor & visitor)
    {
        static_cast<Visits<Call> *>(&visitor)->visit(*this);
    }

    ExpressionPtr
    Call::left_hand_side() const
    {
        return ExpressionPtr();
    }

    ExpressionPtr
    Call::right_hand_side() const
    {
        return ExpressionPtr();
    }

    const std::string &
    Call::function() const
    {
        return _imp->function;
    }

    Sequence<ExpressionPtr>
    Call::parameters() const
    {
        return _imp->parameters;
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
    Visits<Product>::~Visits()
    {
    }

    template <>
    struct Implementation<Product>
    {
        ExpressionPtr lhs;

        ExpressionPtr rhs;

        Implementation(const ExpressionPtr & lhs, const ExpressionPtr & rhs) :
            lhs(lhs),
            rhs(rhs)
        {
        }
    };

    Product::Product(const ExpressionPtr & lhs, const ExpressionPtr & rhs) :
        PrivateImplementationPattern<Product>(new Implementation<Product>(lhs, rhs))
    {
    }

    Product::~Product()
    {
    }

    void
    Product::accept(ExpressionVisitor & visitor)
    {
        static_cast<Visits<Product> *>(&visitor)->visit(*this);
    }

    ExpressionPtr
    Product::left_hand_side() const
    {
        return _imp->lhs;
    }

    ExpressionPtr
    Product::right_hand_side() const
    {
        return _imp->rhs;
    }

    template <>
    Visits<Quotient>::~Visits()
    {
    }

    template <>
    struct Implementation<Quotient>
    {
        ExpressionPtr lhs;

        ExpressionPtr rhs;

        Implementation(const ExpressionPtr & lhs, const ExpressionPtr & rhs) :
            lhs(lhs),
            rhs(rhs)
        {
        }
    };

    Quotient::Quotient(const ExpressionPtr & lhs, const ExpressionPtr & rhs) :
        PrivateImplementationPattern<Quotient>(new Implementation<Quotient>(lhs, rhs))
    {
    }

    Quotient::~Quotient()
    {
    }

    void
    Quotient::accept(ExpressionVisitor & visitor)
    {
        static_cast<Visits<Quotient> *>(&visitor)->visit(*this);
    }

    ExpressionPtr
    Quotient::left_hand_side() const
    {
        return _imp->lhs;
    }

    ExpressionPtr
    Quotient::right_hand_side() const
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
        Number value;

        Implementation(const Number & value) :
            value(value)
        {
        }
    };

    Value::Value(const Number & value) :
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

    Number
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

    unsigned long dehexify(const std::string & s)
    {
        unsigned long result;
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
            result = new Value(Number(first));
        }
        else if (is_hex_value(first))
        {
            result = new Value(Number(dehexify(first)));
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
                rhs = new Value(Number(operand));
            }
            else if (is_hex_value(first))
            {
                result = new Value(Number(dehexify(first)));
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

        std::vector<Number> stack;

        Implementation(const ExpressionEvaluator::LookupFunction & lookup) :
            lookup(lookup)
        {
        }

        virtual ~Implementation()
        {
        }

        void clear()
        {
            stack.clear();
        }

        virtual void visit(Call & c)
        {
            throw InternalError("common", "Call expression encountered in ExpressionEvaluator");
        }

        virtual void visit(Difference & d)
        {
            d.left_hand_side()->accept(*this);
            d.right_hand_side()->accept(*this);

            Number subtrahend(stack.back());
            stack.pop_back();

            Number minuend(stack.back());

            stack.back() = minuend - subtrahend;
        }

        virtual void visit(Product & p)
        {
            p.left_hand_side()->accept(*this);
            p.right_hand_side()->accept(*this);

            Number factor(stack.back());
            stack.pop_back();

            stack.back() = stack.back() * factor;
        }

        virtual void visit(Quotient & q)
        {
            q.left_hand_side()->accept(*this);
            q.right_hand_side()->accept(*this);

            Number factor(stack.back());
            stack.pop_back();

            stack.back() = stack.back() / factor;
        }

        virtual void visit(Sum & s)
        {
            s.left_hand_side()->accept(*this);
            s.right_hand_side()->accept(*this);

            Number addend(stack.back());
            stack.pop_back();

            Number augent(stack.back());

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

    Number
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
    ExpressionPrinter::visit(Call & c)
    {
        _imp->output += "@" + c.function() + "(";

        ExpressionPrinter e;
        Sequence<ExpressionPtr>::Iterator i(c.parameters().begin()), i_end(c.parameters().end());

        if (i != i_end)
        {
            _imp->output += e.print(*i);
            ++i;
        }

        for ( ; i != i_end ; ++i)
        {
            _imp->output += "," + e.print(*i);
        }

        _imp->output += ")";
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
    ExpressionPrinter::visit(Product & p)
    {
        _imp->output += "*(";

        p.left_hand_side()->accept(*this);

        _imp->output += ",";

        p.right_hand_side()->accept(*this);

        _imp->output += ")";
    }

    void
    ExpressionPrinter::visit(Quotient & q)
    {
        _imp->output += "/(";

        q.left_hand_side()->accept(*this);

        _imp->output += ",";

        q.right_hand_side()->accept(*this);

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

