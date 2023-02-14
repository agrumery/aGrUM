/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Inline implementation of ClusteredLayerGenerator.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRM< GUM_SCALAR >* ClusteredLayerGenerator< GUM_SCALAR >::generate() {
      if (_layers_.size() == 0) {
        GUM_ERROR(OperationNotAllowed, "cannot generate a layered PRM<GUM_SCALAR> without layers")
      }

      std::vector< MyData >    l;
      PRMFactory< GUM_SCALAR > factory;
      std::string              type = _generateType_(factory);
      _generateInterfaces_(factory, type, l);
      _generateClasses_(factory, type, l);
      _generateSystem_(factory, l);
      return factory.prm();
    }

    template < typename GUM_SCALAR >
    std::string
       ClusteredLayerGenerator< GUM_SCALAR >::_generateType_(PRMFactory< GUM_SCALAR >& factory) {
      std::string name = this->name_gen_.nextName(PRMObject::prm_type::TYPE);
      factory.startDiscreteType(name);

      for (Size i = 0; i < _domain_size_; ++i) {
        std::stringstream sBuff;
        sBuff << i;
        factory.addLabel(sBuff.str());
      }

      factory.endDiscreteType();
      return name;
    }

    template < typename GUM_SCALAR >
    void ClusteredLayerGenerator< GUM_SCALAR >::_generateInterfaces_(
       PRMFactory< GUM_SCALAR >&                                              f,
       const std::string&                                                     type,
       std::vector< typename ClusteredLayerGenerator< GUM_SCALAR >::MyData >& l) {
      for (Size lvl = 0; lvl < _layers_.size(); ++lvl) {
        l.push_back(ClusteredLayerGenerator< GUM_SCALAR >::MyData());
        l[lvl].i = this->name_gen_.nextName(PRMObject::prm_type::PRM_INTERFACE);
        f.startInterface(l[lvl].i);

        for (Size a = 0; a < _layers_[lvl].a; ++a) {
          l[lvl].a.push_back(this->name_gen_.nextName(PRMObject::prm_type::CLASS_ELT));
          f.addAttribute(type, l[lvl].a.back());
        }

        if (lvl) {
          for (Size g = 0; g < _layers_[lvl].g; ++g) {
            l[lvl].g.push_back(this->name_gen_.nextName(PRMObject::prm_type::CLASS_ELT));
            f.addAttribute("boolean", l[lvl].g.back());
          }

          l[lvl].r = this->name_gen_.nextName(PRMObject::prm_type::CLASS_ELT);
          f.addReferenceSlot(l[lvl - 1].i, l[lvl].r, true);
        }

        f.endInterface();
      }
    }

    template < typename GUM_SCALAR >
    void ClusteredLayerGenerator< GUM_SCALAR >::_generateClasses_(
       PRMFactory< GUM_SCALAR >&                                              f,
       const std::string&                                                     type,
       std::vector< typename ClusteredLayerGenerator< GUM_SCALAR >::MyData >& l) {
      // double ratio = getClusterRatio() + RAND_MAX;
      Set< std::string > i;

      for (Size lvl = 0; lvl < _layers_.size(); ++lvl) {
        i.insert(l[lvl].i);

        for (Size c = 0; c < _layers_[lvl].c; ++c) {
          // if (std::rand() < ratio)
          _generateCluster_(f, type, l, lvl, i);
          // else
          //   _generateClass_(f, type, l, lvl, i);
        }

        i.erase(l[lvl].i);
      }
    }

    template < typename GUM_SCALAR >
    void ClusteredLayerGenerator< GUM_SCALAR >::_generateCluster_(
       PRMFactory< GUM_SCALAR >&                                              f,
       const std::string&                                                     type,
       std::vector< typename ClusteredLayerGenerator< GUM_SCALAR >::MyData >& l,
       Size                                                                   lvl,
       Set< std::string >&                                                    i) {
      Size                        size = 0;
      GUM_SCALAR                  sum  = 0.0;
      std::string                 first, second, third;
      std::vector< std::string >* v = 0;

      switch (randomValue(2)) {
        // Shape A->B
        // v == [first, second, second.ref -> first]
        case 0: {
          v = new std::vector< std::string >();
          _generateClass_(f, type, l, lvl, i);
          first = l[lvl].c.back();
          v->push_back(first);
          v->push_back(this->name_gen_.nextName(PRMObject::prm_type::CLASS));
          f.startClass(v->back());
          v->push_back(this->name_gen_.nextName(PRMObject::prm_type::CLASS_ELT));
          f.addReferenceSlot(first, v->back(), true);
          DAG                              dag;
          Bijection< std::string, NodeId > names;
          _generateClassDag_(lvl, dag, names, l);

          // Adding aggregates
          for (std::vector< std::string >::iterator g = l[lvl].g.begin(); g != l[lvl].g.end();
               ++g) {
            std::stringstream s;
            s << v->back() << "." << l[lvl].a[randomValue(l[lvl].a.size())];
            std::vector< std::string > chain(1, s.str()), param(1, "1");
            f.addAggregator(*g, "exists", chain, param);
          }

          // Adding attributes
          for (std::vector< std::string >::iterator a = l[lvl].a.begin(); a != l[lvl].a.end();
               ++a) {
            f.startAttribute(type, *a, true);
            size = getDomainSize();

            for (const auto par: dag.parents(names.second(*a))) {
              f.addParent(names.first(par));
              size *= f.retrieveClass(l[lvl].c.back()).get(names.first(par)).type()->domainSize();
            }

            std::vector< GUM_SCALAR > cpf(size), val(getDomainSize());

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
          v = new std::vector< std::string >();
          _generateClass_(f, type, l, lvl, i);
          {
            first = l[lvl].c.back();
            v->push_back(first);
            v->push_back(this->name_gen_.nextName(PRMObject::prm_type::CLASS));
            second = v->back();
            f.startClass(second);
            v->push_back(this->name_gen_.nextName(PRMObject::prm_type::CLASS_ELT));
            f.addReferenceSlot(first, v->back(), true);
            DAG                              dag;
            Bijection< std::string, NodeId > names;
            _generateClassDag_(lvl, dag, names, l);

            // Adding aggregates
            for (std::vector< std::string >::iterator g = l[lvl].g.begin(); g != l[lvl].g.end();
                 ++g) {
              std::stringstream s;
              s << v->back() << "." << l[lvl].a[randomValue(l[lvl].a.size())];
              std::vector< std::string > chain(1, s.str()), param(1, "1");
              f.addAggregator(*g, "exists", chain, param);
            }

            // Adding attributes
            for (std::vector< std::string >::iterator a = l[lvl].a.begin(); a != l[lvl].a.end();
                 ++a) {
              f.startAttribute(type, *a, true);
              size = getDomainSize();

              for (const auto par: dag.parents(names.second(*a))) {
                f.addParent(names.first(par));
                size *= f.retrieveClass(l[lvl].c.back()).get(names.first(par)).type()->domainSize();
              }

              std::vector< GUM_SCALAR > cpf(size), val(getDomainSize());

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
            v->push_back(this->name_gen_.nextName(PRMObject::prm_type::CLASS));
            third = v->back();
            f.startClass(third);
            v->push_back(this->name_gen_.nextName(PRMObject::prm_type::CLASS_ELT));
            f.addReferenceSlot(second, v->back(), true);
            DAG                              dag;
            Bijection< std::string, NodeId > names;
            _generateClassDag_(lvl, dag, names, l);

            // Adding aggregates
            for (std::vector< std::string >::iterator g = l[lvl].g.begin(); g != l[lvl].g.end();
                 ++g) {
              std::stringstream s;
              s << v->back() << "." << l[lvl].a[randomValue(l[lvl].a.size())];
              std::vector< std::string > chain(1, s.str()), param(1, "1");
              f.addAggregator(*g, "exists", chain, param);
            }

            // Adding attributes
            for (std::vector< std::string >::iterator a = l[lvl].a.begin(); a != l[lvl].a.end();
                 ++a) {
              f.startAttribute(type, *a, true);
              size = getDomainSize();

              for (const auto par: dag.parents(names.second(*a))) {
                f.addParent(names.first(par));
                size *= f.retrieveClass(l[lvl].c.back()).get(names.first(par)).type()->domainSize();
              }

              std::vector< GUM_SCALAR > cpf(size), val(getDomainSize());

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

        default: {
          GUM_ERROR(OperationNotAllowed, "unexpected value")
        }
      }

      _cluster_map_.insert(first, v);
    }

    template < typename GUM_SCALAR >
    void ClusteredLayerGenerator< GUM_SCALAR >::_generateClass_(
       PRMFactory< GUM_SCALAR >&                                              f,
       const std::string&                                                     type,
       std::vector< typename ClusteredLayerGenerator< GUM_SCALAR >::MyData >& l,
       Size                                                                   lvl,
       Set< std::string >&                                                    i) {
      Size       size = 0;
      GUM_SCALAR sum  = 0.0;
      l[lvl].c.push_back(this->name_gen_.nextName(PRMObject::prm_type::CLASS));
      f.startClass(l[lvl].c.back(), "", &i);

      if (lvl) f.addReferenceSlot(l[lvl - 1].i, l[lvl].r, true);

      DAG                              dag;
      Bijection< std::string, NodeId > names;
      _generateClassDag_(lvl, dag, names, l);

      // Adding aggregates
      if (lvl) {
        for (const auto& agg: l[lvl].g) {
          std::stringstream s;
          s << l[lvl].r << "." << l[lvl - 1].a[randomValue(l[lvl - 1].a.size())];
          std::vector< std::string > chain(1, s.str()), param(1, "1");
          f.addAggregator(agg, "exists", chain, param);
        }
      }

      // Adding attributes
      for (const auto& attr: l[lvl].a) {
        f.startAttribute(type, attr, true);
        size = getDomainSize();

        for (const auto par: dag.parents(names.second(attr))) {
          f.addParent(names.first(par));
          size *= f.retrieveClass(l[lvl].c.back()).get(names.first(par)).type()->domainSize();
        }

        std::vector< GUM_SCALAR > cpf(size), val(getDomainSize());

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

    template < typename GUM_SCALAR >
    void ClusteredLayerGenerator< GUM_SCALAR >::_generateClassDag_(
       Size                                                                   lvl,
       DAG&                                                                   dag,
       Bijection< std::string, NodeId >&                                      names,
       std::vector< typename ClusteredLayerGenerator< GUM_SCALAR >::MyData >& l) {
      float                 density = _layers_[lvl].inner_density;
      std::vector< NodeId > nodes;
      NodeId                id = 0;

      if (lvl) {
        for (std::vector< std::string >::iterator g = l[lvl].g.begin(); g != l[lvl].g.end(); ++g) {
          id = dag.addNode();
          names.insert(*g, id);
          nodes.push_back(id);
        }
      }

      for (std::vector< std::string >::iterator a = l[lvl].a.begin(); a != l[lvl].a.end(); ++a) {
        id = dag.addNode();
        names.insert(*a, id);

        for (std::vector< NodeId >::iterator prnt = nodes.begin(); prnt != nodes.end(); ++prnt)
          if (randomProba() < density) dag.addArc(*prnt, names.second(*a));

        nodes.push_back(id);
      }

      // For each nodes with #parents >  _max_parents_ we randomly remove parents
      // until
      // #parents <=  _max_parents_
      for (const auto node: dag.nodes()) {
        if (dag.parents(node).size() > getMaxParents()) {
          std::vector< NodeId > v;

          for (const auto par: dag.parents(node))
            v.push_back(par);

          while (dag.parents(node).size() > getMaxParents()) {
            size_t idx = randomValue(v.size());
            Arc    arc(v[idx], node);
            GUM_ASSERT(dag.existsArc(arc));
            dag.eraseArc(arc);
            v[idx] = v.back();
            v.pop_back();
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    void ClusteredLayerGenerator< GUM_SCALAR >::_generateSystem_(
       PRMFactory< GUM_SCALAR >&                                              factory,
       std::vector< typename ClusteredLayerGenerator< GUM_SCALAR >::MyData >& l) {
      factory.startSystem(this->name_gen_.nextName(PRMObject::prm_type::SYSTEM));
      std::vector< std::vector< std::string > > o(_layers_.size());
      std::string                               name, c, first, second, third;
      std::vector< std::string >*               v   = 0;
      size_t                                    idx = 0;

      for (size_t lvl = 0; lvl < _layers_.size(); ++lvl) {
        float density = _layers_[lvl].outter_density;

        for (size_t count = 0; count < _layers_[lvl].o; ++count) {
          c = l[lvl].c[randomValue(l[lvl].c.size())];

          if (_cluster_map_.exists(c)) {
            v = _cluster_map_[c];

            switch (v->size()) {
              case 3: {
                first = this->name_gen_.nextName(PRMObject::prm_type::INSTANCE);
                factory.addInstance(c, first);
                second = this->name_gen_.nextName(PRMObject::prm_type::INSTANCE);
                factory.addInstance(v->at(1), second);
                std::stringstream chain;
                chain << second << "." << v->at(2);
                factory.setReferenceSlot(chain.str(), first);
                break;
              }

              case 5: {
                first = this->name_gen_.nextName(PRMObject::prm_type::INSTANCE);
                factory.addInstance(c, first);
                second = this->name_gen_.nextName(PRMObject::prm_type::INSTANCE);
                factory.addInstance(v->at(1), second);
                std::stringstream chain_1, chain_2;
                chain_1 << second << "." << v->at(2);
                factory.setReferenceSlot(chain_1.str(), first);
                third = this->name_gen_.nextName(PRMObject::prm_type::INSTANCE);
                factory.addInstance(v->at(3), third);
                chain_2 << third << "." << v->at(4);
                factory.setReferenceSlot(chain_2.str(), second);
                break;
              }

              default: {
                GUM_ERROR(OperationNotAllowed, "unexpected vector size")
              }
            }

            //  _cluster_map_.erase(c);
            // delete v;
            name = first;
          } else {
            name = this->name_gen_.nextName(PRMObject::prm_type::INSTANCE);
            factory.addInstance(c, name);
          }

          o[lvl].push_back(name);

          if (lvl) {
            std::stringstream chain;
            chain << name << "." << l[lvl].r;
            std::vector< std::string > ref2add;

            for (std::vector< std::string >::iterator iter = o[lvl - 1].begin();
                 iter != o[lvl - 1].end();
                 ++iter)
              if (randomProba() <= density) ref2add.push_back(*iter);

            if (ref2add.empty())
              factory.setReferenceSlot(chain.str(), o[lvl - 1][randomValue(o[lvl - 1].size())]);

            while (ref2add.size() > getMaxParents()) {
              idx          = randomValue(ref2add.size());
              ref2add[idx] = ref2add.back();
              ref2add.pop_back();
            }

            for (std::vector< std::string >::iterator iter = ref2add.begin(); iter != ref2add.end();
                 ++iter)
              factory.setReferenceSlot(chain.str(), *iter);
          }
        }
      }

      factory.endSystem();
    }

    template < typename GUM_SCALAR >
    INLINE ClusteredLayerGenerator< GUM_SCALAR >::ClusteredLayerGenerator() :
        _layers_(), _domain_size_(2), _max_parents_(INT_MAX), _cluster_ratio_(0.0) {
      GUM_CONSTRUCTOR(ClusteredLayerGenerator);
    }

    template < typename GUM_SCALAR >
    INLINE ClusteredLayerGenerator< GUM_SCALAR >::ClusteredLayerGenerator(
       const ClusteredLayerGenerator< GUM_SCALAR >& source) :
        _layers_(source._layers_),
        _domain_size_(source._domain_size_), _max_parents_(source._max_parents_) {
      GUM_CONS_CPY(ClusteredLayerGenerator);
    }

    template < typename GUM_SCALAR >
    INLINE ClusteredLayerGenerator< GUM_SCALAR >::~ClusteredLayerGenerator() {
      GUM_DESTRUCTOR(ClusteredLayerGenerator);
    }

    template < typename GUM_SCALAR >
    INLINE ClusteredLayerGenerator< GUM_SCALAR >& ClusteredLayerGenerator< GUM_SCALAR >::operator=(
       const ClusteredLayerGenerator< GUM_SCALAR >& source) {
      _layers_      = source._layers_;
      _domain_size_ = source._domain_size_;
      _max_parents_ = source._max_parents_;
      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE Size ClusteredLayerGenerator< GUM_SCALAR >::getDomainSize() const {
      return _domain_size_;
    }

    template < typename GUM_SCALAR >
    INLINE void ClusteredLayerGenerator< GUM_SCALAR >::setDomainSize(Size s) {
      _domain_size_ = s;
    }

    template < typename GUM_SCALAR >
    INLINE Size ClusteredLayerGenerator< GUM_SCALAR >::getMaxParents() const {
      return _max_parents_;
    }

    template < typename GUM_SCALAR >
    INLINE void ClusteredLayerGenerator< GUM_SCALAR >::setMaxParents(Size s) {
      _max_parents_ = s;
    }

    template < typename GUM_SCALAR >
    INLINE void ClusteredLayerGenerator< GUM_SCALAR >::setLayers(
       const std::vector< typename LayerGenerator< GUM_SCALAR >::LayerData >& v) {
      _layers_ = v;
    }

    template < typename GUM_SCALAR >
    INLINE std::vector< typename LayerGenerator< GUM_SCALAR >::LayerData >&
           ClusteredLayerGenerator< GUM_SCALAR >::getLayer() {
      return _layers_;
    }

    template < typename GUM_SCALAR >
    INLINE const std::vector< typename LayerGenerator< GUM_SCALAR >::LayerData >&
                 ClusteredLayerGenerator< GUM_SCALAR >::getLayer() const {
      return _layers_;
    }

    template < typename GUM_SCALAR >
    INLINE double ClusteredLayerGenerator< GUM_SCALAR >::getClusterRatio() const {
      return _cluster_ratio_;
    }

    template < typename GUM_SCALAR >
    INLINE void ClusteredLayerGenerator< GUM_SCALAR >::setClusterRatio(double ratio) {
      _cluster_ratio_ = ratio;
    }

  } /* namespace prm */
} /* namespace gum */
