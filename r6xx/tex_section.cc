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
#include <r6xx/tex_section.hh>

namespace gpu
{
    template <>
    ConstVisits<r6xx::tex::Section>::~ConstVisits()
    {
    }

    namespace r6xx
    {
        namespace tex
        {
            Section::Section()
            {
            }

            Section::~Section()
            {
            }

            void
            Section::append(const AssemblyEntityPtr & ptr)
            {
                EntityPtr converted(EntityConverter::convert(ptr));

                if (0 != converted.get())
                {
                    entities.append(converted);
                }
            }

            std::string
            Section::name() const
            {
                return ".tex";
            }

            void
            Section::accept(SectionVisitor & v) const
            {
                static_cast<ConstVisits<r6xx::tex::Section> *>(&v)->visit(*this);
            }

        }
    }
}
