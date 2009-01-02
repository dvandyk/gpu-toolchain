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

#include <common/syntax.hh>
#include <utils/exception.hh>
#include <utils/stringify.hh>

#include <list>
#include <string>
#include <utility>

namespace gpu
{
    namespace internal
    {
        static __thread std::string * file = 0;

        static __thread unsigned line = 0;
    }

    SyntaxContext::File::File(const std::string & file)
    {
        if (! internal::file)
        {
            internal::file = new std::string(file);
        }
        else
        {
            *internal::file = file;
        }
    }

    SyntaxContext::File::~File()
    {
    }

    SyntaxContext::Line::Line(unsigned line)
    {
        internal::line = line;
    }

    SyntaxContext::Line::~Line()
    {
    }

    static std::string make_prefix()
    {
        std::string file("<none>");
        if (internal::file)
        {
            file = *internal::file;
        }

        return file + ":" + stringify(internal::line);
    }

    SyntaxError::SyntaxError(const std::string & backend, const std::string & message) :
        Exception(make_prefix() + ": (" + backend + ") " + message)
    {
    }
}
