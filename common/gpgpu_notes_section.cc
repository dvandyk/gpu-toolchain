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

#include <common/gpgpu_notes_section.hh>
#include <common/syntax.hh>
#include <elf/note_table.hh>
#include <elf/section.hh>
#include <utils/sequence-impl.hh>

#include <string>

#include <elf.h>

namespace gpu
{
    GPGPUNotesSection::GPGPUNotesSection()
    {
    }

    GPGPUNotesSection::~GPGPUNotesSection()
    {
    }

    void
    GPGPUNotesSection::append(const AssemblyEntityPtr &)
    {
        throw CommonSyntaxError("Section '.gpgpu.notes' does not accept any entities.");
    }

    std::string
    GPGPUNotesSection::name() const
    {
        return ".gpgpu.notes";
    }

    Sequence<elf::Section>
    GPGPUNotesSection::sections(const elf::SymbolTable &, const Sequence<elf::Symbol> &) const
    {
        Sequence<elf::Section> result;
        elf::NoteTable notetab;
        elf::Section section(elf::Section::Parameters()
                .alignment(0)
                .flags(0)
                .name(".gpgpu.notes")
                .type(SHT_NOTE));

        result.append(section);

        notetab.append(elf::Note(0x0, GPU_VERSION));
        notetab.write(section.data());

        return result;
    }

    Sequence<elf::Symbol>
    GPGPUNotesSection::symbols() const
    {
        return Sequence<elf::Symbol>();
    }
}
