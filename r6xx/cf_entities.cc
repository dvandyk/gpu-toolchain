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

#include <r6xx/cf_entities.hh>
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
    ConstVisits<r6xx::cf::Label>::~ConstVisits()
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
                opcode(opcode),
                clause(clause)
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

                    void visit(const Label & l)
                    {
                        output = "Label(text='" + l.text + "')";
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
                    AClause("alu", 8)
                };
                const AClause * aclause_instructions_begin(aclause_instructions);
                const AClause * aclause_instructions_end(aclause_instructions + sizeof(aclause_instructions) / sizeof(AClause));

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

                struct EntityConverter :
                    public AssemblyEntityVisitor
                {
                    EntityPtr result;

                    void visit(const Comment &) { }
                    void visit(const Data &) { }
                    void visit(const Directive &) { }

                    void visit(const Instruction & i)
                    {
                        const AClause * aclause(std::find_if(aclause_instructions_begin, aclause_instructions_end, AClauseComparator(i.mnemonic)));

                        if (aclause != aclause_instructions_end)
                        {
                            if (1 != i.operands.size())
                                throw SyntaxError(0, "expected 1 source operand, got " + stringify(i.operands.size()));

                            result = EntityPtr(new ALUClause(Enumeration<4>(aclause->second), i.operands.first()));
                        }
                        else
                        {
                            throw SyntaxError(0, "unknown CF mnemonic '" + i.mnemonic + "'");
                        }
                    }

                    void visit(const gpu::Label & l)
                    {
                        result = EntityPtr(new r6xx::cf::Label(l.text));
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
