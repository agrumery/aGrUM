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
/** @file
 * @brief Inline implementation of PRMFactory.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/PRMFactory.h>

#include <iostream>
#include <sstream>

#include <agrum/core/math/formula.h>

#include <agrum/PRM/elements/PRMFormAttribute.h>
#include <agrum/PRM/elements/PRMFuncAttribute.h>

namespace gum {

  namespace prm {
    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addLabel( const std::string& l,
                                                  std::string        extends ) {
      if ( extends == "" ) {
        PRMType<GUM_SCALAR>* t = static_cast<PRMType<GUM_SCALAR>*>(
            __checkStack( 1, PRMObject::prm_type::TYPE ) );
        LabelizedVariable* var = dynamic_cast<LabelizedVariable*>( t->__var );

        if ( !var ) {
          GUM_ERROR( FatalError,
                     "the current type's variable is not a LabelizedVariable." );
        } else if ( t->__superType ) {
          GUM_ERROR( OperationNotAllowed, "current type is a subtype." );
        }

        try {
          var->addLabel( l );
        } catch ( DuplicateElement& ) {
          GUM_ERROR( DuplicateElement,
                     "a label with the same value already exists" );
        }
      } else {
        PRMType<GUM_SCALAR>* t = static_cast<PRMType<GUM_SCALAR>*>(
            __checkStack( 1, PRMObject::prm_type::TYPE ) );
        LabelizedVariable* var = dynamic_cast<LabelizedVariable*>( t->__var );

        if ( !var ) {
          GUM_ERROR( FatalError,
                     "the current type's variable is not a LabelizedVariable." );
        } else if ( !t->__superType ) {
          GUM_ERROR( OperationNotAllowed, "current type is not a subtype." );
        }

        bool found = false;

        for ( Idx i = 0; i < t->__superType->__var->domainSize(); ++i ) {
          if ( t->__superType->__var->label( i ) == extends ) {
            try {
              var->addLabel( l );
            } catch ( DuplicateElement& ) {
              GUM_ERROR( DuplicateElement,
                         "a label with the same value already exists" );
            }

            t->__label_map->push_back( i );

            found = true;
            break;
          }
        }

        if ( !found ) {
          GUM_ERROR( NotFound, "inexistent label in super type." );
        }
      }
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::startClass( const std::string&      name,
                                        const std::string&      extends,
                                        const Set<std::string>* implements,
                                        bool delayInheritance ) {
      std::string real_name = __addPrefix( name );
      if ( __prm->__classMap.exists( real_name ) ||
           __prm->__interfaceMap.exists( real_name ) ) {
        std::stringstream msg;
        msg << "\"" << real_name << "\" is already used.";
        GUM_ERROR( DuplicateElement, msg.str() );
      }
      PRMClass<GUM_SCALAR>*          c = nullptr;
      PRMClass<GUM_SCALAR>*          mother = nullptr;
      Set<PRMInterface<GUM_SCALAR>*> impl;

      if ( implements != 0 ) {
        for ( const auto& imp : *implements ) {
          impl.insert( __retrieveInterface( imp ) );
        }
      }

      if ( extends != "" ) {
        mother = __retrieveClass( extends );
      }

      if ( ( extends == "" ) && impl.empty() ) {
        c = new PRMClass<GUM_SCALAR>( real_name );
      } else if ( ( extends != "" ) && impl.empty() ) {
        c = new PRMClass<GUM_SCALAR>( real_name, *mother, delayInheritance );
      } else if ( ( extends == "" ) && ( !impl.empty() ) ) {
        c = new PRMClass<GUM_SCALAR>( real_name, impl, delayInheritance );
      } else if ( ( extends != "" ) && ( !impl.empty() ) ) {
        c = new PRMClass<GUM_SCALAR>( real_name, *mother, impl, delayInheritance );
      }

      __prm->__classMap.insert( c->name(), c );
      __prm->__classes.insert( c );
      __stack.push_back( c );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::continueClass( const std::string& name ) {
      std::string real_name = __addPrefix( name );
      if ( !( __prm->__classMap.exists( real_name ) ) ) {
        std::stringstream msg;
        msg << "\"" << real_name << "\" not found";
        GUM_ERROR( NotFound, msg.str() );
      }
      __stack.push_back( &( __prm->getClass( real_name ) ) );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::endClass( bool checkImplementations ) {
      PRMClass<GUM_SCALAR>* c = static_cast<PRMClass<GUM_SCALAR>*>(
          __checkStack( 1, PRMObject::prm_type::CLASS ) );

      if ( checkImplementations ) {
        __checkInterfaceImplementation( c );
      }

      __stack.pop_back();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::__checkInterfaceImplementation(
        PRMClass<GUM_SCALAR>* c ) {
      try {
        for ( const auto& i : c->implements() ) {
          try {
            for ( const auto& node : i->dag().nodes() ) {
              std::string name = i->get( node ).name();

              switch ( i->get( node ).elt_type() ) {
                case PRMClassElement<GUM_SCALAR>::prm_aggregate:
                case PRMClassElement<GUM_SCALAR>::prm_attribute: {
                  if ( ( c->get( name ).elt_type() ==
                         PRMClassElement<GUM_SCALAR>::prm_attribute ) ||
                       ( c->get( name ).elt_type() ==
                         PRMClassElement<GUM_SCALAR>::prm_aggregate ) ) {

                    if ( !c->get( name ).type().isSubTypeOf(
                             i->get( name ).type() ) ) {

                      std::stringstream msg;
                      msg << "class " << c->name()
                          << " does not respect interface ";
                      GUM_ERROR( TypeError, msg.str() + i->name() );
                    }
                  } else {

                    std::stringstream msg;
                    msg << "class " << c->name() << " does not respect interface ";
                    GUM_ERROR( TypeError, msg.str() + i->name() );
                  }

                  break;
                }

                case PRMClassElement<GUM_SCALAR>::prm_refslot: {
                  if ( c->get( name ).elt_type() ==
                       PRMClassElement<GUM_SCALAR>::prm_refslot ) {

                    const PRMReferenceSlot<GUM_SCALAR>& ref_i =
                        static_cast<const PRMReferenceSlot<GUM_SCALAR>&>(
                            i->get( name ) );
                    const PRMReferenceSlot<GUM_SCALAR>& ref_this =
                        static_cast<const PRMReferenceSlot<GUM_SCALAR>&>(
                            c->get( name ) );

                    if ( !ref_this.slotType().isSubTypeOf( ref_i.slotType() ) ) {

                      std::stringstream msg;
                      msg << "class " << c->name()
                          << " does not respect interface ";
                      GUM_ERROR( TypeError, msg.str() + i->name() );
                    }
                  } else {

                    std::stringstream msg;
                    msg << "class " << c->name() << " does not respect interface ";
                    GUM_ERROR( TypeError, msg.str() + i->name() );
                  }

                  break;
                }

                case PRMClassElement<GUM_SCALAR>::prm_slotchain: {
                  // Nothing to check: they are automatically inherited
                  break;
                }

                default: {
                  std::string msg =
                      "unexpected ClassElement<GUM_SCALAR> in interface ";
                  GUM_ERROR( FatalError, msg + i->name() );
                }
              }
            }
          } catch ( NotFound& ) {
            std::stringstream msg;
            msg << "class " << c->name() << " does not respect interface ";
            GUM_ERROR( TypeError, msg.str() + i->name() );
          }
        }
      } catch ( NotFound& ) {
        // this Class<GUM_SCALAR> does not implement any
        // PRMInterface<GUM_SCALAR>
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::startInterface( const std::string& name,
                                                        const std::string& extends,
                                                        bool delayInheritance ) {
      std::string real_name = __addPrefix( name );
      if ( __prm->__classMap.exists( real_name ) ||
           __prm->__interfaceMap.exists( real_name ) ) {
        std::stringstream msg;
        msg << "\"" << real_name << "\" is already used.";
        GUM_ERROR( DuplicateElement, msg.str() );
      }
      PRMInterface<GUM_SCALAR>* i = nullptr;
      PRMInterface<GUM_SCALAR>* super = nullptr;

      if ( extends != "" ) {
        super = __retrieveInterface( extends );
      }

      if ( super != nullptr ) {
        i = new PRMInterface<GUM_SCALAR>( real_name, *super, delayInheritance );
      } else {
        i = new PRMInterface<GUM_SCALAR>( real_name );
      }

      __prm->__interfaceMap.insert( i->name(), i );
      __prm->__interfaces.insert( i );
      __stack.push_back( i );
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::continueInterface( const std::string& name ) {
      std::string real_name = __addPrefix( name );
      if ( !__prm->__interfaceMap.exists( real_name ) ) {
        std::stringstream msg;
        msg << "\"" << real_name << "\" not found.";
        GUM_ERROR( DuplicateElement, msg.str() );
      }

      PRMInterface<GUM_SCALAR>* i = __retrieveInterface( real_name );
      __stack.push_back( i );
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::addAttribute( PRMAttribute<GUM_SCALAR>* attr ) {
      PRMClass<GUM_SCALAR>* c = static_cast<PRMClass<GUM_SCALAR>*>(
          __checkStack( 1, PRMObject::prm_type::CLASS ) );
      c->add( attr );
      Size                                     count = 0;
      const Sequence<const DiscreteVariable*>& vars =
          attr->cpf().variablesSequence();

      for ( const auto& node : c->dag().nodes() ) {
        try {
          if ( vars.exists( &( c->get( node ).type().variable() ) ) ) {
            ++count;

            if ( &( attr->type().variable() ) !=
                 &( c->get( node ).type().variable() ) ) {
              c->addArc( c->get( node ).safeName(), attr->safeName() );
            }
          }
        } catch ( OperationNotAllowed& ) {
        }
      }

      if ( count != attr->cpf().variablesSequence().size() ) {
        GUM_ERROR( NotFound, "unable to found all parents of this attribute" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::__addParent( PRMClassElementContainer<GUM_SCALAR>* c,
                                         PRMAttribute<GUM_SCALAR>*             a,
                                         const std::string& name ) {
      try {
        PRMClassElement<GUM_SCALAR>& elt = c->get( name );

        switch ( elt.elt_type() ) {
          case PRMClassElement<GUM_SCALAR>::prm_refslot: {
            GUM_ERROR(
                OperationNotAllowed,
                "can not add a reference slot as a parent of an attribute" );
            break;
          }

          case PRMClassElement<GUM_SCALAR>::prm_slotchain: {
            if ( static_cast<PRMSlotChain<GUM_SCALAR>&>( elt ).isMultiple() ) {
              GUM_ERROR( OperationNotAllowed,
                         "can not add a multiple slot chain to an attribute" );
            }

            c->addArc( name, a->name() );

            break;
          }

          case PRMClassElement<GUM_SCALAR>::prm_attribute:
          case PRMClassElement<GUM_SCALAR>::prm_aggregate: {
            c->addArc( name, a->name() );
            break;
          }

          default: { GUM_ERROR( FatalError, "unknown ClassElement<GUM_SCALAR>" ); }
        }
      } catch ( NotFound& ) {
        // Check if name is a slot chain
        PRMSlotChain<GUM_SCALAR>* sc = __buildSlotChain( c, name );

        if ( sc == nullptr ) {
          std::string msg =
              "found no ClassElement<GUM_SCALAR> with the given name ";
          GUM_ERROR( NotFound, msg + name );
        } else if ( !sc->isMultiple() ) {
          c->add( sc );
          c->addArc( sc->name(), a->name() );
        } else {
          delete sc;
          GUM_ERROR( OperationNotAllowed,
                     "Impossible to add a multiple reference slot as"
                     " direct parent of an PRMAttribute<GUM_SCALAR>." );
        }
      }
    }


    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addParent( const std::string& name ) {
      PRMClassElementContainer<GUM_SCALAR>* c = __checkStackContainter( 2 );
      try {
        // Retrieving pointers
        PRMAttribute<GUM_SCALAR>* a = static_cast<PRMAttribute<GUM_SCALAR>*>(
            __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_attribute ) );
        __addParent( c, a, name );
      } catch ( FactoryInvalidState& ) {
        auto agg = static_cast<PRMAggregate<GUM_SCALAR>*>(
            __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_aggregate ) );
        __addParent( static_cast<PRMClass<GUM_SCALAR>*>( c ), agg, name );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::setRawCPFByFloatLines(
        const std::vector<float>& array ) {
      PRMAttribute<GUM_SCALAR>* a = static_cast<PRMAttribute<GUM_SCALAR>*>(
          __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_attribute ) );
      __checkStack( 2, PRMObject::prm_type::CLASS );

      if ( a->cpf().domainSize() != array.size() )
        GUM_ERROR( OperationNotAllowed, "illegal CPF size" );

      std::vector<GUM_SCALAR> array2( array.begin(), array.end() );
      a->cpf().fillWith( array2 );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::setRawCPFByLines(
        const std::vector<GUM_SCALAR>& array ) {
      auto elt = __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_attribute );
      auto a = static_cast<PRMAttribute<GUM_SCALAR>*>( elt );
      __checkStack( 2, PRMObject::prm_type::CLASS );

      if ( a->cpf().domainSize() != array.size() ) {
        GUM_ERROR( OperationNotAllowed, "illegal CPF size" );
      }

      a->cpf().fillWith( array );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::setRawCPFByFloatColumns(
        const std::vector<float>& array ) {
      PRMAttribute<GUM_SCALAR>* a = static_cast<PRMAttribute<GUM_SCALAR>*>(
          __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_attribute ) );

      if ( a->cpf().domainSize() != array.size() ) {
        GUM_ERROR( OperationNotAllowed, "illegal CPF size" );
      }

      std::vector<GUM_SCALAR> array2( array.begin(), array.end() );
      setRawCPFByColumns( array2 );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::setRawCPFByColumns(
        const std::vector<GUM_SCALAR>& array ) {
      PRMAttribute<GUM_SCALAR>* a = static_cast<PRMAttribute<GUM_SCALAR>*>(
          __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_attribute ) );

      if ( a->cpf().domainSize() != array.size() ) {
        GUM_ERROR( OperationNotAllowed, "illegal CPF size" );
      }

      if ( a->cpf().nbrDim() == 1 ) {

        setRawCPFByLines( array );

      } else {

        Instantiation inst( a->cpf() );
        Instantiation jnst;
        for ( auto idx = inst.variablesSequence().rbegin();
              idx != inst.variablesSequence().rend();
              --idx ) {
          jnst.add( **idx );
        }

        jnst.begin();
        auto idx = (std::size_t)0;
        while ( ( !jnst.end() ) && idx < array.size() ) {
          inst.setVals( jnst );
          a->cpf().set( inst, array[idx] );
          jnst.inc();
          ++idx;
        }
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::setCPFByFloatRule(
        const std::vector<std::string>& parents,
        const std::vector<float>&       values ) {
      auto a = static_cast<PRMAttribute<GUM_SCALAR>*>(
          __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_attribute ) );

      if ( ( parents.size() + 1 ) != a->cpf().variablesSequence().size() ) {
        GUM_ERROR( OperationNotAllowed, "wrong number of parents" );
      }

      if ( values.size() != a->type().variable().domainSize() ) {
        GUM_ERROR( OperationNotAllowed, "wrong number of values" );
      }

      std::vector<GUM_SCALAR> values2( values.begin(), values.end() );
      setCPFByRule( parents, values2 );
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::setCPFByRule( const std::vector<std::string>& parents,
                                          const std::vector<GUM_SCALAR>& values ) {

      auto a = static_cast<PRMAttribute<GUM_SCALAR>*>(
          __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_attribute ) );

      if ( ( parents.size() + 1 ) != a->cpf().variablesSequence().size() ) {
        GUM_ERROR( OperationNotAllowed, "wrong number of parents" );
      }

      if ( values.size() != a->type().variable().domainSize() ) {
        GUM_ERROR( OperationNotAllowed, "wrong number of values" );
      }

      if ( dynamic_cast<PRMFormAttribute<GUM_SCALAR>*>( a ) ) {
        auto form = static_cast<PRMFormAttribute<GUM_SCALAR>*>( a );
        // jnst holds parents with a specific value (not "*")
        // knst holds parents without a specific value ("*")
        Instantiation           jnst, knst;
        const DiscreteVariable* var = 0;
        // not_used Size pos = 0;
        bool found = false;

        for ( Idx i = 0; i < parents.size(); ++i ) {
          var = form->formulas().variablesSequence().atPos( 1 + i );

          if ( parents[i] == "*" ) {
            knst.add( *var );
          } else {
            jnst.add( *var );
            // not_used pos = 0;
            found = false;

            for ( Size j = 0; j < var->domainSize(); ++j ) {
              if ( var->label( j ) == parents[i] ) {
                jnst.chgVal( *var, j );
                found = true;
                break;
              }
            }

            if ( !found ) {
              std::string msg = "could not find label ";
              GUM_ERROR( NotFound, msg + parents[i] );
            }
          }
        }

        Instantiation inst( form->formulas() );
        inst.setVals( jnst );

        for ( Size i = 0; i < form->type()->domainSize(); ++i ) {
          inst.chgVal( form->type().variable(), i );

          for ( inst.setFirstIn( knst ); !inst.end(); inst.incIn( knst ) ) {
            form->formulas().set( inst, std::to_string( values[i] ) );
          }
        }

      } else {
        GUM_ERROR( OperationNotAllowed, "invalide attribute type" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::setCPFByRule(
        const std::vector<std::string>& parents,
        const std::vector<std::string>& values ) {

      auto a = static_cast<PRMAttribute<GUM_SCALAR>*>(
          __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_attribute ) );

      if ( ( parents.size() + 1 ) != a->cpf().variablesSequence().size() ) {
        GUM_ERROR( OperationNotAllowed, "wrong number of parents" );
      }

      if ( values.size() != a->type().variable().domainSize() ) {
        GUM_ERROR( OperationNotAllowed, "wrong number of values" );
      }

      if ( dynamic_cast<PRMFormAttribute<GUM_SCALAR>*>( a ) ) {
        auto form = static_cast<PRMFormAttribute<GUM_SCALAR>*>( a );
        // jnst holds parents with a specific value (not "*")
        // knst holds parents without a specific value ("*")
        Instantiation           jnst, knst;
        const DiscreteVariable* var = 0;
        // not_used Size pos = 0;
        bool found = false;

        for ( Idx i = 0; i < parents.size(); ++i ) {
          var = form->formulas().variablesSequence().atPos( 1 + i );

          if ( parents[i] == "*" ) {
            knst.add( *var );
          } else {
            jnst.add( *var );
            // not_used pos = 0;
            found = false;

            for ( Size j = 0; j < var->domainSize(); ++j ) {
              if ( var->label( j ) == parents[i] ) {
                jnst.chgVal( *var, j );
                found = true;
                break;
              }
            }

            if ( !found ) {
              std::string msg = "could not find label ";
              GUM_ERROR( NotFound, msg + parents[i] );
            }
          }
        }

        Instantiation inst( form->formulas() );
        inst.setVals( jnst );

        for ( Size i = 0; i < form->type()->domainSize(); ++i ) {
          inst.chgVal( form->type().variable(), i );

          for ( inst.setFirstIn( knst ); !inst.end(); inst.incIn( knst ) ) {
            form->formulas().set( inst, values[i] );
          }
        }

      } else {
        GUM_ERROR( OperationNotAllowed, "invalide attribute type" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addParameter( const std::string& type,
                                                      const std::string& name,
                                                      double             value ) {
      auto c = static_cast<PRMClass<GUM_SCALAR>*>(
          __checkStack( 1, PRMObject::prm_type::CLASS ) );

      PRMParameter<GUM_SCALAR>* p = nullptr;
      if ( type == "int" ) {
        p = new PRMParameter<GUM_SCALAR>(
            name,
            PRMParameter<GUM_SCALAR>::ParameterType::INT,
            (GUM_SCALAR)value );
      } else if ( type == "real" ) {
        p = new PRMParameter<GUM_SCALAR>(
            name,
            PRMParameter<GUM_SCALAR>::ParameterType::REAL,
            (GUM_SCALAR)value );
      }

      try {
        c->add( p );
      } catch ( DuplicateElement& ) {
        c->overload( p );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::startAggregator(
        const std::string&              name,
        const std::string&              agg_type,
        const std::string&              rv_type,
        const std::vector<std::string>& params ) {

      PRMClass<GUM_SCALAR>* c = static_cast<PRMClass<GUM_SCALAR>*>(
          __checkStack( 1, PRMObject::prm_type::CLASS ) );

      auto agg = new PRMAggregate<GUM_SCALAR>(
          name,
          PRMAggregate<GUM_SCALAR>::str2enum( agg_type ),
          *__retrieveType( rv_type ) );

      try {
        c->add( agg );
      } catch ( DuplicateElement& ) {
        c->overload( agg );
      }

      switch ( agg->agg_type() ) {
        case PRMAggregate<GUM_SCALAR>::AggregateType::COUNT:
        case PRMAggregate<GUM_SCALAR>::AggregateType::EXISTS:
        case PRMAggregate<GUM_SCALAR>::AggregateType::FORALL: {
          if ( params.size() != 1 ) {
            GUM_ERROR( OperationNotAllowed, "aggregate requires a parameter" );
          }
          agg->setLabel( params.front() );
          break;
        }
        default: {
          // Nothing to do
        }
      }
      __stack.push_back( agg );
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::continueAggregator( const std::string& name ) {

      PRMClassElementContainer<GUM_SCALAR>* c = __checkStackContainter( 1 );
      if ( !c->exists( name ) ) {
        GUM_ERROR( NotFound, name << "not found" );
      }
      auto& agg = c->get( name );
      if ( !PRMClassElement<GUM_SCALAR>::isAggregate( agg ) ) {
        GUM_ERROR( OperationNotAllowed, name << " not an aggregate" );
      }
      __stack.push_back( &agg );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::__addParent( PRMClass<GUM_SCALAR>*     c,
                                                     PRMAggregate<GUM_SCALAR>* agg,
                                                     const std::string& name ) {

      auto chains = std::vector<std::string>{name};
      auto inputs = std::vector<PRMClassElement<GUM_SCALAR>*>();
      __retrieveInputs( c, chains, inputs );

      switch ( agg->agg_type() ) {
        case PRMAggregate<GUM_SCALAR>::AggregateType::OR:
        case PRMAggregate<GUM_SCALAR>::AggregateType::AND: {

          if ( inputs.front()->type() != *( __retrieveType( "boolean" ) ) ) {
            GUM_ERROR( WrongType, "expected booleans" );
          }

          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::COUNT:
        case PRMAggregate<GUM_SCALAR>::AggregateType::EXISTS:
        case PRMAggregate<GUM_SCALAR>::AggregateType::FORALL: {

          if ( !agg->hasLabel() ) {
            auto param = agg->labelValue();
            Idx  label_idx = 0;

            while ( label_idx < inputs.front()->type()->domainSize() ) {
              if ( inputs.front()->type()->label( label_idx ) == param ) {
                break;
              }

              ++label_idx;
            }

            if ( label_idx == inputs.front()->type()->domainSize() ) {
              GUM_ERROR( NotFound, "could not find label" );
            }

            agg->setLabel( label_idx );
          }

          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::MEDIAN:
        case PRMAggregate<GUM_SCALAR>::AggregateType::AMPLITUDE:
        case PRMAggregate<GUM_SCALAR>::AggregateType::MIN:
        case PRMAggregate<GUM_SCALAR>::AggregateType::MAX: {

          break;
        }

        default: { GUM_ERROR( FatalError, "Unknown aggregator." ); }
      }

      c->addArc( inputs.front()->safeName(), agg->safeName() );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::endAggregator() {

      __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_aggregate );
      __stack.pop_back();
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::addAggregator( const std::string& name,
                                           const std::string& agg_type,
                                           const std::vector<std::string>& chains,
                                           const std::vector<std::string>& params,
                                           std::string                     type ) {
      PRMClass<GUM_SCALAR>* c = static_cast<PRMClass<GUM_SCALAR>*>(
          __checkStack( 1, PRMObject::prm_type::CLASS ) );
      // Checking call legality

      if ( chains.size() == 0 ) {
        GUM_ERROR( OperationNotAllowed,
                   "a PRMAggregate<GUM_SCALAR> requires at least one parent" );
      }

      // Retrieving the parents of the aggregate
      std::vector<PRMClassElement<GUM_SCALAR>*> inputs;

      // This helps knowing if the aggregate has parents outside the current
      // class
      // (see below)
      bool hasSC = __retrieveInputs( c, chains, inputs );

      // Checking that all inputs shares the same PRMType<GUM_SCALAR> (trivial
      // if
      // inputs.size() == 1)
      if ( inputs.size() > 1 ) {
        for ( auto iter = inputs.begin() + 1; iter != inputs.end(); ++iter ) {
          if ( ( **( iter - 1 ) ).type() != ( **iter ).type() ) {
            GUM_ERROR( WrongType, "found different types" );
          }
        }
      }

      // Different treatments for different types of aggregate.
      PRMAggregate<GUM_SCALAR>* agg = nullptr;

      switch ( PRMAggregate<GUM_SCALAR>::str2enum( agg_type ) ) {
        case PRMAggregate<GUM_SCALAR>::AggregateType::OR:
        case PRMAggregate<GUM_SCALAR>::AggregateType::AND: {
          if ( inputs.front()->type() != *( __retrieveType( "boolean" ) ) ) {
            GUM_ERROR( WrongType, "expected booleans" );
          }
          if ( params.size() != 0 ) {
            GUM_ERROR( OperationNotAllowed, "invalid number of paramaters" );
          }

          agg = new PRMAggregate<GUM_SCALAR>(
              name,
              PRMAggregate<GUM_SCALAR>::str2enum( agg_type ),
              inputs.front()->type() );

          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::EXISTS:
        case PRMAggregate<GUM_SCALAR>::AggregateType::FORALL: {
          if ( params.size() != 1 ) {
            GUM_ERROR( OperationNotAllowed, "invalid number of parameters" );
          }

          Idx label_idx = 0;

          while ( label_idx < inputs.front()->type()->domainSize() ) {
            if ( inputs.front()->type()->label( label_idx ) == params.front() ) {
              break;
            }

            ++label_idx;
          }

          if ( label_idx == inputs.front()->type()->domainSize() ) {
            GUM_ERROR( NotFound, "could not find label" );
          }

          // Creating and adding the PRMAggregate<GUM_SCALAR>
          agg = new PRMAggregate<GUM_SCALAR>(
              name,
              PRMAggregate<GUM_SCALAR>::str2enum( agg_type ),
              *( __retrieveType( "boolean" ) ),
              label_idx );
          agg->label();

          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::MEDIAN:
        case PRMAggregate<GUM_SCALAR>::AggregateType::AMPLITUDE:
        case PRMAggregate<GUM_SCALAR>::AggregateType::MIN:
        case PRMAggregate<GUM_SCALAR>::AggregateType::MAX: {
          if ( params.size() != 0 ) {
            GUM_ERROR( OperationNotAllowed, "invalid number of parameters" );
          }

          auto output_type = __retrieveType( type );

          // Creating and adding the PRMAggregate<GUM_SCALAR>
          agg = new PRMAggregate<GUM_SCALAR>(
              name, PRMAggregate<GUM_SCALAR>::str2enum( agg_type ), *output_type );

          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::COUNT: {
          if ( params.size() != 1 ) {
            GUM_ERROR( OperationNotAllowed, "invalid number of parameters" );
          }

          Idx label_idx = 0;

          while ( label_idx < inputs.front()->type()->domainSize() ) {
            if ( inputs.front()->type()->label( label_idx ) == params.front() ) {
              break;
            }

            ++label_idx;
          }

          if ( label_idx == inputs.front()->type()->domainSize() ) {
            GUM_ERROR( NotFound, "could not find label" );
          }

          auto output_type = __retrieveType( type );

          // Creating and adding the PRMAggregate<GUM_SCALAR>
          agg = new PRMAggregate<GUM_SCALAR>(
              name,
              PRMAggregate<GUM_SCALAR>::str2enum( agg_type ),
              *output_type,
              label_idx );

          break;
        }

        default: { GUM_ERROR( FatalError, "Unknown aggregator." ); }
      }

      std::string safe_name = agg->safeName();

      try {
        if ( hasSC ) {
          try {
            c->add( agg );
          } catch ( DuplicateElement& ) {
            c->overload( agg );
          }
        } else {
          // Inner aggregators can be directly used as attributes
          auto attr = new PRMScalarAttribute<GUM_SCALAR>(
              agg->name(), agg->type(), agg->buildImpl() );

          try {
            c->add( attr );
          } catch ( DuplicateElement& ) {
            c->overload( attr );
          }

          delete agg;
        }
      } catch ( DuplicateElement& ) {
        delete agg;
        throw;
      }

      for ( const auto& elt : inputs ) {
        c->addArc( elt->safeName(), safe_name );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addReferenceSlot( const std::string& type,
                                                          const std::string& name,
                                                          bool isArray ) {
      PRMClassElementContainer<GUM_SCALAR>* owner = __checkStackContainter( 1 );
      PRMClassElementContainer<GUM_SCALAR>* slotType = 0;

      try {
        slotType = __retrieveClass( type );
      } catch ( NotFound& ) {
        try {
          slotType = __retrieveInterface( type );
        } catch ( NotFound& ) {
          GUM_ERROR( NotFound, "unknown ReferenceSlot<GUM_SCALAR> slot type" );
        }
      }

      PRMReferenceSlot<GUM_SCALAR>* ref =
          new PRMReferenceSlot<GUM_SCALAR>( name, *slotType, isArray );

      try {
        owner->add( ref );
      } catch ( DuplicateElement& ) {
        owner->overload( ref );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addArray( const std::string& type,
                                                  const std::string& name,
                                                  Size               size ) {
      PRMSystem<GUM_SCALAR>* model = static_cast<PRMSystem<GUM_SCALAR>*>(
          __checkStack( 1, PRMObject::prm_type::SYSTEM ) );
      PRMClass<GUM_SCALAR>*    c = __retrieveClass( type );
      PRMInstance<GUM_SCALAR>* inst = 0;

      try {
        model->addArray( name, *c );

        for ( Size i = 0; i < size; ++i ) {
          std::stringstream elt_name;
          elt_name << name << "[" << i << "]";
          inst = new PRMInstance<GUM_SCALAR>( elt_name.str(), *c );
          model->add( name, inst );
        }
      } catch ( TypeError& ) {
        delete inst;
        throw;
      } catch ( NotFound& ) {
        delete inst;
        throw;
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::incArray( const std::string& l_i,
                                                  const std::string& r_i ) {
      PRMSystem<GUM_SCALAR>* model = static_cast<PRMSystem<GUM_SCALAR>*>(
          __checkStack( 1, PRMObject::prm_type::SYSTEM ) );

      if ( model->isArray( l_i ) ) {
        if ( model->isInstance( r_i ) ) {
          model->add( l_i, model->get( r_i ) );
        } else {
          GUM_ERROR( NotFound, "right value is not an instance" );
        }
      } else {
        GUM_ERROR( NotFound, "left value is no an array" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::setReferenceSlot( const std::string& l_i,
                                              const std::string& l_ref,
                                              const std::string& r_i ) {

      auto model = static_cast<PRMSystem<GUM_SCALAR>*>(
          __checkStack( 1, PRMObject::prm_type::SYSTEM ) );
      std::vector<PRMInstance<GUM_SCALAR>*> lefts;
      std::vector<PRMInstance<GUM_SCALAR>*> rights;

      if ( model->isInstance( l_i ) ) {
        lefts.push_back( &( model->get( l_i ) ) );
      } else if ( model->isArray( l_i ) ) {
        for ( const auto& elt : model->getArray( l_i ) )
          lefts.push_back( elt );
      } else {
        GUM_ERROR( NotFound, "left value does not name an instance or an array" );
      }

      if ( model->isInstance( r_i ) ) {
        rights.push_back( &( model->get( r_i ) ) );
      } else if ( model->isArray( r_i ) ) {
        for ( const auto& elt : model->getArray( r_i ) )
          rights.push_back( elt );
      } else {
        GUM_ERROR( NotFound, "left value does not name an instance or an array" );
      }

      for ( const auto l : lefts ) {
        for ( const auto r : rights ) {

          auto& elt = l->type().get( l_ref );
          if ( PRMClassElement<GUM_SCALAR>::isReferenceSlot( elt ) ) {

            l->add( elt.id(), *r );

          } else {
            GUM_ERROR( NotFound, "unfound reference slot" );
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMSlotChain<GUM_SCALAR>* PRMFactory<GUM_SCALAR>::__buildSlotChain(
        PRMClassElementContainer<GUM_SCALAR>* start, const std::string& name ) {
      std::vector<std::string> v;
      decomposePath( name, v );
      PRMClassElementContainer<GUM_SCALAR>*  current = start;
      PRMReferenceSlot<GUM_SCALAR>*          ref = nullptr;
      Sequence<PRMClassElement<GUM_SCALAR>*> elts;

      for ( size_t i = 0; i < v.size(); ++i ) {
        try {
          switch ( current->get( v[i] ).elt_type() ) {
            case PRMClassElement<GUM_SCALAR>::prm_refslot:
              ref = &( static_cast<PRMReferenceSlot<GUM_SCALAR>&>(
                  current->get( v[i] ) ) );
              elts.insert( ref );
              current = &( /*const_cast<PRMClassElementContainer<GUM_SCALAR>&>*/ (
                  ref->slotType() ) );
              break;

            case PRMClassElement<GUM_SCALAR>::prm_aggregate:
            case PRMClassElement<GUM_SCALAR>::prm_attribute:

              if ( i == v.size() - 1 ) {
                elts.insert( &( current->get( v[i] ) ) );
                break;
              } else {
                return nullptr;
              }

            default: { return nullptr; }
          }
        } catch ( NotFound& ) {
          return nullptr;
        }
      }

      GUM_ASSERT( v.size() == elts.size() );

      current->setOutputNode( *( elts.back() ), true );

      return new PRMSlotChain<GUM_SCALAR>( name, elts );
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMFactory<GUM_SCALAR>::__retrieveInputs(
        PRMClass<GUM_SCALAR>*                      c,
        const std::vector<std::string>&            chains,
        std::vector<PRMClassElement<GUM_SCALAR>*>& inputs ) {
      bool retVal = false;

      for ( size_t i = 0; i < chains.size(); ++i ) {
        try {
          inputs.push_back( &( c->get( chains[i] ) ) );
          retVal = retVal ||
                   PRMClassElement<GUM_SCALAR>::isSlotChain( *( inputs.back() ) );
        } catch ( NotFound& ) {
          inputs.push_back( __buildSlotChain( c, chains[i] ) );
          retVal = true;

          if ( inputs.back() ) {
            c->add( inputs.back() );
          } else {
            GUM_ERROR( NotFound, "unknown slot chain" );
          }
        }
      }

      PRMType<GUM_SCALAR>* t = __retrieveCommonType( inputs );

      std::vector<
          std::pair<PRMClassElement<GUM_SCALAR>*, PRMClassElement<GUM_SCALAR>*>>
          toAdd;

      for ( const auto& elt : inputs ) {
        if ( ( *elt ).type() != ( *t ) ) {
          if ( PRMClassElement<GUM_SCALAR>::isSlotChain( *elt ) ) {
            PRMSlotChain<GUM_SCALAR>* sc =
                static_cast<PRMSlotChain<GUM_SCALAR>*>( elt );
            std::stringstream name;

            for ( Size idx = 0; idx < sc->chain().size() - 1; ++idx ) {
              name << sc->chain().atPos( idx )->name() << ".";
            }

            name << ".(" << t->name() << ")" << sc->lastElt().name();

            try {
              toAdd.push_back( std::make_pair( elt, &( c->get( name.str() ) ) ) );
            } catch ( NotFound& ) {
              toAdd.push_back(
                  std::make_pair( elt, __buildSlotChain( c, name.str() ) ) );
            }
          } else {
            std::stringstream name;
            name << "(" << t->name() << ")" << elt->name();
            toAdd.push_back( std::make_pair( elt, &( c->get( name.str() ) ) ) );
          }
        }
      }

      return retVal;
    }

    template <typename GUM_SCALAR>
    INLINE PRMType<GUM_SCALAR>* PRMFactory<GUM_SCALAR>::__retrieveCommonType(
        const std::vector<PRMClassElement<GUM_SCALAR>*>& elts ) {
      const PRMType<GUM_SCALAR>* current = nullptr;
      HashTable<std::string, Size> counters;
      // Finding all types and super types

      for ( const auto& elt : elts ) {
        try {
          current = &( ( *elt ).type() );

          while ( current != 0 ) {
            // Filling counters
            if ( counters.exists( current->name() ) ) {
              ++( counters[current->name()] );
            } else {
              counters.insert( current->name(), 1 );
            }

            // Loop guard
            if ( current->isSubType() ) {
              current = &( current->superType() );
            } else {
              current = nullptr;
            }
          }
        } catch ( OperationNotAllowed& ) {
          GUM_ERROR( WrongClassElement,
                     "found a ClassElement<GUM_SCALAR> without a type" );
        }
      }

      // We need to find the most specialized (i.e. max depth) common type
      current = nullptr;

      int max_depth = -1;

      int current_depth = 0;

      for ( const auto& elt : counters ) {
        if ( ( elt.second ) == elts.size() ) {
          current_depth = __typeDepth( __retrieveType( elt.first ) );

          if ( current_depth > max_depth ) {
            max_depth = current_depth;
            current = __retrieveType( elt.first );
          }
        }
      }

      if ( current ) {
        return const_cast<PRMType<GUM_SCALAR>*>( current );
      }

      GUM_ERROR( NotFound, "could not find a common type" );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addNoisyOrCompound(
        const std::string&              name,
        const std::vector<std::string>& chains,
        const std::vector<float>&       numbers,
        float                           leak,
        const std::vector<std::string>& labels ) {
      if ( currentType() != PRMObject::prm_type::CLASS ) {
        GUM_ERROR( gum::FactoryInvalidState, "invalid state to add a noisy-or" );
      }

      PRMClass<GUM_SCALAR>* c =
          dynamic_cast<gum::prm::PRMClass<GUM_SCALAR>*>( getCurrent() );

      std::vector<PRMClassElement<GUM_SCALAR>*> parents;

      for ( const auto& elt : chains )
        parents.push_back( &( c->get( elt ) ) );

      PRMType<GUM_SCALAR>* common_type = __retrieveCommonType( parents );

      for ( size_t idx = 0; idx < parents.size(); ++idx ) {
        if ( parents[idx]->type() != ( *common_type ) ) {
          PRMClassElement<GUM_SCALAR>* parent = parents[idx];
          // Either safe_name is an non existing slot chain or an existing cast
          // descendant
          std::string safe_name = parent->cast( *common_type );

          if ( !c->exists( safe_name ) ) {
            if ( PRMClassElement<GUM_SCALAR>::isSlotChain( *parent ) ) {
              parents[idx] = __buildSlotChain( c, safe_name );
              c->add( parents[idx] );
            } else {
              GUM_ERROR( NotFound, "unable to find parent" );
            }
          } else {
            parents[idx] = &( c->get( safe_name ) );
          }
        }
      }

      if ( numbers.size() == 1 ) {
        auto impl =
            new gum::MultiDimNoisyORCompound<GUM_SCALAR>( leak, numbers.front() );
        auto attr = new PRMScalarAttribute<GUM_SCALAR>(
            name, retrieveType( "boolean" ), impl );
        addAttribute( attr );
      } else if ( numbers.size() == parents.size() ) {
        gum::MultiDimNoisyORCompound<GUM_SCALAR>* noisy =
            new gum::MultiDimNoisyORCompound<GUM_SCALAR>( leak );
        gum::prm::PRMFuncAttribute<GUM_SCALAR>* attr =
            new gum::prm::PRMFuncAttribute<GUM_SCALAR>(
                name, retrieveType( "boolean" ), noisy );

        for ( size_t idx = 0; idx < numbers.size(); ++idx ) {
          noisy->causalWeight( parents[idx]->type().variable(), numbers[idx] );
        }

        addAttribute( attr );
      } else {
        GUM_ERROR( OperationNotAllowed, "invalid parameters for a noisy or" );
      }

      if ( !labels.empty() ) {
        GUM_ERROR( OperationNotAllowed,
                   "labels definitions not handle for noisy-or" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMType<GUM_SCALAR>*
    PRMFactory<GUM_SCALAR>::__retrieveType( const std::string& name ) const {

      PRMType<GUM_SCALAR>* type = nullptr;
      std::string          full_name;

      // Looking for the type using its name
      if ( __prm->__typeMap.exists( name ) ) {
        type = __prm->__typeMap[name];
        full_name = name;
      }

      // Looking for the type in current package
      std::string prefixed = __addPrefix( name );
      if ( __prm->__typeMap.exists( prefixed ) ) {
        if ( type == 0 ) {
          type = __prm->__typeMap[prefixed];
          full_name = prefixed;
        } else if ( full_name != prefixed ) {
          GUM_ERROR( DuplicateElement,
                     "Type name '" << name
                                   << "' is ambiguous: specify full name." );
        }
      }

      // Looking for the type relatively to current package
      std::string relatif_ns = currentPackage();
      size_t      last_dot = relatif_ns.find_last_of( '.' );
      if ( last_dot != std::string::npos ) {
        relatif_ns = relatif_ns.substr( 0, last_dot ) + '.' + name;
        if ( __prm->__typeMap.exists( relatif_ns ) ) {
          if ( type == 0 ) {
            type = __prm->__typeMap[relatif_ns];
            full_name = relatif_ns;
          } else if ( full_name != relatif_ns ) {
            GUM_ERROR( DuplicateElement,
                       "Type name '" << name
                                     << "' is ambiguous: specify full name." );
          }
        }
      }


      // Looking for the type using all declared namespaces
      if ( !__namespaces.empty() ) {
        auto ns_list = __namespaces.back();
        for ( gum::Size i = 0; i < ns_list->size(); ++i ) {
          std::string ns = ( *ns_list )[i];
          std::string ns_name = ns + "." + name;
          if ( __prm->__typeMap.exists( ns_name ) ) {
            if ( type == 0 ) {
              type = __prm->__typeMap[ns_name];
              full_name = ns_name;
            } else if ( full_name != ns_name ) {
              GUM_ERROR( DuplicateElement,
                         "Type name '" << name
                                       << "' is ambiguous: specify full name." );
            }
          }
        }
      }

      if ( type == 0 ) {
        GUM_ERROR( NotFound, "Type '" << name << "' not found, check imports." );
      }

      return type;
    }

    template <typename GUM_SCALAR>
    PRMClass<GUM_SCALAR>*
    PRMFactory<GUM_SCALAR>::__retrieveClass( const std::string& name ) const {

      PRMClass<GUM_SCALAR>* a_class = nullptr;
      std::string           full_name;

      // Looking for the type using its name
      if ( __prm->__classMap.exists( name ) ) {
        a_class = __prm->__classMap[name];
        full_name = name;
      }

      // Looking for the type using current package
      std::string prefixed = __addPrefix( name );
      if ( __prm->__classMap.exists( prefixed ) ) {
        if ( a_class == nullptr ) {
          a_class = __prm->__classMap[prefixed];
          full_name = prefixed;
        } else if ( full_name != prefixed ) {
          GUM_ERROR( DuplicateElement,
                     "Class name '" << name
                                    << "' is ambiguous: specify full name." );
        }
      }

      // Looking for the class using all declared namespaces
      if ( !__namespaces.empty() ) {
        auto ns_list = __namespaces.back();
        for ( gum::Size i = 0; i < ns_list->size(); ++i ) {
          std::string ns = ( *ns_list )[i];
          std::string ns_name = ns + "." + name;
          if ( __prm->__classMap.exists( ns_name ) ) {
            if ( a_class == 0 ) {
              a_class = __prm->__classMap[ns_name];
              full_name = ns_name;
            } else if ( full_name != ns_name ) {
              GUM_ERROR( DuplicateElement,
                         "Class name '" << name
                                        << "' is ambiguous: specify full name." );
            }
          }
        }
      }

      if ( a_class == 0 ) {
        GUM_ERROR( NotFound, "Class '" << name << "' not found, check imports." );
      }

      return a_class;
    }

    template <typename GUM_SCALAR>
    PRMInterface<GUM_SCALAR>*
    PRMFactory<GUM_SCALAR>::__retrieveInterface( const std::string& name ) const {

      PRMInterface<GUM_SCALAR>* interface = nullptr;
      std::string               full_name;

      // Looking for the type using its name
      if ( __prm->__interfaceMap.exists( name ) ) {
        interface = __prm->__interfaceMap[name];
        full_name = name;
      }

      // Looking for the type using current package
      std::string prefixed = __addPrefix( name );
      if ( __prm->__interfaceMap.exists( prefixed ) ) {
        if ( interface == nullptr ) {
          interface = __prm->__interfaceMap[prefixed];
          full_name = prefixed;
        } else if ( full_name != prefixed ) {
          GUM_ERROR( DuplicateElement,
                     "Interface name '" << name
                                        << "' is ambiguous: specify full name." );
        }
      }

      // Looking for the interf using all declared namespaces
      if ( !__namespaces.empty() ) {
        auto ns_list = __namespaces.back();
        // for( const auto & ns : *(__namespaces.top()) ) {
        for ( gum::Size i = 0; i < ns_list->size(); ++i ) {
          std::string ns = ( *ns_list )[i];
          std::string ns_name = ns + "." + name;

          if ( __prm->__interfaceMap.exists( ns_name ) ) {
            if ( interface == nullptr ) {
              interface = __prm->__interfaceMap[ns_name];
              full_name = ns_name;
            } else if ( full_name != ns_name ) {
              GUM_ERROR( DuplicateElement,
                         "Interface name '"
                             << name
                             << "' is ambiguous: specify full name." );
            }
          }
        }
      }

      if ( interface == nullptr ) {
        GUM_ERROR( NotFound,
                   "Interface '" << name << "' not found, check imports." );
      }

      return interface;
    }

    template <typename GUM_SCALAR>
    INLINE PRMFactory<GUM_SCALAR>::PRMFactory() {
      GUM_CONSTRUCTOR( PRMFactory );
      __prm = new PRM<GUM_SCALAR>();
    }

    template <typename GUM_SCALAR>
    INLINE PRMFactory<GUM_SCALAR>::PRMFactory( PRM<GUM_SCALAR>* prm )
        : IPRMFactory()
        , __prm( prm ) {
      GUM_CONSTRUCTOR( PRMFactory );
    }

    template <typename GUM_SCALAR>
    INLINE PRMFactory<GUM_SCALAR>::~PRMFactory() {
      GUM_DESTRUCTOR( PRMFactory );
      while ( !__namespaces.empty() ) {
        auto ns = __namespaces.back();
        __namespaces.pop_back();
        delete ns;
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRM<GUM_SCALAR>* PRMFactory<GUM_SCALAR>::prm() const {
      return __prm;
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject::prm_type PRMFactory<GUM_SCALAR>::currentType() const {
      if ( __stack.size() == 0 ) {
        GUM_ERROR( NotFound, "no object being built" );
      }

      return __stack.back()->obj_type();
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject* PRMFactory<GUM_SCALAR>::getCurrent() {
      if ( __stack.size() == 0 ) {
        GUM_ERROR( NotFound, "no object being built" );
      }

      return __stack.back();
    }

    template <typename GUM_SCALAR>
    INLINE const PRMObject* PRMFactory<GUM_SCALAR>::getCurrent() const {
      if ( __stack.size() == 0 ) {
        GUM_ERROR( NotFound, "no object being built" );
      }

      return __stack.back();
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject* PRMFactory<GUM_SCALAR>::closeCurrent() {
      if ( __stack.size() > 0 ) {
        PRMObject* obj = __stack.back();
        __stack.pop_back();
        return obj;
      } else {
        return 0;
      }
    }

    template <typename GUM_SCALAR>
    INLINE std::string PRMFactory<GUM_SCALAR>::currentPackage() const {
      return ( __packages.empty() ) ? "" : __packages.back();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::startDiscreteType( const std::string& name,
                                                           std::string super ) {
      std::string real_name = __addPrefix( name );
      if ( __prm->__typeMap.exists( real_name ) ) {
        std::stringstream msg;
        msg << "\"" << real_name << "\" is already used.";
        GUM_ERROR( DuplicateElement, msg.str() );
      }
      if ( super == "" ) {
        auto t = new PRMType<GUM_SCALAR>( LabelizedVariable( real_name, "", 0 ) );
        __stack.push_back( t );
      } else {
        auto t = new PRMType<GUM_SCALAR>( LabelizedVariable( real_name, "", 0 ) );
        t->__superType = __retrieveType( super );
        t->__label_map = new std::vector<Idx>();
        __stack.push_back( t );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::endDiscreteType() {
      PRMType<GUM_SCALAR>* t = static_cast<PRMType<GUM_SCALAR>*>(
          __checkStack( 1, PRMObject::prm_type::TYPE ) );

      if ( !t->__isValid() ) {
        GUM_ERROR( OperationNotAllowed, "current type is not a valid subtype" );
      } else if ( t->variable().domainSize() < 2 ) {
        GUM_ERROR( OperationNotAllowed,
                   "current type is not a valid discrete type" );
      }

      __prm->__typeMap.insert( t->name(), t );

      __prm->__types.insert( t );
      __stack.pop_back();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::endInterface() {
      __checkStack( 1, PRMObject::prm_type::PRM_INTERFACE );
      __stack.pop_back();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addAttribute( const std::string& type,
                                                      const std::string& name ) {
      __checkStack( 1, PRMObject::prm_type::PRM_INTERFACE );
      startAttribute( type, name );
      endAttribute();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::startAttribute( const std::string& type,
                                                        const std::string& name,
                                                        bool scalar_attr ) {
      PRMClassElementContainer<GUM_SCALAR>* c = __checkStackContainter( 1 );
      PRMAttribute<GUM_SCALAR>*             a = nullptr;

      if ( PRMObject::isClass( *c ) && ( !scalar_attr ) ) {

        a = new PRMFormAttribute<GUM_SCALAR>(
            static_cast<PRMClass<GUM_SCALAR>&>( *c ),
            name,
            *__retrieveType( type ) );

      } else {

        a = new PRMScalarAttribute<GUM_SCALAR>( name, *__retrieveType( type ) );
      }

      std::string dot = ".";

      try {
        try {
          c->add( a );
        } catch ( DuplicateElement& ) {
          c->overload( a );
        }
      } catch ( Exception& ) {
        if ( a != nullptr && ( !c->exists( a->id() ) ) ) {
          delete a;
        }
      }

      __stack.push_back( a );
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::continueAttribute( const std::string& name ) {
      PRMClassElementContainer<GUM_SCALAR>* c = __checkStackContainter( 1 );
      if ( !c->exists( name ) ) {
        GUM_ERROR( NotFound, name << "not found" );
      }
      auto& a = c->get( name );
      if ( !PRMClassElement<GUM_SCALAR>::isAttribute( a ) ) {
        GUM_ERROR( OperationNotAllowed, name << " not an attribute" );
      }
      __stack.push_back( &a );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::endAttribute() {
      __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_attribute );
      __stack.pop_back();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::startSystem( const std::string& name ) {
      if ( __prm->__systemMap.exists( name ) ) {
        std::stringstream msg;
        msg << "\"" << name << "\" is already used.";
        GUM_ERROR( DuplicateElement, msg.str() );
      }
      PRMSystem<GUM_SCALAR>* model =
          new PRMSystem<GUM_SCALAR>( __addPrefix( name ) );
      __stack.push_back( model );
      __prm->__systemMap.insert( model->name(), model );
      __prm->__systems.insert( model );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::endSystem() {
      try {
        PRMSystem<GUM_SCALAR>* model = static_cast<PRMSystem<GUM_SCALAR>*>(
            __checkStack( 1, PRMObject::prm_type::SYSTEM ) );
        __stack.pop_back();
        model->instantiate();
      } catch ( Exception& ) {
        GUM_ERROR( FatalError, "could not create system" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addInstance( const std::string& type,
                                                     const std::string& name ) {

      auto c = __retrieveClass( type );

      // If class contains parameters, calls the proper addIsntance method
      if ( c->parameters().size() > 0 ) {

        HashTable<std::string, double> params;
        addInstance( type, name, params );

      } else {

        __addInstance( c, name );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addInstance(
        const std::string& type,
        const std::string& name,
        const HashTable<std::string, double>& params ) {

      auto c = __retrieveClass( type );

      if ( c->parameters().empty() ) {

        if ( params.empty() ) {
          __addInstance( c, name );
        } else {
          GUM_ERROR( OperationNotAllowed,
                     "Class " + type + " does not have parameters" );
        }

      } else {

        auto my_params = params;
        // Adding all parameters to my_params
        for ( const auto& p : c->parameters() ) {

          if ( !my_params.exists( p->name() ) ) {
            my_params.insert( p->name(), p->value() );
          }
        }

        // Building sub class name using my_params
        std::stringstream sBuff;
        sBuff << c->name() << "<";

        for ( const auto& p : my_params ) {
          sBuff << p.first << "=" << p.second << ",";
        }

        // Removing last , and adding closing >
        std::string sub_c = sBuff.str().substr( 0, sBuff.str().size() - 1 ) + ">";

        // Adding class in current package
        try {

          auto pck_cpy = __packages;
          __packages.clear();

          startClass( sub_c, c->name() );

          // Update inherited parameters
          for ( auto p : my_params ) {

            auto type = static_cast<PRMParameter<GUM_SCALAR>&>( c->get( p.first ) )
                            .valueType();
            if ( type == PRMParameter<GUM_SCALAR>::ParameterType::INT ) {

              addParameter( "int", p.first, p.second );

            } else {

              addParameter( "real", p.first, p.second );
            }
          }

          endClass();

          __packages = pck_cpy;

        } catch ( DuplicateElement& ) {
          // Sub Class already exists in this system
        }
        c = __retrieveClass( sub_c );
        __addInstance( c, name );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::__addInstance( PRMClass<GUM_SCALAR>* type,
                                                       const std::string& name ) {
      PRMInstance<GUM_SCALAR>* i = nullptr;
      try {

        auto s = static_cast<PRMSystem<GUM_SCALAR>*>(
            __checkStack( 1, PRMObject::prm_type::SYSTEM ) );
        i = new PRMInstance<GUM_SCALAR>( name, *type );
        s->add( i );

      } catch ( OperationNotAllowed& ) {

        if ( i ) {
          delete i;
        }
        throw;
      }
    }

    template <typename GUM_SCALAR>
    INLINE std::string
    PRMFactory<GUM_SCALAR>::__addPrefix( const std::string& str ) const {
      if ( !__packages.empty() ) {
        std::string full_name = __packages.back();
        full_name.append( "." );
        full_name.append( str );
        return full_name;
      } else {
        return str;
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject*
    PRMFactory<GUM_SCALAR>::__checkStack( Idx i, PRMObject::prm_type obj_type ) {
      // Don't forget that Idx are unsigned int
      if ( __stack.size() - i > __stack.size() ) {
        GUM_ERROR( FactoryInvalidState, "illegal sequence of calls" );
      }

      PRMObject* obj = __stack[__stack.size() - i];

      if ( obj->obj_type() != obj_type ) {
        GUM_ERROR( FactoryInvalidState, "illegal sequence of calls" );
      }

      return obj;
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElementContainer<GUM_SCALAR>*
    PRMFactory<GUM_SCALAR>::__checkStackContainter( Idx i ) {
      // Don't forget that Idx are unsigned int
      if ( __stack.size() - i > __stack.size() ) {
        GUM_ERROR( FactoryInvalidState, "illegal sequence of calls" );
      }

      PRMObject* obj = __stack[__stack.size() - i];

      if ( ( obj->obj_type() == PRMObject::prm_type::CLASS ) ||
           ( obj->obj_type() == PRMObject::prm_type::PRM_INTERFACE ) ) {
        return static_cast<PRMClassElementContainer<GUM_SCALAR>*>( obj );
      } else {
        GUM_ERROR( FactoryInvalidState, "illegal sequence of calls" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElement<GUM_SCALAR>* PRMFactory<GUM_SCALAR>::__checkStack(
        Idx i, typename PRMClassElement<GUM_SCALAR>::ClassElementType elt_type ) {
      // Don't forget that Idx are unsigned int
      if ( __stack.size() - i > __stack.size() ) {
        GUM_ERROR( FactoryInvalidState, "illegal sequence of calls" );
      }

      PRMClassElement<GUM_SCALAR>* obj =
          dynamic_cast<PRMClassElement<GUM_SCALAR>*>(
              __stack[__stack.size() - i] );

      if ( obj == 0 ) {
        GUM_ERROR( FactoryInvalidState, "illegal sequence of calls" );
      }

      if ( obj->elt_type() != elt_type ) {
        GUM_ERROR( FactoryInvalidState, "illegal sequence of calls" );
      }

      return obj;
    }

    template <typename GUM_SCALAR>
    INLINE int
    PRMFactory<GUM_SCALAR>::__typeDepth( const PRMType<GUM_SCALAR>* t ) {
      int                        depth = 0;
      const PRMType<GUM_SCALAR>* current = t;

      while ( current->isSubType() ) {
        ++depth;
        current = &( current->superType() );
      }

      return depth;
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::pushPackage( const std::string& name ) {
      __packages.push_back( name );
      __namespaces.push_back( new List<std::string>() );
    }

    template <typename GUM_SCALAR>
    INLINE std::string PRMFactory<GUM_SCALAR>::popPackage() {
      std::string plop = currentPackage();

      if ( !__packages.empty() ) {
        std::string s = __packages.back();
        __packages.pop_back();

        if ( __namespaces.size() > 0 ) {
          delete __namespaces.back();
          __namespaces.pop_back();
        }
        return s;
      }

      return plop;
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::addImport( const std::string& name ) {
      if ( name.size() == 0 ) {
        GUM_ERROR( OperationNotAllowed, "illegal import name" );
      }
      if ( __namespaces.empty() ) {
        __namespaces.push_back( new List<std::string>() );
      }
      __namespaces.back()->push_back( name );
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMFactory<GUM_SCALAR>::setReferenceSlot( const std::string& l_i,
                                              const std::string& r_i ) {
      size_t pos = l_i.find_last_of( '.' );

      if ( pos != std::string::npos ) {
        std::string l_ref = l_i.substr( pos + 1, std::string::npos );
        setReferenceSlot( l_i.substr( 0, pos ), l_ref, r_i );
      } else {
        GUM_ERROR( NotFound, "left value does not name an instance or an array" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMClass<GUM_SCALAR>&
    PRMFactory<GUM_SCALAR>::retrieveClass( const std::string& name ) {
      return *__retrieveClass( name );
    }

    template <typename GUM_SCALAR>
    INLINE PRMType<GUM_SCALAR>&
    PRMFactory<GUM_SCALAR>::retrieveType( const std::string& name ) {
      return *__retrieveType( name );
    }

    template <typename GUM_SCALAR>
    INLINE PRMType<GUM_SCALAR>& PRMFactory<GUM_SCALAR>::retrieveCommonType(
        const std::vector<PRMClassElement<GUM_SCALAR>*>& elts ) {
      return *( __retrieveCommonType( elts ) );
    }


    template <typename GUM_SCALAR>
    INLINE bool
    PRMFactory<GUM_SCALAR>::isClassOrInterface( const std::string& type ) const {
      try {

        __retrieveClass( type );
        return true;

      } catch ( NotFound& ) {
      } catch ( DuplicateElement& ) {
      }

      try {

        __retrieveInterface( type );
        return true;

      } catch ( NotFound& ) {
      } catch ( DuplicateElement& ) {
      }

      return false;
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMFactory<GUM_SCALAR>::isArrayInCurrentSystem(
        const std::string& name ) const {
      const PRMSystem<GUM_SCALAR>* system =
          static_cast<const PRMSystem<GUM_SCALAR>*>( getCurrent() );
      return ( system && system->isArray( name ) );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::setRawCPFByColumns(
        const std::vector<std::string>& array ) {
      __checkStack( 2, PRMObject::prm_type::CLASS );

      auto a = static_cast<PRMFormAttribute<GUM_SCALAR>*>(
          __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_attribute ) );

      if ( a->formulas().domainSize() != array.size() ) {
        GUM_ERROR( OperationNotAllowed, "illegal CPF size" );
      }

      if ( a->formulas().nbrDim() == 1 ) {

        setRawCPFByLines( array );

      } else {

        Instantiation inst( a->formulas() );
        Instantiation jnst;
        for ( auto idx = inst.variablesSequence().rbegin();
              idx != inst.variablesSequence().rend();
              --idx ) {
          jnst.add( **idx );
        }

        jnst.begin();
        auto idx = (std::size_t)0;
        while ( ( !jnst.end() ) && idx < array.size() ) {
          inst.setVals( jnst );
          a->formulas().set( inst, array[idx] );
          jnst.inc();
          ++idx;
        }

        // Generate cpf by calling it
        a->cpf();
      }
    }

    template <typename GUM_SCALAR>
    INLINE void PRMFactory<GUM_SCALAR>::setRawCPFByLines(
        const std::vector<std::string>& array ) {
      __checkStack( 2, PRMObject::prm_type::CLASS );

      auto a = static_cast<PRMFormAttribute<GUM_SCALAR>*>(
          __checkStack( 1, PRMClassElement<GUM_SCALAR>::prm_attribute ) );

      if ( a->formulas().domainSize() != array.size() ) {
        GUM_ERROR( OperationNotAllowed, "illegal CPF size" );
      }

      a->formulas().populate( array );

      /// Generate CPF
      a->cpf();
    }

  } /* namespace prm */
} /* namespace gum */
