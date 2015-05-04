/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of PRMObject.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_PRM_OBJECT_H
#define GUM_PRM_OBJECT_H

#include <string>

#include <agrum/config.h>

namespace gum {

  namespace prm {
    /**
     * @class PRMObject
     *
     * @brief Abstract base class for any element defined in a PRM.
     *
     * This class is a base class for any element defined in a PRM. Since objects
     * in a PRM are differentiated by their names, the only information we need
     * about them is their name.
     *
     * Furthermore we use an enumeration to know the concrete type of a given
     * PRMObject preventing faster type checking.
     *
     * @ingroup prm_group
     */

    class PRMObject {
      public:
      // ==========================================================================
      /// @name Built-in types.
      // ==========================================================================
      /// @{

      /**
       * Enumeration of the different types of objects handled by a PRM.
       * The "all" type is used to tell that we want any kind of Type
       * (useful with iterators for example). No PRMObject will ever have
       * "all" as type.
       */
      enum class PRMType : char {
        ALL,
        CLASS,
        INTERFACE,
        CLASS_ELT,
        TYPE,
        SYSTEM,
        INSTANCE
      };

      static std::string LEFT_CAST() { return "("; }
      static std::string RIGHT_CAST() { return ")"; }

      /// Returns the string representation of a PRMObject.
      static std::string enum2str(PRMType type) {
        switch (type) {
          case PRMType::CLASS:
            return "PRMType::CLASS";

          case PRMType::CLASS_ELT:
            return "PRMType::CLASS_ELT";

          case PRMType::TYPE:
            return "PRMType::TYPE";

          case PRMType::SYSTEM:
            return "PRMType::SYSTEM";

          case PRMType::INSTANCE:
            return "PRMType::INSTANCE";

          case PRMType::INTERFACE:
            return "PRMType::INTERFACE";

          default:
            return "unknown";
        }
      }

      /// Returns true if obj_ptr is of type Class.
      static INLINE bool isClass(const PRMObject &obj) {
        return obj.obj_type() == PRMType::CLASS;
      }

      /// Returns true if obj_ptr is of type Interface.
      static INLINE bool isInterface(const PRMObject &obj) {
        return obj.obj_type() == PRMType::INTERFACE;
      }

      /// Returns true if obj_ptr is of type Instance.
      static INLINE bool isInstance(const PRMObject &obj) {
        return obj.obj_type() == PRMType::INSTANCE;
      }

      /// @}
      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

      /**
       * Constructor.
       * @param name The name of this object.
       */
      PRMObject(const std::string &name);

      /**
       * Copy constructor.
       */
      PRMObject(const PRMObject &source);

      /**
       * Destructor.
       */
      virtual ~PRMObject();

      /// @}
      // ==========================================================================
      /// @name Getters & setters.
      // ==========================================================================
      /// @{

      /**
       * Returns the name of this object.
       */
      const std::string &name() const;

      /**
       * Returns the type of this object.
       */
      virtual PRMType obj_type() const = 0;

      /// @}
      // ==========================================================================
      /// @name Operators
      // ==========================================================================
      /// @{

      /**
       * To PRMObject are equal if they have the same name (which is unique).
       */
      bool operator==(const PRMObject &obj) const;

      /**
       * To PRMObject are equal if they have the same name (which is unique).
       */
      bool operator!=(const PRMObject &obj) const;

      /**
       * Copy operator.
       */
      PRMObject &operator=(const PRMObject &source);

      /// @}
      private:
      // ==========================================================================
      /// @name Private members.
      // ==========================================================================
      /// @{

      // The name of this object
      // ======================================================================
      std::string __name;

      /// @}
    };

    /// For printing Type easily.
    std::ostream &operator<<(std::ostream &out, PRMObject::PRMType obj_type);

    // list of declarations of PRMObjects
    template <typename GUM_SCALAR> class Type;
    template <typename GUM_SCALAR> class ClassElement;
    template <typename GUM_SCALAR> class ClassElementContainer;
    template <typename GUM_SCALAR> class Aggregate;
    template <typename GUM_SCALAR> class Interface;
    template <typename GUM_SCALAR> class Attribute;
    template <typename GUM_SCALAR> class SlotChain;
    template <typename GUM_SCALAR> class ReferenceSlot;
    template <typename GUM_SCALAR> class Class;
    template <typename GUM_SCALAR> class Instance;
    template <typename GUM_SCALAR> class System;

  } /* namespace prm */
} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/PRM/elements/PRMObject.inl>
#endif // GUM_NO_INLINE

#endif /* GUM_PRM_OBJECT_H */
