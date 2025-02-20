Quick start using `graph-tool`
==============================

The :mod:`graph_tool` module provides a :class:`~graph_tool.Graph` class
and several algorithms that operate on it. The internals of this class,
and of most algorithms, are written in C++ for performance, using the
`Boost Graph Library <http://www.boost.org>`_.

The module must be of course imported before it can be used. The package is
subdivided into several sub-modules. To import everything from all of them, one
can do:

.. testsetup::

   np.random.seed(42)
   gt.seed_rng(42)

.. doctest::

   >>> from graph_tool.all import *

In the following, it will always be assumed that the previous line was run.

Creating and manipulating graphs
--------------------------------

An empty graph can be created by instantiating a :class:`~graph_tool.Graph`
class:

.. doctest::

   >>> g = Graph()

By default, newly created graphs are always directed. To construct undirected
graphs, one must pass a value to the ``directed`` parameter:

.. doctest::

   >>> ug = Graph(directed=False)

A graph can always be switched *on-the-fly* from directed to undirected
(and vice versa), with the :meth:`~graph_tool.Graph.set_directed`
method. The "directedness" of the graph can be queried with the
:meth:`~graph_tool.Graph.is_directed` method,

.. doctest::

   >>> ug = Graph()
   >>> ug.set_directed(False)
   >>> assert ug.is_directed() == False

A graph can also be created by providing another graph, in which case
the entire graph (and its internal property maps, see
:ref:`sec_property_maps`) is copied:

.. doctest::

   >>> g1 = Graph()
   >>> # ... construct g1 ...
   >>> g2 = Graph(g1)                 # g1 and g2 are copies

Above, ``g2`` is a "deep" copy of ``g1``, i.e. any modification of
``g2`` will not affect ``g1``.

Once a graph is created, it can be populated with vertices and edges. A
vertex can be added with the :meth:`~graph_tool.Graph.add_vertex`
method, which returns an instance of a :class:`~graph_tool.Vertex`
class, also called a *vertex descriptor*. For instance, the following
code creates two vertices, and returns vertex descriptors stored in the
variables ``v1`` and ``v2``.

.. doctest::

   >>> v1 = g.add_vertex()
   >>> v2 = g.add_vertex()

Edges can be added in an analogous manner, by calling the
:meth:`~graph_tool.Graph.add_edge` method, which returns an edge
descriptor (an instance of the :class:`~graph_tool.Edge` class):

.. doctest::

   >>> e = g.add_edge(v1, v2)

The above code creates a directed edge from ``v1`` to ``v2``. We can
visualize the graph we created so far with the
:func:`~graph_tool.draw.graph_draw` function.

.. doctest::

   >>> graph_draw(g, vertex_text=g.vertex_index, output="two-nodes.pdf")
   <...>

.. testcleanup::

    conv_png("two-nodes.pdf")

   
.. figure:: two-nodes.png
   :align: center
   :width: 200px

   A simple directed graph with two vertices and one edge, created by
   the commands above.

With vertex and edge descriptors, one can examine and manipulate the
graph in an arbitrary manner. For instance, in order to obtain the
out-degree of a vertex, we can simply call the
:meth:`~graph_tool.Vertex.out_degree` method:

.. doctest::

   >>> print(v1.out_degree())
   1

Analogously, we could have used the :meth:`~graph_tool.Vertex.in_degree`
method to query the in-degree.

.. note::

   For undirected graphs, the "out-degree" is synonym for degree, and
   in this case the in-degree of a vertex is always zero.

Edge descriptors have two useful methods, :meth:`~graph_tool.Edge.source`
and :meth:`~graph_tool.Edge.target`, which return the source and target
vertex of an edge, respectively.

.. doctest::

   >>> print(e.source(), e.target())
   0 1

The :meth:`~graph_tool.Graph.add_vertex` method also accepts an optional
parameter which specifies the number of vertices to create. If this
value is greater than 1, it returns an iterator on the added vertex
descriptors:

.. doctest::

   >>> vlist = g.add_vertex(10)
   >>> print(len(list(vlist)))
   10

Each vertex in a graph has an unique index, which is *always* between
:math:`0` and :math:`N-1`, where :math:`N` is the number of
vertices. This index can be obtained by using the
:attr:`~graph_tool.Graph.vertex_index` attribute of the graph (which is
a *property map*, see :ref:`sec_property_maps`), or by converting the
vertex descriptor to an ``int``.

.. doctest::

   >>> v = g.add_vertex()
   >>> print(g.vertex_index[v])
   12
   >>> print(int(v))
   12

   
Edges and vertices can also be removed at any time with the
:meth:`~graph_tool.Graph.remove_vertex` and :meth:`~graph_tool.Graph.remove_edge` methods,

.. doctest::

   >>> g.remove_edge(e)                               # e no longer exists
   >>> g.remove_vertex(v2)                # the second vertex is also gone

   
.. note::

   Removing a vertex is typically an :math:`O(N)` operation. The
   vertices are internally stored in a `STL vector
   <http://en.wikipedia.org/wiki/Vector_%28STL%29>`_, so removing an
   element somewhere in the middle of the list requires the shifting of
   the rest of the list. Thus, fast :math:`O(1)` removals are only
   possible either if one can guarantee that only vertices in the end of
   the list are removed (the ones last added to the graph), or if the
   relative vertex ordering is invalidated. The latter behavior can be
   achieved by passing the option ``fast == True``, to
   :meth:`~graph_tool.Graph.remove_vertex`, which causes the vertex
   being deleted to be 'swapped' with the last vertex (i.e. with the
   largest index), which will in turn inherit the index of the vertex
   being deleted.

.. warning::

   Because of the above, removing a vertex with an index smaller than
   :math:`N-1` will **invalidate either the last** (``fast = True``)
   **or all** (``fast = False``) **descriptors pointing to vertices with
   higher index**.

   As a consequence, if more than one vertex is to be removed at a given
   time, they should **always** be removed in decreasing index order:
   
   .. code-block::

       # 'del_list' is a list of vertex descriptors
       for v in reversed(sorted(del_list)):
           g.remove_vertex(v)

   Alternatively (and preferably), a list (or any iterable) may be
   passed directly as the ``vertex`` parameter of the
   :meth:`~graph_tool.Graph.remove_vertex` function, and the above is
   performed internally (in C++).

   Note that property map values (see :ref:`sec_property_maps`) are
   unaffected by the index changes due to vertex removal, as they are
   modified accordingly by the library.

.. note::

   Removing an edge is an :math:`O(k_{s} + k_{t})` operation, where
   :math:`k_{s}` is the out-degree of the source vertex, and
   :math:`k_{t}` is the in-degree of the target vertex. This can be made
   faster by setting :meth:`~graph_tool.Graph.set_fast_edge_removal` to
   `True`, in which case it becomes :math:`O(1)`, at the expense of
   additional data of size :math:`O(E)`.

   No edge descriptors are ever invalidated after edge removal, with the
   exception of the edge being removed.

Since vertices are uniquely identifiable by their indexes, there is no
need to keep the vertex descriptor lying around to access them at a
later point. If we know its index, we can obtain the descriptor of a
vertex with a given index using the :meth:`~graph_tool.Graph.vertex`
method,

.. doctest::

   >>> v = g.vertex(8)

which takes an index, and returns a vertex descriptor. Edges cannot be
directly obtained by its index, but if the source and target vertices of
a given edge are known, it can be retrieved with the
:meth:`~graph_tool.Graph.edge` method

.. doctest::

   >>> g.add_edge(g.vertex(2), g.vertex(3))
   <...>
   >>> e = g.edge(2, 3)


Another way to obtain edge or vertex descriptors is to *iterate* through
them, as described in section :ref:`sec_iteration`. This is in fact the
most useful way of obtaining vertex and edge descriptors.

Like vertices, edges also have unique indexes, which are given by the
:attr:`~graph_tool.Graph.edge_index` property:

.. doctest::

   >>> e = g.add_edge(g.vertex(0), g.vertex(1))
   >>> print(g.edge_index[e])
   1

Differently from vertices, edge indexes do not necessarily conform to
any specific range. If no edges are ever removed, the indexes will be in
the range :math:`[0, E-1]`, where :math:`E` is the number of edges, and
edges added earlier have lower indexes. However if an edge is removed,
its index will be "vacant", and the remaining indexes will be left
unmodified, and thus will not all lie in the range :math:`[0, E-1]`.  If
a new edge is added, it will reuse old indexes, in an increasing order.


.. _sec_iteration:

Iterating over vertices and edges
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Algorithms must often iterate through vertices, edges, out-edges of a
vertex, etc. The :class:`~graph_tool.Graph` and
:class:`~graph_tool.Vertex` classes provide different types of iterators
for doing so. The iterators always point to edge or vertex descriptors.

Iterating over all vertices or edges
""""""""""""""""""""""""""""""""""""

In order to iterate through all the vertices or edges of a graph, the
:meth:`~graph_tool.Graph.vertices` and :meth:`~graph_tool.Graph.edges`
methods should be used:

.. testcode::

   for v in g.vertices():
       print(v)
   for e in g.edges():
       print(e)

.. testoutput::
   :hide:

   0
   1
   2
   3
   4
   5
   6
   7
   8
   9
   10
   11
   (0, 1)
   (2, 3)

The code above will print the vertices and edges of the graph in the order they
are found.

Iterating over the neighborhood of a vertex
""""""""""""""""""""""""""""""""""""""""""""

The out- and in-edges of a vertex, as well as the out- and in-neighbors can be
iterated through with the :meth:`~graph_tool.Vertex.out_edges`,
:meth:`~graph_tool.Vertex.in_edges`, :meth:`~graph_tool.Vertex.out_neighbors`
and :meth:`~graph_tool.Vertex.in_neighbors` methods, respectively.

.. testcode::

   for v in g.vertices():
      for e in v.out_edges():
          print(e)
      for w in v.out_neighbors():
          print(w)

      # the edge and neighbors order always match
      for e, w in zip(v.out_edges(), v.out_neighbors()):
          assert e.target() == w

.. testoutput::
   :hide:

   (0, 1)
   1
   (2, 3)
   3

The code above will print the out-edges and out-neighbors of all
vertices in the graph.

.. warning::

   You should never remove vertex or edge descriptors when iterating
   over them, since this invalidates the iterators. If you plan to
   remove vertices or edges during iteration, you must first store them
   somewhere (such as in a list) and remove them only after no iterator
   is being used. Removal during iteration will cause bad things to
   happen.

Fast iteration over vertices and edges
""""""""""""""""""""""""""""""""""""""

While convenient, looping over the graph as described in the previous
section is not the most efficient approach. This is because the loops
are performed in pure Python, and hence it undermines the main feature
of the library, which is the offloading of loops from Python to
C++. Following the :mod:`numpy` philosophy, :mod:`graph_tool` also
provides an array-based interface that avoids loops in Python. This is
done with the :meth:`~graph_tool.Graph.get_vertices`,
:meth:`~graph_tool.Graph.get_edges`,
:meth:`~graph_tool.Graph.get_out_edges`,
:meth:`~graph_tool.Graph.get_in_edges`,
:meth:`~graph_tool.Graph.get_all_edges`,
:meth:`~graph_tool.Graph.get_out_neighbors`,
:meth:`~graph_tool.Graph.get_in_neighbors`,
:meth:`~graph_tool.Graph.get_all_neighbors`,
:meth:`~graph_tool.Graph.get_out_degrees`,
:meth:`~graph_tool.Graph.get_in_degrees` and
:meth:`~graph_tool.Graph.get_total_degrees` methods, which return
:class:`numpy.ndarray` instances instead of iterators.

For example, using this interface we can get the out-degree of each node via:

.. testcode::

   print(g.get_out_degrees(g.get_vertices()))

.. testoutput::

   [1 0 1 0 0 0 0 0 0 0 0 0]

or the sum of the product of the in and out-degrees of the endpoints of
each edge with:

.. testcode::

   edges = g.get_edges()
   in_degs = g.get_in_degrees(g.get_vertices())
   out_degs = g.get_out_degrees(g.get_vertices())
   print((out_degs[edges[:,0]] * in_degs[edges[:,1]]).sum())

.. testoutput::

   2
   
.. _sec_property_maps:

Property maps
-------------

Property maps are a way of associating additional information to the
vertices, edges or to the graph itself. There are thus three types of
property maps: vertex, edge and graph. They are handled by the
classes :class:`~graph_tool.VertexPropertyMap`,
:class:`~graph_tool.EdgePropertyMap`, and
:class:`~graph_tool.GraphPropertyMap`. Each created property map has an
associated *value type*, which must be chosen from the predefined set:

.. tabularcolumns:: |l|l|

.. table::

    ========================     ======================
     Type name                   Alias
    ========================     ======================
    ``bool``                     ``uint8_t``
    ``int16_t``                  ``short``
    ``int32_t``                  ``int``
    ``int64_t``                  ``long``, ``long long``
    ``double``                   ``float``
    ``long double``
    ``string``
    ``vector<bool>``             ``vector<uint8_t>``
    ``vector<int16_t>``          ``vector<short>``
    ``vector<int32_t>``          ``vector<int>``
    ``vector<int64_t>``          ``vector<long>``, ``vector<long long>``
    ``vector<double>``           ``vector<float>``
    ``vector<long double>``
    ``vector<string>``
    ``python::object``           ``object``
    ========================     ======================

New property maps can be created for a given graph by calling one of the
methods :meth:`~graph_tool.Graph.new_vertex_property` (alias
:meth:`~graph_tool.Graph.new_vp`),
:meth:`~graph_tool.Graph.new_edge_property` (alias
:meth:`~graph_tool.Graph.new_ep`), or
:meth:`~graph_tool.Graph.new_graph_property` (alias
:meth:`~graph_tool.Graph.new_gp`), for each map type. The values are
then accessed by vertex or edge descriptors, or the graph itself, as
such:

.. testcode::

    from numpy.random import randint

    g = Graph()
    g.add_vertex(100)

    # insert some random links
    for s,t in zip(randint(0, 100, 100), randint(0, 100, 100)):
        g.add_edge(g.vertex(s), g.vertex(t))

    vprop_double = g.new_vertex_property("double")            # Double-precision floating point
    v = g.vertex(10)
    vprop_double[v] = 3.1416

    vprop_vint = g.new_vertex_property("vector<int>")         # Vector of ints
    v = g.vertex(40)
    vprop_vint[v] = [1, 3, 42, 54]
    
    eprop_dict = g.new_edge_property("object")                # Arbitrary Python object.
    e = g.edges().next()
    eprop_dict[e] = {"foo": "bar", "gnu": 42}                 # In this case, a dict.

    gprop_bool = g.new_graph_property("bool")                 # Boolean
    gprop_bool[g] = True

Property maps with scalar value types can also be accessed as a
:class:`numpy.ndarray`, with the
:meth:`~graph_tool.PropertyMap.get_array` method, or the
:attr:`~graph_tool.PropertyMap.a` attribute, e.g.,

.. testcode::

    from numpy.random import random

    # this assigns random values to the vertex properties
    vprop_double.get_array()[:] = random(g.num_vertices())

    # or more conveniently (this is equivalent to the above)
    vprop_double.a = random(g.num_vertices())

.. _sec_internal_props:

Internal property maps
^^^^^^^^^^^^^^^^^^^^^^

Any created property map can be made "internal" to the corresponding
graph. This means that it will be copied and saved to a file together
with the graph. Properties are internalized by including them in the
graph's dictionary-like attributes
:attr:`~graph_tool.Graph.vertex_properties`,
:attr:`~graph_tool.Graph.edge_properties` or
:attr:`~graph_tool.Graph.graph_properties` (or their aliases,
:attr:`~graph_tool.Graph.vp`, :attr:`~graph_tool.Graph.ep` or
:attr:`~graph_tool.Graph.gp`, respectively). When inserted in the graph,
the property maps must have an unique name (between those of the same
type):

.. doctest::

    >>> eprop = g.new_edge_property("string")
    >>> g.edge_properties["some name"] = eprop
    >>> g.list_properties()
    some name      (edge)    (type: string)

Internal graph property maps behave slightly differently. Instead of
returning the property map object, the value itself is returned from the
dictionaries:

.. doctest::

    >>> gprop = g.new_graph_property("int")
    >>> g.graph_properties["foo"] = gprop   # this sets the actual property map
    >>> g.graph_properties["foo"] = 42      # this sets its value
    >>> print(g.graph_properties["foo"])
    42
    >>> del g.graph_properties["foo"]       # the property map entry is deleted from the dictionary

For convenience, the internal property maps can also be accessed via
attributes:

.. doctest::

    >>> vprop = g.new_vertex_property("double")
    >>> g.vp.foo = vprop                        # equivalent to g.vertex_properties["foo"] = vprop
    >>> v = g.vertex(0)
    >>> g.vp.foo[v] = 3.14
    >>> print(g.vp.foo[v])
    3.14

.. _sec_graph_io:

Graph I/O
---------

Graphs can be saved and loaded in four formats: `graphml
<http://graphml.graphdrawing.org/>`_, `dot
<http://www.graphviz.org/doc/info/lang.html>`_, `gml
<http://www.fim.uni-passau.de/en/fim/faculty/chairs/theoretische-informatik/projects.html>`_
and a custom binary format ``gt`` (see :ref:`sec_gt_format`). 

.. warning::

    The binary format ``gt`` and the text-based ``graphml`` are the
    preferred formats, since they are by far the most complete. Both
    these formats are equally complete, but the ``gt`` format is faster
    and requires less storage.

    The ``dot`` and ``gml`` formats are fully supported, but since they
    contain no precise type information, all properties are read as
    strings (or also as double, in the case of ``gml``), and must be
    converted by hand to the desired type. Therefore you should always
    use either ``gt`` or ``graphml``, since they implement an exact
    bit-for-bit representation of all supported :ref:`sec_property_maps`
    types, except when interfacing with other software, or existing
    data, which uses ``dot`` or ``gml``.

A graph can be saved or loaded to a file with the :attr:`~graph_tool.Graph.save`
and :attr:`~graph_tool.Graph.load` methods, which take either a file name or a
file-like object. A graph can also be loaded from disc with the
:func:`~graph_tool.load_graph` function, as such:

.. testcode::

    g = Graph()
    #  ... fill the graph ...
    g.save("my_graph.xml.gz")    
    g2 = load_graph("my_graph.xml.gz")
    # g and g2 should be copies of each other

Graph classes can also be pickled with the :mod:`pickle` module.


An Example: Building a Price Network
------------------------------------

A Price network is the first known model of a "scale-free" graph,
invented in 1976 by `de Solla Price
<http://en.wikipedia.org/wiki/Derek_J._de_Solla_Price>`_. It is defined
dynamically, where at each time step a new vertex is added to the graph,
and connected to an old vertex, with probability proportional to its
in-degree. The following program implements this construction using
``graph-tool``.

.. note::

   Note that it would be much faster just to use the
   :func:`~graph_tool.generation.price_network` function, which is
   implemented in C++, as opposed to the script below which is in pure
   Python. The code below is merely a demonstration on how to use the
   library.

.. literalinclude:: price.py
   :linenos:

The following is what should happen when the program is run.

.. testcode::
   :hide:

   from price import *
   clf()

.. testoutput::

    vertex: 36063 in-degree: 0 out-degree: 1 age: 36063
    vertex: 9075 in-degree: 4 out-degree: 1 age: 9075
    vertex: 5967 in-degree: 3 out-degree: 1 age: 5967
    vertex: 1113 in-degree: 7 out-degree: 1 age: 1113
    vertex: 25 in-degree: 84 out-degree: 1 age: 25
    vertex: 10 in-degree: 541 out-degree: 1 age: 10
    vertex: 5 in-degree: 140 out-degree: 1 age: 5
    vertex: 2 in-degree: 459 out-degree: 1 age: 2
    vertex: 1 in-degree: 520 out-degree: 1 age: 1
    vertex: 0 in-degree: 210 out-degree: 0 age: 0
    Nowhere else to go... We found the main hub!

Below is the degree distribution, with :math:`10^5` nodes (in order to
the asymptotic behavior to be even clearer, the number of vertices needs
to be increased to something like :math:`10^6` or :math:`10^7`).

.. figure:: price-deg-dist.*
   :align: center

   In-degree distribution of a price network with :math:`10^5` nodes.


We can draw the graph to see some other features of its topology. For that we
use the :func:`~graph_tool.draw.graph_draw` function.

.. testcode::

   g = load_graph("price.xml.gz")
   age = g.vertex_properties["age"]

   pos = sfdp_layout(g)
   graph_draw(g, pos, output_size=(1000, 1000), vertex_color=[1,1,1,0],
              vertex_fill_color=age, vertex_size=1, edge_pen_width=1.2,
              vcmap=matplotlib.cm.gist_heat_r, output="price.pdf")

.. testcleanup::

   conv_png("price.pdf")

.. figure:: price.png
   :align: center
   :width: 95%        

   A Price network with :math:`10^5` nodes. The vertex colors represent
   the age of the vertex, from older (red) to newer (black).

.. _sec_graph_filtering:

Graph filtering
---------------

One of the very nice features of ``graph-tool`` is the "on-the-fly" filtering of
edges and/or vertices. Filtering means the temporary masking of vertices/edges,
which are in fact not really removed, and can be easily recovered. Vertices or
edges which are to be filtered should be marked with a
:class:`~graph_tool.PropertyMap` with value type ``bool``, and then set with
:meth:`~graph_tool.Graph.set_vertex_filter` or
:meth:`~graph_tool.Graph.set_edge_filter` methods. By default, vertex or edges
with value "1" are `kept` in the graphs, and those with value "0" are filtered
out. This behaviour can be modified with the ``inverted`` parameter of the
respective functions. All manipulation functions and algorithms will work as if
the marked edges or vertices were removed from the graph, with minimum overhead.

.. note::

    It is important to emphasize that the filtering functionality does not add
    any overhead when the graph is not being filtered. In this case, the
    algorithms run just as fast as if the filtering functionality didn't exist.

Here is an example which obtains the minimum spanning tree of a graph,
using the function :func:`~graph_tool.topology.min_spanning_tree` and
edge filtering.

.. testcode::
   :hide:

   from numpy.random import *
   seed(42)

.. testcode::

   g, pos = triangulation(random((500, 2)) * 4, type="delaunay")
   tree = min_spanning_tree(g)
   graph_draw(g, pos=pos, edge_color=tree, output="min_tree.svg")


The ``tree`` property map has a bool type, with value "1" if the edge belongs to
the tree, and "0" otherwise. Below is an image of the original graph, with the
marked edges.

.. figure:: min_tree.*
   :align: center
   :figwidth: 400

We can now filter out the edges which don't belong to the minimum spanning tree.

.. testcode::

   g.set_edge_filter(tree)
   graph_draw(g, pos=pos, output="min_tree_filtered.svg")

This is how the graph looks when filtered:

.. figure:: min_tree_filtered.*
   :align: center
   :figwidth: 400

Everything should work transparently on the filtered graph, simply as if the
masked edges were removed. For instance, the following code will calculate the
:func:`~graph_tool.centrality.betweenness` centrality of the edges and vertices,
and draws them as colors and line thickness in the graph.

.. testcode::

    bv, be = betweenness(g)
    be.a /= be.a.max() / 5
    graph_draw(g, pos=pos, vertex_fill_color=bv, edge_pen_width=be,
               output="filtered-bt.svg")

.. figure:: filtered-bt.*
   :align: center
   :figwidth: 400


The original graph can be recovered by setting the edge filter to ``None``.

.. testcode::

    g.set_edge_filter(None)
    bv, be = betweenness(g)
    be.a /= be.a.max() / 5
    graph_draw(g, pos=pos, vertex_fill_color=bv, edge_pen_width=be,
               output="nonfiltered-bt.svg")

.. figure:: nonfiltered-bt.*
   :align: center
   :figwidth: 400

Everything works in analogous fashion with vertex filtering.

Additionally, the graph can also have its edges reversed with the
:meth:`~graph_tool.Graph.set_reversed` method. This is also an :math:`O(1)`
operation, which does not really modify the graph.

As mentioned previously, the directedness of the graph can also be changed
"on-the-fly" with the :meth:`~graph_tool.Graph.set_directed` method.

.. _sec_graph_views:

Graph views
^^^^^^^^^^^

It is often desired to work with filtered and unfiltered graphs
simultaneously, or to temporarily create a filtered version of graph for
some specific task. For these purposes, graph-tool provides a
:class:`~graph_tool.GraphView` class, which represents a filtered "view"
of a graph, and behaves as an independent graph object, which shares the
underlying data with the original graph. Graph views are constructed by
instantiating a :class:`~graph_tool.GraphView` class, and passing a
graph object which is supposed to be filtered, together with the desired
filter parameters. For example, to create a directed view of the graph
``g`` constructed above, one should do:

.. doctest::

    >>> ug = GraphView(g, directed=True)
    >>> ug.is_directed()
    True

Graph views also provide a much more direct and convenient approach to
vertex/edge filtering: To construct a filtered minimum spanning tree
like in the example above, one must only pass the filter property as the
"efilt" parameter:

.. doctest::

    >>> tv = GraphView(g, efilt=tree)

Note that this is an :math:`O(1)` operation, since it is equivalent (in
speed) to setting the filter in graph ``g`` directly, but in this case
the object ``g`` remains unmodified.

Like above, the result should be the isolated minimum spanning tree:

.. doctest::

    >>> bv, be = betweenness(tv)
    >>> be.a /= be.a.max() / 5
    >>> graph_draw(tv, pos=pos, vertex_fill_color=bv,
    ...            edge_pen_width=be, output="mst-view.svg")
    <...>

.. figure:: mst-view.*
   :align: center
   :figwidth: 400

   A view of the Delaunay graph, isolating only the minimum spanning tree.

.. note::

   :class:`~graph_tool.GraphView` objects behave *exactly* like regular
   :class:`~graph_tool.Graph` objects. In fact,
   :class:`~graph_tool.GraphView` is a subclass of
   :class:`~graph_tool.Graph`. The only difference is that a
   :class:`~graph_tool.GraphView` object shares its internal data with
   its parent :class:`~graph_tool.Graph` class. Therefore, if the
   original :class:`~graph_tool.Graph` object is modified, this
   modification will be reflected immediately in the
   :class:`~graph_tool.GraphView` object, and vice versa.

For even more convenience, one can supply a function as filter
parameter, which takes a vertex or an edge as single parameter, and
returns ``True`` if the vertex/edge should be kept and ``False``
otherwise. For instance, if we want to keep only the most "central"
edges, we can do:

.. doctest::

    >>> bv, be = betweenness(g)
    >>> u = GraphView(g, efilt=lambda e: be[e] > be.a.max() / 2)

This creates a graph view ``u`` which contains only the edges of ``g``
which have a normalized betweenness centrality larger than half of the
maximum value. Note that, differently from the case above, this is an
:math:`O(E)` operation, where :math:`E` is the number of edges, since
the supplied function must be called :math:`E` times to construct a
filter property map. Thus, supplying a constructed filter map is always
faster, but supplying a function can be more convenient.

The graph view constructed above can be visualized as

.. doctest::

    >>> be.a /= be.a.max() / 5
    >>> graph_draw(u, pos=pos, vertex_fill_color=bv, output="central-edges-view.svg")
    <...>

.. figure:: central-edges-view.*
   :align: center
   :figwidth: 400

   A view of the Delaunay graph, isolating only the edges with
   normalized betweenness centrality larger than 0.01.

Composing graph views
"""""""""""""""""""""

Since graph views are regular graphs, one can just as easily create
graph views `of graph views`. This provides a convenient way of
composing filters. For instance, in order to isolate the minimum
spanning tree of all vertices of the example above which have a degree
larger than four, one can do:


    >>> u = GraphView(g, vfilt=lambda v: v.out_degree() > 4)
    >>> tree = min_spanning_tree(u)
    >>> u = GraphView(u, efilt=tree)

The resulting graph view can be visualized as

.. doctest::

    >>> graph_draw(u, pos=pos, output="composed-filter.svg")
    <...>

.. figure:: composed-filter.*
   :align: center
   :figwidth: 400

   A composed view, obtained as the minimum spanning tree of all vertices
   in the graph which have a degree larger than four.