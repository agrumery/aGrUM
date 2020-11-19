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
 * @brief Headers of the Fusion Context class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_FUSION_CONTEXT_H
#define GUM_FUSION_CONTEXT_H
// =========================================================================
#include <agrum/tools/core/multiPriorityQueue.h>
// =========================================================================
#include <agrum/tools/graphs/graphElements.h>
// =========================================================================
#include <agrum/FMDP/learning/core/templateStrategy.h>
#include <agrum/FMDP/learning/datastructure/leaves/abstractLeaf.h>
#include <agrum/FMDP/learning/datastructure/leaves/leafPair.h>
// =========================================================================

namespace gum {

  /**
   * @class FusionContext fusionContext.h
   * <agrum/FMDP/learning/datastructure/leaves/fusionContext.h>
   * @brief Contains leaves situation after a merging have been made
   * @ingroup fmdp_group
   *
   */

  typedef HashTableConstIteratorSafe< LeafPair*, std::vector< Size > >
     pair_iterator;


  template < bool isInitial = false >
  class FusionContext {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    FusionContext(AbstractLeaf*);

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~FusionContext();

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) {
      return SmallObjectAllocator::instance().allocate(s);
    }
    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(FusionContext));
    }

    /// @}

    // ==========================================================================
    /// @name Associated Leaves Handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    public:
    bool containsAssociatedLeaf(AbstractLeaf* l) {
      return containsAssociatedLeaf__(l, Int2Type< isInitial >());
    }

    private:
    bool containsAssociatedLeaf__(AbstractLeaf* l, Int2Type< false >) {
      return leaf2Pair__.exists(l);
    }
    bool containsAssociatedLeaf__(AbstractLeaf*, Int2Type< true >) {
      return false;
    }

    // ###################################################################
    ///
    // ###################################################################
    public:
    bool associateLeaf(AbstractLeaf* l) {
      return associateLeaf__(l, Int2Type< isInitial >());
    }

    private:
    bool associateLeaf__(AbstractLeaf*, Int2Type< false >);
    bool associateLeaf__(AbstractLeaf*, Int2Type< true >) { return false; }


    // ###################################################################
    ///
    // ###################################################################
    public:
    bool updateAssociatedLeaf(AbstractLeaf* l) {
      return updateAssociatedLeaf__(l, Int2Type< isInitial >());
    }

    private:
    bool updateAssociatedLeaf__(AbstractLeaf*, Int2Type< false >);
    bool updateAssociatedLeaf__(AbstractLeaf*, Int2Type< true >) { return false; }

    public:
    bool updateAllAssociatedLeaves() {
      return updateAllAssociatedLeaves__(Int2Type< isInitial >());
    }

    private:
    bool updateAllAssociatedLeaves__(Int2Type< false >);
    bool updateAllAssociatedLeaves__(Int2Type< true >) { return false; }


    // ###################################################################
    ///
    /// @warning : won't delete Associated Pair created (because subsequent
    /// fusioncontexts might be using it)
    // ###################################################################
    public:
    bool deassociateLeaf(AbstractLeaf* l) {
      return deassociateLeaf__(l, Int2Type< isInitial >());
    }

    private:
    bool deassociateLeaf__(AbstractLeaf*, Int2Type< false >);
    bool deassociateLeaf__(AbstractLeaf*, Int2Type< true >) { return false; }

    /// @}

    public:
    // ==========================================================================
    /// @name Pair handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    bool addPair(LeafPair* p);

    // ###################################################################
    ///
    // ###################################################################
    bool updatePair(LeafPair* p);

    // ###################################################################
    ///
    // ###################################################################
    bool removePair(LeafPair* p);


    pair_iterator beginPairs() { return pairsHeap__.allValues().beginSafe(); }
    pair_iterator endPairs() { return pairsHeap__.allValues().endSafe(); }

    /// @}

    // ==========================================================================
    /// @name Best Pair access methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    LeafPair* top() { return !pairsHeap__.empty() ? pairsHeap__.top() : nullptr; }

    // ###################################################################
    ///
    // ###################################################################
    double topLikelyhood() {
      return !pairsHeap__.empty() ? pairsHeap__.topPriority() : 1.0;
    }

    /// @}

    // ==========================================================================
    /// @name FusionContext Leaf and associated pairs handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    AbstractLeaf* leaf() { return leaf__; }

    // ###################################################################
    ///
    // ###################################################################
    LeafPair* leafAssociatedPair(AbstractLeaf* l) {
      return leaf2Pair__.getWithDefault(l, nullptr);
    }

    // ###################################################################
    ///
    // ###################################################################
    public:
    Set< LeafPair* > associatedPairs() {
      return associatedPairs__(Int2Type< isInitial >());
    }

    private:
    Set< LeafPair* > associatedPairs__(Int2Type< false >);
    Set< LeafPair* > associatedPairs__(Int2Type< true >) {
      return Set< LeafPair* >();
    }
    /// @}

    public:
    std::string toString();

    private:
    MultiPriorityQueue< LeafPair*, double, std::less< double > > pairsHeap__;

    HashTable< AbstractLeaf*, LeafPair* > leaf2Pair__;

    AbstractLeaf* leaf__;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/leaves/fusionContext_tpl.h>

#endif   // GUM_FUSION_CONTEXT_H
