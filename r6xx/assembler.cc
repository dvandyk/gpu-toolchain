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

#include <common/assembly_entities.hh>
#include <r6xx/error.hh>
#include <r6xx/assembler.hh>
#include <r6xx/section.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/sequence-impl.hh>
#include <utils/wrapped_forward_iterator-impl.hh>

#include <algorithm>
#include <list>

namespace gpu
{
    template
    struct WrappedForwardIterator<r6xx::Assembler::SectionTag, r6xx::Section>;

    struct SectionFilter :
        public AssemblyEntityVisitor
    {
        std::list<r6xx::Section> & sections;
        std::list<r6xx::Section> stack;

        SectionFilter(std::list<r6xx::Section> & sections) :
            sections(sections)
        {
            stack.push_back(sections.back());
        }

        bool balanced()
        {
            return (stack.size() == 1);
        }

        void visit(const Comment & c)
        {
            stack.back().append(make_shared_ptr(new Comment(c)));
        }

        void visit(const Data & d)
        {
            stack.back().append(make_shared_ptr(new Data(d)));
        }

        void visit(const Instruction & i)
        {
            stack.back().append(make_shared_ptr(new Instruction(i)));
        }

        void visit(const Label & l)
        {
            stack.back().append(make_shared_ptr(new Label(l)));
        }

        r6xx::Section find_or_add(const std::string & name)
        {
            std::list<r6xx::Section>::const_iterator s(std::find_if(sections.begin(), sections.end(), r6xx::SectionNameComparator(name)));
            if (sections.end() != s)
                return *s;

            sections.push_back(r6xx::Section(name));

            return sections.back();
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
        }
    };

    template <>
    struct Implementation<r6xx::Assembler>
    {
        std::list<r6xx::Section> sections;
    };

    namespace r6xx
    {
        Assembler::Assembler(const Sequence<std::tr1::shared_ptr<AssemblyEntity> > & entities) :
            PrivateImplementationPattern<r6xx::Assembler>(new Implementation<r6xx::Assembler>)
        {
            _imp->sections.push_back(Section(".cf"));

            SectionFilter filter(_imp->sections);
            for (Sequence<std::tr1::shared_ptr<AssemblyEntity> >::Iterator e(entities.begin()), e_end(entities.end()) ;
                    e != e_end ; ++e)
            {
                (*e)->accept(filter);
            }

            if (! filter.balanced())
                throw UnbalancedSectionStackError();
        }

        Assembler::~Assembler()
        {
        }

        Assembler::SectionIterator
        Assembler::begin_sections() const
        {
            return SectionIterator(_imp->sections.begin());
        }

        Assembler::SectionIterator
        Assembler::end_sections() const
        {
            return SectionIterator(_imp->sections.end());
        }
    }
}
