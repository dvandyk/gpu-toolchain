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

#include <elf/segment.hh>
#include <utils/exception.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/wrapped_forward_iterator-impl.hh>

#include <vector>

namespace gpu
{
    template class WrappedForwardIterator<elf::Segment::IteratorTag, elf::Section>;

    template <>
    struct Implementation<elf::Segment> :
        public elf::Segment::Parameters
    {
        std::vector<elf::Section> sections;

        unsigned size;

        Implementation(const elf::Segment::Parameters & parameters) :
            elf::Segment::Parameters(parameters)
        {
        }
    };

    namespace elf
    {
        Segment::Parameters &
        Segment::Parameters::alignment(unsigned alignment)
        {
            _alignment = alignment;

            return *this;
        }

        Segment::Parameters &
        Segment::Parameters::address(unsigned address)
        {
            _address = address;

            return *this;
        }

        Segment::Parameters &
        Segment::Parameters::flags(unsigned flags)
        {
            _flags = flags;

            return *this;
        }

        Segment::Parameters &
        Segment::Parameters::type(unsigned type)
        {
            _type = type;

            return *this;
        }

        Segment::Segment(const Parameters & parameters) :
            PrivateImplementationPattern<elf::Segment>(new Implementation<elf::Segment>(parameters))
        {
        }

        Segment::~Segment()
        {
        }

        void
        Segment::append(const Section & section)
        {
            _imp->sections.push_back(section);
        }

        Segment::Iterator
        Segment::begin() const
        {
            return Iterator(_imp->sections.begin());
        }

        Segment::Iterator
        Segment::end() const
        {
            return Iterator(_imp->sections.end());
        }

        const Segment::Parameters &
        Segment::parameters() const
        {
            return *_imp;
        }

        unsigned
        Segment::size() const
        {
            unsigned result(0);

            for (std::vector<elf::Section>::iterator i(_imp->sections.begin()), i_end(_imp->sections.end()) ;
                    i != i_end ; ++i)
            {
                result += i->data().size();
            }

            return result;
        }
    }
}
