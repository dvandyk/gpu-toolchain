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

#include <r6xx/assembler.hh>
#include <r6xx/error.hh>
#include <r6xx/section.hh>
#include <r6xx/symbol.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/sequence-impl.hh>
#include <utils/wrapped_forward_iterator-impl.hh>

#include <algorithm>
#include <list>

namespace gpu
{
    template <>
    struct Implementation<r6xx::Assembler>
    {
        Sequence<r6xx::SectionPtr> sections;

        Sequence<r6xx::Symbol> symbols;
    };

    namespace r6xx
    {
        Assembler::Assembler(const Sequence<std::tr1::shared_ptr<AssemblyEntity> > & entities) :
            PrivateImplementationPattern<r6xx::Assembler>(new Implementation<r6xx::Assembler>)
        {
            _imp->sections = SectionConverter::convert(entities);

            _imp->symbols = SymbolScanner::scan(_imp->sections);
        }

        Assembler::~Assembler()
        {
        }

        Assembler::SectionIterator
        Assembler::begin_sections() const
        {
            return SectionIterator(_imp->sections.begin());
        }

        Assembler::SectionIterator
        Assembler::end_sections() const
        {
            return SectionIterator(_imp->sections.end());
        }

        Assembler::SymbolIterator
        Assembler::begin_symbols() const
        {
            return SymbolIterator(_imp->symbols.begin());
        }

        Assembler::SymbolIterator
        Assembler::end_symbols() const
        {
            return SymbolIterator(_imp->symbols.end());
        }
    }
}
