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

#ifndef GPU_GUARD_R6XX_ALU_ENTITIES_HH
#define GPU_GUARD_R6XX_ALU_ENTITIES_HH 1

#include <common/assembly_entities-fwd.hh>
#include <common/expression-fwd.hh>
#include <r6xx/alu_entities-fwd.hh>
#include <r6xx/alu_destination_gpr.hh>
#include <r6xx/alu_source_operand.hh>
#include <utils/enumeration.hh>
#include <utils/sequence.hh>
#include <utils/visitor.hh>

#include <string>

namespace gpu
{
    namespace r6xx
    {
        namespace alu
        {
            typedef ConstVisitorTag<r6xx::alu::Form2Instruction, r6xx::alu::Form3Instruction,
                    r6xx::alu::IndexMode, r6xx::alu::GroupEnd, r6xx::alu::Label, r6xx::alu::Size, r6xx::alu::Type> Entities;

            typedef ConstVisitor<Entities> EntityVisitor;

            struct Entity :
                ConstVisitable<Entities>
            {
                virtual ~Entity() = 0;
            };

            struct Form2Instruction :
                public Entity
            {
                /*
                 * R6xx ISA v0.35, p.8-19:
                 * Opcode is enum(10), but first 3 bits must be 0.
                 */
                Enumeration<7> opcode;

                DestinationGPR destination;

                unsigned slots;

                Sequence<SourceOperandPtr> sources;

                Form2Instruction(const Enumeration<7> &, const DestinationGPR &, const Sequence<SourceOperandPtr> &, unsigned);

                ~Form2Instruction();

                void accept(EntityVisitor &) const;
            };

            struct Form3Instruction :
                public Entity
            {
                Enumeration<5> opcode;

                DestinationGPR destination;

                unsigned slots;

                Sequence<SourceOperandPtr> sources;

                Form3Instruction(const Enumeration<5> &, const DestinationGPR &, const Sequence<SourceOperandPtr> &, unsigned);

                ~Form3Instruction();

                void accept(EntityVisitor &) const;
            };

            struct GroupEnd :
                public Entity
            {
                GroupEnd();

                ~GroupEnd();

                void accept(EntityVisitor &) const;
            };

            struct IndexMode :
                public Entity
            {
                Enumeration<3> mode;

                IndexMode(unsigned mode);

                ~IndexMode();

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

            struct Size :
                public Entity
            {
                std::string symbol;

                ExpressionPtr expression;

                Size(const std::string &, const ExpressionPtr &);

                ~Size();

                void accept(EntityVisitor &) const;
            };

            struct Type :
                public Entity
            {
                std::string symbol;

                unsigned type;

                Type(const std::string &, unsigned);

                ~Type();

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
