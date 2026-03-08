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

// causalImpact, counterfactual and counterfactualModel are renamed with a leading
// underscore so that the %pythoncode wrappers below can expose them with explicit
// keyword arguments and type coercion (str→set for on/doing/knowing).
// %feature("kwargs") is NOT used here: it causes SIGSEGV on inline-constructed
// temporaries because the **kwargs dispatch can drop the refcount of arguments
// (e.g. gum.CausalModel(edex)) before the C++ call completes.
%rename(_causalImpact) gum::causalImpact;
%rename(_counterfactual) gum::counterfactual;
%rename(_counterfactualModel) gum::counterfactualModel;

// LatentDescriptorVector and LatentDescriptorIds are C++-internal types with no
// Python representation. Ignoring them causes SWIG to suppress all overloads
// that use them (including the LatentDescriptorVector constructors of CausalModel),
// keeping the Python error messages free of internal C++ type names.
%ignore gum::LatentDescriptorIds;
%ignore gum::LatentDescriptorVector;


%pythoncode %{
def causalImpact(cm, on, doing, knowing=None, values=None):
    if isinstance(on, str):
        on = {on}
    if isinstance(doing, str):
        doing = {doing}
    if isinstance(knowing, str):
        knowing = {knowing}
    lat, pot, expl = _causalImpact(cm, on, doing,
                                   knowing if knowing is not None else set(),
                                   values  if values  is not None else {})
    pot._model = cm
    return lat, (pot if lat.isIdentified() else None), expl

def counterfactual(cm, on, whatif, profile=None, values=None):
    p=_counterfactual(cm, on, whatif,
                           profile if profile is not None else {},
                           values  if values  is not None else {})
    p._model=cm
    return p

def counterfactualModel(cm, profile=None, whatif=None):
    p=_counterfactualModel(cm,
                           profile if profile is not None else {},
                           whatif  if whatif  is not None else set())
    p._model=cm
    return p

%}
