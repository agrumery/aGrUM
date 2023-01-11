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
 * @brief MultiDimAggregator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_MULTI_DIM_AGGREGATOR_H
#define GUM_MULTI_DIM_AGGREGATOR_H

#include <agrum/tools/multidim/implementations/multiDimReadOnly.h>

namespace gum {
  namespace aggregator {
    // =========================================================================
    // ===                       GUM_MULTI_DIM_AGGREGATOR                    ===
    // =========================================================================

    /**
     * @class MultiDimAggregator
     * @headerfile multiDimAggregator.h
     * <agrum/tools/multidim/aggregators/multiDimAggregator.h>
     * @ingroup multidim_agg_group
     *
     * @brief Abstract base class for all multi dimensionnal aggregator.
     *
     * The principle of a deterministic aggregator is that
     * \f$P[i,J_1,\cdots,J_n]=1 \iff
     * i==f(J_1,f(J_2,\cdots,f(J_n,NeutraElement)\cdots))\f$ and 0 otherwise.
     * f is a virtual function called the folder function (like folder_left or
     * folder_right in OCaml).  NeutralElement is an Idx
     *
     * @warning
     *  - the way aggregators are implemented assumed that the FIRST variable
     *  in the multiDim is the aggregator variable.
     *  - the way aggregators are implemented does not check types and domain
     *  size (e.g domain(folder function)==domain(aggregator variable)).
     *  However, \f$f(J_1,f(J_2,\cdots,f(J_n,NeutraElement)\cdots))\f$ is
     *  truncated in order to fit in domain(aggregator variable).
     */
    template < typename GUM_SCALAR >
    class MultiDimAggregator: public MultiDimReadOnly< GUM_SCALAR > {
      public:
      // =======================================================================
      /// @name Constructors / Destructors
      // =======================================================================
      /// @{

      /**
       * Default constructor.
       */
      MultiDimAggregator();

      /**
       * Copy constructor.
       */
      MultiDimAggregator(const MultiDimAggregator< GUM_SCALAR >& from);

      /**
       * Class destructor.
       */
      virtual ~MultiDimAggregator();

      /// @}

      /**
       * This method creates a clone of this object, withouth its content
       * (including variable), you must use this method if you want to ensure
       * that the generated object has the same type than the object containing
       * the called newFactory()
       *
       * For example :
       * @code
       *   MultiDimArray<double> y;
       *   MultiDimContainer<double>* x = y.newFactory();
       * @endcode
       *
       * Then x is a MultiDimArray<double>*
       *
       * @warning you must desallocate by yourself the memory
       * @return an empty clone of this object with the same type
       */
      virtual MultiDimContainer< GUM_SCALAR >* newFactory() const override = 0;

      // =======================================================================
      /// @name Accessors / Modifiers
      // =======================================================================
      /// @{

      public:
      virtual GUM_SCALAR get(const Instantiation& i) const override;

      virtual std::string aggregatorName() const = 0;
      std::string         toString() const override;

      void changeNotification(const gum::Instantiation&,
                              const gum::DiscreteVariable* const,
                              gum::Idx,
                              gum::Idx) override{};

      void setFirstNotification(const gum::Instantiation&) override{};

      void setLastNotification(const gum::Instantiation&) override{};

      void setIncNotification(const gum::Instantiation&) override{};

      void setDecNotification(const gum::Instantiation&) override{};

      void setChangeNotification(const gum::Instantiation&) override{};

      std::string toString(const gum::Instantiation* i) const override { return i->toString(); };

      /**
       * @return the real number of parameters used for this table.
       *
       * This function is used for compute @see compressionRatio()
       */
      Size realSize() const override { return 0; };

      /**
       * @brief Returns the real name of the multiDimArray.
       *
       * In aGrUM, all the types of multi-dimensional arrays/functionals have a
       * name that describes what they are in reality. For instance, a table
       * stored in extension is a "MultiDimArray", one that stores only non
       * zero elements is a "MultiDimSparseArray", and so on. These names are
       * unique for each type of implementation and is used by the system to
       * determine which is the best functions to use, say, when we wish to use
       * operators such as operator+ on two MultiDimImplementations.
       */
      const std::string& name() const override;

      /**
       * @brief Copy of a multiDimICIModel.
       *
       * This method is virtual because it should be optimized in certain
       * MultiDimContainer.
       *
       * @throw OperationNotAllowed Raised if src does not have the same domain
       * size than this MultiDimContainer.
       **/
      void copyFrom(const MultiDimContainer< GUM_SCALAR >& src) const override;

      /**
       * @return true if the aggregator is decomposable.
       */
      bool isDecomposable() const;

      /// @}
      protected:
      /// by default, buildValue_ uses a "fold" scheme and the user has to
      /// implement neutralElt_ and fold_
      /// but if necessary (as for @ref Median), buildValue_ can be
      /// reimplemented.
      virtual Idx buildValue_(const gum::Instantiation& i) const;

      /// decomposable_ indicates if the aggregator can be decomposed
      bool decomposable_;

      /// neutralElt_() is the result value for the first application of fold_
      virtual Idx neutralElt_() const = 0;

      /// fold_ is applied on value i1 for variable v. the actual result for
      /// precedent applications is i2.
      /// @return the new result for applications up to v.
      virtual Idx fold_(const DiscreteVariable& v, Idx i1, Idx i2, bool& stop_iteration) const = 0;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class MultiDimAggregator< double >;
#endif

    /// For friendly displaying the content of the array.
    template < typename GUM_SCALAR >
    std::ostream& operator<<(std::ostream& s, const MultiDimAggregator< GUM_SCALAR >& ag);

  } /* namespace aggregator */
} /* namespace gum */

#include <agrum/tools/multidim/aggregators/multiDimAggregator_tpl.h>

#endif /* GUM_MULTI_DIM_AGGREGATOR_H */
