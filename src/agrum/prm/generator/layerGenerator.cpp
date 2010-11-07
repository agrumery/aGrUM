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
 * @brief Implementation of LayerGenerator.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/generator/layerGenerator.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/generator/layerGenerator.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
namespace prm {

void
LayerGenerator::setLayers(const std::vector<LayerData>& v) {
  for (std::vector<LayerData>::const_iterator data = v.begin(); data != v.end(); ++data) {
    __layers.push_back(LayerGenerator::Layer());
    __layers.back().data = *data;
  }
}

PRM*
LayerGenerator::generate() {
  std::srand ( std::time(NULL) );
  PRMFactory factory;
  __generateClasses(factory);
  __generateSystem(factory);
  return factory.prm();
}

void
LayerGenerator::__generateClasses(PRMFactory& factory) {
  BayesNetGenerator bn_gen;
  std::vector< Set<std::string> > classes;
  BayesNet<prm_float>* bn = 0;
  std::string type = __generateType(factory);
  if (__layers.size() == 0)
    GUM_ERROR(OperationNotAllowed, "you must define the layers first");
  // Generating the root level
  { __generateInterface(0, factory, type);
    bn = bn_gen.generateBNF(__layer(0).a_count, __layer(0).density);
    Property<std::string>::onNodes outputs;
    __pickOutputs(__layer(0), factory.prm()->getInterface(__interface(0)), *bn, outputs);
    std::string c = __copyClass(__layer(0), factory, *bn, __interface(0), outputs, type);
    classes[0].insert(c);
    delete bn; }
  // Generating the remaining levels
  for (size_t lvl = 1; lvl < __layers.size(); ++lvl) {
    __generateInterface(lvl, factory, type);
    bn = bn_gen.generateBNF(__layer(lvl).a_count, __layer(lvl).density);
    Property<std::string>::onNodes outputs;
    __pickOutputs(__layer(lvl), factory.prm()->getInterface(__interface(lvl)), *bn, outputs);
    std::string c = __copyClass(__layer(lvl), factory, *bn, __interface(lvl), outputs, type);
    classes[lvl].insert(c);
    delete bn;
  }
}

std::string
LayerGenerator::__generateType(PRMFactory& factory) {
  std::string name = _name_gen.nextName(PRMObject::prm_type);
  factory.startDiscreteType(name);
  for (Size i = 0; i < __domainSize; ++i) {
    std::stringstream sBuff;
    sBuff << i;
    factory.addLabel(sBuff.str());
  }
  factory.endDiscreteType();
  return name;
}

std::string
LayerGenerator::__generateInterface(size_t lvl,
                                    PRMFactory& factory,
                                    const std::string& type)
{
  LayerGenerator::LayerData& data = __layer(lvl);
  std::string name = _name_gen.nextName(PRMObject::prm_interface);
  factory.startInterface(name);
  for (size_t i = 0; i < data.interface_size; ++i)
    factory.addAttribute(type, _name_gen.nextName(PRMObject::prm_class_elt));
  if (lvl > 0)
    factory.addReferenceSlot(__interface(lvl - 1),
                             _name_gen.nextName(PRMObject::prm_class_elt),
                             true);
  factory.endInterface();
  __interface(lvl) = name;
  return name;
}

void
LayerGenerator::__pickOutputs(LayerGenerator::LayerData& data, Interface& i,
                              BayesNet<prm_float>& bn,
                              Property<std::string>::onNodes& outputs)
{
  std::vector<NodeId> v;
  for (DAG::NodeIterator n = bn.dag().beginNodes(); n != bn.dag().endNodes(); ++n)
    v.push_back(*n);
  Set<Attribute*>::iterator attr = i.attributes().begin();
  size_t count = 0;
  while ( (count < data.interface_size) and (attr != i.attributes().end()) ) {
    size_t pick = std::rand() % v.size();
    outputs.insert(v[pick], (**attr).safeName());
    v.erase(v.begin() + pick);
    ++count;
    ++attr;
  }
}

std::string
LayerGenerator::__copyClass(LayerGenerator::LayerData& data, PRMFactory& factory,
                            BayesNet<prm_float>& bn, const std::string& i,
                            Property<std::string>::onNodes& outputs,
                            const std::string& type)
{
  Set<std::string> set; set.insert(i);
  std::string name = _name_gen.nextName(PRMObject::prm_class);
  factory.startClass(name, "", &set);
  const Sequence<NodeId>& topo = bn.getTopologicalOrder(true);
  Set<NodeId> agg = __defineAggSet(data, bn, outputs);
  Interface& interface = factory.prm()->getInterface(i);
  Property<std::string>::onNodes name_map;
  std::vector<ReferenceSlot*> refs;
  for (Set<ReferenceSlot*>::iterator ref = interface.referenceSlots().begin(); ref != interface.referenceSlots().end(); ++ref)
    refs.push_back(const_cast<ReferenceSlot*>(*ref));
  for (Sequence<NodeId>::const_iterator node = topo.begin(); node != topo.end(); ++node) {
    if (agg.exists(*node)) {
      __addAggregate(refs, bn, factory, *node, name_map);
    } else {
      std::string name;
      if (outputs.exists(*node))
        name = interface.get(*node).name();
      else
        name = _name_gen.nextName(PRMObject::prm_class_elt);
      factory.startAttribute(type, name);
      name_map.insert(*node, name);
      size_t size = __domainSize;
      for (NodeSet::iterator prnt = bn.dag().parents(*node).begin(); prnt != bn.dag().parents(*node).end(); ++prnt) {
        factory.addParent(name_map[*prnt]);
        size *= (agg.exists(*prnt)?2:__domainSize);
      }
      std::vector<prm_float> cpf = __generateCPF(size);
      factory.setRawCPFByLines(cpf);
      factory.endAttribute();
    }
  }
  factory.endClass();
  return name;
}

void
LayerGenerator::__addAggregate(std::vector<ReferenceSlot*>& refs, BayesNet<prm_float>& bn,
                               PRMFactory& factory, NodeId node,
                               Property<std::string>::onNodes& name_map)
{
  ReferenceSlot& ref = *(refs[std::rand() % refs.size()]);
  std::vector<std::string> chains(bn.dag().parents(node).size() + 1);
  chains[0] = ref.name();
  chains[0] += ".";
  std::vector<Attribute*> attrs;
  const Set<Attribute*>* attr_set = 0;
  if (ref.slotType().obj_type() == PRMObject::prm_class)
    attr_set = &(static_cast<Class&>(ref.slotType()).attributes());
  if (ref.slotType().obj_type() == PRMObject::prm_interface)
    attr_set = &(static_cast<Interface&>(ref.slotType()).attributes());
  for (Set<Attribute*>::iterator a = attr_set->begin(); a != attr_set->end(); ++a)
    attrs.push_back(const_cast<Attribute*>(*a));
  chains[0] += attrs[std::rand() % attrs.size()]->safeName();
  size_t idx = 1;
  for (NodeSet::iterator prnt = bn.dag().parents(node).begin(); prnt != bn.dag().parents(node).end(); ++prnt)
    chains[idx++] = name_map[*prnt];
  std::vector<std::string> params(1, "1");
  factory.addAggregator(_name_gen.nextName(PRMObject::prm_class_elt), "exists", chains, params);
}

Set<NodeId>
LayerGenerator::__defineAggSet(LayerGenerator::LayerData& data, BayesNet<prm_float>& bn,
                               Property<std::string>::onNodes& outputs)
{
  Set<NodeId> agg;
  std::vector<NodeId> nodes;
  for (DAG::NodeIterator n = bn.dag().beginNodes(); n != bn.dag().endNodes(); ++n)
    if (not outputs.exists(*n))
      nodes.push_back(*n);
  for (size_t i = 0; (i < data.agg_size) and nodes.size(); ++i) {
    size_t pick = nodes[std::rand() % nodes.size()];
    agg.insert(nodes[pick]);
    nodes.erase(nodes.begin() + pick);
  }
  return agg;
}

std::vector<prm_float>
LayerGenerator::__generateCPF(size_t size) {
  GUM_ASSERT( (size % __domainSize) == 0);
  std::vector<prm_float> cpf(size);
  prm_float sum;
  for (size_t idx = 0; idx < size; idx += size) {
    sum = 0.0;
    for (size_t mod = 0; mod < __domainSize; ++mod) {
      cpf[idx + mod] = std::rand();
      sum += cpf[idx + mod];
    }
    for (size_t mod = 0; mod < __domainSize; ++mod)
      cpf[idx + mod] = cpf[idx + mod] / sum;
  }
  return cpf;
}

void
LayerGenerator::__generateSystem(PRMFactory& factory) {
  factory.startSystem(_name_gen.nextName(PRMObject::prm_system));
  std::vector< std::vector<std::string> > instances(__layers.size());
  for (size_t lvl = 0; lvl < __layers.size(); ++lvl) {
    std::vector<const Class*> classes;
    Interface& interface = factory.prm()->getInterface(__interface(lvl));
    for (Set<Class*>::const_iterator c = interface.implementations().begin();
         c != interface.implementations().end(); ++c)
      classes.push_back(*c);
    for (size_t count = 0; count < __layer(lvl).i_count; ++count) {
      std::string name = _name_gen.nextName(PRMObject::prm_interface);
      std::string c = classes[std::rand() % classes.size()]->name();
      factory.addInstance(c, name);
      instances[lvl].push_back(name);
      if (lvl) {
        GUM_ASSERT(factory.prm()->getClass(c).referenceSlots().size());
        std::stringstream chain;
        chain << name << "." << (*(factory.prm()->getClass(c).referenceSlots().begin()))->name();
        size_t foo = 0;
        for (std::vector<std::string>::iterator iter = instances[lvl-1].begin();
             iter != instances[lvl-1].end(); ++iter)
          if (std::rand() < __layer(lvl).density) {
            factory.incArray(chain.str(), *iter);
            ++foo;
          }
        if (not foo)
          factory.incArray(chain.str(), instances[lvl-1][std::rand() % instances[lvl-1].size()]);
      }
    }
  }
  factory.endSystem();
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================
