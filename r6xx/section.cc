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
#include <r6xx/alu_section.hh>
#include <r6xx/cf_section.hh>
#include <r6xx/error.hh>
#include <r6xx/section.hh>
#include <r6xx/tex_section.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/sequence-impl.hh>
#include <utils/wrapped_forward_iterator-impl.hh>

#include <algorithm>
#include <list>
#include <string>

namespace gpu
{
    namespace r6xx
    {
        Section::~Section()
        {
        }

        SectionPtr
        Section::make(const std::string & name)
        {
            SectionPtr result;

            if (".alu" == name)
            {
                result = SectionPtr(new alu::Section);
            }
            else if (".cf" == name)
            {
                result = SectionPtr(new cf::Section);
            }
            else if (".tex" == name)
            {
                result = SectionPtr(new tex::Section);
            }
            else
            {
                throw InvalidSectionNameError(name);
            }

            return result;
        }

        bool
        Section::valid(const std::string & name)
        {
            const static std::string section_names[] = 
            {
                ".alu",
                ".cf",
                ".tex"
            };
            const static std::string * const section_names_begin(&section_names[0]);
            const static std::string * const section_names_end(section_names_begin + sizeof(section_names) / sizeof(section_names[0]));

            return section_names_end != std::find(section_names_begin, section_names_end, name);
        }

        namespace internal
        {
            struct SectionPrinter :
                public SectionVisitor
            {
                std::string result;

                void visit(const alu::Section & a)
                {
                    result = "Section(name='.alu')";

                    if (! a.entities.empty())
                        result += "\n";

                    result += alu::EntityPrinter::print(a.entities);
                }

                void visit(const cf::Section & c)
                {
                    result = "Section(name='.cf')";
                    if (! c.entities.empty())
                        result += "\n";

                    result += cf::EntityPrinter::print(c.entities);
                }

                void visit(const tex::Section & t)
                {
                    result = "Section(name='.tex')";
                    if (! t.entities.empty())
                        result += "\n";

                    result += tex::EntityPrinter::print(t.entities);
                }
            };
        }

        std::string
        SectionPrinter::print(const SectionPtr & input)
        {
            internal::SectionPrinter p;

            input->accept(p);

            return p.result;
        }

        namespace internal
        {
            struct ConversionStage :
                public AssemblyEntityVisitor
            {
                Sequence<r6xx::SectionPtr> sections;
                std::list<SectionPtr> stack;

                ConversionStage()
                {
                    sections.append(Section::make(".cf"));
                    stack.push_back(sections.last());
                }

                bool balanced()
                {
                    return (stack.size() == 1);
                }

                void visit(const Comment & c)
                {
                    stack.back()->append(make_shared_ptr(new Comment(c)));
                }

                void visit(const Data & d)
                {
                    stack.back()->append(make_shared_ptr(new Data(d)));
                }

                void visit(const Instruction & i)
                {
                    stack.back()->append(make_shared_ptr(new Instruction(i)));
                }

                void visit(const Label & l)
                {
                    stack.back()->append(make_shared_ptr(new Label(l)));
                }

                void visit(const Line & l)
                {
                    SyntaxContext::Line(l.number);
                }

                r6xx::SectionPtr find_or_add(const std::string & name)
                {
                    Sequence<SectionPtr>::Iterator s(std::find_if(sections.begin(), sections.end(), r6xx::SectionNameComparator(name)));
                    if (sections.end() != s)
                        return *s;

                    sections.append(Section::make(name));

                    return sections.last();
                }

                void visit(const Directive & d)
                {
                    if ("section" == d.name)
                    {
                        stack.back() = find_or_add(d.params);
                    }
                    else if ("pushsection" == d.name)
                    {
                        stack.push_back(find_or_add(d.params));
                    }
                    else if ("popsection" == d.name)
                    {
                        if (stack.size() == 1)
                            throw r6xx::UnbalancedSectionStackError();

                        stack.pop_back();
                    }
                    else if (r6xx::Section::valid("." + d.name))
                    {
                        stack.back() = find_or_add("." + d.name);
                    }
                    else
                    {
                        stack.back()->append(make_shared_ptr(new Directive(d)));
                    }
                }
            };
        }

        Sequence<SectionPtr>
        SectionConverter::convert(const Sequence<AssemblyEntityPtr> & input)
        {
            internal::ConversionStage cs;

            for (Sequence<std::tr1::shared_ptr<AssemblyEntity> >::Iterator e(input.begin()), e_end(input.end()) ;
                    e != e_end ; ++e)
            {
                (*e)->accept(cs);
            }

            if (! cs.balanced())
                throw UnbalancedSectionStackError();

            return cs.sections;
        }
    }
}
