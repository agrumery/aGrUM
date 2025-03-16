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





%module(package="pyAgrum", docstring="Credal networks module", directors="1") cn
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

%import bn.i

%include "credalnet.i"
%include "CNinference.i"

//////////////////////////////////////////////////////////////////
/* extraction of the API for all wrappers */
//////////////////////////////////////////////////////////////////
%include "aGrUM_wrap_CN.i"

//////////////////////////////////////////////////////////////////
/* Many functions added for pyAgrum                             */
//////////////////////////////////////////////////////////////////

%pythoncode "common.py"
