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
 * @brief Implementation of System.
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/system.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/system.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

System::System(const std::string& name):
  PRMObject(name)
{
  GUM_CONSTRUCTOR( System );
}

System::~System()
{
  GUM_DESTRUCTOR( System );
  for (System::iterator iter = begin(); iter != end(); ++iter) {
    delete *iter;
  }
  typedef HashTable<Class*, Set<Instance*>*>::iterator TypeIterator;
  for (TypeIterator iter = __instanceMap.begin();
       iter != __instanceMap.end(); ++iter) {
    delete *iter;
  }
  typedef HashTable< std::string, System::model_pair>::iterator ArIterator;
  for (ArIterator iter = __arrayMap.begin(); iter != __arrayMap.end(); ++iter) {
    delete iter->second;
  }
}

void
System::insertArc(const std::string& u_name, const std::string& v_name,
                  const std::string& ref_name)
{
  Instance* u = 0;
  Instance* v = 0;
  ReferenceSlot* ref = 0;
  try {
    u = __nameMap[u_name];
    v = __nameMap[v_name];
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no instance matching the given names");
  }
  try {
    ClassElement& elt = u->type().get(ref_name);
    if (elt.elt_type() == ClassElement::prm_refslot) {
      ref = static_cast<ReferenceSlot*>(&elt);
    } else {
      GUM_ERROR(WrongClassElement, "the given name does not match a ReferenceSlot in u's type");
    }
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "found no ClassElement matching the given name");
  }
  u->add(ref->id(), *v);
  __skeleton.insertArc(__nodeIdMap.keyByVal(u), __nodeIdMap.keyByVal(v));
}

NodeId
System::add(Instance* i) {
  if (__nameMap.exists(i->name())) {
    GUM_ERROR(DuplicateElement, "an Instance with the same is already in this System");
  }
  NodeId id = __skeleton.insertNode();
  __nodeIdMap.insert(id, i);
  __nameMap.insert(i->name(), i);
  try {
    __instanceMap[&(i->type())]->insert(i);
  } catch (NotFound&) {
    __instanceMap.insert(&(i->type()), new Set<Instance*>());
    __instanceMap[&(i->type())]->insert(i);
  }
  return id;
}

void
System::groundedBN(BayesNetFactory<prm_float>& factory) const
{
  factory.startNetworkDeclaration();
  factory.addNetworkProperty("name", name());
  factory.endNetworkDeclaration();
  // Adding nodes
  for (System::const_iterator iter = begin(); iter != end(); ++iter) {
    __groundAttr(**iter, factory);
  }
  // Adding arcs and filling CPTs
  for (System::const_iterator iter = begin(); iter != end(); ++iter) {
    __groundRef(**iter, factory);
  }
}

void
System::__groundAttr(const Instance& instance, BayesNetFactory<prm_float>& factory) const
{
  const DAG& dag = instance.type().dag();
  for (DAG::NodeIterator node = dag.beginNodes(); node != dag.endNodes(); ++node) {
    // Working a Class level because Aggregate are instantiated as Attribute in an Instance
    switch (instance.type().get(*node).elt_type()) {
      case ClassElement::prm_attribute:
        {
          // TODO: make a special case for noisy-or
          std::stringstream elt_name;
          elt_name << instance.name() << "." << instance.type().get(*node).safeName();
          DiscreteVariable* var = instance.get(*node).type().variable().copyFactory();
          var->setName(elt_name.str());
          factory.setVariable(*var); // var is copied by the factory
          delete var;
          break;
        }
      case ClassElement::prm_aggregate:
        {
          std::stringstream elt_name;
          elt_name << instance.name() << "." << instance.type().get(*node).safeName();
          __groundAgg(instance.type().get(*node), elt_name.str(), factory);
          break;
        }
      default: { /* Do nothing */; }
    }
  }
}

void
System::__groundAgg(const ClassElement& elt, const std::string& name, BayesNetFactory<prm_float>& factory) const {
  factory.startVariableDeclaration();
  factory.variableName(name);
  const DiscreteVariable& agg_var = elt.type().variable();
  for (Idx i = 0; i < agg_var.domainSize(); ++i) {
    factory.addModality(agg_var.label(i));
  }
  const Aggregate& agg = static_cast<const Aggregate&>(elt);
  switch (agg.agg_type()) {
    case Aggregate::agg_min:
      factory.setVariableCPTImplementation(new aggregator::Min<prm_float>());
      break;
    case Aggregate::agg_max:
      factory.setVariableCPTImplementation(new aggregator::Max<prm_float>());
      break;
    case Aggregate::agg_exists:
      factory.setVariableCPTImplementation(new aggregator::Exists<prm_float>(agg.label()));
      break;
    case Aggregate::agg_forall:
      factory.setVariableCPTImplementation(new aggregator::Forall<prm_float>(agg.label()));
      break;
    default:
      GUM_ERROR(OperationNotAllowed, "Aggregator not handled yet");
      break;
  }
  factory.endVariableDeclaration();
}

void
System::__groundRef(const Instance& instance, BayesNetFactory<prm_float>& factory) const
{
  for (Instance::const_iterator iter = instance.begin(); iter != instance.end(); ++iter) {
    std::stringstream elt_name;
    elt_name << instance.name() << "." << (*iter)->safeName();
    factory.startParentsDeclaration(elt_name.str());
    const NodeSet& parents = instance.type().dag().parents((**iter).id());
    for (NodeSetIterator arc = parents.begin(); arc != parents.end(); ++arc) {
      switch (instance.type().get(*arc).elt_type()) {
        case ClassElement::prm_aggregate:
        case ClassElement::prm_attribute:
          {
            std::stringstream parent_name;
            parent_name << instance.name() << "." << instance.get(*arc).safeName();
            factory.addParent(parent_name.str());
            break;
          }
        case ClassElement::prm_slotchain:
          {
            std::string parent_name =
              static_cast<const SlotChain&>(instance.type().get(*arc)).lastElt().safeName();
            const Set<Instance*>& refs = instance.getInstances(*arc);
            for (Set<Instance*>::iterator iter = refs.begin(); iter != refs.end(); ++iter) {
              std::stringstream sBuff;
              sBuff << (*iter)->name() << "." << parent_name;
              factory.addParent(sBuff.str());
            }
            break;
          }
        default: { /* Do nothing */; }
      }
    }
    factory.endParentsDeclaration();
    // Checking if we need to ground the Potential (only for class level attributes since
    // aggregates Potentials are generated)
    if (ClassElement::isAttribute(instance.type().get((**iter).safeName()))) {
      __groundPotential(instance, **iter, factory);
    }
  }
}

void
System::__groundPotential(const Instance& instance, const Attribute& attr, BayesNetFactory<prm_float>& factory) const {
  Bijection<const DiscreteVariable*, const DiscreteVariable*> bijection;
  std::stringstream var_name;
  var_name << instance.name() << "." << attr.safeName();
  bijection.insert(&(attr.type().variable()), &(factory.variable(var_name.str())));
  const DAG& dag = instance.type().dag();
  const NodeSet& parents = dag.parents(attr.id());
  for (NodeSetIterator parent = parents.begin(); parent != parents.end(); ++parent) {
    switch (instance.type().get(*parent).elt_type()) {
      case ClassElement::prm_aggregate:
      case ClassElement::prm_attribute:
        {
          std::stringstream parent_name;
          parent_name << instance.name() << "." << instance.get(*parent).safeName();
          bijection.insert(&(instance.get(*parent).type().variable()), &(factory.variable(parent_name.str())));
          break;
        }
      case ClassElement::prm_slotchain:
        {
          std::stringstream parent_name;
          const SlotChain& sc = static_cast<const SlotChain&>(instance.type().get(*parent));
          parent_name << instance.getInstance(sc.id()).name() << "." << sc.lastElt().safeName();
          bijection.insert(
              &(instance.getInstance(sc.id()).get(sc.lastElt().safeName()).type().variable()),
              &(factory.variable(parent_name.str()))
              );
          break;
        }
      default: {
                 GUM_ERROR(FatalError, "invalid ClassElement type as parent.");
                 break;
               }
    }
  }
  // This should be optimized (using MultiDimBijArray and by handling noisy-or)
  Potential<prm_float>* p = copyPotential(bijection, attr.cpf());
  factory.setVariableCPT(var_name.str(), p, false);
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
