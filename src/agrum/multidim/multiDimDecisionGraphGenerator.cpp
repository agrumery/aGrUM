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
 * @brief Sources of gum::MultiDimDecisionGraphGenerator.
 *
 * @author Jean-Christophe Magnan
 *
 */

#include <agrum/multidim/multiDimDecisionGraphGenerator.h>

namespace gum{
// ==========================================================================
// Constructor
// ==========================================================================
MultiDimDecisionGraphGenerator::MultiDimDecisionGraphGenerator(){

}

// ==========================================================================
// Destructor
// ==========================================================================
MultiDimDecisionGraphGenerator::~MultiDimDecisionGraphGenerator(){

}

MultiDimDecisionGraphGenerator::generate(){


    srand( time(NULL) );
    MultiDimDecisionGraph<double>* generatedDecisionGraph = new MultiDimDecisionGraph<double>();

    for ( SequenceIterator< const DiscreteVariable* > varIter = varList->begin(); varIter != varList->end(); ++varIter )
          generatedDecisionGraph->add(**varIter);

    while ( ( generatedDecisionGraph->variablesSequence().size() < __minNbVarInDiagram ) || ( generatedDecisionGraph->variablesSequence().size() > __maxNbVarInDiagram ) ) {

      gum::PriorityQueue< gum::NodeId > fifo;

      // L'idée est de d'abord générer un arbre avant de fusionner les sous-graphe isomorphe
      generatedDecisionGraph->manager()->setRootNode( generatedDecisionGraph->manager()->addNonTerminalNode( varList->atPos(0) ) );
      fifo.insert(1,generatedDecisionGraph->root());

      while( ! fifo.empty() ){
          gum::NodeId currentId = fifo.pop();
          for( gum::Idx modality = 0; modality < generatedDecisionGraph->node(currentId)->nodeVar->domainSize(); ++modality ){
              const gum::DiscreteVariable* sonVar = varList->atPos( rand() % ( varList->size() ) );
              gum::NodeId sonId = generatedDecisionGraph->manager()->addNonTerminalNode( sonVar );
              fifo.insert(sonId);
              generatedDecisionGraph->manager()->insertArc( currentId, sonId, modality );
          }
      }
    }

    generatedDecisionGraph->reduce();

    return generatedDecisionGraph;
}
} /* end of namespace */
