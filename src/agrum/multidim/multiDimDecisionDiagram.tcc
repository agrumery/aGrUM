/***************************************************************************
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
* @brief Template implementation of MultiDimDecisionDiagram.h class.
*
* @author Jean-Christophe Magnan
*/
// ============================================================================
#include <agrum/multidim/multiDimDecisionDiagram.h>
// ============================================================================

namespace gum {

/* ******************************************************************************************** */
/*                                                */
/*                CONSTRUCTOR, DESTRUCTOR & CO                  */
/*                                                */
/* ******************************************************************************************** */

  // ==============================================================================
  // Default constructor
  // ==============================================================================
  template<typename T_DATA,template <class> class IApproximationPolicy>
  MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >::MultiDimDecisionDiagram( ):
    MultiDimDecisionDiagramBase<T_DATA>( ),  IApproximationPolicy<T_DATA>() {
    
    GUM_CONSTRUCTOR( MultiDimDecisionDiagram ) ;
    
  }


  // ==============================================================================
  // Constructor with approximation initialisation
  // ==============================================================================
  template<typename T_DATA,template <class> class IApproximationPolicy>
  MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >::MultiDimDecisionDiagram( const MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >& source ):
    MultiDimDecisionDiagramBase<T_DATA>( ),  IApproximationPolicy<T_DATA>( source ) {
      
    GUM_CONSTRUCTOR( MultiDimDecisionDiagram ) ;
    
  }


  // ==============================================================================
  // Constructor with approximation initialisation
  // ==============================================================================
  template<typename T_DATA,template <class> class IApproximationPolicy>
  MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >::MultiDimDecisionDiagram( const IApproximationPolicy<T_DATA>& source ):
    MultiDimDecisionDiagramBase<T_DATA>( ), IApproximationPolicy<T_DATA>( source ) {
      
    GUM_CONSTRUCTOR( MultiDimDecisionDiagram ) ;
    
  }


  // ==============================================================================
  // destructor
  // ==============================================================================
  template<typename T_DATA,template <class> class IApproximationPolicy>
  MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >::~MultiDimDecisionDiagram() {
    
    GUM_DESTRUCTOR( MultiDimDecisionDiagram );
    
  }


  // ==============================================================================
  // This method creates a clone of this object, without its content
  // (including variable), you must use this method if you want to ensure
  // that the generated object has the same type than the object containing
  // the called newFactory()
  // For example :
  //   MultiDimArray<double> y;
  //   MultiDimContainer<double>* x = y.newFactory();
  // Then x is a MultiDimArray<double>*
  //
  // @warning you must desallocate by yourself the memory
  // @return an empty clone of this object with the same type
  // ==============================================================================
  template<typename T_DATA,template <class> class IApproximationPolicy>
  MultiDimContainer<T_DATA>*
  MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >::newFactory() const {
    
    return new MultiDimDecisionDiagram<T_DATA, IApproximationPolicy>( *this );
    
  }
  

  // ==============================================================================
  // Returns a factory that used same approximation pattern
  // ==============================================================================
  template< typename T_DATA, template < class > class IApproximationPolicy >
  MultiDimDecisionDiagramFactoryBase<T_DATA>*
  MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >::getFactory() const {
    
    MultiDimDecisionDiagramFactory< T_DATA, IApproximationPolicy >* ret = new MultiDimDecisionDiagramFactory<T_DATA, IApproximationPolicy>( *this );

    return ret;
    
  }
  

  // ==============================================================================
  // Returns a factory that used same approximation pattern
  // ==============================================================================
  template<typename T_DATA,template <class> class IApproximationPolicy>
  MultiDimDecisionDiagramFactoryBase<T_DATA>*
  MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >::getFactory( const ApproximationPolicy<T_DATA>& source ) const {
    
    MultiDimDecisionDiagramFactory< T_DATA, IApproximationPolicy >* ret = new MultiDimDecisionDiagramFactory<T_DATA, IApproximationPolicy>( dynamic_cast<const IApproximationPolicy<T_DATA>& >( source ) );

    return ret;
    
  }
  

  // ==============================================================================
  // Returns a factory that used same approximation pattern
  // ==============================================================================
  template<typename T_DATA,template <class> class IApproximationPolicy>
  MultiDimDecisionDiagramFactoryBase<T_DATA>*
  MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >::getFactory( const ApproximationPolicy<T_DATA>& source,T_DATA newLowLimit,T_DATA newHighLimit ) const {
  MultiDimDecisionDiagramFactory< T_DATA, IApproximationPolicy >* ret = new MultiDimDecisionDiagramFactory<T_DATA, IApproximationPolicy>( dynamic_cast<const IApproximationPolicy<T_DATA>& >( source ) );
  ret->setLimits( newLowLimit,newHighLimit );
  return ret;
  }
// ==============================================================================
// Returns a factory that used same approximation pattern
// Allows to set parameter for that approximation
// ==============================================================================
/*template<typename T_DATA,template <class> class IApproximationPolicy>
MultiDimDecisionDiagramFactoryBase<T_DATA>*
MultiDimDecisionDiagram< T_DATA, IApproximationPolicy >::getFactory( T_DATA gepsilon, T_DATA glowLimit, T_DATA ghighLimit ) const {
MultiDimDecisionDiagramFactory< T_DATA, IApproximationPolicy >* ret = new MultiDimDecisionDiagramFactory<T_DATA, IApproximationPolicy>();
ret->setEpsilon( gepsilon );
ret->setLowLimit( glowLimit );
ret->setHighLimit( ghighLimit );

return ret;
}*/
}
