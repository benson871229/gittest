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

#include "graph_filtering.hh"
#include "graph_python_interface.hh"

#include <boost/python.hpp>

#include "graph.hh"
#include "graph_selectors.hh"
#include "graph_util.hh"

namespace graph_tool
{

using namespace std;
using namespace boost;

class AStarVisitorWrapper
{
public:
    AStarVisitorWrapper(GraphInterface& gi, python::object vis)
        : _gi(gi), _vis(vis) {}

    template <class Vertex, class Graph>
    void initialize_vertex(Vertex u, Graph& g)
    {
        auto gp = retrieve_graph_view<Graph>(_gi, g);
        _vis.attr("initialize_vertex")(PythonVertex<Graph>(gp, u));
    }

    template <class Vertex, class Graph>
    void discover_vertex(Vertex u, Graph& g)
    {
        auto gp = retrieve_graph_view<Graph>(_gi, g);
        _vis.attr("discover_vertex")(PythonVertex<Graph>(gp, u));
    }

    template <class Vertex, class Graph>
    void examine_vertex(Vertex u, Graph& g)
    {
        auto gp = retrieve_graph_view<Graph>(_gi, g);
        _vis.attr("examine_vertex")(PythonVertex<Graph>(gp, u));
    }

    template <class Edge, class Graph>
    void examine_edge(Edge e, Graph& g)
    {
        auto gp = retrieve_graph_view<Graph>(_gi, g);
        _vis.attr("examine_edge") (PythonEdge<Graph>(gp, e));
    }

    template <class Edge, class Graph>
    void edge_relaxed(Edge e, Graph& g)
    {
        auto gp = retrieve_graph_view<Graph>(_gi, g);
        _vis.attr("edge_relaxed") (PythonEdge<Graph>(gp, e));
    }

    template <class Edge, class Graph>
    void edge_not_relaxed(Edge e, Graph& g)
    {
        auto gp = retrieve_graph_view<Graph>(_gi, g);
        _vis.attr("edge_not_relaxed") (PythonEdge<Graph>(gp, e));
    }

    template <class Edge, class Graph>
    void black_target(Edge e, Graph& g)
    {
        auto gp = retrieve_graph_view<Graph>(_gi, g);
        _vis.attr("black_target") (PythonEdge<Graph>(gp, e));
    }

    template <class Vertex, class Graph>
    void finish_vertex(Vertex u, Graph& g)
    {
        auto gp = retrieve_graph_view<Graph>(_gi, g);
        _vis.attr("finish_vertex")(PythonVertex<Graph>(gp, u));
    }

private:
    GraphInterface& _gi;
    python::object _vis;
};


class AStarCmp
{
public:
    AStarCmp() {}
    AStarCmp(python::object cmp): _cmp(cmp) {}

    template <class Value1, class Value2>
    bool operator()(const Value1& v1, const Value2& v2) const
    {
        return python::extract<bool>(_cmp(v1, v2));
    }

private:
    python::object _cmp;
};

class AStarCmb
{
public:
    AStarCmb() {}
    AStarCmb(python::object cmb): _cmb(cmb) {}

    template <class Value1, class Value2 >
    Value1 operator()(const Value1& v1, const Value2& v2) const
    {
        return python::extract<Value1>(_cmb(v1, v2));
    }

private:
    python::object _cmb;
};

template <class Graph, class Value>
class AStarH
{
public:
    AStarH() {}
    AStarH(GraphInterface& gi, Graph& g, python::object h)
        : _h(h), _gp(retrieve_graph_view<Graph>(gi, g)) {}

    Value operator()(GraphInterface::vertex_t v) const
    {
        return python::extract<Value>(_h(PythonVertex<Graph>(_gp, v)));
    }

private:
    python::object _h;
    std::shared_ptr<Graph> _gp;
};

} // namespace graph_tool
