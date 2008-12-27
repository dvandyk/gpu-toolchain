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
#include <utils/sequence.hh>
#include <utils/visitor.hh>

#include <string>

namespace gpu
{
    typedef ConstVisitorTag<Comment, Directive, Instruction> AssemblyEntities;

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
}

#endif
