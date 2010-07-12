/**************************************************************************
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
 * @brief Inline implementation of gum::Aggregate
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
ClassElement::ClassElementType
Aggregate::elt_type() const { return prm_aggregate; }

INLINE
Aggregate::AggregateType
Aggregate::agg_type() const { return __agg_type; }

INLINE
Idx
Aggregate::label() const { return __label; }

INLINE
Type&
Aggregate::type() { return *__type; }

INLINE
const Type&
Aggregate::type() const { return *__type; }

INLINE
Potential<prm_float>&
Aggregate::cpf() { GUM_ERROR(OperationNotAllowed, "This is an aggregate."); }

INLINE
const Potential<prm_float>&
Aggregate::cpf() const { GUM_ERROR(OperationNotAllowed, "This is an aggregate."); }

INLINE
MultiDimImplementation<prm_float>*
Aggregate::buildImpl() const
{
  MultiDimImplementation<prm_float>* impl = 0;
  switch (agg_type()) {
    case agg_min:
      impl = new aggregator::Min<prm_float>();
      break;
    case agg_max:
      impl = new aggregator::Max<prm_float>();
      break;
    //case agg_count:
    //  if (__label < __type.variable().domainSize()) {
    //    impl = new aggregator::Count<prm_float>(__label);
    //  } else {
    //    std::string msg = "This aggregator of type agg_count isn't initialized "
    //      "properly.";
    //    GUM_ERROR(FatalError, msg);
    //  }
    //  break;
    case agg_exists:
      impl = new aggregator::Exists<prm_float>(__label);
      break;
    case agg_forall:
      impl = new aggregator::Forall<prm_float>(__label);
      break;
    case agg_count:
    case agg_mean:
    case agg_or:
    case agg_and:
      GUM_ERROR(OperationNotAllowed,"Aggregator not implemented yet.");
      break;
    default:
      GUM_ERROR(OperationNotAllowed, "Unknown aggregator.");
  }
  return impl;
}

// See gum::ClassElement::_addParent().
INLINE
void
Aggregate::addParent(const ClassElement& elt) { }

// See gum::ClassElement::_addChild().
INLINE
void
Aggregate::addChild(const ClassElement& elt) { }

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
