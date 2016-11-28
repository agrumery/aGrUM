/**************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Implementation for the O3SystemFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3SystemFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      INLINE O3SystemFactory<GUM_SCALAR>::O3SystemFactory(
          PRM<GUM_SCALAR>&          prm,
          O3PRM&                    o3_prm,
          O3NameSolver<GUM_SCALAR>& solver,
          ErrorsContainer&          errors )
          : __prm( &prm )
          , __o3_prm( &o3_prm )
          , __solver( &solver )
          , __errors( &errors ) {
        GUM_CONSTRUCTOR( O3SystemFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3SystemFactory<GUM_SCALAR>::O3SystemFactory(
          const O3SystemFactory<GUM_SCALAR>& src )
          : __prm( src.__prm )
          , __o3_prm( src.__o3_prm )
          , __solver( src.__solver )
          , __errors( src.__errors )
          , __nameMap( src.__nameMap ) {
        GUM_CONS_CPY( O3SystemFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3SystemFactory<GUM_SCALAR>::O3SystemFactory(
          O3SystemFactory<GUM_SCALAR>&& src )
          : __prm( std::move( src.__prm ) )
          , __o3_prm( std::move( src.__o3_prm ) )
          , __solver( std::move( src.__solver ) )
          , __errors( std::move( src.__errors ) )
          , __nameMap( std::move( src.__nameMap ) ) {
        GUM_CONS_MOV( O3SystemFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3SystemFactory<GUM_SCALAR>::~O3SystemFactory() {
        GUM_DESTRUCTOR( O3SystemFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3SystemFactory<GUM_SCALAR>& O3SystemFactory<GUM_SCALAR>::
      operator=( const O3SystemFactory<GUM_SCALAR>& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = src.__prm;
        __o3_prm = src.__o3_prm;
        __solver = src.__solver;
        __errors = src.__errors;
        return *this;
      }

      template <typename GUM_SCALAR>
      INLINE O3SystemFactory<GUM_SCALAR>& O3SystemFactory<GUM_SCALAR>::
      operator=( O3SystemFactory<GUM_SCALAR>&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = std::move( src.__prm );
        __o3_prm = std::move( src.__o3_prm );
        __solver = std::move( src.__solver );
        __errors = std::move( src.__errors );
        return *this;
      }

      template <typename GUM_SCALAR>
      INLINE void O3SystemFactory<GUM_SCALAR>::build() {
        PRMFactory<GUM_SCALAR> factory( __prm );

        for ( auto& sys : __o3_prm->systems() ) {
          // Reseting name map for each system
          __nameMap = HashTable<std::string, O3Instance*>();

          if ( __checkSystem( *sys ) ) {

            factory.startSystem( sys->name().label() );

            __addInstances( factory, *sys );
            __addAssignments( factory, *sys );
            __addIncrements( factory, *sys );

            try {
              factory.endSystem();
            } catch ( FatalError& ) {
              O3PRM_SYSTEM_INSTANTIATION_FAILED( *sys, *__errors );
            }
          }
        }
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3SystemFactory<GUM_SCALAR>::__addInstances( PRMFactory<GUM_SCALAR>& factory,
                                                   O3System&               sys ) {
        for ( auto& i : sys.instances() ) {

          if ( i.parameters().size() > 0 ) {

            auto params = HashTable<std::string, double>();
            for ( auto& p : i.parameters() ) {
              params.insert( p.name().label(), (double)p.value().value() );
            }
            factory.addInstance( i.type().label(), i.name().label(), params );

          } else {

            if ( i.size().value() > 1 ) {
              factory.addArray(
                  i.type().label(), i.name().label(), i.size().value() );
            } else {
              factory.addInstance( i.type().label(), i.name().label() );
            }
          }
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3SystemFactory<GUM_SCALAR>::__addAssignments(
          PRMFactory<GUM_SCALAR>& factory, O3System& sys ) {
        const auto& real_sys = __prm->getSystem( sys.name().label() );

        for ( auto& ass : sys.assignments() ) {

          auto leftInstance = ass.leftInstance().label();
          auto leftReference = ass.leftReference().label();
          auto rightInstance = ass.rightInstance().label();

          if ( ass.leftIndex().value() > -1 && real_sys.isArray( leftInstance ) ) {

            std::stringstream sBuff;
            sBuff << leftInstance << "[" << ass.leftIndex().value() << "]";
            leftInstance = sBuff.str();
          }

          if ( ass.rightIndex().value() > -1 &&
               real_sys.isArray( rightInstance ) ) {

            std::stringstream sBuff;
            sBuff << rightInstance << "[" << ass.rightIndex().value() << "]";
            rightInstance = sBuff.str();
          }

          factory.setReferenceSlot( leftInstance, leftReference, rightInstance );
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3SystemFactory<GUM_SCALAR>::__addIncrements(
          PRMFactory<GUM_SCALAR>& factory, O3System& sys ) {
        const auto& real_sys = __prm->getSystem( sys.name().label() );
        for ( auto& inc : sys.increments() ) {

          auto leftInstance = inc.leftInstance().label();
          auto leftReference = inc.leftReference().label();
          auto rightInstance = inc.rightInstance().label();

          if ( inc.leftIndex().value() > -1 && real_sys.isArray( leftInstance ) ) {

            std::stringstream sBuff;
            sBuff << leftInstance << "[" << inc.leftIndex().value() << "]";
            leftInstance = sBuff.str();
          }

          if ( inc.rightIndex().value() > -1 &&
               real_sys.isArray( rightInstance ) ) {

            std::stringstream sBuff;
            sBuff << rightInstance << "[" << inc.rightIndex().value() << "]";
            rightInstance = sBuff.str();
          }

          factory.setReferenceSlot( leftInstance, leftReference, rightInstance );
        }
      }

      template <typename GUM_SCALAR>
      INLINE bool O3SystemFactory<GUM_SCALAR>::__checkSystem( O3System& sys ) {

        if ( __checkInstance( sys ) && __checkAssignments( sys ) &&
             __checkIncrements( sys ) ) {
          return true;
        }

        return false;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3SystemFactory<GUM_SCALAR>::__checkInstance( O3System& sys ) {

        for ( auto& i : sys.instances() ) {

          if ( !__solver->resolveClass( i.type() ) ) {
            return false;
          }

          const auto& type = __prm->getClass( i.type().label() );
          if ( type.parameters().size() > 0 ) {
            if ( !__checkParameters( type, i ) ) {
              return false;
            }
          }

          if ( __nameMap.exists( i.name().label() ) ) {
            O3PRM_SYSTEM_DUPLICATE_INSTANCE( i, *__errors );
            return false;
          }

          __nameMap.insert( i.name().label(), &i );
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3SystemFactory<GUM_SCALAR>::__checkParameters(
          const PRMClass<GUM_SCALAR>& type, const O3Instance& inst ) {

        for ( const auto& param : inst.parameters() ) {

          if ( !type.exists( param.name().label() ) ) {
            O3PRM_SYSTEM_PARAMETER_NOT_FOUND( param, *__errors );
            return false;
          }

          if ( !PRMClassElement<GUM_SCALAR>::isParameter(
                   type.get( param.name().label() ) ) ) {
            O3PRM_SYSTEM_NOT_A_PARAMETER( param, *__errors );
            return false;
          }

          const auto& type_param = static_cast<const PRMParameter<GUM_SCALAR>&>(
              type.get( param.name().label() ) );

          switch ( type_param.valueType() ) {

            case PRMParameter<GUM_SCALAR>::ParameterType::INT: {
              if ( !param.isInteger() ) {
                O3PRM_SYSTEM_PARAMETER_NOT_INT( param, *__errors );
                return false;
              }
              break;
            }

            case PRMParameter<GUM_SCALAR>::ParameterType::REAL: {
              if ( param.isInteger() ) {
                O3PRM_SYSTEM_PARAMETER_NOT_FLOAT( param, *__errors );
                return false;
              }
              break;
            }

            default: { GUM_ERROR( FatalError, "unknown parameter type" ); }
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool
      O3SystemFactory<GUM_SCALAR>::__checkAssignments( O3System& sys ) {

        for ( auto& ass : sys.assignments() ) {

          // if ( ass.leftInstance().label() == ass.leftReference().label() ) {
          //  O3PRM_SYSTEM_INVALID_LEFT_VALUE( ass.leftInstance(), *__errors );
          //  return false;
          //}

          if ( !__nameMap.exists( ass.leftInstance().label() ) ) {
            O3PRM_SYSTEM_INSTANCE_NOT_FOUND( ass.leftInstance(), *__errors );
            return false;
          }

          auto        i = __nameMap[ass.leftInstance().label()];
          const auto& type = __prm->getClass( i->type().label() );
          const auto& ref = ass.leftReference().label();

          if ( !( type.exists( ass.leftReference().label() ) &&
                  PRMClassElement<GUM_SCALAR>::isReferenceSlot(
                      type.get( ref ) ) ) ) {

            O3PRM_SYSTEM_REFERENCE_NOT_FOUND(
                ass.leftReference(), type.name(), *__errors );
            return false;
          }

          const auto& real_ref =
              static_cast<const PRMReferenceSlot<GUM_SCALAR>&>( type.get( ref ) );

          if ( !__nameMap.exists( ass.rightInstance().label() ) ) {
            O3PRM_SYSTEM_INSTANCE_NOT_FOUND( ass.rightInstance(), *__errors );
            return false;
          }

          if ( real_ref.isArray() &&
               __nameMap[ass.rightInstance().label()]->size().value() == 0 ) {

            O3PRM_SYSTEM_NOT_AN_ARRAY( ass.rightInstance(), *__errors );
            return false;
          }

          if ( ( !real_ref.isArray() ) &&
               __nameMap[ass.rightInstance().label()]->size().value() > 0 &&
               ass.rightIndex().value() == -1 ) {

            O3PRM_SYSTEM_NOT_AN_ARRAY( ass.leftReference(), *__errors );
            return false;
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3SystemFactory<GUM_SCALAR>::__checkIncrements( O3System& sys ) {

        for ( auto& inc : sys.increments() ) {

          // if ( inc.leftInstance().label() == inc.leftReference().label() ) {
          //  O3PRM_SYSTEM_INVALID_LEFT_VALUE( inc.leftInstance(), *__errors );
          //  return false;
          //}

          if ( !__nameMap.exists( inc.leftInstance().label() ) ) {
            O3PRM_SYSTEM_INSTANCE_NOT_FOUND( inc.leftInstance(), *__errors );
            return false;
          }

          auto        i = __nameMap[inc.leftInstance().label()];
          const auto& type = __prm->getClass( i->type().label() );
          const auto& ref = inc.leftReference().label();

          if ( !( type.exists( inc.leftReference().label() ) &&
                  PRMClassElement<GUM_SCALAR>::isReferenceSlot(
                      type.get( ref ) ) ) ) {

            O3PRM_SYSTEM_REFERENCE_NOT_FOUND(
                inc.leftReference(), type.name(), *__errors );
            return false;
          }

          const auto& real_ref =
              static_cast<const PRMReferenceSlot<GUM_SCALAR>&>( type.get( ref ) );

          if ( !real_ref.isArray() ) {

            O3PRM_SYSTEM_NOT_AN_ARRAY( inc.leftReference(), *__errors );
            return false;
          }
        }

        return true;
      }
    }  // o3prm
  }    // prm
}  // gum
