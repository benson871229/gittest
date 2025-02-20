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

#include <boost/python.hpp>

using namespace boost::python;
void export_arf();
void export_fruchterman_reingold();
void export_sfdp();
void export_radial();
void export_planar();

BOOST_PYTHON_MODULE(libgraph_tool_layout)
{
    docstring_options dopt(true, false);
    export_arf();
    export_fruchterman_reingold();
    export_sfdp();
    export_radial();
    export_planar();
}
