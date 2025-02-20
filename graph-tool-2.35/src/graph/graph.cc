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

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

using namespace std;
using namespace boost;
using namespace graph_tool;


// this is the constructor for the graph interface
GraphInterface::GraphInterface()
    :_mg(std::make_shared<multigraph_t>()),
     _vertex_index(get(vertex_index, *_mg)),
     _edge_index(get(edge_index_t(), *_mg)),
     _reversed(false),
     _directed(true),
     _graph_index(0),
     _vertex_filter_map(_vertex_index),
     _vertex_filter_invert(false),
     _vertex_filter_active(false),
     _edge_filter_map(_edge_index),
     _edge_filter_invert(false),
     _edge_filter_active(false)
{
}

// the destructor
GraphInterface::~GraphInterface()
{
}

// this will get the number of vertices, either the "soft" O(1) way, or the hard
// O(V) way, which is necessary if the graph is filtered
size_t GraphInterface::get_num_vertices(bool filtered)
{
    size_t n = 0;
    if (filtered && is_vertex_filter_active())
        run_action<>()(*this, lambda::var(n) =
                       lambda::bind<size_t>(HardNumVertices(),lambda::_1))();
    else
        n = num_vertices(*_mg);
    return n;
}

// this will get the number of edges, either the "soft" O(E) way, or the hard
// O(E) way, which is necessary if the graph is filtered. Both cases are of
// linear complexity, since num_edges() is O(E) in Boost's adjacency_list
size_t GraphInterface::get_num_edges(bool filtered)
{
    using namespace boost::lambda;
    size_t n = 0;
    if (filtered && (is_edge_filter_active() || is_vertex_filter_active()))
        run_action<>()(*this, lambda::var(n) =
                       lambda::bind<size_t>(HardNumEdges(),lambda::_1))();
    else
        n = num_edges(*_mg);
    return n;
}

struct clear_vertices
{
    template <class Graph>
    void operator()(Graph& g) const
    {
        int N = num_vertices(g);
        for (int i = N - 1; i >= 0; --i)
        {
            auto v = vertex(i, g);
            if (!is_valid_vertex(v, g))
                continue;
            remove_vertex(v, g);
        }
    }
};

void GraphInterface::clear()
{
    run_action<>()
        (*this,
         [&](auto&& graph)
         {
             return clear_vertices()
                 (std::forward<decltype(graph)>(graph));
         })();
}

struct do_clear_edges
{
    template <class Graph>
    void operator()(Graph& g) const
    {
        for (auto v : vertices_range(g))
            clear_vertex(v, g);
    }
};

void GraphInterface::clear_edges()
{
    run_action<>()
        (*this,
         [&](auto&& graph)
         {
             return do_clear_edges()
                 (std::forward<decltype(graph)>(graph));
         })();
}
