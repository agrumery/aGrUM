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
 * @brief Headers of gum::MultiDimImplementation.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MULTI_DIM_IMPLEMENTATION_H
#define GUM_MULTI_DIM_IMPLEMENTATION_H

#include <vector>

#include <agrum/agrum.h>

#include <agrum/tools/multidim/implementations/multiDimContainer.h>

namespace gum {

  // ===========================================================================
  // ===                    GUM_MULTI_DIM_IMPLEMENTATION                     ===
  // ===========================================================================
  /**
   * @class MultiDimImplementation
   * @headerfile multiDimImplementation.h
   *<agrum/tools/multidim/multiDimImplementation.h>
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
   * @tparam GUM_SCALAR The type of the scalar stored in this multidimensional
   * matrix.
   */
  template < typename GUM_SCALAR >
  class MultiDimImplementation: public MultiDimContainer< GUM_SCALAR > {
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
    MultiDimImplementation(const MultiDimImplementation< GUM_SCALAR >& from);

    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimImplementation();

    /// @}
    // =========================================================================
    /// @name Accessors / Modifiers
    // =========================================================================
    /// @{

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
     * @return Returns the real name of the multiDim implementation
     */
    virtual const std::string& name() const = 0;

    /**
     * @brief Returns the base class name of this MultiDimImplementation.
     *
     * This method is used for chosing a proposer operator when no specialized
     * operator have been defined.
     */
    const std::string& basename() const;

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
     * This method uses domainSize() and realSize() to compute the ratio,
     * both methods are virtual and should be surcharge if a subclass has a
     * special policies about memory management.
     *
     * @warning This compression ratio is not exactly the memory compression
     * ratio. It is computed in terms of number of parameters.
     *
     * @return Returns the compression ration of the table.
     */
    float compressionRate() const;

    /// @}
    // ========================================================================
    /// @name MultiDimInterface implementation
    // ========================================================================
    /// @{
    virtual Idx nbrDim() const override;

    virtual Size domainSize() const override;

    virtual void add(const DiscreteVariable& v) override;

    virtual void erase(const DiscreteVariable& v) override;

    virtual const Sequence< const DiscreteVariable* >& variablesSequence() const override;

    const DiscreteVariable& variable(Idx i) const override;
    const DiscreteVariable& variable(const std::string& name) const override;

    virtual Idx pos(const DiscreteVariable& v) const override;

    virtual bool contains(const DiscreteVariable& v) const override;

    virtual bool empty() const override;

    /// @}
    // =========================================================================
    /// @name MultiDimAdressable implementation
    // =========================================================================
    /// @{

    virtual bool registerSlave(Instantiation& slave) override;

    virtual bool unregisterSlave(Instantiation& slave) override;

    /// @}
    // =========================================================================
    /// @name MultiDimContainer implementation
    // =========================================================================
    /// @{

    using MultiDimContainer< GUM_SCALAR >::get;

    virtual MultiDimContainer< GUM_SCALAR >* newFactory() const override = 0;

    void beginMultipleChanges() override;

    void endMultipleChanges() override;

    void endMultipleChanges(const GUM_SCALAR&) override;

    /// @}

    protected:
    // =========================================================================
    /// @name Fast large modifications in structures
    // =========================================================================
    /// @{

    /**
     * @brief Synchronize content after MultipleChanges.
     */
    virtual void commitMultipleChanges_();

    /**
     * @brief Synchronize content after MultipleChanges.
     * @param value Default value for uninitialized values.
     */
    virtual void commitMultipleChanges_(const GUM_SCALAR& value);

    /**
     * @brief Get the actual change method of this MultiDimImplementation.
     * @return Returns true if in multiple changes.
     */
    bool isInMultipleChangeMethod_() const;

    /**
     * @brief Get the actual state of *this.
     * @return Returns true if a commit is needed.
     */
    bool isCommitNeeded_() const;

    /**
     * @brief Returns a constant reference over the list of slaved
     * instantiations.
     * @return Returns a constant reference over the list of slaved
     * instantiations.
     */
    const List< Instantiation* >& slaves_() const;

    /**
     * @brief Replace variable x by y.
     *
     * Technically this should be call by any subclass overloading this method
     * to proceed with the changes in this class containers.
     *
     * @param x The first variable to swap.
     * @param y The second variable to swap.
     */
    virtual void replace_(const DiscreteVariable* x, const DiscreteVariable* y) override;

    /**
     * @brief Inverts variables at position p1 and p2
     *
     * Call this from subclass when you want to invert position of two
     * variables in the MultiDimImplementation.
     *
     * @param p1 The first position.
     * @param p2 The second position.
     */
    virtual void invert_(Idx p1, Idx p2);

    /// @}

    /**
     * @brief Returns the implementation for this object (may be *this).
     */
    virtual const MultiDimImplementation< GUM_SCALAR >* content() const final;

    /**
     * @brief Returns the implementation for this object (may be *this).
     */
    virtual MultiDimImplementation< GUM_SCALAR >* content() final;

    private:
    /// List of discrete variables (dimensions).
    Sequence< const DiscreteVariable* > _vars_;

    /// List of instantiations of the tuples (sequences) of variables.
    List< Instantiation* > _slaveInstantiations_;

    /// Used to represent in which change method this MultiDimImplementation is.
    enum class _InternalChangeMethod_ : char {
      DIRECT_CHANGE,
      MULTIPLE_CHANGE
    };

    /// Used to represent in which change state this MultiDimImplementation is.
    enum class _InternalChangeState_ : char {
      NO_CHANGE,
      NOT_COMMITTED_CHANGE
    };

    /// The current change method.
    _InternalChangeMethod_ _internalChangeMethod_;

    /// The current change state.
    _InternalChangeState_ _internalChangeState_;

    /// This MultiDimImplementation domain size.
    Size _domainSize_;

    /**
     * @brief Change the  _internalChangeState_ to NOT_COMMITTED_CHANGE.
     */
    void _setNotCommitedChange_();
  };

  /**
   * @brief For friendly displaying the content of the array.
   * @ingroup multidim_group
   */
  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream&, const MultiDimImplementation< GUM_SCALAR >&);

} /* namespace gum */

#include <agrum/tools/multidim/implementations/multiDimImplementation_tpl.h>

#endif /* GUM_MULTI_DIM_IMPLEMENTATION_H */
