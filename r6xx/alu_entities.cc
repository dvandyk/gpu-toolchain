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

#include <r6xx/alu_entities.hh>
#include <utils/sequence-impl.hh>
#include <utils/stringify.hh>
#include <utils/tuple.hh>
#include <utils/visitor-impl.hh>

#include <algorithm>

namespace gpu
{
    template
    class Sequence<r6xx::alu::EntityPtr>;

    template
    class WrappedForwardIterator<Sequence<r6xx::alu::EntityPtr>::IteratorTag, r6xx::alu::EntityPtr>;

    template <>
    ConstVisits<r6xx::alu::Form2Instruction>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::alu::Form3Instruction>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::alu::GroupEnd>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::alu::IndexMode>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::alu::Label>::~ConstVisits()
    {
    }

    namespace r6xx
    {
        namespace alu
        {
            Entity::~Entity()
            {
            }

            Form2Instruction::Form2Instruction(const Enumeration<7> & o, const DestinationGPR & d, const Sequence<SourceOperandPtr> & so, unsigned sl) :
                opcode(o),
                destination(d),
                slots(sl),
                sources(so)
            {
            }

            Form2Instruction::~Form2Instruction()
            {
            }

            void
            Form2Instruction::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<Form2Instruction> *>(&v)->visit(*this);
            }


            Form3Instruction::Form3Instruction(const Enumeration<5> & o, const DestinationGPR & d, const Sequence<SourceOperandPtr> & so, unsigned sl) :
                opcode(o),
                destination(d),
                slots(sl),
                sources(so)
            {
            }

            Form3Instruction::~Form3Instruction()
            {
            }

            void
            Form3Instruction::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<Form3Instruction> *>(&v)->visit(*this);
            }

            GroupEnd::GroupEnd()
            {
            }

            GroupEnd::~GroupEnd()
            {
            }

            void
            GroupEnd::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<GroupEnd> *>(&v)->visit(*this);
            }

            IndexMode::IndexMode(unsigned mode) :
                mode(mode)
            {
            }

            IndexMode::~IndexMode()
            {
            }

            void
            IndexMode::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<IndexMode> *>(&v)->visit(*this);
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

                    void visit(const Form2Instruction & i)
                    {
                        SourceOperandPrinter p;

                        output = std::string("Form2Instruction(\n")
                                + "\topcode=" + stringify(i.opcode) + ",\n"
                                + "\tdestination=" + DestinationGPRPrinter::print(i.destination) + ",\n"
                                + "\tslots=" + stringify(i.slots) + ",\n";

                        unsigned index(0);
                        for (Sequence<SourceOperandPtr>::Iterator j(i.sources.begin()), j_end(i.sources.end()) ;
                                j != j_end ; ++j, ++index)
                        {
                            output += "\tsource[" + stringify(index) + "]=" + p.print(*j) + ",\n";
                        }

                        output += ")";
                    }

                    void visit(const Form3Instruction & i)
                    {
                        SourceOperandPrinter p;

                        output = std::string("Form3Instruction(\n")
                                + "\topcode=" + stringify(i.opcode) + ",\n"
                                + "\tdestination=" + DestinationGPRPrinter::print(i.destination) + ",\n"
                                + "\tslots=" + stringify(i.slots) + ",\n";

                        unsigned index(0);
                        for (Sequence<SourceOperandPtr>::Iterator j(i.sources.begin()), j_end(i.sources.end()) ;
                                j != j_end ; ++j, ++index)
                        {
                            output += "\tsource[" + stringify(index) + "]=" + p.print(*j) + ",\n";
                        }

                        output += ")";
                    }

                    void visit(const GroupEnd & e)
                    {
                        output += "GroupEnd()";
                    }

                    void visit(const IndexMode & m)
                    {
                        output += "IndexMode(mode=" + stringify(m.mode) + ")";
                    }

                    void visit(const Label & l)
                    {
                        output = "Label(text='" + l.text + "')";
                    }
                };
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
                 * operand count
                 * instruction slots
                 */
                typedef Tuple<std::string, unsigned, unsigned, unsigned> Form2;

                const static Form2 form2_instructions[] =
                {
                    Form2("fadd", 0, 2, 1),
                    Form2("fmul", 1, 2, 1),
                    Form2("fmulieee", 2, 2, 1),
                    Form2("fmax", 3, 1, 1),
                    Form2("fmin", 4, 1, 1),
                    Form2("fmaxdx10", 5, 1, 1),
                    Form2("fmindx10", 6, 1, 1)
                };
                const Form2 * form2_instructions_begin(form2_instructions);
                const Form2 * form2_instructions_end(form2_instructions + sizeof(form2_instructions) / sizeof(Form2));

                struct Form2Comparator
                {
                    std::string mnemonic;

                    Form2Comparator(const std::string mnemonic) :
                        mnemonic(mnemonic)
                    {
                    }

                    bool operator() (const Form2 & f)
                    {
                        return mnemonic == f.first;
                    }
                };

                /*
                 * mnemonic
                 * opcode
                 * slots
                 */
                typedef Tuple<std::string, unsigned, unsigned> Form3;
                const static Form3 form3_instructions[] =
                {
                    Form3("fmuladd64", 8, 2),
                    Form3("fmullit", 12, 4),
                    Form3("fmuladd", 16, 1),
                    Form3("fmuladdieee", 20, 1),
                    Form3("fcnde", 24, 1),
                    Form3("fcndgt", 25, 1),
                    Form3("fcndge", 26, 1),
                    Form3("icnde", 28, 1),
                    Form3("icndgt", 29, 1),
                    Form3("icndge", 30, 1)
                };
                const Form3 * form3_instructions_begin(form3_instructions);
                const Form3 * form3_instructions_end(form3_instructions + sizeof(form3_instructions) / sizeof(Form3));

                struct Form3Comparator
                {
                    std::string mnemonic;

                    Form3Comparator(const std::string mnemonic) :
                        mnemonic(mnemonic)
                    {
                    }

                    bool operator() (const Form3 & f)
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
                        if ("groupend" == d.name)
                        {
                            result = EntityPtr(new GroupEnd);
                        }
                        else if ("indexmode" == d.name)
                        {
                            unsigned mode(0xFF);
                            if ("loop" == d.params)
                            {
                                mode = 4;
                            }
                            else if ("ar." == d.params.substr(0, 3))
                            {
                                char channel(d.params[3]);
                                switch (channel)
                                {
                                    case 'w':
                                        mode = 3;
                                        break;

                                    case 'x':
                                    case 'y':
                                    case 'z':
                                        break;

                                    default:
                                        throw SyntaxError(0, "invalid channel for address register index mode");
                                }
                            }

                            result = EntityPtr(new IndexMode(mode));
                        }
                        else
                        {
                            throw SyntaxError(0, "invalid directive '." + d.name + "' in ALU section");
                        }
                    }

                    void visit(const Instruction & i)
                    {
                        const Form2 * form2(std::find_if(form2_instructions_begin, form2_instructions_end, Form2Comparator(i.mnemonic)));
                        const Form3 * form3(std::find_if(form3_instructions_begin, form3_instructions_end, Form3Comparator(i.mnemonic)));

                        Sequence<std::string>::Iterator j(i.operands.begin()), j_end(i.operands.end());

                        if (j == j_end)
                            throw SyntaxError(0, "no destination operand");

                        DestinationGPR destination(DestinationGPRParser::parse(*j));

                        ++j;

                        Sequence<SourceOperandPtr> sources;
                        for ( ; j != j_end ; ++j)
                        {
                            sources.append(SourceOperandParser::parse(*j));
                        }

                        if (form2 != form2_instructions_end)
                        {
                            if (sources.size() != form2->third)
                                throw SyntaxError(0, "expected " + stringify(form2->third) + " source operands, got " + stringify(sources.size()));

                            result = EntityPtr(new Form2Instruction(Enumeration<7>(form2->second), destination, sources, form2->fourth));
                        }
                        else if (form3 != form3_instructions_end)
                        {
                            if (3 != sources.size())
                                throw SyntaxError(0, "expected 3 source operands, got " + stringify(sources.size()));

                            result = EntityPtr(new Form3Instruction(Enumeration<5>(form3->second), destination, sources, form3->third));
                        }
                        else
                        {
                            throw SyntaxError(0, "unknown ALU mnemonic '" + i.mnemonic + "'");
                        }
                    }

                    void visit(const gpu::Label & l)
                    {
                        result = EntityPtr(new r6xx::alu::Label(l.text));
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
