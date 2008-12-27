/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2006 Stephen Bennett
 *
 * This file is part of the Paludis package manager. Paludis is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * Paludis is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GPU_GUARD_UTILS_DESTRINGIFY_HH
#define GPU_GUARD_UTILS_DESTRINGIFY_HH 1

#include <utils/exception.hh>
#include <sstream>
#include <string>

namespace gpu
{
    class DestringifyError :
        public Exception
    {
        public:
            DestringifyError(const std::string & str) throw ();
    };

    namespace destringify_internals
    {
        template <typename Type_, typename Exception_>
        struct Destringifier
        {
            static Type_ do_destringify(const std::string & s)
            {
                std::istringstream ss(s);
                Type_ t;
                ss >> t;
                if (! ss.eof() || ss.fail())
                    throw Exception_(s);
                return t;
            }
        };

        template <typename Exception_>
        struct Destringifier<std::string, Exception_>
        {
            static std::string do_destringify(const std::string & s)
            {
                return s;
            }
        };

        template <typename Exception_>
        struct Destringifier<bool, Exception_>
        {
            static bool do_destringify(const std::string & s)
            {
                std::istringstream ss(s);
                if ((s[0] >= 'a' && s[0] <= 'z') || (s[0] >= 'A' && s[0] <= 'Z'))
                {
                    bool b;
                    ss >> std::boolalpha >> b;
                    if (ss.eof() && ! ss.bad())
                        return b;
                    else
                        throw Exception_(s);
                }
                else
                {
                    int i;
                    ss >> i;
                    if (ss.eof() && ! ss.bad())
                        return i > 0;
                    else
                        throw Exception_(s);
                }
            }
        };

        template <typename Exception_>
        struct Destringifier<char, Exception_>
        {
            static char do_destringify(const std::string & s)
            {
                if (s.length() == 1)
                    return s[0];
                else
                    throw Exception_(s);
            }
        };
    }

    template <typename Type_, typename Exception_>
    Type_ destringify(const std::string & s)
    {
        if (s == "")
            throw Exception_("");

        return destringify_internals::Destringifier<Type_, Exception_>::do_destringify(s);
    }

    template <typename Type_>
    Type_ destringify(const std::string & s)
    {
        return destringify<Type_, DestringifyError>(s);
    }
}

#endif
