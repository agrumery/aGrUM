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
 * @brief Efficient functionals for combining multiDims
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// allow operatorsPatterns to be used
#define GUM_OPERATOR_PATTERN_ALLOWED


#include <agrum/multidim/operatorRegister4MultiDim.h>

#define GUM_MULTI_DIM_DECORATOR_OP(NAME,OP)       \
  template<typename T> \
  MultiDimImplementation<T>* \
  NAME ( const MultiDimDecorator<T>& t1, const MultiDimDecorator<T>& t2) { \
    const MultiDimImplementation<T>* impl1 = t1.getContent (); \
    const MultiDimImplementation<T>* impl2 = t2.getContent (); \
    return ( *impl1 OP *impl2 ); \
  }
 

namespace gum {

  /* ******************************************************************************************* */
  /*                                                                                             */
  /*                               MultiDimArrays functions                                      */
  /*                                                                                             */
  /* ******************************************************************************************* */
  
  // ==============================================================================
  /// a specialized function for summing two multiDimArrays
  // ==============================================================================
  // addition taking in argument 2 multiDimArrays
  #define GUM_MULTI_DIM_OPERATOR_NAME add2MultiDimArrays
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) + (y)
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME add2MultiDimArrays
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) + (y)
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME
  #undef GUM_MULTI_DIM_OPERATOR
  
  #define GUM_MULTI_DIM_OPERATOR_POINTER_NAME add2MultiDimArrays4Pointers
  #define GUM_MULTI_DIM_OPERATOR(x,y) new T ( (*x) + (*y) )
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_POINTER_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME add2MultiDimArrays4Pointers
  #define GUM_MULTI_DIM_OPERATOR(x,y) new T ( (*x) + (*y) )
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME
  #undef GUM_MULTI_DIM_OPERATOR
  

  // ==============================================================================
  /// a specialized function for subtracting two multiDimArrays
  // ==============================================================================
  #define GUM_MULTI_DIM_OPERATOR_NAME subtract2MultiDimArrays
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) - (y)
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME subtract2MultiDimArrays
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) - (y)
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_POINTER_NAME subtract2MultiDimArrays4Pointers
  #define GUM_MULTI_DIM_OPERATOR(x,y) new T ( (*x) - (*y) )
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_POINTER_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME subtract2MultiDimArrays4Pointers
  #define GUM_MULTI_DIM_OPERATOR(x,y) new T ( (*x) - (*y) )
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  
  // ==============================================================================
  /// a specialized function for multiplying two multiDimArrays
  // ==============================================================================
  #define GUM_MULTI_DIM_OPERATOR_NAME multiply2MultiDimArrays
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) * (y)
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME multiply2MultiDimArrays
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) * (y)
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_POINTER_NAME multiply2MultiDimArrays4Pointers
  #define GUM_MULTI_DIM_OPERATOR(x,y) new T ( (*x) * (*y) )
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_POINTER_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME multiply2MultiDimArrays4Pointers
  #define GUM_MULTI_DIM_OPERATOR(x,y) new T ( (*x) * (*y) )
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME
  #undef GUM_MULTI_DIM_OPERATOR

   
  // ==============================================================================
  /// a specialized function for dividing two multiDimArrays
  // ==============================================================================
  #define GUM_MULTI_DIM_OPERATOR_NAME divide2MultiDimArrays
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) / (y)
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR
 
  #define GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME divide2MultiDimArrays
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) / (y)
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_POINTER_NAME divide2MultiDimArrays4Pointers
  #define GUM_MULTI_DIM_OPERATOR(x,y) new T ( (*x) / (*y) )
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_POINTER_NAME
  #undef GUM_MULTI_DIM_OPERATOR
 
  #define GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME divide2MultiDimArrays4Pointers
  #define GUM_MULTI_DIM_OPERATOR(x,y) new T ( (*x) / (*y) )
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME
  #undef GUM_MULTI_DIM_OPERATOR
  
  
  

  /* ******************************************************************************************* */
  /*                                                                                             */
  /*                          MultiDimDecisionDiagram functions                                  */
  /*                                                                                             */
  /* ******************************************************************************************* */
  
  // ==============================================================================
  /// a specialized function for summing two multiDimDecisionDiagram
  // ==============================================================================
  // addition taking in argument 2 multiDimDecisionDiagram
  
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) + (y)
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION DecisionDiagramRecur4Addition
  
  #define GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION DecisionDiagramGoDownOnLeader4Addition
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionDiagramGoDownOnFollower4Addition
  #include <agrum/multidim/patterns/unsafeDecisionDiagramRecursionFunctions.h>
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION
  #undef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  
  #define GUM_MULTI_DIM_OPERATOR_NAME add2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME

  #define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME add2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME
  
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION
  #undef GUM_MULTI_DIM_OPERATOR
  

  // ==============================================================================
  /// a specialized function for subtracting two multiDimDecisionDiagrams
  // ==============================================================================
  
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) - (y)
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION DecisionDiagramRecur4Subtraction
  
  #define GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION DecisionDiagramGoDownOnLeader4Subtraction
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionDiagramGoDownOnFollower4Subtraction
  #include <agrum/multidim/patterns/unsafeDecisionDiagramRecursionFunctions.h>
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION
  #undef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  
  #define GUM_MULTI_DIM_OPERATOR_NAME subtract2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME

  #define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME subtract2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME
  
  #undef GUM_MULTI_DIM_OPERATOR
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION

  
  // ==============================================================================
  /// a specialized function for multiplying two multiDimDecisionDiagrams
  // ==============================================================================
  
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) * (y)
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION DecisionDiagramRecur4Multiplication
  
  #define GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION DecisionDiagramGoDownOnLeader4Multiplication
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionDiagramGoDownOnFollower4Multiplication
  #include <agrum/multidim/patterns/unsafeDecisionDiagramRecursionFunctions.h>
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION
  #undef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  
  #define GUM_MULTI_DIM_OPERATOR_NAME multiply2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME

  #define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME multiply2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME
  
  #undef GUM_MULTI_DIM_OPERATOR
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION

   
  // ==============================================================================
  /// a specialized function for dividing two multiDimDecisionDiagrams
  // ==============================================================================
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) / (y)
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION DecisionDiagramRecur4Division
  
  #define GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION DecisionDiagramGoDownOnLeader4Division
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionDiagramGoDownOnFollower4Division
  #include <agrum/multidim/patterns/unsafeDecisionDiagramRecursionFunctions.h>
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION
  #undef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  
  #define GUM_MULTI_DIM_OPERATOR_NAME divide2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME

  #define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME divide2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME
  
  #undef GUM_MULTI_DIM_OPERATOR
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION

   
  // ==============================================================================
  /// a specialized function for finding max of two multiDimDecisionDiagrams
  // ==============================================================================
  #define GUM_MULTI_DIM_OPERATOR(x,y) ( x >= y ? x : y )
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION DecisionDiagramRecur4Maximization
  
  #define GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION DecisionDiagramGoDownOnLeader4Maximization
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionDiagramGoDownOnFollower4Maximization
  #include <agrum/multidim/patterns/unsafeDecisionDiagramRecursionFunctions.h>
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION
  #undef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  
  #define GUM_MULTI_DIM_OPERATOR_NAME maximize2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME

  #define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME maximize2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME
  
  #undef GUM_MULTI_DIM_OPERATOR
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION

   
  // ==============================================================================
  /// a specialized function for finding min of two multiDimDecisionDiagrams
  // ==============================================================================
  #define GUM_MULTI_DIM_OPERATOR(x,y) ( x <= y ? x : y )
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION DecisionDiagramRecur4Minimization
  
  #define GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION DecisionDiagramGoDownOnLeader4Minimization
  #define GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION DecisionDiagramGoDownOnFollower4Minimization
  #include <agrum/multidim/patterns/unsafeDecisionDiagramRecursionFunctions.h>
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION
  #undef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
  
  #define GUM_MULTI_DIM_OPERATOR_NAME minimize2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME

  #define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME minimize2MultiDimDecisionDiagrams
  #include <agrum/multidim/patterns/operatorPattern4MultiDimDecisionDiagram.h>
  #undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME
  
  #undef GUM_MULTI_DIM_OPERATOR
  #undef GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION

  
  // ==============================================================================
  /// a specialized function for functionally combining two multiDimArrays
  // ==============================================================================
  #define GUM_MULTI_DIM_OPERATOR_NAME_F combine2MultiDimArrays
  #define GUM_MULTI_DIM_OPERATOR(x,y) f( (x),(y) )
  #include <agrum/multidim/patterns/operatorPattern4MultiDimArray.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME_F
  #undef GUM_MULTI_DIM_OPERATOR

  
  // ==============================================================================
  // the operators that should be used to select appropriately the functions
  // to combine multiDims
  // ==============================================================================

  /// the function to be used to add two MultiDimImplementations
  #define GUM_MULTI_DIM_OPERATOR_NAME operator+
  #define GUM_MULTI_DIM_OPERATOR_FUNC_NAME "+"
  #define GUM_MULTI_DIM_SYMMETRIC_OPERATOR
  #include <agrum/multidim/patterns/operatorPattern4MultiDimImplementation.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR_FUNC_NAME
  #undef GUM_MULTI_DIM_SYMMETRIC_OPERATOR

  /// the function to be used to add two MultiDimDecorators
  GUM_MULTI_DIM_DECORATOR_OP( operator+, + )

  
  /// the function to be used to multiply two MultiDimImplementations
  #define GUM_MULTI_DIM_OPERATOR_NAME operator*
  #define GUM_MULTI_DIM_OPERATOR_FUNC_NAME "*"
  #define GUM_MULTI_DIM_SYMMETRIC_OPERATOR
  #include <agrum/multidim/patterns/operatorPattern4MultiDimImplementation.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR_FUNC_NAME
  #undef GUM_MULTI_DIM_SYMMETRIC_OPERATOR

  /// the function to be used to multiply two MultiDimDecorators
  GUM_MULTI_DIM_DECORATOR_OP( operator*, * )    
  
  /// the function to be used to subtract two MultiDimImplementations
  #define GUM_MULTI_DIM_OPERATOR_NAME operator-
  #define GUM_MULTI_DIM_OPERATOR_FUNC_NAME "-"
  #include <agrum/multidim/patterns/operatorPattern4MultiDimImplementation.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR_FUNC_NAME

  /// the function to be used to subtract two MultiDimDecorators
  GUM_MULTI_DIM_DECORATOR_OP( operator-, - )
  
  /// the function to be used to divide two MultiDimImplementations
  #define GUM_MULTI_DIM_OPERATOR_NAME operator/
  #define GUM_MULTI_DIM_OPERATOR_FUNC_NAME "/"
  #include <agrum/multidim/patterns/operatorPattern4MultiDimImplementation.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR_FUNC_NAME

  /// the function to be used to divide two MultiDimDecorators
  GUM_MULTI_DIM_DECORATOR_OP( operator/, / )




  // ==============================================================================
  /// default "basename" functions for combining two MultiDimImplementations 
  // ==============================================================================

  #define GUM_MULTI_DIM_OPERATOR_NAME add2MultiDimImplementations
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) + (y)
  #include <agrum/multidim/patterns/operatorPattern4BaseName.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_NAME subtract2MultiDimImplementations
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) - (y)
  #include <agrum/multidim/patterns/operatorPattern4BaseName.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_NAME multiply2MultiDimImplementations
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) * (y)
  #include <agrum/multidim/patterns/operatorPattern4BaseName.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR

  #define GUM_MULTI_DIM_OPERATOR_NAME divide2MultiDimImplementations
  #define GUM_MULTI_DIM_OPERATOR(x,y) (x) / (y)
  #include <agrum/multidim/patterns/operatorPattern4BaseName.h>
  #undef GUM_MULTI_DIM_OPERATOR_NAME
  #undef GUM_MULTI_DIM_OPERATOR
  

  // ==============================================================================
  // ==============================================================================
  //
  // DO NOT FORGET TO REGISTER YOUR BINARY FUNCTIONS
  //
  // ==============================================================================
  // ==============================================================================

  /// the function used to register all the above functions
  template<typename T_DATA>
  void operators4MultiDimInit () {
    static bool first_init = true;

    if ( first_init ) {
      first_init = false;
      
      std::string MultiDimArrayString ("MultiDimArray");      
      std::string MultiDimDecisionDiagramString ("MultiDimDecisionDiagram");
      std::string BaseNameString ("MultiDimImplementation");
    
      // register base functions for multiDimArrays
      registerOperator<T_DATA> ( "+", MultiDimArrayString, MultiDimArrayString,
                                 &add2MultiDimArrays);
      registerOperator<T_DATA> ( "-", MultiDimArrayString, MultiDimArrayString,
                                 &subtract2MultiDimArrays);
      registerOperator<T_DATA> ( "*", MultiDimArrayString, MultiDimArrayString,
                                 &multiply2MultiDimArrays);
      registerOperator<T_DATA> ( "/", MultiDimArrayString, MultiDimArrayString,
                                 &divide2MultiDimArrays);
    
      // register base functions for multiDimDecisionDiagrams
      registerOperator<T_DATA> ( "+", MultiDimDecisionDiagramString, MultiDimDecisionDiagramString,
                                 &add2MultiDimDecisionDiagrams);
      registerOperator<T_DATA> ( "-", MultiDimDecisionDiagramString, MultiDimDecisionDiagramString,
                                 &subtract2MultiDimDecisionDiagrams);
      registerOperator<T_DATA> ( "*", MultiDimDecisionDiagramString, MultiDimDecisionDiagramString,
                                 &multiply2MultiDimDecisionDiagrams);
      registerOperator<T_DATA> ( "/", MultiDimDecisionDiagramString, MultiDimDecisionDiagramString,
                                 &divide2MultiDimDecisionDiagrams);

      // register default basename functions 
      registerOperator<T_DATA> ( "+", BaseNameString, BaseNameString,
                                 &add2MultiDimImplementations);
      registerOperator<T_DATA> ( "-", BaseNameString, BaseNameString,
                                 &subtract2MultiDimImplementations);
      registerOperator<T_DATA> ( "*", BaseNameString, BaseNameString,
                                 &multiply2MultiDimImplementations);
      registerOperator<T_DATA> ( "/", BaseNameString, BaseNameString,
                                 &divide2MultiDimImplementations);
    }
  }
  
  
  /// the function used to register all the above functions
  template<typename T_DATA>
  void pointerOperators4MultiDimInit () {
    static bool first_init = true;
    
    if ( first_init ) {
      first_init = false;
      
      std::string MultiDimArrayString ("MultiDimArray");
      std::string BaseNameString ("MultiDimImplementation");

      // register base functions for multiDimArrays
      registerOperator<T_DATA*> ( "+", MultiDimArrayString, MultiDimArrayString,
                                  &add2MultiDimArrays4Pointers);
      registerOperator<T_DATA*> ( "-", MultiDimArrayString, MultiDimArrayString,
                                  &subtract2MultiDimArrays4Pointers);
      registerOperator<T_DATA*> ( "*", MultiDimArrayString, MultiDimArrayString,
                                  &multiply2MultiDimArrays4Pointers);
      registerOperator<T_DATA*> ( "/", MultiDimArrayString, MultiDimArrayString,
                                  &divide2MultiDimArrays4Pointers);
    }
  }
  

} /* namespace gum */


// remove permission to use operatorsPatterns
#undef GUM_OPERATOR_PATTERN_ALLOWED


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
