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

#include "graph_blockmodel_overlap_util.hh"
#include "graph_blockmodel_overlap.hh"
#include "../blockmodel/graph_blockmodel_multiflip_mcmc.hh"
#include "../blockmodel/graph_blockmodel_multicanonical.hh"
#include "../loops/mcmc_loop.hh"

using namespace boost;
using namespace graph_tool;

GEN_DISPATCH(overlap_block_state, OverlapBlockState, OVERLAP_BLOCK_STATE_params)

template <class State>
GEN_DISPATCH(mcmc_block_state, MCMC<State>::template MCMCBlockState,
             MCMC_BLOCK_STATE_params(State))

template <class State>
GEN_DISPATCH(multicanonical_block_state,
             Multicanonical<State>::template MulticanonicalBlockState,
             MULTICANONICAL_BLOCK_STATE_params(State))

python::object multicanonical_overlap_multiflip_sweep(python::object omulticanonical_state,
                                                      python::object oblock_state,
                                                      rng_t& rng)
{
    python::object ret;
    auto dispatch = [&](auto& block_state)
    {
        typedef typename std::remove_reference<decltype(block_state)>::type
            state_t;

        mcmc_block_state<state_t>::make_dispatch
            (omulticanonical_state,
                 [&](auto& mcmc_state)
                 {
                     typedef typename std::remove_reference<decltype(mcmc_state)>::type
                         mcmc_state_t;

                     omulticanonical_state.attr("state") = boost::any(mcmc_state);

                     multicanonical_block_state<mcmc_state_t>::make_dispatch
                         (omulticanonical_state,
                          [&](auto& mc_state)
                          {
                              auto ret_ = mcmc_sweep(mc_state, rng);
                              ret = tuple_apply([&](auto&... args){ return python::make_tuple(args...); }, ret_);
                          });
                 });
    };
    overlap_block_state::dispatch(oblock_state, dispatch);
    return ret;
}

void export_overlap_blockmodel_multicanonical_multiflip()
{
    using namespace boost::python;
    def("multicanonical_overlap_multiflip_sweep",
        &multicanonical_overlap_multiflip_sweep);
}
