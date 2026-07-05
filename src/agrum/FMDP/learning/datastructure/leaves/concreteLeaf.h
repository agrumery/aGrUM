/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
                 const Sequence< ValueType >*                  valueDomain);

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~ConcreteLeaf() override;

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s);

    void operator delete(void* p);

    /// @}

    // ###################################################################
    /// Gaves the leaf effectif for given modality
    // ###################################################################
    double effectif(Idx moda) const override;

    private:
    double _effectif_(Idx moda, Int2Type< true >) const;

    double _effectif_(Idx moda, Int2Type< false >) const;

    public:
    double total() const override;

    Idx nbModa() const override;

    private:
    Idx _nbModa_(Int2Type< true >) const;

    Idx _nbModa_(Int2Type< false >) const;

    public:
    std::string toString() override;

    private:
    NodeDatabase< AttributeSelection, isScalar >* _n1_;
    const Sequence< ValueType >*                  _valueDomain_;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/leaves/concreteLeaf_tpl.h>
#endif   // GUM_CONCRETE_LEAF_H
