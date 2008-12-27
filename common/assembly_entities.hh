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

#ifndef GPU_GUARD_COMMON_ASSEMBLY_ENTITIES_HH
#define GPU_GUARD_COMMON_ASSEMBLY_ENTITIES_HH 1

#include <common/assembly_entities-fwd.hh>
#include <common/expression-fwd.hh>
#include <utils/sequence.hh>
#include <utils/visitor.hh>

#include <string>

namespace gpu
{
    typedef ConstVisitorTag<Comment, Data, Directive, Instruction, Label> AssemblyEntities;

    typedef ConstVisitor<AssemblyEntities> AssemblyEntityVisitor;

    struct AssemblyEntity :
        public ConstVisitable<AssemblyEntities>
    {
        virtual ~AssemblyEntity() = 0;
    };

    struct Comment :
        public AssemblyEntity
    {
        Comment(const std::string & text);

        ~Comment();

        void accept(AssemblyEntityVisitor &) const;

        std::string text;
    };

    struct Data :
        public AssemblyEntity
    {
        Data(unsigned size, const ExpressionPtr & expression);

        ~Data();

        void accept(AssemblyEntityVisitor &) const;

        unsigned size;

        ExpressionPtr expression;
    };

    struct Directive :
        public AssemblyEntity
    {
        Directive(const std::string & name, const std::string & params);

        ~Directive();

        void accept(AssemblyEntityVisitor &) const;

        std::string name;

        std::string params;
    };

    struct Instruction :
        public AssemblyEntity
    {
        Instruction(const std::string & mnemonic);

        ~Instruction();

        void accept(AssemblyEntityVisitor &) const;

        std::string mnemonic;

        Sequence<std::string> operands;
    };

    struct Label :
        public AssemblyEntity
    {
        Label(const std::string & text);

        ~Label();

        void accept(AssemblyEntityVisitor &) const;

        std::string text;
    };

    class AssemblyEntityPrinter :
        public AssemblyEntityVisitor,
        public PrivateImplementationPattern<AssemblyEntityPrinter>
    {
        public:
            AssemblyEntityPrinter();

            ~AssemblyEntityPrinter();

            void visit(const Comment &);

            void visit(const Data &);

            void visit(const Directive &);

            void visit(const Instruction &);

            void visit(const Label &);

            std::string output() const;
    };
}

#endif
