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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing in IDE
#  include <agrum/ID/io/GUM/GumIDWriter.h>

#  include <agrum/base/external/json/json.hpp>
using json         = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

namespace gum {
  template < GUM_Numeric GUM_SCALAR >
  INLINE GumIDWriter< GUM_SCALAR >::GumIDWriter(bool binary, int indent) :
      IDWriter< GUM_SCALAR >() {
    _binary_ = binary;
    _indent_ = (indent < -1) ? -1 : indent;
    GUM_CONSTRUCTOR(GumIDWriter);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE GumIDWriter< GUM_SCALAR >::~GumIDWriter() {
    GUM_DESTRUCTOR(GumIDWriter);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void GumIDWriter< GUM_SCALAR >::write(std::ostream&                       output,
                                               const InfluenceDiagram< GUM_SCALAR >& id) {
    if (!output.good()) GUM_ERROR(IOError, "Input/Output error : stream not writable.")

    ordered_json content;
    content["type"]           = "ID";
    content["GumJsonVersion"] = "1.0";

    // classify nodes by type
    for (const auto& node: id.nodes()) {
      const auto fast = id.variable(node).toFast();
      if (id.isChanceNode(node)) {
        content["chance"].push_back(fast);
      } else if (id.isUtilityNode(node)) {
        content["utility"].push_back(fast);
      } else {
        content["decision"].push_back(fast);
      }
    }

    // parents for every node (all types)
    for (const auto& node: id.nodes()) {
      ordered_json  parentList;
      const auto&   name = id.variable(node).name();
      if (id.isChanceNode(node)) {
        const auto& cpt = id.cpt(node);
        for (Idx i = 1; i < cpt.nbrDim(); i++) parentList.push_back(cpt.variable(i).name());
      } else if (id.isUtilityNode(node)) {
        const auto& ut = id.utility(node);
        for (Idx i = 1; i < ut.nbrDim(); i++) parentList.push_back(ut.variable(i).name());
      } else {
        // decision node: parents from the DAG
        for (const auto& p: id.parents(node)) parentList.push_back(id.variable(p).name());
      }
      content["parents"][name] = parentList;
    }

    // CPT values for chance nodes
    for (const auto& node: id.nodes()) {
      if (!id.isChanceNode(node)) continue;
      json          vals;
      const auto&   cpt = id.cpt(node);
      Instantiation I(cpt);
      for (I.setFirst(); !I.end(); ++I) vals.push_back(cpt[I]);
      content["cpt"][id.variable(node).name()] = vals;
    }

    // reward values for utility nodes
    for (const auto& node: id.nodes()) {
      if (!id.isUtilityNode(node)) continue;
      json          vals;
      const auto&   ut = id.utility(node);
      Instantiation I(ut);
      for (I.setFirst(); !I.end(); ++I) vals.push_back(ut[I]);
      content["reward"][id.variable(node).name()] = vals;
    }

    // properties
    for (const auto& prop: id.properties()) content["properties"][prop] = id.property(prop);

    if (_binary_) {
      _writeVector_(output, json::to_msgpack(content));
    } else {
      output << content.dump(_indent_);
    }

    if (output.fail()) {
      GUM_ERROR(IOError, "Writing in the ostream failed. Check if the stream is writable.")
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void GumIDWriter< GUM_SCALAR >::write(std::string_view                      filePath,
                                               InfluenceDiagram< GUM_SCALAR >&       id) {
    id.updateMetaData();
    write(filePath, static_cast< const InfluenceDiagram< GUM_SCALAR >& >(id));
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void GumIDWriter< GUM_SCALAR >::write(std::string_view                      filePath,
                                               const InfluenceDiagram< GUM_SCALAR >& id) {
    std::ofstream output(std::string(filePath), std::ios_base::trunc);
    write(output, id);
    output.close();
    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE std::string GumIDWriter< GUM_SCALAR >::toString(const InfluenceDiagram< GUM_SCALAR >& id) {
    std::ostringstream oss;
    write(oss, id);
    return oss.str();
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void GumIDWriter< GUM_SCALAR >::_writeVector_(std::ostream&                 os,
                                                       const std::vector< uint8_t >& vec) {
    uint64_t size = vec.size();
    os.write(reinterpret_cast< const char* >(&size), sizeof(size));
    os.write(reinterpret_cast< const char* >(vec.data()), size);
  }
} // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
