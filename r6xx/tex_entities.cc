/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Danny van Dyk <danny.dyk@tu-dortmund.de>
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
#include <r6xx/tex_entities.hh>
#include <r6xx/error.hh>
#include <utils/sequence-impl.hh>
#include <utils/stringify.hh>
#include <utils/tuple.hh>
#include <utils/visitor-impl.hh>

#include <algorithm>

namespace gpu
{
    template
    class Sequence<r6xx::tex::EntityPtr>;

    template
    class WrappedForwardIterator<Sequence<r6xx::tex::EntityPtr>::IteratorTag, r6xx::tex::EntityPtr>;

    template <>
    ConstVisits<r6xx::tex::Label>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::tex::LoadInstruction>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::tex::Size>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::tex::Type>::~ConstVisits()
    {
    }

    namespace r6xx
    {
        namespace tex
        {
            Entity::~Entity()
            {
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

            LoadInstruction::LoadInstruction(const Enumeration<5> & o, const DestinationGPR & d, const SourceGPR & s) :
                opcode(o),
                destination(d),
                source(s)
            {
            }

            LoadInstruction::~LoadInstruction()
            {
            }

            void
            LoadInstruction::accept(EntityVisitor & v) const
            {
                static_cast<ConstVisits<LoadInstruction> *>(&v)->visit(*this);
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

            namespace internal
            {
                struct EntityPrinter :
                    public EntityVisitor
                {
                    std::string output;

                    void visit(const Label & l)
                    {
                        output = "Label(text='" + l.text + "')";
                    }

                    void visit(const LoadInstruction & l)
                    {
                        output = "LoadInstruction(\n";
                        output += "\topcode=" + stringify(l.opcode) + ",\n";
                        output += "\tdestination=" + DestinationGPRPrinter::print(l.destination) + ",\n";
                        output += ")";
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
                typedef Tuple<std::string, unsigned> Load;
                typedef TupleComparator<Load, std::string, 1> LoadComparator;
                const static Load load_instructions[] =
                {
                    Load("ld", 3)
                };
                const static Load * load_instructions_begin(load_instructions);
                const static Load * load_instructions_end(load_instructions + sizeof(load_instructions) / sizeof(Load));

                struct EntityConverter :
                    public AssemblyEntityVisitor
                {
                    EntityPtr result;

                    void visit(const Comment &) { }
                    void visit(const Data &) { }

                    void visit(const Directive & d)
                    {
                        if ("size" == d.name)
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
                            throw SyntaxError("unknown directive '." + d.name + "' in texture fetch section");
                        }
                    }

                    void visit(const Instruction & i)
                    {
                        const Load * load(std::find_if(load_instructions_begin, load_instructions_end, LoadComparator(i.mnemonic)));

                        if (load != load_instructions_end)
                        {
                            if (2 != i.operands.size())
                                throw SyntaxError("expected 2 operands, got " + stringify(i.operands.size()));

                            DestinationGPR destination(DestinationGPRParser::parse(i.operands.first()));
                            SourceGPR source(SourceGPRParser::parse(i.operands.last()));
                            // TODO
                            // - resource id

                            result = EntityPtr(new LoadInstruction(Enumeration<5>(load->second), destination, source));
                        }
                        else
                        {
                            throw SyntaxError("unknown Texture Fetch mnemonic '" + i.mnemonic + "'");
                        }
                    }

                    void visit(const gpu::Label & l)
                    {
                        result = EntityPtr(new r6xx::tex::Label(l.text));
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
