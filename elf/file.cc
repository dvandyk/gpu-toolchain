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
        // Sections
        std::vector<elf::Section> sections;

        elf::SectionTable section_table;

        elf::StringTable sh_strtab;

        // Segments
        std::vector<elf::Segment> segments;

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

        void
        File::append(const Segment & segment)
        {
            _imp->segments.push_back(segment);

            for (Segment::Iterator i(segment.begin()), i_end(segment.end()) ;
                    i != i_end ; ++i)
            {
                append(*i);
            }
        }

        File
        File::create(const Parameters & parameters)
        {
            return File(parameters);
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

        File
        File::open(const std::string & filename)
        {
            if (elf_version(EV_CURRENT) == EV_NONE)
                throw InternalError("elf", "libelf version mismatch");

            int fd(::open(filename.c_str(), O_RDONLY, 0777));
            if (fd < 0)
                throw InternalError("elf", "Could not open file '" + filename + "'");

            Elf * e(elf_begin(fd, ELF_C_READ, NULL));
            if (0 == e)
                throw Error("elf_begin");

            if (ELF_K_ELF != elf_kind(e))
                throw InternalError("elf", "Not an ELF file");

            Elf32_Ehdr * ehdr(elf32_getehdr(e));
            if (0 == ehdr)
                throw Error("elf32_getehdr");

            File result(File::Parameters()
                    .data(ehdr->e_ident[EI_DATA])
                    .machine(ehdr->e_machine)
                    .type(ehdr->e_type));

            // load .shstrtab
            {
                size_t shstrndx(SHN_UNDEF);
                if (0 != elf_getshstrndx(e, &shstrndx))
                    throw Error("elf_getshstrndx");

                Elf_Scn * shstrscn(elf_getscn(e, shstrndx));
                Elf_Data * shstrdata(elf_getdata(shstrscn, 0));

                Data data;
                data.resize(shstrdata->d_size);
                data.write(0, reinterpret_cast<const char *>(shstrdata->d_buf), shstrdata->d_size);

                result._imp->sh_strtab.read(data);
            }

            // load sections
            unsigned index(1);
            for (Elf_Scn * scn(elf_nextscn(e, 0)) ; 0 != scn ; scn = elf_nextscn(e, scn), ++index)
            {
                Elf32_Shdr * shdr(elf32_getshdr(scn));
                Elf_Data * data(elf_getdata(scn, 0));

                Section section(Section::Parameters()
                        .alignment(data->d_align)
                        .flags(shdr->sh_flags)
                        .link(shdr->sh_link)
                        .name(result._imp->sh_strtab[shdr->sh_name])
                        .type(shdr->sh_type));

                section.data().resize(data->d_size);
                section.data().write(0, reinterpret_cast<const char *>(data->d_buf), data->d_size);

                result._imp->sections.push_back(section);
                result._imp->section_table.append(section.name());
            }

            return result;
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

            // Sections
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

            // Segments
            unsigned offset(0);
            Elf32_Phdr * phdr(elf32_newphdr(e, _imp->segments.size()));
            for (std::vector<Segment>::const_iterator s(_imp->segments.begin()), s_end(_imp->segments.end()) ;
                    s != s_end ; ++s)
            {
                phdr->p_vaddr = s->parameters()._address;
                phdr->p_align = s->parameters()._alignment;
                phdr->p_flags = s->parameters()._flags;
                phdr->p_offset = offset;
                phdr->p_filesz = phdr->p_memsz = s->size();
                phdr->p_type = s->parameters()._type;

                offset += s->size();
            }

            if (elf_update(e, ELF_C_WRITE) < 0)
                throw Error("elf_update");

            elf_end(e);
            close(fd);
        }
    }
}
