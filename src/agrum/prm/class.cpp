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
 * @brief Headers of gum::Class
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/class.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/class.inl>
#endif //GUM_NO_INLINE
// ============================================================================
namespace gum {
// ============================================================================

Class::Class(const std::string& name):
  ClassElementContainer(name)
{
  GUM_CONSTRUCTOR( Class );
}

Class::Class(const std::string& name, Class& mother):
  ClassElementContainer(name, mother, false)
{
  GUM_CONSTRUCTOR( Class );
  typedef Property<std::pair<bool, bool>*>::onNodes::iterator FlagIterator;
  for (FlagIterator iter = mother.__IOFlags.begin(); iter != mother.__IOFlags.end(); ++iter) {
    __IOFlags.insert(iter.key(), new std::pair<bool, bool>(**iter));
  }
}

Class::~Class()
{
  GUM_DESTRUCTOR( Class );
  typedef Property<std::pair<bool, bool>*>::onNodes::iterator FlagIterator;
  for (FlagIterator iter = __IOFlags.begin(); iter != __IOFlags.end(); ++iter) {
    delete *iter;
  }
}

Class::Class(const Class& source):
  ClassElementContainer(source)
{
  GUM_CONS_CPY( Class );
  GUM_ERROR(FatalError, "illegal call of gum::Class copy constructor.");
}

Class&
Class::operator=(const Class& source)
{
  GUM_ERROR(FatalError, "illegal call of gum::Class copy operator.");
}

void
Class::buildInstantiationSequence() const {
  // We clear this to prevent duplicate elements if this is called multiple times
  __instantiations.clear();
  std::list<NodeId> l;
  for (DAG::NodeIterator node = dag().beginNodes(); node != dag().endNodes(); ++node) {
    if (parents(*node).empty()) {
      l.push_back(*node);
    }
  }
  while (not l.empty()) {
    switch (get(l.front()).elt_type()) {
      case ClassElement::prm_aggregate:
      case ClassElement::prm_attribute: {
                                          if (not isInnerNode(l.front())) {
                                            __instantiations.insert(l.front());
                                          }
                                        }
      default: {
                 for (DAG::ArcIterator child = children(l.front()).begin(); child != children(l.front()).end(); ++child) {
                   l.push_back(child->head());
                 }
               }
    }
    l.pop_front();
  }
}

void
Class::__overload(Attribute* attr, ClassElement& elt) {
  try {
    if (attr->type() <= elt.type()) {
      _add(attr, elt.id(), true);
    } else {
      GUM_ERROR(OperationNotAllowed, "Invalid overload type.");
    }
  } catch (OperationNotAllowed&) {
    GUM_ERROR(OperationNotAllowed, "Invalid overload element.");
  }
}

void
Class::__addSuperType(Attribute* attr) {
  try {
    Attribute* previous = attr;
    const Type* super = 0;
    while (true) {
      super = &(previous->type().super());
      std::stringstream sBuff;
      sBuff << "<" << super->name() << ">" << attr->name();
      Attribute* current
        = new Attribute(sBuff.str(), *super, attr->type().cast_CPT());
      _add(current);
      insertArc(previous->name(), current->name());
      previous = current;
    }
  } catch (NotFound&) {
    // No or no more super types
  }
}

// ============================================================================
} /* namespace gum */
// ============================================================================
