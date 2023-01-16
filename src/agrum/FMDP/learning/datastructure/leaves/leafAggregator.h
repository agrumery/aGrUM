/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Headers of the Leaf Aggregator class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_LEAF_AGGREGATOR_H
#define GUM_LEAF_AGGREGATOR_H
// =========================================================================
// =========================================================================
// =========================================================================
// =========================================================================
#include <agrum/FMDP/learning/datastructure/leaves/concreteLeaf.h>
#include <agrum/FMDP/learning/datastructure/leaves/fusionContext.h>
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

    bool needsUpdate() { return _needsUpdate_; }

    HashTable< NodeId, AbstractLeaf* > leavesMap();

    std::string toString();

    private:
    void _removeContext_(Idx);
    void _addInitialPair_(LeafPair*);
    void _updateInitialPair_(LeafPair*);
    void _removeInitialPair_(LeafPair*);

    Sequence< FusionContext< false >* > _fusionSeq_;

    FusionContext< true >* _initialContext_;

    HashTable< AbstractLeaf*, Set< LeafPair* >* > _leaf2Pair_;

    NodeGraphPart* _leavesCpt_;

    double _similarityThreshold_;
    bool   _needsUpdate_;
  };


} /* namespace gum */


#endif   // GUM_LEAF_AGGREGATOR_H
