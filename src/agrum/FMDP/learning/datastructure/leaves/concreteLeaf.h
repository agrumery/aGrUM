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
  class ConcreteLeaf: public AbstractLeaf {
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
        AbstractLeaf(leafId),
        _n1_(n1), _valueDomain_(valueDomain) {
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
    void  operator delete(void* p) {
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
