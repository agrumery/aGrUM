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
 * @brief Headers of the abstract Leaf class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_ABSTRACT_LEAF_H
#define GUM_ABSTRACT_LEAF_H
// =========================================================================
#include <agrum/tools/core/multiPriorityQueue.h>
#include <agrum/tools/core/sequence.h>
#include <agrum/tools/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/tools/graphs/graphElements.h>
// =========================================================================

namespace gum {

  /**
   * @class AbstractLeaf abstractLeaf.h
   * <agrum/FMDP/learning/datastructure/leaves/abstractLeaf.h>
   * @brief Abstract Class implementing a Leaf
   * @ingroup fmdp_group
   *
   */


  class AbstractLeaf {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    AbstractLeaf(NodeId leafId) : _leafId_(leafId) { GUM_CONSTRUCTOR(AbstractLeaf); }

    // ###################################################################
    /// Default destructor
    // ###################################################################
    virtual ~AbstractLeaf() {
      GUM_DESTRUCTOR(AbstractLeaf);
      ;
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }
    void  operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(AbstractLeaf));
    }

    /// @}

    // ###################################################################
    /// Gaves the leaf effectif for given modality
    // ###################################################################
    virtual double effectif(Idx) const = 0;
    virtual double total() const       = 0;

    // ###################################################################
    /// Returns true if abstractleaf has leaf in it
    // ###################################################################
    virtual bool contains(NodeId testedId) const { return _leafId_ == testedId; }

    NodeId id() { return _leafId_; }

    virtual Idx nbModa() const = 0;

    virtual std::string toString() = 0;

    private:
    NodeId _leafId_;
  };


} /* namespace gum */


#endif   // GUM_ABSTRACT_LEAF_H
