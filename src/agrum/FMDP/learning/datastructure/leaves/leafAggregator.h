
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief Headers of the Leaf Aggregator class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_LEAF_AGGREGATOR_H
#define GUM_LEAF_AGGREGATOR_H
// =========================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/multiPriorityQueue.h>
#include <agrum/core/sequence.h>
// =========================================================================
#include <agrum/graphs/graphElements.h>
#include <agrum/graphs/parts/nodeGraphPart.h>
// =========================================================================
#include <agrum/multidim/utils/FunctionGraphUtilities/link.h>
// =========================================================================
#include <agrum/FMDP/learning/datastructure/leaves/abstractLeaf.h>
#include <agrum/FMDP/learning/datastructure/leaves/concreteLeaf.h>
#include <agrum/FMDP/learning/datastructure/leaves/fusionContext.h>
#include <agrum/FMDP/learning/datastructure/leaves/leafPair.h>
// =========================================================================

namespace gum {

  /**
   * @class LeafAggregator leafAggregator.h
   * <agrum/FMDP/learning/FunctionGraph/leafAggregator.h>
   * @brief Gather together leaves with similar distributions
   * @ingroup fmdp_group
   *
   */


  class LeafAggregator {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    LeafAggregator(NodeGraphPart* idSource, double similarityThreshold);

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~LeafAggregator();

    /// @}

    // ==========================================================================
    /// @name Leaf Handling methods
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    void addLeaf(AbstractLeaf*);

    // ###################################################################
    ///
    // ###################################################################
    bool updateLeaf(AbstractLeaf*);

    // ###################################################################
    ///
    // ###################################################################
    void removeLeaf(AbstractLeaf*);

    /// @}

    // ==========================================================================
    /// @name
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    void begin();

    // ###################################################################
    ///
    // ###################################################################
    bool hasNext();

    // ###################################################################
    ///
    // ###################################################################
    double next();

    /// @}

    void update();

    bool needsUpdate() { return __needsUpdate; }

    HashTable< NodeId, AbstractLeaf* > leavesMap();

    std::string toString();

    private:
    void __removeContext(Idx);
    void __addInitialPair(LeafPair*);
    void __updateInitialPair(LeafPair*);
    void __removeInitialPair(LeafPair*);

    Sequence< FusionContext< false >* > __fusionSeq;

    FusionContext< true >* __initialContext;

    HashTable< AbstractLeaf*, Set< LeafPair* >* > __leaf2Pair;

    NodeGraphPart* __leavesCpt;

    double __similarityThreshold;
    bool   __needsUpdate;
  };


} /* namespace gum */


#endif   // GUM_LEAF_AGGREGATOR_H
