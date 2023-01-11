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
 * @brief Headers of MultiDimSparse.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MULTI_DIM_SPARSE_H
#define GUM_MULTI_DIM_SPARSE_H

#include <agrum/agrum.h>
#include <agrum/tools/multidim/implementations/multiDimWithOffset.h>
#include <vector>

namespace gum {

  // ===========================================================================
  // ===                          GUM_MULTI_DIM_ARRAY                        ===
  // ===========================================================================
  /**
   * @class MultiDimSparse
   * @headerfile multiDimSparse.h <agrum/tools/multidim/multiDimSparse.h>
   * @ingroup multidim_group
   *
   * @brief Multidimensional matrix stored as a sparse array in memory.
   * @tparam GUM_SCALAR The type of scalars stored in the multidimensional
   * table.
   */
  template < typename GUM_SCALAR >
  class MultiDimSparse: public MultiDimWithOffset< GUM_SCALAR > {
    public:
    // =========================================================================
    /// @name Constructors / Destructors
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor: creates an empty null dimensional matrix.
     * @param default_value The default value of this MultiDimSparse.
     */
    explicit MultiDimSparse(const GUM_SCALAR& default_value);

    /**
     * @brief Copy constructor.
     *
     * The newly created matrix contains the same variables and the same values
     * as from, but no instantiation is associated to it.
     *
     * @param from The MultiDimSparse to copy.
     */
    MultiDimSparse(const MultiDimSparse< GUM_SCALAR >& from);

    /**
     * @todo operator=
     *
     * @brief Copy operator.
     * @param from The MultiDimSparse to copy.
    MultiDimSparse<GUM_SCALAR>& operator=( const MultiDimSparse<GUM_SCALAR>&
    from );
     */

    /**
     * @brief Class destructor.
     */
    virtual ~MultiDimSparse();


    /// @}
    // =========================================================================
    /// @name Inherited methods
    // =========================================================================
    /// @{

    virtual MultiDimContainer< GUM_SCALAR >* newFactory() const;

    void add(const DiscreteVariable& v);

    void erase(const DiscreteVariable& v);

    virtual void fill(const GUM_SCALAR& d) const;

    virtual GUM_SCALAR get(const Instantiation& i) const;

    virtual void set(const Instantiation& i, const GUM_SCALAR& value) const;

    virtual const std::string& name() const;

    virtual Size realSize() const;

    /// @}

    protected:
    /// The true data : the values is mutable since we can change the value
    /// in a const multiDimArray
    mutable HashTable< Size, GUM_SCALAR > params_;

    /// The default value.
    mutable GUM_SCALAR default_;

    /// Synchronise content after MultipleChanges.
    virtual void commitMultipleChanges_();

    /**
     * @warning Will raise an OperationNotAllowed as this use its own internal
     * datastructure.
     */
    virtual GUM_SCALAR& get_(const Instantiation& i) const;

    // =========================================================================
    /// @name Inherited methods
    // =========================================================================
    /// @{

    virtual void replace_(const DiscreteVariable* x, const DiscreteVariable* y);

    /// @}
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MultiDimSparse< double >;
#endif


} /* namespace gum */

#include <agrum/tools/multidim/implementations/multiDimSparse_tpl.h>

#endif /* GUM_MULTI_DIM_SPARSE_H */
