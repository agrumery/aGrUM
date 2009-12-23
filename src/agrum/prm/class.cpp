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
namespace prm {
// ============================================================================

Class::Class(const std::string& name, Class& super):
  ClassElementContainer(name, super, false), __implements(0)
{
  GUM_CONSTRUCTOR( Class );
  typedef Property<std::pair<bool, bool>*>::onNodes::iterator FlagIterator;
  for (FlagIterator iter = super.__IOFlags.begin(); iter != super.__IOFlags.end(); ++iter) {
    __IOFlags.insert(iter.key(), new std::pair<bool, bool>(**iter));
  }
  if (super.__implements) {
    __implements = new Set<Interface*>(*(super.__implements));
  }
}

Class::Class(const std::string& name, Class& super, Interface& i):
  ClassElementContainer(name, super, false), __implements(0)
{
  GUM_CONSTRUCTOR( Class );
  typedef Property<std::pair<bool, bool>*>::onNodes::iterator FlagIterator;
  for (FlagIterator iter = super.__IOFlags.begin(); iter != super.__IOFlags.end(); ++iter) {
    __IOFlags.insert(iter.key(), new std::pair<bool, bool>(**iter));
  }
  if (super.__implements) {
    delete __implements;
    __implements = new Set<Interface*>(*(super.__implements));
  } else {
    __implements = new Set<Interface*>();
  }
  __implements->insert(&i);
}

Class::Class(const std::string& name, Class& super, Set<Interface*>& set):
  ClassElementContainer(name, super, false), __implements(new Set<Interface*>(set))
{
  GUM_CONSTRUCTOR( Class );
  typedef Property<std::pair<bool, bool>*>::onNodes::iterator FlagIterator;
  for (FlagIterator iter = super.__IOFlags.begin(); iter != super.__IOFlags.end(); ++iter) {
    __IOFlags.insert(iter.key(), new std::pair<bool, bool>(**iter));
  }
  if (super.__implements) {
    for (Set<Interface*>::iterator i = super.__implements->begin();
         i != super.__implements->end(); ++i) {
      __implements->insert(*i);
    }
  }
}

Class::~Class()
{
  GUM_DESTRUCTOR( Class );
  typedef Property<std::pair<bool, bool>*>::onNodes::iterator FlagIterator;
  for (FlagIterator iter = __IOFlags.begin(); iter != __IOFlags.end(); ++iter) {
    delete *iter;
  }
  if (__implements) {
    delete __implements;
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

bool
Class::isValid() const {
  if (__implements) {
    bool retVal = true;
    for (Set<Interface*>::iterator iter = __implements->begin();
         iter != __implements->end(); ++iter) {
      retVal = retVal or isValid(**iter);
      if (not retVal) {
        return false;
      }
    }
    return true;
  }
  return false;
}

bool
Class::isValid(const Interface& i) const {
  for (DAG::NodeIterator node = i.dag().beginNodes();
       node != i.dag().endNodes(); ++node) {
    switch (i.get(*node).elt_type()) {
      case ClassElement::prm_aggregate:
      case ClassElement::prm_attribute: {
                                          if (not get(*node).type().isSubTypeOf(i.get(*node).type())) {
                                            return false;
                                          }
                                          break;
                                        }
      case ClassElement::prm_refslot: {
                                        if (get(*node).elt_type() == ClassElement::prm_refslot) {
                                          const ReferenceSlot& ref_i = static_cast<const ReferenceSlot&>(i.get(*node));
                                          const ReferenceSlot& ref_this = static_cast<const ReferenceSlot&>(get(*node));
                                          if (not ref_this.slotType().isSubTypeOf(ref_i.slotType())) {
                                            return false;
                                          }
                                        } else {
                                          return false;
                                        }
                                        break;
                                      }
      default: {
                 GUM_ERROR(FatalError, "unexpected ClassElement in Interface.");
               }
    }
  }
  return true;
}

// ============================================================================
} /* namespace prm */
} /* namespace gum */
// ============================================================================
