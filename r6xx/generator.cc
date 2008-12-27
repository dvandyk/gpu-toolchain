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
#include <r6xx/generator.hh>
#include <r6xx/section.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/sequence-impl.hh>
#include <utils/wrapped_forward_iterator-impl.hh>

#include <algorithm>
#include <list>

namespace gpu
{
    template
    struct WrappedForwardIterator<r6xx::Generator::SectionTag, r6xx::Section>;

    struct SectionFilter :
        public AssemblyEntityVisitor
    {
        std::list<r6xx::Section> & sections;
        std::list<r6xx::Section> stack;

        SectionFilter(std::list<r6xx::Section> & sections) :
            sections(sections)
        {
        }

        bool balanced()
        {
            return stack.empty();
        }

        void visit(const Comment &) { }
        void visit(const Data &) { }
        void visit(const Instruction &) { }
        void visit(const Label &) { }

        void visit(const Directive & d)
        {
            if ("section" == d.name)
            {
                std::list<r6xx::Section>::const_iterator s(std::find_if(sections.begin(), sections.end(), r6xx::SectionNameComparator(d.params)));
                if (sections.end() == s)
                    sections.push_back(r6xx::Section(d.params));
            }
            else if ("pushsection" == d.name)
            {
                std::list<r6xx::Section>::const_iterator s(std::find_if(sections.begin(), sections.end(), r6xx::SectionNameComparator(d.params)));
                if (sections.end() == s)
                    s = sections.insert(sections.end(), r6xx::Section(d.params));

                stack.push_back(*s);
            }
            else if ("popsection" == d.name)
            {
                if (stack.empty())
                    throw r6xx::UnbalancedSectionStackError();

                stack.pop_back();
            }
            else if (r6xx::Section::valid("." + d.name))
            {
                std::list<r6xx::Section>::const_iterator s(std::find_if(sections.begin(), sections.end(), r6xx::SectionNameComparator(d.name)));
                if (sections.end() == s)
                    sections.push_back(r6xx::Section("." + d.name));
            }
        }
    };

    template <>
    struct Implementation<r6xx::Generator>
    {
        std::list<r6xx::Section> sections;
    };

    namespace r6xx
    {
        Generator::Generator(const Sequence<std::tr1::shared_ptr<AssemblyEntity> > & entities) :
            PrivateImplementationPattern<r6xx::Generator>(new Implementation<r6xx::Generator>)
        {
            SectionFilter filter(_imp->sections);
            for (Sequence<std::tr1::shared_ptr<AssemblyEntity> >::Iterator e(entities.begin()), e_end(entities.end()) ;
                    e != e_end ; ++e)
            {
                (*e)->accept(filter);
            }

            if (! filter.balanced())
                throw UnbalancedSectionStackError();
        }

        Generator::~Generator()
        {
        }

        Generator::SectionIterator
        Generator::begin_sections() const
        {
            return SectionIterator(_imp->sections.begin());
        }

        Generator::SectionIterator
        Generator::end_sections() const
        {
            return SectionIterator(_imp->sections.end());
        }
    }
}
