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

#ifndef GPU_GUARD_ELF_FILE_HH
#define GPU_GUARD_ELF_FILE_HH 1

#include <elf/section.hh>
#include <elf/segment.hh>
#include <elf/string_table.hh>
#include <elf/symbol_table.hh>
#include <utils/private_implementation_pattern.hh>
#include <utils/sequence.hh>
#include <utils/wrapped_forward_iterator.hh>

namespace gpu
{
    namespace elf
    {
        class File :
            public PrivateImplementationPattern<elf::File>
        {
            public:
                class Parameters
                {
                    protected:
                        unsigned _data;

                        unsigned _machine;

                        unsigned _type;

                    public:
                        friend class File;

                        Parameters & data(unsigned data);

                        Parameters & machine(unsigned machine);

                        Parameters & type(unsigned type);
                };

            private:
                File(const Parameters & parameters);

            public:
                static File open(const std::string & filename);

                static File create(const Parameters & parameters);

                ~File();

                struct SectionIteratorTag;
                typedef WrappedForwardIterator<SectionIteratorTag, elf::Section> Iterator;

                Iterator begin() const;

                Iterator end() const;

                Iterator find(const std::string & name) const;

                void append(const Section & section);

                void append(const Sequence<Section> & sections);

                void append(const Segment & segment);

                unsigned index(const Section & section);

                const SectionTable & section_table() const;

                SymbolTable symbol_table() const;

                void write(const std::string & filename);
        };
    }
}

#endif
