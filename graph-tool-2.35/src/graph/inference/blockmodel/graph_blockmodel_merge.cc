// graph-tool -- a general graph modification and manipulation thingy
//
// Copyright (C) 2006-2020 Tiago de Paula Peixoto <tiago@skewed.de>
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "graph_tool.hh"
#include "random.hh"

#include <boost/python.hpp>

#include "graph_blockmodel_util.hh"
#include "graph_blockmodel.hh"
#include "graph_blockmodel_merge.hh"
#include "../loops/merge_loop.hh"

using namespace boost;
using namespace graph_tool;

GEN_DISPATCH(block_state, BlockState, BLOCK_STATE_params)

template <class State>
GEN_DISPATCH(merge_block_state, Merge<State>::template MergeBlockState,
             MERGE_BLOCK_STATE_params(State))

python::object do_merge_sweep(python::object omerge_state,
                              python::object oblock_state,
                              rng_t& rng)
{
    python::object ret;
    auto dispatch = [&](auto& block_state)
    {
        typedef typename std::remove_reference<decltype(block_state)>::type
            state_t;

        merge_block_state<state_t>::make_dispatch
           (omerge_state,
            [&](auto& s)
            {
                auto ret_ = merge_sweep(s, rng);
                ret = tuple_apply([&](auto&... args){ return python::make_tuple(args...); }, ret_);
            });
    };
    block_state::dispatch(oblock_state, dispatch);
    return ret;
}

void export_blockmodel_merge()
{
    using namespace boost::python;
    def("merge_sweep", &do_merge_sweep);
}
