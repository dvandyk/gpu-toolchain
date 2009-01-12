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

#include <common/assembly_entities.hh>
#include <common/directives.hh>
#include <common/expression.hh>
#include <r6xx/cf_entities.hh>
#include <r6xx/error.hh>
#include <utils/destringify.hh>
#include <utils/sequence-impl.hh>
#include <utils/stringify.hh>
#include <utils/tuple.hh>
#include <utils/visitor-impl.hh>

#include <algorithm>

namespace gpu
{
    template
    class Sequence<r6xx::cf::EntityPtr>;

    template
    class WrappedForwardIterator<Sequence<r6xx::cf::EntityPtr>::IteratorTag, r6xx::cf::EntityPtr>;

    template <>
    ConstVisits<r6xx::cf::ALUClause>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::cf::BranchInstruction>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::cf::Label>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::cf::LoopInstruction>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::cf::NopInstruction>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::cf::ProgramEnd>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::cf::Size>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::cf::Type>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::cf::TextureFetchClause>::~ConstVisits()
    {
    }

    namespace r6xx
    {
        namespace cf
        {
            Entity::~Entity()
            {
            }

            ALUClause::ALUClause(const Enumeration<4> & opcode, const std::string & clause) :
                clause(clause),
                opcode(opcode)
            {
            }

            ALUClause::~ALUClause()
            {
            }

            void
            ALUClause::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<ALUClause> *>(&v)->visit(*this);
            }

            BranchInstruction::BranchInstruction(const Enumeration<7> & opcode, const std::string & target, unsigned count) :
                count(count),
                opcode(opcode),
                target(target)
            {
            }

            BranchInstruction::~BranchInstruction()
            {
            }

            void
            BranchInstruction::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<BranchInstruction> *>(&v)->visit(*this);
            }

            Label::Label(const std::string & t) :
                text(t)
            {
            }

            Label::~Label()
            {
            }

            void
            Label::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<Label> *>(&v)->visit(*this);
            }

            LoopInstruction::LoopInstruction(const Enumeration<7> & opcode, const std::string & target, const std::string & counter) :
                counter(counter),
                opcode(opcode),
                target(target)
            {
            }

            LoopInstruction::~LoopInstruction()
            {
            }

            void
            LoopInstruction::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<LoopInstruction> *>(&v)->visit(*this);
            }

            NopInstruction::NopInstruction()
            {
            }

            NopInstruction::~NopInstruction()
            {
            }

            void
            NopInstruction::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<NopInstruction> *>(&v)->visit(*this);
            }

            ProgramEnd::ProgramEnd()
            {
            }

            ProgramEnd::~ProgramEnd()
            {
            }

            void
            ProgramEnd::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<ProgramEnd> *>(&v)->visit(*this);
            }

            Size::Size(const std::string & s, const ExpressionPtr & e) :
                symbol(s),
                expression(e)
            {
            }

            Size::~Size()
            {
            }

            void
            Size::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<Size> *>(&v)->visit(*this);
            }

            Type::Type(const std::string & s, unsigned t) :
                symbol(s),
                type(t)
            {
            }

            Type::~Type()
            {
            }

            void
            Type::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<Type> *>(&v)->visit(*this);
            }

            TextureFetchClause::TextureFetchClause(const std::string & clause) :
                clause(clause)
            {
            }

            TextureFetchClause::~TextureFetchClause()
            {
            }

            void
            TextureFetchClause::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<TextureFetchClause> *>(&v)->visit(*this);
            }

            namespace internal
            {
                struct EntityPrinter :
                    public EntityVisitor
                {
                    std::string output;

                    void visit(const ALUClause & a)
                    {
                        output += "ALUClause(clause='" + a.clause + "')";
                    }

                    void visit(const BranchInstruction & b)
                    {
                        output += "BranchInstruction(opcode=" + stringify(b.opcode) + ", target='" + b.target + "', count=" + stringify(b.count) + ")";
                    }

                    void visit(const Label & l)
                    {
                        output = "Label(text='" + l.text + "')";
                    }

                    void visit(const LoopInstruction & l)
                    {
                        output = "LoopInstruction(opcode=" + stringify(l.opcode) + ", target='" + l.target + "'";

                        if (! l.counter.empty())
                            output += ", counter='" + l.counter + "'";

                        output += ")";
                    }

                    void visit(const NopInstruction &)
                    {
                        output = "NopInstruction()";
                    }

                    void visit(const ProgramEnd &)
                    {
                        output = "ProgramEnd()";
                    }

                    void visit(const Size & s)
                    {
                        ExpressionPrinter p;

                        output = "Size(\n";
                        output += "\tsymbol=" + s.symbol + ",\n";
                        output += "\texpression=" + p.print(s.expression) + ",\n";
                        output += ")";
                    }

                    void visit(const Type & t)
                    {
                        output = "Type(\n";
                        output += "\tsymbol=" + t.symbol + ",\n";
                        output += "\ttype=" + stringify(t.type) + "\n";
                        output += ")";
                    }

                    void visit(const TextureFetchClause & t)
                    {
                        output = "TextureFetchClause(clause='" + t.clause + "')";
                    }
                };
            }

            std::string
            EntityPrinter::print(const Sequence<EntityPtr> & entities)
            {
                std::string result;

                for (Sequence<EntityPtr>::Iterator i(entities.begin()), i_end(entities.end()) ;
                        i != i_end ; ++i)
                {
                    if (! result.empty())
                        result += "\n";

                    internal::EntityPrinter p;

                    (*i)->accept(p);

                    result += p.output;

                }

                return result;
            }

            std::string
            EntityPrinter::print(const EntityPtr & ptr)
            {
                internal::EntityPrinter p;

                ptr->accept(p);

                return p.output;
            }

            namespace internal
            {
                /*
                 * mnemonic
                 * opcode
                 */
                typedef Tuple<std::string, unsigned> AClause;

                const static AClause aclause_instructions[] =
                {
                    AClause("alu", 8),
                    AClause("alu_push_before", 9),
                    AClause("alu_pop_after", 10),
                    AClause("alu_pop_twice_after", 11),
                    AClause("alu_continue", 13),
                    AClause("alu_break", 14),
                    AClause("alu_else_after", 15)
                };
                const static AClause * aclause_instructions_begin(aclause_instructions);
                const static AClause * aclause_instructions_end(aclause_instructions + sizeof(aclause_instructions) / sizeof(AClause));

                struct AClauseComparator
                {
                    std::string mnemonic;

                    AClauseComparator(const std::string mnemonic) :
                        mnemonic(mnemonic)
                    {
                    }

                    bool operator() (const AClause & f)
                    {
                        return mnemonic == f.first;
                    }
                };

                /*
                 * mnemonic
                 * opcode
                 */
                typedef Tuple<std::string, unsigned> Branch;

                const static Branch branch_instructions[] =
                {
                    Branch("call", 0x0d)
                };
                const static Branch * branch_instructions_begin(branch_instructions);
                const static Branch * branch_instructions_end(branch_instructions + sizeof(branch_instructions) / sizeof(Branch));

                struct BranchComparator
                {
                    std::string mnemonic;

                    BranchComparator(const std::string mnemonic) :
                        mnemonic(mnemonic)
                    {
                    }

                    bool operator() (const Branch & b)
                    {
                        return mnemonic == b.first;
                    }
                };

                /*
                 * mnemonic
                 * opcode
                 * needs counter?
                 */
                typedef Tuple<std::string, unsigned, bool> Loop;
                const static Loop loop_instructions[] =
                {
                    Loop("loop_start", 4, true),
                    Loop("loop_end", 5, false)
                };
                const static Loop * loop_instructions_begin(loop_instructions);
                const static Loop * loop_instructions_end(loop_instructions + sizeof(loop_instructions) / sizeof(Loop));

                struct LoopComparator
                {
                    std::string mnemonic;

                    LoopComparator(const std::string mnemonic) :
                        mnemonic(mnemonic)
                    {
                    }

                    bool operator() (const Loop & f)
                    {
                        return mnemonic == f.first;
                    }
                };

                struct EntityConverter :
                    public AssemblyEntityVisitor
                {
                    EntityPtr result;

                    void visit(const Comment &) { }
                    void visit(const Data &) { }

                    void visit(const Directive & d)
                    {
                        if ("programend" == d.name)
                        {
                            if (! d.params.empty())
                                throw SyntaxError("the '.programend' directive does not take parameters");

                            result = EntityPtr(new ProgramEnd);
                        }
                        else if ("size" == d.name)
                        {
                            Tuple<std::string, ExpressionPtr> parameters(SizeParser::parse(d.params));

                            result = EntityPtr(new Size(parameters.first, parameters.second));
                        }
                        else if ("type" == d.name)
                        {
                            Tuple<std::string, unsigned> parameters(TypeParser::parse(d.params));

                            result = EntityPtr(new Type(parameters.first, parameters.second));
                        }
                        else
                        {
                            throw SyntaxError("unknown directive '." + d.name + "' in control flow section");
                        }
                    }

                    void visit(const Instruction & i)
                    {
                        const AClause * aclause(std::find_if(aclause_instructions_begin, aclause_instructions_end, AClauseComparator(i.mnemonic)));
                        const Branch * branch(std::find_if(branch_instructions_begin, branch_instructions_end, BranchComparator(i.mnemonic)));
                        const Loop * loop(std::find_if(loop_instructions_begin, loop_instructions_end, LoopComparator(i.mnemonic)));

                        if (aclause != aclause_instructions_end)
                        {
                            if (1 != i.operands.size())
                                throw SyntaxError("expected 1 source operand, got " + stringify(i.operands.size()));

                            result = EntityPtr(new ALUClause(Enumeration<4>(aclause->second), i.operands.first()));
                        }
                        else if (branch != branch_instructions_end)
                        {
                            if (2 != i.operands.size())
                                throw SyntaxError("expected 2 source operands, got " + stringify(i.operands.size()));

                            std::string target(i.operands.first());
                            unsigned count(destringify<unsigned>(i.operands.last()));

                            result = EntityPtr(new BranchInstruction(Enumeration<7>(branch->second), target, count));
                        }
                        else if (loop != loop_instructions_end)
                        {
                            unsigned operand_count(loop->third ? 2 : 1);
                            if (i.operands.size() != operand_count)
                                throw SyntaxError("expected " + stringify(operand_count) + " operands, got " + stringify(i.operands.size()));

                            std::string counter("");
                            if (loop->third)
                                counter = i.operands.last();

                            result = EntityPtr(new LoopInstruction(Enumeration<7>(loop->second), i.operands.first(), counter));
                        }
                        else if ("nop" == i.mnemonic)
                        {
                            if (0 != i.operands.size())
                                throw SyntaxError("'nop' takes no operands");

                            result = EntityPtr(new NopInstruction);
                        }
                        else if ("tex" == i.mnemonic)
                        {
                            if (1 != i.operands.size())
                                throw SyntaxError("expected 1 source operand, got " + stringify(i.operands.size()));

                            result = EntityPtr(new TextureFetchClause(i.operands.first()));
                        }
                        else
                        {
                            throw SyntaxError("unknown CF mnemonic '" + i.mnemonic + "'");
                        }
                    }

                    void visit(const gpu::Label & l)
                    {
                        result = EntityPtr(new r6xx::cf::Label(l.text));
                    }

                    void visit(const gpu::Line & l)
                    {
                        SyntaxContext::Line(l.number);
                    }
                };
            }

            Sequence<EntityPtr>
            EntityConverter::convert(const Sequence<AssemblyEntityPtr> & input)
            {
                Sequence<EntityPtr> result;
                internal::EntityConverter converter;

                for (Sequence<AssemblyEntityPtr>::Iterator i(input.begin()), i_end(input.end()) ;
                        i != i_end ; ++i)
                {
                    converter.result = EntityPtr();
                    (*i)->accept(converter);
                    if (EntityPtr() != converter.result)
                        result.append(converter.result);
                }

                return result;
            }

            EntityPtr
            EntityConverter::convert(const AssemblyEntityPtr & input)
            {
                internal::EntityConverter converter;

                converter.result = EntityPtr();
                input->accept(converter);

                return converter.result;
            }
        }
    }
}
