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

#include <elf/function.hh>
#include <r6xx/linker.hh>
#include <utils/sequence-impl.hh>

#include <elf.h>

#include <iostream> // remove!

namespace gpu
{
    template class Sequence<elf::File>;

    namespace r6xx
    {
        Sequence<elf::Segment>
        Linker::link(const Sequence<elf::File> & files)
        {
            Sequence<elf::Segment> result;

            elf::FunctionPtr main(elf::FunctionFinder::find("main", files));

            if (0 != main.get())
            {
                std::cout << "main: section = " << main->section.name() << std::endl;
            }

            return result;
        }
    }
}
