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

#include <utils/private_implementation_pattern-impl.hh>
#include <r6xx/error.hh>
#include <r6xx/section.hh>

#include <algorithm>
#include <string>

namespace gpu
{
    template <>
    struct Implementation<r6xx::Section>
    {
        std::string name;

        Implementation(const std::string & name) :
            name(name)
        {
        }
    };

    namespace r6xx
    {
        Section::Section(const std::string & name) :
            PrivateImplementationPattern<r6xx::Section>(new Implementation<r6xx::Section>(name))
        {
            if (! valid(name))
                throw InvalidSectionNameError(0, name);
        }

        Section::~Section()
        {
        }

        std::string
        Section::name() const
        {
            return _imp->name;
        }

        bool
        Section::valid(const std::string & name)
        {
#define NAME(x) std::string(x)
            const static std::string section_names[] = 
            {
                NAME(".alu"),
                NAME(".cf")
            };
            const static std::string * const section_names_begin(&section_names[0]);
            const static std::string * const section_names_end(section_names_begin + sizeof(section_names) / sizeof(section_names[0]));
#undef NAME

            return section_names_end != std::find(section_names_begin, section_names_end, name);
        }
    }
}
