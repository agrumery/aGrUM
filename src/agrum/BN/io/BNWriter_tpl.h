/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


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

    std::string pat = "[^_a-z0-9]+";
    std::regex  reg(pat, std::regex::icase);
    std::smatch sm;

    for (const auto& nod: bn.nodes()) {
      auto& v = bn.variable(nod);
      if (std::regex_search(v.name(), sm, reg))
        GUM_ERROR(FatalError,
                  "The variable name '" << v.name() << "' contains invalid characters ('"
                                        << _onlyValidCharsInName_(v.name()) << "').")
      for (const auto& lab: v.labels()) {
        if (std::regex_search(lab, sm, reg))
          GUM_ERROR(FatalError,
                    "The variable  '" << v << "' contains label '" << lab
                                      << "' with invalid characters ('"
                                      << _onlyValidCharsInName_(lab) << "').")
      }
    }
  }

  template < typename GUM_SCALAR >
  std::string BNWriter< GUM_SCALAR >::_onlyValidCharsInName(const std::string& name) {
    if (!_allowModification_)
      return name;   // we do anything if the names will be modified when saved ...
    return _onlyValidCharsInName_(name);
  }

  template < typename GUM_SCALAR >
  std::string BNWriter< GUM_SCALAR >::_onlyValidCharsInName_(const std::string& name) {
    std::string pat = "[^_a-z0-9]+";
    std::regex  reg(pat, std::regex::icase);
    std::smatch sm;

    std::string out = name;
    while (std::regex_search(out, sm, reg)) {
      out = std::regex_replace(out, reg, "_");
    }
    return out;
  }
} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
