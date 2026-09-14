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




%module(package="pyagrum.ktbn", docstring="pyagrum.ktbn module") ktbn

// %import only carries typemap/type declarations into this module's own
// generated .cxx, not raw %{ %} code blocks -- PyAgrumHelper and the
// PyAgrumSetOf*/PyAgrumList*/... sentinel typedefs (used by typemaps
// declared in pyagrum.i) must be re-declared here too.
%{
#include "extensions/helpers.h"
#include "extensions/helpersKTBN.h"
#include "extensions/pyagrumSentinelTypes.h"
#include "extensions/pyagrumExceptionHandling.h"
%}

%import "pyagrum.i"

//////////////////////////////////
/////// KTBN submodule ///////////
//////////////////////////////////

// Pre-template directives (must precede aGrUM_wrap_KTBN.i's %template, same
// rule as %rename/%feature elsewhere -- see ktbnModel.i/ktbnInference.i/
// ktbnLearner.i themselves). Note the pre-template model file is named
// ktbnModel.i, not ktbn.i, to avoid a %include basename collision with this
// very file (wrappers/pyagrum/ktbn.i).
%include "ktbnModel.i"
%include "ktbnInference.i"
%include "ktbnLearner.i"

/* extraction of the API for all wrappers */
%include "aGrUM_wrap_KTBN.i"

// after_templates MUST come after aGrUM_wrap_KTBN.i: %extend on template
// specializations (gum::KTBN<double>, gum::KTBNInference<double>,
// gum::learning::KTBNDatabaseGenerator<double>) requires prior %template.
// KTBN, unlike MRF, needs no pre-template swigsrc file for KTBN/KTBNGenerator/
// the learner classes: no ambiguous overload requiring %rename there, and the
// learner overloads (string,string) vs (string,int,string,int) are
// disambiguated by SWIG on arity alone.
%include "ktbn_after_templates.i"
%include "ktbnInference_after_templates.i"
%include "ktbnDatabaseGenerator_after_templates.i"

%include "gum_functions_KTBN.i"
