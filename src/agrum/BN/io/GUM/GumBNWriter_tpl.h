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
#  include <agrum/BN/io/GUM/GumBNWriter.h>

#  include <agrum/base/external/json/json.hpp>
#  include <agrum/base/io/GumBinaryIO.h>
using json         = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

namespace gum {
  // Default constructor.
  template < GUM_Numeric GUM_SCALAR >
  INLINE GumBNWriter< GUM_SCALAR >::GumBNWriter(bool binary, int indent) :
    BNWriter< GUM_SCALAR >() {
    _binary_ = binary;
    _indent_ = (indent < -1) ? -1 : indent;
    GUM_CONSTRUCTOR(GumBNWriter);
  }

  // Default destructor.
  template < GUM_Numeric GUM_SCALAR >
  INLINE GumBNWriter< GUM_SCALAR >::~GumBNWriter() { GUM_DESTRUCTOR(GumBNWriter); }

  //
  // Writes a Bayesian network in the output stream using the BN format.
  //
  // @param ouput The output stream.
  // @param bn The Bayesian network writen in output.
  // @throws Raised if an I/O error occurs.
  template < GUM_Numeric GUM_SCALAR >
  INLINE void GumBNWriter< GUM_SCALAR >::_doWrite(std::ostream&                  output,
                                                  const IBayesNet< GUM_SCALAR >& bn) {
    if (!output.good()) GUM_ERROR(IOError, "Input/Output error : stream not writable.");

    ordered_json content;
    content["type"]           = "BN";
    content["GumJsonVersion"] = "1.0";

    // add variables (always written, even empty, so the section always exists)
    content["nodes"] = ordered_json::array();
    for (const auto& node: bn.nodes()) { content["nodes"].push_back(bn.variable(node).toFast()); }
    // add parents (always written, even empty, so the section always exists)
    content["parents"] = ordered_json::object();
    for (const auto& node: bn.nodes()) {
      ordered_json parentList = ordered_json::array();
      const auto&  cpt        = bn.cpt(node);
      for (Idx i = 1; i < cpt.nbrDim(); i++)
        parentList.push_back(cpt.variable(i).name());
      content["parents"][bn.variable(node).name()] = parentList;
    }
    // add cpts (always written, even empty, so the section always exists)
    content["cpt"] = ordered_json::object();
    for (const auto& node: bn.nodes()) {
      json          cptValues;
      const auto&   cpt = bn.cpt(node);
      Instantiation I(cpt);
      for (I.setFirst(); !I.end(); ++I) { cptValues.push_back(cpt[I]); }
      content["cpt"][bn.variable(node).name()] = cptValues;
    }
    // add properties
    for (const auto& prop: bn.properties()) { content["properties"][prop] = bn.property(prop); }

    // write the content in the output stream
    if (_binary_) {
      // binary mode
      _writeVector_(output, json::to_msgpack(content));
    } else {
      // text mode
      output << content.dump(_indent_); // pretty print with 2 spaces indentation
    }

    if (output.fail()) {
      GUM_ERROR(IOError, "Writing in the ostream failed. Check if the stream is writable.")
    }
  }

  // Writes a Bayesian network in the referenced file using the BN format.
  // If the file doesn't exists, it is created.
  // If the file exists, it's content will be erased.
  //
  // @param filePath The path to the file used to write the Bayesian network.
  // @param bn The Bayesian network writed in the file.
  // @throws Raised if an I/O error occurs.
  template < GUM_Numeric GUM_SCALAR >
  INLINE void GumBNWriter< GUM_SCALAR >::_doWrite(std::string_view               filePath,
                                                  const IBayesNet< GUM_SCALAR >& bn) {
    std::ofstream output(std::string(filePath),
                         _binary_ ? (std::ios_base::trunc | std::ios::binary) : std::ios_base::trunc);

    _doWrite(output, bn);

    output.close();
    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

} // namespace gum
#endif   // DOXYGEN_SHOULD_SKIP_THIS
