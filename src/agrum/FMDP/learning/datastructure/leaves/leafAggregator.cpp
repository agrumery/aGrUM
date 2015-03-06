/*********************************************************************************
 * Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES           *
 * {prenom.nom}_at_lip6.fr                                                       *
 *                                                                               *
 * This program is free software; you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by          *
 * the Free Software Foundation; either version 2 of the License, or             *
 * (at your option) any later version.                                           *
 *                                                                               *
 * This program is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  *
 * GNU General Public License for more details.                                  *
 *                                                                               *
 * You should have received a copy of the GNU General Public License             *
 * along with this program; if not, write to the                                 *
 * Free Software Foundation, Inc.,                                               *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                      *
 *********************************************************************************/
/**
* @file
* @brief Sources for Leaf Aggregator class
*
* @author Jean-Christophe Magnan
*/
// =======================================================
#include <cmath>
// =======================================================
#include <agrum/FMDP/learning/datastructure/leaves/leafAggregator.h>
// =======================================================

namespace gum {

  // ############################################################################
  // Constructors / Destructors
  // ############################################################################

    // ============================================================================
    // Default constructor.
    // ============================================================================
    LeafAggregator::LeafAggregator(NodeGraphPart *idSource , double similarityThreshold )
      : __leavesCpt(idSource),
        __similarityThreshold(similarityThreshold){
      GUM_CONSTRUCTOR(LeafAggregator)
      __initialContext = new FusionContext<true>( nullptr );
      __needsUpdate = false;
    }

    // ============================================================================
    // Default constructor.
    // ============================================================================
    LeafAggregator::~LeafAggregator(){
      GUM_DESTRUCTOR(LeafAggregator)
    }

  // ############################################################################
  //
  // ############################################################################

    // ============================================================================
    //
    // ============================================================================
    void LeafAggregator::addLeaf( AbstractLeaf* l ){

//      std::cout << "\tLeafAggregator::addLeaf " << l->id() << " begin" << std::endl;
//      std::cout << this->toString() << std::endl;

//      bool want2leave = false;
//      if( __fusionSeq.size() >= 2 ){
//        want2leave = true;
//      }

      Set<LeafPair*>* leafPairSet = new Set<LeafPair*>();
      Set<LeafPair*> bag;

      // ****************************************************************************************
      // Création et ajout des pairs de base (Feuille de base + nouvelle Feuille)
      for( HashTableConstIteratorSafe<AbstractLeaf*, Set<LeafPair*>* > leafIter = __leaf2Pair.cbeginSafe();
           leafIter != __leaf2Pair.cendSafe(); ++leafIter ){

        // Création de la pair et ajout dans les listes de pair des feuilles de base
        LeafPair* p = new LeafPair( l, leafIter.key() );
        p->updateLikelyhood();
        leafPairSet->insert( p );
        (leafIter.val())->insert( p );

        // Ajout de la nouvelle pair au tas initial
        __addInitialPair(p);

        bag.insert(p);
      }
//      std::cout << __initialContext->toString() << std::endl;

      // ****************************************************************************************
      // Enregistrement de la nouvelle Feuille en tant que feuille de base
      __leaf2Pair.insert( l, leafPairSet );

      // ****************************************************************************************
      // Ajout de la feuille aux FusionContext

      for( SequenceIteratorSafe<FusionContext<false>*> fusIter = __fusionSeq.beginSafe(); fusIter != __fusionSeq.endSafe(); ++fusIter ){

        if( (*fusIter)->associateLeaf( l ) )
          __removeContext( fusIter.pos() + 1 );

        // Ajout de la nouvelle pair composée de la feuille de FusIter et de la nouvelle feuille aux FusionContext suivant
        for( SetIteratorSafe<LeafPair*> pairIter = bag.beginSafe(); pairIter != bag.endSafe(); ++pairIter )
          if( !(*fusIter)->leaf()->contains( (*pairIter)->secondLeaf()->id() ) ){
            if( (*fusIter)->addPair( *pairIter ) )
              __removeContext( fusIter.pos() + 1 );
          } else {
            bag >> *pairIter;
          }
        (*fusIter)->leafAssociatedPair(l)->updateLikelyhood();
        bag << (*fusIter)->leafAssociatedPair(l);
      }

      __needsUpdate = true;

//      std::cout << this->toString() << std::endl;
//      std::cout << "\tLeafAggregator::addLeaf end - Update? : " << __needsUpdate << std::endl;

//      if( want2leave ){
//        exit(1);
//      }
    }


    // ============================================================================
    //
    // ============================================================================
    bool LeafAggregator::updateLeaf( AbstractLeaf* l ){

//      std::cout << "\tLeafAggregator::updateLeaf " << l->id() << " begin" << std::endl;
//      std::cout << this->toString() << std::endl;

//      bool want2leave = false;
//      if( __fusionSeq.size() >= 2 ){
//        want2leave = true;
//      }

      // ***********************************************************************************
      // First we update every base pair linked to that leaf
      Set<LeafPair*> bag( *(__leaf2Pair[l]) );
      for( SetIteratorSafe<LeafPair*> pairIter = bag.beginSafe(); pairIter != bag.endSafe(); ++pairIter ){
        (*pairIter)->updateLikelyhood();
        __updateInitialPair( *pairIter );
      }

//      std::cout << "\tFin Update Initial" << std::endl;

      // **********************************************************************************
      // The we have top update FusionContext pairs associated to that leaf
      AbstractLeaf* curLeaf = l;
      for( SequenceIteratorSafe<FusionContext<false>*> fusIter = __fusionSeq.beginSafe(); fusIter != __fusionSeq.endSafe(); ++fusIter ){

//        std::cout << "\tUpdate Context " << fusIter.pos() << std::endl;
//        std::cout << "----------------------- Rappel Context -----------------------" << std::endl;
//        std::cout << (*fusIter)->toString() << std::endl;
//        std::cout << "--------------------------------------------------------------" << std::endl;

        if( (*fusIter)->leaf()->contains( curLeaf->id() ) ){
          bag.clear();
          __needsUpdate = __needsUpdate | (*fusIter)->updateAllAssociatedLeaves();
          bag = (*fusIter)->associatedPairs();
          curLeaf = (*fusIter)->leaf();
//          std::cout << "Boucle" << std::endl;
//          std::cout << "--------------------------------------------------------------" << std::endl;
          continue;
        }

        for( SetIteratorSafe<LeafPair*> pairIter = bag.beginSafe(); pairIter != bag.endSafe(); ++ pairIter ){

          if( !(*fusIter)->leaf()->contains( (*pairIter)->secondLeaf()->id() ) && !(*fusIter)->leaf()->contains( (*pairIter)->firstLeaf()->id() ) ){
            if( (*fusIter)->updatePair( *pairIter ) )
              __removeContext( fusIter.pos() + 1 );
          } else {
            bag >> *pairIter;
          }
        }
//        std::cout << "Fin update bag" << std::endl;
//        std::cout << "Update pair associée " << (*fusIter)->leafAssociatedPair(curLeaf)->toString() << std::endl;
        (*fusIter)->leafAssociatedPair(curLeaf)->updateLikelyhood();
//        std::cout << "Récup pair associée" << std::endl;
        bag << (*fusIter)->leafAssociatedPair(curLeaf);
//        std::cout << "Boucle" << std::endl;
//        std::cout << "--------------------------------------------------------------" << std::endl;

      }

//      std::cout << this->toString() << std::endl;
//      std::cout << "\tLeafAggregator::updateLeaf end - Update? : " << __needsUpdate << std::endl;

//      if( want2leave ){
//        exit(1);
//      }

      return __needsUpdate;
    }


    // ============================================================================
    //
    // ============================================================================
    void LeafAggregator::removeLeaf( AbstractLeaf* l ){

//      std::cout << "\tLeafAggregator::removeLeaf begin " << l->id() << std::endl;
//      std::cout << this->toString() << std::endl;

//      bool want2leave = false;
//      if( __fusionSeq.size() >= 2 ){
//        want2leave = true;
//      }

      // ***********************************************************************************
      // First we update every base pair linked to that leaf
      Set<LeafPair*> bag( *(__leaf2Pair[l]) );
      for( SetIteratorSafe<LeafPair*> pairIter = bag.beginSafe(); pairIter != bag.endSafe(); ++pairIter ){
        __removeInitialPair( *pairIter );
        (*__leaf2Pair[(*pairIter)->otherLeaf(l)]) >> *pairIter;
      }
//      std::cout << "\t Fin Nettoyage Init" << std::endl;

//      std::cout << "\t Début Nettoyage Seq" << std::endl;
      // **********************************************************************************
      // The we have top update FusionContext pairs associated to that leaf
      Set<LeafPair*> toBeDeleted;
      for( SequenceIteratorSafe<FusionContext<false>*> fusIter = __fusionSeq.beginSafe(); fusIter != __fusionSeq.endSafe(); ++ fusIter ){

//        std::cout << "\tErase Context " << fusIter.pos() << std::endl;
//        std::cout << "----------------------- Rappel Context -----------------------" << std::endl;
//        std::cout << (*fusIter)->toString() << std::endl;
//        std::cout << "--------------------------------------------------------------" << std::endl;

        for( SetIteratorSafe<LeafPair*> pairIter = bag.beginSafe(); pairIter != bag.endSafe(); ++ pairIter )
          if( !(*fusIter)->leaf()->contains( (*pairIter)->secondLeaf()->id() ) && !(*fusIter)->leaf()->contains( (*pairIter)->firstLeaf()->id() ) ){
            if( (*fusIter)->removePair( *pairIter ) ){
//              std::cout << "Nettoyage FusionContext sur ordre Associated Leaf" << std::endl;
              __removeContext( fusIter.pos() + 1 );
            }
          } else {
            bag >> *pairIter;
          }
        bag << (*fusIter)->leafAssociatedPair(l);
        toBeDeleted << (*fusIter)->leafAssociatedPair(l);

        if( (*fusIter)->deassociateLeaf( l ) ){
//            std::cout << "Nettoyage FusionContext sur ordre Associated Leaf" << std::endl;
          __removeContext( fusIter.pos() + 1 );
        }
//        std::cout << "Boucle" << std::endl;
//        std::cout << "--------------------------------------------------------------" << std::endl;
      }
//      std::cout << "\t Fin Nettoyage Seq" << std::endl;

      for( SetIteratorSafe<LeafPair*> pairIter = toBeDeleted.beginSafe(); pairIter != toBeDeleted.endSafe(); ++pairIter )
        delete *pairIter;
//      std::cout << "Fin 2BeDel" << std::endl;

      for( SetIteratorSafe<LeafPair*> pairIter = __leaf2Pair[l]->beginSafe(); pairIter != __leaf2Pair[l]->endSafe(); ++pairIter )
        delete *pairIter;
//      std::cout << "Fin del pair" << std::endl;
      delete __leaf2Pair[l];
//      std::cout << "Leaf set deleted" << std::endl;
      __leaf2Pair.erase(l);
//      std::cout << "Leaf removed" << std::endl;

      __needsUpdate = true;

//      std::cout << this->toString() << std::endl;
//      std::cout << "\tLeafAggregator::removeLeaf end - Update? : " << __needsUpdate << std::endl;

//      if( want2leave ){
//        exit(1);
//      }
    }


    // ============================================================================
    //
    // ============================================================================
    void LeafAggregator::update(  ){

//      std::cout << "\tLeafAggregator::update begin" << std::endl;
//      std::cout << this->toString() << std::endl;

      LeafPair* nextPair = __initialContext->top();
      pair_iterator pb = __initialContext->beginPairs();
      pair_iterator pe = __initialContext->endPairs();
      if( !__fusionSeq.empty() ){
        nextPair = __fusionSeq.back()->top();
        pb = __fusionSeq.back()->beginPairs();
        pe = __fusionSeq.back()->endPairs();
      }

      if( nextPair ){
//        std::cout << "NP Likely " << nextPair->likelyhood() << " - Threshold : " << __similarityThreshold << std::endl;
//        std::cout << "Starting pairs : " << std::endl;
//        for( pair_iterator pairIter = pb; pairIter != pe; ++pairIter )
//          std::cout << "\t\t\t" << pairIter.key()->toString() << std::endl;

      }


      while ( nextPair && nextPair->likelyhood() < __similarityThreshold ){
//        std::cout << "----------------------------------------------------------" << std::endl;
//        std::cout << "Selected pair : " << nextPair->toString() << std::endl;
        AbstractLeaf* newLeaf = nextPair->convert2Leaf( __leavesCpt->addNode() );
//        std::cout << "Creating Context" << std::endl;
        FusionContext<false>* newContext = new FusionContext<false>( newLeaf );

//        std::cout << "Adding Pairs ... " << std::endl;
        for( pair_iterator pairIter = pb; pairIter != pe; ++pairIter ){
//          std::cout << "\t\t\t" << pairIter.key()->toString() << std::endl;
          if( !newLeaf->contains(pairIter.key()->firstLeaf()->id()) && !newLeaf->contains(pairIter.key()->secondLeaf()->id()) )
            newContext->addPair(pairIter.key());
          if( !newLeaf->contains(pairIter.key()->firstLeaf()->id()) && !newContext->containsAssociatedLeaf(pairIter.key()->firstLeaf()))
            newContext->associateLeaf(pairIter.key()->firstLeaf());
          if( !newLeaf->contains(pairIter.key()->secondLeaf()->id()) && !newContext->containsAssociatedLeaf(pairIter.key()->secondLeaf()))
            newContext->associateLeaf(pairIter.key()->secondLeaf());
        }
//        std::cout << "Pushing context" << std::endl;

        __fusionSeq.insert( newContext );
        nextPair = __fusionSeq.back()->top();
        pb = __fusionSeq.back()->beginPairs();
        pe = __fusionSeq.back()->endPairs();
//        std::cout << "Done" << std::endl;
      }
      __needsUpdate = false;

//      std::cout << this->toString() << std::endl;
//      std::cout << "\tLeafAggregator::update end" << std::endl;
      //exit(1);
    }


    HashTable<NodeId,AbstractLeaf*> LeafAggregator::leavesMap(){

//      std::cout << "\tLeafAggregator::leavesMap begin" << std::endl;
//      std::cout << this->toString() << std::endl;

      HashTable<NodeId, AbstractLeaf*> retMap;
//      std::cout << "Recherche Feuille complexe" << std::endl;
      for( SequenceIteratorSafe<FusionContext<false>*> fusIter = __fusionSeq.rbeginSafe(); fusIter != __fusionSeq.rendSafe(); --fusIter ){
        bool alreadyIn = false;
        for( HashTableIteratorSafe<NodeId, AbstractLeaf*> mapIter = retMap.beginSafe(); mapIter != retMap.endSafe(); ++mapIter )
          if( mapIter.val()->contains( (*fusIter)->leaf()->id() ) ){
            alreadyIn = true;
            break;
          }
        if(!alreadyIn)
          retMap.insert( (*fusIter)->leaf()->id(), (*fusIter)->leaf() );
      }
//      std::cout << "Recherche Feuille complexe done" << std::endl;
//      std::cout << "Mapping Fuille simple -> Feuille complexe" << std::endl;

      for( HashTableIteratorSafe<AbstractLeaf*, Set<LeafPair*>*> leafIter = __leaf2Pair.beginSafe(); leafIter != __leaf2Pair.endSafe(); ++leafIter ){
        for( HashTableIteratorSafe<NodeId, AbstractLeaf*> mapIter = retMap.beginSafe(); mapIter != retMap.endSafe(); ++mapIter )
          if( mapIter.val()->contains( leafIter.key()->id() ) ){
            retMap.insert( leafIter.key()->id(), mapIter.val() );
            break;
          }
        if( !retMap.exists(leafIter.key()->id()) )
          retMap.insert( leafIter.key()->id(), leafIter.key());
      }

//      std::cout << retMap << std::endl;
//      std::cout << "\tLeafAggregator::leavesMap end" << std::endl;

      return retMap;
    }


    std::string LeafAggregator::toString(){

      std::stringstream ss;
      ss << "################\nTas Initial : " << std::endl << __initialContext->toString() << std::endl;

      for( auto fusIter = __fusionSeq.beginSafe(); fusIter != __fusionSeq.endSafe(); ++fusIter ){
        ss << "################\nTas " << fusIter.pos() << " : " << std::endl << (*fusIter)->toString();
      }

      return ss.str();
    }

  // ############################################################################
  //
  // ############################################################################

    // ============================================================================
    //
    // ============================================================================
    void LeafAggregator::__removeContext( Idx startingPos ){

//      std::cout << "\tLeafAggregator::__removeContext begin" << std::endl;

      for(Idx i = __fusionSeq.size() - 1; !__fusionSeq.empty() && i >= startingPos; --i ){
//        std::cout << "LOOP" << __fusionSeq.toString() << " - i " << i << std::endl;
        __leavesCpt->eraseNode( __fusionSeq.atPos(i)->leaf()->id() );
        delete __fusionSeq.atPos(i);
        __fusionSeq.erase( __fusionSeq.atPos(i) );
//        std::cout << "LEAP" << std::endl;
      }

      __needsUpdate = true;

//      std::cout << "\tLeafAggregator::__removeContext end" << std::endl;
    }


    // ============================================================================
    //
    // ============================================================================
    void LeafAggregator::__addInitialPair( LeafPair* p ){

//      std::cout << "\tLeafAggregator::__addInitialPair begin" << std::endl;
      bool res = __initialContext->addPair(p);
      if( res )
        __removeContext(0);

//      std::cout << "\tLeafAggregator::__addInitialPair end " << res << std::endl;
    }


    // ============================================================================
    //
    // ============================================================================
    void LeafAggregator::__updateInitialPair( LeafPair* p ){

//      std::cout << "\tLeafAggregator::__updateInitialPair begin" << std::endl;
      bool res = __initialContext->updatePair(p);
      if( res )
        __removeContext(0);

//      std::cout << "\tLeafAggregator::__updateInitialPair end " << res << std::endl;
    }

    // ============================================================================
    //
    // ============================================================================
    void LeafAggregator::__removeInitialPair( LeafPair* p ){

//      std::cout << "\tLeafAggregator::__removeInitialPair begin" << std::endl;
      bool res = __initialContext->removePair(p);
      if( res )
        __removeContext(0);

//      std::cout << "\tLeafAggregator::__removeInitialPair end " << res << std::endl;
    }

} // end gum namespace
