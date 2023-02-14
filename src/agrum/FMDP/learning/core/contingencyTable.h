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
 * @brief Headers of the ContingencyTable class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */


#ifndef GUM_CONTINGENCY_TABLE_H
#define GUM_CONTINGENCY_TABLE_H

#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/core/set.h>

#include <agrum/FMDP/learning/observation.h>


namespace gum {

  /**
   * @class ContingencyTable contingencyTable.h
   * <agrum/FMDP/learning/core/contingencyTable.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template < typename GUM_SCALAR_A, typename GUM_SCALAR_B >
  class ContingencyTable {
    public:
    // ##########################################################################
    /// @name Constructor & destructor.
    // ##########################################################################
    /// @{


    /// Default constructor

    ContingencyTable();


    /// Default destructor

    ~ContingencyTable();

    // ============================================================================
    /// Allocators and Deallocators redefinition
    // ============================================================================
    void* operator new(size_t s) { return SmallObjectAllocator::instance().allocate(s); }
    void  operator delete(void* p) {
      SmallObjectAllocator::instance().deallocate(p, sizeof(ContingencyTable));
    }

    /// @}


    /// @name

    /// @{


    /// Increments the number of sample for case( iattr, ivalue )

    void add(GUM_SCALAR_A valueA, GUM_SCALAR_B valueB);


    /// Returns the number of samples for case (iattr, ivalue)

    Idx joint(GUM_SCALAR_A valueA, GUM_SCALAR_B valueB) const {
      return _jointTable_.exists(std::pair< GUM_SCALAR_A, GUM_SCALAR_B >(valueA, valueB))
              ? _jointTable_[std::pair< GUM_SCALAR_A, GUM_SCALAR_B >(valueA, valueB)]
              : 0;
    }


    /// Returns the number of samples for case (iattr, ivalue)

    Idx attrAMarginal(GUM_SCALAR_A valueA) const {
      return _attrAMarginalTable_.exists(valueA) ? _attrAMarginalTable_[valueA] : 0;
    }


    /// Returns the number of samples for case (iattr, ivalue)

    Idx attrBMarginal(GUM_SCALAR_B valueB) const {
      return _attrAMarginalTable_.exists(valueB) ? _attrAMarginalTable_[valueB] : 0;
    }


    /// Returns the number of samples for line iattr

    //        Idx aMarginal( GUM_SCALAR_A iattr ) { return
    //         _attrMarginalTable_[iattr]; }
    HashTableConstIteratorSafe< GUM_SCALAR_A, Idx > attrABeginSafe() const {
      return _attrAMarginalTable_.cbeginSafe();
    }
    HashTableConstIteratorSafe< GUM_SCALAR_A, Idx > attrAEndSafe() const {
      return _attrAMarginalTable_.cendSafe();
    }


    /// Returns the number of samples for column ivalue

    //        Idx vMarginal( GUM_SCALAR_B ivalue ) { return
    //         _valueMarginalTable_[ivalue]; }
    HashTableConstIteratorSafe< GUM_SCALAR_B, Idx > attrBBeginSafe() const {
      return _attrBMarginalTable_.cbeginSafe();
    }
    HashTableConstIteratorSafe< GUM_SCALAR_B, Idx > attrBEndSafe() const {
      return _attrBMarginalTable_.cendSafe();
    }


    /// Returns the number of samples for line iattr

    Idx attrASize() const { return _attrAMarginalTable_.size(); }


    /// Returns the number of samples for column ivalue

    Idx attrBSize() const { return _attrBMarginalTable_.size(); }

    /// @}

    ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >&
       operator+=(const ContingencyTable< GUM_SCALAR_A, GUM_SCALAR_B >& src);

    std::string toString() const {
      std::stringstream ss;
      ss << "\t\t\t\t" << _attrAMarginalTable_ << std::endl
         << "\t\t\t\t" << _attrBMarginalTable_ << std::endl
         << "\t\t\t\t" << _jointTable_ << std::endl;
      return ss.str();
    }

    private:
    /**
     * The contingency table used to compute the GStat
     * Left Idx is for the attribute
     * Right Idx for the value
     *
     * NB: This is a silly and in a hurry implementation of contingency table
     * If someone ever use this class and has time to correctly implements
     * a efficient contingency table, you're welcome
     */
    HashTable< std::pair< GUM_SCALAR_A, GUM_SCALAR_B >, Idx > _jointTable_;
    HashTable< GUM_SCALAR_A, Idx >                            _attrAMarginalTable_;
    HashTable< GUM_SCALAR_B, Idx >                            _attrBMarginalTable_;
  };

} /* namespace gum */

#include <agrum/FMDP/learning/core/contingencyTable_tpl.h>
#endif   // GUM_CONTINGENCY_TABLE_H
