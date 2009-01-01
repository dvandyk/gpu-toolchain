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

#ifndef GPU_GUARD_R6XX_SECTION_HH
#define GPU_GUARD_R6XX_SECTION_HH 1

#include <common/assembly_entities-fwd.hh>
#include <utils/private_implementation_pattern.hh>
#include <utils/wrapped_forward_iterator.hh>

namespace gpu
{
    namespace r6xx
    {
        class Section;

        typedef std::tr1::shared_ptr<Section> SectionPtr;

        class Section
        {
            public:
                virtual ~Section() = 0;

                struct IteratorTag;
                typedef WrappedForwardIterator<IteratorTag, AssemblyEntityPtr> Iterator;

                virtual Iterator begin() const = 0;

                virtual Iterator end() const = 0;

                virtual void append(const AssemblyEntityPtr &) = 0;

                virtual std::string name() const = 0;

                static SectionPtr make(const std::string & name);

                static bool valid(const std::string & name);
        };

        class SectionNameComparator
        {
            private:
                const std::string _name;

            public:
                SectionNameComparator(const std::string & name) :
                    _name(name)
                {
                }

                bool operator() (const SectionPtr & section)
                {
                    return section->name() == _name;
                }
        };
    }
}

#endif
