/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





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
