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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Headers of the Concrete Leaf class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

// =========================================================================
#ifndef GUM_CONCRETE_LEAF_H
#define GUM_CONCRETE_LEAF_H
// =========================================================================
#include <agrum/agrum.h>
// =========================================================================
// =========================================================================
#include <agrum/FMDP/learning/datastructure/leaves/abstractLeaf.h>
#include <agrum/FMDP/learning/datastructure/nodeDatabase.h>

// =========================================================================

namespace gum {

  /**
   * @class ConcreteLeaf concreteLeaf.h
   * <agrum/FMDP/learning/datastructure/leaves/concreteLeaf.h>
   * @brief A concrete leaf
   * @ingroup fmdp_group
   *
   */


  template < TESTNAME AttributeSelection, bool isScalar >
  class ConcreteLeaf final: public AbstractLeaf {
    using ValueType = typename ValueSelect< isScalar, double, Idx >::type;

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    ConcreteLeaf(NodeId                                        leafId,
                 NodeDatabase< AttributeSelection, isScalar >* n1,
                 const Sequence< ValueType >*                  valueDomain) :
        AbstractLeaf(leafId), _n1_(n1), _valueDomain_(valueDomain) {
      GUM_CONSTRUCTOR(ConcreteLeaf);
    }

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~ConcreteLeaf() {
      GUM_DESTRUCTOR(ConcreteLeaf);
      ;
    }

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }

    void operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(ConcreteLeaf));
    }

    /// @}

    // ###################################################################
    /// Gaves the leaf effectif for given modality
    // ###################################################################
    virtual double effectif(Idx moda) const { return _effectif_(moda, Int2Type< isScalar >()); }

    private:
    double _effectif_(Idx moda, Int2Type< true >) const {
      return (double)_n1_->effectif(Idx(_valueDomain_->atPos(moda)));
    }

    double _effectif_(Idx moda, Int2Type< false >) const { return (double)_n1_->effectif(moda); }

    public:
    virtual double total() const { return double(_n1_->nbObservation()); }

    Idx nbModa() const { return _nbModa_(Int2Type< isScalar >()); }

    private:
    Idx _nbModa_(Int2Type< true >) const { return _valueDomain_->size(); }

    Idx _nbModa_(Int2Type< false >) const { return _n1_->valueDomain(); }

    public:
    std::string toString() {
      std::stringstream ss;
      ss << "{ Id : " << this->id() << "}";
      return ss.str();
    }

    private:
    NodeDatabase< AttributeSelection, isScalar >* _n1_;
    const Sequence< ValueType >*                  _valueDomain_;
  };


} /* namespace gum */


#endif   // GUM_CONCRETE_LEAF_H
