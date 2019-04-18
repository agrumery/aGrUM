
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
 * @brief Class used to manipulate o4DGContext in Function Graph Operations
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

#ifndef O4DGCONTEXT_H
#define O4DGCONTEXT_H

#include <agrum/agrum.h>
#include <agrum/core/math/math.h>
#include <agrum/multidim/implementations/multiDimFunctionGraph.h>

namespace gum {

  // clang-format off
  /**
   * @class O4DGContext
   * @headerfile o4DGContext.h <agrum/multidim/FunctionGraphUtilities/o4DGContext.h>
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
    const NodeId& DG1Node() const { return __DG1ExploredNode; }

    /// Set DG2 diagram current explored Node
    void setDG2Node(const NodeId&);

    /// Get DG2 diagram current explored Node
    const NodeId& DG2Node() const { return __DG2ExploredNode; }
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
    NodeId __DG1ExploredNode;

    /// DG2 Diagram current explored node
    NodeId __DG2ExploredNode;

    /// Vector containing for each retrograde variable its current modality
    /// 0 meaning no instantiation done => there's an offset
    Idx* __varInstantiation;
    Idx  __nbVar;

    /// The key use to store the context as a key in the hashtable
    double __key;

    /// Table containing the log2 of prime numbers
    static const double __logPrime[];
    static const Idx    __nbLogPrime, __offsetv, __offset1, __offset2;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/multidim/utils/FunctionGraphUtilities/operators/o4DGContext_inl.h>
#endif /* GUM_NO_INLINE */

#endif   // O4DGCONTEXT_H
