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
 * @brief Headers of PRM.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_PRM_H
#define GUM_PRM_H

#include <string>

#include <agrum/PRM/elements/PRMFuncAttribute.h>
#include <agrum/PRM/elements/PRMSystem.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    class PRMSystem;
    template < typename GUM_SCALAR >
    class PRMFactory;

    /**
     * @class PRM
     * @headerfile PRM.h <agrum/PRM/PRM.h>
     * @brief This class represents a Probabilistic Relational
     *PRMSystem<GUM_SCALAR>.
     *
     * @ingroup prm_group
     */
    template < typename GUM_SCALAR >
    class PRM {
      public:
      friend class PRMFactory< GUM_SCALAR >;

      // ========================================================================
      /// @name Destructor.
      // ========================================================================
      /// @{

      /**
       * Default constructor.
       */
      PRM();

      /**
       * Destructor.
       */
      ~PRM();

      /// @}
      // ========================================================================
      /// @name Getters and setters.
      // ========================================================================
      /// @{

      /**
       * @param name The name of a possible PRMType in this PRM.
       * @return Returns true if name names a PRMType in this PRM.
       */
      bool isType(const std::string& name) const;

      /**
       * @param name The name of a possible Class<GUM_SCALAR> in this PRM.
       * @return Returns true if name names a Class<GUM_SCALAR> in this PRM.
       */
      bool isClass(const std::string& name) const;

      /**
       * @param name The name of a possible PRMInterface<GUM_SCALAR> in this
       * PRM.
       * @return Returns true if name names a PRMInterface<GUM_SCALAR> in this
       * PRM.
       */
      bool isInterface(const std::string& name) const;

      /**
       * @param name The name of a possible PRMSystem<GUM_SCALAR> in this PRM.
       * @return Returns true if name names a PRMSystem<GUM_SCALAR> in this PRM.
       */
      bool isSystem(const std::string& name) const;

      /**
       * Returns a constant reference on a PRMType given it's name.
       * @throw NotFound Raised if no type is found with the given name.
       */
      PRMType& type(const std::string& name);

      /**
       * Returns a constant reference on a PRMType given it's name.
       * @throw NotFound Raised if no type is found with the given name.
       */
      const PRMType& type(const std::string& name) const;

      /**
       * Returns the Set of all PRMType in this PRM.
       */
      const Set< PRMType* >& types() const;

      /**
       * Returns a constant reference on a Class<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no class is found with the given name.
       */
      PRMClass< GUM_SCALAR >& getClass(const std::string& name);

      /**
       * Returns a constant reference on a Class<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no class is found with the given name.
       */
      const PRMClass< GUM_SCALAR >& getClass(const std::string& name) const;

      /**
       * Returns the Set of all Class<GUM_SCALAR> in this PRM.
       */
      const Set< PRMClass< GUM_SCALAR >* >& classes() const;

      /**
       * Returns a constant reference on a Class<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no class is found with the given name.
       */
      PRMInterface< GUM_SCALAR >& getInterface(const std::string& name);

      /**
       * Returns a constant reference on a Class<GUM_SCALAR> given it's name.
       * @throw NotFound Raised if no class is found with the given name.
       */
      const PRMInterface< GUM_SCALAR >& getInterface(const std::string& name) const;

      /**
       * Returns the Set of all Class<GUM_SCALAR> in this PRM.
       */
      const Set< PRMInterface< GUM_SCALAR >* >& interfaces() const;

      /**
       * Returns a constant reference on a PRMSystem<GUM_SCALAR> given it's
       * name.
       * @throw NotFound Raised if no model is found with the given name.
       */
      PRMSystem< GUM_SCALAR >& getSystem(const std::string& name);

      /**
       * Returns a constant reference on a PRMSystem<GUM_SCALAR> given it's
       * name.
       * @throw NotFound Raised if no model is found with the given name.
       */
      const PRMSystem< GUM_SCALAR >& getSystem(const std::string& name) const;

      /**
       * Returns the Set of all Systems in this PRM.
       */
      const Set< PRMSystem< GUM_SCALAR >* >& systems() const;

      /// @}

      private:
      // ========================================================================
      /// @name Private methods.
      // ========================================================================
      /// @{

      /**
       * Copy constructor. Not Implemented.
       */
      PRM(const PRM< GUM_SCALAR >& source);

      /**
       * Copy operator. Not Implemented.
       */
      PRM< GUM_SCALAR >& operator=(const PRM< GUM_SCALAR >& source);

      /// Add the built-in types in the PRM
      void _addBuiltInTypes_();

      /// @}
      // ========================================================================
      /// @name Private members.
      // ========================================================================
      /// @{

      /// Mapping of all Class<GUM_SCALAR> given their name.
      HashTable< std::string, PRMClass< GUM_SCALAR >* > _classMap_;

      /// Set of all Class<GUM_SCALAR> in this PRM.
      Set< PRMClass< GUM_SCALAR >* > _classes_;

      /// Mapping of all Class<GUM_SCALAR> given their name.
      HashTable< std::string, PRMInterface< GUM_SCALAR >* > _interfaceMap_;

      /// Set of all Class<GUM_SCALAR> in this PRM.
      Set< PRMInterface< GUM_SCALAR >* > _interfaces_;

      /// Mapping of all PRMType given their name.
      HashTable< std::string, PRMType* > _typeMap_;

      /// Set of all PRMType in this PRM.
      Set< PRMType* > _types_;

      /// Mapping of all Systems given their name.
      HashTable< std::string, PRMSystem< GUM_SCALAR >* > _systemMap_;

      /// Set of all Systems in this PRM.
      Set< PRMSystem< GUM_SCALAR >* > _systems_;

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRM< double >;
#endif


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/PRM_tpl.h>

#endif /* GUM_PRM_H */
