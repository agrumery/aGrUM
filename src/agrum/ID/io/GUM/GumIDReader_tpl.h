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


#pragma once

#include <agrum/ID/io/GUM/GumIDReader.h>   // to ease IDE parser
#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing in IDE
#  include <agrum/base/io/GumBinaryIO.h>
#  include <agrum/ID/io/GUM/GumIDReader.h>

#  include <agrum/base/external/json/json.hpp>
using json = nlohmann::json;

namespace gum {
  template < GUM_Numeric GUM_SCALAR >
  GumIDReader< GUM_SCALAR >::GumIDReader(InfluenceDiagram< GUM_SCALAR >* id,
                                         std::string_view                filename,
                                         bool binary) : IDReader< GUM_SCALAR >(id, filename) {
    GUM_CONSTRUCTOR(GumIDReader)
    _id_         = id;
    _streamName_ = filename;
    _parseDone_  = false;
    _binary_     = binary;
  }

  template < GUM_Numeric GUM_SCALAR >
  GumIDReader< GUM_SCALAR >::GumIDReader(InfluenceDiagram< GUM_SCALAR >* id) :
      IDReader< GUM_SCALAR >(id, "") {
    GUM_CONSTRUCTOR(GumIDReader)
    _id_         = id;
    _streamName_ = "";
    _parseDone_  = false;
    _binary_     = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  GumIDReader< GUM_SCALAR >::~GumIDReader() {
    GUM_DESTRUCTOR(GumIDReader)
  }

  template < GUM_Numeric GUM_SCALAR >
  template < typename JsonType >
  Size GumIDReader< GUM_SCALAR >::_proceedFromJson_(const JsonType& content) {
    Size nberrors = 0;

    if (content.contains("type") && content["type"].template get< std::string >() != "ID") {
      addError("Invalid GUM file format: expected 'ID' type, got '"
                   + content["type"].template get< std::string >() + "'",
               _streamName_,
               0,
               0);
      return ++nberrors;
    }

    if (!content.contains("chance") || !content.contains("utility") || !content.contains("decision")
        || !content.contains("parents")) {
      addError(
          "Invalid GUM file format: missing 'chance', 'utility', 'decision' or 'parents' sections",
          _streamName_,
          0,
          0);
      return ++nberrors;
    }

    auto& id = *_id_;

    // add chance nodes
    for (const auto& node: content["chance"]) {
      id.addChanceNode(node.template get< std::string >());
    }
    // add utility nodes
    for (const auto& node: content["utility"]) {
      id.addUtilityNode(node.template get< std::string >());
    }
    // add decision nodes
    for (const auto& node: content["decision"]) {
      id.addDecisionNode(node.template get< std::string >());
    }

    // add arcs
    id.beginTopologyTransformation();
    for (const auto& entry: content["parents"].items()) {
      const auto& nodeName = entry.key();
      for (const auto& p: entry.value()) {
        id.addArc(p.template get< std::string >(), nodeName);
      }
    }
    id.endTopologyTransformation();

    // fill CPTs for chance nodes
    if (content.contains("cpt")) {
      for (const auto& entry: content["cpt"].items()) {
        id.cpt(entry.key()).fillWith(entry.value().template get< std::vector< double > >());
      }
    }

    // fill utility tables
    if (content.contains("reward")) {
      for (const auto& entry: content["reward"].items()) {
        id.utility(entry.key()).fillWith(entry.value().template get< std::vector< double > >());
      }
    }

    // properties
    if (content.contains("properties")) {
      for (const auto& prop: content["properties"].items()) {
        id.setProperty(prop.key(), prop.value().template get< std::string >());
      }
    }

    _parseDone_ = true;
    return nberrors;
  }

  template < GUM_Numeric GUM_SCALAR >
  Size GumIDReader< GUM_SCALAR >::proceed() {
    if (_parseDone_) { return 0; }
    if (_streamName_.empty()) {
      GUM_ERROR(OperationNotAllowed,
                "GumIDReader was constructed without a filename: use proceedFromString() instead "
                "of proceed()")
    }
    Size nberrors = 0;

    std::ifstream file(_streamName_, _binary_ ? std::ios::binary : std::ios::in);
    if (!file.is_open()) {
      addException("No such file " + _streamName_, _streamName_);
      return ++nberrors;
    }

    const auto content
        = _binary_ ? json::from_msgpack(_readVector_(file)) : json::parse(file, nullptr, false);
    file.close();

    if (content.is_discarded()) {
      addException("Error parsing file", _streamName_);
      return ++nberrors;
    }
    return _proceedFromJson_(content);
  }

  template < GUM_Numeric GUM_SCALAR >
  Size GumIDReader< GUM_SCALAR >::proceedFromString(std::string_view content) {
    if (_parseDone_) { return 0; }
    const auto j = json::parse(content, nullptr, false);
    if (j.is_discarded()) {
      addException("Invalid JSON string", _streamName_);
      return 1;
    }
    return _proceedFromJson_(j);
  }

  template < GUM_Numeric GUM_SCALAR >
  void GumIDReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& stream) const {
    if (_parseDone_ || count() > 0) {
      elegantErrorsAndWarnings(stream);
    } else {
      GUM_ERROR(OperationNotAllowed, "File not parsed yet")
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void GumIDReader< GUM_SCALAR >::showErrorCounts(std::ostream& stream) const {
    if (_parseDone_ || count() > 0) {
      syntheticResults(stream);
    } else {
      GUM_ERROR(OperationNotAllowed, "File not parsed yet")
    }
  }
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
