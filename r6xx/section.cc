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

#include <utils/private_implementation_pattern-impl.hh>
#include <utils/wrapped_forward_iterator-impl.hh>
#include <r6xx/alu_section.hh>
#include <r6xx/cf_section.hh>
#include <r6xx/error.hh>
#include <r6xx/section.hh>

#include <algorithm>
#include <list>
#include <string>

namespace gpu
{
    template
    struct WrappedForwardIterator<r6xx::Section::IteratorTag, AssemblyEntityPtr>;

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
            else
            {
                throw InvalidSectionNameError(0, name);
            }

            return result;
        }

        bool
        Section::valid(const std::string & name)
        {
            const static std::string section_names[] = 
            {
                ".alu",
                ".cf"
            };
            const static std::string * const section_names_begin(&section_names[0]);
            const static std::string * const section_names_end(section_names_begin + sizeof(section_names) / sizeof(section_names[0]));

            return section_names_end != std::find(section_names_begin, section_names_end, name);
        }
    }
}
