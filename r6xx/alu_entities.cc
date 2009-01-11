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
#include <r6xx/alu_entities.hh>
#include <r6xx/error.hh>
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

    template <>
    ConstVisits<r6xx::alu::Size>::~ConstVisits()
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

                    void visit(const Size & s)
                    {
                        ExpressionPrinter p;

                        output = "Size(\n";
                        output += "\tsymbol=" + s.symbol + ",\n";
                        output += "\texpression=" + p.print(s.expression) + ",\n";
                        output += ")";
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
                 * operand count
                 * instruction slots
                 */
                typedef Tuple<std::string, unsigned, unsigned, unsigned> Form2;

                const static Form2 form2_instructions[] =
                {
                    /* arithmetic */
                    Form2("ashl",               0x72, 2, 1), // Alias for shl
                    Form2("ashr",               0x70, 2, 1),
                    /* float double */
                    Form2("d2f",                0x1c, 1, 2),
                    Form2("dadd",               0x17, 2, 2),
                    Form2("dfract",             0x7b, 1, 2),
                    Form2("dfrexp",             0x07, 1, 4),
                    Form2("dldexp",             0x7a, 2, 2),
                    Form2("dpsete",             0x7d, 2, 2),
                    Form2("dpsetge",            0x7e, 2, 2),
                    Form2("dpsetgt",            0x7c, 2, 2),
                    /* float single */
                    Form2("f2i",                0x6b, 1, 1),
                    Form2("f2d",                0x1d, 1, 2),
                    Form2("fadd",               0x00, 2, 1),
                    Form2("fceil",              0x12, 1, 1),
                    Form2("fcos",               0x6f, 1, 1),
                    Form2("fdot4",              0x50, 2, 4),
                    Form2("fdot4ieee",          0x51, 2, 4),
                    Form2("fexp2",              0x61, 1, 1),
                    Form2("ffloor",             0x14, 1, 1),
                    Form2("ffract",             0x10, 1, 1),
                    Form2("fkille",             0x2c, 2, 1),
                    Form2("fkillge",            0x2e, 2, 1),
                    Form2("fkillgt",            0x2d, 2, 1),
                    Form2("fkillne",            0x2f, 2, 1),
                    Form2("flog",               0x62, 1, 1),
                    Form2("flogieee",           0x63, 1, 1),
                    Form2("fmax",               0x03, 2, 1),
                    Form2("fmin",               0x04, 2, 1),
                    Form2("fmova",              0x15, 1, 1),
                    Form2("fmovafloor",         0x16, 1, 1),
                    Form2("fmul",               0x01, 2, 1),
                    Form2("fmulieee",           0x02, 2, 1),
                    Form2("fppushe",            0x28, 2, 1),
                    Form2("fppushge",           0x2a, 2, 1),
                    Form2("fppushne",           0x2b, 2, 1),
                    Form2("fpsetclr",           0x26, 0, 1),
                    Form2("fpsetinv",           0x24, 1, 1),
                    Form2("fpsetpop",           0x25, 2, 1),
                    Form2("fpsetrest",          0x27, 1, 1),
                    Form2("fpsete",             0x20, 2, 1),
                    Form2("fpsetge",            0x22, 2, 1),
                    Form2("fpsetgt",            0x21, 2, 1),
                    Form2("fpsetne",            0x23, 2, 1),
                    Form2("frecip",             0x64, 1, 1),
                    Form2("frecipff",           0x65, 1, 1),
                    Form2("frecipieee",         0x66, 1, 1),
                    Form2("frecipsqrt",         0x67, 1, 1),
                    Form2("frecipsqrtff",       0x68, 1, 1),
                    Form2("frecipsqrtieee",     0x69, 1, 1),
                    Form2("frndne",             0x13, 1, 1),
                    Form2("fsete",              0x08, 2, 1),
                    Form2("fsetge",             0x0a, 2, 1),
                    Form2("fsetgt",             0x09, 2, 1),
                    Form2("fsetne",             0x0b, 2, 1),
                    Form2("fsin",               0x6e, 1, 1),
                    Form2("fsqrtieee",          0x6a, 1, 1),
                    Form2("ftrunc",             0x11, 1, 1),
                    /* signed int */
                    Form2("i2f",                0x6c, 1, 1),
                    Form2("iadd",               0x34, 2, 1),
                    Form2("imax",               0x36, 2, 1),
                    Form2("imin",               0x37, 2, 1),
                    Form2("imova",              0x18, 1, 1),
                    Form2("imulhi",             0x74, 2, 1),
                    Form2("imullo",             0x73, 2, 1),
                    Form2("ippushe",            0x4a, 2, 1),
                    Form2("ippushge",           0x4c, 2, 1),
                    Form2("ippushgt",           0x4b, 2, 1),
                    Form2("ippushle",           0x4f, 2, 1),
                    Form2("ippushlt",           0x4e, 2, 1),
                    Form2("ippushne",           0x4d, 2, 1),
                    Form2("ipsete",             0x42, 2, 1),
                    Form2("ipsetge",            0x44, 2, 1),
                    Form2("ipsetgt",            0x43, 2, 1),
                    Form2("ipsetle",            0x47, 2, 1),
                    Form2("ipsetlt",            0x46, 2, 1),
                    Form2("ipsetne",            0x45, 2, 1),
                    Form2("isete",              0x3a, 2, 1),
                    Form2("isetge",             0x3c, 2, 1),
                    Form2("isetgt",             0x3b, 2, 1),
                    Form2("isetne",             0x3d, 2, 1),
                    Form2("isub",               0x35, 2, 1),
                    /* unsigned int */
                    Form2("u2f",                0x6d, 1, 1),
                    Form2("uadd",               0x34, 2, 1), // Alias for iadd
                    Form2("umax",               0x38, 2, 1),
                    Form2("umin",               0x39, 2, 1),
                    Form2("umulhi",             0x76, 2, 1),
                    Form2("umullo",             0x75, 2, 1),
                    Form2("usetge",             0x3f, 2, 1),
                    Form2("usetgt",             0x3e, 2, 1),
                    /* logical */
                    Form2("and",                0x30, 2, 1),
                    Form2("not",                0x33, 1, 1),
                    Form2("or",                 0x31, 2, 1),
                    Form2("shl",                0x72, 2, 1),
                    Form2("shr",                0x71, 2, 1),
                    Form2("xor",                0x32, 2, 1),
                    /* general */
                    Form2("mov",                0x19, 1, 1),
                    Form2("nop",                0x1a, 0, 1)
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
                    /* float double */
                    Form3("dmuladd",            0x08, 2),
                    Form3("dmuladd*2",          0x09, 2),
                    Form3("dmuladd*4",          0x0a, 2),
                    Form3("dmuladd/2",          0x0b, 2),
                    /* float single */
                    Form3("fcmove",             0x18, 1),
                    Form3("fcmovge",            0x1a, 1),
                    Form3("fcmovgt",            0x19, 1),
                    Form3("fmuladd",            0x10, 1),
                    Form3("fmuladd*2",          0x11, 1),
                    Form3("fmuladd*4",          0x12, 1),
                    Form3("fmuladd/2",          0x13, 1),
                    Form3("fmuladdieee",        0x14, 1),
                    Form3("fmuladdieee*2",      0x15, 1),
                    Form3("fmuladdieee*4",      0x16, 1),
                    Form3("fmuladdieee/2",      0x17, 1),
                    Form3("fmullit",            0x0c, 1),
                    Form3("fmullit*2",          0x0d, 1),
                    Form3("fmullit*4",          0x0e, 1),
                    Form3("fmullit/2",          0x0f, 1),
                    /* signed int */
                    Form3("icmove",             0x1c, 1),
                    Form3("icmovge",            0x1e, 1),
                    Form3("icmovgt",            0x1d, 1)
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
                                        throw SyntaxError("invalid channel for address register index mode");
                                }
                            }

                            result = EntityPtr(new IndexMode(mode));
                        }
                        else if ("size" == d.name)
                        {
                            Tuple<std::string, ExpressionPtr> parameters(SizeParser::parse(d.params));

                            result = EntityPtr(new Size(parameters.first, parameters.second));
                        }
                        else
                        {
                            throw SyntaxError("invalid directive '." + d.name + "' in ALU section");
                        }
                    }

                    void visit(const Instruction & i)
                    {
                        const Form2 * form2(std::find_if(form2_instructions_begin, form2_instructions_end, Form2Comparator(i.mnemonic)));
                        const Form3 * form3(std::find_if(form3_instructions_begin, form3_instructions_end, Form3Comparator(i.mnemonic)));

                        Sequence<std::string>::Iterator j(i.operands.begin()), j_end(i.operands.end());

                        if (j == j_end)
                            throw SyntaxError("no destination operand");

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
                                throw SyntaxError("expected " + stringify(form2->third) + " source operands, got " + stringify(sources.size()));

                            result = EntityPtr(new Form2Instruction(Enumeration<7>(form2->second), destination, sources, form2->fourth));
                        }
                        else if (form3 != form3_instructions_end)
                        {
                            if (3 != sources.size())
                                throw SyntaxError("expected 3 source operands, got " + stringify(sources.size()));

                            result = EntityPtr(new Form3Instruction(Enumeration<5>(form3->second), destination, sources, form3->third));
                        }
                        else
                        {
                            throw SyntaxError("unknown ALU mnemonic '" + i.mnemonic + "'");
                        }
                    }

                    void visit(const gpu::Label & l)
                    {
                        result = EntityPtr(new r6xx::alu::Label(l.text));
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
