/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
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
  LeafAggregator::LeafAggregator(NodeGraphPart* idSource, double similarityThreshold) :
      _leavesCpt_(idSource), _similarityThreshold_(similarityThreshold) {
    GUM_CONSTRUCTOR(LeafAggregator);
    _initialContext_ = new FusionContext< true >(nullptr);
    _needsUpdate_    = false;
  }

  // ============================================================================
  // Default constructor.
  // ============================================================================
  LeafAggregator::~LeafAggregator() {
    _removeContext_(0);

    delete _initialContext_;

    for (HashTableIteratorSafe< AbstractLeaf*, Set< LeafPair* >* > leafIter
         = _leaf2Pair_.beginSafe();
         leafIter != _leaf2Pair_.endSafe();
         ++leafIter) {
      for (SetIteratorSafe< LeafPair* > pairIter = leafIter.val()->beginSafe();
           pairIter != leafIter.val()->endSafe();
           ++pairIter) {
        LeafPair* curPair = *pairIter;
        _leaf2Pair_[curPair->otherLeaf(leafIter.key())]->erase(*pairIter);
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
         = _leaf2Pair_.cbeginSafe();
         leafIter != _leaf2Pair_.cendSafe();
         ++leafIter) {
      // Création de la pair et ajout dans les listes de pair des feuilles de
      // base
      LeafPair* p = new LeafPair(l, leafIter.key());
      p->updateLikelyhood();
      leafPairSet->insert(p);
      (leafIter.val())->insert(p);

      // Ajout de la nouvelle pair au tas initial
      _addInitialPair_(p);

      bag.insert(p);
    }

    // ****************************************************************************************
    // Enregistrement de la nouvelle Feuille en tant que feuille de base
    _leaf2Pair_.insert(l, leafPairSet);

    // ****************************************************************************************
    // Ajout de la feuille aux FusionContext

    for (SequenceIteratorSafe< FusionContext< false >* > fusIter = _fusionSeq_.beginSafe();
         fusIter != _fusionSeq_.endSafe();
         ++fusIter) {
      // Ajout de la nouvelle pair composée de la feuille de FusIter et de la
      // nouvelle feuille aux FusionContext suivant
      for (SetIteratorSafe< LeafPair* > pairIter = bag.beginSafe(); pairIter != bag.endSafe();
           ++pairIter) {
        if ((*fusIter)->leaf()->contains((*pairIter)->secondLeaf()->id())) {
          bag >> *pairIter;
          continue;
        }

        if ((*fusIter)->addPair(*pairIter)) _removeContext_(fusIter.pos() + 1);
      }

      if ((*fusIter)->associateLeaf(l)) _removeContext_(fusIter.pos() + 1);

      bag << (*fusIter)->leafAssociatedPair(l);
    }

    _needsUpdate_ = true;
  }


  // ============================================================================
  //
  // ============================================================================
  bool LeafAggregator::updateLeaf(AbstractLeaf* l) {
    // ***********************************************************************************
    // First we update every base pair linked to that leaf
    Set< LeafPair* > bag(*(_leaf2Pair_[l]));
    for (SetIteratorSafe< LeafPair* > pairIter = bag.beginSafe(); pairIter != bag.endSafe();
         ++pairIter) {
      (*pairIter)->updateLikelyhood();
      _updateInitialPair_(*pairIter);
    }

    // **********************************************************************************
    // The we have top update FusionContext pairs associated to that leaf
    AbstractLeaf* curLeaf = l;
    for (SequenceIteratorSafe< FusionContext< false >* > fusIter = _fusionSeq_.beginSafe();
         fusIter != _fusionSeq_.endSafe();
         ++fusIter) {
      if ((*fusIter)->leaf()->contains(curLeaf->id())) {
        bag.clear();
        if ((*fusIter)->updateAllAssociatedLeaves()) _removeContext_(fusIter.pos() + 1);
        bag     = (*fusIter)->associatedPairs();
        curLeaf = (*fusIter)->leaf();
        continue;
      }

      for (SetIteratorSafe< LeafPair* > pairIter = bag.beginSafe(); pairIter != bag.endSafe();
           ++pairIter) {
        if ((*fusIter)->leaf()->contains((*pairIter)->secondLeaf()->id())
            || (*fusIter)->leaf()->contains((*pairIter)->firstLeaf()->id())) {
          bag >> *pairIter;
          continue;
        }

        if ((*fusIter)->updatePair(*pairIter)) _removeContext_(fusIter.pos() + 1);
      }
      if ((*fusIter)->updateAssociatedLeaf(curLeaf)) _removeContext_(fusIter.pos() + 1);
      bag << (*fusIter)->leafAssociatedPair(curLeaf);
    }

    return _needsUpdate_;
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::removeLeaf(AbstractLeaf* l) {
    // ***********************************************************************************
    // First we update every base pair linked to that leaf
    Set< LeafPair* > bag(*(_leaf2Pair_[l]));
    for (SetIteratorSafe< LeafPair* > pairIter = bag.beginSafe(); pairIter != bag.endSafe();
         ++pairIter) {
      _removeInitialPair_(*pairIter);
      (*_leaf2Pair_[(*pairIter)->otherLeaf(l)]) >> *pairIter;
    }

    // **********************************************************************************
    // The we have top update FusionContext pairs associated to that leaf
    Set< LeafPair* > toBeDeleted;
    for (SequenceIteratorSafe< FusionContext< false >* > fusIter = _fusionSeq_.beginSafe();
         fusIter != _fusionSeq_.endSafe();
         ++fusIter) {
      for (SetIteratorSafe< LeafPair* > pairIter = bag.beginSafe(); pairIter != bag.endSafe();
           ++pairIter) {
        if ((*fusIter)->leaf()->contains((*pairIter)->secondLeaf()->id())
            || (*fusIter)->leaf()->contains((*pairIter)->firstLeaf()->id())) {
          bag >> *pairIter;
          continue;
        }

        if ((*fusIter)->removePair(*pairIter)) { _removeContext_(fusIter.pos() + 1); }
      }

      bag << (*fusIter)->leafAssociatedPair(l);
      toBeDeleted << (*fusIter)->leafAssociatedPair(l);

      if ((*fusIter)->deassociateLeaf(l)) { _removeContext_(fusIter.pos() + 1); }
    }

    for (SetIteratorSafe< LeafPair* > pairIter = toBeDeleted.beginSafe();
         pairIter != toBeDeleted.endSafe();
         ++pairIter)
      delete *pairIter;

    for (SetIteratorSafe< LeafPair* > pairIter = _leaf2Pair_[l]->beginSafe();
         pairIter != _leaf2Pair_[l]->endSafe();
         ++pairIter)
      delete *pairIter;
    delete _leaf2Pair_[l];
    _leaf2Pair_.erase(l);

    _needsUpdate_ = true;
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::update() {
    LeafPair*     nextPair = _initialContext_->top();
    pair_iterator pb       = _initialContext_->beginPairs();
    pair_iterator pe       = _initialContext_->endPairs();
    if (!_fusionSeq_.empty()) {
      nextPair = _fusionSeq_.back()->top();
      pb       = _fusionSeq_.back()->beginPairs();
      pe       = _fusionSeq_.back()->endPairs();
    }


    while (nextPair && nextPair->likelyhood() < _similarityThreshold_) {
      AbstractLeaf*           newLeaf    = nextPair->convert2Leaf(_leavesCpt_->addNode());
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

      _fusionSeq_.insert(newContext);
      nextPair = _fusionSeq_.back()->top();
      pb       = _fusionSeq_.back()->beginPairs();
      pe       = _fusionSeq_.back()->endPairs();
    }
    _needsUpdate_ = false;
  }


  HashTable< NodeId, AbstractLeaf* > LeafAggregator::leavesMap() {
    HashTable< NodeId, AbstractLeaf* > retMap;
    for (SequenceIteratorSafe< FusionContext< false >* > fusIter = _fusionSeq_.rbeginSafe();
         fusIter != _fusionSeq_.rendSafe();
         --fusIter) {
      bool alreadyIn = false;
      for (HashTableIteratorSafe< NodeId, AbstractLeaf* > mapIter = retMap.beginSafe();
           mapIter != retMap.endSafe();
           ++mapIter)
        if (mapIter.val()->contains((*fusIter)->leaf()->id())) {
          alreadyIn = true;
          break;
        }
      if (!alreadyIn) retMap.insert((*fusIter)->leaf()->id(), (*fusIter)->leaf());
    }

    for (HashTableIteratorSafe< AbstractLeaf*, Set< LeafPair* >* > leafIter
         = _leaf2Pair_.beginSafe();
         leafIter != _leaf2Pair_.endSafe();
         ++leafIter) {
      for (HashTableIteratorSafe< NodeId, AbstractLeaf* > mapIter = retMap.beginSafe();
           mapIter != retMap.endSafe();
           ++mapIter)
        if (mapIter.val()->contains(leafIter.key()->id())) {
          retMap.insert(leafIter.key()->id(), mapIter.val());
          break;
        }
      if (!retMap.exists(leafIter.key()->id())) retMap.insert(leafIter.key()->id(), leafIter.key());
    }

    return retMap;
  }


  std::string LeafAggregator::toString() {
    std::stringstream ss;
    ss << "################\nTas Initial : " << std::endl
       << _initialContext_->toString() << std::endl;

    for (auto fusIter = _fusionSeq_.beginSafe(); fusIter != _fusionSeq_.endSafe(); ++fusIter) {
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
  void LeafAggregator::_removeContext_(Idx startingPos) {
    for (Idx i = _fusionSeq_.size() - 1; !_fusionSeq_.empty() && i >= startingPos; --i) {
      _leavesCpt_->eraseNode(_fusionSeq_.atPos(i)->leaf()->id());
      delete _fusionSeq_.atPos(i);
      _fusionSeq_.erase(_fusionSeq_.atPos(i));
    }

    _needsUpdate_ = true;
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::_addInitialPair_(LeafPair* p) {
    bool res = _initialContext_->addPair(p);
    if (res) _removeContext_(0);
  }


  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::_updateInitialPair_(LeafPair* p) {
    bool res = _initialContext_->updatePair(p);
    if (res) _removeContext_(0);
  }

  // ============================================================================
  //
  // ============================================================================
  void LeafAggregator::_removeInitialPair_(LeafPair* p) {
    bool res = _initialContext_->removePair(p);
    if (res) _removeContext_(0);
  }

}   // namespace gum
