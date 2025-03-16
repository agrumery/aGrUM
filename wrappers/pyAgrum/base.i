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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





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

%include "tensor.i"

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
