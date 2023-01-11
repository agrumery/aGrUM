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
 * @brief Class used to manipulate o4DGContext in Function Graph Operations
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/o4DGContext.h>

namespace gum {

  // Set DG1 diagram current explored Node
  INLINE void O4DGContext::setDG1Node(const NodeId& exploredNode) {
    _DG1ExploredNode_ = exploredNode;
  }

  // Set DG2 diagram current explored Node
  INLINE void O4DGContext::setDG2Node(const NodeId& exploredNode) {
    _DG2ExploredNode_ = exploredNode;
  }

  // Changes given variable modality
  INLINE void O4DGContext::chgVarModality(Idx varIndex, Idx newModality) {
    _varInstantiation_[varIndex] = newModality;
  }

  // Changes given variable modality
  INLINE Idx O4DGContext::varModality(Idx varIndex) { return _varInstantiation_[varIndex]; }

  // Returns o4DGContext key
  INLINE const double& O4DGContext::key(short int* instNeeded) {
    _key_ = _DG1ExploredNode_ * _logPrime_[_offset1_] + _DG2ExploredNode_ * _logPrime_[_offset2_];

    for (Idx varPos = 0, offset = _offsetv_; varPos < _nbVar_; varPos++, offset--)
      if (instNeeded[varPos]) _key_ += _varInstantiation_[varPos] * _logPrime_[offset];

    return _key_;
  }

  INLINE void* O4DGContext::operator new(size_t s) {
    return SmallObjectAllocator::instance().allocate(s);
  }

  INLINE void O4DGContext::operator delete(void* p) {
    SmallObjectAllocator::instance().deallocate(p, sizeof(O4DGContext));
  }
  /// @}


} /* end of namespace gum */
