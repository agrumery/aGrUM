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
 * @brief Implementation of Model.
 *
 * @author Lionel TORTI
 *
 */
// ============================================================================
#include <agrum/prm/model.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/model.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

Model::Model(const std::string& name):
  PRMObject(name)
{
  GUM_CONSTRUCTOR( Model );
}

Model::~Model()
{
  GUM_DESTRUCTOR( Model );
  for (HashTable<std::string, Instance*>::iterator iter = __nameMap.begin(); iter != __nameMap.end(); ++iter) {
    delete *iter;
  }
  __nameMap.clear();
  typedef HashTableIterator<Class*, Set<Instance*>* > instance_map_iterator;
  for (instance_map_iterator iter = __instanceMap.begin(); iter != __instanceMap.end(); ++iter) {
    delete *iter;
  }
  __instanceMap.clear();
  typedef HashTableIterator< std::string, Model::model_pair > array_map_iterator;
  for (array_map_iterator iter = __arrayMap.begin(); iter != __arrayMap.end(); ++iter) {
    delete iter->second;
  }
  __arrayMap.clear();
}

Model::Model(const Model& from):
  PRMObject(from.name())
{
  GUM_CONS_CPY( Model );
  GUM_ERROR(FatalError, "Illegal call to copy constructor.");
}

Model&
Model::operator=(const Model& from)
{
  GUM_ERROR(FatalError, "Illegal call to copy constructor.");
}

void
Model::instantiate()
{
  for (HashTable< Class*, Set<Instance*>* >::iterator iter = __instanceMap.begin(); iter != __instanceMap.end(); ++iter) {
    for (Sequence<NodeId>::iterator node = iter.key()->instantiations().begin(); node != iter.key()->instantiations().end(); ++node) {
      for (Set<Instance*>::iterator instance = (**iter).begin(); instance != (**iter).end(); ++instance) {
        (**instance).instantiate(*node);
      }
    }
  }
}

void
Model::groundedBN(BayesNetFactory<prm_float>& factory) const
{
  factory.startNetworkDeclaration();
  factory.addNetworkProperty("name", name());
  factory.endNetworkDeclaration();
  // Adding nodes
  for (Model::const_iterator iter = begin(); not iter.isEnd(); ++iter) {
    __groundAttr(*iter, factory);
  }
  // Adding arcs and filling CPTs
  for (Model::const_iterator iter = begin(); not iter.isEnd(); ++iter) {
    __groundRef(*iter, factory);
  }
}

void
Model::__groundAttr(const Instance& instance, BayesNetFactory<prm_float>& factory) const
{
  for (DAG::NodeIterator node = instance.dag().beginNodes(); node != instance.dag().endNodes(); ++node) {
    // Working a Class level because Aggregate are instantiated as Attribute in an Instance
    std::stringstream elt_name;
    elt_name << instance.name() << "." << instance.type().get(*node).name();
    switch (instance.type().get(*node).elt_type()) {
      case ClassElement::prm_attribute: {
                                          DiscreteVariable* var = instance.get(*node).type().variable().copyFactory();
                                          var->setName(elt_name.str());
                                          factory.setVariable(*var); // var is copied by the factory
                                          delete var;
                                          break;
                                        }
      case ClassElement::prm_aggregate: {
                                          __groundAgg(instance.type().get(*node), elt_name.str(), factory);
                                          break;
                                        }
      default: { /* Do nothing */; }
    }
  }
}

void
Model::__groundAgg(const ClassElement& elt, const std::string& name,
                   BayesNetFactory<prm_float>& factory) const {
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
Model::__groundRef(const Instance& instance, BayesNetFactory<prm_float>& factory) const
{
  const ClassElement* elt = 0;
  for (DAG::NodeIterator node = instance.dag().beginNodes(); node != instance.dag().endNodes(); ++node) {
    elt = &(instance.get(*node));
    std::stringstream elt_name;
    elt_name << instance.name() << "." << elt->name();
    if (elt->elt_type() == ClassElement::prm_attribute) {
      factory.startParentsDeclaration(elt_name.str());
      for (DAG::ArcIterator arc = instance.parents(*elt).begin(); arc != instance.parents(*elt).end(); ++arc) {
        switch (instance.get(arc->tail()).elt_type()) {
          case ClassElement::prm_attribute: {
                                              std::stringstream parent_name;
                                              parent_name << instance.name() << "." << instance.get(arc->tail()).name();
                                              factory.addParent(parent_name.str());
                                              break;
                                            }
          case ClassElement::prm_slotchain: {
                                              //GUM_TRACE_VAR(instance.get(arc->tail()).name());
                                              for (Set<Instance*>::iterator iter = instance.getInstances(arc->tail()).begin();
                                                  iter != instance.getInstances(arc->tail()).end(); ++iter)
                                              {
                                                std::stringstream parent_name;
                                                parent_name << (*iter)->name() << "." << static_cast<const SlotChain&>(instance.get(arc->tail())).lastElt().name();
                                                factory.addParent(parent_name.str());
                                              }
                                              break;
                                            }
          default: { /* Do nothing */; }
        }
      }
      factory.endParentsDeclaration();
      // Checking if we need to ground the Potential (only for attributes at the class level since
      // aggregate's Potential are generated)
      if (instance.type().get(elt->id()).elt_type() == ClassElement::prm_attribute) {
        __groundPotential(instance, *node, factory);
      }
    }
  }
}

void
Model::__groundPotential(const Instance& instance, NodeId node, BayesNetFactory<prm_float>& factory) const {
  Bijection<const DiscreteVariable*, const DiscreteVariable*> bijection;
  std::stringstream var_name;
  var_name << instance.name() << "." << instance.get(node).name();

  bijection.insert(&(factory.variable(var_name.str())), &(instance.get(node).type().variable()));
  for (DAG::ArcIterator parent = instance.parents(node).begin(); parent != instance.parents(node).end(); ++parent) {
    switch (instance.get(parent->tail()).elt_type()) {
      case ClassElement::prm_attribute: {
                                          std::stringstream parent_name;
                                          parent_name << instance.name() << "." << instance.get(parent->tail()).name();
                                          bijection.insert(&(factory.variable(parent_name.str())), &(instance.get(parent->tail()).type().variable()));
                                          break;
                                        }
      case ClassElement::prm_slotchain: {
                                          std::stringstream parent_name;
                                          const SlotChain& sc = static_cast<const SlotChain&>(instance.get(parent->tail()));
                                          parent_name << instance.getInstance(sc.id()).name() << "." << sc.lastElt().name();
                                          bijection.insert(&(factory.variable(parent_name.str())), &(instance.getInstance(sc.id()).get(sc.lastElt().id()).type().variable()));
                                          break;
                                        }
      default: {
                 GUM_ERROR(FatalError, "invalid ClassElement type as parent.");
                 break;
               }
    }
  }

  Potential<prm_float>* p = new Potential<prm_float>();
  typedef Sequence<const DiscreteVariable*>::const_iterator VarSeqIterator;
  const Sequence<const DiscreteVariable*>& var_seq = instance.get(node).cpf().variablesSequence();
  for (VarSeqIterator iter = var_seq.begin(); iter != var_seq.end(); ++iter) {
    p->add(*(bijection.first(*iter)));
  }

  Instantiation i(p);
  Instantiation j(instance.get(node).cpf());
  for (j.setFirst(); not j.end(); j.inc()) {
    typedef Sequence<const DiscreteVariable*>::const_iterator VarSeqIterator;
    for (VarSeqIterator iter = j.variablesSequence().begin(); iter != j.variablesSequence().end(); ++iter) {
      i.chgVal(*(bijection.first(*iter)), j.val(*iter));
    }
    p->set(i, instance.get(node).cpf().get(j));
  }

  factory.setVariableCPT(var_name.str(), p, false);
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
