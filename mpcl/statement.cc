
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
#include <mpcl/statement.hh>
#include <utils/sequence-impl.hh>

#include <sstream>

namespace gpu
{
    Statement::~Statement()
    {
    }

    Assignment::Assignment(const std::string & lhs, const ExpressionPtr & rhs) :
        lhs(lhs),
        rhs(rhs)
    {
    }

    Assignment::~Assignment()
    {
    }

    void
    Assignment::accept(StatementVisitor & v) const
    {
        static_cast<ConstVisits<Assignment> *>(&v)->visit(*this);
    }

    template <>
    ConstVisits<Assignment>::~ConstVisits()
    {
    }

    Declaration::Declaration(const std::string & type, const std::string & name) :
        name(name),
        type(type)
    {
    }

    Declaration::~Declaration()
    {
    }

    void
    Declaration::accept(StatementVisitor & v) const
    {
        static_cast<ConstVisits<Declaration> *>(&v)->visit(*this);
    }

    template <>
    ConstVisits<Declaration>::~ConstVisits()
    {
    }

    Return::Return(const ExpressionPtr & exp) :
        exp(exp)
    {
    }

    Return::~Return()
    {
    }

    void
    Return::accept(StatementVisitor & v) const
    {
        static_cast<ConstVisits<Return> *>(&v)->visit(*this);
    }

    template <>
    ConstVisits<Return>::~ConstVisits()
    {
    }

    template <>
    struct Implementation<StatementPrinter>
    {
        std::stringstream stream;
    };

    StatementPrinter::StatementPrinter() :
        PrivateImplementationPattern<StatementPrinter>(new Implementation<StatementPrinter>)
    {
    }

    StatementPrinter::~StatementPrinter()
    {
    }

    void
    StatementPrinter::visit(const Assignment & a)
    {
        ExpressionPrinter p;

        this->_imp->stream << "Assignment('" << a.lhs << "' = Expression(" << p.print(a.rhs) << "))" << std::endl;
    }

    void
    StatementPrinter::visit(const Declaration & d)
    {
        this->_imp->stream << "Declaration('" << d.name << "' of type '" << d.type << "')" << std::endl;
    }

    void
    StatementPrinter::visit(const Return & r)
    {
        ExpressionPrinter p;

        this->_imp->stream << "Return(Expression(" << p.print(r.exp) << "))" << std::endl;
    }

    std::string
    StatementPrinter::output() const
    {
        return this->_imp->stream.str();
    }

    template class Sequence<StatementPtr>;

    template class WrappedForwardIterator<Sequence<StatementPtr>::IteratorTag, StatementPtr>;
}
