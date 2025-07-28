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


/**
 * @file
 * @brief Inline implementation of PRM.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/PRM/PRM.h>

namespace gum {
  namespace prm {

    // Default constructor.
    template < typename GUM_SCALAR >
    PRM< GUM_SCALAR >::PRM() {
      GUM_CONSTRUCTOR(PRM);
      _addBuiltInTypes_();
    }

    // Destructor.
    template < typename GUM_SCALAR >
    PRM< GUM_SCALAR >::~PRM() {
      GUM_DESTRUCTOR(PRM);
      _classMap_.clear();
      _typeMap_.clear();
      _systemMap_.clear();

      for (const auto sys: _systems_)
        delete sys;

      for (const auto cla: _classes_)
        delete cla;

      for (const auto inter: _interfaces_)
        delete inter;

      for (const auto typ: _types_)
        delete typ;
    }

    // Add the built-in types in the PRM
    template < typename GUM_SCALAR >
    void PRM< GUM_SCALAR >::_addBuiltInTypes_() {
      LabelizedVariable var("boolean", "built-in type", 0);
      var.addLabel("false");
      var.addLabel("true");
      PRMType* boolean = new PRMType(var);
      _types_.insert(boolean);
      _typeMap_.insert("boolean", boolean);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isType(const std::string& name) const {
      return _typeMap_.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isClass(const std::string& name) const {
      return _classMap_.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isInterface(const std::string& name) const {
      return _interfaceMap_.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isSystem(const std::string& name) const {
      return _systemMap_.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE PRMType& PRM< GUM_SCALAR >::type(const std::string& name) {
      return *(_typeMap_[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMType& PRM< GUM_SCALAR >::type(const std::string& name) const {
      return *(_typeMap_[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMType* >& PRM< GUM_SCALAR >::types() const {
      return _types_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMClass< GUM_SCALAR >& PRM< GUM_SCALAR >::getClass(const std::string& name) {
      return *(_classMap_[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClass< GUM_SCALAR >&
                 PRM< GUM_SCALAR >::getClass(const std::string& name) const {
      return *(_classMap_[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMClass< GUM_SCALAR >* >& PRM< GUM_SCALAR >::classes() const {
      return _classes_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMInterface< GUM_SCALAR >& PRM< GUM_SCALAR >::getInterface(const std::string& name) {
      return *_interfaceMap_[name];
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInterface< GUM_SCALAR >&
                 PRM< GUM_SCALAR >::getInterface(const std::string& name) const {
      return *_interfaceMap_[name];
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMInterface< GUM_SCALAR >* >& PRM< GUM_SCALAR >::interfaces() const {
      return _interfaces_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMSystem< GUM_SCALAR >& PRM< GUM_SCALAR >::getSystem(const std::string& name) {
      return *(_systemMap_[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMSystem< GUM_SCALAR >&
                 PRM< GUM_SCALAR >::getSystem(const std::string& name) const {
      return *(_systemMap_[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMSystem< GUM_SCALAR >* >& PRM< GUM_SCALAR >::systems() const {
      return _systems_;
    }

  } /* namespace prm */
} /* namespace gum */
