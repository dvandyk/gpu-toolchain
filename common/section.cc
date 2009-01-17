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

#include <common/gpgpu_data_section.hh>
#include <common/gpgpu_notes_section.hh>
#include <common/section.hh>
#include <utils/exception.hh>

#include <algorithm>

namespace gpu
{
    Section::~Section()
    {
    }

    bool
    SectionFactory::valid(const std::string & name)
    {
        const static std::string section_names[] =
        {
            ".gpgpu.data",
            ".gpgpu.notes"
        };
        const static std::string * const section_names_begin(&section_names[0]);
        const static std::string * const section_names_end(section_names_begin + sizeof(section_names) / sizeof(section_names[0]));

        return (section_names_end != std::find(section_names_begin, section_names_end, name));
    }

    SectionPtr
    SectionFactory::make(const std::string & name)
    {
        if (".gpgpu.data" == name)
        {
            return SectionPtr(new common::GPGPUDataSection);
        }
        else if (".gpgpu.notes" == name)
        {
            return SectionPtr(new GPGPUNotesSection);
        }
        else
        {
            throw InternalError("common", "Not a common section: '" + name + "'");
        }
    }
}
