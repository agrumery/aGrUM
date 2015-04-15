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
/** @file
 * @brief class for LOGIT implementation as multiDim
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#ifndef GUM_MULTI_DIM_LOGIT_H
#define GUM_MULTI_DIM_LOGIT_H

#include <agrum/multidim/ICIModels/multiDimICIModel.h>

namespace gum {

  /** @class MultiDimLogit
   * @brief Logit representation
   * @ingroup multidim_group
   *
   * Logit instantiation of GLM (Generalized Linear Models)
   *
   * @warning
   *   - The first variable is assumed to be the class estimated by a logit. The
   *latter are
   *     the features.
   *   - This code give probabilities for BINARY VARIABLES (other values are
   *     assumed to be of probability 0). But for optimization reason, we will
   *     never check if it is the case.
   */
  /* =========================================================================== */
  template <typename GUM_SCALAR>

  class MultiDimLogit : public MultiDimICIModel<GUM_SCALAR> {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /** Default constructor.
     * default_weight is 0 for logit model.
     **/
    MultiDimLogit(GUM_SCALAR external_weight,
                  GUM_SCALAR default_weight = (GUM_SCALAR)0.0);

    ///
    MultiDimLogit(const MultiDimLogit<GUM_SCALAR> &from);

    /** Copy constructor using a bijection to swap variables from source.
    * @param bij First variables are new variables, seconds are in from.
    * @param from the copied instance
    */
    MultiDimLogit(
        const Bijection<const DiscreteVariable *, const DiscreteVariable *> &bij,
        const MultiDimLogit<GUM_SCALAR> &from);

    /// Destructor.
    virtual ~MultiDimLogit();

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

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    public:
    virtual GUM_SCALAR get(const Instantiation &i) const;

    const std::string toString(void) const;

    /// returns the real name of the multiDimArray
    /** In aGrUM, all the types of multi-dimensional arrays/functionals have a
     * name that describes what they are in reality. For instance, a table stored
     * in extension is a "MultiDimArray", one that stores only non zero elements
     * is a "MultiDimSparseArray", and so on. These names are unique for each type
     * of implementation and is used by the system to determine which is the best
     * functions to use, say, when we wish to use operators such as operator+ on
     * two MultiDimImplementations */
    virtual const std::string &name() const;

    /// @}
  };

  extern template class MultiDimLogit<float>;
  extern template class MultiDimLogit<double>;

  /// For friendly displaying the content of the array.
  template <typename GUM_SCALAR>
  std::ostream &operator<<(std::ostream &s, const MultiDimLogit<GUM_SCALAR> &ag);

} /* namespace gum */

#include <agrum/multidim/ICIModels/multiDimLogit.tcc>

#endif /* GUM_MULTI_DIM_LOGIT_H */
