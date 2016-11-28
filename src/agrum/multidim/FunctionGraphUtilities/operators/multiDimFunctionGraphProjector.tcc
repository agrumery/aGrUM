/****************************************************************************
 *  Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES     *
 *  {prenom.nom}_at_lip6.fr                                                 *
 *                                                                          *
 *  This program is free software; you can redistribute it and/or modify    *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation; either version 2 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  This program is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program; if not, write to the                           *
 *  Free Software Foundation, Inc.,                                         *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                *
 ****************************************************************************/
/**
 * @file
 * @brief Class used to compute the projection of a function graph
 *
 * @author Jean-Christophe Magnan
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/multidim/FunctionGraphUtilities/internalNode.h>
#include <agrum/multidim/FunctionGraphUtilities/operators/multiDimFunctionGraphProjector.h>
#include <agrum/variables/discreteVariable.h>

namespace gum {

  // CONSTRUCTOR
  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  MultiDimFunctionGraphProjector<GUM_SCALAR, FUNCTOR, TerminalNodePolicy>::
      MultiDimFunctionGraphProjector(
          const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* src,
          const Set<const DiscreteVariable*>& delVars,
          const GUM_SCALAR                    neutral )
      : __src( src )
      , __delVars( delVars )
      , __function()
      , __neutral( neutral ) {

    GUM_CONSTRUCTOR( MultiDimFunctionGraphProjector );
    __rd = MultiDimFunctionGraph<GUM_SCALAR>::getReducedAndOrderedInstance();
  }


  // DESTRUCTOR
  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  MultiDimFunctionGraphProjector<GUM_SCALAR, FUNCTOR, TerminalNodePolicy>::
      ~MultiDimFunctionGraphProjector() {
    GUM_DESTRUCTOR( MultiDimFunctionGraphProjector );
  }


  // This function is the main function. To be call every time an Projection
  // between the two given Function Graphs is required
  template <typename GUM_SCALAR,
            template <typename> class FUNCTOR,
            template <typename> class TerminalNodePolicy>
  MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>*
  MultiDimFunctionGraphProjector<GUM_SCALAR,
                                 FUNCTOR,
                                 TerminalNodePolicy>::project() {

    __rd->copy( *__src );

    for ( SetIteratorSafe<const DiscreteVariable*> varIter = __delVars.beginSafe();
          varIter != __delVars.endSafe();
          ++varIter ) {

      const DiscreteVariable* curVar = *varIter;

      // Tout d'abord, on déplace la variable à projeter en fin de séquence afin
      // de simplifier la projection
      if ( __rd->variablesSequence().exists( curVar ) )
        __rd->manager()->moveTo( curVar, __rd->variablesSequence().size() - 1 );

      // 1er cas spécial : le diagramme est un un simple noeud terminal
      if ( __rd->isTerminalNode( __rd->root() ) ) {

        GUM_SCALAR newVal = __neutral, oldVal = __rd->nodeValue( __rd->root() );
        for ( Idx curVarModality = 0; curVarModality < curVar->domainSize();
              ++curVarModality )
          newVal = __function( newVal, oldVal );

        NodeId newSonId = __rd->manager()->addTerminalNode( newVal );
        __rd->manager()->setRootNode( newSonId );

        if ( __rd->variablesSequence().exists( curVar ) ) __rd->erase( *curVar );
        continue;
      }

      // 2ème cas spécial : la racine du diagramme est associée à la variable
      // projetée
      if ( __rd->node( __rd->root() )->nodeVar() == curVar ) {

        const InternalNode* curVarNode = __rd->node( __rd->root() );
        GUM_SCALAR          newVal = __neutral;
        for ( Idx curVarModality = 0; curVarModality < curVar->domainSize();
              ++curVarModality )
          newVal = __function(
              newVal, __rd->nodeValue( curVarNode->son( curVarModality ) ) );

        NodeId newSonId = __rd->manager()->addTerminalNode( newVal );

        __rd->manager()->eraseNode( __rd->root(), newSonId, false );

        if ( __rd->variablesSequence().exists( curVar ) ) __rd->erase( *curVar );
        continue;
      }

      // Cas général
      HashTable<NodeId, NodeId> visitedNode( 2 * __rd->realSize(), true, false );
      std::vector<NodeId> filo;
      filo.push_back( __rd->root() );

      while ( !filo.empty() ) {
        NodeId curNodeId = filo.back();
        filo.pop_back();

        const InternalNode* curNode = __rd->node( curNodeId );

        for ( Idx modality = 0; modality < curNode->nodeVar()->domainSize();
              ++modality ) {

          NodeId oldSonId = curNode->son( modality );

          if ( !visitedNode.exists( oldSonId ) ) {

            NodeId newSonId = oldSonId;

            if ( !__rd->isTerminalNode( oldSonId ) ) {


              if ( __rd->node( oldSonId )->nodeVar() != curVar ) {
                filo.push_back( oldSonId );
              } else {

                const InternalNode* curVarNode = __rd->node( oldSonId );
                GUM_SCALAR          newVal = __neutral;
                for ( Idx curVarModality = 0;
                      curVarModality < curVar->domainSize();
                      ++curVarModality )
                  newVal = __function(
                      newVal,
                      __rd->nodeValue( curVarNode->son( curVarModality ) ) );

                newSonId = __rd->manager()->addTerminalNode( newVal );

                __rd->manager()->eraseNode( oldSonId, newSonId, false );
                __rd->manager()->setSon( curNodeId, modality, newSonId );
              }

            } else {

              GUM_SCALAR newVal = __neutral, oldVal = __rd->nodeValue( oldSonId );
              for ( Idx curVarModality = 0; curVarModality < curVar->domainSize();
                    ++curVarModality )
                newVal = __function( newVal, oldVal );

              newSonId = __rd->manager()->addTerminalNode( newVal );
              __rd->manager()->setSon( curNodeId, modality, newSonId );
            }

            visitedNode.insert( oldSonId, newSonId );

          } else {

            if ( __rd->node( curNodeId )->son( modality ) !=
                 visitedNode[oldSonId] )
              __rd->manager()->setSon(
                  curNodeId, modality, visitedNode[oldSonId] );
          }
        }
      }

      if ( __rd->variablesSequence().exists( curVar ) ) __rd->erase( *curVar );
    }

    return __rd;
  }

}  // namespace gum
