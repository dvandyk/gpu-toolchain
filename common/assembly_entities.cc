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

#include <common/assembly_entities.hh>
#include <common/expression.hh>
#include <utils/sequence-impl.hh>

#include <sstream>

namespace gpu
{
    AssemblyEntity::~AssemblyEntity()
    {
    }

    Comment::Comment(const std::string & text) :
        text(text)
    {
    }

    Comment::~Comment()
    {
    }

    void
    Comment::accept(AssemblyEntityVisitor & v) const
    {
        static_cast<ConstVisits<Comment> *>(&v)->visit(*this);
    }

    template <>
    ConstVisits<Comment>::~ConstVisits()
    {
    }

    Directive::Directive(const std::string & name, const std::string & params) :
        name(name),
        params(params)
    {
    }

    Directive::~Directive()
    {
    }

    void
    Directive::accept(AssemblyEntityVisitor & v) const
    {
        static_cast<ConstVisits<Directive> *>(&v)->visit(*this);
    }

    template <>
    ConstVisits<Directive>::~ConstVisits()
    {
    }

    Data::Data(unsigned size, const ExpressionPtr & expression) :
        size(size),
        expression(expression)
    {
    }

    Data::~Data()
    {
    }

    void
    Data::accept(AssemblyEntityVisitor & v) const
    {
        static_cast<ConstVisits<Data> *>(&v)->visit(*this);
    }

    template <>
    ConstVisits<Data>::~ConstVisits()
    {
    }

    Instruction::Instruction(const std::string & mnemonic) :
        mnemonic(mnemonic)
    {
    }

    Instruction::~Instruction()
    {
    }

    void
    Instruction::accept(AssemblyEntityVisitor & v) const
    {
        static_cast<ConstVisits<Instruction> *>(&v)->visit(*this);
    }

    template <>
    ConstVisits<Instruction>::~ConstVisits()
    {
    }

    Label::Label(const std::string & text) :
        text(text)
    {
    }

    Label::~Label()
    {
    }

    void
    Label::accept(AssemblyEntityVisitor & v) const
    {
        static_cast<ConstVisits<Label> *>(&v)->visit(*this);
    }

    template <>
    ConstVisits<Label>::~ConstVisits()
    {
    }

    template <>
    struct Implementation<AssemblyEntityPrinter>
    {
        std::stringstream stream;
    };

    AssemblyEntityPrinter::AssemblyEntityPrinter() :
        PrivateImplementationPattern<AssemblyEntityPrinter>(new Implementation<AssemblyEntityPrinter>)
    {
    }

    AssemblyEntityPrinter::~AssemblyEntityPrinter()
    {
    }

    void
    AssemblyEntityPrinter::visit(const Comment & c)
    {
        this->_imp->stream << "Comment '" << c.text << "'" << std::endl;
    }

    void
    AssemblyEntityPrinter::visit(const Data & d)
    {
        ExpressionPrinter p;

        this->_imp->stream << "Data (" << d.size << ") '" << p.print(d.expression) << "'" << std::endl;
    }

    void
    AssemblyEntityPrinter::visit(const Directive & d)
    {
        this->_imp->stream << "Directive '" << d.name << "' '" << d.params << "'" << std::endl;
    }

    void
    AssemblyEntityPrinter::visit(const Instruction & i)
    {
        this->_imp->stream << "Instruction '" << i.mnemonic << "'" << std::endl;

        for (Sequence<std::string>::Iterator j(i.operands.begin()), j_end(i.operands.end()) ; j != j_end ; ++j)
        {
            this->_imp->stream << "  Operand '" << *j << "'" << std::endl;
        }
    }

    void
    AssemblyEntityPrinter::visit(const Label & l)
    {
        this->_imp->stream << "Label '" << l.text << "'" << std::endl;
    }

    std::string
    AssemblyEntityPrinter::output() const
    {
        return this->_imp->stream.str();
    }
}
