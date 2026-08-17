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

#include <agrum/BN/io/GUM/GumBNReader.h>   // to ease IDE parser
#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing in IDE
#  include <agrum/base/io/GumBinaryIO.h>
#  include <agrum/base/multidim/ICIModels/multiDimICIModel.h>
#  include <agrum/base/variables/allDiscreteVariables.h>
#  include <agrum/BN/io/GUM/GumBNReader.h>

#  include <agrum/base/external/json/json.hpp>
using json = nlohmann::json;

#  include <unordered_map>

namespace gum {
  template < GUM_Numeric GUM_SCALAR >
  GumBNReader< GUM_SCALAR >::GumBNReader(BayesNet< GUM_SCALAR >* bn,
                                         std::string_view        filename,
                                         bool binary) : BNReader< GUM_SCALAR >(bn, filename) {
    GUM_CONSTRUCTOR(GumBNReader)
    _bn_         = bn;
    _streamName_ = filename;
    _parseDone_  = false;
    _binary_     = binary;
  }

  template < GUM_Numeric GUM_SCALAR >
  GumBNReader< GUM_SCALAR >::GumBNReader(BayesNet< GUM_SCALAR >* bn) :
      BNReader< GUM_SCALAR >(bn, "") {
    GUM_CONSTRUCTOR(GumBNReader)
    _bn_         = bn;
    _streamName_ = "";
    _parseDone_  = false;
    _binary_     = false;
  }

  template < GUM_Numeric GUM_SCALAR >
  GumBNReader< GUM_SCALAR >::~GumBNReader() {
    GUM_DESTRUCTOR(GumBNReader)
  }

  template < GUM_Numeric GUM_SCALAR >
  template < typename JsonType >
  Size GumBNReader< GUM_SCALAR >::_proceedFromJson_(const JsonType& content) {
    Size nberrors = 0;

    if (content.contains("type") && content["type"].template get< std::string >() != "BN") {
      addError("Invalid GUM file format: expected 'BN' type, got '"
                   + content["type"].template get< std::string >() + "'",
               _streamName_,
               0,
               0);
      return ++nberrors;
    }

    if (!content.contains("nodes") || !content.contains("parents") || !content.contains("cpt")) {
      addError("Invalid GUM file format: missing 'nodes', 'parents' or 'cpt' sections",
               _streamName_,
               0,
               0);
      return ++nberrors;
    }

    try {
      BayesNet< GUM_SCALAR > tmp;

      // index cpt entries by node name once: content["cpt"].contains()/.at() are each an O(N)
      // linear scan on nlohmann::ordered_json, so calling them per node (below) would make
      // node reading O(N^2) on the number of nodes.
      const auto&                                        cptSection = content["cpt"];
      std::unordered_map< std::string, const JsonType* > cptByName;
      cptByName.reserve(cptSection.size());
      for (const auto& entry: cptSection.items()) {
        cptByName.emplace(entry.key(), &entry.value());
      }

      // iterate on nodes in json
      for (const auto& node: content["nodes"]) {
        auto var = fastVariable< GUM_SCALAR >(node.template get< std::string >());
        if (var->domainSize() < 2)
          GUM_ERROR(OperationNotAllowed, var->name() << " has a domain size <2")
        const auto& nodeName = var->name();
        const auto  cptIt    = cptByName.find(nodeName);
        if (cptIt == cptByName.end())
          GUM_ERROR(NotFound, "Node '" << nodeName << "' has no entry in the 'cpt' section")
        const auto& cptEntry = *(cptIt->second);

        if (cptEntry.is_object()) {
          if (!cptEntry.contains("kind"))
            GUM_ERROR(NotFound, "Missing 'kind' for node '" << nodeName << "'")
          const auto kind = cptEntry.at("kind").template get< std::string >();
          if (kind == "aggregator") {
            if (!cptEntry.contains("name"))
              GUM_ERROR(NotFound, "Missing 'name' for aggregator node '" << nodeName << "'")
            const auto name  = cptEntry.at("name").template get< std::string >();
            const Idx  value = cptEntry.value("value", Idx(1));
            tmp._addAggregator_(name, *var, value);
          } else if (kind == "ici") {
            if (!cptEntry.contains("name") || !cptEntry.contains("externalWeight"))
              GUM_ERROR(NotFound,
                        "Missing 'name' or 'externalWeight' for ici node '" << nodeName << "'")
            const auto name           = cptEntry.at("name").template get< std::string >();
            const auto externalWeight = cptEntry.at("externalWeight").template get< GUM_SCALAR >();
            tmp._addICIModel_(name, *var, externalWeight);
          } else {
            GUM_ERROR(NotFound, "Unknown cpt kind '" << kind << "' for node " << nodeName)
          }
        } else {
          tmp.add(*var);
        }
      }
      // iterate on parents in json
      for (const auto& parent: content["parents"].items()) {
        const auto& nodeName = parent.key();
        for (const auto& p: parent.value()) {
          tmp.addArc(p.template get< std::string >(), nodeName);
        }
      }
      // iterate on cpt in json
      for (const auto& cpt: content["cpt"].items()) {
        const auto& nodeName = cpt.key();
        const auto& values   = cpt.value();
        if (values.is_object()) {
          if (!values.contains("kind"))
            GUM_ERROR(NotFound, "Missing 'kind' for node '" << nodeName << "'")
          const auto kind = values.at("kind").template get< std::string >();
          if (kind == "ici") {
            const auto* ici = dynamic_cast< const MultiDimICIModel< GUM_SCALAR >* >(
                tmp.cpt(nodeName).content());
            if (ici == nullptr)
              GUM_ERROR(NotFound,
                        "Node " << nodeName << " is tagged as an ICI model but does not hold one")
            if (!values.contains("causalWeights"))
              GUM_ERROR(NotFound, "Missing 'causalWeights' for ici node '" << nodeName << "'")
            for (const auto& w: values.at("causalWeights").items()) {
              ici->causalWeight(tmp.variable(w.key()), w.value().template get< GUM_SCALAR >());
            }
          }
          // aggregator nodes are already fully configured, nothing to fill
        } else {
          tmp.cpt(nodeName).fillWith(values.template get< std::vector< double > >());
        }
      }
      // iterate on properties in json (optional section)
      if (content.contains("properties")) {
        for (const auto& prop: content["properties"].items()) {
          tmp.setProperty(prop.key(), prop.value().template get< std::string >());
        }
      }
      *_bn_       = std::move(tmp);
      _parseDone_ = true;
    } catch (const gum::Exception& e) {
      addError(e.errorContent(), _streamName_, 0, 0);
      return ++nberrors;
    }
    return nberrors;
  }

  template < GUM_Numeric GUM_SCALAR >
  Size GumBNReader< GUM_SCALAR >::proceed() {
    if (_parseDone_) { return 0; }
    if (_streamName_.empty()) {
      GUM_ERROR(OperationNotAllowed,
                "GumBNReader was constructed without a filename: use proceedFromString() instead "
                "of proceed()")
    }
    Size nberrors = 0;

    std::ifstream file(_streamName_, _binary_ ? std::ios::binary : std::ios::in);
    if (!file.is_open()) {
      addException("No such file " + _streamName_, _streamName_);
      return ++nberrors;
    }
    try {
      const auto content
          = _binary_ ? json::from_msgpack(_readVector_(file)) : json::parse(file, nullptr, false);
      file.close();
      if (content.is_discarded()) {
        addException("Error parsing file", _streamName_);
        return ++nberrors;
      }
      return _proceedFromJson_(content);
    } catch (const std::exception& e) {
      addException(std::string("Error reading binary file: ") + e.what(), _streamName_);
      return ++nberrors;
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  Size GumBNReader< GUM_SCALAR >::proceedFromString(std::string_view content) {
    if (_parseDone_) { return 0; }
    const auto j = json::parse(content, nullptr, false);
    if (j.is_discarded()) {
      addException("Invalid JSON string", _streamName_);
      return 1;
    }
    try {
      return _proceedFromJson_(j);
    } catch (const std::exception& e) {
      addException(std::string("Error reading string: ") + e.what(), _streamName_);
      return 1;
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  void GumBNReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& stream) const {
    if (_parseDone_ || count() > 0) elegantErrorsAndWarnings(stream);
    else { GUM_ERROR(OperationNotAllowed, "File not parsed yet") }
  }

  template < GUM_Numeric GUM_SCALAR >
  void GumBNReader< GUM_SCALAR >::showErrorCounts(std::ostream& stream) const {
    if (_parseDone_ || count() > 0) syntheticResults(stream);
    else { GUM_ERROR(OperationNotAllowed, "File not parsed yet") }
  }
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
