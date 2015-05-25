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
 * @brief Headers of the MultiDimContainer class.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */

#ifndef GUM_MULTIDIM_CONTAINER_H
#define GUM_MULTIDIM_CONTAINER_H

#include <iostream>
#include <vector>
#include <cmath>

#include <agrum/config.h>

#include <agrum/multidim/multiDimAdressable.h>

namespace gum {
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                             GUM_MULTI_DIM                            === */
  /* ============================================================================ */
  /* ============================================================================ */
  /**
   * @class MultiDimContainer
   * @brief Abstract base class for all multi dimensionnal containers.
   * @ingroup multidim_group
   *
   * The gum::MultiDimContainer is an abstract base class for all multi
   * dimensionnal container of GUM_SCALAR. Its purpose is to deal with GUM_SCALAR
   access
   * with no regard to how the storage is done (tree, matrix...).
   *
   * MultiDimContainer is considered as const if its dimension does not change.
   * Contained values may change even in a const MultiDimContainer :
   * @code
   MultiDimArray<char> t;
   LabelizedVariable v( "v" ,"v" , 3);
   LabelizedVariable w( "w" ,"w" , 3);
   {
   t<<v;
   Instantiation I( t );I.setFirst();
   t[I]='a';
   }
   {
   MultiDimContainer<char>& notconst_t=t;
   notconst_t<<w;
   Instantiation I( notconst_t );I.setFirst();
   notconst_t[I]='b';
   }
   {
   const MultiDimContainer<char>& const_t=t;
   Instantiation I( const_t );I.setFirst();
   // const_t<<w; : ******************** DOES NOT COMPILE : YOU CANNOT DO THIS !!!!
   const_t[I]='c';
   }
   * @endcode
   */
  /* ============================================================================ */
  template <typename GUM_SCALAR>

  class MultiDimContainer : public MultiDimAdressable {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor.

    MultiDimContainer();

    /// Copy constructor.

    MultiDimContainer(const MultiDimContainer<GUM_SCALAR> &src);

    /// Destructor.

    virtual ~MultiDimContainer();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{
    public:
    /**
     * Changes the value pointed by i.
     *
     * @warning If i variables set is disjoint with this gum::MultiDimContainer
     * then 0 is assumed for dimensions (i.e. variables) not prensent in the
     * instantiation.
     */

    virtual void set(const Instantiation &i, const GUM_SCALAR &value) const;

    /**
     * Returns the value pointed by i.
     *
     * @warning If i variables set is disjoint with this gum::MultiDimContainer
     * then 0 is assumed for dimensions (i.e. variables) not prensent in the
     * instantiation.
     */

    virtual GUM_SCALAR get(const Instantiation &i) const;

    /**
     * An [] operator using a Instantiation as argument.
     *
     * @warning If i variables set is disjoint with this gum::MultiDimContainer
     * then 0 is assumed for dimensions (i.e. variables) not prensent in the
     * instantiation.
     *
     * @param i An Instantiation.
     * @return Returns the adressed (GUM_SCALAR) value.
     */

    GUM_SCALAR operator[](const Instantiation &i) const;

    /**
     * Fill the table with d.
     * @param d The value used to fill this gum::MultiDimContainer.
     */

    virtual void fill(const GUM_SCALAR &d) const = 0;

    /**
     * @brief Automatically fills this gum::MultiDimContainer with the values in v.
     *
     * The order used to fill this gum::MultiDimContainer is the same as with an
     * instantiation over it.
     * @code
     Size cpt = 0;
     Instantiation i( *this );
     for (i.setFirst(); !i.end(); ++i, ++cpt) {
       set(i, v[cpt]);
     }
     @endcode
     *
     * @param v Vector of values.
     * @throw SizeError If v size's does not matches this gum::MultiDimContainer
     *                  domain size.
     */

    virtual void fillWith(const std::vector<GUM_SCALAR> &v) const;

    /// @}

    // ############################################################################
    /// @name Copy methods.
    // ############################################################################
    /// @{
    /**
     * @brief Basic copy src a gum::MultiDimContainer.
     * This method is virtual because it should be optimized in certain
     * gum::MultiDimContainer.
     *
     * @todo specific versions for decorator and for MultiDimArray
     *
     * @param src The gum::MultiDimContainer src which values are copied.
     * @param p_i Give the order to iterate in this gum::MultiDimContainer during
     *            the copy (natural order if null).
     * @throw OperationNotAllowed Raised if src does not have the same domain size
     *                            than this gum::MultiDimContainer.
     */

    virtual void copyFrom(const MultiDimContainer<GUM_SCALAR> &src,
                          Instantiation *p_i = (Instantiation *)0) const;

    /**
     * Removes all variables in this gum::MultiDimContainer and copy the content
     * of src, variables included.
     */

    virtual void copy(const MultiDimContainer<GUM_SCALAR> &src);

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
    virtual MultiDimContainer<GUM_SCALAR> *newFactory() const = 0;

    /// @}

    // ############################################################################
    /// @name Various methods.
    // ############################################################################
    /// @{

    /// Display the content of the array.

    virtual const std::string toString() const;

    /// Display the internal representation of i.

    virtual const std::string toString(const Instantiation *i) const = 0;

    /// Test if this potential is equal to p.

    bool operator==(const MultiDimContainer<GUM_SCALAR> &p) const;

    /// Test if this potential is different of p.

    bool operator!=(const MultiDimContainer<GUM_SCALAR> &p) const;

    /// @}

    // ############################################################################
    /// @name Fast large modifications in structures.
    // ############################################################################
    /// @{

    /**
     * @brief Call this method before doing important changes in this
     *        gum::MultiDimContainer.
     * @warning Remember to call endMultipleChanges() when you finish your changes.
     */

    virtual void beginMultipleChanges(void) = 0;

    /**
     * @brief Call this method after doing important changes in this
     *        gum::MultiDimContainer.
     */

    virtual void endMultipleChanges(void) = 0;
    virtual void endMultipleChanges(const GUM_SCALAR &) = 0;

    /// @}

    protected:
    /**
     * @brief Return a data, given a Instantiation.
     *
     * Note that get allows to change a value in the container.
     * The method is still tagged as const.
     *
     * @warning If i variables set is disjoint with this gum::MultiDimContainer
     * then 0 is assumed for dimensions (i.e. variables) not prensent in the
     * instantiation.
     *
     * @param i The instantiation used to find the data.
     */

    virtual GUM_SCALAR &_get(const Instantiation &i) const = 0;
  };
} /* namespace gum */

#include <agrum/multidim/multiDimContainer.tcc>

#endif /* GUM_MULTIDIM_CONTAINER_H */
