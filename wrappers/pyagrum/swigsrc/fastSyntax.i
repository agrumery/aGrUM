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

// Python side of the fast syntax tools (base/core/fastSyntax.h, base/graphicalModels/fastSyntaxCheck.h)

// the names of the syntaxes are given by pyagrum.FastSyntax (an IntEnum)
%ignore gum::toString(FastSyntax);
%ignore gum::operator<<(std::ostream&, FastSyntax);
// a list of pyagrum.FastSyntax (see below)
%ignore gum::FastSyntaxCheck::validSyntaxes;

%extend gum::FastSyntaxCheck {
%pythoncode %{
  def validSyntaxes(self) -> list["FastSyntax"]:
    """
    The syntaxes for which the description is valid.

    Returns
    -------
    list[pyagrum.FastSyntax]
      the syntaxes, in the order of `pyagrum.FastSyntax`
    """
    return [syntax for syntax in FastSyntax if self.isValid(syntax)]
%}
  // valid for at least one syntax (the syntaxes are checked until the first valid one)
  bool __bool__() const {
    for (std::size_t i = 0; i <= static_cast< std::size_t >(gum::FastSyntax::MarkovRandomField); ++i)
      if (self->isValid(static_cast< gum::FastSyntax >(i))) return true;
    return false;
  }
  // the description and its valid syntaxes, e.g. (pyagrum.FastSyntaxCheck) 'A->B': DiGraph, ...
  std::string __repr__() const {
    std::string valid;
    for (const auto syntax: self->validSyntaxes())
      valid += (valid.empty() ? "" : ", ") + gum::toString(syntax);
    return "(pyagrum.FastSyntaxCheck) '" + self->description() + "': " + (valid.empty() ? "invalid" : valid);
  }
}

%pythoncode %{
import enum as _enum


class FastSyntax(_enum.IntEnum):
  """
  The fast syntaxes: the kinds of objects that a fast description can build (see `pyagrum.checkFastSyntax`).
  """

  # the values of the C++ enumeration gum::FastSyntax
  UndiGraph = _pyagrumcpp.FastSyntax_UndiGraph
  DiGraph = _pyagrumcpp.FastSyntax_DiGraph
  MixedGraph = _pyagrumcpp.FastSyntax_MixedGraph
  MarkedGraph = _pyagrumcpp.FastSyntax_MarkedGraph
  BayesNet = _pyagrumcpp.FastSyntax_BayesNet
  InfluenceDiagram = _pyagrumcpp.FastSyntax_InfluenceDiagram
  MarkovRandomField = _pyagrumcpp.FastSyntax_MarkovRandomField
%}
