/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Headers of PRMObject.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_PRM_OBJECT_H
#define GUM_PRM_OBJECT_H

#include <string>

#include <agrum/agrum.h>

namespace gum {

  namespace prm {
    /**
     * @class PRMObject
     *
     * @brief Abstract base class for any element defined in a PRM.
     *
     * This class is a base class for any element defined in a PRM. Since
     *objects
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
       * The "all" type is used to tell that we want any kind of PRMType
       * (useful with iterators for example). No PRMObject will ever have
       * "all" as type.
       */
      enum class prm_type : char {
        ALL,
        CLASS,
        PRM_INTERFACE,
        CLASS_ELT,
        TYPE,
        SYSTEM,
        INSTANCE
      };

      static std::string LEFT_CAST() { return "("; }
      static std::string RIGHT_CAST() { return ")"; }

      /// Returns the string representation of a PRMObject.
      static std::string enum2str(prm_type type) {
        switch (type) {
          case prm_type::CLASS: return "PRMType::CLASS";

          case prm_type::CLASS_ELT: return "PRMType::CLASS_ELT";

          case prm_type::TYPE: return "PRMType::TYPE";

          case prm_type::SYSTEM: return "PRMType::SYSTEM";

          case prm_type::INSTANCE: return "PRMType::INSTANCE";

          case prm_type::PRM_INTERFACE: return "PRMType::PRM_INTERFACE";

          default: return "unknown";
        }
      }

      /// Returns true if obj_ptr is of type Class.
      static INLINE bool isClass(const PRMObject& obj) { return obj.obj_type() == prm_type::CLASS; }

      /// Returns true if obj_ptr is of type PRMInterface.
      static INLINE bool isInterface(const PRMObject& obj) {
        return obj.obj_type() == prm_type::PRM_INTERFACE;
      }

      /// Returns true if obj_ptr is of type PRMInstance.
      static INLINE bool isInstance(const PRMObject& obj) {
        return obj.obj_type() == prm_type::INSTANCE;
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
      explicit PRMObject(const std::string& name);

      /**
       * Copy constructor.
       */
      PRMObject(const PRMObject& source);

      /**
       * Move constructor.
       */
      PRMObject(PRMObject&& source);

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
      const std::string& name() const;

      /**
       * @brief Change the name of the PRM Object.
       * @warning Don't do this unless you know what you are doing !
       */
      void name(const std::string& name);

      /**
       * Returns the type of this object.
       */
      virtual prm_type obj_type() const = 0;

      /// @}
      // ==========================================================================
      /// @name Operators
      // ==========================================================================
      /// @{

      /**
       * To PRMObject are equal if they have the same name (which is unique).
       */
      bool operator==(const PRMObject& obj) const;

      /**
       * To PRMObject are equal if they have the same name (which is unique).
       */
      bool operator!=(const PRMObject& obj) const;

      /**
       * Copy operator.
       */
      PRMObject& operator=(const PRMObject& source);

      /**
       * Move operator.
       */
      PRMObject& operator=(PRMObject&& source);

      /// @}
      private:
      // ==========================================================================
      /// @name Private members.
      // ==========================================================================
      /// @{

      // The name of this object
      // ======================================================================
      std::string _name_;

      /// @}
    };

    /// For printing PRMType easily.
    std::ostream& operator<<(std::ostream& out, PRMObject::prm_type obj_type);

    // list of declarations of PRMObjects
    class PRMType;
    template < typename GUM_SCALAR >
    class PRMClassElement;
    template < typename GUM_SCALAR >
    class PRMClassElementContainer;
    template < typename GUM_SCALAR >
    class PRMAggregate;
    template < typename GUM_SCALAR >
    class PRMInterface;
    template < typename GUM_SCALAR >
    class PRMAttribute;
    template < typename GUM_SCALAR >
    class PRMSlotChain;
    template < typename GUM_SCALAR >
    class PRMReferenceSlot;
    template < typename GUM_SCALAR >
    class PRMClass;
    template < typename GUM_SCALAR >
    class PRMInstance;
    template < typename GUM_SCALAR >
    class PRMSystem;

  } /* namespace prm */
} /* namespace gum */

#ifndef GUM_NO_INLINE
#  include <agrum/PRM/elements/PRMObject_inl.h>
#endif   // GUM_NO_INLINE

#endif /* GUM_PRM_OBJECT_H */
