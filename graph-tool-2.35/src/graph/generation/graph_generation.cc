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

#define BOOST_PYTHON_MAX_ARITY 20

#include "graph.hh"
#include "graph_util.hh"
#include "graph_filtering.hh"
#include "graph_generation.hh"
#include "sampler.hh"
#include "dynamic_sampler.hh"
#include <boost/python.hpp>

using namespace std;
using namespace boost;
using namespace graph_tool;

class PythonFuncWrap
{
public:
    PythonFuncWrap(boost::python::object o): _o(o) {}

    pair<size_t, size_t> operator()(size_t i) const
    {
        boost::python::object ret = _o(i);
        return boost::python::extract<pair<size_t,size_t> >(ret);
    }

    size_t operator()(size_t i, bool) const
    {
        boost::python::object ret = _o(i);
        return boost::python::extract<size_t>(ret);
    }

private:
    boost::python::object _o;
};

void generate_graph(GraphInterface& gi, size_t N,
                    boost::python::object deg_sample, bool no_parallel,
                    bool no_self_loops, bool undirected, rng_t& rng,
                    bool verbose, bool verify)
{
    typedef graph_tool::detail::get_all_graph_views::apply<
    graph_tool::detail::filt_scalar_type, boost::mpl::bool_<false>,
        boost::mpl::bool_<false>, boost::mpl::bool_<false>,
        boost::mpl::bool_<true>, boost::mpl::bool_<true> >::type graph_views;

    if (undirected)
        gi.set_directed(false);

    run_action<graph_views>()
        (gi,
         [&](auto&& graph)
         {
             return gen_graph()
                 (std::forward<decltype(graph)>(graph), N,
                  PythonFuncWrap(deg_sample), no_parallel, no_self_loops, rng,
                  verbose, verify);
         })();
}

void generate_sbm(GraphInterface& gi, boost::any ab, boost::python::object ors,
                  boost::python::object oss, boost::python::object oprobs,
                  boost::any ain_deg, boost::any aout_deg, bool micro_ers,
                  bool micro_degs, rng_t& rng);

void generate_knn(GraphInterface& gi, boost::python::object om, size_t k,
                  double r, double epsilon, bool cache, boost::any aw,
                  rng_t& rng);

void generate_knn_exact(GraphInterface& gi, boost::python::object om, size_t k,
                        boost::any aw);

size_t random_rewire(GraphInterface& gi, string strat, size_t niter,
                     bool no_sweep, bool self_loops, bool parallel_edges,
                     bool configuration, bool traditional, bool micro,
                     bool persist, boost::python::object corr_prob,
                     boost::any apin, boost::any block, bool cache, rng_t& rng,
                     bool verbose);
void predecessor_graph(GraphInterface& gi, GraphInterface& gpi,
                       boost::any pred_map);
void line_graph(GraphInterface& gi, GraphInterface& lgi,
                boost::any edge_index);
boost::python::tuple graph_union(GraphInterface& ugi, GraphInterface& gi,
                          boost::any avprop);
void vertex_property_union(GraphInterface& ugi, GraphInterface& gi,
                           boost::any p_vprop, boost::any p_eprop,
                           boost::any uprop, boost::any prop);
void edge_property_union(GraphInterface& ugi, GraphInterface& gi,
                         boost::any p_vprop, boost::any p_eprop,
                         boost::any uprop, boost::any prop);
void triangulation(GraphInterface& gi, boost::python::object points,
                   boost::any pos, string type, bool periodic);
void lattice(GraphInterface& gi, boost::python::object oshape, bool periodic);
void geometric(GraphInterface& gi, boost::python::object opoints, double r,
               boost::python::object orange, bool periodic, boost::any pos);
void price(GraphInterface& gi, size_t N, double gamma, double c, size_t m,
           rng_t& rng);
void complete(GraphInterface& gi, size_t N, bool directed, bool self_loops);
void circular(GraphInterface& gi, size_t N, size_t k, bool directed,
              bool self_loops);

void community_network(GraphInterface& gi, GraphInterface& cgi,
                       boost::any community_property,
                       boost::any condensed_community_property,
                       boost::any vertex_count, boost::any edge_count,
                       boost::any vweight, boost::any eweight, bool self_loops,
                       bool parallel_edges);

void community_network_vavg(GraphInterface& gi, GraphInterface& cgi,
                            boost::any community_property,
                            boost::any condensed_community_property,
                            boost::any vweight, boost::python::list avprops);

void community_network_eavg(GraphInterface& gi, GraphInterface& cgi,
                            boost::any community_property,
                            boost::any condensed_community_property,
                            boost::any eweight, boost::python::list aeprops,
                            bool self_loops, bool parallel_edges);

void export_maxent_sbm();

using namespace boost::python;

BOOST_PYTHON_MODULE(libgraph_tool_generation)
{
    docstring_options dopt(true, false);
    def("gen_graph", &generate_graph);
    def("gen_sbm", &generate_sbm);
    def("gen_knn", &generate_knn);
    def("gen_knn_exact", &generate_knn_exact);
    def("random_rewire", &random_rewire);
    def("predecessor_graph", &predecessor_graph);
    def("line_graph", &line_graph);
    def("graph_union", &graph_union);
    def("vertex_property_union", &vertex_property_union);
    def("edge_property_union", &edge_property_union);
    def("triangulation", &triangulation);
    def("lattice", &lattice);
    def("geometric", &geometric);
    def("price", &price);
    def("complete", &complete);
    def("circular", &circular);
    def("community_network", &community_network);
    def("community_network_vavg", &community_network_vavg);
    def("community_network_eavg", &community_network_eavg);
    export_maxent_sbm();

    class_<Sampler<int, boost::mpl::false_>>("Sampler",
                                             init<const vector<int>&, const vector<double>&>())
        .def("sample", &Sampler<int, boost::mpl::false_>::sample<rng_t>,
             return_value_policy<copy_const_reference>());

    class_<DynamicSampler<int>>("DynamicSampler",
                                init<const vector<int>&,
                                     const vector<double>&>())
        .def("sample", &DynamicSampler<int>::sample<rng_t>,
             return_value_policy<copy_const_reference>())
        .def("insert", &DynamicSampler<int>::insert)
        .def("remove", &DynamicSampler<int>::remove)
        .def("clear", &DynamicSampler<int>::clear)
        .def("rebuild", &DynamicSampler<int>::rebuild);
}
