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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing in IDE
#  include <agrum/BN/io/net/netWriter.h>
#endif

#include <agrum/base/external/json/json.hpp>
using json = nlohmann::json;

namespace gum {
  template < typename GUM_SCALAR >
  BNGumReader< GUM_SCALAR >::BNGumReader(BayesNet< GUM_SCALAR >* bn, const std::string& filename) :
      BNReader< GUM_SCALAR >(bn, filename) {
    GUM_CONSTRUCTOR(BNGumReader)
    _bn_         = bn;
    _streamName_ = filename;
    _parseDone_  = false;
  }

  template < typename GUM_SCALAR >
  BNGumReader< GUM_SCALAR >::~BNGumReader() {
    GUM_DESTRUCTOR(BNGumReader)
  }

  template < typename GUM_SCALAR >
  Size BNGumReader< GUM_SCALAR >::proceed() {
    if (_parseDone_) {
      // if the parse is already done, we do not proceed
      return 0;
    }
    Size nberrors = 0;
    // read a json from a file
    std::ifstream file(_streamName_);
    if (!file.is_open()) { addException("No such file " + _streamName_, _streamName_); }
    const auto content = json::parse(file, nullptr, false);

    if (content.is_discarded()) {
      nberrors++;
      addException("Error parsing file", _streamName_);
    }

    file.close();
    if (nberrors > 0) {
      // if there is an error, we do not proceed
      return nberrors;
    }
    auto& bn = *_bn_;
    // check the json content
    if (!content.contains("nodes") || !content.contains("parents") || !content.contains("cpt")) {
      addError("Invalid GUM file format: missing 'nodes', 'parents' or 'cpt' sections",
               _streamName_,
               0,
               0);
      return ++nberrors;
    }

    // iterate on nodes in json
    for (const auto& node: content["nodes"]) {
      bn.add(node.get< std::string >());
    }
    // iterate on parents in json
    for (const auto& parent: content["parents"].items()) {
      const auto& nodeName = parent.key();
      for (const auto& p: parent.value()) {
        const auto& pName = p.get< std::string >();
        bn.addArc(pName, nodeName);
      }
    }
    // iterate on cpt in json
    for (const auto& cpt: content["cpt"].items()) {
      const auto& nodeName = cpt.key();
      const auto& values   = cpt.value();
      bn.cpt(nodeName).fillWith(values.get< std::vector< double > >());
    }
    // iterate on properties in json
    for (const auto& prop: content["properties"].items()) {
      const auto& key   = prop.key();
      const auto& value = prop.value();
      bn.setProperty(key, value.get< std::string >());
    }
    _parseDone_ = true;
    return nberrors;
  }
}   // namespace gum
