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
 * @brief Headers of gum::MultiDimImplementation.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
#ifndef GUM_MULTI_DIM_IMPLEMENTATION_H
#define GUM_MULTI_DIM_IMPLEMENTATION_H

#include <vector>

#include <agrum/config.h>

#include <agrum/multidim/multiDimContainer.h>

namespace gum {

  // ===========================================================================
  // ===                    GUM_MULTI_DIM_IMPLEMENTATION                     ===
  // ===========================================================================
  /**
   * @class MultiDimImplementation
   * @headerfile multiDimImplementation.h <agrum/multidim/multiDimImplementation.h>
   * @ingroup multidim_group
   *
   * @brief Abstract base class for all multi dimensionnal implementations
   *
   * The gum::MultiDimImplementation is an abstract class for all
   * multidimensional implementation of container of GUM_SCALAR. Its purpose is
   * to implement base algorithms with no regard to how the storage is done
   * (tree, matrix...)
   *
   * It deals also with variables and implements pure virtual methods concerned
   * with dimensions.
   *
   * See operator<<(std::ostream&, const MultiDimImplementation<GUM_SCALAR>&)
   * to print a gum::MultiDimImplementation.
   *
   * @tparam GUM_SCALAR The type of the values stored in the
   * MultiDimImplementation.
   */
  template <typename GUM_SCALAR>
  class MultiDimImplementation : public MultiDimContainer<GUM_SCALAR> {
    public:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MultiDimImplementation();

    /**
     * @brief Copy constructor.
     * @param from The MultiDimImplementation to copy.
     */
    MultiDimImplementation( const MultiDimImplementation<GUM_SCALAR>& from );

    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimImplementation();

    /// @}
    // =========================================================================
    /// @name Accessors / Modifiers
    // =========================================================================
    /// @{

    /// Using MultiDimContainer::get(const Instantiation&).
    using MultiDimContainer<GUM_SCALAR>::get;

    /// @see gum::MultiDimInterface::variablesSequence()
    virtual const Sequence<const DiscreteVariable*>& variablesSequence( void ) const;

    /**
     * @brief Returns the real name of the multiDim implementation
     *
     * In aGrUM, all the types of multi-dimensional arrays/functionals have a
     * name that describes what they are in reality. For instance, a table
     * stored in extension is a "MultiDimArray", one that stores only non zero
     * elements is a "MultiDimSparseArray", and so on. These names are unique
     * for each type of implementation and is used by the system to determine
     * which is the best functions to use, say, when we wish to use operators
     * such as operator+ on two MultiDimImplementations.
     *
     * @Return Returns the real name of the multiDim implementation
     */
    virtual const std::string& name() const = 0;

    /**
     * @brief Returns the base class name of this MultiDimImplementation.
     *
     * This method is used for chosing a proposer operator when no specialized
     * operator have been defined.
     */
    const std::string& basename() const;

    /// @}
    // ========================================================================
    /// @name Implementation of MultiDimInterface
    // ========================================================================
    /// @{

    /// @see gum::MultiDimInterface::add(const DiscreteVariable& v)
    virtual void add( const DiscreteVariable& v );

    /// @see gum::MultiDimInterface::erase(const DiscreteVariable& v)
    virtual void erase( const DiscreteVariable& v );

    /// @see gum::MultiDimInterface::nbrDim()
    virtual Idx nbrDim() const;

    /// @see gum::MultiDimInterface::domainSize()
    virtual Size domainSize() const;

    /**
     * @brief Returns the real number of parameters used for this table.
     *
     * This function is used by the MultiDimImplementation::compressionRate()
     * method.
     *
     * @see MultiDimImplementation::compressionRate()
     *
     * @return Returns the real number of parameters used for this table.
     */
    virtual Size realSize() const = 0;

    /**
     * @brief The compression ratio of the table (depending on the type of
     * implementation).
     *
     * This method uses domainSize() and realSize() to compute the ration,
     * both methods are virtual and should be surcharge if a subclass has a
     * special policies about memory management.
     *
     * @warning This compression ratio is not exactly the memory compression
     * ratio. It is computed in terms of number of parameters.
     *
     * @return Returns the compression ration of the table.
     */
    float compressionRate() const;

    /// @see gum::MultiDimInterface::variable(Idx i)
    const DiscreteVariable& variable( Idx i ) const;

    /// @see gum::MultiDimInterface::pos( const DiscreteVariable& )
    Idx pos( const DiscreteVariable& v ) const;

    /// @see gum::MultiDimInterface::contains(const DiscreteVariable& v)
    bool contains( const DiscreteVariable& v ) const;

    /// @see gum::MultiDimInterface::clear()
    bool empty() const;

    /// @}
    // =========================================================================
    /// @name Slave management and extension due to slave management
    // =========================================================================
    /// @{

    /// @see MultiDimAdressable::getMasterRef()
    virtual MultiDimAdressable& getMasterRef( void );

    /// @see MultiDimAdressable::getMasterRef() const
    virtual const MultiDimAdressable& getMasterRef( void ) const;

    /// @see MultiDimAdressable::registerSlave( Instantiation& )
    virtual bool registerSlave( Instantiation& i );

    /// @see MultiDimArray::unregisterSlave( Instantiation& )
    virtual bool unregisterSlave( Instantiation& i );

    /// @}
    // =========================================================================
    /// @name Fast large modifications in structures
    // =========================================================================
    /// @{

    /// @see gum::MultiDimContainer::beginMultipleChanges()
    virtual void beginMultipleChanges( void );

    /// @see gum::MultiDimContainer::endMultipleChanges()
    virtual void endMultipleChanges( void );

    /// @see gum::MultiDimContainer::endMultipleChanges(const GUM_SCALAR& )
    virtual void endMultipleChanges( const GUM_SCALAR& );

    /// @}
    // =========================================================================
    /// @name MultiDimContainer implementation
    // =========================================================================
    /// @{

    /// @see MultiDimContainer::newFactory()
    virtual MultiDimContainer<GUM_SCALAR>* newFactory() const = 0;

    /// @}

    protected:
    // =========================================================================
    /// @name Fast large modifications in structures
    // =========================================================================
    /// @{

    /**
     * @brief Synchronize content after MultipleChanges.
     */
    virtual void _commitMultipleChanges( void );

    /**
     * @brief Synchronize content after MultipleChanges.
     * @param value Default value for uninitialized values.
     */
    virtual void _commitMultipleChanges( const GUM_SCALAR& value );

    /**
     * @brief Get the actual change method of this MultiDimImplementation.
     * @return Returns true if in multiple changes.
     */
    bool _isInMultipleChangeMethod() const;

    /**
     * @brief Get the actual state of *this.
     * @return Returns true if a commit is needed.
     */
    bool _isCommitNeeded() const;

    /**
     * @brief Returns a constant reference over the list of slaved
     * instantiations.
     * @return Returns a constant reference over the list of slaved
     * instantiations.
     */
    const List<Instantiation*>& _slaves() const;

    /**
     * @brief Replace variable x by y.
     *
     * Technically this should be call by any subclass overloading this method
     * to proceed with the changes in this class containers.
     *
     * @param x The first variable to swap.
     * @param y The second variable to swap.
     */
    virtual void _swap( const DiscreteVariable* x, const DiscreteVariable* y );

    /**
     * @brief Inverts variables at position p1 and p2
     *
     * Call this from subclass when you want to invert position of two
     * variables in the MultiDimImplementation.
     *
     * @param p1 The first position.
     * @param p2 The second position.
     */
    virtual void _invert( Idx p1, Idx p2 );

    /// @}

    private:
    /// List of discrete variables (dimensions).
    Sequence<const DiscreteVariable*> __vars;

    /// List of instantiations of the tuples (sequences) of variables.
    List<Instantiation*> __slaveInstantiations;

    /// Used to represent in wich change method this MultiDimImplementation is.
    enum class __InternalChangeMethod : char { DIRECT_CHANGE, MULTIPLE_CHANGE };

    /// Used to represent in wich change state this MultiDimImplementation is.
    enum class __InternalChangeState : char { NO_CHANGE, NOT_COMMITTED_CHANGE };

    /// The current change method.
    __InternalChangeMethod __internalChangeMethod;

    /// The current change state.
    __InternalChangeState __internalChangeState;

    /// This MultiDimImplementation domain size.
    Size __domainSize;

    /**
     * @brief Change the __internalChangeState to NOT_COMMITTED_CHANGE.
     */
    void __setNotCommitedChange();
  };

  /**
   * @brief For friendly displaying the content of the array.
   * @ingroup multidim_group
   */
  template <typename GUM_SCALAR>
  std::ostream& operator<<( std::ostream&, const MultiDimImplementation<GUM_SCALAR>& );

} /* namespace gum */

#include <agrum/multidim/multiDimImplementation.tcc>

#endif /* GUM_MULTI_DIM_IMPLEMENTATION_H */
