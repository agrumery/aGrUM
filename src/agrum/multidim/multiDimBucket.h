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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_MULTIDIM_BUCKET_H
#define GUM_MULTIDIM_BUCKET_H

#include <sstream>
#include <climits>

#include <agrum/core/set.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/sequence.h>
#include <agrum/core/bijection.h>

#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/multiDimReadOnly.h>

namespace gum {

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
   * Since a MultiDimArray is used as a buffer, an instantiation real master
   * will be the internal buffer of a gum::MultiDimBucket. This is why you should
   * always call the gum::Instantiation::isMaster() method with the reference
   * retuned by gum::MultiDimBucket::getMasterRef().
   *
   * TODO handle slave switch between buffer and hashtable.
   * @ingroup multidim_group
   */

  template <typename GUM_SCALAR>
  class MultiDimBucket : public MultiDimReadOnly<GUM_SCALAR> {
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
    MultiDimBucket(Size bufferSize = INT_MAX);

    /// Copy constructor.
    MultiDimBucket(const MultiDimBucket<GUM_SCALAR> &source);

    /// Destructor.
    virtual ~MultiDimBucket();

    /// @}

    /**
     * This method creates a clone of this object, withouth its content
     * (including variable), you must use this method if you want to ensure
     * that the generated object has the same type than the object containing
     * the called newFactory()
     * For example :
     *   MultiDimArray<double> y;
     *   MultiDimContainer<double>* x = y.newFactory();
     * Then x is a MultiDimArray<double>*
     *
     * @warning you must desallocate by yourself the memory
     * @return an empty clone of this object with the same type
     */
    virtual MultiDimContainer<GUM_SCALAR> *newFactory() const;

    // ========================================================================
    /// @name Bucket methods.
    // ========================================================================
    /// @{

    /// Add a gum::MultiDimContainer in the bucket.
    /// @throw gum::DuplicateElement Raised if impl is already in the bucket.
    void add(const MultiDimContainer<GUM_SCALAR> &impl);

    /// Add a gum::MultiDimContainer in the bucket.
    /// @throw gum::DuplicateElement Raised if impl is already in the bucket.
    void add(const MultiDimContainer<GUM_SCALAR> *impl);

    /// Remove a gum::MultiDimContainer from this bucket.
    void erase(const MultiDimContainer<GUM_SCALAR> &impl);

    /// Remove a gum::MultiDimContainer from this bucket.
    void erase(const MultiDimContainer<GUM_SCALAR> *impl);

    /// Returns true if the gum::MultiDimContainer is in this bucket.
    bool contains(const MultiDimContainer<GUM_SCALAR> &impl) const;

    /// Returns the sequence of all the variables contained in the bucket.
    const Set<const DiscreteVariable *> &allVariables() const;

    /// Returns the number of gum::MultiDimContainer in in this bukcet.
    Size bucketSize() const;

    /**
     * Returns true if this bucket is empty.
     *
     * Which mean thath there is no gum::MultiDimContainer in this bucket.
     */
    bool isBucketEmpty() const;

    /**
     * Returns true if the bucket need re-computation since the last
     * computation.
     */
    bool bucketChanged() const;

    /**
     * Returns the MultiDimArray used by this MultiDimBucket.
     * @throw OperationNotAllowed If it'not built.
     */
    const MultiDimArray<GUM_SCALAR> &bucket() const;

    /// Returns the amount of memory allowed for this bucket.
    Size bufferSize() const;

    /**
     * @brief Changes the amount of memory allowed for this bucket.
     *
     * If the new amount is not enough for the current size of this bucket, then
     * internal buffer is deleted. In the other case, the internal buffer is created
     * but not computed.
     */
    void setBufferSize(Size ammount);

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
     * @throw SizeError Raised if the bucket is empty.
     */
    void compute(bool force = false) const;

    const std::string &name() const;

    /// @}

    // ========================================================================
    /// @name Accessors and modifiers over variables.
    // ========================================================================
    /// @{

    /// See gum::MultiDimInterface::add().
    virtual void add(const DiscreteVariable &v);

    /// See gum::MultiDimInterface::erase().
    virtual void erase(const DiscreteVariable &v);

    /// See gum::MultiDimImplementation::realSize().
    virtual Size realSize() const;

    /// See gum::MultiDimImplementation::contains(const DiscreteVariable& v).
    bool contains(const DiscreteVariable &v) const;

    /// @}
    // ========================================================================
    /// @name Accessors over values.
    // ========================================================================
    /// @{

    /// See gum::MultiDimContainer::get().
    virtual GUM_SCALAR get(const Instantiation &i) const;

    /// @}
    // ========================================================================
    /// @name Slave management and extension due to slave management
    // ========================================================================
    /// @{

    /// See gum::MultiDimAdressable::changeNotification().
    virtual void changeNotification(Instantiation &i,
                                    const DiscreteVariable *const var,
                                    const Idx &oldval, const Idx &newval);

    /// See gum::MultiDimAdressable::setFirstNotification().
    virtual void setFirstNotification(Instantiation &i);

    /// See gum::MultiDimAdressable::setLastNotification().
    virtual void setLastNotification(Instantiation &i);

    /// See gum::MultiDimAdressable::setIncNotification().
    virtual void setIncNotification(Instantiation &i);

    /// See gum::MultiDimAdressable::setDecNotification().
    virtual void setDecNotification(Instantiation &i);

    /// See gum::MultiDimAdressable::setFirstNotification().
    virtual void setChangeNotification(Instantiation &i);

    /// See gum::MultiDimAdressable::registerSlave().
    virtual bool registerSlave(Instantiation &i);

    /// See gum::MultiDimAdressable::unregisterSlave().
    virtual bool unregisterSlave(Instantiation &i);

    /// See gum::MultiDimAdressable::getMasterRef().
    virtual MultiDimAdressable &getMasterRef(void);

    /// See gum::MultiDimAdressable::getMasterRef().
    virtual const MultiDimAdressable &getMasterRef(void) const;

    /// @}

    /// String representation of internal data about i in this.
    virtual const std::string toString(const Instantiation *i) const;

    const HashTable<const MultiDimContainer<GUM_SCALAR> *, Instantiation *> &
    multidims() const;

    protected:
    /// See gum::MultiDimImplementation::_commitMultipleChanges().
    virtual void _commitMultipleChanges();

    /// This will raise en exception, you should directly use the get() and
    /// operator[]() methods.
    virtual GUM_SCALAR &_get(const Instantiation &i) const;

    virtual void _swap(const DiscreteVariable *x, const DiscreteVariable *y);

    private:
    /// The number of element allowed in __bucket.
    Size __bufferSize;

    /// The list of instantiations registered on __bucket.
    // List<Instantiation*>* __instantiations;

    /// Bijection between instantiations registered on this and
    /// their equivalent on __bucket
    Bijection<Instantiation *, Instantiation *> __instantiations;

    /// The result table of this bucket.
    MultiDimArray<GUM_SCALAR> *__bucket;

    /// The set of gum::MultiDimContainer in this bucket.
    mutable HashTable<const MultiDimContainer<GUM_SCALAR> *, Instantiation *>
        __multiDims;

    /// The set of all variables of the multidims in this bucket.
    Set<const DiscreteVariable *> __allVariables;

    /// Instantiation over all variable in this
    mutable Instantiation __allVarsInst;

    /// Add a variable to __allVariables, and do nothing if var is already in
    /// the set.
    void __addVariable(const DiscreteVariable *var);

    /// Erase a variable from __allVariables if no other multidimensional table
    /// uses it in this bucket.
    void __eraseVariable(const DiscreteVariable *var);

    /// Initialize the internal buffer.
    /// This method delete __bucket after saving it's slave instantiations.
    void __initializeBuffer();

    /// Clean the buffer and switch it's instantiation to this bucket.
    void __eraseBuffer();

    /// Compute the value of the final table of this bucket given i.
    /// If i variables are a subset of this bucket, then the missing values are
    /// supposed to be at 0.
    /// @param value The value to compute.
    /// @throw SizeError Raised if the bucket is empty.
    GUM_SCALAR __computeValue(const Instantiation &value) const;

    /// Flag used to know if changes has occurred in the bucket since last
    /// computation.
    mutable bool __changed;

    /// This table is used to keep the last value computed for an instantiation
    /// when the value are computed on the fly.
    mutable HashTable<const Instantiation *, GUM_SCALAR> __slavesValue;

    std::string __name;
  };

} /* namespace gum */

#include <agrum/multidim/multiDimBucket.tcc>

#endif /* GUM_MULTIDIM_BUCKET_H */
