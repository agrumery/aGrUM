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

#include <agrum/PRM/newo3prm/O3SystemFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      O3SystemFactory<GUM_SCALAR>::O3SystemFactory() {
        GUM_CONSTRUCTOR( O3SystemFactory );
      }

      template <typename GUM_SCALAR>
      O3SystemFactory<GUM_SCALAR>::O3SystemFactory(
          const O3SystemFactory<GUM_SCALAR>& src ) {
        GUM_CONS_CPY( O3SystemFactory );
      }

      template <typename GUM_SCALAR>
      O3SystemFactory<GUM_SCALAR>::O3SystemFactory(
          O3SystemFactory<GUM_SCALAR>&& src ) {
        GUM_CONS_MOV( O3SystemFactory );
      }

      template <typename GUM_SCALAR>
      O3SystemFactory<GUM_SCALAR>::~O3SystemFactory() {
        GUM_DESTRUCTOR( O3SystemFactory );
      }

      template <typename GUM_SCALAR>
      O3SystemFactory<GUM_SCALAR>& O3SystemFactory<GUM_SCALAR>::
      operator=( const O3SystemFactory<GUM_SCALAR>& src ) {
        return *this;
      }

      template <typename GUM_SCALAR>
      O3SystemFactory<GUM_SCALAR>& O3SystemFactory<GUM_SCALAR>::
      operator=( O3SystemFactory<GUM_SCALAR>&& src ) {
        return *this;
      }

      template <typename GUM_SCALAR>
      void O3SystemFactory<GUM_SCALAR>::build( PRM<GUM_SCALAR>& prm,
                                               O3PRM& o3_prm,
                                               std::ostream& output ) {
        __nameMap = HashTable<std::string, O3Instance*>();
        PRMFactory<GUM_SCALAR> factory( &prm );
        for ( auto& sys : o3_prm.systems() ) {
          if ( __checkSystem( prm, *sys, output ) ) {
            factory.startSystem( sys->name().label() );
            for ( auto& i : sys->instances() ) {
              if ( i.size().value() > 1 ) {
                factory.addArray(
                    i.type().label(), i.name().label(), i.size().value() );
              } else {
                factory.addInstance( i.type().label(), i.name().label() );
              }
            }
            const auto& real_sys = prm.system( sys->name().label() );
            for ( auto& ass : sys->assignments() ) {
              auto sBuff = std::stringstream();
              if ( real_sys.isArray( ass.leftInstance().label() ) ) {
                sBuff << ass.leftInstance().label() << "["
                      << ass.index().value() << "]";
              } else {
                sBuff << ass.leftInstance().label();
              }
              factory.setReferenceSlot( sBuff.str(),
                                        ass.leftReference().label(),
                                        ass.rightInstance().label() );
            }
            for ( auto& inc : sys->increments() ) {
              auto sBuff = std::stringstream();
              if ( real_sys.isArray( inc.leftInstance().label() ) ) {
                sBuff << inc.leftInstance().label() << "["
                      << inc.index().value() << "]";
              } else {
                sBuff << inc.leftInstance().label();
              }
              factory.setReferenceSlot( inc.leftInstance().label(),
                                        inc.leftReference().label(),
                                        inc.rightInstance().label() );
            }
            factory.endSystem();
          }
        }
      }

      template <typename GUM_SCALAR>
      bool O3SystemFactory<GUM_SCALAR>::__checkSystem( PRM<GUM_SCALAR>& prm,
                                                       O3System& sys,
                                                       std::ostream& output ) {
        if ( not __checkInstance( prm, sys, output ) ) {
          return false;
        }
        if ( not( __checkAssignments( prm, sys, output ) and
                  __checkIncrements( prm, sys, output ) ) ) {
          return false;
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool O3SystemFactory<GUM_SCALAR>::__checkIncrements(
          PRM<GUM_SCALAR>& prm, O3System& sys, std::ostream& output ) {
        for ( auto& inc : sys.increments() ) {
          if ( inc.leftInstance().label() == inc.leftReference().label() ) {
            const auto& pos = inc.leftInstance().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Increment error : "
                   << "Invalid left expression " << inc.leftInstance().label()
                   << std::endl;
            return false;
          }
          if ( not __nameMap.exists( inc.leftInstance().label() ) ) {
            const auto& pos = inc.leftInstance().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Increment error : "
                   << "Instance " << inc.leftInstance().label() << " not found"
                   << std::endl;
            return false;
          }
          auto i = __nameMap[inc.leftInstance().label()];
          const auto& type = prm.getClass( i->type().label() );
          const auto& ref = inc.leftReference().label();
          if ( not( type.exists( inc.leftReference().label() ) and
                    ClassElement<GUM_SCALAR>::isReferenceSlot(
                        type.get( ref ) ) ) ) {
            const auto& pos = inc.leftReference().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Increment error : "
                   << "Reference " << inc.leftReference().label()
                   << " not found in class " << type.name() << std::endl;
            return false;
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool O3SystemFactory<GUM_SCALAR>::__checkParameters(
          const Class<GUM_SCALAR>& type,
          const O3Instance& inst,
          std::ostream& output ) {
        for ( const auto& param : inst.parameters() ) {
          if ( not type.exists( param.name().label() ) ) {
            const auto& pos = param.name().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Instance error : "
                   << "Parameter " << param.name().label() << " not found"
                   << std::endl;
            return false;
          }
          if ( not ClassElement<GUM_SCALAR>::isParameter(
                   type.get( param.name().label() ) ) ) {
            const auto& pos = param.name().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Instance error : " << param.name().label()
                   << " is not a parameter" << std::endl;
            return false;
          }
          const auto& type_param = static_cast<const Parameter<GUM_SCALAR>&>(
              type.get( param.name().label() ) );
          switch ( type_param.valueType() ) {
            case Parameter<GUM_SCALAR>::ParameterType::INT: {
              if ( not param.isInteger() ) {
                const auto& pos = param.value().position();
                output << pos.file() << "|" << pos.line() << " col "
                       << pos.column() << "|"
                       << " Instance error : "
                       << "Parameter " << param.name().label()
                       << " is an integer" << std::endl;
                return false;
              }
              break;
              case Parameter<GUM_SCALAR>::ParameterType::REAL: {
                if ( param.isInteger() ) {
                  const auto& pos = param.value().position();
                  output << pos.file() << "|" << pos.line() << " col "
                         << pos.column() << "|"
                         << " Instance error : "
                         << "Parameter " << param.name().label()
                         << " is a float" << std::endl;
                  return false;
                }
                break;
              }
              default: { GUM_ERROR( FatalError, "unknown parameter type" ); }
            }
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool O3SystemFactory<GUM_SCALAR>::__checkInstance(
          PRM<GUM_SCALAR>& prm, O3System& sys, std::ostream& output ) {
        for ( auto& i : sys.instances() ) {
          if ( not prm.isClass( i.type().label() ) ) {
            const auto& pos = i.type().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Instance error : "
                   << "Instance type is not a class " << i.type().label()
                   << std::endl;
            return false;
          }
          const auto& type = prm.getClass( i.type().label() );
          if ( type.parameters().size() > 0 ) {
            if ( not __checkParameters( type, i, output ) ) {
              return false;
            }
          }
          if ( __nameMap.exists( i.name().label() ) ) {
            const auto& pos = i.type().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Instance error : "
                   << "Instance " << i.name().label() << " already exists"
                   << std::endl;
            return false;
          }
          __nameMap.insert( i.name().label(), &i );
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool O3SystemFactory<GUM_SCALAR>::__checkAssignments(
          PRM<GUM_SCALAR>& prm, O3System& sys, std::ostream& output ) {
        for ( auto& ass : sys.assignments() ) {
          if ( ass.leftInstance().label() == ass.leftReference().label() ) {
            const auto& pos = ass.leftInstance().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Assignment error : "
                   << "Invalid left expression " << ass.leftInstance().label()
                   << std::endl;
            return false;
          }
          if ( not __nameMap.exists( ass.leftInstance().label() ) ) {
            const auto& pos = ass.leftInstance().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Assignment error : "
                   << "Instance " << ass.leftInstance().label() << " not found"
                   << std::endl;
            return false;
          }
          auto i = __nameMap[ass.leftInstance().label()];
          const auto& type = prm.getClass( i->type().label() );
          const auto& ref = ass.leftReference().label();
          if ( not( type.exists( ass.leftReference().label() ) and
                    ClassElement<GUM_SCALAR>::isReferenceSlot(
                        type.get( ref ) ) ) ) {
            const auto& pos = ass.leftReference().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Assignment error : "
                   << "Reference " << ass.leftReference().label()
                   << " not found in class " << type.name() << std::endl;
            return false;
          }
        }
        return true;
      }
    }  // o3prm
  }    // prm
}  // gum

