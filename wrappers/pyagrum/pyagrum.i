/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/



%module(package="pyagrum", docstring="pyagrum module", directors="1") pyagrum

///////////////////////////////////
/////// base submodule ////////////
///////////////////////////////////
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


/* declaration of code enhancers for pyAgrum */
%include "pgm.i"

%include "core.i"
%include "variables.i"
%include "instantiation.i"

%include "tensor.i"

%include "graphs.i"

%include "MeekRules.i"


/* extraction of the API for all wrappers */
%include "aGrUM_wrap_BASE.i"

%include "gum_functions_BASE.i"

/////////////////////////////////
/////// BN submodule ////////////
/////////////////////////////////

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

/* extraction of the API for all wrappers */
%include "aGrUM_wrap_BN.i"

%include "gum_functions_BN.i"

/////////////////////////////////
/////// CN submodule ////////////
/////////////////////////////////
%include "credalnet.i"
%include "CNinference.i"

/* extraction of the API for all wrappers */
%include "aGrUM_wrap_CN.i"

/////////////////////////////////
/////// ID submodule ////////////
/////////////////////////////////
%include "influenceDiagram.i"
%include "influenceDiagramInference.i"

/* extraction of the API for all wrappers */
%include "aGrUM_wrap_ID.i"

/* Many functions added for pyAgrum                             */
%pythoncode "common.py"

%include "gum_functions_ID.i"

/////////////////////////////////
/////// MRF submodule ///////////
/////////////////////////////////
%include "markovRandomField.i"
%include "MRFinference.i"

/* extraction of the API for all wrappers */
%include "aGrUM_wrap_MRF.i"

%include "gum_functions_MRF.i"

/////////////////////////////////////////////
///// Many functions added for pyAgrum //////
/////////////////////////////////////////////
%pythoncode "common.py"

