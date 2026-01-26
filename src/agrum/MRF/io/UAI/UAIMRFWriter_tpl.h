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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/MRF/io/UAI/UAIMRFWriter.h>

namespace gum {

  /*
   * Default constructor.
   */
  template < typename GUM_SCALAR >
  INLINE UAIMRFWriter< GUM_SCALAR >::UAIMRFWriter() {
    GUM_CONSTRUCTOR(UAIMRFWriter);
  }

  /*
   * Destructor.
   */
  template < typename GUM_SCALAR >
  INLINE UAIMRFWriter< GUM_SCALAR >::~UAIMRFWriter() {
    GUM_DESTRUCTOR(UAIMRFWriter);
  }

  /*
   * Writes a Markov net in the given output stream.
   *
   * @param output The output stream.
   * @param MRF The Markov net writen in the stream.
   * @throws IOError Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  INLINE void UAIMRFWriter< GUM_SCALAR >::write(std::ostream&                           output,
                                                const IMarkovRandomField< GUM_SCALAR >& MN) {
    if (!output.good()) { GUM_ERROR(IOError, "Input/Output error : stream not writable.") }

    output << _preambule_(MN) << std::endl;

    for (const auto& kv: MN.factors())
      output << _factorBloc_(MN, *kv.second) << std::endl;

    output << std::endl;

    output.flush();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  /*
   * Writes a Markov net in the file referenced by filePath.
   * If the file doesn't exists, it is created.
   * If the file exists, it's content will be erased.
   *
   * @param filePath The path to the file used to write the Markov net.
   * @param MRF The Markov net writen in the file.
   * @throw IOError Raised if an I/O error occurs.
   */
  template < typename GUM_SCALAR >
  INLINE void UAIMRFWriter< GUM_SCALAR >::write(const std::string&                      filePath,
                                                const IMarkovRandomField< GUM_SCALAR >& MN) {
    std::ofstream output(filePath.c_str(), std::ios_base::trunc);

    write(output, MN);

    output.close();

    if (output.fail()) { GUM_ERROR(IOError, "Writing in the ostream failed.") }
  }

  template < typename GUM_SCALAR >
  INLINE std::string
         UAIMRFWriter< GUM_SCALAR >::_preambule_(const IMarkovRandomField< GUM_SCALAR >& MN) {
    std::stringstream str;

    str << "MARKOV" << std::endl;

    str << MN.size() << " # nbr Nodes" << std::endl;

    for (auto node: MN.nodes())
      str << MN.variable(node).domainSize() << " ";
    str << std::endl;

    str << MN.factors().size() << " # nbr Factors " << std::endl;   // number of cliques

    for (const auto& kv: MN.factors()) {
      const auto& nodeset = kv.first;
      str << nodeset.size() << " ";
      for (auto k: nodeset) {
        str << k << " ";
      }
      str << std::endl;
    }

    return str.str();
  }

  template < typename GUM_SCALAR >
  INLINE std::string
         UAIMRFWriter< GUM_SCALAR >::_factorBloc_(const IMarkovRandomField< GUM_SCALAR >& MN,
                                               const Tensor< GUM_SCALAR >&             clikpot) {
    std::stringstream str;

    str << clikpot.domainSize() << " # {";

    for (Idx k = 0; k < clikpot.nbrDim(); k++) {
      str << MN.idFromName(clikpot.variable(k).name());
      if (k == clikpot.nbrDim() - 1) str << "}";
      else str << ", ";
    }
    Instantiation I(clikpot);
    for (I.setFirst(); !I.end(); ++I) {
      if (I.val(0) == 0) str << std::endl << "  ";
      str << clikpot[I] << " ";
    }
    str << std::endl;

    return str.str();
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
