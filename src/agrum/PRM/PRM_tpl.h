
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/PRM.h>

namespace gum {
  namespace prm {

    // Default constructor.
    template < typename GUM_SCALAR >
    PRM< GUM_SCALAR >::PRM() {
      GUM_CONSTRUCTOR(PRM);
      __addBuiltInTypes();
    }

    // Destructor.
    template < typename GUM_SCALAR >
    PRM< GUM_SCALAR >::~PRM() {
      GUM_DESTRUCTOR(PRM);
      __classMap.clear();
      __typeMap.clear();
      __systemMap.clear();

      for (const auto sys : __systems)
        delete sys;

      for (const auto cla : __classes)
        delete cla;

      for (const auto inter : __interfaces)
        delete inter;

      for (const auto typ : __types)
        delete typ;
    }

    // Add the built-in types in the PRM
    template < typename GUM_SCALAR >
    void PRM< GUM_SCALAR >::__addBuiltInTypes() {
      LabelizedVariable var("boolean", "built-in type", 0);
      var.addLabel("false");
      var.addLabel("true");
      PRMType* boolean = new PRMType(var);
      __types.insert(boolean);
      __typeMap.insert("boolean", boolean);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isType(const std::string& name) const {
      return __typeMap.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isClass(const std::string& name) const {
      return __classMap.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isInterface(const std::string& name) const {
      return __interfaceMap.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRM< GUM_SCALAR >::isSystem(const std::string& name) const {
      return __systemMap.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE PRMType& PRM< GUM_SCALAR >::type(const std::string& name) {
      return *(__typeMap[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMType& PRM< GUM_SCALAR >::type(const std::string& name) const {
      return *(__typeMap[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMType* >& PRM< GUM_SCALAR >::types() const {
      return __types;
    }

    template < typename GUM_SCALAR >
    INLINE PRMClass< GUM_SCALAR >&
           PRM< GUM_SCALAR >::getClass(const std::string& name) {
      return *(__classMap[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClass< GUM_SCALAR >&
                 PRM< GUM_SCALAR >::getClass(const std::string& name) const {
      return *(__classMap[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMClass< GUM_SCALAR >* >&
                 PRM< GUM_SCALAR >::classes() const {
      return __classes;
    }

    template < typename GUM_SCALAR >
    INLINE PRMInterface< GUM_SCALAR >&
           PRM< GUM_SCALAR >::getInterface(const std::string& name) {
      return *__interfaceMap[name];
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInterface< GUM_SCALAR >&
                 PRM< GUM_SCALAR >::getInterface(const std::string& name) const {
      return *__interfaceMap[name];
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMInterface< GUM_SCALAR >* >&
                 PRM< GUM_SCALAR >::interfaces() const {
      return __interfaces;
    }

    template < typename GUM_SCALAR >
    INLINE PRMSystem< GUM_SCALAR >&
           PRM< GUM_SCALAR >::getSystem(const std::string& name) {
      return *(__systemMap[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMSystem< GUM_SCALAR >&
                 PRM< GUM_SCALAR >::getSystem(const std::string& name) const {
      return *(__systemMap[name]);
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMSystem< GUM_SCALAR >* >&
                 PRM< GUM_SCALAR >::systems() const {
      return __systems;
    }

  } /* namespace prm */
} /* namespace gum */
