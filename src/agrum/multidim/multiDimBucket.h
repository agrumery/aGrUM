/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of the class MultiDimBucket.
 *
 * @author Lionel Torti
 */
// ============================================================================
#ifndef GUM_MULTIDIM_BUCKET_H
#define GUM_MULTIDIM_BUCKET_H
// ============================================================================
#include <sstream>
// ============================================================================
#include <agrum/multidim/multiDimReadOnly.h>
// ============================================================================
namespace gum {
// ============================================================================
/**
 * @class MultiDimBucket
 * @brief A multidim implementation for buckets.
 *
 * This class behaves in two different ways, depending on the value of memory
 * allowed and the size of the resulting multidimensional table. If the
 * table is above the allowed amount of memory then value are computed when
 * demanded, having a cache for each registered instantiation to not compute
 * several times in a row the same value.
 *
 * If the memory allowed is enough to contain the bucket's table, then the
 * resulting table is computed when a value is demanded for the first time.
 *
 * @ingroup multidim_group
 */
// ============================================================================
template<typename T_DATA>
class MultiDimBucket : public MultiDimReadOnly<T_DATA> {
  public:
    // ========================================================================
    /// @name Constructor & destructor.
    // ========================================================================
    /// @{

    /**
     * Default constructor.
     *
     * The default amount of memory available for a bucket is 2^16 (65536)
     * elements in the table.
     * @param bufferSize The amount of memory allowed for this bucket.
     */
    MultiDimBucket(Size bufferSize=65536);

    /// Copy constructor.
    MultiDimBucket(const MultiDimBucket<T_DATA>& source);

    /// Destructor.
    virtual ~MultiDimBucket();

    /// @}
    // ========================================================================
    /// @name Bucket methods.
    // ========================================================================
    /// @{

    /// Add a gum::MultiDimImplementation in the bucket.
    /// @throw gum::DuplicateElement Raised if impl is already in the bucket.
    void add(const MultiDimImplementation<T_DATA>& impl);

    /// Remove a gum::MultiDimImplementation from this bucket.
    void erase(const MultiDimImplementation<T_DATA>& impl);

    /// Returns true if the gum::MultiDimImplementation is in this bucket.
    bool contains(const MultiDimImplementation<T_DATA>& impl) const;

    /// Returns the number of gum::MultiDimImplementation in in this bukcet.
    Size bucketSize() const;

    /**
     * Returns true if this bucket is empty.
     *
     * Which mean thath there is no gum::MultiDimImplementation in this bucket.
     */
    bool isBucketEmpty() const;

    /**
     * Returns true if the bucket need re-computation since the last
     * computation.
     */
    bool bucketChanged() const;

    /**
     * @brief This method computes the final table of this bucket.
     *
     * A flag is used to prevent unnecessary computation if the table has
     * already been computed.
     *
     * If the size of the final table is above the amount of authorized memory,
     * an gum::OperationNotAllowed is raised.
     *
     * Remember that this method is constant because the content of a
     * multidimensional table is mutable.
     *
     * @param force If true (default set at false) then the final table is
     *              re-computed.
     * @throw OperationNotAllowed Raised if the size of the final table is above
     *                            the authorized amount of memory.
     */
    void compute(bool force=false) const;

    /// @}

    // ========================================================================
    /// Accessors and modifiers over variables.
    // ========================================================================
    /// @{

    /// See gum::MultiDimInterface::add().
    virtual void add (const DiscreteVariable &v);

    /// See gum::MultiDimInterface::erase().
    virtual void erase (const DiscreteVariable &v);

    /// See gum::MultiDimImplementation::realSize().
    virtual Size realSize() const;

    /// @}
    // ========================================================================
    /// Accessors over values.
    // ========================================================================
    /// @{

    /// See gum::MultiDimContainer::get().
    virtual T_DATA get (const Instantiation &i) const;


    /// @}
    // ========================================================================
    /// Slave management and extension due to slave management
    // ========================================================================
    /// @{

    /// See gum::MultiDimAdressable::changeNotification().
    virtual void changeNotification (Instantiation &i,
                                     const DiscreteVariable *const var,
                                     const Idx &oldval, const Idx &newval);

    /// See gum::MultiDimAdressable::setFirstNotification().
    virtual void setFirstNotification (Instantiation &i);

    /// See gum::MultiDimAdressable::setLastNotification().
    virtual void setLastNotification (Instantiation &i);

    /// See gum::MultiDimAdressable::setIncNotification().
    virtual void setIncNotification (Instantiation &i);

    /// See gum::MultiDimAdressable::setDecNotification().
    virtual void setDecNotification (Instantiation &i);

    /// See gum::MultiDimAdressable::setFirstNotification().
    virtual void setChangeNotification (Instantiation &i);

    /// See gum::MultiDimAdressable::registerSlave().
    virtual bool registerSlave (Instantiation &i);

    /// See gum::MultiDimAdressable::unregisterSlave().
    virtual bool unregisterSlave (Instantiation &i);

    /// @}

    /// String representation of internal data about i in this.
    virtual const std::string toString (const Instantiation *i) const;

  protected:
    /// See gum::MultiDimImplementation::_commitMultipleChanges().
    virtual void _commitMultipleChanges();

    /// This will raise en exception, you should directly use the get() and
    /// operator[]() methods.
    virtual T_DATA& _get(const Instantiation &i) const;

  private:
    /// The number of element allowed in __bucket.
    Size __bufferSize;

    /// The result table of this bucket.
    MultiDimArray<T_DATA>* __bucket;

    /// The set of gum::MultiDimImplementation in this bucket.
    Set<const MultiDimImplementation<T_DATA>* > __multiDims;

    /// The set of all variables of the multidims in this bucket.
    Set<const DiscreteVariable*> __allVariables;

    /// Add a variable to __allVariables, and do nothing if var is already in
    /// the set.
    void __addVariable(const DiscreteVariable* var);

    /// Erase a variable from __allVariables if no other multidimensional table
    /// uses it in this bucket.
    void __eraseVariable(const DiscreteVariable* var);

    /// Compute the value of the final table of this bucket given i.
    /// If i variables are a subset of this bucket, then the missing values are
    /// supposed to be at 0.
    /// @param value The value to compute.
    /// @param buffer An initialized MultiDimArray with all variables not in this
    ///               bucket final table.
    T_DATA __computeValue(const Instantiation& value) const;

    /// Flag used to know if changes has occurred in the bucket since last
    /// computation.
    mutable bool __changed;

    /// This table is used to keep the last value computed for an instantiation
    /// when the value are computed on the fly.
    mutable HashTable<const Instantiation*, T_DATA> __slavesValue;

};
// ============================================================================
} /* namespace gum */
// ============================================================================
#include <agrum/multidim/multiDimBucket.tcc>
// ============================================================================
#endif /* GUM_MULTIDIM_BUCKET_H */
// ============================================================================
