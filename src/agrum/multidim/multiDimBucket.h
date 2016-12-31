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
 * @brief Headers of the MultiDimBucket class.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 * @author Lionel TORTI
 */

#ifndef GUM_MULTIDIM_BUCKET_H
#define GUM_MULTIDIM_BUCKET_H

#include <climits>
#include <sstream>

#include <agrum/core/bijection.h>
#include <agrum/core/hashTable.h>
#include <agrum/core/sequence.h>
#include <agrum/core/set.h>

#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/multiDimReadOnly.h>

namespace gum {

  /**
   * @class MultiDimBucket
   * @headerfile multiDimBucket.h <agrum/multidim/multiDimBucket.h>
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

  template <typename GUM_SCALAR>
  class MultiDimBucket : public MultiDimReadOnly<GUM_SCALAR> {
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
    MultiDimBucket( Size bufferSize = INT_MAX );

    /**
     * @brief Copy constructor.
     * @param source The MultiDimBucket to copy.
     */
    MultiDimBucket( const MultiDimBucket<GUM_SCALAR>& source );

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
    void add( const MultiDimContainer<GUM_SCALAR>& impl );

    /**
     * @brief Add a MultiDimContainer in the bucket.
     * @param impl The MultiDimContainer to add.
     * @throw DuplicateElement Raised if impl is already in the bucket.
     */
    void add( const MultiDimContainer<GUM_SCALAR>* impl );

    /**
     * @brief Remove a MultiDimContainer from this bucket.
     * @param impl The MultiDimContainer to remove.
     */
    void erase( const MultiDimContainer<GUM_SCALAR>& impl );

    /**
     * @brief Remove a MultiDimContainer from this bucket.
     * @param impl The MultiDimContainer to remove.
     */
    void erase( const MultiDimContainer<GUM_SCALAR>* impl );

    /**
     * @brief Returns true if the MultiDimContainer is in this bucket.
     * @param impl The impl to test for existence.
     * @return Returns true if the MultiDimContainer is in this bucket.
     */
    bool contains( const MultiDimContainer<GUM_SCALAR>& impl ) const;

    /**
     * @brief Returns the sequence of all the variables contained in the
     * bucket.
     * @return Returns the sequence of all the variables contained in the
     * bucket.
     */
    const Set<const DiscreteVariable*>& allVariables() const;

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
    const MultiDimArray<GUM_SCALAR>& bucket() const;

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
    void setBufferSize( Size amount );

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
    void compute( bool force = false ) const;

    /// @}
    // ========================================================================
    /// @name Inherited methods
    // ========================================================================
    /// @{

    virtual MultiDimContainer<GUM_SCALAR>* newFactory() const;

    const std::string& name() const;

    virtual void add( const DiscreteVariable& v );

    virtual void erase( const DiscreteVariable& v );

    virtual Size realSize() const;

    bool contains( const DiscreteVariable& v ) const;

    virtual GUM_SCALAR get( const Instantiation& i ) const;

    virtual void changeNotification( Instantiation&                i,
                                     const DiscreteVariable* const var,
                                     const Idx&                    oldval,
                                     const Idx&                    newval );

    virtual void setFirstNotification( Instantiation& i );

    virtual void setLastNotification( Instantiation& i );

    virtual void setIncNotification( Instantiation& i );

    virtual void setDecNotification( Instantiation& i );

    virtual void setChangeNotification( Instantiation& i );

    virtual bool registerSlave( Instantiation& i );

    virtual bool unregisterSlave( Instantiation& i );

    virtual MultiDimAdressable& getMasterRef( void );

    virtual const MultiDimAdressable& getMasterRef( void ) const;

    virtual const std::string toString( const Instantiation* i ) const;

    /// @}

    /**
     * @brief Returns the MultiDimContainer and their respective Instantiation.
     * @return Returns the MultiDimContainer and their respective Instantiation.
     */
    const HashTable<const MultiDimContainer<GUM_SCALAR>*, Instantiation*>&
    multidims() const;

    protected:
    virtual void _commitMultipleChanges();

    /**
     * @warning This will raise en exception, you should directly use the get()
     * and operator[]() methods.
     */
    virtual GUM_SCALAR& _get( const Instantiation& i ) const;

    virtual void _swap( const DiscreteVariable* x, const DiscreteVariable* y );

    private:
    /// The number of element allowed in __bucket.
    Size __bufferSize;

    /// Bijection between instantiations registered on this and
    /// their equivalent on __bucket
    Bijection<Instantiation*, Instantiation*> __instantiations;

    /// The result table of this bucket.
    MultiDimArray<GUM_SCALAR>* __bucket;

    /// The set of MultiDimContainer in this bucket.
    mutable HashTable<const MultiDimContainer<GUM_SCALAR>*, Instantiation*>
        __multiDims;

    /// The set of all variables of the multidims in this bucket.
    Set<const DiscreteVariable*> __allVariables;

    /// Instantiation over all variable in this
    mutable Instantiation __allVarsInst;

    /**
     * @brief Add a variable to __allVariables, and do nothing if var is already
     * in
     * the set.
     * @param var The DiscreteVariable to add.
     */
    void __addVariable( const DiscreteVariable* var );

    /**
     * @brief Erase a variable from __allVariables if no other multidimensional
     * table
     * uses it in this bucket.
     * @param var The DiscreteVariable to remove.
     */
    void __eraseVariable( const DiscreteVariable* var );

    /**
     * @brief Initialize the internal buffer.
     *
     * This method delete __bucket after saving it's slave instantiations.
     */
    void __initializeBuffer();

    /**
     * @brief Clean the buffer and switch it's instantiation to this bucket.
     */
    void __eraseBuffer();

    /**
     * @brief Compute the value of the final table of this bucket given i.
     *
     * If i variables are a subset of this bucket, then the missing values are
     * supposed to be at 0.
     *
     * @param value The value to compute.
     * @throw SizeError Raised if the bucket is empty.
     */
    GUM_SCALAR __computeValue( const Instantiation& value ) const;

    /// Flag used to know if changes has occurred in the bucket since last
    /// computation.
    mutable bool __changed;

    /// This table is used to keep the last value computed for an instantiation
    /// when the value are computed on the fly.
    mutable HashTable<const Instantiation*, GUM_SCALAR> __slavesValue;

    /// The class name.
    std::string __name;
  };


  extern template class MultiDimBucket<float>;
  extern template class MultiDimBucket<double>;


} /* namespace gum */

#include <agrum/multidim/multiDimBucket_tpl.h>

#endif /* GUM_MULTIDIM_BUCKET_H */
