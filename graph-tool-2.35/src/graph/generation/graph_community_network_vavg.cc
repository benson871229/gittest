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

#include "graph_python_interface.hh"
#include "graph_filtering.hh"
#include "graph.hh"
#include "graph_selectors.hh"
#include "graph_properties.hh"

#include <boost/mpl/push_back.hpp>
#include <boost/python.hpp>

#include "graph_community_network.hh"

using namespace std;
using namespace boost;

using namespace graph_tool;

typedef UnityPropertyMap<int,GraphInterface::vertex_t> no_vweight_map_t;
typedef vprop_map_t<int32_t>::type vcount_map_t;


struct get_weighted_vertex_property_dispatch
{
    template <class Graph, class VertexWeightMap, class Vprop>
    void operator()(const Graph& g, VertexWeightMap vweight, Vprop vprop,
                    boost::any atemp) const
    {
        typename Vprop::checked_t temp = boost::any_cast<typename Vprop::checked_t>(atemp);
        get_weighted_vertex_property()(g, vweight, vprop, temp.get_unchecked(num_vertices(g)));
    }
};


struct get_vertex_sum_dispatch
{
    template <class Graph, class CommunityGraph, class CommunityMap, class Vprop>
    void operator()(const Graph& g, CommunityGraph& cg, CommunityMap s_map,
                    boost::any acs_map, Vprop vprop, boost::any acvprop) const
    {
        typename CommunityMap::checked_t cs_map = boost::any_cast<typename CommunityMap::checked_t>(acs_map);
        typename Vprop::checked_t cvprop = boost::any_cast<typename Vprop::checked_t>(acvprop);
        get_vertex_community_property_sum()(g, cg, s_map,
                                            cs_map.get_unchecked(num_vertices(cg)), vprop,
                                            cvprop.get_unchecked(num_vertices(cg)));
    }
};


void community_network_vavg(GraphInterface& gi, GraphInterface& cgi,
                            boost::any community_property,
                            boost::any condensed_community_property,
                            boost::any vweight,
                            boost::python::list avprops)
{
    typedef boost::mpl::push_back<writable_vertex_scalar_properties, no_vweight_map_t>::type
        vweight_properties;

    bool no_weight = false;
    if (vweight.empty())
    {
        no_weight = true;
        vweight = no_vweight_map_t();
    }

    typedef boost::mpl::insert_range<writable_vertex_scalar_properties,
                                     boost::mpl::end<writable_vertex_scalar_properties>::type,
                                     vertex_scalar_vector_properties>::type vprops_temp;
    typedef boost::mpl::push_back<vprops_temp,
                                  vprop_map_t<boost::python::object>::type >::type
        vprops_t;

    for(int i = 0; i < boost::python::len(avprops); ++i)
    {
        boost::any vprop = boost::python::extract<any>(avprops[i][0])();
        boost::any temp = boost::python::extract<any>(avprops[i][1])();
        boost::any cvprop = boost::python::extract<any>(avprops[i][2])();

        if (!no_weight)
        {
            // compute weighted values to temp
            run_action<>()
                (gi,
                 [&](auto&& graph, auto&& a2, auto&& a3)
                 {
                     return get_weighted_vertex_property_dispatch()
                         (std::forward<decltype(graph)>(graph),
                          std::forward<decltype(a2)>(a2),
                          std::forward<decltype(a3)>(a3), temp);
                 },
                 vweight_properties(), vprops_t())(vweight, vprop);

            // sum weighted values
            run_action<>()
                (gi,
                 [&](auto&& graph, auto&& a2, auto&& a3)
                 {
                     return get_vertex_sum_dispatch()
                         (std::forward<decltype(graph)>(graph), cgi.get_graph(),
                          std::forward<decltype(a2)>(a2),
                          condensed_community_property,
                          std::forward<decltype(a3)>(a3), cvprop);
                 },
                 writable_vertex_properties(),
                 vprops_t())(community_property, temp);
        }
        else
        {
            // sum unweighted values
            run_action<>()
                (gi,
                 [&](auto&& graph, auto&& a2, auto&& a3)
                 {
                     return get_vertex_sum_dispatch()
                         (std::forward<decltype(graph)>(graph), cgi.get_graph(),
                          std::forward<decltype(a2)>(a2),
                          condensed_community_property,
                          std::forward<decltype(a3)>(a3), cvprop);
                 },
                 writable_vertex_properties(),
                 vprops_t())(community_property, vprop);
        }
    }
}
