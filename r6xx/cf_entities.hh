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

#ifndef GPU_GUARD_R6XX_CF_ENTITIES_HH
#define GPU_GUARD_R6XX_CF_ENTITIES_HH 1

#include <common/assembly_entities-fwd.hh>
#include <common/expression-fwd.hh>
#include <r6xx/cf_entities-fwd.hh>
#include <utils/enumeration.hh>
#include <utils/sequence.hh>
#include <utils/visitor.hh>

#include <string>

namespace gpu
{
    namespace r6xx
    {
        namespace cf
        {
            typedef ConstVisitorTag<r6xx::cf::ALUClause, r6xx::cf::Label, r6xx::cf::LoopInstruction,
                    r6xx::cf::TextureFetchClause, r6xx::cf::Size> Entities;

            typedef ConstVisitor<Entities> EntityVisitor;

            struct Entity :
                ConstVisitable<Entities>
            {
                virtual ~Entity() = 0;
            };

            struct ALUClause :
                public Entity
            {
                std::string clause;

                Enumeration<4> opcode;

                ALUClause(const Enumeration<4> & opcode, const std::string &);

                ~ALUClause();

                void accept(EntityVisitor &) const;
            };

            struct Label :
                public Entity
            {
                std::string text;

                Label(const std::string &);

                ~Label();

                void accept(EntityVisitor &) const;
            };

            struct LoopInstruction :
                public Entity
            {
                std::string counter;

                std::string target;

                Enumeration<7> opcode;

                LoopInstruction(const Enumeration<7> & opcode, const std::string & target, const std::string & counter);

                ~LoopInstruction();

                void accept(EntityVisitor &) const;
            };

            struct Size :
                public Entity
            {
                std::string symbol;

                ExpressionPtr expression;

                Size(const std::string &, const ExpressionPtr &);

                ~Size();

                void accept(EntityVisitor &) const;
            };

            struct TextureFetchClause :
                public Entity
            {
                std::string clause;

                TextureFetchClause(const std::string &);

                ~TextureFetchClause();

                void accept(EntityVisitor &) const;
            };

            struct EntityConverter
            {
                static Sequence<EntityPtr> convert(const Sequence<AssemblyEntityPtr> &);

                static EntityPtr convert(const AssemblyEntityPtr &);
            };

            struct EntityPrinter
            {
                static std::string print(const Sequence<EntityPtr> &);

                static std::string print(const EntityPtr &);
            };
        }
    }
}

#endif
