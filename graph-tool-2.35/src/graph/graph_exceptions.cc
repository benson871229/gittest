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

#include "graph_exceptions.hh"

using namespace std;
using namespace graph_tool;

GraphException::GraphException(const string& error) {_error = error;}
GraphException::~GraphException() throw () {}
const char * GraphException::what () const throw () {return _error.c_str();}

IOException::IOException(const string& error): GraphException(error) {}
IOException::~IOException() throw () {}

ValueException::ValueException(const string& error): GraphException(error) {}
ValueException::~ValueException() throw () {}
