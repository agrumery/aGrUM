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

%module(package="pyAgrum", docstring="Bayesian networks module", directors="1") bn
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
%import base.i

%include "pylisteners.i"

ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::MarkovBlanket);
ADD_METHODS_FOR_ALL_GUM_GRAPHCLASS(gum::EssentialGraph);

ADD_DI_METHOD_TO_GRAPHCLASS(gum::EssentialGraph);
%ignore gum::EssentialGraph::arcs const;
%ignore gum::EssentialGraph::parents const;
%ignore gum::EssentialGraph::children const;
ADD_DI_METHOD_TO_GRAPHCLASS(gum::MarkovBlanket);
%ignore gum::MarkovBlanket::arcs const;
%ignore gum::MarkovBlanket::parents const;
%ignore gum::MarkovBlanket::children const;

ADD_UNDI_METHOD_TO_GRAPHCLASS(gum::EssentialGraph);

%include "bayesnet.i"
%include "inference.i"
%include "bndistance.i"
%include "BNDatabaseGenerator.i"

%include "learning.i"

%include "prm.i"

//////////////////////////////////////////////////////////////////
/* extraction of the API for all wrappers */
//////////////////////////////////////////////////////////////////
%include "aGrUM_wrap_BN.i"

//////////////////////////////////////////////////////////////////
/* Many functions added for pyAgrum                             */
//////////////////////////////////////////////////////////////////
%pythoncode "common.py"

%include "gum_functions_BN.i"
