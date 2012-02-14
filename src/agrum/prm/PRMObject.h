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
 * @author Lionel TORTI
 */
// ============================================================================
#ifndef GUM_PRM_OBJECT_H
#define GUM_PRM_OBJECT_H
// ============================================================================
#include <string>
// ============================================================================
#include <agrum/config.h>
// ============================================================================
#include <agrum/prm/utils_prm.h>
// ============================================================================

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
         * The "all" type is used to tell that we want any kind of ObjectType
         * (useful with iterators for example). No PRMObject will ever have
         * "all" as type.
         */
        enum ObjectType { all, prm_class, prm_interface, prm_class_elt,
                          prm_type, prm_system, prm_instance
                      };

        /// Returns the string representation of a PRMObject.
        static std::string enum2str( ObjectType type ) {
          switch ( type ) {
            case prm_class:
              return "prm_class";
            case prm_class_elt:
              return "prm_class_elt";
            case prm_type:
              return "prm_type";
            case prm_system:
              return "prm_system";
            case prm_instance:
              return "prm_instance";
            case prm_interface:
              return "prm_interface";
            default:
              return "unknown";
          }
        }

        /// Returns true if obj_ptr is of type Class.
        static INLINE bool isClass( const PRMObject& obj ) {
          return obj.obj_type() == prm_class;
        }

        /// Returns true if obj_ptr is of type Interface.
        static INLINE bool isInterface( const PRMObject& obj ) {
          return obj.obj_type() == prm_interface;
        }

        /// Returns true if obj_ptr is of type Instance.
        static INLINE bool isInstance( const PRMObject& obj ) {
          return obj.obj_type() == prm_instance;
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
        PRMObject( const std::string& name );

        /**
         * Copy constructor.
         */
        PRMObject( const PRMObject& source );

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
         * Returns the type of this object.
         */
        virtual ObjectType obj_type() const =0;

        /// @}
        // ==========================================================================
        /// @name Operators
        // ==========================================================================
        /// @{

        /**
         * To PRMObject are equal if they have the same name (which is unique).
         */
        bool operator==( const PRMObject& obj ) const;

        /**
         * To PRMObject are equal if they have the same name (which is unique).
         */
        bool operator!=( const PRMObject& obj ) const;

        /**
         * Copy operator.
         */
        PRMObject& operator=( const PRMObject& source );

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

/// For printing ObjectType easily.
    std::ostream& operator<<( std::ostream& out, PRMObject::ObjectType obj_type );
  } /* namespace prm */
} /* namespace gum */

// ============================================================================
#ifndef GUM_NO_INLINE
#include <agrum/prm/PRMObject.inl>
#endif // GUM_NO_INLINE
// ============================================================================
#endif /* GUM_PRM_OBJECT_H */
// ============================================================================
