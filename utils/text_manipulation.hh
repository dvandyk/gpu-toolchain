/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
* Copyright (c) 2008 Danny van Dyk <danny.dyk@tu-dortmund.de>
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

#ifndef GPU_GUARD_UTILS_TEXT_MANIPULATION_HH
#define GPU_GUARD_UTILS_TEXT_MANIPULATION_HH 1

#include <string>

namespace
{
    std::string strip_whitespaces(const std::string & line)
    {
        std::string result(line);

        std::string::size_type begin(result.find_first_not_of(" \t"));
        if (std::string::npos != begin)
            result.erase(0, begin);

        std::string::size_type comment(result.find_first_of('#'));
        if (std::string::npos != comment)
            result.erase(comment);

        std::string::size_type end(result.find_last_not_of(" \t") + 1);
        if (end < result.size())
            result.erase(end);

        return result;
    }

    std::string split_first_word(const std::string & separators, std::string & line)
    {
        std::string::size_type end_word(line.find_first_of(separators));
        std::string::size_type begin_rest(line.find_first_not_of(separators, end_word));
        std::string result(line.substr(0, end_word));

        line.erase(0, begin_rest);

        return result;
    }
}

#endif
