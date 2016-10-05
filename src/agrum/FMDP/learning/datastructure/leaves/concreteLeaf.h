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
 * @brief Headers of the Concrete Leaf class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_CONCRETE_LEAF_H
#define GUM_CONCRETE_LEAF_H
// =========================================================================
#include <agrum/config.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/multiPriorityQueue.h>
#include <agrum/core/sequence.h>
// =========================================================================
#include <agrum/graphs/graphElements.h>
// =========================================================================
#include <agrum/FMDP/learning/core/templateStrategy.h>
#include <agrum/FMDP/learning/datastructure/leaves/abstractLeaf.h>
#include <agrum/FMDP/learning/datastructure/nodeDatabase.h>
// =========================================================================

namespace gum {

  /**
   * @class ConcreteLeaf concreteLeaf.h
   * <agrum/FMDP/learning/datastructure/leaves/concreteLeaf.h>
   * @brief A concrete leaf
   * @ingroup fmdp_group
   *
   */


  template <TESTNAME AttributeSelection, bool isScalar>
  class ConcreteLeaf : public AbstractLeaf {

    typedef typename ValueSelect<isScalar, double, Idx>::type ValueType;

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    ConcreteLeaf( NodeId leafId,
                  NodeDatabase<AttributeSelection, isScalar>* n1,
                  const Sequence<ValueType>* valueDomain )
        : AbstractLeaf( leafId )
        , __n1( n1 )
        , __valueDomain( valueDomain ) {
      GUM_CONSTRUCTOR( ConcreteLeaf )
    }

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~ConcreteLeaf() { GUM_DESTRUCTOR( ConcreteLeaf ) }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new( size_t s ) {
      return SmallObjectAllocator::instance().allocate( s );
    }
    void operator delete( void* p ) {
      SmallObjectAllocator::instance().deallocate( p, sizeof( ConcreteLeaf ) );
    }

    /// @}

    // ###################################################################
    /// Gaves the leaf effectif for given modality
    // ###################################################################
    virtual double effectif( Idx moda ) const {
      return __effectif( moda, Int2Type<isScalar>() );
    }

    private:
    double __effectif( Idx moda, Int2Type<true> ) const {
      return (double)__n1->effectif( Idx( __valueDomain->atPos( moda ) ) );
    }
    double __effectif( Idx moda, Int2Type<false> ) const {
      return (double)__n1->effectif( moda );
    }

    public:
    virtual double total() const { return __n1->nbObservation(); }

    Idx nbModa() const { return __nbModa( Int2Type<isScalar>() ); }

    private:
    Idx __nbModa( Int2Type<true> ) const { return __valueDomain->size(); }
    Idx __nbModa( Int2Type<false> ) const { return __n1->valueDomain(); }

    public:
    std::string toString() {
      std::stringstream ss;
      ss << "{ Id : " << this->id() << "}";
      return ss.str();
    }

    private:
    NodeDatabase<AttributeSelection, isScalar>* __n1;
    const Sequence<ValueType>* __valueDomain;
  };


} /* namespace gum */


#endif  // GUM_CONCRETE_LEAF_H
