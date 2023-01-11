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
 * @brief Headers of the MultiDimContainer class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_MULTIDIM_CONTAINER_H
#define GUM_MULTIDIM_CONTAINER_H

#include <functional>
#include <initializer_list>
#include <iostream>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/core/math/math_utils.h>

#include <agrum/tools/multidim/implementations/multiDimAdressable.h>

namespace gum {
  // needed for content()
  template < typename GUM_SCALAR >
  class MultiDimImplementation;

  // ==========================================================================
  // ===                             GUM_MULTI_DIM                          ===
  // ==========================================================================
  /**
   * @class MultiDimContainer
   * @headerfile multiDimContainer.h <agrum/tools/multidim/multiDimContainer.h>
   * @ingroup multidim_group
   *
   * @brief Abstract base class for all multi dimensionnal containers.
   *
   * The MultiDimContainer is an abstract base class for all multi
   * dimensionnal container of GUM_SCALAR. Its purpose is to deal with
   * GUM_SCALAR access with no regard to how the storage is done (tree,
   * matrix...).
   *
   * MultiDimContainer is considered as const if its dimension does not change.
   * Contained values may change even in a const MultiDimContainer :
   * @code
   * MultiDimArray<char> t;
   * LabelizedVariable v( "v" ,"v" , 3);
   * LabelizedVariable w( "w" ,"w" , 3);
   * {
   * t<<v;
   * Instantiation I( t );I.setFirst();
   * t[I]='a';
   * }
   * {
   * MultiDimContainer<char>& notconst_t=t;
   * notconst_t<<w;
   * Instantiation I( notconst_t );I.setFirst();
   * notconst_t[I]='b';
   * }
   * {
   * const MultiDimContainer<char>& const_t=t;
   * Instantiation I( const_t );I.setFirst();
   * // const_t<<w; : ******************** DOES NOT COMPILE : YOU CANNOT DO THIS
   * !!!!
   * const_t[I]='c';
   * }
   * @endcode
   *
   * @tparam GUM_SCALAR The type of the scalar stored in this multidimensional
   * matrix.
   */
  template < typename GUM_SCALAR >
  class MultiDimContainer: public MultiDimAdressable {
    public:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MultiDimContainer();

    /**
     * @brief Copy constructor.
     * @param src The MultiDimContainer to copy.
     */
    MultiDimContainer(const MultiDimContainer< GUM_SCALAR >& src);

    MultiDimContainer& operator=(const MultiDimContainer< GUM_SCALAR >& src);

    /**
     * @brief Class move constructor.
     */
    MultiDimContainer(MultiDimContainer< GUM_SCALAR >&&);

    MultiDimContainer& operator=(MultiDimContainer< GUM_SCALAR >&& src);

    /**
     * @brief Destructor.
     */
    virtual ~MultiDimContainer();

    /// @}
    // =========================================================================
    /// @name Accessors / Modifiers
    // =========================================================================
    /// @{
    /**
     * @brief Changes the value pointed by i.
     *
     * @warning If i variables set is disjoint with this MultiDimContainer
     * then 0 is assumed for dimensions (i.e. variables) not prensent in the
     * instantiation.
     *
     * @param i An Instantiation of this MultiDimContainer.
     * @param value The new value.
     */
    virtual void set(const Instantiation& i, const GUM_SCALAR& value) const;

    /**
     * @brief Returns the value pointed by i.
     *
     * @warning If i variables set is disjoint with this MultiDimContainer
     * then 0 is assumed for dimensions (i.e. variables) not present in the
     * instantiation.
     *
     * @param i An Instantiation of this MultiDimContainer.
     * @return Returns the value pointe by i.
     */
    virtual GUM_SCALAR get(const Instantiation& i) const;

    /**
     * @brief An [] operator using a Instantiation as argument.
     *
     * @warning If i variables set is disjoint with this MultiDimContainer
     * then 0 is assumed for dimensions (i.e. variables) not prensent in the
     * instantiation.
     *
     * @param i An Instantiation.
     * @return Returns the adressed (GUM_SCALAR) value.
     */
    GUM_SCALAR operator[](const Instantiation& i) const;

    /**
     * @brief Fill the table with d.
     * @param d The value used to fill this MultiDimContainer.
     */
    virtual void fill(const GUM_SCALAR& d) const = 0;

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
    virtual void populate(const std::vector< GUM_SCALAR >& v) const;

    /**
     * @brief Automatically fills this MultiDimContainer with the values in
     * l.
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
     * @param l contains the data.
     * @throw SizeError Raised if l size's does not matches this
     * MultiDimContainer domain size.
     */
    virtual void populate(std::initializer_list< GUM_SCALAR > l) const;

    /// @}
    // =========================================================================
    /// @name Copy methods.
    // =========================================================================
    /// @{

    /**
     * @brief Basic copy of a MultiDimContainer.
     *
     * This method is virtual because it should be optimized in certain
     * MultiDimContainer.
     *
     * @param src The MultiDimContainer src which values are copied. This is a
     * full copy with no verification of dimensions.
     *
     * @throw OperationNotAllowed Raised if src does not have the same domain
     * size than this MultiDimContainer.
     */
    virtual void copyFrom(const MultiDimContainer< GUM_SCALAR >& src) const;

    /**
     * @brief Basic copy of a MultiDimContainer.
     *
     * This method is virtual because it should be optimized in certain
     * MultiDimContainer.
     *
     * @param src The MultiDimContainer src which values are copied.
     * @param p_i Give the order to iterate in this MultiDimContainer during
     * the copy (nullptr will correctly copy if this is a reorganization of
     *src).
     *
     * @throw OperationNotAllowed Raised if src does not have the same domain
     * size than this MultiDimContainer.
     */
    virtual void copyFrom(const MultiDimContainer< GUM_SCALAR >& src, Instantiation* p_i) const;

    /**
     * @brief Basic extraction of a MultiDimContainer.
     *
     * This method is virtual because it should be optimized in certain
     * MultiDimContainer.
     *
     * @param src The MultiDimContainer src which datas are copied.
     * @param mask partial instantiation of variables of the Potential : the
     *
     * extraction will concern every variable not in the instantiation and the
     * copy of data will use the (relevant) values in this instantiation.
     */
    virtual void extractFrom(const MultiDimContainer< GUM_SCALAR >& src, const Instantiation& mask);

    /**
     * @brief Returns the implementation for this object (may be *this).
     */
    virtual const MultiDimImplementation< GUM_SCALAR >* content() const = 0;

    /**
     * @brief Returns the implementation for this object (may be *this).
     */
    virtual MultiDimImplementation< GUM_SCALAR >* content() = 0;

    /**
     * @brief In order to insure the dereference for decorators, we need to
     * virtualize the access to master pointer.
     * @return Returns the ref to content as MultiDimAdressable&
     */
    virtual MultiDimAdressable& getMasterRef();

    /**
     * @brief In order to insure the dereference for decorators, we need to
     * virtualize the access to master pointer.
     * @return Returns the master of this MultiDimAdressable.
     */
    virtual const MultiDimAdressable& getMasterRef() const;


    /**
     * @brief Removes all variables in this MultiDimContainer and copy the
     * content of src, variables included.
     *
     * @param src The MultiDimContainer to copy.
     */
    virtual void copy(const MultiDimContainer< GUM_SCALAR >& src);

    /**
     * @brief Creates an empty clone of this MultiDimContainer.
     *
     * This method creates a clone of this object, withouth its content
     * (including variable), you must use this method if you want to ensure
     * that the generated object has the same type than the object containing
     * the called newFactory()
     * For example :
     * @code
     *   MultiDimArray<double> y;
     *   MultiDimContainer<double>* x = y.newFactory();
     * @endcode
     * Then x is a MultiDimArray<double>*.
     *
     * @warning You must free by yourself the returned pointer.
     *
     * @return Returns an empty clone of this object with the same type.
     */
    virtual MultiDimContainer< GUM_SCALAR >* newFactory() const = 0;

    /// @}
    // =========================================================================
    /// @name Various methods.
    // =========================================================================
    /// @{

    /**
     * @brief Returns a representation of this MultiDimContainer.
     * @return Returns a representation of this MultiDimContainer.
     */
    virtual std::string toString() const;

    /**
     * @brief Display the internal representation of i.
     * @return Returns an internal representation of i.
     */
    virtual std::string toString(const Instantiation* i) const = 0;

    /**
     * @brief Test if this MultiDimContainer is equal to p.
     * @param p The MultiDimContainer to test for equality.
     * @return Returns true if this MultiDimContainer is equal to p.
     */
    bool operator==(const MultiDimContainer< GUM_SCALAR >& p) const;

    /**
     * @brief Test if this MultiDimContainer is different of p.
     * @param p The MultiDimContainer to test for inequality.
     * @return Returns true if this MultiDimContainer is different of p.
     */
    bool operator!=(const MultiDimContainer< GUM_SCALAR >& p) const;

    /**
     * @brief Apply a function on every element of the container
     * @param f the function to apply
     */
    virtual void apply(std::function< GUM_SCALAR(GUM_SCALAR) > f) const;

    /**
     * @brief compute lfold for this container
     * @param f the function to apply
     * @param base the initial value
     */
    virtual GUM_SCALAR reduce(std::function< GUM_SCALAR(GUM_SCALAR, GUM_SCALAR) > f,
                              GUM_SCALAR                                          base) const;


    /// @}
    // =========================================================================
    /// @name Fast large modifications in structures.
    // =========================================================================
    /// @{

    /**
     * @brief Call this method before doing important changes in this
     * MultiDimContainer.
     *
     * @warning Remember to call endMultipleChanges() when you finish your
     * changes.
     */
    virtual void beginMultipleChanges() = 0;

    /**
     * @brief Call this method after doing important changes in this
     * MultiDimContainer.
     */
    virtual void endMultipleChanges() = 0;

    /**
     * @brief Call this method after doing important changes in this
     * MultiDimContainer.
     */
    virtual void endMultipleChanges(const GUM_SCALAR& v) = 0;

    /// @}

    protected:
    /**
     * @brief Return a data, given a Instantiation.
     *
     * Note that get allows to change a value in the container.
     * The method is tagged as const since a multidim is not const if its dimension
     * changed.
     *
     * @warning If i variables set is disjoint with this MultiDimContainer
     * then 0 is assumed for dimensions (i.e. variables) not present in the
     * instantiation.
     *
     * @param i The instantiation used to find the data.
     */
    virtual GUM_SCALAR& get_(const Instantiation& i) const = 0;
  };
} /* namespace gum */

#include <agrum/tools/multidim/implementations/multiDimContainer_tpl.h>

#endif /* GUM_MULTIDIM_CONTAINER_H */
