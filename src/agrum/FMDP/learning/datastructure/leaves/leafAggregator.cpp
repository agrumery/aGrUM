/*********************************************************************************
 * Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES *
 * {prenom.nom}_at_lip6.fr *
 *                                                                               *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or *
 * (at your option) any later version. *
 *                                                                               *
 * This program is distributed in the hope that it will be useful, *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the *
 * GNU General Public License for more details. *
 *                                                                               *
 * You should have received a copy of the GNU General Public License *
 * along with this program; if not, write to the *
 * Free Software Foundation, Inc., *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. *
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
  LeafAggregator::LeafAggregator( NodeGraphPart* idSource,
                                  double         similarityThreshold )
      : __leavesCpt( idSource )
      , __similarityThreshold( similarityThreshold ) {
    GUM_CONSTRUCTOR( LeafAggregator )
    __initialContext = new FusionContext<true>( nullptr );
    __needsUpdate = false;
  }

  // ============================================================================
  // Default constructor.
  // ============================================================================
  LeafAggregator::~LeafAggregator() {

    __removeContext( 0 );

    delete __initialContext;

    for ( HashTableIteratorSafe<AbstractLeaf*, Set<LeafPair*>*> leafIter =
              __leaf2Pair.beginSafe();
          leafIter != __leaf2Pair.endSafe();
          ++leafIter ) {
      for ( SetIteratorSafe<LeafPair*> pairIter = leafIter.val()->beginSafe();
            pairIter != leafIter.val()->endSafe();
            ++pairIter ) {
        LeafPair* curPair = *pairIter;
        __leaf2Pair[curPair->otherLeaf( leafIter.key() )]->erase( *pairIter );
        leafIter.val()->erase( curPair );
        delete curPair;
      }
      delete leafIter.val();
    }


    GUM_DESTRUCTOR( LeafAggregator )
  }

  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::addLeaf( AbstractLeaf* l ) {

    Set<LeafPair*>* leafPairSet = new Set<LeafPair*>();
    Set<LeafPair*>  bag;

    // ****************************************************************************************
    // Création et ajout des pairs de base (Feuille de base + nouvelle Feuille)
    for ( HashTableConstIteratorSafe<AbstractLeaf*, Set<LeafPair*>*> leafIter =
              __leaf2Pair.cbeginSafe();
          leafIter != __leaf2Pair.cendSafe();
          ++leafIter ) {

      // Création de la pair et ajout dans les listes de pair des feuilles de
      // base
      LeafPair* p = new LeafPair( l, leafIter.key() );
      p->updateLikelyhood();
      leafPairSet->insert( p );
      ( leafIter.val() )->insert( p );

      // Ajout de la nouvelle pair au tas initial
      __addInitialPair( p );

      bag.insert( p );
    }

    // ****************************************************************************************
    // Enregistrement de la nouvelle Feuille en tant que feuille de base
    __leaf2Pair.insert( l, leafPairSet );

    // ****************************************************************************************
    // Ajout de la feuille aux FusionContext

    for ( SequenceIteratorSafe<FusionContext<false>*> fusIter =
              __fusionSeq.beginSafe();
          fusIter != __fusionSeq.endSafe();
          ++fusIter ) {

      // Ajout de la nouvelle pair composée de la feuille de FusIter et de la
      // nouvelle feuille aux FusionContext suivant
      for ( SetIteratorSafe<LeafPair*> pairIter = bag.beginSafe();
            pairIter != bag.endSafe();
            ++pairIter ) {
        if ( ( *fusIter )
                 ->leaf()
                 ->contains( ( *pairIter )->secondLeaf()->id() ) ) {
          bag >> *pairIter;
          continue;
        }

        if ( ( *fusIter )->addPair( *pairIter ) )
          __removeContext( fusIter.pos() + 1 );
      }

      if ( ( *fusIter )->associateLeaf( l ) ) __removeContext( fusIter.pos() + 1 );

      bag << ( *fusIter )->leafAssociatedPair( l );
    }

    __needsUpdate = true;
  }


  // ============================================================================
  //
  // ============================================================================
  bool LeafAggregator::updateLeaf( AbstractLeaf* l ) {

    // ***********************************************************************************
    // First we update every base pair linked to that leaf
    Set<LeafPair*> bag( *( __leaf2Pair[l] ) );
    for ( SetIteratorSafe<LeafPair*> pairIter = bag.beginSafe();
          pairIter != bag.endSafe();
          ++pairIter ) {
      ( *pairIter )->updateLikelyhood();
      __updateInitialPair( *pairIter );
    }

    // **********************************************************************************
    // The we have top update FusionContext pairs associated to that leaf
    AbstractLeaf* curLeaf = l;
    for ( SequenceIteratorSafe<FusionContext<false>*> fusIter =
              __fusionSeq.beginSafe();
          fusIter != __fusionSeq.endSafe();
          ++fusIter ) {

      if ( ( *fusIter )->leaf()->contains( curLeaf->id() ) ) {
        bag.clear();
        if ( ( *fusIter )->updateAllAssociatedLeaves() )
          __removeContext( fusIter.pos() + 1 );
        bag = ( *fusIter )->associatedPairs();
        curLeaf = ( *fusIter )->leaf();
        continue;
      }

      for ( SetIteratorSafe<LeafPair*> pairIter = bag.beginSafe();
            pairIter != bag.endSafe();
            ++pairIter ) {

        if ( ( *fusIter )->leaf()->contains( ( *pairIter )->secondLeaf()->id() ) ||
             ( *fusIter )->leaf()->contains( ( *pairIter )->firstLeaf()->id() ) ) {
          bag >> *pairIter;
          continue;
        }

        if ( ( *fusIter )->updatePair( *pairIter ) )
          __removeContext( fusIter.pos() + 1 );
      }
      if ( ( *fusIter )->updateAssociatedLeaf( curLeaf ) )
        __removeContext( fusIter.pos() + 1 );
      bag << ( *fusIter )->leafAssociatedPair( curLeaf );
    }

    return __needsUpdate;
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::removeLeaf( AbstractLeaf* l ) {

    // ***********************************************************************************
    // First we update every base pair linked to that leaf
    Set<LeafPair*> bag( *( __leaf2Pair[l] ) );
    for ( SetIteratorSafe<LeafPair*> pairIter = bag.beginSafe();
          pairIter != bag.endSafe();
          ++pairIter ) {
      __removeInitialPair( *pairIter );
      ( *__leaf2Pair[( *pairIter )->otherLeaf( l )] ) >> *pairIter;
    }

    // **********************************************************************************
    // The we have top update FusionContext pairs associated to that leaf
    Set<LeafPair*> toBeDeleted;
    for ( SequenceIteratorSafe<FusionContext<false>*> fusIter =
              __fusionSeq.beginSafe();
          fusIter != __fusionSeq.endSafe();
          ++fusIter ) {

      for ( SetIteratorSafe<LeafPair*> pairIter = bag.beginSafe();
            pairIter != bag.endSafe();
            ++pairIter ) {

        if ( ( *fusIter )->leaf()->contains( ( *pairIter )->secondLeaf()->id() ) ||
             ( *fusIter )->leaf()->contains( ( *pairIter )->firstLeaf()->id() ) ) {
          bag >> *pairIter;
          continue;
        }

        if ( ( *fusIter )->removePair( *pairIter ) ) {
          __removeContext( fusIter.pos() + 1 );
        }
      }

      bag << ( *fusIter )->leafAssociatedPair( l );
      toBeDeleted << ( *fusIter )->leafAssociatedPair( l );

      if ( ( *fusIter )->deassociateLeaf( l ) ) {
        __removeContext( fusIter.pos() + 1 );
      }
    }

    for ( SetIteratorSafe<LeafPair*> pairIter = toBeDeleted.beginSafe();
          pairIter != toBeDeleted.endSafe();
          ++pairIter )
      delete *pairIter;

    for ( SetIteratorSafe<LeafPair*> pairIter = __leaf2Pair[l]->beginSafe();
          pairIter != __leaf2Pair[l]->endSafe();
          ++pairIter )
      delete *pairIter;
    delete __leaf2Pair[l];
    __leaf2Pair.erase( l );

    __needsUpdate = true;
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::update() {

    LeafPair*     nextPair = __initialContext->top();
    pair_iterator pb = __initialContext->beginPairs();
    pair_iterator pe = __initialContext->endPairs();
    if ( !__fusionSeq.empty() ) {
      nextPair = __fusionSeq.back()->top();
      pb = __fusionSeq.back()->beginPairs();
      pe = __fusionSeq.back()->endPairs();
    }


    while ( nextPair && nextPair->likelyhood() < __similarityThreshold ) {
      AbstractLeaf* newLeaf = nextPair->convert2Leaf( __leavesCpt->addNode() );
      FusionContext<false>* newContext = new FusionContext<false>( newLeaf );

      for ( pair_iterator pairIter = pb; pairIter != pe; ++pairIter ) {
        if ( !newLeaf->contains( pairIter.key()->firstLeaf()->id() ) &&
             !newLeaf->contains( pairIter.key()->secondLeaf()->id() ) )
          newContext->addPair( pairIter.key() );
        if ( !newLeaf->contains( pairIter.key()->firstLeaf()->id() ) &&
             !newContext->containsAssociatedLeaf( pairIter.key()->firstLeaf() ) )
          newContext->associateLeaf( pairIter.key()->firstLeaf() );
        if ( !newLeaf->contains( pairIter.key()->secondLeaf()->id() ) &&
             !newContext->containsAssociatedLeaf( pairIter.key()->secondLeaf() ) )
          newContext->associateLeaf( pairIter.key()->secondLeaf() );
      }

      __fusionSeq.insert( newContext );
      nextPair = __fusionSeq.back()->top();
      pb = __fusionSeq.back()->beginPairs();
      pe = __fusionSeq.back()->endPairs();
    }
    __needsUpdate = false;
  }


  HashTable<NodeId, AbstractLeaf*> LeafAggregator::leavesMap() {

    HashTable<NodeId, AbstractLeaf*> retMap;
    for ( SequenceIteratorSafe<FusionContext<false>*> fusIter =
              __fusionSeq.rbeginSafe();
          fusIter != __fusionSeq.rendSafe();
          --fusIter ) {
      bool alreadyIn = false;
      for ( HashTableIteratorSafe<NodeId, AbstractLeaf*> mapIter =
                retMap.beginSafe();
            mapIter != retMap.endSafe();
            ++mapIter )
        if ( mapIter.val()->contains( ( *fusIter )->leaf()->id() ) ) {
          alreadyIn = true;
          break;
        }
      if ( !alreadyIn )
        retMap.insert( ( *fusIter )->leaf()->id(), ( *fusIter )->leaf() );
    }

    for ( HashTableIteratorSafe<AbstractLeaf*, Set<LeafPair*>*> leafIter =
              __leaf2Pair.beginSafe();
          leafIter != __leaf2Pair.endSafe();
          ++leafIter ) {
      for ( HashTableIteratorSafe<NodeId, AbstractLeaf*> mapIter =
                retMap.beginSafe();
            mapIter != retMap.endSafe();
            ++mapIter )
        if ( mapIter.val()->contains( leafIter.key()->id() ) ) {
          retMap.insert( leafIter.key()->id(), mapIter.val() );
          break;
        }
      if ( !retMap.exists( leafIter.key()->id() ) )
        retMap.insert( leafIter.key()->id(), leafIter.key() );
    }

    return retMap;
  }


  std::string LeafAggregator::toString() {

    std::stringstream ss;
    ss << "################\nTas Initial : " << std::endl
       << __initialContext->toString() << std::endl;

    for ( auto fusIter = __fusionSeq.beginSafe(); fusIter != __fusionSeq.endSafe();
          ++fusIter ) {
      ss << "################\nTas " << fusIter.pos() << " : " << std::endl
         << ( *fusIter )->toString();
    }

    return ss.str();
  }

  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::__removeContext( Idx startingPos ) {

    for ( Idx i = __fusionSeq.size() - 1; !__fusionSeq.empty() && i >= startingPos;
          --i ) {
      __leavesCpt->eraseNode( __fusionSeq.atPos( i )->leaf()->id() );
      delete __fusionSeq.atPos( i );
      __fusionSeq.erase( __fusionSeq.atPos( i ) );
    }

    __needsUpdate = true;
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::__addInitialPair( LeafPair* p ) {
    bool res = __initialContext->addPair( p );
    if ( res ) __removeContext( 0 );
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::__updateInitialPair( LeafPair* p ) {
    bool res = __initialContext->updatePair( p );
    if ( res ) __removeContext( 0 );
  }

  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::__removeInitialPair( LeafPair* p ) {
    bool res = __initialContext->removePair( p );
    if ( res ) __removeContext( 0 );
  }

}  // end gum namespace
