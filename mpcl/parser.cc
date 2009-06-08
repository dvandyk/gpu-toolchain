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

#include <mpcl/parser.hh>
#include <utils/private_implementation_pattern-impl.hh>

#include <iostream>

namespace gpu
{
    template <> struct Implementation<Parser>
    {
        Sequence<std::tr1::shared_ptr<Token> > tokens;

        Implementation(const Sequence<std::tr1::shared_ptr<Token> > & tokens) :
            tokens(tokens)
        {
        }

        ~Implementation()
        {
        }

        bool error(const std::string message)
        {
            std::cout << "[ERROR] " << message << std::endl;

            return false;
        }

        std::tr1::shared_ptr<Token> pop()
        {
            if (tokens.empty())
            {
                std::cout << "NO MORE TOKENS" << std::endl;
                throw std::string("foo");
            }

            //std::cout << "  <pop> '" << tokens.first()->text << "', next is '";
            std::tr1::shared_ptr<Token> result(tokens.first());
            tokens.drop_first();
            //if (! tokens.empty())
            //    std::cout << tokens.first()->text;
            //std::cout << "'" << std::endl;

            return result;
        }

        bool match(const std::string & text)
        {
            bool result(text == tokens.first()->text);

    //        std::cout << "[MATCH] '" << _tokens.front()->text << "' == '" << text << "' -> " << std::boolalpha << result << std::endl;

            return result;
        }

        bool match(TokenType type)
        {
            bool result(type == tokens.first()->type);

    //        std::cout << "[MATCH] TokenType(" << _tokens.front()->type << ") == TokenType(" << type << ") -> " << std::boolalpha << result << std::endl;

            return result;
        }

        std::tr1::shared_ptr<Token> match_and_pop(const std::string & text)
        {
            std::tr1::shared_ptr<Token> result;

            if ((! tokens.empty()) && (match(text)))
            {
                result = pop();
            }

            return result;
        }

        bool function();


        bool parameter_list();

        bool parameter();

        bool parameter_type();


        bool expression();

        bool sum();

        bool product();

        bool scalar();

        bool statement_block();

        bool statement();


        bool declaration_statement();

        bool declaration_type();

        bool declaration_list();

        bool declaration_element();


        bool assignment_statement();


        bool return_statement();
    };

    bool
    Implementation<Parser>::function()
    {
        if (! match(tt_identifier))
            return false;

        std::string return_type(pop()->text);

        if (! match(tt_identifier))
            return error("expected identifier");

        std::string name(pop()->text);

        if (! parameter_list())
            return false;

        if (match(";"))
        {
            pop();
        }
        else if (statement_block())
        {
        }
        else
            return false;

        std::cout << "[function] " << name << ": (unknown) -> " << return_type << std::endl;

        return true;
    }

    bool
    Implementation<Parser>::parameter_list()
    {
        unsigned parameters(1);

        if (! match("("))
            return false;

        pop();

        if (! parameter())
            return false;

        while (match(","))
        {
            pop();
            if (! parameter())
                return false;

            ++parameters;
        }

        if (! match(")"))
            return false;

        pop();

        std::cout << "[parameter-list] " << parameters << " parameters in list" << std::endl;

        return true;
    }

    bool
    Implementation<Parser>::parameter()
    {
        std::string text(tokens.first()->text);

        if (match("void"))
        {
            pop();

            std::cout << "[parameter] void";

            return true;
        }

        if (("in" == text) || ("out" == text) || ("global" == text) || ("const" == text))
            pop();

        if (! parameter_type())
            return false;

        if (! match(tt_identifier))
            return false;

        std::string name(pop()->text);
        std::cout << "[parameter]" << name << std::endl;

        return true;
    }

    bool
    Implementation<Parser>::parameter_type()
    {
        std::string type;

        if (match("vector"))
        {
            pop();

            if (! match("<"))
                return false;

            pop();

            if (! match(tt_identifier))
                return false;

            type = "vector of " + pop()->text;

            if (! match(">"))
                return false;

            pop();
        }
        else if (match(tt_identifier))
        {
            type = pop()->text;
        }
        else
            return false;

        std::cout << "[parameter-type] " << type << std::endl;

        return true;
    }

    bool
    Implementation<Parser>::expression()
    {
        std::cout << "[expression?]" << std::endl;

        return sum();
    }

    bool
    Implementation<Parser>::sum()
    {
        if (! product())
            return false;

        while ((! tokens.empty()) && (("+" == tokens.first()->text) || ("-" == tokens.first()->text)))
        {
            pop();

            if (! product())
                return false;
        }

        return true;
    }

    bool
    Implementation<Parser>::product()
    {
        if (! scalar())
            return false;

        while ((! tokens.empty()) && (("*" == tokens.first()->text) || ("/" == tokens.first()->text)))
        {
            pop();

            if (! scalar())
                return false;
        }

        return true;
    }

    bool
    Implementation<Parser>::scalar()
    {
        if (match(tt_integer) || match(tt_float) || match(tt_identifier))
        {
            pop();

            return true;
        }
        else
        {
            if (! match("("))
                return false;

            pop();

            if (! expression())
                return false;

            if (! match(")"))
                return false;

            pop();

            return true;
        }
    }

    bool
    Implementation<Parser>::statement_block()
    {
        if (! match("{"))
            return false;

        pop();

        std::cout << "[block] starts" << std::endl;

        if (! statement())
            return false;

        while (! match("}"))
        {
            if (! statement())
                return false;
        }

        if (! match("}"))
            return false;

        pop();

        std::cout << "[block] ends" << std::endl;

        return true;
    }

    bool
    Implementation<Parser>::statement()
    {
        if (! tokens.empty())
            std::cout << "[statement?] first is '" << tokens.first()->text << "'" << std::endl;

        if (return_statement())
            return true;
        else if (declaration_statement())
            return true;
        else if (assignment_statement())
            return true;
        else
            return false;
    }

    bool
    Implementation<Parser>::declaration_statement()
    {
        if (! declaration_type())
            return false;

        if (! declaration_list())
            return false;

        if (! match(";"))
            return false;

        pop();

        std::cout << "[declaration-statement] ends" << std::endl;

        return true;
    }

    bool
    Implementation<Parser>::declaration_type()
    {
        if ((! tokens.empty()) && ("const" == tokens.first()->text))
        {
            pop();
        }

        if (! match(tt_identifier))
            return false;

        pop();

        return true;
    }

    bool
    Implementation<Parser>::declaration_list()
    {
        if (! declaration_element())
            return false;

        while (match(","))
        {
            pop();

            if (! declaration_element())
                return false;
        }

        std::cout << "[declaration-list] ends" << std::endl;

        return true;
    }

    bool
    Implementation<Parser>::declaration_element()
    {
        if (! match(tt_identifier))
            return false;

        std::string name(pop()->text);

        if (match("="))
        {
            pop();

            if (! expression())
                return false;
        }

        std::cout << "[declaration-element] " << name << std::endl;

        return true;
    }

    bool
    Implementation<Parser>::assignment_statement()
    {
        return false;
    }

    bool
    Implementation<Parser>::return_statement()
    {
        std::cout << "[return-statement?]" << std::endl;

        std::cout << "[return-statement?] first is '" << tokens.first()->text << "'" << std::endl;

        if (! match("return"))
            return false;

        pop();

        if (! expression())
            return false;

        if (! match(";"))
            return false;

        pop();

        std::cout << "[return-statement]" << std::endl;

        return true;
    }

    Parser::Parser(const Sequence<std::tr1::shared_ptr<Token> > & tokens) :
        PrivateImplementationPattern<Parser>(new Implementation<Parser>(tokens))
    {
    }

    Parser::~Parser()
    {
    }

    void
    Parser::parse()
    {
        while (! _imp->tokens.empty())
        {
            if (! _imp->function())
            {
                std::cout << "not a function" << std::endl;
                throw std::string("foo");
            }
        }
    }
}
