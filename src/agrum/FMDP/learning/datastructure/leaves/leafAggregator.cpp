/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Sources for Leaf Aggregator class
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 */
// =======================================================
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/FMDP/learning/datastructure/leaves/leafAggregator.h>
// =======================================================

namespace gum {

  // ############################################################################
  // Constructors / Destructors
  // ############################################################################

  // ============================================================================
  // Default constructor.
  // ============================================================================
  LeafAggregator::LeafAggregator(NodeGraphPart* idSource,
                                 double         similarityThreshold) :
      leavesCpt__(idSource),
      similarityThreshold__(similarityThreshold) {
    GUM_CONSTRUCTOR(LeafAggregator);
    initialContext__ = new FusionContext< true >(nullptr);
    needsUpdate__    = false;
  }

  // ============================================================================
  // Default constructor.
  // ============================================================================
  LeafAggregator::~LeafAggregator() {
    removeContext__(0);

    delete initialContext__;

    for (HashTableIteratorSafe< AbstractLeaf*, Set< LeafPair* >* > leafIter
         = leaf2Pair__.beginSafe();
         leafIter != leaf2Pair__.endSafe();
         ++leafIter) {
      for (SetIteratorSafe< LeafPair* > pairIter = leafIter.val()->beginSafe();
           pairIter != leafIter.val()->endSafe();
           ++pairIter) {
        LeafPair* curPair = *pairIter;
        leaf2Pair__[curPair->otherLeaf(leafIter.key())]->erase(*pairIter);
        leafIter.val()->erase(curPair);
        delete curPair;
      }
      delete leafIter.val();
    }


    GUM_DESTRUCTOR(LeafAggregator);
  }

  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::addLeaf(AbstractLeaf* l) {
    Set< LeafPair* >* leafPairSet = new Set< LeafPair* >();
    Set< LeafPair* >  bag;

    // ****************************************************************************************
    // Création et ajout des pairs de base (Feuille de base + nouvelle Feuille)
    for (HashTableConstIteratorSafe< AbstractLeaf*, Set< LeafPair* >* > leafIter
         = leaf2Pair__.cbeginSafe();
         leafIter != leaf2Pair__.cendSafe();
         ++leafIter) {
      // Création de la pair et ajout dans les listes de pair des feuilles de
      // base
      LeafPair* p = new LeafPair(l, leafIter.key());
      p->updateLikelyhood();
      leafPairSet->insert(p);
      (leafIter.val())->insert(p);

      // Ajout de la nouvelle pair au tas initial
      addInitialPair__(p);

      bag.insert(p);
    }

    // ****************************************************************************************
    // Enregistrement de la nouvelle Feuille en tant que feuille de base
    leaf2Pair__.insert(l, leafPairSet);

    // ****************************************************************************************
    // Ajout de la feuille aux FusionContext

    for (SequenceIteratorSafe< FusionContext< false >* > fusIter
         = fusionSeq__.beginSafe();
         fusIter != fusionSeq__.endSafe();
         ++fusIter) {
      // Ajout de la nouvelle pair composée de la feuille de FusIter et de la
      // nouvelle feuille aux FusionContext suivant
      for (SetIteratorSafe< LeafPair* > pairIter = bag.beginSafe();
           pairIter != bag.endSafe();
           ++pairIter) {
        if ((*fusIter)->leaf()->contains((*pairIter)->secondLeaf()->id())) {
          bag >> *pairIter;
          continue;
        }

        if ((*fusIter)->addPair(*pairIter)) removeContext__(fusIter.pos() + 1);
      }

      if ((*fusIter)->associateLeaf(l)) removeContext__(fusIter.pos() + 1);

      bag << (*fusIter)->leafAssociatedPair(l);
    }

    needsUpdate__ = true;
  }


  // ============================================================================
  //
  // ============================================================================
  bool LeafAggregator::updateLeaf(AbstractLeaf* l) {
    // ***********************************************************************************
    // First we update every base pair linked to that leaf
    Set< LeafPair* > bag(*(leaf2Pair__[l]));
    for (SetIteratorSafe< LeafPair* > pairIter = bag.beginSafe();
         pairIter != bag.endSafe();
         ++pairIter) {
      (*pairIter)->updateLikelyhood();
      updateInitialPair__(*pairIter);
    }

    // **********************************************************************************
    // The we have top update FusionContext pairs associated to that leaf
    AbstractLeaf* curLeaf = l;
    for (SequenceIteratorSafe< FusionContext< false >* > fusIter
         = fusionSeq__.beginSafe();
         fusIter != fusionSeq__.endSafe();
         ++fusIter) {
      if ((*fusIter)->leaf()->contains(curLeaf->id())) {
        bag.clear();
        if ((*fusIter)->updateAllAssociatedLeaves())
          removeContext__(fusIter.pos() + 1);
        bag     = (*fusIter)->associatedPairs();
        curLeaf = (*fusIter)->leaf();
        continue;
      }

      for (SetIteratorSafe< LeafPair* > pairIter = bag.beginSafe();
           pairIter != bag.endSafe();
           ++pairIter) {
        if ((*fusIter)->leaf()->contains((*pairIter)->secondLeaf()->id())
            || (*fusIter)->leaf()->contains((*pairIter)->firstLeaf()->id())) {
          bag >> *pairIter;
          continue;
        }

        if ((*fusIter)->updatePair(*pairIter)) removeContext__(fusIter.pos() + 1);
      }
      if ((*fusIter)->updateAssociatedLeaf(curLeaf))
        removeContext__(fusIter.pos() + 1);
      bag << (*fusIter)->leafAssociatedPair(curLeaf);
    }

    return needsUpdate__;
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::removeLeaf(AbstractLeaf* l) {
    // ***********************************************************************************
    // First we update every base pair linked to that leaf
    Set< LeafPair* > bag(*(leaf2Pair__[l]));
    for (SetIteratorSafe< LeafPair* > pairIter = bag.beginSafe();
         pairIter != bag.endSafe();
         ++pairIter) {
      removeInitialPair__(*pairIter);
      (*leaf2Pair__[(*pairIter)->otherLeaf(l)]) >> *pairIter;
    }

    // **********************************************************************************
    // The we have top update FusionContext pairs associated to that leaf
    Set< LeafPair* > toBeDeleted;
    for (SequenceIteratorSafe< FusionContext< false >* > fusIter
         = fusionSeq__.beginSafe();
         fusIter != fusionSeq__.endSafe();
         ++fusIter) {
      for (SetIteratorSafe< LeafPair* > pairIter = bag.beginSafe();
           pairIter != bag.endSafe();
           ++pairIter) {
        if ((*fusIter)->leaf()->contains((*pairIter)->secondLeaf()->id())
            || (*fusIter)->leaf()->contains((*pairIter)->firstLeaf()->id())) {
          bag >> *pairIter;
          continue;
        }

        if ((*fusIter)->removePair(*pairIter)) {
          removeContext__(fusIter.pos() + 1);
        }
      }

      bag << (*fusIter)->leafAssociatedPair(l);
      toBeDeleted << (*fusIter)->leafAssociatedPair(l);

      if ((*fusIter)->deassociateLeaf(l)) { removeContext__(fusIter.pos() + 1); }
    }

    for (SetIteratorSafe< LeafPair* > pairIter = toBeDeleted.beginSafe();
         pairIter != toBeDeleted.endSafe();
         ++pairIter)
      delete *pairIter;

    for (SetIteratorSafe< LeafPair* > pairIter = leaf2Pair__[l]->beginSafe();
         pairIter != leaf2Pair__[l]->endSafe();
         ++pairIter)
      delete *pairIter;
    delete leaf2Pair__[l];
    leaf2Pair__.erase(l);

    needsUpdate__ = true;
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::update() {
    LeafPair*     nextPair = initialContext__->top();
    pair_iterator pb       = initialContext__->beginPairs();
    pair_iterator pe       = initialContext__->endPairs();
    if (!fusionSeq__.empty()) {
      nextPair = fusionSeq__.back()->top();
      pb       = fusionSeq__.back()->beginPairs();
      pe       = fusionSeq__.back()->endPairs();
    }


    while (nextPair && nextPair->likelyhood() < similarityThreshold__) {
      AbstractLeaf* newLeaf = nextPair->convert2Leaf(leavesCpt__->addNode());
      FusionContext< false >* newContext = new FusionContext< false >(newLeaf);

      for (pair_iterator pairIter = pb; pairIter != pe; ++pairIter) {
        if (!newLeaf->contains(pairIter.key()->firstLeaf()->id())
            && !newLeaf->contains(pairIter.key()->secondLeaf()->id()))
          newContext->addPair(pairIter.key());
        if (!newLeaf->contains(pairIter.key()->firstLeaf()->id())
            && !newContext->containsAssociatedLeaf(pairIter.key()->firstLeaf()))
          newContext->associateLeaf(pairIter.key()->firstLeaf());
        if (!newLeaf->contains(pairIter.key()->secondLeaf()->id())
            && !newContext->containsAssociatedLeaf(pairIter.key()->secondLeaf()))
          newContext->associateLeaf(pairIter.key()->secondLeaf());
      }

      fusionSeq__.insert(newContext);
      nextPair = fusionSeq__.back()->top();
      pb       = fusionSeq__.back()->beginPairs();
      pe       = fusionSeq__.back()->endPairs();
    }
    needsUpdate__ = false;
  }


  HashTable< NodeId, AbstractLeaf* > LeafAggregator::leavesMap() {
    HashTable< NodeId, AbstractLeaf* > retMap;
    for (SequenceIteratorSafe< FusionContext< false >* > fusIter
         = fusionSeq__.rbeginSafe();
         fusIter != fusionSeq__.rendSafe();
         --fusIter) {
      bool alreadyIn = false;
      for (HashTableIteratorSafe< NodeId, AbstractLeaf* > mapIter
           = retMap.beginSafe();
           mapIter != retMap.endSafe();
           ++mapIter)
        if (mapIter.val()->contains((*fusIter)->leaf()->id())) {
          alreadyIn = true;
          break;
        }
      if (!alreadyIn) retMap.insert((*fusIter)->leaf()->id(), (*fusIter)->leaf());
    }

    for (HashTableIteratorSafe< AbstractLeaf*, Set< LeafPair* >* > leafIter
         = leaf2Pair__.beginSafe();
         leafIter != leaf2Pair__.endSafe();
         ++leafIter) {
      for (HashTableIteratorSafe< NodeId, AbstractLeaf* > mapIter
           = retMap.beginSafe();
           mapIter != retMap.endSafe();
           ++mapIter)
        if (mapIter.val()->contains(leafIter.key()->id())) {
          retMap.insert(leafIter.key()->id(), mapIter.val());
          break;
        }
      if (!retMap.exists(leafIter.key()->id()))
        retMap.insert(leafIter.key()->id(), leafIter.key());
    }

    return retMap;
  }


  std::string LeafAggregator::toString() {
    std::stringstream ss;
    ss << "################\nTas Initial : " << std::endl
       << initialContext__->toString() << std::endl;

    for (auto fusIter = fusionSeq__.beginSafe(); fusIter != fusionSeq__.endSafe();
         ++fusIter) {
      ss << "################\nTas " << fusIter.pos() << " : " << std::endl
         << (*fusIter)->toString();
    }

    return ss.str();
  }

  // ############################################################################
  //
  // ############################################################################

  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::removeContext__(Idx startingPos) {
    for (Idx i = fusionSeq__.size() - 1; !fusionSeq__.empty() && i >= startingPos;
         --i) {
      leavesCpt__->eraseNode(fusionSeq__.atPos(i)->leaf()->id());
      delete fusionSeq__.atPos(i);
      fusionSeq__.erase(fusionSeq__.atPos(i));
    }

    needsUpdate__ = true;
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::addInitialPair__(LeafPair* p) {
    bool res = initialContext__->addPair(p);
    if (res) removeContext__(0);
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::updateInitialPair__(LeafPair* p) {
    bool res = initialContext__->updatePair(p);
    if (res) removeContext__(0);
  }

  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::removeInitialPair__(LeafPair* p) {
    bool res = initialContext__->removePair(p);
    if (res) removeContext__(0);
  }

}   // namespace gum
