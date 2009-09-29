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

#include <common/expression.hh>
#include <mpcl/function.hh>
#include <mpcl/parameter.hh>
#include <mpcl/parser.hh>
#include <mpcl/statement.hh>
#include <utils/destringify.hh>
#include <utils/private_implementation_pattern-impl.hh>
#include <utils/sequence-impl.hh>
#include <utils/wrapped_forward_iterator-impl.hh>

#include <iostream>

namespace gpu
{
    ParserErrorMessage::ParserErrorMessage(ParserErrorType type, unsigned line, const std::string & message) :
        type(type),
        line(line),
        message(message)
    {
    }

    bool
    ParserErrorMessage::operator!= (const ParserErrorMessage & other)
    {
        if (this->type != other.type)
            return true;

        if (this->line != other.line)
            return true;

        if (this->message != other.message)
            return true;

        return false;
    }

    std::ostream & operator<< (std::ostream & lhs, const ParserErrorMessage & rhs)
    {
        lhs << "<" << rhs.type << "," << rhs.line << ",'" << rhs.message << "'>";

        return lhs;
    }

    template class Sequence<ParserErrorMessage>;

    template class WrappedForwardIterator<Sequence<ParserErrorMessage>::IteratorTag, ParserErrorMessage>;

    template <> struct Implementation<Parser>
    {
        Sequence<std::tr1::shared_ptr<Token> > tokens;

        std::tr1::shared_ptr<Token> last_token;

        unsigned line_number;

        Sequence<ParserErrorMessage> errors;

        Sequence<FunctionPtr> functions;

        Sequence<StatementPtr> statements;

        Implementation(const Sequence<std::tr1::shared_ptr<Token> > & tokens) :
            tokens(tokens),
            line_number(1)
        {
        }

        ~Implementation()
        {
        }

        bool error(ParserErrorType e)
        {
            bool result(false);

            switch (e)
            {
                case pet_expected_semicolon:
                case pet_expected_closing_brace:
                case pet_expected_closing_chevron:
                case pet_expected_closing_parenthesis:
                    errors.append(ParserErrorMessage(e, line_number, tokens.first()->text));
                    result = true;
                    break;
            }

            return result;
        }

        std::tr1::shared_ptr<Token> pop()
        {
            if (tokens.empty())
            {
                std::cout << "NO MORE TOKENS" << std::endl;
                throw std::string("foo");
            }

            last_token = tokens.first();
            tokens.drop_first();

            while ((! tokens.empty()) && (tt_linebreak == tokens.first()->type))
            {
                ++line_number;
                tokens.drop_first();
            }

            return last_token;
        }

        void push()
        {
            tokens.insert_first(last_token);
        }

        bool match(const std::string & text)
        {
            bool result(text == tokens.first()->text);

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

        std::tr1::shared_ptr<Token> match_and_pop(TokenType type)
        {
            std::tr1::shared_ptr<Token> result;

            if ((! tokens.empty()) && (match(type)))
            {
                result = pop();
            }

            return result;
        }


        FunctionPtr function();


        bool parameter_list(Sequence<std::tr1::shared_ptr<Parameter> >);

        std::tr1::shared_ptr<Parameter> parameter();

        bool parameter_type(std::string & type);


        ExpressionPtr expression();

        ExpressionPtr sum();

        ExpressionPtr product();

        ExpressionPtr scalar();

        bool call_list(Sequence<ExpressionPtr>);


        bool statement_block();

        bool statement();


        bool declaration_statement();

        bool declaration_type(std::string &);

        bool declaration_list(const std::string &);

        bool declaration_element(std::string &);


        bool assignment_statement();


        bool foreach_statement();

        bool foreach_list();

        bool foreach_iterator();


        bool return_statement();
    };

    FunctionPtr
    Implementation<Parser>::function()
    {
        if (! match(tt_identifier))
            return FunctionPtr();

        std::string return_type(pop()->text);

        if (! match(tt_identifier))
            return FunctionPtr();

        FunctionPtr result(new Function(pop()->text));

        if (! parameter_list(result->parameters))
            return FunctionPtr();

        if (match_and_pop(";"))
        {
        }
        else if (statement_block())
        {
        }
        else
            return FunctionPtr();

        result->statements = statements;
        statements = Sequence<StatementPtr>();

        return result;
    }

    bool
    Implementation<Parser>::parameter_list(Sequence<std::tr1::shared_ptr<Parameter> > parameters)
    {
        if (! match_and_pop("("))
            return false;

        std::tr1::shared_ptr<Parameter> p;
        if (! (p = parameter()))
            return false;

        parameters.append(p);

        while (match_and_pop(","))
        {
            if (! (p = parameter()))
                return false;

            parameters.append(p);
        }

        if (! match_and_pop(")"))
            return error(pet_expected_closing_parenthesis);

        return true;
    }

    std::tr1::shared_ptr<Parameter>
    Implementation<Parser>::parameter()
    {
        std::tr1::shared_ptr<Parameter> result;

        if (match_and_pop("void"))
        {
            return std::tr1::shared_ptr<Parameter>(new Parameter("void", ""));
        }

        std::string text(tokens.first()->text);
        if (("in" == text) || ("out" == text) || ("global" == text) || ("const" == text))
            pop();

        std::string type;
        if (! parameter_type(type))
            return result;

        if (! match(tt_identifier))
            return result;

        std::string name(pop()->text);

        return std::tr1::shared_ptr<Parameter>(new Parameter(type, name));
    }

    bool
    Implementation<Parser>::parameter_type(std::string & type)
    {
        if (match_and_pop("vector"))
        {
            if (! match_and_pop("<"))
                return false;

            if (! match(tt_identifier))
                return false;

            type = "vector of " + pop()->text;

            if (! match_and_pop(">"))
                return error(pet_expected_closing_chevron);
        }
        else if (match(tt_identifier))
        {
            type = pop()->text;
        }
        else
            return false;

        return true;
    }

    ExpressionPtr
    Implementation<Parser>::expression()
    {
        return sum();
    }

    ExpressionPtr
    Implementation<Parser>::sum()
    {
        ExpressionPtr lhs, rhs;
        std::string op;

        if (! (lhs = product()))
            return ExpressionPtr();

        while ((! tokens.empty()) && (("+" == tokens.first()->text) || ("-" == tokens.first()->text)))
        {
            op = pop()->text;

            if (! (rhs = product()))
                return ExpressionPtr();

            if ("+" == op)
                lhs = ExpressionPtr(new Sum(lhs, rhs));
            else
                lhs = ExpressionPtr(new Difference(lhs, rhs));
        }

        return lhs;
    }

    ExpressionPtr
    Implementation<Parser>::product()
    {
        ExpressionPtr lhs, rhs;
        std::string op;

        if (! (lhs = scalar()))
            return ExpressionPtr();

        while ((! tokens.empty()) && (("*" == tokens.first()->text) || ("/" == tokens.first()->text)))
        {
            op = pop()->text;

            if (! (rhs = scalar()))
                return ExpressionPtr();

            if ("*" == op)
                lhs = ExpressionPtr(new Product(lhs, rhs));
            else
                lhs = ExpressionPtr(new Quotient(lhs, rhs));
        }

        return lhs;
    }

    ExpressionPtr
    Implementation<Parser>::scalar()
    {
        ExpressionPtr result;
        std::string text(tokens.first()->text);

        switch (tokens.first()->type)
        {
            case tt_integer:
            case tt_float:
                result = ExpressionPtr(new Value(Number(text)));
                pop();
                break;

            case tt_identifier:
                pop();

                if (match("("))
                {
                    Sequence<ExpressionPtr> params;

                    if (! call_list(params))
                        return ExpressionPtr();

                    result = ExpressionPtr(new Call(text, params));
                }
                else
                {
                    result = ExpressionPtr(new Variable(text));
                }

                break;

            default:
                if (! match_and_pop("("))
                    return ExpressionPtr();

                if (! (result = expression()))
                    return ExpressionPtr();

                if (! match_and_pop(")"))
                {
                    if (! error(pet_expected_closing_parenthesis))
                        result = ExpressionPtr();
                }
        }

        return result;
    }

    bool
    Implementation<Parser>::call_list(Sequence<ExpressionPtr> params)
    {
        if (! match_and_pop("("))
            return false;

        if (match_and_pop(")"))
            return true;

        std::string next(tokens.first()->text);
        ExpressionPtr e(expression());

        if (! e)
        {
            return false;
        }

        params.append(e);

        while (match_and_pop(","))
        {
            e = expression();

            if (! e)
                return false;

            params.append(e);
        }

        if (! match_and_pop(")"))
            error(pet_expected_closing_parenthesis);

        return true;
    }

    bool
    Implementation<Parser>::statement_block()
    {
        if (! match_and_pop("{"))
            return false;

        if (! statement())
            return false;

        while (! match("}"))
        {
            if (! statement())
                return false;
        }

        if (! match("}"))
            return error(pet_expected_closing_brace);

        pop();

        return true;
    }

    bool
    Implementation<Parser>::statement()
    {
        if (return_statement())
            return true;
        else if (foreach_statement())
            return true;
        else if (assignment_statement())
            return true;
        else if (declaration_statement())
            return true;
        else
            return false;
    }

    bool
    Implementation<Parser>::declaration_statement()
    {
        std::string type;
        if (! declaration_type(type))
            return false;

        if (! declaration_list(type))
            return false;

        if (! match_and_pop(";"))
            return error(pet_expected_semicolon);

        return true;
    }

    bool
    Implementation<Parser>::declaration_type(std::string & type)
    {
        std::string modifier;

        if ((! tokens.empty()) && ("const" == tokens.first()->text))
        {
            modifier = "const ";
            pop();
        }

        if (! match(tt_identifier))
            return false;

        type = modifier + pop()->text;

        return true;
    }

    bool
    Implementation<Parser>::declaration_list(const std::string & type)
    {
        std::string name;

        if (! declaration_element(name))
            return false;

        statements.append(StatementPtr(new Declaration(type, name)));

        while (match(","))
        {
            name = "";
            pop();

            if (! declaration_element(name))
                return false;

            statements.append(StatementPtr(new Declaration(type, name)));
        }

        return true;
    }

    bool
    Implementation<Parser>::declaration_element(std::string & name)
    {
        if (! match(tt_identifier))
            return false;

        name = pop()->text;

        if (match_and_pop("="))
        {
            if (! expression())
                return false;
        }

        return true;
    }

    bool
    Implementation<Parser>::assignment_statement()
    {
        std::tr1::shared_ptr<Token> lhs;
        if (! (lhs = match_and_pop(tt_identifier)))
            return false;

        if (! match_and_pop("="))
        {
            push();

            return false;
        }

        ExpressionPtr exp;
        if (! (exp = expression()))
            return false;

        if (! match_and_pop(";"))
            return error(pet_expected_semicolon);

        statements.append(StatementPtr(new Assignment(lhs->text, exp)));

        return true;
    }

    bool
    Implementation<Parser>::foreach_statement()
    {
        if (! match_and_pop("foreach"))
            return false;

        if (! foreach_list())
            return false;

        if (! statement_block())
            return false;

        // add to list of blocks

        return true;
    }

    bool
    Implementation<Parser>::foreach_list()
    {
        if (! match_and_pop("("))
            return false;

        if (! foreach_iterator())
            return false;

        while (match_and_pop(","))
        {
            if (! foreach_iterator())
                return false;
        }

        if (! match_and_pop(")"))
            return error(pet_expected_closing_parenthesis);

        return true;
    }

    bool
    Implementation<Parser>::foreach_iterator()
    {
        if (! match_and_pop(tt_identifier))
            return false;

        if (! match_and_pop("in"))
            return false;

        if (! match_and_pop(tt_identifier))
            return false;

        return true;
    }

    bool
    Implementation<Parser>::return_statement()
    {
        if (! match_and_pop("return"))
            return false;

        ExpressionPtr exp;
        if (! (exp = expression()))
            return false;

        if (! match_and_pop(";"))
            return error(pet_expected_semicolon);

        statements.append(StatementPtr(new Return(exp)));

        return true;
    }

    Parser::Parser(const Sequence<std::tr1::shared_ptr<Token> > & tokens) :
        PrivateImplementationPattern<Parser>(new Implementation<Parser>(tokens))
    {
    }

    Parser::~Parser()
    {
    }

    Sequence<FunctionPtr>
    Parser::parse()
    {
        while (! _imp->tokens.empty())
        {
            FunctionPtr f(_imp->function());

            if (! f)
            {
                std::cout << "file.mpc" << ":" << _imp->line_number << ": " << "The previous translation element was not a proper function!" << std::endl;
                break;
            }
            else
            {
                _imp->functions.append(f);

                std::cout << "FUNCTION '" << f->name << "'" << std::endl;
                StatementPrinter p;
                for (Sequence<StatementPtr>::Iterator i(f->statements.begin()), i_end(f->statements.end()) ;
                        i != i_end ; ++i)
                {
                    (*i)->accept(p);
                }

                std::cout << p.output() << std::flush;
            }
        }

        return _imp->functions;
    }

    Sequence<ParserErrorMessage>
    Parser::errors()
    {
        return _imp->errors;
    }
}
