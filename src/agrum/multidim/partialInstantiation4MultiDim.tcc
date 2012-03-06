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
/** @file
 * @brief Efficient functionals for instantiating some variables in
 * multiDimensional tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// allow partialInstantiationPatterns to be used
#define GUM_PARTIAL_INSTANTIATION_PATTERN_ALLOWED


#include <agrum/multidim/partialInstantiationRegister4MultiDim.h>


#define GUM_MULTI_DIM_DECORATOR_PARTIAL_INST(NAME)                    \
  namespace gum{ \
  template<typename T_DATA>                                           \
  MultiDimImplementation<T_DATA>*                                     \
  NAME ( const MultiDimDecorator<T_DATA>& table,                      \
         const HashTable<const DiscreteVariable *,Idx>& inst_vars ) { \
    const MultiDimImplementation<T_DATA>* impl = table.content ();    \
    return NAME ( *impl, inst_vars );                                 \
  }\
  } 


  
  // ==============================================================================
  /// a specialized partial instantiation function for multiDimArrays
  // ==============================================================================

  #define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME partialInstantiationMultiDimArray
  #include <agrum/multidim/patterns/partialInstantiationPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME
  

  #define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_IMPL2ARRAY_NAME partialInstantiationMultiDimArray
  #include <agrum/multidim/patterns/partialInstantiationPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_IMPL2ARRAY_NAME

  
  #define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_NAME partialInstantiationMultiDimArray4Pointers
  #include <agrum/multidim/patterns/partialInstantiationPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_NAME

  #define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_IMPL2ARRAY_NAME partialInstantiationMultiDimArray4Pointers
  #include <agrum/multidim/patterns/partialInstantiationPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_IMPL2ARRAY_NAME


  

  // ==============================================================================
  /// default "basename" functions for instantiating MultiDimImplementations 
  // ==============================================================================

  #define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME partialInstantiationMultiDimImplementation
  #include <agrum/multidim/patterns/partialInstantiationPattern4BaseName.h>
  #undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME

  #define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_NAME partialInstantiationMultiDimImplementation4Pointers
  #include <agrum/multidim/patterns/partialInstantiationPattern4BaseName.h>
  #undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME



  
  // ==============================================================================
  // the operators that should be used to select appropriately the functions
  // to partially instantiate multiDims
  // ==============================================================================
 
  /// the function to be used to partially instantiate a MultiDimImplementation 
  #define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME partialInstantiation
  #include <agrum/multidim/patterns/partialInstantiationPattern4MultiDimImplementation.h>
  #undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME

  /// the function to be used to partially instantiate a MultiDimDecorator
  GUM_MULTI_DIM_DECORATOR_PARTIAL_INST( partialInstantiation )
 

  

  // ==============================================================================
  // ==============================================================================
  //
  // DO NOT FORGET TO REGISTER YOUR BINARY FUNCTIONS
  //
  // ==============================================================================
  // ==============================================================================

namespace gum {
  
  /// the function used to register all the above functions
  template<typename T_DATA>
  void partialInstantiation4MultiDimInit () {
    static bool first_init = true;

    if ( first_init ) {
      first_init = false;
      
      std::string MultiDimArrayString ("MultiDimArray");     
      std::string MultiDimDecisionDiagramString ("MultiDimDecisionDiagram");
      std::string BaseNameString ("MultiDimImplementation");
    
      // register base functions for multiDimArrays
      registerPartialInstantiation<T_DATA>
        ( "i", MultiDimArrayString,
          &partialInstantiationMultiDimArray );
    
      // register default basename functions 
      registerPartialInstantiation<T_DATA>
        ( "i", BaseNameString,
          &partialInstantiationMultiDimImplementation );
    }
  }
  
  
  /// the function used to register all the above functions
  template<typename T_DATA>
  void pointerPartialInstantiation4MultiDimInit () {
    static bool first_init = true;
    
    if ( first_init ) {
      first_init = false;
      
      std::string MultiDimArrayString ("MultiDimArray");
      std::string BaseNameString ("MultiDimImplementation");

      // register base functions for multiDimArrays
      registerPartialInstantiation<T_DATA*>
        ( "i", MultiDimArrayString,
          &partialInstantiationMultiDimArray4Pointers);

      // register default basename functions 
      registerPartialInstantiation<T_DATA*>
        ( "i", BaseNameString,
          &partialInstantiationMultiDimImplementation4Pointers );
    }
  }
  

} /* namespace gum */


// remove permission to use operatorsPatterns
#undef GUM_PARTIAL_INSTANTIATION_PATTERN_ALLOWED


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
