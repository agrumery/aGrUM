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

%module(package="pyAgrum", docstring="Markov random field module", directors="1") mrf
%include "docs.i"

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

%include "pylisteners.i"

%import bn.i

%include "markovRandomField.i"
%include "MRFinference.i"

//////////////////////////////////////////////////////////////////
/* extraction of the API for all wrappers */
//////////////////////////////////////////////////////////////////
%include "aGrUM_wrap_MRF.i"

//////////////////////////////////////////////////////////////////
/* Many functions added for pyAgrum                             */
//////////////////////////////////////////////////////////////////
%pythoncode "common.py"

%include "gum_functions_MRF.i"
