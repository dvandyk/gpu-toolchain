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

#include <r6xx/alu_source_operand.hh>
#include <r6xx/error.hh>
#include <utils/destringify.hh>
#include <utils/exception.hh>
#include <utils/enumeration.hh>
#include <utils/hexify.hh>
#include <utils/stringify.hh>
#include <utils/visitor-impl.hh>

namespace
{
    gpu::Enumeration<2> channel_from_suffix(const std::string & suffix)
    {
        try
        {
            switch (suffix[0])
            {
                case 'x':
                case 'y':
                case 'z':
                    return gpu::Enumeration<2>(suffix[0] - 'x');

                case 'w':
                    return gpu::Enumeration<2>(3);
            }
        }
        catch (gpu::EnumerationValueOutOfBoundsError & e)
        {
        }

        throw gpu::r6xx::SourceOperandSyntaxError("'" + gpu::stringify(suffix[0]) + "' is not a valid channel");
    }

    bool relative_from_suffix(const std::string & suffix)
    {
        bool result(false);

        try
        {
            if (2 == suffix.size())
                result = ('r' == suffix[1]);

            return result;
        }
        catch (gpu::EnumerationValueOutOfBoundsError & e)
        {
        }

        throw gpu::r6xx::SourceOperandSyntaxError("'" + gpu::stringify(suffix[1]) + "' is not a address mode");
    }
}

namespace gpu
{
    template <>
    ConstVisits<r6xx::alu::SourceGPR>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::alu::SourceKCache>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::alu::SourceCFile>::~ConstVisits()
    {
    }

    template <>
    ConstVisits<r6xx::alu::SourceLiteral>::~ConstVisits()
    {
    }

    namespace r6xx
    {
        namespace alu
        {
            SourceOperand::~SourceOperand()
            {
            }

            SourceGPR::SourceGPR(const Enumeration<2> & c, const Enumeration<7> & i, bool n, bool r) :
                channel(c),
                index(i),
                negated(n),
                relative(r)
            {
            }

            SourceGPR::~SourceGPR()
            {
            }

            void
            SourceGPR::accept(SourceOperandVisitor & v) const
            {
                static_cast<ConstVisits<SourceGPR> *>(&v)->visit(*this);
            }

            SourceKCache::SourceKCache(const Enumeration<2> & c, const Enumeration<6> & i, bool n, bool r) :
                channel(c),
                index(i),
                negated(n),
                relative(r)
            {
            }

            SourceKCache::~SourceKCache()
            {
            }

            void
            SourceKCache::accept(SourceOperandVisitor & v) const
            {
                static_cast<ConstVisits<SourceKCache> *>(&v)->visit(*this);
            }

            SourceCFile::SourceCFile(const Enumeration<2> & c, const Enumeration<8> & i, bool n, bool r) :
                channel(c),
                index(i),
                negated(n),
                relative(r)
            {
            }

            SourceCFile::~SourceCFile()
            {
            }

            void
            SourceCFile::accept(SourceOperandVisitor & v) const
            {
                static_cast<ConstVisits<SourceCFile> *>(&v)->visit(*this);
            }

            SourceLiteral::SourceLiteral(const Enumeration<32> & d) :
                data(d)
            {
            }

            SourceLiteral::~SourceLiteral()
            {
            }

            void
            SourceLiteral::accept(SourceOperandVisitor & v) const
            {
                static_cast<ConstVisits<SourceLiteral> *>(&v)->visit(*this);
            }

            SourceOperandPtr
            SourceOperandParser::parse(const std::string & input)
            {
                const static std::string digits("0123456789");
                const static std::string modes("ar");
                const static std::string prefixes("$KC");

                if (input.empty())
                    throw InternalError("r6xx", "empty input");

                SourceOperandPtr result;
                std::string operand(input);

                bool negate(false);
                if ('-' == operand[0])
                {
                    negate = true;
                    operand.erase(0, 1);
                }

                if (operand.empty())
                    throw SourceOperandSyntaxError("no main part");

                if (std::string::npos != prefixes.find(operand[0]))
                {
                    if (operand.empty())
                        throw SourceOperandSyntaxError("no main part");

                    char prefix(operand[0]);
                    operand.erase(0, 1);

                    if (operand.empty())
                        throw SourceOperandSyntaxError("no main part");

                    std::string::size_type sep(operand.find('.'));
                    if (std::string::npos == sep)
                        throw SourceOperandSyntaxError("no separator");

                    if (0 == sep)
                        throw SourceOperandSyntaxError("no index part");

                    std::string suffix(operand.substr(sep + 1));
                    operand.erase(sep);

                    if (std::string::npos != operand.find_first_not_of(digits))
                        throw SourceOperandSyntaxError("'" + operand + "' is not a number");

                    if ((suffix.empty()) || (2 < suffix.size()))
                        throw SourceOperandSyntaxError("invalid suffix '" + stringify(suffix) + "'");

                    Enumeration<2> channel(::channel_from_suffix(suffix));
                    bool relative(::relative_from_suffix(suffix));
                    unsigned index(destringify<unsigned>(operand));

                    try
                    {
                        if ('$' == prefix) // GPR
                        {
                            result = SourceOperandPtr(new SourceGPR(channel, Enumeration<7>(index), negate, relative));
                        }
                        else if ('K' == prefix) // KCache register
                        {
                            result = SourceOperandPtr(new SourceKCache(channel, Enumeration<6>(index), negate, relative));
                        }
                        else if ('C' == prefix) // Constant file register
                        {
                            result = SourceOperandPtr(new SourceCFile(channel, Enumeration<8>(index), negate, relative));
                        }

                        return result;
                    }
                    catch (EnumerationValueOutOfBoundsError & e)
                    {
                        throw SourceOperandSyntaxError("index out of bounds: " + stringify(index));
                    }
                }
                else // Constant or Literal
                {
                    Enumeration<32> data(0);

                    if (std::string::npos != input.find('.')) // Float literal
                    {
                        float value(destringify<float>(input));
                        data = Enumeration<32>(*reinterpret_cast<unsigned *>(&value));

                    }
                    else if ('u' == input[input.size() - 1]) // Unsigned integer
                    {
                        if (negate)
                            throw SourceOperandSyntaxError("negative sign in unsigned interger literal");

                        data = Enumeration<32>(destringify<unsigned>(input.substr(0, input.size() - 1)));
                    }
                    else // Signed integer
                    {
                        signed value(destringify<signed>(input));
                        data = Enumeration<32>(*reinterpret_cast<unsigned *>(&value));
                    }

                    return SourceOperandPtr(new SourceLiteral(data));
                }

                throw InternalError("r6xx", "Unhandled operand");
            }

            void
            SourceOperandPrinter::visit(const SourceGPR & gpr)
            {
                _output = "SourceGPR(" + stringify(gpr.channel)
                        + ", " + stringify(gpr.index) 
                        + ", " + stringify(gpr.negated)
                        + ", " + stringify(gpr.relative) + ")";
            }

            void
            SourceOperandPrinter::visit(const SourceKCache & kcache)
            {
                _output = "SourceKCache(" + stringify(kcache.channel)
                        + ", " + stringify(kcache.index)
                        + ", " + stringify(kcache.negated)
                        + ", " + stringify(kcache.relative) + ")";
            }

            void
            SourceOperandPrinter::visit(const SourceCFile & cfile)
            {
                _output = "SourceCFile(" + stringify(cfile.channel)
                        + ", " + stringify(cfile.index)
                        + ", " + stringify(cfile.negated)
                        + ", " + stringify(cfile.relative) + ")";
            }

            void
            SourceOperandPrinter::visit(const SourceLiteral & literal)
            {
                _output = "SourceLiteral(" + hexify(literal.data) + ")";
            }

            std::string
            SourceOperandPrinter::print(const SourceOperandPtr & ptr)
            {
                ptr->accept(*this);

                return _output;
            }
        }
    }
}
