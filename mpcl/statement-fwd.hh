/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Danny van Dyk <danny.dyk@tu-dortmund.de>
 * Copyright (c) 2009 Dirk Fischer <dirk.fischer@gmail.com>
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

#ifndef GPU_GUARD_MPCL_STATEMENT_FWD_HH
#define GPU_GUARD_MPCL_STATEMENT_FWD_HH 1

#include <utils/memory.hh>

namespace gpu
{
    struct Statement;

    typedef std::tr1::shared_ptr<Statement> StatementPtr;

    struct Assignment;

    struct Declaration;

    struct Return;
}

#endif
