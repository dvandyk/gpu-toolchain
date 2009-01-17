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

#include <elf/note_table.hh>
#include <utils/private_implementation_pattern-impl.hh>

#include <string>
#include <vector>

#include <elf.h>

namespace
{
    unsigned multiple_of_four(unsigned input)
    {
        unsigned result(input);

        if (0 == result % 4)
            return result;

        result /= 4;
        result += 1;
        result *= 4;

        return result;
    }
}

namespace gpu
{
    template <>
    struct Implementation<elf::NoteTable>
    {
        static const std::string name;

        std::vector<elf::Note> entries;

        unsigned size;

        Implementation() :
            size(0)
        {
        }
    };

    const std::string Implementation<elf::NoteTable>::name = "GPGPU";

    namespace elf
    {
        Note::Note(unsigned type, const std::string & description) :
            description(description),
            type(type)
        {
        }

        NoteTable::NoteTable() :
            PrivateImplementationPattern<elf::NoteTable>(new Implementation<elf::NoteTable>)
        {
        }

        NoteTable::~NoteTable()
        {
        }

        void
        NoteTable::append(const Note & note)
        {
            _imp->size += sizeof(Elf32_Nhdr)
                + multiple_of_four(Implementation<NoteTable>::name.size())
                + multiple_of_four(note.description.size());
            _imp->entries.push_back(note);
        }

        void
        NoteTable::write(Data data)
        {
            const unsigned name_size(multiple_of_four(Implementation<NoteTable>::name.size()));
            data.resize(_imp->size);

            unsigned offset(0);
            for (std::vector<Note>::const_iterator i(_imp->entries.begin()), i_end(_imp->entries.end()) ;
                    i != i_end ; ++i)
            {
                unsigned desc_size(multiple_of_four(i->description.size()));
                Elf32_Nhdr nhdr;

                nhdr.n_namesz = name_size;
                nhdr.n_descsz = desc_size;
                nhdr.n_type = i->type;

                data.write(offset, reinterpret_cast<char *>(&nhdr), sizeof(nhdr));
                offset += sizeof(nhdr);

                data.write(offset, Implementation<NoteTable>::name.c_str(), Implementation<NoteTable>::name.size());
                offset += name_size;

                data.write(offset, i->description.c_str(), i->description.size());
                offset += desc_size;
            }
        }
    }
}
