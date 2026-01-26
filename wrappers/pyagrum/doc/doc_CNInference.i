%define ADD_CNINFERENCE_DOC(classname...)
/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/







%feature("docstring") gum::credal::classname::dynamicExpMax
"
Get the upper dynamic expectation of a given variable prefix.

Parameters
----------
varName : str
	the variable name prefix which upper expectation we want.

Returns
-------
float
    a constant reference to the variable upper expectation over all time steps.
"

%feature("docstring") gum::credal::classname::dynamicExpMin
"
Get the lower dynamic expectation of a given variable prefix.

Parameters
----------
varName : str
	the variable name prefix which lower expectation we want.

Returns
-------
float
    a constant reference to the variable lower expectation over all time steps.
"

%feature("docstring") gum::credal::classname::eraseAllEvidence
"
Erase all inference related data to perform another one.

You need to insert evidence again if needed but modalities are kept. You can insert new ones by using the appropriate method which will delete the old ones.
"

%feature("docstring") gum::credal::classname::insertEvidenceFile
"
Insert evidence from file.

Parameters
----------
path : str
	the path to the evidence file.
"

%feature("docstring") gum::credal::classname::insertModalsFile
"
Insert variables modalities from file to compute expectations.

Parameters
----------
path : str
	The path to the modalities file.
"

%feature("docstring") gum::credal::classname::makeInference
"
Starts the inference.
"

%feature("docstring") gum::credal::classname::marginalMax
"
Get the upper marginals of a given node id.

Parameters
----------
id : int
	the node id which upper marginals we want.
varName : str
	the variable name which upper marginals we want.

Returns
-------
list
    a constant reference to this node upper marginals.

Raises
------
  pyagrum.IndexError
	If the node does not belong to the Credal network
"

%feature("docstring") gum::credal::classname::marginalMin
"
Get the lower marginals of a given node id.

Parameters
----------
id : int
	the node id which lower marginals we want.
varName : str
	the variable name which lower marginals we want.

Returns
-------
list
    a constant reference to this node lower marginals.

Raises
------
  pyagrum.IndexError
	If the node does not belong to the Credal network
"

%feature("docstring") gum::credal::classname::saveInference
"
Saves marginals.

Parameters
----------
path : str
	The path to the file to save marginals.
"

%feature("docstring") gum::credal::classname::setRepetitiveInd
"
Parameters
----------
flag : bool
	True if repetitive independence is to be used, false otherwise. Only usefull with dynamic networks.
"

%feature("docstring") gum::credal::classname::inferenceType
"
Returns
-------
int
	the inference type
"
%enddef

ADD_CNINFERENCE_DOC(CNMonteCarloSampling<double>)
ADD_CNINFERENCE_DOC(CNLoopyPropagation<double>)
