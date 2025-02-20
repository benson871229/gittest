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
#include "graph_selectors.hh"

#include "graph_components.hh"

#include "numpy_bind.hh"

#include <boost/python.hpp>

using namespace std;
using namespace boost;
using namespace graph_tool;

python::object do_label_components(GraphInterface& gi, boost::any prop)
{
    vector<size_t> hist;
    run_action<graph_tool::all_graph_views, mpl::true_>()
        (gi,
         [&](auto&& graph, auto&& a2)
         {
             return label_components()
                 (std::forward<decltype(graph)>(graph),
                  std::forward<decltype(a2)>(a2), hist);
         },
         writable_vertex_scalar_properties())(prop);
    return wrap_vector_owned(hist);
}

python::object
do_label_biconnected_components(GraphInterface& gi, boost::any comp,
                                boost::any art)
{
    vector<size_t> hist;
    run_action<graph_tool::detail::never_directed>()
        (gi,
         [&](auto&& graph, auto&& a2, auto&& a3)
         {
             return label_biconnected_components()
                 (std::forward<decltype(graph)>(graph),
                  std::forward<decltype(a2)>(a2),
                  std::forward<decltype(a3)>(a3), hist);
         },
         writable_edge_scalar_properties(),
         writable_vertex_scalar_properties())(comp, art);
    return wrap_vector_owned(hist);
}

void do_label_out_component(GraphInterface& gi, size_t root, boost::any prop)
{
    run_action<graph_tool::all_graph_views, mpl::true_>()
        (gi,
         [&](auto&& graph, auto&& a2)
         {
             return label_out_component()
                 (std::forward<decltype(graph)>(graph),
                  std::forward<decltype(a2)>(a2), root);
         },
         writable_vertex_scalar_properties())(prop);
}

void do_label_attractors(GraphInterface& gi, boost::any cprop, python::object oavec)
{

    multi_array_ref<bool,1> avec = get_array<bool,1>(oavec);

    run_action<>()
        (gi,
         [&](auto&& graph, auto&& a2)
         {
             return label_attractors()
                 (std::forward<decltype(graph)>(graph),
                  std::forward<decltype(a2)>(a2), avec);
         },
         vertex_scalar_properties())(cprop);
}

void export_components()
{
    python::def("label_components", &do_label_components);
    python::def("label_biconnected_components",
                &do_label_biconnected_components);
    python::def("label_out_component", &do_label_out_component);
    python::def("label_attractors", &do_label_attractors);
};
