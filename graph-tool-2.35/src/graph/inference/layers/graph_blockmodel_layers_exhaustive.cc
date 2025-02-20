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

#define GRAPH_BLOCKMODEL_RMAP_ENABLE
#include "../blockmodel/graph_blockmodel_util.hh"
#include "../blockmodel/graph_blockmodel.hh"
#define BASE_STATE_params BLOCK_STATE_params
#include "graph_blockmodel_layers.hh"
#include "../blockmodel/graph_blockmodel_exhaustive.hh"
#include "../loops/exhaustive_loop.hh"

#include "numpy_bind.hh"
#include "coroutine.hh"
#include "graph_python_interface.hh"

using namespace boost;
using namespace graph_tool;

GEN_DISPATCH(block_state, BlockState, BLOCK_STATE_params)

template <class BaseState>
GEN_DISPATCH(layered_block_state, Layers<BaseState>::template LayeredBlockState,
             LAYERED_BLOCK_STATE_params)

template <class State>
GEN_DISPATCH(exhaustive_layered_block_state,
             Exhaustive<State>::template ExhaustiveBlockState,
             EXHAUSTIVE_BLOCK_STATE_params(State))

void do_exhaustive_layered_sweep(python::object oexhaustive_state,
                                 python::object oblock_state,
                                 python::object callback)
{
#ifdef GRAPH_BLOCKMODEL_LAYERS_ENABLE

    auto dispatch = [&](auto* block_state)
    {
        typedef typename std::remove_pointer<decltype(block_state)>::type
            state_t;

        layered_block_state<state_t>::dispatch
            (oblock_state,
             [&](auto& ls)
             {
                 typedef typename std::remove_reference<decltype(ls)>::type
                     layered_state_t;

                 exhaustive_layered_block_state<layered_state_t>::make_dispatch
                     (oexhaustive_state,
                      [&](auto& s)
                      {
                          exhaustive_sweep(s,
                                 [&](auto& state)
                                 {
                                     callback(state._S,
                                              state._S_min);
                                 });
                      });
             });
    };
    block_state::dispatch(dispatch);
#endif
}


python::object do_exhaustive_layered_sweep_iter(python::object oexhaustive_state,
                                                python::object oblock_state)
{
#ifdef HAVE_BOOST_COROUTINE
#ifdef GRAPH_BLOCKMODEL_LAYERS_ENABLE
    auto coro_dispatch = [&](auto& yield)
        {
            auto dispatch = [&](auto* block_state)
            {
                typedef typename std::remove_pointer<decltype(block_state)>::type
                    state_t;

                layered_block_state<state_t>::dispatch
                (oblock_state,
                 [&](auto& ls)
                 {
                     typedef typename std::remove_reference<decltype(ls)>::type
                         layered_state_t;
                     exhaustive_layered_block_state<layered_state_t>::make_dispatch
                         (oexhaustive_state,
                          [&](auto& s)
                          {
                              exhaustive_sweep
                                  (s,
                                   [&](auto& state)
                                   {
                                       yield(python::make_tuple(state._S,
                                                                state._S_min));
                                   });
                          });
                 });
            };
            block_state::dispatch(dispatch);
        };
    return python::object(CoroGenerator(coro_dispatch));
#endif
#else
    throw GraphException("This functionality is not available because boost::coroutine was not found at compile-time");
#endif // HAVE_BOOST_COROUTINE
}

void do_exhaustive_layered_dens(python::object oexhaustive_state,
                                python::object oblock_state,
                                double S_min, double S_max,
                                python::object ohist)
{
#ifdef GRAPH_BLOCKMODEL_LAYERS_ENABLE
    multi_array_ref<uint64_t, 1> hist = get_array<uint64_t, 1>(ohist);
    int N = hist.shape()[0];
    double dS = S_max - S_min;
    auto dispatch = [&](auto* block_state)
    {
        typedef typename std::remove_pointer<decltype(block_state)>::type
            state_t;

        layered_block_state<state_t>::dispatch
            (oblock_state,
             [&](auto& ls)
             {
                 typedef typename std::remove_reference<decltype(ls)>::type
                     layered_state_t;
                 exhaustive_layered_block_state<layered_state_t>::make_dispatch
                     (oexhaustive_state,
                      [&](auto& s)
                      {
                          exhaustive_sweep(s,
                                           [&](auto& state)
                                           {
                                               auto S = state._S;
                                               int i = round((N - 1) * (S - S_min) / dS);
                                               if (i >= 0 && i < N)
                                                   hist[i]++;
                                           });
                      });
             });
    };
    block_state::dispatch(dispatch);
#endif
}


void export_layered_blockmodel_exhaustive()
{
    using namespace boost::python;
    def("exhaustive_layered_sweep", &do_exhaustive_layered_sweep);
    def("exhaustive_layered_sweep_iter", &do_exhaustive_layered_sweep_iter);
    def("exhaustive_layered_dens", &do_exhaustive_layered_dens);
}
