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
#  include <agrum/BN/io/GUM/BNGumWriter.h>
#include <agrum/base/external/json/json.hpp>
using json = nlohmann::json;

namespace gum {
  // Default constructor.
  template <typename GUM_SCALAR>
  INLINE BNGumWriter< GUM_SCALAR >::BNGumWriter() { GUM_CONSTRUCTOR(BNGumWriter); }

  // Default destructor.
  template <typename GUM_SCALAR>
  INLINE BNGumWriter< GUM_SCALAR >::~BNGumWriter() { GUM_DESTRUCTOR(BNGumWriter); }

  //
  // Writes a Bayesian network in the output stream using the BN format.
  //
  // @param ouput The output stream.
  // @param bn The Bayesian network writen in output.
  // @throws Raised if an I/O error occurs.
  template <typename GUM_SCALAR>
  INLINE void BNGumWriter< GUM_SCALAR >::_doWrite(std::ostream&                  output,
                                                  const IBayesNet< GUM_SCALAR >& bn) {
    if (!output.good()) GUM_ERROR(IOError, "Input/Output error : stream not writable.");

    json content;

    content["type"]             = "Bayesian Network";
    content["GumFormatVersion"] = "1.0";

    // add variables
    for (const auto& node: bn.nodes()) { content["nodes"].push_back(bn.variable(node).toFast()); }
    // add parents
    for (const auto& node: bn.nodes()) {
      json parentList;
      // dans l'orde des variables dans la GUM_I_CPT_GENERATOR_H
      const auto& cpt = bn.cpt(node);
      for (Idx i = 1; i < cpt.domainSize(); i++)
        parentList.push_back(cpt.variable(i).toFast());
      content["parents"][bn.variable(node).name()] = parentList;
    }
    // add cpts
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
    output << content.dump(2); // pretty print with 2 spaces indentation
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
  template <typename GUM_SCALAR>
  INLINE void BNGumWriter< GUM_SCALAR >::_doWrite(const std::string&             filePath,
                                                  const IBayesNet< GUM_SCALAR >& bn) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);

    _doWrite(output, bn);

    output.close();
    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }
}
#endif   // DOXYGEN_SHOULD_SKIP_THIS
