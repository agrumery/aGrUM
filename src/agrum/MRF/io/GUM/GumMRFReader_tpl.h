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
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3imit.html)    *
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
#  include <agrum/MRF/io/GUM/GumMRFReader.h>

#  include <agrum/base/external/json/json.hpp>
using json = nlohmann::json;

namespace gum {
  template < GUM_Numeric GUM_SCALAR >
  GumMRFReader< GUM_SCALAR >::GumMRFReader(MarkovRandomField< GUM_SCALAR >* mrf,
                                           std::string_view                 filename,
                                           bool                             binary) :
      MRFReader< GUM_SCALAR >(mrf, filename) {
    GUM_CONSTRUCTOR(GumMRFReader)
    _mrf_        = mrf;
    _streamName_ = filename;
    _parseDone_  = false;
    _binary_     = binary;
  }

  template < GUM_Numeric GUM_SCALAR >
  GumMRFReader< GUM_SCALAR >::~GumMRFReader() {
    GUM_DESTRUCTOR(GumMRFReader)
  }

  template < GUM_Numeric GUM_SCALAR >
  Size GumMRFReader< GUM_SCALAR >::proceed() {
    if (_parseDone_) { return 0; }

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

    if (content.contains("type") && content["type"].get< std::string >() != "MRF") {
      addError("Invalid GUM file format: expected 'MRF' type, got '"
                   + content["type"].get< std::string >() + "'",
               _streamName_,
               0,
               0);
      return ++nberrors;
    }

    if (!content.contains("nodes") || !content.contains("factors")) {
      addError("Invalid GUM file format: missing 'nodes' or 'factors' sections",
               _streamName_,
               0,
               0);
      return ++nberrors;
    }

    auto& mrf = *_mrf_;

    // add nodes
    for (const auto& node: content["nodes"]) { mrf.add(node.get< std::string >()); }

    // add factor topology
    mrf.beginTopologyTransformation();
    for (const auto& factor: content["factors"]) {
      mrf.addFactor(factor["vars"].get< std::vector< std::string > >());
    }
    mrf.endTopologyTransformation();

    // fill factor values
    for (const auto& factor: content["factors"]) {
      const auto varnames = factor["vars"].get< std::vector< std::string > >();
      mrf.factor(varnames).fillWith(factor["values"].get< std::vector< double > >());
    }

    // properties
    if (content.contains("properties")) {
      for (const auto& prop: content["properties"].items()) {
        mrf.setProperty(prop.key(), prop.value().get< std::string >());
      }
    }

    _parseDone_ = true;
    return nberrors;
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE std::vector< uint8_t > GumMRFReader< GUM_SCALAR >::_readVector_(std::istream& is) {
    uint64_t size = 0;
    is.read(reinterpret_cast< char* >(&size), sizeof(size));
    std::vector< uint8_t > vec(size);
    is.read(reinterpret_cast< char* >(vec.data()), size);
    return vec;
  }

  template < GUM_Numeric GUM_SCALAR >
  void GumMRFReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& stream) const {
    if (_parseDone_) {
      elegantErrorsAndWarnings(stream);
    } else {
      GUM_ERROR(OperationNotAllowed, "File not parsed yet")
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void GumMRFReader< GUM_SCALAR >::showErrorCounts(std::ostream& stream) const {
    if (_parseDone_) {
      syntheticResults(stream);
    } else {
      GUM_ERROR(OperationNotAllowed, "File not parsed yet")
    }
  }
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
