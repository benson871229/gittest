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

// based on code written by Alexandre Hannud Abdo <abdo@member.fsf.org>

#include "graph_filtering.hh"
#include "graph_selectors.hh"
#include "graph_properties.hh"

#include "graph_extended_clustering.hh"

#include <boost/python.hpp>

using namespace std;
using namespace boost;
using namespace graph_tool;

template <class PropertySequence>
struct prop_vector
{
    boost::any operator()(const vector<boost::any>& props, size_t size) const
    {
        boost::any prop_vec;
        boost::mpl::for_each<PropertySequence>(
            [&](auto&& graph)
            {
                return get_prop_vector()
                    (std::forward<decltype(graph)>(graph), props, prop_vec,
                     size);
            });
        return prop_vec;
    }

    struct get_prop_vector
    {
        template <class Property>
        void operator()(Property, const vector<boost::any>& props,
                        boost::any& prop_vec, size_t size) const
        {
            if (typeid(Property) == props[0].type())
            {
                try
                {
                    vector<typename Property::unchecked_t> vec;
                    vec.resize(props.size());
                    for (size_t i = 0; i < props.size(); ++i)
                        vec[i] =
                            any_cast<Property>(props[i]).get_unchecked(size);
                    prop_vec = vec;
                }
                catch (bad_any_cast&){}
            }
        }
    };
};


struct get_property_vector_type
{
    template <class Property>
    struct apply
    {
        typedef vector<typename Property::unchecked_t> type;
    };
};

void extended_clustering(GraphInterface& g, boost::python::list props)
{
    vector<any> cmaps(boost::python::len(props));
    for (size_t i = 0; i < cmaps.size(); ++i)
        cmaps[i] = boost::python::extract<boost::any>(props[i])();

    boost::any vprop =
        prop_vector<writable_vertex_scalar_properties>()
        (cmaps, num_vertices(g.get_graph()));
    if (vprop.empty())
        throw ValueException("all vertex properties must be of the same"
                             " floating point type");

    typedef boost::mpl::transform<writable_vertex_scalar_properties,
                                  get_property_vector_type>::type
        properties_vector;

    run_action<>()
        (g,
         [&](auto&& graph, auto&& a2)
         {
             return get_extended_clustering()
                 (std::forward<decltype(graph)>(graph),
                  any_cast<GraphInterface::vertex_index_map_t>(
                      g.get_vertex_index()),
                  std::forward<decltype(a2)>(a2));
         },
         properties_vector())(vprop);
}
