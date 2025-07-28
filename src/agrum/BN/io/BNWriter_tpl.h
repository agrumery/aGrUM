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


/** @file
 * @brief Templates implementation of bns/io/gumBNWriter.h classes.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <regex>

namespace gum {

  /* =========================================================================*/
  /* ===                           GUM_BN_WRITER === */
  /* =========================================================================*/
  template < typename GUM_SCALAR >
  INLINE BNWriter< GUM_SCALAR >::BNWriter() {
    GUM_CONSTRUCTOR(BNWriter);
  }

  template < typename GUM_SCALAR >
  INLINE BNWriter< GUM_SCALAR >::~BNWriter() {
    GUM_DESTRUCTOR(BNWriter);
  }

  template < typename GUM_SCALAR >
  INLINE bool BNWriter< GUM_SCALAR >::isModificationAllowed() const {
    return _allowModification_;
  }

  template < typename GUM_SCALAR >
  INLINE void BNWriter< GUM_SCALAR >::setAllowModification(bool am) {
    _allowModification_ = am;
  }

  template < typename GUM_SCALAR >
  void BNWriter< GUM_SCALAR >::write(std::ostream& output, const IBayesNet< GUM_SCALAR >& bn) {
    _syntacticalCheck(bn);
    _doWrite(output, bn);
  }

  template < typename GUM_SCALAR >
  void BNWriter< GUM_SCALAR >::write(const std::string&             filePath,
                                     const IBayesNet< GUM_SCALAR >& bn) {
    _syntacticalCheck(bn);
    _doWrite(filePath, bn);
  }

  template < typename GUM_SCALAR >
  void BNWriter< GUM_SCALAR >::_syntacticalCheck(const IBayesNet< GUM_SCALAR >& bn) {
    // no check by default
  }

  template < typename GUM_SCALAR >
  void BNWriter< GUM_SCALAR >::_validCharInNamesCheck(const IBayesNet< GUM_SCALAR >& bn) {
    if (_allowModification_)
      return;   // we do anything if the names will be modified when saved ...

    for (const auto& nod: bn.nodes()) {
      auto&       v       = bn.variable(nod);
      std::string valid_n = _buildNameWithOnlyValidChars(v.name());
      if (v.name() != valid_n)
        GUM_ERROR(FatalError,
                  "The variable name '" << v.name() << "' contains invalid characters ('" << valid_n
                                        << "').")
      for (const auto& lab: v.labels()) {
        std::string valid_l = _buildNameWithOnlyValidChars(lab);
        if (lab != valid_l)
          GUM_ERROR(FatalError,
                    "The variable  '" << v << "' contains label '" << lab
                                      << "' with invalid characters ('" << valid_l << "').")
      }
    }
  }

  template < typename GUM_SCALAR >
  std::string BNWriter< GUM_SCALAR >::_onlyValidCharsInName(const std::string& name) {
    if (!_allowModification_)
      return name;   // we do anything if the names will be modified when saved ...
    return _buildNameWithOnlyValidChars(name);
  }

  template < typename GUM_SCALAR >
  std::string BNWriter< GUM_SCALAR >::_buildNameWithOnlyValidChars(const std::string& name) {
    std::string pat = "[^_a-z0-9]+";
    std::regex  reg(pat, std::regex::icase);
    std::smatch sm;

    std::string out = name;
    while (std::regex_search(out, sm, reg)) {
      out = std::regex_replace(out, reg, "_");
    }
    // first char can not be a digit
    if (std::isdigit(out[0]))
      // we allow name containing only an int
      if (out.find_first_not_of("0123456789") != std::string::npos) out = "_" + out;

    return out;
  }
} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
