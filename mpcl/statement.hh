/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Danny van Dyk <danny.dyk@tu-dortmund.de>
 * Copyright (c) 2009 Dirk Fischer <dirk.fischer@gmail.com>
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

#ifndef GPU_GUARD_MPCL_STATEMENT_HH
#define GPU_GUARD_MPCL_STATEMENT_HH 1

#include <common/expression-fwd.hh>
#include <mpcl/statement-fwd.hh>
#include <utils/sequence.hh>
#include <utils/visitor.hh>

#include <string>

namespace gpu
{
    typedef ConstVisitorTag<Assignment, Declaration, Return> Statements;

    typedef ConstVisitor<Statements> StatementVisitor;

    struct Statement :
        public ConstVisitable<Statements>
    {
        virtual ~Statement() = 0;
    };

    struct Assignment :
        public Statement
    {
        std::string lhs;

        ExpressionPtr rhs;

        Assignment(const std::string & lhs, const ExpressionPtr & rhs);

        ~Assignment();

        void accept(StatementVisitor &) const;
    };

    struct Declaration :
        public Statement
    {
        std::string type;

        std::string name;

        ExpressionPtr exp;

        Declaration(const std::string & type, const std::string & name);

        ~Declaration();

        void accept(StatementVisitor &) const;
    };

    struct Return :
        public Statement
    {
        ExpressionPtr exp;

        Return(const ExpressionPtr & exp);

        ~Return();

        void accept(StatementVisitor &) const;
    };

    class StatementPrinter :
        public StatementVisitor,
        public PrivateImplementationPattern<StatementPrinter>
    {
        public:
            StatementPrinter();

            ~StatementPrinter();

            void visit(const Assignment &);

            void visit(const Declaration &);

            void visit(const Return &);

            std::string output() const;
    };

    extern template class Sequence<StatementPtr>;
}

#endif
