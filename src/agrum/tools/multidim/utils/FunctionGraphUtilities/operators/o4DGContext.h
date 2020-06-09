
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Class used to manipulate o4DGContext in Function Graph Operations
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef O4DGCONTEXT_H
#define O4DGCONTEXT_H

#include <agrum/agrum.h>
#include <agrum/tools/core/math/math.h>
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>

namespace gum {

  // clang-format off
  /**
   * @class O4DGContext
   * @headerfile o4DGContext.h <agrum/tools/multidim/FunctionGraphUtilities/o4DGContext.h>
   * @ingroup multidim_group
   *
   * @brief Class used to manipulate context during Function Graph Operations
   */
  // clang-format on
  class O4DGContext {
    public:
    // ============================================================================
    /// @name Constructors, Destructors.
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    O4DGContext(Idx*, Idx);

    /**
     * @brief Default destructor.
     */
    ~O4DGContext();

    /**
     * @brief Operator new overload to use the SmallObjectAllocator
     */
    void* operator new(size_t s);

    /**
     * @brief Operator delete overload to use the SmallObjectAllocator
     */
    void operator delete(void* p);
    /// @}

    // ============================================================================
    /// @name Diagrams current nodes setters & getters.
    // ============================================================================

    /// @{
    /// Set DG1 diagram current explored Node
    void setDG1Node(const NodeId&);

    /// Get DG1 diagram current explored Node
    const NodeId& DG1Node() const { return DG1ExploredNode__; }

    /// Set DG2 diagram current explored Node
    void setDG2Node(const NodeId&);

    /// Get DG2 diagram current explored Node
    const NodeId& DG2Node() const { return DG2ExploredNode__; }
    /// @}

    // ============================================================================
    /// @name Variables modalities handlers
    // ============================================================================
    /// @{

    /// Changes given variable modality
    void chgVarModality(Idx, Idx);

    /// Changes given variable modality
    Idx varModality(Idx);

    /// @}
    // ============================================================================
    /// @name O4DG Handling methods
    // ============================================================================
    /// @{

    /// Returns o4DGContext key
    const double& key(short int* instNeeded);

    /// @}

    private:
    /// DG1 Diagram current explored node
    NodeId DG1ExploredNode__;

    /// DG2 Diagram current explored node
    NodeId DG2ExploredNode__;

    /// Vector containing for each retrograde variable its current modality
    /// 0 meaning no instantiation done => there's an offset
    Idx* varInstantiation__;
    Idx  nbVar__;

    /// The key use to store the context as a key in the hashtable
    double key__;

    /// Table containing the log2 of prime numbers
    static const double logPrime__[];
    static const Idx    nbLogPrime__, offsetv__, offset1__, offset2__;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/o4DGContext_inl.h>
#endif /* GUM_NO_INLINE */

#endif   // O4DGCONTEXT_H
