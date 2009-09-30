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

#ifndef GPU_GUARD_COMMON_EXPRESSION_HH
#define GPU_GUARD_COMMON_EXPRESSION_HH 1

#include <common/expression-fwd.hh>
#include <utils/memory.hh>
#include <utils/number.hh>
#include <utils/private_implementation_pattern.hh>
#include <utils/sequence.hh>
#include <utils/visitor.hh>

#include <tr1/functional>

namespace gpu
{
    typedef VisitorTag<Call, Difference, List, Power, Product, Quotient, Sum, Value, Variable> Expressions;

    typedef Visitor<Expressions> ExpressionVisitor;

    typedef std::tr1::shared_ptr<Expression> ExpressionPtr;

    class Expression :
        public Visitable<Expressions>
    {
        public:
            virtual ~Expression() = 0;

            virtual ExpressionPtr left_hand_side() const = 0;

            virtual ExpressionPtr right_hand_side() const = 0;
    };

    class Call :
        public Expression,
        public PrivateImplementationPattern<Call>
    {
        public:
            Call(const std::string & function, Sequence<ExpressionPtr> parameters);

            virtual ~Call();

            virtual void accept(ExpressionVisitor & visitor);

            virtual ExpressionPtr left_hand_side() const;

            virtual ExpressionPtr right_hand_side() const;

            const std::string & function() const;

            Sequence<ExpressionPtr> parameters() const;
    };

    class Difference :
        public Expression,
        public PrivateImplementationPattern<Difference>
    {
        public:
            Difference(const ExpressionPtr & lhs, const ExpressionPtr & rhs);

            virtual ~Difference();

            virtual void accept(ExpressionVisitor & visitor);

            virtual ExpressionPtr left_hand_side() const;

            virtual ExpressionPtr right_hand_side() const;
    };

    class List :
        public Expression,
        public PrivateImplementationPattern<List>
    {
        public:
            List(Sequence<ExpressionPtr>);

            virtual ~List();

            virtual void accept(ExpressionVisitor & visitor);

            virtual ExpressionPtr left_hand_side() const;

            virtual ExpressionPtr right_hand_side() const;

            Sequence<ExpressionPtr>::Iterator begin() const;

            Sequence<ExpressionPtr>::Iterator end() const;
    };

    class Power :
        public Expression,
        public PrivateImplementationPattern<Power>
    {
        public:
            Power(const ExpressionPtr & lhs, const ExpressionPtr & rhs);

            virtual ~Power();

            virtual void accept(ExpressionVisitor & visitor);

            virtual ExpressionPtr left_hand_side() const;

            virtual ExpressionPtr right_hand_side() const;
    };

    class Product :
        public Expression,
        public PrivateImplementationPattern<Product>
    {
        public:
            Product(const ExpressionPtr & lhs, const ExpressionPtr & rhs);

            virtual ~Product();

            virtual void accept(ExpressionVisitor & visitor);

            virtual ExpressionPtr left_hand_side() const;

            virtual ExpressionPtr right_hand_side() const;
    };

    class Quotient :
        public Expression,
        public PrivateImplementationPattern<Quotient>
    {
        public:
            Quotient(const ExpressionPtr & lhs, const ExpressionPtr & rhs);

            virtual ~Quotient();

            virtual void accept(ExpressionVisitor & visitor);

            virtual ExpressionPtr left_hand_side() const;

            virtual ExpressionPtr right_hand_side() const;
    };

    class Sum :
        public Expression,
        public PrivateImplementationPattern<Sum>
    {
        public:
            Sum(const ExpressionPtr & lhs, const ExpressionPtr & rhs);

            virtual ~Sum();

            virtual void accept(ExpressionVisitor & visitor);

            virtual ExpressionPtr left_hand_side() const;

            virtual ExpressionPtr right_hand_side() const;
    };

    class Value :
        public Expression,
        public PrivateImplementationPattern<Value>
    {
        public:
            Value(const Number & value);

            virtual ~Value();

            virtual void accept(ExpressionVisitor & visitor);

            virtual ExpressionPtr left_hand_side() const;

            virtual ExpressionPtr right_hand_side() const;

            Number value() const;
    };

    class Variable :
        public Expression,
        public PrivateImplementationPattern<Variable>
    {
        public:
            Variable(const std::string & variable);

            virtual ~Variable();

            virtual void accept(ExpressionVisitor & visitor);

            virtual ExpressionPtr left_hand_side() const;

            virtual ExpressionPtr right_hand_side() const;

            std::string variable() const;
    };

    struct ExpressionParser
    {
        static ExpressionPtr parse(const std::string & expression);
    };

    class ExpressionEvaluator :
        public PrivateImplementationPattern<ExpressionEvaluator>
    {
        public:
            typedef std::tr1::function<Number (const std::string &)> LookupFunction;

            ExpressionEvaluator(const LookupFunction & lookup);

            ~ExpressionEvaluator();

            Number evaluate(const ExpressionPtr & expression);
    };

    class ExpressionPrinter :
        public ExpressionVisitor,
        public PrivateImplementationPattern<ExpressionPrinter>
    {
        public:
            ExpressionPrinter();

            ~ExpressionPrinter();

            std::string print(const ExpressionPtr & expression);

            virtual void visit(Call & c);

            virtual void visit(Difference & d);

            virtual void visit(List & l);

            virtual void visit(Power & p);

            virtual void visit(Product & p);

            virtual void visit(Quotient & q);

            virtual void visit(Sum & s);

            virtual void visit(Value & v);

            virtual void visit(Variable & v);
    };
}

#endif
