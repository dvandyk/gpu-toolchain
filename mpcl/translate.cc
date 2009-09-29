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

#include <mpcl/translate.hh>
#include <utils/sequence-impl.hh>
#include <utils/wrapped_forward_iterator-impl.hh>

#include <map>
#include <ostream>

namespace gpu
{
    PILOperand::PILOperand(PILOperandType type, const Number & value) :
        type(type),
        value(value)
    {
    }

    bool
    PILOperand::operator!= (const PILOperand & other)
    {
        if (this->type != other.type)
            return true;

        if (this->value != other.value)
            return true;

        return false;
    }

    PILOperand
    PILOperand::Constant(const Number & value)
    {
        return PILOperand(pot_constant, value);
    }

    PILOperand
    PILOperand::Symbol(unsigned value)
    {
        return PILOperand(pot_symbol, Number(value));
    }

    PILOperand
    PILOperand::Temporary(unsigned value)
    {
        return PILOperand(pot_temporary, Number(value));
    }

    PILOperand
    PILOperand::Undefined()
    {
        return PILOperand(pot_undefined, Number(-1));
    }

    std::ostream & operator<< (std::ostream & lhs, const PILOperand & rhs)
    {
        switch (rhs.type)
        {
            case pot_constant:
                lhs << "c[" << rhs.value << "]";
                break;

            case pot_symbol:
                lhs << "@[" << rhs.value << "]";
                break;

            case pot_temporary:
                lhs << "t[" << rhs.value << "]";
                break;

            default:
                lhs << "undefined";
        }

        return lhs;
    }

    PILOperation::PILOperation(const PILOperand & result, PILOpcode opcode,
            const PILOperand & a, const PILOperand & b, bool binary) :
        opcode(opcode),
        result(result),
        a(a),
        b(b),
        binary(binary)
    {
    }

    bool
    PILOperation::operator!= (const PILOperation & other)
    {
        if (this->binary != other.binary)
            return true;

        if (this->opcode != other.opcode)
            return true;

        if (this->result != other.result)
            return true;

        if (this->a != other.a)
            return true;

        if (this->b != other.b)
            return true;

        return false;
    }

    PILOperationPtr
    PILOperation::Binary(const PILOperand & result, PILOpcode opcode,
            const PILOperand & a, const PILOperand & b)
    {
        return PILOperationPtr(new PILOperation(result, opcode, a, b, true));
    }

    PILOperationPtr
    PILOperation::Unary(const PILOperand & result, PILOpcode opcode,
            const PILOperand & a)
    {
        return PILOperationPtr(new PILOperation(result, opcode, a, PILOperand::Undefined(), false));
    }

    std::ostream & operator<< (std::ostream & lhs, const PILOperation & rhs)
    {
        static const char arithmetic_operators[] = { '+', '-', '*', '/' };

        lhs << rhs.result << " <- ";

        switch (rhs.opcode)
        {
            case pop_assign:
                lhs << rhs.a;
                break;

            case pop_iload:
                lhs << "iload(" << rhs.a << ")";
                break;

            case pop_add:
            case pop_div:
            case pop_mul:
            case pop_sub:
                lhs << rhs.a << ' ' << arithmetic_operators[rhs.opcode - pop_add] << ' ' << rhs.b;
                break;

            case pop_get_param:
                lhs << "get_param(" << rhs.a << ")";
                break;

            case pop_set_param:
                lhs << "set_param(" << rhs.a << ", " << rhs.b << ")";

            case pop_return:
                lhs << "return_value(" << rhs.a << ")";
                break;

            default:
                lhs << "unknown(" << rhs.a << ", " << rhs.b << ")";
        }

        return lhs;
    }

    template class Sequence<PILOperationPtr>;

    template class WrappedForwardIterator<Sequence<PILOperationPtr>::IteratorTag, PILOperationPtr>;

    template <> struct Implementation<Translator> :
        public ExpressionVisitor,
        public StatementVisitor
    {
        unsigned parameters;

        unsigned counter;

        std::map<std::string, unsigned> mapping;

        Sequence<PILOperationPtr> operations;

        Implementation() :
            parameters(0),
            counter(1)
        {
        }

        void visit(Call & c)
        {
            Sequence<ExpressionPtr> params(c.parameters());

            // TODO: Insert symbol into symbol-name to symbol-idx map or find symbol-idx if already used.
            unsigned symbol_idx(-1), param_idx(0);
            for (Sequence<ExpressionPtr>::Iterator i(params.begin()), i_end(params.end()) ;
                    i != i_end ; ++i, ++param_idx)
            {
                (*i)->accept(*this);
                operations.append(PILOperation::Binary(
                            PILOperand::Undefined(),
                            pop_set_param,
                            PILOperand::Constant(Number(param_idx)),
                            PILOperand::Temporary(counter - 1)));
            }

            operations.append(PILOperation::Unary(
                        PILOperand::Temporary(counter),
                        pop_call,
                        PILOperand::Symbol(symbol_idx)));

            ++counter;
        }

        void visit(Difference & d)
        {
            unsigned lhs;

            d.left_hand_side()->accept(*this);
            lhs = counter - 1;
            d.right_hand_side()->accept(*this);

            operations.append(PILOperation::Binary(
                        PILOperand::Temporary(counter),
                        pop_sub,
                        PILOperand::Temporary(lhs),
                        PILOperand::Temporary(counter - 1)));

            ++counter;
        }

        void visit(Product & p)
        {
            unsigned lhs;

            p.left_hand_side()->accept(*this);
            lhs = counter - 1;
            p.right_hand_side()->accept(*this);
            operations.append(PILOperation::Binary(
                        PILOperand::Temporary(counter),
                        pop_mul,
                        PILOperand::Temporary(lhs),
                        PILOperand::Temporary(counter - 1)));
            ++counter;
        }

        void visit(Quotient & q)
        {
            unsigned lhs;

            q.left_hand_side()->accept(*this);
            lhs = counter - 1;
            q.right_hand_side()->accept(*this);
            operations.append(PILOperation::Binary(
                        PILOperand::Temporary(counter),
                        pop_div,
                        PILOperand::Temporary(lhs),
                        PILOperand::Temporary(counter - 1)));

            ++counter;
        }

        void visit(Sum & s)
        {
            unsigned lhs;

            s.left_hand_side()->accept(*this);
            lhs = counter - 1;
            s.right_hand_side()->accept(*this);
            operations.append(PILOperation::Binary(
                        PILOperand::Temporary(counter),
                        pop_add,
                        PILOperand::Temporary(lhs),
                        PILOperand::Temporary(counter - 1)));

            ++counter;
        }

        void visit(Value & v)
        {
            operations.append(PILOperation::Unary(
                        PILOperand::Temporary(counter),
                        pop_iload,
                        PILOperand::Constant(v.value())));

            ++counter;
        }

        void visit(Variable & v)
        {
            std::map<std::string, unsigned>::const_iterator i(mapping.find(v.variable()));
            if (mapping.end() == i)
                throw std::string("Whaa");
                //std::cout << "Whaa! '" << v.variable() << "' is undeclared" << std::endl;

            operations.append(PILOperation::Unary(
                        PILOperand::Temporary(counter),
                        pop_assign,
                        PILOperand::Temporary(i->second)));

            ++counter;
        }

        void visit(const Assignment & a)
        {
            a.rhs->accept(*this);
            operations.append(PILOperation::Unary(
                        PILOperand::Temporary(counter),
                        pop_assign,
                        PILOperand::Temporary(counter - 1)));

            mapping[a.lhs] = counter;

            ++counter;
        }

        void visit(const Declaration & d)
        {
            if (d.exp)
            {
                d.exp->accept(*this);
                operations.append(PILOperation::Unary(
                            PILOperand::Temporary(counter),
                            pop_assign,
                            PILOperand::Temporary(counter - 1)));
            }
            else
            {
                operations.append(PILOperation::Unary(
                            PILOperand::Temporary(counter),
                            pop_assign,
                            PILOperand::Undefined()));
            }

            mapping[d.name] = counter;

            ++counter;
        }

        void visit(const Return & r)
        {
            r.exp->accept(*this);
            operations.append(PILOperation::Unary(
                            PILOperand::Temporary(counter),
                            pop_return,
                            PILOperand::Temporary(counter - 1)));
            mapping["__result__"] = counter;
            ++counter;
        }

        void visit(const Parameter & p)
        {
            ++parameters;

            operations.append(PILOperation::Unary(
                        PILOperand::Temporary(counter),
                        pop_get_param,
                        PILOperand::Constant(Number(parameters))));

            mapping[p.name] = counter;

            ++counter;
        }
    };

    Translator::Translator() :
        PrivateImplementationPattern<Translator>(new Implementation<Translator>)
    {
    }

    Translator::~Translator()
    {
    }

    Sequence<PILOperationPtr>
    Translator::translate(const FunctionPtr & f)
    {
        for (Sequence<ParameterPtr>::Iterator p(f->parameters.begin()),
                p_end(f->parameters.end()) ; p != p_end ; ++p)
        {
            _imp->visit(*(*p));
        }

        for (Sequence<StatementPtr>::Iterator s(f->statements.begin()),
                s_end(f->statements.end()) ; s != s_end ; ++s)
        {
            (*s)->accept(*_imp);
        }

        return _imp->operations;
    }
}
