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

#include <elf/error.hh>
#include <elf/file.hh>
#include <elf/string_table.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/sequence-impl.hh>
#include <utils/wrapped_forward_iterator-impl.hh>

#include <algorithm>
#include <vector>

#include <fcntl.h>
#include <libelf.h>

namespace gpu
{
    template
    struct WrappedForwardIterator<elf::File::SectionIteratorTag, elf::Section>;

    template <>
    struct Implementation<elf::File> :
        public elf::File::Parameters
    {
        std::vector<elf::Section> sections;

        elf::SectionTable section_table;

        elf::StringTable sh_strtab;

        Implementation(const elf::File::Parameters & parameters) :
            elf::File::Parameters(parameters)
        {
            sections.push_back(elf::Section(elf::Section::Parameters()
                        .name(".shstrtab")
                        .type(SHT_STRTAB)
                        .flags(SHF_STRINGS)));
            sh_strtab[".shstrtab"];
            section_table.append(".shstrtab");
        }
    };

    namespace elf
    {
        namespace internal
        {
            struct SectionComparator
            {
                std::string name;

                SectionComparator(const std::string & name) :
                    name(name)
                {
                }

                bool operator() (const Section & section)
                {
                    return section.name() == name;
                }
            };
        }

        File::Parameters &
        File::Parameters::data(unsigned data)
        {
            _data = data;

            return *this;
        }

        File::Parameters &
        File::Parameters::machine(unsigned machine)
        {
            _machine = machine;

            return *this;
        }

        File::Parameters &
        File::Parameters::type(unsigned type)
        {
            _type = type;

            return *this;
        }

        File::File(const Parameters & parameters) :
            PrivateImplementationPattern<elf::File>(new Implementation<elf::File>(parameters))
        {
        }

        File::~File()
        {
        }

        File::Iterator
        File::begin()
        {
            return Iterator(_imp->sections.begin());
        }

        File::Iterator
        File::end()
        {
            return Iterator(_imp->sections.end());
        }

        void
        File::append(const Section & section)
        {
            std::vector<Section>::const_iterator s(std::find_if(
                        _imp->sections.begin(), _imp->sections.end(), internal::SectionComparator(section.name())));
            if (_imp->sections.end() != s)
                throw InternalError("elf", "Trying to append a section that already exists");

            _imp->sections.push_back(section);
            _imp->sh_strtab[section.name()];
            _imp->section_table.append(section.name());
        }

        void
        File::append(const Sequence<Section> & sections)
        {
            for (Sequence<Section>::Iterator s(sections.begin()), s_end(sections.end()) ;
                    s != s_end ; ++s)
            {
                append(*s);
            }
        }

        unsigned
        File::index(const Section & section)
        {
            std::vector<Section>::iterator s(std::find_if(
                        _imp->sections.begin(), _imp->sections.end(), internal::SectionComparator(section.name())));
            if (_imp->sections.end() == s)
                throw InternalError("elf", "Trying to get the index of a section that has not been added yet");

            return std::distance(_imp->sections.begin(), s) + 1;
        }

        const SectionTable &
        File::section_table() const
        {
            return _imp->section_table;
        }

        void
        File::write(const std::string & filename)
        {
            if (elf_version(EV_CURRENT) == EV_NONE)
                throw InternalError("elf", "libelf version mismatch");

            int fd(::open(filename.c_str(), O_WRONLY | O_CREAT, 0777));
            if (fd < 0)
                throw InternalError("elf", "Could not open file '" + filename + "'");

            Elf * e(elf_begin(fd, ELF_C_WRITE, NULL));
            if (0 == e)
                throw Error("elf_begin");

            Elf32_Ehdr * ehdr(elf32_newehdr(e));
            if (0 == ehdr)
                throw Error("elf32_newehdr");

            ehdr->e_ident[EI_DATA] = _imp->_data;
            ehdr->e_machine = _imp->_machine;
            ehdr->e_type = _imp->_type;
            ehdr->e_version = EV_CURRENT;

            _imp->sh_strtab.write(_imp->sections.front().data());
            bool shstrab(true);
            for (std::vector<Section>::iterator s(_imp->sections.begin()), s_end(_imp->sections.end()) ;
                    s != s_end ; ++s)
            {
                Elf_Scn * scn(elf_newscn(e));
                if (0 == scn)
                    throw Error("elf_newscn");

                Elf_Data * data(elf_newdata(scn));
                if (0 == scn)
                    throw Error("elf_newdata");

                data->d_align = s->parameters()._alignment;
                data->d_buf = const_cast<void *>(s->data().buffer());
                data->d_off = 0;
                data->d_size = s->data().size();
                data->d_type = ELF_T_WORD;
                data->d_version = EV_CURRENT;

                Elf32_Shdr * shdr(elf32_getshdr(scn));
                if (0 == shdr)
                    throw Error("elf32_getshdr");

                shdr->sh_entsize = 0;
                shdr->sh_flags = s->parameters()._flags;
                shdr->sh_link = s->parameters()._link;
                shdr->sh_name = _imp->sh_strtab[s->name()];
                shdr->sh_type = s->parameters()._type;

                if (shstrab)
                {
                    ehdr->e_shstrndx = elf_ndxscn(scn);
                    shstrab = false;
                }
            }

            if (elf_update(e, ELF_C_WRITE) < 0)
                throw Error("elf_update");

            elf_end(e);
            close(fd);
        }
    }
}
