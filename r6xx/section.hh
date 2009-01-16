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
#include <elf/symbol.hh>
#include <r6xx/section-fwd.hh>
#include <utils/private_implementation_pattern.hh>
#include <utils/sequence.hh>
#include <utils/visitor.hh>

#include <string>

namespace gpu
{
    namespace r6xx
    {
        typedef ConstVisitorTag<r6xx::alu::Section, r6xx::cf::Section, r6xx::tex::Section> Sections;

        typedef ConstVisitor<Sections> SectionVisitor;

        enum SectionId
        {
            sid_cf = 0,
            sid_alu = 1,
            sid_tex = 2
        };

        class Section :
            public ConstVisitable<Sections>
        {
            public:
                virtual ~Section() = 0;

                virtual void append(const AssemblyEntityPtr &) = 0;

                virtual std::string name() const = 0;

                virtual Sequence<elf::Symbol> symbols() const = 0;

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

        struct SectionPrinter
        {
            static std::string print(const SectionPtr &);
        };

        struct SectionConverter
        {
            static Sequence<SectionPtr> convert(const Sequence<AssemblyEntityPtr> &);
        };
    }
}

#endif
