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
#include <istream>
#include <sstream>
#include <vector>

namespace gpu
{
    template <>
    struct Implementation<ParserError>
    {
        unsigned line;

        std::string message;

        Implementation(unsigned line, const std::string & message) :
            line(line),
            message(message)
        {
        }
    };

    ParserError::ParserError(unsigned line, ParserErrorType type, const std::string & text) :
        PrivateImplementationPattern<ParserError>(new Implementation<ParserError>(line, ""))
    {
        static const std::string messages[] = 
        {
            "Expected ',' before '@'",
            "Expected ';' before '@'",
            "Expected '}' before '@'",
            "Expected '>' before '@'",
            "Expected ')' before '@'"
        };

        if (type >= pet_last)
            throw InternalError("mpcl", "ParserErrorType 'type' exceeds bounds");

        _imp->message = messages[type]; // TODO: Replace @ by text.
    }

    ParserError::ParserError(unsigned line, const std::string & msg) :
        PrivateImplementationPattern<ParserError>(new Implementation<ParserError>(line, msg))
    {
    }

    ParserError::~ParserError()
    {
    }

    unsigned
    ParserError::line() const
    {
        return _imp->line;
    }

    const std::string &
    ParserError::message() const
    {
        return _imp->message;
    }

    bool operator!= (const ParserError & lhs, const ParserError & rhs)
    {
        if (lhs.line() != rhs.line())
            return true;

        if (lhs.message() != rhs.message())
            return true;

        return false;
    }

    std::ostream & operator<< (std::ostream & lhs, const ParserError & rhs)
    {
        lhs << rhs.line() << ":" << rhs.message();

        return lhs;
    }

    template class Sequence<ParserError>;

    template class WrappedForwardIterator<Sequence<ParserError>::IteratorTag, ParserError>;

    namespace internal
    {
        struct Scope
        {
            Sequence<StatementPtr> statements;
        };
    }

    template <> struct Implementation<Parser>
    {
        std::stringbuf buffer;

        unsigned line;

        Sequence<ParserError> errors;

        Sequence<FunctionPtr> functions;

        std::vector<internal::Scope> scopes;

        Implementation(std::istream & stream) :
            line(1)
        {
            stream.get(buffer, '\0');

            while (0 < buffer.in_avail())
            {
                if (! func())
                {
                    std::cout << "Translation element is not a function" << std::endl;
                    error("Translation element is not a function");
                    break;
                }
            }
        }

        ~Implementation()
        {
        }

        void error(ParserErrorType type, const std::string & text)
        {
            errors.append(ParserError(line, type, text));
            //std::cout << "ERROR: " << errors.last() << std::endl;
        }

        void error(const std::string & message)
        {
            errors.append(ParserError(line, message));
            //std::cout << "ERROR: " << errors.last() << std::endl;
        }

        bool matches_any_of(const std::string & ensemble)
        {
            char c(buffer.sgetc());

            return std::string::npos != ensemble.find(c);
        }

        bool matches(char c)
        {
            return c == buffer.sgetc();
        }

        bool matches(const std::string & s)
        {
            std::string data(s.size(), '\0');

            if (s.size() != buffer.sgetn(&data[0], s.size()))
                return false;

            return s == data;
        }

        bool match_and_discard(char c)
        {
            if (c == char(buffer.sgetc()))
            {
                buffer.snextc();

                return true;
            }

            return false;
        }

        bool match_and_discard(const std::string & s)
        {
            std::string data(s.size(), '\0');

            if (s.size() != buffer.sgetn(&data[0], s.size()))
                return false;

            if (data != s)
                return false;

            buffer.pubseekoff(s.size(), std::ios_base::cur);

            return true;
        }

        char get_one()
        {
            return buffer.sbumpc();
        }

        std::string get_any_of(const std::string & ensemble)
        {
            std::string result;

            char next(buffer.sgetc());
            while (std::string::npos != ensemble.find(next))
            {
                result += next;
                next = buffer.snextc();
            }

            return result;
        }

        void skip_irrelevant()
        {
            static const std::string whitespaces(" \t\n");

            while (matches_any_of(whitespaces))
            {
                if ('\n' == buffer.snextc())
                    ++line;
            }
        }

        void push_statement(const StatementPtr & statement)
        {
            if (scopes.empty())
                throw InternalError("mpcl", "Empty stack of Scopes when pushing statement");

            scopes.back().statements.append(statement);
        }

        /* Directly from the grammar (mpcl/mpcl.txt) */

        std::string id();
        NumberPtr number();
        std::string comment();

        FunctionPtr func();
        bool func_param_list(Sequence<ParameterPtr>);
        std::tr1::shared_ptr<Parameter> func_param();

        std::tr1::shared_ptr<Type> type();

        ExpressionPtr exp();
        ExpressionPtr exp_sum();
        ExpressionPtr exp_prod();
        ExpressionPtr exp_power();
        ExpressionPtr exp_elem();
        Sequence<ExpressionPtr> exp_list_elem();

        bool stmt_block();
        bool stmt();

        bool stmt_decl();
        bool stmt_decl_list();
        bool stmt_decl_elem();

        bool stmt_assign();

        bool stmt_foreach();
        bool stmt_foreach_list();
        bool stmt_foreach_elem();

        bool stmt_return();
    };


    std::string
    Implementation<Parser>::id()
    {
        //std::cout << "id()" << std::endl;
        static const std::string head("abcdefghijklmnopqrstuvwxyz"
                                      "ABCDEFGHIKKLMNOPQRSTUVWXYZ");
        static const std::string body(head + "0123456789_");

        skip_irrelevant();

        std::string result;
        if (! matches_any_of(head))
        {
            return result;
        }

        result = get_one();
        result += get_any_of(body);

        std::cout << "id(): id = " << result << std::endl;

        return result;
    }

    NumberPtr
    Implementation<Parser>::number()
    {
        std::cout << "number()" << std::endl;
        static const std::string digits("0123456789");
        std::string raw;
        NumberPtr result;

        skip_irrelevant();

        if (matches_any_of("+-"))
        {
            raw = get_one();
        }

        std::string integer(get_any_of(digits));
        if (integer.empty())
        {
            error(std::string("Expected at least one digit after '") + raw[0] + "'");
            return result;
        }
        raw += integer;

        if (! matches('.'))
        {
            return NumberPtr(new Number(raw));
        }
        else
        {
            get_one();
        }

        std::string fractional(get_any_of(digits));
        if (fractional.empty())
        {
            error("Expected at least one digit after '.'");
            return NumberPtr();
        }
        raw += fractional;

        if (! matches_any_of("eE"))
        {
            return NumberPtr(new Number(raw));
        }

        raw += get_one();

        if (! matches_any_of("+-"))
        {
            raw += get_one();
        }

        std::string exponent(get_any_of(digits));
        if (exponent.empty())
        {
            error("Expected at least one digit after '" + raw + "'");
            return NumberPtr();
        }
        raw += exponent;

        return NumberPtr(new Number(raw));
    }

    FunctionPtr
    Implementation<Parser>::func()
    {
        std::cout << "func()" << std::endl;
        FunctionPtr result;

        skip_irrelevant();

        std::tr1::shared_ptr<Type> func_type;
        if (! (func_type = type()))
        {
            error("Expected return type specifier");
            return FunctionPtr();
        }
        std::cout << "func(): func_type = " << func_type->type << std::endl;

        skip_irrelevant();

        std::string func_name;
        if ((func_name = id()).empty())
        {
            error("Expected identifier");
            return FunctionPtr();
        }

        std::cout << "func(): func_name = " << func_name << std::endl;

        skip_irrelevant();

        Sequence<ParameterPtr> func_params;
        if (! func_param_list(func_params))
        {
            return FunctionPtr();
        }

        skip_irrelevant();

        Sequence<StatementPtr> func_statements;
        if (matches('{'))
        {
            scopes.push_back(internal::Scope());
            stmt_block();
            func_statements = scopes.back().statements;
            scopes.pop_back();
        }
        else if (match_and_discard(';'))
        {
        }
        else
        {
            error("Expected either ';' or block of statements after ')'");
        }

        result = FunctionPtr(new Function(func_name, func_type, func_params, func_statements));

        return result;
    }

    bool
    Implementation<Parser>::func_param_list(Sequence<ParameterPtr> parameters)
    {
        skip_irrelevant();

        if (! match_and_discard("("))
            return false;

        skip_irrelevant();

        ParameterPtr param;
        if (! (param = func_param()))
            return false;

        parameters.append(param);

        skip_irrelevant();

        while (match_and_discard(','))
        {
            skip_irrelevant();

            if (! (param = func_param()))
            {
                return false;
            }

            parameters.append(param);

            skip_irrelevant();
        }

        if (! match_and_discard(")"))
        {
            error(pet_expected_closing_parenthesis, "NYI");
        }

        return true;
    }

    ParameterPtr
    Implementation<Parser>::func_param()
    {
        ParameterPtr result;

        skip_irrelevant();

        if (match_and_discard("void"))
        {
            return ParameterPtr(new Parameter("void", ""));
        }

        // TODO: func-param-mod

        TypePtr func_param_type(type());
        if (! func_param_type)
            return result;

        skip_irrelevant();

        std::string func_param_name;
        if ((func_param_name = id()).empty())
            return result;

        result = ParameterPtr(new Parameter(func_param_type->type, func_param_name));

        return result;
    }

    TypePtr
    Implementation<Parser>::type()
    {
        //std::cout << "type()" << std::endl;
        std::string type_name;
        if ((type_name = id()).empty())
        {
            return TypePtr();
        }

        if (! match_and_discard('<'))
        {
            return TypePtr(new Type(type_name));
        }

        std::string type_spec;
        if ((type_spec = id()).empty())
        {
            error("Expected identifier after '<'");
            return TypePtr();
        }

        if (! match_and_discard('>'))
        {
            error(pet_expected_closing_chevron, "?");
        }

        return TypePtr(new Type(type_name + " of " + type_spec));
    }

    ExpressionPtr
    Implementation<Parser>::exp()
    {
        return exp_sum();
    }

    ExpressionPtr
    Implementation<Parser>::exp_sum()
    {
        ExpressionPtr lhs, rhs;

        if (! (lhs = exp_prod()))
            return ExpressionPtr();

        while (matches_any_of("+-"))
        {
            char op(get_one());

            if (! (rhs = exp_prod()))
                return ExpressionPtr();

            if ('+' == op)
                lhs = ExpressionPtr(new Sum(lhs, rhs));
            else
                lhs = ExpressionPtr(new Difference(lhs, rhs));
        }

        return lhs;
    }

    ExpressionPtr
    Implementation<Parser>::exp_prod()
    {
        ExpressionPtr lhs, rhs;

        if (! (lhs = exp_power()))
            return ExpressionPtr();

        while (matches_any_of("*/"))
        {
            char op(get_one());

            if (! (rhs = exp_power()))
                return ExpressionPtr();

            if ('*' == op)
                lhs = ExpressionPtr(new Product(lhs, rhs));
            else
                lhs = ExpressionPtr(new Quotient(lhs, rhs));
        }

        return lhs;
    }

    ExpressionPtr
    Implementation<Parser>::exp_power()
    {
        ExpressionPtr lhs, rhs;

        if (! (lhs = exp_elem()))
            return ExpressionPtr();

        while (matches('^'))
        {
            char op(get_one());

            if (! (rhs = exp_elem()))
                return ExpressionPtr();

            lhs = ExpressionPtr(new Power(lhs, rhs));
        }

        return lhs;
    }

    ExpressionPtr
    Implementation<Parser>::exp_elem()
    {
        ExpressionPtr result;
        std::string exp_id;
        NumberPtr exp_number;

        if ((exp_id = id()).empty()) // exp-var or exp-call
        {
            if (! match_and_discard('('))
            {
                result = ExpressionPtr(new Variable(exp_id));
            }
            else
            {
                Sequence<ExpressionPtr> call_params;
                ExpressionPtr param;

                if (! (param = exp()))
                {
                    error("Expected expression after '('");
                    result = ExpressionPtr(new Call(exp_id, call_params));
                }
                else
                {
                    call_params.append(param);

                    while (match_and_discard(','))
                    {
                        if (! (param = exp()))
                        {
                            error("Expected expression after ','");
                            return ExpressionPtr(new Call(exp_id, call_params));
                        }

                        call_params.append(param);
                    }

                    result = ExpressionPtr(new Call(exp_id, call_params));

                    if (! matches(')'))
                    {
                        error(pet_expected_closing_parenthesis, "?");
                    }
                }
            }
        }
        else if (match_and_discard('(')) // exp-paren
        {
            result = exp();

            if (! match_and_discard(')'))
            {
                error(pet_expected_closing_parenthesis, "?");
            }
        }
        else if (match_and_discard('{')) // exp-list
        {
            Sequence<ExpressionPtr> list_elements, next_elements;

            if ((next_elements = exp_list_elem()).empty())
            {
                error("Expected expression after '{'");
            }
            else
            {
                list_elements.append(next_elements);

                while (match_and_discard(','))
                {
                    if ((next_elements = exp_list_elem()).empty())
                    {
                        error("Expected expression after ','");
                        break;
                    }

                    list_elements.append(next_elements);
                }

                if (! match_and_discard('}'))
                {
                    error(pet_expected_closing_brace, "?");
                }

                result = ExpressionPtr(new List(list_elements));
            }
        }
        else if (! (exp_number = number())) // exp-value
        {
            result = ExpressionPtr(new Value(*exp_number));
        }
        else
        {
            error("Invalid input in place of expression element");
        }

        return result;
    }

    Sequence<ExpressionPtr>
    Implementation<Parser>::exp_list_elem()
    {
        Sequence<ExpressionPtr> result;
        ExpressionPtr element;

        if (! (element = exp()))
        {
            error("Expected expression");
            return result;
        }

        skip_irrelevant();

        unsigned repetitions(1);

        if (match_and_discard('x'))
        {
            NumberPtr n;

            if ((! (n = number())) || (! interpretable_as<unsigned>(*n)))
            {
                error("Expected unsigned integer after repetition operator 'x'");
                return result;
            }

            repetitions = interpret_number_as<unsigned>(*n);
        }

        for (unsigned i(0) ; i < repetitions ; ++i)
        {
            result.append(element);
        }
    }

    bool
    Implementation<Parser>::stmt_block()
    {
        if (! match_and_discard('{'))
            return false;

        if (! stmt())
        {
            error("Expected statement after '{'");
            return false;
        }

        while (! matches('}'))
        {
            if (! stmt())
            {
                error("Expected statement or '}'");
                return false; // TODO Continue on failure?
            }
        }

        if (! matches('}'))
        {
            error(pet_expected_closing_brace, "? ? ?"); // TODO
        }

        return true;
    }

    bool
    Implementation<Parser>::stmt()
    {
        bool result(true);

        if (matches('{'))
        {
            scopes.push_back(internal::Scope());

            if (! stmt_block());
            {
                error("Not a valid block of statements");
                result = false;
            }

            push_statement(StatementPtr(new Block(scopes.back().statements)));
            scopes.pop_back();
        }
        else if (matches("foreach"))
        {
            scopes.push_back(internal::Scope());

            if (! stmt_foreach())
            {
                error("Not a valid foreach statement");
                result = false;
            }

            scopes.pop_back();
        }
        else if (stmt_return())
        {
        }
        else if (stmt_decl())
        {
        }
        else if (stmt_assign())
        {
        }
        else
        {
            error("Expected statement");
        }

        return result;
    }

    bool
    Implementation<Parser>::stmt_decl()
    {
        bool result(true);

        TypePtr decl_type;
        if (! (decl_type = type()))
        {
            return false;
        }

        std::string decl_id;
        if ((decl_id = id()).empty())
        {
            error("Expected identifier after 'TODO: TYPE'");
            result = false;
        }

        if (! match_and_discard(';'))
        {
            error("Expected ';'");
            result = false;
        }

        push_statement(StatementPtr(new Declaration(decl_type->type, decl_id)));

        result = true;
    }
#if 0
    bool
    Implementation<Parser>::stmt_decl_list(const std::string & type)
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
#endif
    bool
    Implementation<Parser>::stmt_assign()
    {
        bool result(true);

        std::string lhs;
        if ((lhs = id()).empty())
        {
            error("Expected identifier");
            result = false;
        }

        skip_irrelevant();

        if (! matches('='))
        {
            error("Expected assignment-like operator after '" + lhs + "'");
        }

        skip_irrelevant();

        ExpressionPtr rhs;
        if (! (rhs = exp()))
        {
            error("Expected expression after '='");
            result = false;
        }

        if (! match_and_discard(";"))
        {
            error(pet_expected_semicolon, "");
            result = false;
        }

        push_statement(StatementPtr(new Assignment(lhs, rhs)));

        return result;
    }

    bool
    Implementation<Parser>::stmt_foreach()
    {
        if (! match_and_discard("foreach"))
            return false;

        if (! stmt_foreach_list())
            return false;

        if (! stmt_block())
            return false;

        return true;
    }

    bool
    Implementation<Parser>::stmt_foreach_list()
    {
        if (! match_and_discard('('))
            return false;

        if (! stmt_foreach_elem())
            return false;

        while (match_and_discard(','))
        {
            if (! stmt_foreach_elem())
                return false;
        }

        if (! match_and_discard(')'))
        {
            error(pet_expected_closing_parenthesis, "? ? ?");
        }

        return true;
    }

    bool
    Implementation<Parser>::stmt_foreach_elem()
    {
        std::string iterator;
        if ((iterator = id()).empty())
            return false;

        if (! match_and_discard("in"))
            return false;

        ExpressionPtr range_exp1, range_exp2;
        if (! (range_exp1 = exp()))
            return false;

        if (match_and_discard(".."))
        {
            if (! (range_exp2 = exp()))
                return false;
        }

        return true;
    }

    bool
    Implementation<Parser>::stmt_return()
    {
        if (! match_and_discard("return"))
            return false;

        ExpressionPtr expression;
        if (! (expression = exp()))
            return false;

        if (! match_and_discard(';'))
        {
            error(pet_expected_semicolon, "? ?");
        }

        push_statement(StatementPtr(new Return(expression)));

        return true;
    }

    Parser::Parser(std::istream & stream) :
        PrivateImplementationPattern<Parser>(new Implementation<Parser>(stream))
    {
    }

    Parser::~Parser()
    {
    }

    Sequence<FunctionPtr>
    Parser::result()
    {
        return _imp->functions;
    }

    Sequence<ParserError>
    Parser::errors()
    {
        return _imp->errors;
    }
}
