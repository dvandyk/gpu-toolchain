/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include <mpcl/function.hh>
#include <utils/sequence-impl.hh>

namespace gpu
{
    template class Sequence<FunctionPtr>;

    template class WrappedForwardIterator<Sequence<FunctionPtr>::IteratorTag, FunctionPtr>;
}
