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

#include <elf/section.hh>
#include <utils/exception.hh>
#include <utils/private_implementation_pattern-impl.hh>

#include <map>
#include <string>

namespace gpu
{
    template <>
    struct Implementation<elf::Section> :
        public elf::Section::Parameters
    {
        elf::Data data;

        Implementation(const elf::Section::Parameters & parameters) :
            elf::Section::Parameters(parameters),
            data(0)
        {
        }
    };

    template <>
    struct Implementation<elf::SectionTable>
    {
        std::map<std::string, unsigned> sections;

        Implementation()
        {
        }
    };

    namespace elf
    {
        Section::Parameters &
        Section::Parameters::alignment(unsigned alignment)
        {
            _alignment = alignment;

            return *this;
        }

        Section::Parameters &
        Section::Parameters::flags(unsigned flags)
        {
            _flags = flags;

            return *this;
        }

        Section::Parameters &
        Section::Parameters::link(unsigned link)
        {
            _link = link;

            return *this;
        }

        Section::Parameters &
        Section::Parameters::name(const std::string & name)
        {
            _name = name;

            return *this;
        }

        Section::Parameters &
        Section::Parameters::type(unsigned type)
        {
            _type = type;

            return *this;
        }

        Section::Section(const Parameters & parameters) :
            PrivateImplementationPattern<elf::Section>(new Implementation<elf::Section>(parameters))
        {
        }

        Section::~Section()
        {
        }

        Data
        Section::data()
        {
            return _imp->data;
        }

        void
        Section::link(unsigned link)
        {
            _imp->_link = link;
        }

        std::string
        Section::name() const
        {
            return _imp->_name;
        }

        const Section::Parameters &
        Section::parameters() const
        {
            return *_imp;
        }

        SectionTable::SectionTable() :
            PrivateImplementationPattern<elf::SectionTable>(new Implementation<elf::SectionTable>)
        {
        }

        SectionTable::~SectionTable()
        {
        }

        void
        SectionTable::append(const std::string & section)
        {
            _imp->sections.insert(std::pair<const std::string, unsigned>(section, _imp->sections.size() + 1));
        }

        unsigned
        SectionTable::operator[] (const std::string & section) const
        {
            std::map<std::string, unsigned>::const_iterator s(_imp->sections.find(section));
            if (_imp->sections.end() == s)
                return 0;

            return s->second;
        }
    }
}
