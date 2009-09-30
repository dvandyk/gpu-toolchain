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

#ifndef MPCL_GUARD_TRANSLATE_HH
#define MPCL_GUARD_TRANSLATE_HH 1

#include <common/expression.hh>
#include <mpcl/function.hh>
#include <mpcl/statement.hh>
#include <utils/number.hh>
#include <utils/sequence.hh>
#include <utils/wrapped_forward_iterator.hh>

namespace gpu
{
    enum PILOperandType
    {
        pot_undefined = 0,
        pot_temporary,
        pot_constant,
        pot_symbol
    };

    class PILOperand
    {
        public:
            PILOperandType type;

            Number value;

        private:
            PILOperand(PILOperandType type, const Number & value);

        public:
            bool operator!= (const PILOperand &);

            static PILOperand Constant(const Number & value);

            static PILOperand Symbol(unsigned value);

            static PILOperand Temporary(unsigned value);

            static PILOperand Undefined();
    };

    std::ostream & operator<< (std::ostream & lhs, const PILOperand & rhs);

    enum PILOpcode
    {
        pop_nop = 0,

        /* arithmetic */
        pop_add,
        pop_sub,
        pop_mul,
        pop_div,
        pop_pow,

        /* immediate */
        pop_iload,

        /* abstract */
        pop_assign,
        pop_call,
        pop_get_param,
        pop_set_param,
        pop_return
    };

    class PILOperation;

    typedef std::tr1::shared_ptr<PILOperation> PILOperationPtr;

    class PILOperation
    {
        public:
            PILOpcode opcode;

            PILOperand result;

            PILOperand a;

            PILOperand b;

            const bool binary;

        private:
            PILOperation(const PILOperand & result, PILOpcode opcode,
                    const PILOperand & a, const PILOperand & b, bool binary);

        public:
            bool operator!= (const PILOperation &);

            static PILOperationPtr Binary(const PILOperand & result, PILOpcode opcode,
                    const PILOperand & a, const PILOperand & b);

            static PILOperationPtr Unary(const PILOperand & result, PILOpcode opcode,
                    const PILOperand & a);
    };

    std::ostream & operator<< (std::ostream & lhs, const PILOperation & rhs);

    extern template class Sequence<PILOperationPtr>;

    extern template class WrappedForwardIterator<Sequence<PILOperationPtr>::IteratorTag, PILOperationPtr>;

    class Translator :
        public PrivateImplementationPattern<Translator>
    {
        public:
            Translator();

            ~Translator();

            Sequence<PILOperationPtr> translate(const FunctionPtr & f);
    };
}

#endif
