/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Headers of gum::Aggregate.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_AGGREGATE_H
#define GUM_AGGREGATE_H

#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/aggregators/multiDimAggregator.h>
#include <agrum/multidim/aggregators/min.h>
#include <agrum/multidim/aggregators/max.h>
#include <agrum/multidim/aggregators/exists.h>
#include <agrum/multidim/aggregators/forall.h>

#include <agrum/PRM/elements/classElement.h>

namespace gum {
  namespace prm {

    /**
     * @class gum::Aggregate aggregate.h <agrum/PRM/elements/aggregate.h>
     * @brief Defines an aggregate in a PRM.
     *
     * An Aggregate is defined by its type, and by the slot chain on which it
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
     * @see PRM PRMFactory Class SlotChain
     * @ingroup prm_group
     */
    template <typename GUM_SCALAR>
    class Aggregate : public ClassElement<GUM_SCALAR> {
      friend class Class<GUM_SCALAR>;

      public:
      // ========================================================================
      /// @name Built-in types.
      // ========================================================================
      /// @{

      /// The different type of aggregates we can have.
      enum class AggregateType : char {
        MIN,
        MAX,
        MEAN,
        COUNT,
        EXISTS,
        FORALL,
        OR,
        AND
      };

      /**
       * Static method which returns the AggregateType given its string
       * representation.
       * We suppose that str is given either entirely in lower case or upper
       * case. It will also match if only the first letter is in upper case.
       *
       * @throw Raise NotFound exception if no matches is found.
       */
      static AggregateType str2enum(const std::string &str) {
        if (str == "min" || str == "MIN" || str == "Min") {
          return AggregateType::MIN;
        } else if (str == "max" || str == "MAX" || str == "Max") {
          return AggregateType::MAX;
        } else if (str == "mean" || str == "MEAN" || str == "Mean") {
          return AggregateType::MEAN;
        } else if (str == "count" || str == "COUNT" || str == "Count") {
          return AggregateType::COUNT;
        } else if (str == "exists" || str == "EXISTS" || str == "Exists") {
          return AggregateType::EXISTS;
        } else if (str == "or" || str == "OR" || str == "Or") {
          return AggregateType::OR;
        } else if (str == "and" || str == "AND" || str == "And") {
          return AggregateType::AND;
        } else if (str == "forall" || str == "FORALL" || str == "ForAll") {
          return AggregateType::FORALL;
        } else {
          std::string msg = "Unknown aggregate: ";
          msg.append(str);
          GUM_ERROR(NotFound, msg);
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
       * @param rvType The random variable type of this aggregate, which is copied.
       */
      Aggregate(const std::string &name, AggregateType aggType,
                const Type<GUM_SCALAR> &rvType);

      /**
       * Default constructor.
       * @param name The name of this aggregate.
       * @param aggType The aggregate type of this aggregate.
       * @param rvType The random variable type of this aggregate, which is copied.
       * @param label The index of the label on which this aggregate applies.
       */
      Aggregate(const std::string &name, AggregateType aggType,
                const Type<GUM_SCALAR> &rvType, Idx label);

      /// Destructor.
      virtual ~Aggregate();

      /// @}
      // ========================================================================
      /// @name Getters and Setters.
      // ========================================================================
      /// @{

      /// See gum::ClassElement::elt_type().
      virtual typename ClassElement<GUM_SCALAR>::ClassElementType elt_type() const;

      /// Returns the aggregate of *this.
      AggregateType agg_type() const;

      /**
       * Returns the label's index on which this aggregate applies.
       * @throw OperationNotAllowed Raised if the aggregate does not applies
       *                            on a label.
       */
      Idx label() const;

      /// See gum::ClassElement::_addParent().
      virtual void addParent(const ClassElement<GUM_SCALAR> &elt);

      /// See gum::ClassElement::_addChild().
      virtual void addChild(const ClassElement<GUM_SCALAR> &elt);

      /// See gum::ClassElement::type().
      virtual Type<GUM_SCALAR> &type();

      /// See gum::ClassElement::type().
      virtual const Type<GUM_SCALAR> &type() const;

      /**
       * @brief Aggregates don't have Potential until they are instantiated as
       * Attribute,
       *        so this will raise an OperationNotAllowed exception.
       * See gum::ClassElement::cpf().
       */
      virtual Potential<GUM_SCALAR> &cpf();

      /**
       * @brief Aggregates don't have Potential until they are instantiated as
       * Attribute,
       *        so this will raise an OperationNotAllowed exception.
       * See gum::ClassElement::cpf().
       */
      virtual const Potential<GUM_SCALAR> &cpf() const;

      /**
       * Returns a pointer over an empty gum::MultiDimImplementation of the good
       * type for this Aggregate.
       *
       * This should be use when manipulating instantiations of aggregates.
       */
      MultiDimImplementation<GUM_SCALAR> *buildImpl() const;

      /// @}
      private:
      // ========================================================================
      /// @name Private methods.
      // ========================================================================
      /// @{

      /// Copy constructor. Don't use it.
      Aggregate(const Aggregate &source);

      /// Copy operator. Don't use it.
      Aggregate &operator=(const Aggregate &source);

      /// @}
      // ========================================================================
      /// @name Private members.
      // ========================================================================
      /// @{

      /// The AggregateType of this aggregate.
      AggregateType __agg_type;

      /// The random variable type of this aggregate
      /// It is deleted with the aggregate.
      Type<GUM_SCALAR> *__type;

      /// Some aggregators applies only on a given label. This attribute must
      /// have the concerned Idx. If not initialized the pointer equals 0.
      /// It is deleted with the aggregate.
      Idx __label;

      /// @}
    };

    extern template class Aggregate<double>;
  } /* namespace prm */
} // namespace gum

#include <agrum/PRM/elements/aggregate.tcc>

#endif /* GUM_AGGREGATE_H */
