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
 * @brief Headers for MultiDimDecorator.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MULTI_DIM_DECORATOR_H
#define GUM_MULTI_DIM_DECORATOR_H

#include <mutex>

#include <agrum/agrum.h>

#include <agrum/base/multidim/implementations/multiDimArray.h>

namespace gum {
  class DiscreteVariable;

  // ===========================================================================
  // ===                         MULTIDIM DECORATOR                          ===
  // ===========================================================================

  /**
   * @class MultiDimDecorator
   * @headerfile multiDimDecorator.h <agrum/base/multidim/multiDimDecorator.h>
   * @ingroup multidim_group
   *
   * @brief Decorator design pattern in order to separate implementations from
   * multidimensional matrix concepts.
   *
   * A MultiDimDecorator is a virtual class for all encapsulation of
   * MultiDimImplementation, for example probability, utility, etc.  It
   * implements a decorator design pattern in order to have an array, tree,
   * sparse or matrix (...) implementation of MultiDimImplementation.
   *
   * @tparam GUM_ELEMENT The type of the scalar stored in this multidimensional
   * matrix.
   */
  template < typename GUM_ELEMENT >
  class MultiDimDecorator: public MultiDimContainer< GUM_ELEMENT > {
    public:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Class constructor.
     * @param aContent The implementation used by this MultiDimDecorator.
     */
    MultiDimDecorator(MultiDimImplementation< GUM_ELEMENT >* aContent    = nullptr,
                      GUM_ELEMENT                            empty_value = (GUM_ELEMENT)0);

    /**
     * @brief copy constructor & assignment
     */
    MultiDimDecorator(const MultiDimDecorator< GUM_ELEMENT >& from);

    /**
     * @brief copy operator
     */
    MultiDimDecorator< GUM_ELEMENT >& operator=(const MultiDimDecorator& from) noexcept;

    /**
     * @brief Class move constructor
     */
    MultiDimDecorator(MultiDimDecorator< GUM_ELEMENT >&&) noexcept;

    /**
     * @brief assignment operator.
     */
    MultiDimDecorator< GUM_ELEMENT >& operator=(MultiDimDecorator&& from);

    /**
     * @brief Class destructor.
     */
    ~MultiDimDecorator() override;

    /// @}
    // =========================================================================
    /// @name MultiDimInterface implementation
    // =========================================================================
    /// @{

    Idx nbrDim() const final;

    Size domainSize() const final;

    void add(const DiscreteVariable& v) final;

    void         erase(const DiscreteVariable& var) final;
    virtual void erase(std::string_view name) final;

    const Sequence< const DiscreteVariable* >& variablesSequence() const final;
    const DiscreteVariable&                    variable(Idx) const final;
    const DiscreteVariable&                    variable(std::string_view name) const final;

    Idx pos(const DiscreteVariable& var) const final;

    bool contains(const DiscreteVariable& var) const final;

    bool empty() const final;

    /// @}
    // ========================================================================
    /// @name MultiDimAddressable implementation
    // ========================================================================
    /// @{

    bool unregisterSlave(Instantiation& i) final;

    bool registerSlave(Instantiation& i) final;

    void changeNotification(const Instantiation&          i,
                            const DiscreteVariable* const var,
                            Idx                           oldval,
                            Idx                           newval) final;

    void setChangeNotification(const Instantiation& i) final;

    void setFirstNotification(const Instantiation& i) final;

    void setLastNotification(const Instantiation& i) final;

    void setIncNotification(const Instantiation& i) final;

    void setDecNotification(const Instantiation& i) final;

    virtual void notifyChange() const final;

    /// @}
    // =========================================================================
    /// @name MultiDimContainer implementation
    // =========================================================================
    /// @{

    /**
     * @brief Default implementation of MultiDimContainer::set().
     *
     * Calls get_ as a r-value.
     */
    void set(const Instantiation& i, const GUM_ELEMENT& value) const final;

    /**
     * @brief Default implementation of MultiDimContainer::get().
     *
     * Calls get_ as a l-value.
     */
    GUM_ELEMENT get(const Instantiation& i) const final;

    void fill(const GUM_ELEMENT& d) const final;

    /**
     * @brief Automatically fills this MultiDimContainer with the values in
     * v.
     *
     * The order used to fill this MultiDimContainer is the same as with an
     * instantiation over it.
     * @code
     * Size cpt = 0;
     * Instantiation i( *this );
     * for (i.setFirst(); !i.end(); ++i, ++cpt) {
     *   set(i, v[cpt]);
     * }
     * @endcode
     *
     * @param v Vector of values.
     * @throw SizeError Raised if v size's does not matches this
     * MultiDimContainer domain size.
     */
    void populate(const std::vector< GUM_ELEMENT >& v) const final;

    /**
     * @brief Apply a function on every element of the container
     * @param f the function to apply
     */
    void apply(std::function< GUM_ELEMENT(GUM_ELEMENT) > f) const final;

    /**
     * @brief compute lfold for this container
     * @param f the function to apply
     * @param base the initial value
     */
    GUM_ELEMENT reduce(std::function< GUM_ELEMENT(GUM_ELEMENT, GUM_ELEMENT) > f,
                       GUM_ELEMENT                                            base) const final;

    [[nodiscard]] MultiDimDecorator< GUM_ELEMENT >* newFactory() const override = 0;

    void beginMultipleChanges() final;

    void endMultipleChanges() final;

    void endMultipleChanges(const GUM_ELEMENT&) final;

    std::string toString(const Instantiation* i) const override;

    std::string toString() const override;

    /// @}
    // =========================================================================
    /// @name Various methods.
    // =========================================================================
    /// @{


    /**
     * @brief Returns the implementation for this object (may be *this).
     */
    const MultiDimImplementation< GUM_ELEMENT >* content() const final;

    /**
     * @brief Returns the implementation for this object (may be *this).
     */
    MultiDimImplementation< GUM_ELEMENT >* content() final;

    /// @}

    protected:
    void replace_(const DiscreteVariable* x, const DiscreteVariable* y) override;

    /**
     * protected method to swap the implementation behind the Tensor
     * @warning unsafe method for slave Instantiations !
     */
    void swapContent_(MultiDimImplementation< GUM_ELEMENT >* aContent) const;

    /**
     * The true container.
     */
    mutable MultiDimImplementation< GUM_ELEMENT >* content_;

    /**
     * Return a data, given a Instantiation - final method.
     * @param i The instantiation.
     * @throw NullElement
     * @throw NotFound
     */
    GUM_ELEMENT& get_(const Instantiation& i) const final;

    /**
     * value of the MultiDimDecorator if no dimension.
     *
     */
    mutable GUM_ELEMENT empty_value_;
  };

} /* namespace gum */

#include <agrum/base/multidim/implementations/multiDimDecorator_tpl.h>

#endif /* GUM_MULTI_DIM_DECORATOR_H */
