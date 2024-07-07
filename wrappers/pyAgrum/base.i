/**
 *
 *  Copyright 2005-2024 Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

%module(package="pyAgrum", docstring="base module", directors="1") base
%include "docs.i"

// 302 : Identifier 'name' redefined (ignored).
// 315 : Nothing known about 'identifier'.
// 317 : Specialization of non-template
// 320 : Explicit template instantiation ignored.
// 325 : Nested class not currently supported (Database ignored)
// 327 : extern template effectively ignored
// 401 : not wrapped class
// 503 : DAG is not a correct name (still correctly wrapped ... ???)
// 509 : Overloaded method effectively ignore
#pragma SWIG nowarn=302,315,317,320,325,327,401,503,509

// ignore constexpr constructor redefinition
%ignorewarn("302") SetIteratorSafe;
%ignorewarn("302") SetIterator;
%ignorewarn("302") ListIteratorSafe;
%ignorewarn("302") ListIterator;
%ignorewarn("302") ListConstIteratorSafe;
%ignorewarn("302") ListConstIterator;

%feature("python:annotations", "c");  // Turn on function annotations and variable annotations globally
%feature("python:annotations:novar"); // Turn off variable annotations globally

%include "std_vector.i"
%include "std_string.i"
%include "std_pair.i"
%{
#include "extensions/helpers.h"
%}

//////////////////////////////////////////////////////////////////
/* declaration of code modifiers for 'pythonification' of aGrUM */
//////////////////////////////////////////////////////////////////
%include "exceptions.i"
%include "pythonize.i"

%include "gum_typemaps.i"

//////////////////////////////////////////////////////////////////
/* declaration of code enhancers for pyAgrum */
//////////////////////////////////////////////////////////////////
%include "pgm.i"

%include "core.i"
%include "variables.i"
%include "instantiation.i"

%include "potential.i"

%include "graphs.i"

%include "MeekRules.i"

//////////////////////////////////////////////////////////////////
/* extraction of the API for all wrappers */
//////////////////////////////////////////////////////////////////
%include "aGrUM_wrap_BASE.i"

//////////////////////////////////////////////////////////////////
/* Many functions added for pyAgrum                             */
//////////////////////////////////////////////////////////////////
%pythoncode "common.py"

%include "gum_functions_BASE.i"
