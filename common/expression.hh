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

#ifndef GPU_GUARD_COMMON_EXPRESSION_HH
#define GPU_GUARD_COMMON_EXPRESSION_HH 1

#include <common/expression-fwd.hh>
#include <utils/memory.hh>
#include <utils/private_implementation_pattern.hh>
#include <utils/visitor.hh>

#include <tr1/functional>

namespace gpu
{
    typedef VisitorTag<Difference, Sum, Value, Variable> Expressions;

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
            Value(unsigned value);

            virtual ~Value();

            virtual void accept(ExpressionVisitor & visitor);

            virtual ExpressionPtr left_hand_side() const;

            virtual ExpressionPtr right_hand_side() const;

            unsigned value() const;
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
            typedef std::tr1::function<unsigned (const std::string &)> LookupFunction;

            ExpressionEvaluator(const LookupFunction & lookup);

            ~ExpressionEvaluator();

            unsigned evaluate(const ExpressionPtr & expression);
    };

    class ExpressionPrinter :
        public ExpressionVisitor,
        public PrivateImplementationPattern<ExpressionPrinter>
    {
        public:
            ExpressionPrinter();

            ~ExpressionPrinter();

            std::string print(const ExpressionPtr & expression);

            virtual void visit(Difference & d);

            virtual void visit(Sum & s);

            virtual void visit(Value & v);

            virtual void visit(Variable & v);
    };
}

#endif
