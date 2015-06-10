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
 * @brief Inline implementation of ClusteredLayerGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    PRM<GUM_SCALAR> *ClusteredLayerGenerator<GUM_SCALAR>::generate() {
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
    std::string ClusteredLayerGenerator<GUM_SCALAR>::__generateType(
        PRMFactory<GUM_SCALAR> &factory) {
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
    void ClusteredLayerGenerator<GUM_SCALAR>::__generateInterfaces(
        PRMFactory<GUM_SCALAR> &f, const std::string &type,
        std::vector<ClusteredLayerGenerator<GUM_SCALAR>::MyData> &l) {
      for (Size lvl = 0; lvl < __layers.size(); ++lvl) {
        l.push_back(ClusteredLayerGenerator<GUM_SCALAR>::MyData());
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
    void ClusteredLayerGenerator<GUM_SCALAR>::__generateClasses(
        PRMFactory<GUM_SCALAR> &f, const std::string &type,
        std::vector<ClusteredLayerGenerator<GUM_SCALAR>::MyData> &l) {
      // double ratio = getClusterRatio() + RAND_MAX;
      Set<std::string> i;

      for (Size lvl = 0; lvl < __layers.size(); ++lvl) {
        i.insert(l[lvl].i);

        for (Size c = 0; c < __layers[lvl].c; ++c) {
          // if (std::rand() < ratio)
          __generateCluster(f, type, l, lvl, i);
          // else
          //  __generateClass(f, type, l, lvl, i);
        }

        i.erase(l[lvl].i);
      }
    }

    template <typename GUM_SCALAR>
    void ClusteredLayerGenerator<GUM_SCALAR>::__generateCluster(
        PRMFactory<GUM_SCALAR> &f, const std::string &type,
        std::vector<ClusteredLayerGenerator<GUM_SCALAR>::MyData> &l, Size lvl,
        Set<std::string> &i) {
      Size size = 0;
      GUM_SCALAR sum = 0.0;
      std::string first, second, third;
      std::vector<std::string> *v = 0;

      switch (std::rand() % 2) {
        // Shape A->B
        // v == [first, second, second.ref -> first]
        case 0: {
          v = new std::vector<std::string>();
          __generateClass(f, type, l, lvl, i);
          first = l[lvl].c.back();
          v->push_back(first);
          v->push_back(this->_name_gen.nextName(PRMObject::PRMType::CLASS));
          f.startClass(v->back());
          v->push_back(this->_name_gen.nextName(PRMObject::PRMType::CLASS_ELT));
          f.addReferenceSlot(first, v->back(), true);
          DAG dag;
          Bijection<std::string, NodeId> names;
          __generateClassDag(lvl, dag, names, l);

          // Adding aggregates
          for (std::vector<std::string>::iterator g = l[lvl].g.begin();
               g != l[lvl].g.end(); ++g) {
            std::stringstream s;
            s << v->back() << "." << l[lvl].a[std::rand() % l[lvl].a.size()];
            std::vector<std::string> chain(1, s.str()), param(1, "1");
            f.addAggregator(*g, "exists", chain, param);
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
          break;
        }

        // Shape A -> B -> C
        // v == [first, second, second.ref -> first, third, third.ref -> second]
        case 1: {
          v = new std::vector<std::string>();
          __generateClass(f, type, l, lvl, i);
          {
            first = l[lvl].c.back();
            v->push_back(first);
            v->push_back(this->_name_gen.nextName(PRMObject::PRMType::CLASS));
            second = v->back();
            f.startClass(second);
            v->push_back(this->_name_gen.nextName(PRMObject::PRMType::CLASS_ELT));
            f.addReferenceSlot(first, v->back(), true);
            DAG dag;
            Bijection<std::string, NodeId> names;
            __generateClassDag(lvl, dag, names, l);

            // Adding aggregates
            for (std::vector<std::string>::iterator g = l[lvl].g.begin();
                 g != l[lvl].g.end(); ++g) {
              std::stringstream s;
              s << v->back() << "." << l[lvl].a[std::rand() % l[lvl].a.size()];
              std::vector<std::string> chain(1, s.str()), param(1, "1");
              f.addAggregator(*g, "exists", chain, param);
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
          {
            v->push_back(this->_name_gen.nextName(PRMObject::PRMType::CLASS));
            third = v->back();
            f.startClass(third);
            v->push_back(this->_name_gen.nextName(PRMObject::PRMType::CLASS_ELT));
            f.addReferenceSlot(second, v->back(), true);
            DAG dag;
            Bijection<std::string, NodeId> names;
            __generateClassDag(lvl, dag, names, l);

            // Adding aggregates
            for (std::vector<std::string>::iterator g = l[lvl].g.begin();
                 g != l[lvl].g.end(); ++g) {
              std::stringstream s;
              s << v->back() << "." << l[lvl].a[std::rand() % l[lvl].a.size()];
              std::vector<std::string> chain(1, s.str()), param(1, "1");
              f.addAggregator(*g, "exists", chain, param);
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
          break;
        }

        default: { GUM_ERROR(OperationNotAllowed, "unexpected value"); }
      }

      __cluster_map.insert(first, v);
    }

    template <typename GUM_SCALAR>
    void ClusteredLayerGenerator<GUM_SCALAR>::__generateClass(
        PRMFactory<GUM_SCALAR> &f, const std::string &type,
        std::vector<ClusteredLayerGenerator<GUM_SCALAR>::MyData> &l, Size lvl,
        Set<std::string> &i) {
      Size size = 0;
      GUM_SCALAR sum = 0.0;
      l[lvl].c.push_back(this->_name_gen.nextName(PRMObject::PRMType::CLASS));
      f.startClass(l[lvl].c.back(), "", &i);

      if (lvl)
        f.addReferenceSlot(l[lvl - 1].i, l[lvl].r, true);

      DAG dag;
      Bijection<std::string, NodeId> names;
      __generateClassDag(lvl, dag, names, l);

      // Adding aggregates
      if (lvl) {
        for (const auto agg : l[lvl].g) {
          std::stringstream s;
          s << l[lvl].r << "." << l[lvl - 1].a[std::rand() % l[lvl - 1].a.size()];
          std::vector<std::string> chain(1, s.str()), param(1, "1");
          f.addAggregator(agg, "exists", chain, param);
        }
      }

      // Adding attributes
      for (const auto attr : l[lvl].a) {
        f.startAttribute(type, attr);
        size = getDomainSize();

        for (const auto par : dag.parents(names.second(attr))) {
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

    template <typename GUM_SCALAR>
    void ClusteredLayerGenerator<GUM_SCALAR>::__generateClassDag(
        Size lvl, DAG &dag, Bijection<std::string, NodeId> &names,
        std::vector<ClusteredLayerGenerator<GUM_SCALAR>::MyData> &l) {
      float density = __layers[lvl].inner_density * RAND_MAX;
      std::vector<NodeId> nodes;
      NodeId id = 0;

      if (lvl) {
        for (std::vector<std::string>::iterator g = l[lvl].g.begin();
             g != l[lvl].g.end(); ++g) {
          id = dag.addNode();
          names.insert(*g, id);
          nodes.push_back(id);
        }
      }

      for (std::vector<std::string>::iterator a = l[lvl].a.begin();
           a != l[lvl].a.end(); ++a) {
        id = dag.addNode();
        names.insert(*a, id);

        for (std::vector<NodeId>::iterator prnt = nodes.begin(); prnt != nodes.end();
             ++prnt)
          if (std::rand() < density)
            dag.addArc(*prnt, names.second(*a));

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
    void ClusteredLayerGenerator<GUM_SCALAR>::__generateSystem(
        PRMFactory<GUM_SCALAR> &factory,
        std::vector<ClusteredLayerGenerator<GUM_SCALAR>::MyData> &l) {
      factory.startSystem(this->_name_gen.nextName(PRMObject::PRMType::SYSTEM));
      std::vector<std::vector<std::string>> o(__layers.size());
      std::string name, c, first, second, third;
      std::vector<std::string> *v = 0;
      size_t idx = 0;

      for (size_t lvl = 0; lvl < __layers.size(); ++lvl) {
        float density = __layers[lvl].outter_density * RAND_MAX;

        for (size_t count = 0; count < __layers[lvl].o; ++count) {
          c = l[lvl].c[std::rand() % l[lvl].c.size()];

          if (__cluster_map.exists(c)) {
            v = __cluster_map[c];

            switch (v->size()) {
              case 3: {
                first = this->_name_gen.nextName(PRMObject::PRMType::INSTANCE);
                factory.addInstance(c, first);
                second = this->_name_gen.nextName(PRMObject::PRMType::INSTANCE);
                factory.addInstance(v->at(1), second);
                std::stringstream chain;
                chain << second << "." << v->at(2);
                factory.setReferenceSlot(chain.str(), first);
                break;
              }

              case 5: {
                first = this->_name_gen.nextName(PRMObject::PRMType::INSTANCE);
                factory.addInstance(c, first);
                second = this->_name_gen.nextName(PRMObject::PRMType::INSTANCE);
                factory.addInstance(v->at(1), second);
                std::stringstream chain_1, chain_2;
                chain_1 << second << "." << v->at(2);
                factory.setReferenceSlot(chain_1.str(), first);
                third = this->_name_gen.nextName(PRMObject::PRMType::INSTANCE);
                factory.addInstance(v->at(3), third);
                chain_2 << third << "." << v->at(4);
                factory.setReferenceSlot(chain_2.str(), second);
                break;
              }

              default: {
                GUM_TRACE_VAR(v->size());
                GUM_ERROR(OperationNotAllowed, "unexpected vector size");
              }
            }

            // __cluster_map.erase(c);
            // delete v;
            name = first;
          } else {
            name = this->_name_gen.nextName(PRMObject::PRMType::INSTANCE);
            factory.addInstance(c, name);
          }

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
    INLINE ClusteredLayerGenerator<GUM_SCALAR>::ClusteredLayerGenerator()
        : __layers(), __domain_size(2), __max_parents(INT_MAX),
          __cluster_ratio(0.0) {
      GUM_CONSTRUCTOR(ClusteredLayerGenerator);
    }

    template <typename GUM_SCALAR>
    INLINE ClusteredLayerGenerator<GUM_SCALAR>::ClusteredLayerGenerator(
        const ClusteredLayerGenerator<GUM_SCALAR> &source)
        : __layers(source.__layers), __domain_size(source.__domain_size),
          __max_parents(source.__max_parents) {
      GUM_CONS_CPY(ClusteredLayerGenerator);
    }

    template <typename GUM_SCALAR>
    INLINE ClusteredLayerGenerator<GUM_SCALAR>::~ClusteredLayerGenerator() {
      GUM_DESTRUCTOR(ClusteredLayerGenerator);
      // typedef HashTable<std::string, std::vector<std::string>*>::iterator Iter;
      // for (Iter iter = __cluster_map.begin(); iter != __cluster_map.end(); ++iter)
      // {
      //   delete *iter;
      // }
    }

    template <typename GUM_SCALAR>
    INLINE ClusteredLayerGenerator<GUM_SCALAR> &ClusteredLayerGenerator<GUM_SCALAR>::
    operator=(const ClusteredLayerGenerator<GUM_SCALAR> &source) {
      __layers = source.__layers;
      __domain_size = source.__domain_size;
      __max_parents = source.__max_parents;
      return *this;
    }

    template <typename GUM_SCALAR>
    INLINE Size ClusteredLayerGenerator<GUM_SCALAR>::getDomainSize() const {
      return __domain_size;
    }

    template <typename GUM_SCALAR>
    INLINE void ClusteredLayerGenerator<GUM_SCALAR>::setDomainSize(Size s) {
      __domain_size = s;
    }

    template <typename GUM_SCALAR>
    INLINE unsigned int ClusteredLayerGenerator<GUM_SCALAR>::getMaxParents() const {
      return __max_parents;
    }

    template <typename GUM_SCALAR>
    INLINE void ClusteredLayerGenerator<GUM_SCALAR>::setMaxParents(Size s) {
      __max_parents = s;
    }

    template <typename GUM_SCALAR>
    INLINE void ClusteredLayerGenerator<GUM_SCALAR>::setLayers(
        const std::vector<typename LayerGenerator<GUM_SCALAR>::LayerData> &v) {
      __layers = v;
    }

    template <typename GUM_SCALAR>
    INLINE std::vector<typename LayerGenerator<GUM_SCALAR>::LayerData> &
    ClusteredLayerGenerator<GUM_SCALAR>::getLayer() {
      return __layers;
    }

    template <typename GUM_SCALAR>
    INLINE const std::vector<typename LayerGenerator<GUM_SCALAR>::LayerData> &
    ClusteredLayerGenerator<GUM_SCALAR>::getLayer() const {
      return __layers;
    }

    template <typename GUM_SCALAR>
    INLINE double ClusteredLayerGenerator<GUM_SCALAR>::getClusterRatio() const {
      return __cluster_ratio;
    }

    template <typename GUM_SCALAR>
    INLINE void ClusteredLayerGenerator<GUM_SCALAR>::setClusterRatio(double ratio) {
      __cluster_ratio = ratio;
    }

  } /* namespace prm */
} /* namespace gum */
