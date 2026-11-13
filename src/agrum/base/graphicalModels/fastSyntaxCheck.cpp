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


/** @file
 * @brief Checks a fast description for every fast syntax.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/base/core/exceptions.h>
#include <agrum/base/graphicalModels/fastModel.h>
#include <agrum/base/graphicalModels/fastSyntaxCheck.h>
#include <agrum/base/graphs/diGraph.h>
#include <agrum/base/graphs/fastGraph.h>
#include <agrum/base/graphs/fastMarkedGraph.h>
#include <agrum/base/graphs/markedGraph.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/graphs/undiGraph.h>
#include <agrum/base/variables/allDiscreteVariables.h>

namespace gum {

  std::string toString(FastSyntax syntax) {
    switch (syntax) {
      case FastSyntax::UndiGraph : return "UndiGraph";
      case FastSyntax::DiGraph : return "DiGraph";
      case FastSyntax::MixedGraph : return "MixedGraph";
      case FastSyntax::MarkedGraph : return "MarkedGraph";
      case FastSyntax::BayesNet : return "BayesNet";
      case FastSyntax::InfluenceDiagram : return "InfluenceDiagram";
      case FastSyntax::MarkovRandomField : return "MarkovRandomField";
    }
    GUM_ERROR(InvalidArgument, "unknown fast syntax " << static_cast< int >(syntax))
  }

  std::ostream& operator<<(std::ostream& stream, FastSyntax syntax) {
    return stream << toString(syntax);
  }

  FastSyntaxCheck::FastSyntaxCheck(std::string_view description, std::string_view defaultDomain) :
      _description_(description), _defaultDomain_(defaultDomain) {
    fastVariable< double >("x", defaultDomain);   // checks the default domain
  }

  const std::string& FastSyntaxCheck::_check_(FastSyntax syntax) const {
    const auto i = static_cast< std::size_t >(syntax);
    if (i >= _nbSyntaxes_) GUM_ERROR(OutOfBounds, "unknown fast syntax " << i)
    if (!_errors_[i]) {
      // valid for a syntax = accepted by its fast function; a graph is cheap to build, a model is
      // only analysed (its parameters are not generated)
      using fast::detail::FastModelType;
      using fast::detail::parseFastModel;
      try {
        switch (syntax) {
          case FastSyntax::UndiGraph : fastGraph< UndiGraph >(_description_); break;
          case FastSyntax::DiGraph : fastGraph< DiGraph >(_description_); break;
          case FastSyntax::MixedGraph : fastGraph< MixedGraph >(_description_); break;
          case FastSyntax::MarkedGraph : fastMarkedGraph< MarkedGraph >(_description_); break;
          case FastSyntax::BayesNet :
            parseFastModel(_description_, FastModelType::BayesNet, _defaultDomain_);
            break;
          case FastSyntax::InfluenceDiagram :
            parseFastModel(_description_, FastModelType::InfluenceDiagram, _defaultDomain_);
            break;
          case FastSyntax::MarkovRandomField :
            parseFastModel(_description_, FastModelType::MarkovRandomField, _defaultDomain_);
            break;
        }
        _errors_[i] = std::string();
      } catch (const Exception& e) { _errors_[i] = e.errorType() + ": " + e.errorContent(); }
    }
    return *_errors_[i];
  }

  const std::string& FastSyntaxCheck::description() const { return _description_; }

  bool FastSyntaxCheck::isValid(FastSyntax syntax) const { return _check_(syntax).empty(); }

  const std::string& FastSyntaxCheck::error(FastSyntax syntax) const { return _check_(syntax); }

  std::vector< FastSyntax > FastSyntaxCheck::validSyntaxes() const {
    std::vector< FastSyntax > valid;
    for (std::size_t i = 0; i < _nbSyntaxes_; ++i)
      if (isValid(static_cast< FastSyntax >(i))) valid.push_back(static_cast< FastSyntax >(i));
    return valid;
  }

  FastSyntaxCheck checkFastSyntax(std::string_view description, std::string_view defaultDomain) {
    return FastSyntaxCheck(description, defaultDomain);
  }

}   // namespace gum
