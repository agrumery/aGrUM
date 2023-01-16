/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Headers of gum::PRMAggregate.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_AGGREGATE_H
#define GUM_AGGREGATE_H

#include <memory>
#include <string>


#include <agrum/PRM/elements/PRMType.h>
#include <agrum/PRM/elements/PRMScalarAttribute.h>


namespace gum {
  namespace prm {

    /**
     * @class gum::PRMAggregate
     * @headerfile aggregate.h <agrum/PRM/elements/aggregate.h>
     * @brief Defines an aggregate in a PRM.
     *
     * An PRMAggregate is defined by its type, and by the slot chain on which it
     * is applied.
     *
     * The name of an aggregate is always of the form:
     * AggregateType::type(slot_chain[, param])
     *
     * An aggregate is a member of a class.
     *
     * An aggregate in a PRM is used to summarized information about n-ary
     * relations.
     *
     * @see PRM PRMFactory Class PRMSlotChain
     * @ingroup prm_group
     */
    template < typename GUM_SCALAR >
    class PRMAggregate: public PRMClassElement< GUM_SCALAR > {
      friend class PRMClass< GUM_SCALAR >;

      public:
      // ========================================================================
      /// @name Built-in types.
      // ========================================================================
      /// @{

      /// The different type of aggregates we can have.
      enum class AggregateType : char {
        MIN,
        MAX,
        COUNT,
        EXISTS,
        FORALL,
        OR,
        AND,
        AMPLITUDE,
        MEDIAN,
        SUM
      };

      /**
       * Static method which returns the AggregateType given its string
       * representation.
       * We suppose that str is given either entirely in lower case or upper
       * case. It will also match if only the first letter is in upper case.
       *
       * @throw Raise NotFound exception if no matches is found.
       */
      static AggregateType str2enum(const std::string& str) {
        if (toLower(str) == "min") {
          return AggregateType::MIN;
        } else if (toLower(str) == "max") {
          return AggregateType::MAX;
        } else if (toLower(str) == "count") {
          return AggregateType::COUNT;
        } else if (toLower(str) == "exists") {
          return AggregateType::EXISTS;
        } else if (toLower(str) == "or") {
          return AggregateType::OR;
        } else if (toLower(str) == "and") {
          return AggregateType::AND;
        } else if (toLower(str) == "forall") {
          return AggregateType::FORALL;
        } else if (toLower(str) == "amplitude") {
          return AggregateType::AMPLITUDE;
        } else if (toLower(str) == "median") {
          return AggregateType::MEDIAN;
        } else if (toLower(str) == "sum") {
          return AggregateType::SUM;
        } else {
          std::string msg = "Unknown aggregate: ";
          msg.append(str);
          GUM_ERROR(NotFound, msg)
        }
      }

      /// @}
      // ========================================================================
      /// @name Constructor & destructor.
      // ========================================================================
      /// @{

      /**
       * Default constructor.
       * @param name The name of this aggregate.
       * @param aggType The aggregate type of this aggregate.
       * @param rvType The random variable type of this aggregate, which is
       * copied.
       */
      PRMAggregate(const std::string& name, AggregateType aggType, const PRMType& rvType);

      /**
       * Default constructor.
       * @param name The name of this aggregate.
       * @param aggType The aggregate type of this aggregate.
       * @param rvType The random variable type of this aggregate, which is
       * copied.
       * @param label The index of the label on which this aggregate applies.
       */
      PRMAggregate(const std::string& name,
                   AggregateType      aggType,
                   const PRMType&     rvType,
                   Idx                label);

      /// Destructor.
      virtual ~PRMAggregate();

      /// @}
      // ========================================================================
      /// @name Getters and Setters.
      // ========================================================================
      /// @{

      /// See gum::PRMClassElement::elt_type().
      virtual typename PRMClassElement< GUM_SCALAR >::ClassElementType elt_type() const;

      /// Returns the aggregate of *this.
      AggregateType agg_type() const;

      /**
       * Returns the label's index on which this aggregate applies.
       * @throw OperationNotAllowed Raised if the aggregate does not applies
       *                            on a label.
       */
      Idx                label() const;
      const std::string& labelValue() const;

      /**
       * @brief Returns the shared_ptr holding this Aggregate label.
       *
       * This is used for inherited Aggregates to share labels in O3PRM.
       */
      std::shared_ptr< Idx > sharedLabel() const;

      /**
       * @brief Sets the shared_ptr of this Aggregate.
       *
       * This is used for inherited aggregates to share labels in O3PRM.
       */
      void sharedLabel(std::shared_ptr< Idx > label);

      /**
       * @brief Set the aggregator's label.
       */
      void setLabel(Idx idx);
      void setLabel(const std::string& label);

      /**
       * @brief Returns true if the label is defined.
       */
      bool hasLabel() const;

      /**
       * @brief Returns true if the aggregator is decomposable.
       */
      bool isDecomposable() const;

      /// See gum::PRMClassElement::addParent_().
      virtual void addParent(const PRMClassElement< GUM_SCALAR >& elt);

      /// See gum::PRMClassElement::addChild_().
      virtual void addChild(const PRMClassElement< GUM_SCALAR >& elt);

      /// See gum::PRMClassElement::type().
      virtual PRMType& type();

      /// See gum::PRMClassElement::type().
      virtual const PRMType& type() const;

      /**
       * @brief Aggregates don't have Potential until they are instantiated as
       * PRMAttribute, so this will raise an OperationNotAllowed exception.
       * See gum::PRMClassElement::cpf().
       */
      virtual Potential< GUM_SCALAR >& cpf();

      /**
       * @brief Aggregates don't have Potential until they are instantiated as
       * PRMAttribute, so this will raise an OperationNotAllowed exception.
       * See gum::PRMClassElement::cpf().
       */
      virtual const Potential< GUM_SCALAR >& cpf() const;

      /**
       * Returns a pointer over an empty gum::MultiDimImplementation of the good
       * type for this PRMAggregate.
       *
       * This should be use when manipulating instantiations of aggregates.
       */
      MultiDimImplementation< GUM_SCALAR >* buildImpl() const;

      virtual PRMAttribute< GUM_SCALAR >* getCastDescendant() const;
      /// @}
      private:
      // ========================================================================
      /// @name Private methods.
      // ========================================================================
      /// @{

      /// Copy constructor. Don't use it.
      PRMAggregate(const PRMAggregate< GUM_SCALAR >& source);

      /// Copy operator. Don't use it.
      PRMAggregate< GUM_SCALAR >& operator=(const PRMAggregate< GUM_SCALAR >& source);

      /// @}
      // ========================================================================
      /// @name Private members.
      // ========================================================================
      /// @{

      /// The AggregateType of this aggregate.
      AggregateType _agg_type_;

      /// The random variable type of this aggregate
      /// It is deleted with the aggregate.
      PRMType* _type_;

      /// Some aggregators applies only on a given label. This attribute must
      /// have the concerned Idx. If not initialized the pointer equals 0.
      /// It is deleted with the aggregate.
      std::shared_ptr< Idx > _label_;
      // Idx*  _label_;
      std::string _label_value_;

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class PRMAggregate< double >;
#endif

  } /* namespace prm */
}   // namespace gum

#include <agrum/PRM/elements/PRMAggregate_tpl.h>

#endif /* GUM_AGGREGATE_H */
