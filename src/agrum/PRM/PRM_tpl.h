/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


/**
 * @file
 * @brief Inline implementation of PRM.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/PRM/PRM.h>

namespace gum {
  namespace prm {

    // Default constructor.
    template < typename GUM_SCALAR >
    PRM< GUM_SCALAR >::PRM() {
      GUM_CONSTRUCTOR(PRM);
      addBuiltInTypes__();
    }

    // Destructor.
    template < typename GUM_SCALAR >
    PRM< GUM_SCALAR >::~PRM() {
      GUM_DESTRUCTOR(PRM);
      classMap__.clear();
      typeMap__.clear();
      systemMap__.clear();

      for (const auto sys: systems__)
        delete sys;

      for (const auto cla: classes__)
        delete cla;

      for (const auto inter: interfaces__)
        delete inter;

      for (const auto typ: types__)
        delete typ;
    }

    // Add the built-in types in the PRM
    template < typename GUM_SCALAR >
    void PRM< GUM_SCALAR >::addBuiltInTypes__() {
      LabelizedVariable var("boolean", "built-in type", 0);
      var.addLabel("false");
      var.addLabel("true");
      PRMType* boolean = new PRMType(var);
      types__.insert(boolean);
      typeMap__.insert("boolean", boolean);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isType(const std::string& name) const {
      return typeMap__.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isClass(const std::string& name) const {
      return classMap__.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isInterface(const std::string& name) const {
      return interfaceMap__.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isSystem(const std::string& name) const {
      return systemMap__.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE PRMType& PRM< GUM_SCALAR >::type(const std::string& name) {
      return *(typeMap__[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMType& PRM< GUM_SCALAR >::type(const std::string& name) const {
      return *(typeMap__[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMType* >& PRM< GUM_SCALAR >::types() const {
      return types__;
    }

    template < typename GUM_SCALAR >
    INLINE PRMClass< GUM_SCALAR >&
           PRM< GUM_SCALAR >::getClass(const std::string& name) {
      return *(classMap__[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClass< GUM_SCALAR >&
                 PRM< GUM_SCALAR >::getClass(const std::string& name) const {
      return *(classMap__[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMClass< GUM_SCALAR >* >&
                 PRM< GUM_SCALAR >::classes() const {
      return classes__;
    }

    template < typename GUM_SCALAR >
    INLINE PRMInterface< GUM_SCALAR >&
           PRM< GUM_SCALAR >::getInterface(const std::string& name) {
      return *interfaceMap__[name];
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInterface< GUM_SCALAR >&
                 PRM< GUM_SCALAR >::getInterface(const std::string& name) const {
      return *interfaceMap__[name];
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMInterface< GUM_SCALAR >* >&
                 PRM< GUM_SCALAR >::interfaces() const {
      return interfaces__;
    }

    template < typename GUM_SCALAR >
    INLINE PRMSystem< GUM_SCALAR >&
           PRM< GUM_SCALAR >::getSystem(const std::string& name) {
      return *(systemMap__[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMSystem< GUM_SCALAR >&
                 PRM< GUM_SCALAR >::getSystem(const std::string& name) const {
      return *(systemMap__[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMSystem< GUM_SCALAR >* >&
                 PRM< GUM_SCALAR >::systems() const {
      return systems__;
    }

  } /* namespace prm */
} /* namespace gum */
