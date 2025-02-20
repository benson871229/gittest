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

#include "graph.hh"
#include "graph_filtering.hh"
#include "graph_properties.hh"

#include "graph_parallel.hh"

#include <boost/python.hpp>

#include <iostream>

using namespace std;
using namespace boost;
using namespace graph_tool;

void do_label_parallel_edges(GraphInterface& gi, boost::any property,
                             bool mark_only)
{
    run_action<>()
        (gi,
         [&](auto&& graph, auto&& a2)
         {
             return label_parallel_edges
                 (std::forward<decltype(graph)>(graph),
                  std::forward<decltype(a2)>(a2), mark_only);
         },
         writable_edge_scalar_properties())(property);
}

void do_label_self_loops(GraphInterface& gi, boost::any property,
                         bool mark_only)
{
    run_action<>()
        (gi,
         [&](auto&& graph, auto&& a2)
         {
             return label_self_loops
                 (std::forward<decltype(graph)>(graph),
                  std::forward<decltype(a2)>(a2), mark_only);
         },
         writable_edge_scalar_properties())(property);
}

void do_remove_labeled_edges(GraphInterface& gi, boost::any property)
{
    run_action<graph_tool::detail::always_directed_never_reversed, mpl::true_>()
        (gi,
         [&](auto&& graph, auto&& a2)
         {
             return remove_labeled_edges
                 (std::forward<decltype(graph)>(graph),
                  std::forward<decltype(a2)>(a2));
         },
         edge_scalar_properties())(property);
}

using namespace boost::python;

void export_parallel()
{
    def("label_parallel_edges", &do_label_parallel_edges);
    def("label_self_loops", &do_label_self_loops);
    def("remove_labeled_edges", &do_remove_labeled_edges);
}
