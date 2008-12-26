/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2005, 2006, 2007 Ciaran McCreesh
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

#ifndef GPU_GUARD_UTILS_STRINGIFY_HH
#define GPU_GUARD_UTILS_STRINGIFY_HH 1

#include <sstream>
#include <string>
#include <tr1/memory>

namespace gpu
{
    namespace stringify_internals
    {
        /**
         * Check that T_ is a sane type to be stringified.
         */
        template <typename T_>
        struct CheckType
        {
            /// Yes, we are a sane type.
            enum { value = 0 } Value;
        };

        /**
         * Check that T_ is a sane type to be stringified, which it isn't
         * if it's a pointer unless it's a char * pointer.
         */
        template <typename T_>
        struct CheckType<T_ *>
        {
        };

        /**
         * Check that T_ is a sane type to be stringified, which it isn't
         * if it's a CountedPtr.
         */
        template <typename T_>
        struct CheckType<std::tr1::shared_ptr<T_> >
        {
        };

        /**
         * Check that T_ is a sane type to be stringified, which it isn't
         * if it's a pointer unless it's a char * pointer.
         */
        template <>
        struct CheckType<char *>
        {
            /// Yes, we are a sane type.
            enum { value = 0 } Value;
        };
    }

    /**
     * Convert item to a string.
     */
    template <typename T_>
    std::string
    stringify(const T_ & item)
    {
        std::ostringstream s;
        s << item;
        return s.str();
    }

    /**
     * Convert item to a string (overload for std::string).
     */
    inline std::string
    stringify(const std::string & item)
    {
        return item;
    }

    /**
     * Convert item to a string (overload for char).
     */
    inline std::string
    stringify(const char & item)
    {
        return std::string(1, item);
    }

    /**
     * Convert item to a string (overload for unsigned char).
     */
    inline std::string
    stringify(const unsigned char & item)
    {
        return std::string(1, item);
    }

    /**
     * Convert item to a string (overload for bool).
     */
    inline std::string
    stringify(const bool & item)
    {
        return item ? "true" : "false";
    }

    /**
     * Convert item to a string (overload for char *, which isn't a
     * screwup like other pointers).
     */
    inline std::string
    stringify(const char * const item)
    {
        return std::string(item);
    }
}

#endif
