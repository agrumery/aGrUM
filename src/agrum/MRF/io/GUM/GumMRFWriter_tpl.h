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

#include <agrum/MRF/io/GUM/GumMRFWriter.h>   // to ease IDE parser
#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing in IDE
#  include <agrum/base/io/GumBinaryIO.h>
#  include <agrum/MRF/io/GUM/GumMRFWriter.h>

#  include <agrum/base/external/json/json.hpp>
using json         = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

namespace gum {
  template < GUM_Numeric GUM_SCALAR >
  INLINE GumMRFWriter< GUM_SCALAR >::GumMRFWriter(bool binary, int indent) :
      MRFWriter< GUM_SCALAR >() {
    _binary_ = binary;
    _indent_ = (indent < -1) ? -1 : indent;
    GUM_CONSTRUCTOR(GumMRFWriter);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE GumMRFWriter< GUM_SCALAR >::~GumMRFWriter() {
    GUM_DESTRUCTOR(GumMRFWriter);
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void GumMRFWriter< GUM_SCALAR >::write(std::ostream&                           output,
                                                const IMarkovRandomField< GUM_SCALAR >& mrf) {
    if (!output.good()) GUM_ERROR(IOError, "Input/Output error : stream not writable.")

    ordered_json content;
    content["type"]           = "MRF";
    content["GumJsonVersion"] = "1.0";

    // nodes (always written, even if empty)
    content["nodes"] = ordered_json::array();
    for (const auto& node: mrf.nodes()) {
      content["nodes"].push_back(mrf.variable(node).toFast());
    }

    // factors: each factor as an inline object {"vars": [...], "values": [...]}
    // always written, even if empty, so the section is always present
    content["factors"] = ordered_json::array();
    for (const auto& [nodeSet, tensor_ptr]: mrf.factors()) {
      ordered_json factor;
      // variable names in tensor axis order (deterministic, matches fillWith order)
      for (Idx i = 0; i < tensor_ptr->nbrDim(); i++) {
        factor["vars"].push_back(tensor_ptr->variable(i).name());
      }
      // flattened values
      json          vals;
      Instantiation I(*tensor_ptr);
      for (I.setFirst(); !I.end(); ++I) {
        vals.push_back((*tensor_ptr)[I]);
      }
      factor["values"] = vals;
      content["factors"].push_back(factor);
    }

    // properties
    for (const auto& prop: mrf.properties()) {
      content["properties"][prop] = mrf.property(prop);
    }

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
  INLINE void GumMRFWriter< GUM_SCALAR >::write(std::string_view                  filePath,
                                                IMarkovRandomField< GUM_SCALAR >& mrf) {
    mrf.updateMetaData();
    write(filePath, static_cast< const IMarkovRandomField< GUM_SCALAR >& >(mrf));
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE void GumMRFWriter< GUM_SCALAR >::write(std::string_view                        filePath,
                                                const IMarkovRandomField< GUM_SCALAR >& mrf) {
    std::ofstream output(std::string(filePath),
                         _binary_ ? (std::ios_base::trunc | std::ios::binary)
                                  : std::ios_base::trunc);
    write(output, mrf);
    output.close();
    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE std::string
         GumMRFWriter< GUM_SCALAR >::toString(const IMarkovRandomField< GUM_SCALAR >& mrf) {
    std::ostringstream oss;
    write(oss, mrf);
    return oss.str();
  }

}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
