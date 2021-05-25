/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Sources of MultiDimFunctionGraphGenerator.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 *
 */

#include <cstdlib>
#include <random>

#include <agrum/tools/core/priorityQueue.h>
#include <agrum/tools/multidim/implementations/multiDimFunctionGraphGenerator.h>

namespace gum {
  // Constructor
  MultiDimFunctionGraphGenerator::MultiDimFunctionGraphGenerator(
     Idx                                        maxVar,
     Idx                                        minVar,
     const Sequence< const DiscreteVariable* >& varSeq) :
      _varSeq_(varSeq) {
    GUM_CONSTRUCTOR(MultiDimFunctionGraphGenerator);

    _nbTotalVar_ = _varSeq_.size();
  }

  // Destructor
  MultiDimFunctionGraphGenerator::~MultiDimFunctionGraphGenerator() {
    GUM_DESTRUCTOR(MultiDimFunctionGraphGenerator);
  }

  MultiDimFunctionGraph< double >* MultiDimFunctionGraphGenerator::generate() {
    MultiDimFunctionGraph< double >* generatedFunctionGraph
       = MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();

    for (SequenceIteratorSafe< const DiscreteVariable* > varIter = _varSeq_.beginSafe();
         varIter != _varSeq_.endSafe();
         ++varIter) {
      generatedFunctionGraph->add(**varIter);
    }

    HashTable< NodeId, Idx > node2MinVar;
    Idx                      nbIter = 0;

    std::vector< NodeId > filo;

    generatedFunctionGraph->manager()->setRootNode(
       generatedFunctionGraph->manager()->addInternalNode(_varSeq_.atPos(0)));
    node2MinVar.insert(generatedFunctionGraph->root(), 0);
    filo.push_back(generatedFunctionGraph->root());

    while (!filo.empty()) {   //&& nbIter < 20 ){

      NodeId currentNodeId = filo.back();
      filo.pop_back();
      Idx                 cvp         = node2MinVar[currentNodeId];
      const InternalNode* currentNode = generatedFunctionGraph->node(currentNodeId);

      LinkedList< NodeId > potentialSons;
      Idx                  nbPotentialSons = 0;
      for (Idx varPos = 0; varPos < generatedFunctionGraph->variablesSequence().size(); varPos++) {
        const DiscreteVariable* var = generatedFunctionGraph->variablesSequence().atPos(varPos);

        Idx vsp = _varSeq_.pos(var);
        if (vsp > cvp) {
          const Link< NodeId >* nicleIter = generatedFunctionGraph->varNodeListe(var)->list();
          while (nicleIter) {
            nbPotentialSons++;
            potentialSons.addLink(nicleIter->element());
            nicleIter = nicleIter->nextLink();
          }
        }
      }

      for (Idx modality = 0; modality < currentNode->nodeVar()->domainSize(); modality++) {
        if (!potentialSons.list()
            || (double)std::rand() / (double)RAND_MAX > (1.0 / (1.0 + 3.0 / nbPotentialSons))) {
          if (_createLeaf_(currentNodeId, node2MinVar)) {
            generatedFunctionGraph->manager()->setSon(
               currentNodeId,
               modality,
               generatedFunctionGraph->manager()->addTerminalNode((double)std::rand()
                                                                  / (double)RAND_MAX * 100));
          } else {
            Idx sonVarPos = _generateVarPos_(node2MinVar[currentNodeId] + 1,
                                             _nbTotalVar_ - node2MinVar[currentNodeId] - 2);
            generatedFunctionGraph->manager()->setSon(
               currentNodeId,
               modality,
               generatedFunctionGraph->manager()->addInternalNode(_varSeq_.atPos(sonVarPos)));
            filo.push_back(currentNode->son(modality));
            node2MinVar.insert(currentNode->son(modality), sonVarPos);
          }
        } else {
          Idx sonPos = (Idx)(((double)std::rand() / (double)RAND_MAX) * nbPotentialSons);
          sonPos     = sonPos == nbPotentialSons ? nbPotentialSons - 1 : sonPos;
          Link< NodeId >* nicleIter = potentialSons.list();
          while (sonPos) {
            nicleIter = nicleIter->nextLink();
            sonPos--;
          }
          generatedFunctionGraph->manager()->setSon(currentNodeId, modality, nicleIter->element());
        }
      }
      ++nbIter;
    }

    generatedFunctionGraph->manager()->reduce();
    generatedFunctionGraph->manager()->clean();

    return generatedFunctionGraph;
  }

  bool MultiDimFunctionGraphGenerator::_createLeaf_(NodeId                    currentNodeId,
                                                    HashTable< NodeId, Idx >& node2MinVar) {
    return !(
       currentNodeId == 1
       || ((double)std::rand() / (double)RAND_MAX
              < 0.9
                   + 0.01 * (float(_nbTotalVar_ - node2MinVar[currentNodeId]) / float(_nbTotalVar_))
           && node2MinVar[currentNodeId] < _nbTotalVar_ - 1));
  }

  Idx MultiDimFunctionGraphGenerator::_generateVarPos_(Idx offset, Idx span) {
    Idx randOut = 0;

    if (span != 0) {
      auto                                generator = gum::getRandomGenerator();
      std::weibull_distribution< double > distribution(double(span), 1.0);
      randOut = (Idx)(distribution(generator) * span / 2);
      if (randOut > span) randOut = span;
    }

    return offset + randOut;
  }

}   // namespace gum
