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

#include <mpcl/scanner.hh>
#include <utils/private_implementation_pattern-impl.hh>

#include <istream>
#include <string>

namespace gpu
{
    template <> struct Implementation<Scanner>
    {
        std::tr1::function<void (void)> scan;

        std::istream & stream;

        std::string text;

        Sequence<std::tr1::shared_ptr<Token> > tokens;

        TokenType type;

        Implementation(std::istream & stream) :
            stream(stream),
            text(""),
            type(tt_invalid)
        {
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_default), this);
        }

        ~Implementation()
        {
        }

        void accept()
        {
            tokens.append(std::tr1::shared_ptr<Token>(new Token(text, type)));
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_default), this);
            text = "";
        }

        void error()
        {
            throw std::string("Scanner");
        }

        void get()
        {
            char c;
            stream.get(c);

            text += c;
        }

        char peek()
        {
            return static_cast<char>(stream.peek());
        }

        void scan_comment();

        void scan_default();

        void scan_float_exp();

        void scan_float();

        void scan_identifier();

        void scan_integer();

        void scan_maybe_comment();

        void scan_maybe_comment_end();

        void scan_maybe_trailing_asterisk();

        void scan_maybe_trailing_equal();
    };

    /* stateful scanning functions */
    void
    Implementation<Scanner>::scan_comment()
    {
        get();

        char c(*text.rbegin());

        if ('*' == c)
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_maybe_comment_end), this);
    }

    void
    Implementation<Scanner>::scan_default()
    {
        static const std::string delimiters(":.;,()[]{}");
        static const std::string digits("0123456789");
        static const std::string letters("abcdefghijklmnopqrstuvwxyz"
                                         "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        static const std::string maybe_trailing_equal("<>=+-");
        static const std::string maybe_trailing_asterisk("*");
        static const std::string whitespaces(" \t\n");

        type = tt_invalid;
        get();

        char c(*text.rbegin());

        if ('/' == c)
        {
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_maybe_comment), this);
        }
        else if (std::string::npos != delimiters.find_first_of(c))
        {
            type = tt_delimiter;
            accept();
        }
        else if (std::string::npos != letters.find_first_of(c))
        {
            type = tt_identifier;
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_identifier), this);
        }
        else if (std::string::npos != digits.find_first_of(c))
        {
            type = tt_integer;
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_integer), this);
        }
        else if (std::string::npos != maybe_trailing_equal.find_first_of(c))
        {
            type = tt_operator;
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_maybe_trailing_equal), this);
        }
        else if (std::string::npos != maybe_trailing_asterisk.find_first_of(c))
        {
            type = tt_operator;
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_maybe_trailing_asterisk), this);
        }
        else if (std::string::npos != whitespaces.find_first_of(c))
        {
            text = "";
        }
        else if (! stream.eof())
        {
            error();
        }
    }

    void
    Implementation<Scanner>::scan_float_exp()
    {
        static const std::string digits("0123456789");

        char c(peek());

        if (std::string::npos != digits.find_first_of(c))
        {
            get();
        }
        else if (('E' == *text.rbegin()) && (('-' == c) || ('+' == c)))
        {
            get();
        }
        else
        {
            type = tt_float;
            accept();
        }
    }

    void
    Implementation<Scanner>::scan_float()
    {
        static std::string digits("0123456789");

        char c(peek());

        if ('E' == c)
        {
            get();
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_float_exp), this);
        }
        else if (std::string::npos != digits.find_first_of(c))
            get();
        else
        {
            type = tt_float;
            accept();
        }
    }

    void
    Implementation<Scanner>::scan_identifier()
    {
        static const std::string valid("abcdefghijklmnopqrstuvwxyz"
                                       "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                       "0123456789_");
        char c(peek());

        if (std::string::npos != valid.find_first_of(c))
            get();
        else
        {
            type = tt_identifier;
            accept();
        }
    }

    void
    Implementation<Scanner>::scan_integer()
    {
        static const std::string digits("0123456789");

        char c(peek());

        if (std::string::npos != digits.find_first_of(c))
        {
            get();
        }
        else if ('.' == c)
        {
            get();
            type = tt_float;
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_float), this);
        }
        else if ('E' == c)
        {
            get();
            type = tt_float;
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_float_exp), this);
        }
        else
        {
            accept();
        }
    }

    void
    Implementation<Scanner>::scan_maybe_comment()
    {
        char c(peek());

        if ('*' == c)
        {
            get();
            type = tt_comment;
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_comment), this);
        }
        else
        {
            accept();
        }
    }

    void
    Implementation<Scanner>::scan_maybe_comment_end()
    {
        get();

        char c(*text.rbegin());

        if ('/' == c)
        {
            accept();
        }
        else
        {
            scan = std::tr1::bind(std::tr1::mem_fn(&Implementation<Scanner>::scan_comment), this);
        }
    }

    void
    Implementation<Scanner>::scan_maybe_trailing_asterisk()
    {
        char c(peek());

        if ('*' == c)
        {
            get();
        }

        accept();
    }

    void
    Implementation<Scanner>::scan_maybe_trailing_equal()
    {
        char c(peek());

        if ('=' == c)
        {
            get();
        }

        accept();
    }

    Scanner::Scanner(std::istream & stream) :
        PrivateImplementationPattern<Scanner>(new Implementation<Scanner>(stream))
    {
    }

    Scanner::~Scanner()
    {
    }

    void
    Scanner::scan()
    {
        while (_imp->stream)
        {
            _imp->scan();
        }
    }

    Sequence<std::tr1::shared_ptr<Token> >
    Scanner::tokens() const
    {
        return _imp->tokens;
    }

}
