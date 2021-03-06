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

#ifndef GPU_GUARD_ELF_SECTION_HH
#define GPU_GUARD_ELF_SECTION_HH 1

#include <elf/data.hh>
#include <utils/private_implementation_pattern.hh>

#include <string>

namespace gpu
{
    namespace elf
    {
        class File;

        class Section :
            public PrivateImplementationPattern<elf::Section>
        {
            public:
                class Parameters
                {
                    protected:
                        unsigned _alignment;

                        unsigned _flags;

                        unsigned _link;

                        std::string _name;

                        unsigned _type;

                    public:
                        friend class File;
                        friend class Section;

                        Parameters & alignment(unsigned);

                        Parameters & flags(unsigned);

                        Parameters & link(unsigned);

                        Parameters & name(const std::string &);

                        Parameters & type(unsigned);
                };

                Section(const Parameters & parameters);

                ~Section();

                Data data();

                void link(unsigned);

                std::string name() const;

                const Parameters & parameters() const;
        };

        class SectionTable :
            PrivateImplementationPattern<elf::SectionTable>
        {
            private:
                SectionTable();

                void append(const std::string &);

            public:
                friend class File;
                friend class Implementation<elf::File>;

                ~SectionTable();

                unsigned operator[] (const std::string &) const;
        };
    }
}

#endif
