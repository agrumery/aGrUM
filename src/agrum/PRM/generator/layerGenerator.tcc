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
 * @brief Inline implementation of LayerGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/generator/layerGenerator.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    PRM<GUM_SCALAR> *LayerGenerator<GUM_SCALAR>::generate() {
      if (__layers.size() == 0) {
        GUM_ERROR(OperationNotAllowed,
                  "cannot generate a layered PRM<GUM_SCALAR> without layers");
      }

      std::vector<MyData> l;
      PRMFactory<GUM_SCALAR> factory;
      std::string type = __generateType(factory);
      __generateInterfaces(factory, type, l);
      __generateClasses(factory, type, l);
      __generateSystem(factory, l);
      return factory.prm();
    }

    template <typename GUM_SCALAR>
    std::string
    LayerGenerator<GUM_SCALAR>::__generateType(PRMFactory<GUM_SCALAR> &factory) {
      std::string name = this->_name_gen.nextName(PRMObject::PRMType::TYPE);
      factory.startDiscreteType(name);

      for (Size i = 0; i < __domain_size; ++i) {
        std::stringstream sBuff;
        sBuff << i;
        factory.addLabel(sBuff.str());
      }

      factory.endDiscreteType();
      return name;
    }

    template <typename GUM_SCALAR>
    void LayerGenerator<GUM_SCALAR>::__generateInterfaces(
        PRMFactory<GUM_SCALAR> &f, const std::string &type,
        std::vector<LayerGenerator<GUM_SCALAR>::MyData> &l) {
      for (Size lvl = 0; lvl < __layers.size(); ++lvl) {
        l.push_back(LayerGenerator<GUM_SCALAR>::MyData());
        l[lvl].i = this->_name_gen.nextName(PRMObject::PRMType::INTERFACE);
        f.startInterface(l[lvl].i);

        for (Size a = 0; a < __layers[lvl].a; ++a) {
          l[lvl].a.push_back(
              this->_name_gen.nextName(PRMObject::PRMType::CLASS_ELT));
          f.addAttribute(type, l[lvl].a.back());
        }

        if (lvl) {
          for (Size g = 0; g < __layers[lvl].g; ++g) {
            l[lvl].g.push_back(
                this->_name_gen.nextName(PRMObject::PRMType::CLASS_ELT));
            f.addAttribute("boolean", l[lvl].g.back());
          }

          l[lvl].r = this->_name_gen.nextName(PRMObject::PRMType::CLASS_ELT);
          f.addReferenceSlot(l[lvl - 1].i, l[lvl].r, true);
        }

        f.endInterface();
      }
    }

    template <typename GUM_SCALAR>
    void LayerGenerator<GUM_SCALAR>::__generateClasses(
        PRMFactory<GUM_SCALAR> &f, const std::string &type,
        std::vector<LayerGenerator<GUM_SCALAR>::MyData> &l) {
      const Set<NodeId> *parents = 0;
      Size size = 0;
      GUM_SCALAR sum = 0.0;
      Set<std::string> i;

      for (Size lvl = 0; lvl < __layers.size(); ++lvl) {
        i.insert(l[lvl].i);

        for (Size c = 0; c < __layers[lvl].c; ++c) {
          l[lvl].c.push_back(this->_name_gen.nextName(PRMObject::PRMType::CLASS));
          f.startClass(l[lvl].c.back(), "", &i);

          if (lvl)
            f.addReferenceSlot(l[lvl - 1].i, l[lvl].r, true);

          DAG dag;
          Bijection<std::string, NodeId> names;
          __generateClassDag(lvl, dag, names, l);

          // Adding aggregates
          if (lvl) {
            for (std::vector<std::string>::iterator g = l[lvl].g.begin();
                 g != l[lvl].g.end(); ++g) {
              std::stringstream s;
              s << l[lvl].r << "."
                << l[lvl - 1].a[std::rand() % l[lvl - 1].a.size()];
              std::vector<std::string> chain(1, s.str()), param(1, "1");
              f.addAggregator(*g, "exists", chain, param);
            }
          }

          // Adding attributes
          for (std::vector<std::string>::iterator a = l[lvl].a.begin();
               a != l[lvl].a.end(); ++a) {
            f.startAttribute(type, *a);
            size = getDomainSize();

            for (const auto par : dag.parents(names.second(*a))) {
              f.addParent(names.first(par));
              size *= f.retrieveClass(l[lvl].c.back())
                          .get(names.first(par))
                          .type()
                          ->domainSize();
            }

            std::vector<GUM_SCALAR> cpf(size), val(getDomainSize());

            for (size_t norms = 0; norms < size; norms += getDomainSize()) {
              sum = 0.0;

              for (size_t idx = 0; idx < getDomainSize(); ++idx) {
                val[idx] = 1 + std::rand();
                sum += val[idx];
              }

              for (size_t idx = 0; idx < getDomainSize(); ++idx)
                cpf[norms + idx] = val[idx] / sum;
            }

            f.setRawCPFByLines(cpf);
            f.endAttribute();
          }

          f.endClass();
        }

        i.erase(l[lvl].i);
      }
    }

    template <typename GUM_SCALAR>
    void LayerGenerator<GUM_SCALAR>::__generateClassDag(
        Size lvl, DAG &dag, Bijection<std::string, NodeId> &names,
        std::vector<LayerGenerator<GUM_SCALAR>::MyData> &l) {
      float density = __layers[lvl].inner_density * RAND_MAX;
      std::vector<NodeId> nodes;
      NodeId id = 0;

      if (lvl) {
        for (const auto agg : l[lvl].g) {
          id = dag.addNode();
          names.insert(agg, id);
          nodes.push_back(id);
        }
      }

      for (const auto attr : l[lvl].a) {
        id = dag.addNode();
        names.insert(attr, id);

        for (const auto node : nodes)
          if (std::rand() < density)
            dag.addArc(node, names.second(attr));

        nodes.push_back(id);
      }

      // For each nodes with #parents > __max_parents we randomly remove parents
      // until
      // #parents <= __max_parents
      for (const auto node : dag.nodes()) {
        if (dag.parents(node).size() > getMaxParents()) {
          std::vector<NodeId> v;

          for (const auto par : dag.parents(node))
            v.push_back(par);

          while (dag.parents(node).size() > getMaxParents()) {
            size_t idx = std::rand() % v.size();
            Arc arc(v[idx], node);
            GUM_ASSERT(dag.existsArc(arc));
            dag.eraseArc(arc);
            v[idx] = v.back();
            v.pop_back();
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    void LayerGenerator<GUM_SCALAR>::__generateSystem(
        PRMFactory<GUM_SCALAR> &factory,
        std::vector<LayerGenerator<GUM_SCALAR>::MyData> &l) {
      factory.startSystem(this->_name_gen.nextName(PRMObject::PRMType::SYSTEM));
      std::vector<std::vector<std::string>> o(__layers.size());
      std::string name;
      size_t idx = 0;

      for (size_t lvl = 0; lvl < __layers.size(); ++lvl) {
        float density = __layers[lvl].outter_density * RAND_MAX;

        for (size_t count = 0; count < __layers[lvl].o; ++count) {
          name = this->_name_gen.nextName(PRMObject::PRMType::INTERFACE);
          factory.addInstance(l[lvl].c[std::rand() % l[lvl].c.size()], name);
          o[lvl].push_back(name);

          if (lvl) {
            std::stringstream chain;
            chain << name << "." << l[lvl].r;
            std::vector<std::string> ref2add;

            for (std::vector<std::string>::iterator iter = o[lvl - 1].begin();
                 iter != o[lvl - 1].end(); ++iter)
              if (std::rand() <= density)
                ref2add.push_back(*iter);

            if (ref2add.empty())
              factory.setReferenceSlot(chain.str(),
                                       o[lvl - 1][std::rand() % o[lvl - 1].size()]);

            while (ref2add.size() > getMaxParents()) {
              idx = std::rand() % ref2add.size();
              ref2add[idx] = ref2add.back();
              ref2add.pop_back();
            }

            for (std::vector<std::string>::iterator iter = ref2add.begin();
                 iter != ref2add.end(); ++iter)
              factory.setReferenceSlot(chain.str(), *iter);
          }
        }
      }

      factory.endSystem();
    }

    template <typename GUM_SCALAR>
    INLINE LayerGenerator<GUM_SCALAR>::LayerGenerator()
        : __layers(), __domain_size(2), __max_parents(INT_MAX) {
      GUM_CONSTRUCTOR(LayerGenerator);
    }

    template <typename GUM_SCALAR>
    INLINE LayerGenerator<GUM_SCALAR>::LayerGenerator(
        const LayerGenerator<GUM_SCALAR> &source)
        : __layers(source.__layers), __domain_size(source.__domain_size),
          __max_parents(source.__max_parents) {
      GUM_CONS_CPY(LayerGenerator);
    }

    template <typename GUM_SCALAR>
    INLINE LayerGenerator<GUM_SCALAR>::~LayerGenerator() {
      GUM_DESTRUCTOR(LayerGenerator);
    }

    template <typename GUM_SCALAR>
    INLINE LayerGenerator<GUM_SCALAR> &LayerGenerator<GUM_SCALAR>::
    operator=(const LayerGenerator<GUM_SCALAR> &source) {
      __layers = source.__layers;
      __domain_size = source.__domain_size;
      __max_parents = source.__max_parents;
      return *this;
    }

    template <typename GUM_SCALAR>
    INLINE Size LayerGenerator<GUM_SCALAR>::getDomainSize() const {
      return __domain_size;
    }

    template <typename GUM_SCALAR>
    INLINE void LayerGenerator<GUM_SCALAR>::setDomainSize(Size s) {
      __domain_size = s;
    }

    template <typename GUM_SCALAR>
    INLINE unsigned int LayerGenerator<GUM_SCALAR>::getMaxParents() const {
      return __max_parents;
    }

    template <typename GUM_SCALAR>
    INLINE void LayerGenerator<GUM_SCALAR>::setMaxParents(Size s) {
      __max_parents = s;
    }

    template <typename GUM_SCALAR>
    INLINE void LayerGenerator<GUM_SCALAR>::setLayers(
        const std::vector<LayerGenerator<GUM_SCALAR>::LayerData> &v) {
      __layers = v;
    }

    template <typename GUM_SCALAR>
    INLINE std::vector<typename LayerGenerator<GUM_SCALAR>::LayerData> &
    LayerGenerator<GUM_SCALAR>::getLayer() {
      return __layers;
    }

    template <typename GUM_SCALAR>
    INLINE const std::vector<typename LayerGenerator<GUM_SCALAR>::LayerData> &
    LayerGenerator<GUM_SCALAR>::getLayer() const {
      return __layers;
    }

  } /* namespace prm */
} /* namespace gum */
