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
 * @brief Headers of the MultiDimBucket class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * @author Lionel TORTI
 */

#ifndef GUM_MULTIDIM_BUCKET_H
#define GUM_MULTIDIM_BUCKET_H

#include <climits>
#include <sstream>

#include <agrum/tools/core/bijection.h>

#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/multidim/implementations/multiDimReadOnly.h>

namespace gum {

  /**
   * @class MultiDimBucket
   * @headerfile multiDimBucket.h <agrum/tools/multidim/multiDimBucket.h>
   * @ingroup multidim_group
   *
   * @brief A multidim implementation for buckets.
   *
   * This class behaves in two different ways, depending on the value of memory
   * allowed and the size of the resulting multidimensional table. If the table
   * is above the allowed amount of memory then value are computed when
   * demanded, having a cache for each registered instantiation to not compute
   * several times in a row the same value.
   *
   * If the memory allowed is enough to contain the bucket's table, then the
   * resulting table is computed when a value is demanded for the first time.
   *
   * Since a MultiDimArray is used as a buffer, an instantiation real master
   * will be the internal buffer of a MultiDimBucket. This is why you should
   * always call the Instantiation::isMaster() method with the reference
   * retuned by MultiDimBucket::getMasterRef().
   *
   * TODO handle slave switch between buffer and hashtable.
   *
   * @tparam GUM_SCALAR The type of scalars stored in this multidimensional
   * table.
   */

  template < typename GUM_SCALAR >
  class MultiDimBucket: public MultiDimReadOnly< GUM_SCALAR > {
    public:
    // ========================================================================
    /// @name Constructor & destructor.
    // ========================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * The default amount of memory available for a bucket is 2^16 (65536)
     * elements in the table.
     *
     * @param bufferSize The amount of memory allowed for this bucket.
     */
    explicit MultiDimBucket(Size bufferSize = INT_MAX);

    /**
     * @brief Copy constructor.
     * @param source The MultiDimBucket to copy.
     */
    MultiDimBucket(const MultiDimBucket< GUM_SCALAR >& source);

    /**
     * @brief Destructor.
     */
    virtual ~MultiDimBucket();

    /// @}
    // ========================================================================
    /// @name Bucket methods
    // ========================================================================
    /// @{

    /**
     * @brief Add a MultiDimContainer in the bucket.
     * @param impl The MultiDimContainer to copy.
     * @throw DuplicateElement Raised if impl is already in the bucket.
     */
    void add(const MultiDimContainer< GUM_SCALAR >& impl);

    /**
     * @brief Add a MultiDimContainer in the bucket.
     * @param impl The MultiDimContainer to add.
     * @throw DuplicateElement Raised if impl is already in the bucket.
     */
    void add(const MultiDimContainer< GUM_SCALAR >* impl);

    /**
     * @brief Remove a MultiDimContainer from this bucket.
     * @param impl The MultiDimContainer to remove.
     */
    void erase(const MultiDimContainer< GUM_SCALAR >& impl);

    /**
     * @brief Remove a MultiDimContainer from this bucket.
     * @param impl The MultiDimContainer to remove.
     */
    void erase(const MultiDimContainer< GUM_SCALAR >* impl);

    /**
     * @brief Returns true if the MultiDimContainer is in this bucket.
     * @param impl The impl to test for existence.
     * @return Returns true if the MultiDimContainer is in this bucket.
     */
    bool contains(const MultiDimContainer< GUM_SCALAR >& impl) const;

    /**
     * @brief Returns the sequence of all the variables contained in the
     * bucket.
     * @return Returns the sequence of all the variables contained in the
     * bucket.
     */
    const Set< const DiscreteVariable* >& allVariables() const;

    /**
     * @brief Returns the number of MultiDimContainer in in this bukcet.
     * @return Returns the number of MultiDimContainer in in this bukcet.
     */
    Size bucketSize() const;

    /**
     * @brief Returns true if this bucket is empty.
     * @return Returns true if this bucket is empty.
     */
    bool isBucketEmpty() const;

    /**
     * @brief Returns true if the bucket need re-computation since the last
     * computation.
     * @return Returns true if the bucket need re-computation since the last
     * computation.
     */
    bool bucketChanged() const;

    /**
     * @brief Returns the MultiDimArray used by this MultiDimBucket.
     * @return Returns the MultiDimArray used by this MultiDimBucket.
     * @throw OperationNotAllowed Raised if the bucket has not been built.
     */
    const MultiDimArray< GUM_SCALAR >& bucket() const;

    /**
     * @brief Returns the amount of memory allowed for this bucket.
     * @return Returns the amount of memory allowed for this bucket.
     */
    Size bufferSize() const;

    /**
     * @brief Changes the amount of memory allowed for this bucket.
     *
     * If the new amount is not enough for the current size of this bucket,
     * then internal buffer is deleted. In the other case, the internal buffer
     * is created but not computed.
     *
     * @param amount The amount of memory allowed for this bucket.
     */
    void setBufferSize(Size amount);

    /**
     * @brief This method computes the final table of this bucket.
     *
     * A flag is used to prevent unnecessary computation if the table has
     * already been computed.
     *
     * If the size of the final table is above the amount of authorized memory,
     * an OperationNotAllowed is raised.
     *
     * Remember that this method is constant because the content of a
     * multidimensional table is mutable.
     *
     * @param force If true (default set at false) then the final table is
     * re-computed.
     *
     * @throw OperationNotAllowed Raised if the size of the final table is
     * above the authorized amount of memory.
     * @throw SizeError Raised if the bucket is empty.
     */
    void compute(bool force = false) const;

    /// @}
    // ========================================================================
    /// @name Inherited methods
    // ========================================================================
    /// @{

    virtual MultiDimContainer< GUM_SCALAR >* newFactory() const override;

    const std::string& name() const override;

    virtual void add(const DiscreteVariable& v) override;

    virtual void erase(const DiscreteVariable& v) override;

    virtual Size realSize() const override;

    bool contains(const DiscreteVariable& v) const override;

    virtual GUM_SCALAR get(const Instantiation& i) const override;

    virtual void changeNotification(const Instantiation&          i,
                                    const DiscreteVariable* const var,
                                    Idx                           oldval,
                                    Idx                           newval) override;

    virtual void setFirstNotification(const Instantiation& i) override;

    virtual void setLastNotification(const Instantiation& i) override;

    virtual void setIncNotification(const Instantiation& i) override;

    virtual void setDecNotification(const Instantiation& i) override;

    virtual void setChangeNotification(const Instantiation& i) override;

    virtual bool registerSlave(Instantiation& i) override;

    virtual bool unregisterSlave(Instantiation& i) override;

    virtual MultiDimAdressable& getMasterRef() override;

    virtual const MultiDimAdressable& getMasterRef() const override;

    virtual std::string toString(const Instantiation* i) const override;

    /// @}

    /**
     * @brief Returns the MultiDimContainer and their respective Instantiation.
     * @return Returns the MultiDimContainer and their respective Instantiation.
     */
    const HashTable< const MultiDimContainer< GUM_SCALAR >*, Instantiation* >& multidims() const;

    protected:
    virtual void commitMultipleChanges_() override;

    /**
     * @warning This will raise en exception, you should directly use the get()
     * and operator[]() methods.
     */
    virtual GUM_SCALAR& get_(const Instantiation& i) const override;

    virtual void replace_(const DiscreteVariable* x, const DiscreteVariable* y) override;

    private:
    /// The number of element allowed in  _bucket_.
    Size _bufferSize_;

    /// Bijection between instantiations registered on this and
    /// their equivalent on  _bucket_
    Bijection< Instantiation*, Instantiation* > _instantiations_;

    /// The result table of this bucket.
    MultiDimArray< GUM_SCALAR >* _bucket_;

    /// The set of MultiDimContainer in this bucket.
    mutable HashTable< const MultiDimContainer< GUM_SCALAR >*, Instantiation* > _multiDims_;

    /// The set of all variables of the multidims in this bucket.
    Set< const DiscreteVariable* > _allVariables_;

    /// Instantiation over all variable in this
    mutable Instantiation _allVarsInst_;

    /**
     * @brief Add a variable to  _allVariables_, and do nothing if var is already
     * in
     * the set.
     * @param var The DiscreteVariable to add.
     */
    void _addVariable_(const DiscreteVariable* var);

    /**
     * @brief Erase a variable from  _allVariables_ if no other multidimensional
     * table
     * uses it in this bucket.
     * @param var The DiscreteVariable to remove.
     */
    void _eraseVariable_(const DiscreteVariable* var);

    /**
     * @brief Initialize the internal buffer.
     *
     * This method delete  _bucket_ after saving it's slave instantiations.
     */
    void _initializeBuffer_();

    /**
     * @brief Clean the buffer and switch it's instantiation to this bucket.
     */
    void _eraseBuffer_();

    /**
     * @brief Compute the value of the final table of this bucket given i.
     *
     * If i variables are a subset of this bucket, then the missing values are
     * supposed to be at 0.
     *
     * @param value The value to compute.
     * @throw SizeError Raised if the bucket is empty.
     */
    GUM_SCALAR _computeValue_(const Instantiation& value) const;

    /// Flag used to know if changes has occurred in the bucket since last
    /// computation.
    mutable bool _changed_;

    /// This table is used to keep the last value computed for an instantiation
    /// when the value are computed on the fly.
    mutable HashTable< const Instantiation*, GUM_SCALAR > _slavesValue_;

    /// The class name.
    std::string _name_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MultiDimBucket< double >;
#endif


} /* namespace gum */

#include <agrum/tools/multidim/implementations/multiDimBucket_tpl.h>

#endif /* GUM_MULTIDIM_BUCKET_H */
