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
 * @brief Inline implementation of the SearchStrategy class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/PRM/gspan/searchStrategy.h>

namespace gum {
  namespace prm {
    namespace gspan {

      template < typename GUM_SCALAR >
      double SearchStrategy< GUM_SCALAR >::computeCost_(const Pattern& p) {
        double                                        cost = 0;
        const Sequence< PRMInstance< GUM_SCALAR >* >& seq
           = *(this->tree_->data(p).iso_map.begin().val());
        Sequence< PRMClassElement< GUM_SCALAR >* > input_set;

        for (const auto inst: seq) {
          for (const auto input: inst->type().slotChains())
            for (const auto inst2: inst->getInstances(input->id()))
              if ((!seq.exists(inst2))
                  && (!input_set.exists(&(inst2->get(input->lastElt().safeName()))))) {
                cost += std::log(input->type().variable().domainSize());
                input_set.insert(&(inst2->get(input->lastElt().safeName())));
              }

          for (auto vec = inst->beginInvRef(); vec != inst->endInvRef(); ++vec)
            for (const auto& inverse: *vec.val())
              if (!seq.exists(inverse.first)) {
                cost += std::log(inst->get(vec.key()).type().variable().domainSize());
                break;
              }
        }

        return cost;
      }

      template < typename GUM_SCALAR >
      void StrictSearch< GUM_SCALAR >::_buildPatternGraph_(
         typename StrictSearch< GUM_SCALAR >::PData&   data,
         Set< Potential< GUM_SCALAR >* >&              pool,
         const Sequence< PRMInstance< GUM_SCALAR >* >& match) {
        for (const auto inst: match) {
          for (const auto& elt: *inst) {
            // Adding the node
            NodeId id = data.graph.addNode();
            data.node2attr.insert(id, _str_(inst, elt.second));
            data.mod.insert(id, elt.second->type()->domainSize());
            data.vars.insert(id, &elt.second->type().variable());
            pool.insert(const_cast< Potential< GUM_SCALAR >* >(&(elt.second->cpf())));
          }
        }

        // Second we add edges and nodes to inners or outputs
        for (const auto inst: match)
          for (const auto& elt: *inst) {
            NodeId node  = data.node2attr.first(_str_(inst, elt.second));
            bool   found = false;   // If this is set at true, then node is an outer node

            // Children existing in the instance type's DAG
            for (const auto chld: inst->type().containerDag().children(elt.second->id())) {
              data.graph.addEdge(node, data.node2attr.first(_str_(inst, inst->get(chld))));
            }

            // Parents existing in the instance type's DAG
            for (const auto par: inst->type().containerDag().parents(elt.second->id())) {
              switch (inst->type().get(par).elt_type()) {
                case PRMClassElement< GUM_SCALAR >::prm_attribute:
                case PRMClassElement< GUM_SCALAR >::prm_aggregate: {
                  data.graph.addEdge(node, data.node2attr.first(_str_(inst, inst->get(par))));
                  break;
                }

                case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
                  for (const auto inst2: inst->getInstances(par))
                    if (match.exists(inst2))
                      data.graph.addEdge(node,
                                         data.node2attr.first(
                                            _str_(inst2,
                                                  static_cast< const PRMSlotChain< GUM_SCALAR >& >(
                                                     inst->type().get(par)))));

                  break;
                }

                default: { /* Do nothing */
                }
              }
            }

            // Referring PRMAttribute<GUM_SCALAR>
            if (inst->hasRefAttr(elt.second->id())) {
              const std::vector< std::pair< PRMInstance< GUM_SCALAR >*, std::string > >& ref_attr
                 = inst->getRefAttr(elt.second->id());

              for (auto pair = ref_attr.begin(); pair != ref_attr.end(); ++pair) {
                if (match.exists(pair->first)) {
                  NodeId id = pair->first->type().get(pair->second).id();

                  for (const auto child: pair->first->type().containerDag().children(id))
                    data.graph.addEdge(
                       node,
                       data.node2attr.first(_str_(pair->first, pair->first->get(child))));
                } else {
                  found = true;
                }
              }
            }

            if (found) data.outputs.insert(node);
            else data.inners.insert(node);
          }
      }

      template < typename GUM_SCALAR >
      std::pair< Size, Size > StrictSearch< GUM_SCALAR >::_elimination_cost_(
         typename StrictSearch< GUM_SCALAR >::PData& data,
         Set< Potential< GUM_SCALAR >* >&            pool) {
        List< NodeSet > partial_order;

        if (data.inners.size()) partial_order.insert(data.inners);

        if (data.outputs.size()) partial_order.insert(data.outputs);

        PartialOrderedTriangulation     t(&(data.graph), &(data.mod), &partial_order);
        const std::vector< NodeId >&    elim_order = t.eliminationOrder();
        Size                            max(0), max_count(1);
        Set< Potential< GUM_SCALAR >* > trash;
        Potential< GUM_SCALAR >*        pot = 0;

        for (size_t idx = 0; idx < data.inners.size(); ++idx) {
          pot = new Potential< GUM_SCALAR >(new MultiDimSparse< GUM_SCALAR >(0));
          pot->add(*(data.vars.second(elim_order[idx])));
          trash.insert(pot);
          Set< Potential< GUM_SCALAR >* > toRemove;

          for (const auto p: pool)
            if (p->contains(*(data.vars.second(elim_order[idx])))) {
              for (auto var = p->variablesSequence().begin(); var != p->variablesSequence().end();
                   ++var) {
                try {
                  pot->add(**var);
                } catch (DuplicateElement const&) {}
              }

              toRemove.insert(p);
            }

          if (pot->domainSize() > max) {
            max       = pot->domainSize();
            max_count = 1;
          } else if (pot->domainSize() == max) {
            ++max_count;
          }

          for (const auto p: toRemove)
            pool.erase(p);

          pot->erase(*(data.vars.second(elim_order[idx])));
        }

        for (const auto pot: trash)
          delete pot;

        return std::make_pair(max, max_count);
      }

      // The SearchStrategy class
      template < typename GUM_SCALAR >
      INLINE SearchStrategy< GUM_SCALAR >::SearchStrategy() : tree_(0) {
        GUM_CONSTRUCTOR(SearchStrategy);
      }

      template < typename GUM_SCALAR >
      INLINE
         SearchStrategy< GUM_SCALAR >::SearchStrategy(const SearchStrategy< GUM_SCALAR >& from) :
          tree_(from.tree_) {
        GUM_CONS_CPY(SearchStrategy);
      }

      template < typename GUM_SCALAR >
      INLINE SearchStrategy< GUM_SCALAR >::~SearchStrategy() {
        GUM_DESTRUCTOR(SearchStrategy);
      }

      template < typename GUM_SCALAR >
      INLINE SearchStrategy< GUM_SCALAR >&
         SearchStrategy< GUM_SCALAR >::operator=(const SearchStrategy< GUM_SCALAR >& from) {
        this->tree_ = from.tree_;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE void SearchStrategy< GUM_SCALAR >::setTree(DFSTree< GUM_SCALAR >* tree) {
        this->tree_ = tree;
      }

      // FrequenceSearch

      // The FrequenceSearch class
      template < typename GUM_SCALAR >
      INLINE FrequenceSearch< GUM_SCALAR >::FrequenceSearch(Size freq) :
          SearchStrategy< GUM_SCALAR >(), _freq_(freq) {
        GUM_CONSTRUCTOR(FrequenceSearch);
      }

      template < typename GUM_SCALAR >
      INLINE
         FrequenceSearch< GUM_SCALAR >::FrequenceSearch(const FrequenceSearch< GUM_SCALAR >& from) :
          SearchStrategy< GUM_SCALAR >(from),
          _freq_(from._freq_) {
        GUM_CONS_CPY(FrequenceSearch);
      }

      template < typename GUM_SCALAR >
      INLINE FrequenceSearch< GUM_SCALAR >::~FrequenceSearch() {
        GUM_DESTRUCTOR(FrequenceSearch);
      }

      template < typename GUM_SCALAR >
      INLINE FrequenceSearch< GUM_SCALAR >&
         FrequenceSearch< GUM_SCALAR >::operator=(const FrequenceSearch< GUM_SCALAR >& from) {
        _freq_ = from._freq_;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE bool FrequenceSearch< GUM_SCALAR >::accept_root(const Pattern* r) {
        return this->tree_->frequency(*r) >= _freq_;
      }

      template < typename GUM_SCALAR >
      INLINE bool
         FrequenceSearch< GUM_SCALAR >::accept_growth(const Pattern*                  parent,
                                                      const Pattern*                  child,
                                                      const EdgeGrowth< GUM_SCALAR >& growh) {
        return this->tree_->frequency(*child) >= _freq_;
      }

      template < typename GUM_SCALAR >
      INLINE bool FrequenceSearch< GUM_SCALAR >::operator()(gspan::Pattern* i, gspan::Pattern* j) {
        // We want a descending order
        return this->tree_->frequency(*i) > this->tree_->frequency(*j);
      }

      template < typename GUM_SCALAR >
      INLINE bool FrequenceSearch< GUM_SCALAR >::operator()(LabelData* i, LabelData* j) {
        return (this->tree_->graph().size(i) > this->tree_->graph().size(j));
      }

      // StrictSearch

      // The StrictSearch class
      template < typename GUM_SCALAR >
      INLINE StrictSearch< GUM_SCALAR >::StrictSearch(Size freq) :
          SearchStrategy< GUM_SCALAR >(), _freq_(freq), _dot_(".") {
        GUM_CONSTRUCTOR(StrictSearch);
      }

      template < typename GUM_SCALAR >
      INLINE StrictSearch< GUM_SCALAR >::StrictSearch(const StrictSearch< GUM_SCALAR >& from) :
          SearchStrategy< GUM_SCALAR >(from), _freq_(from._freq_) {
        GUM_CONS_CPY(StrictSearch);
      }

      template < typename GUM_SCALAR >
      INLINE StrictSearch< GUM_SCALAR >::~StrictSearch() {
        GUM_DESTRUCTOR(StrictSearch);
      }

      template < typename GUM_SCALAR >
      INLINE StrictSearch< GUM_SCALAR >&
         StrictSearch< GUM_SCALAR >::operator=(const StrictSearch< GUM_SCALAR >& from) {
        _freq_ = from._freq_;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE bool StrictSearch< GUM_SCALAR >::accept_root(const Pattern* r) {
        return (this->tree_->frequency(*r) >= _freq_);
      }

      template < typename GUM_SCALAR >
      INLINE bool
         StrictSearch< GUM_SCALAR >::accept_growth(const Pattern*                  parent,
                                                   const Pattern*                  child,
                                                   const EdgeGrowth< GUM_SCALAR >& growth) {
        return _inner_cost_(child) + this->tree_->frequency(*child) * _outer_cost_(child)
             < this->tree_->frequency(*child) * _outer_cost_(parent);
      }

      template < typename GUM_SCALAR >
      INLINE bool StrictSearch< GUM_SCALAR >::operator()(gspan::Pattern* i, gspan::Pattern* j) {
        return _inner_cost_(i) + this->tree_->frequency(*i) * _outer_cost_(i)
             < _inner_cost_(j) + this->tree_->frequency(*j) * _outer_cost_(j);
      }

      template < typename GUM_SCALAR >
      INLINE bool StrictSearch< GUM_SCALAR >::operator()(LabelData* i, LabelData* j) {
        return i->tree_width * this->tree_->graph().size(i)
             < j->tree_width * this->tree_->graph().size(j);
      }

      template < typename GUM_SCALAR >
      INLINE double StrictSearch< GUM_SCALAR >::_inner_cost_(const Pattern* p) {
        try {
          return _map_[p].first;
        } catch (NotFound const&) {
          _compute_costs_(p);
          return _map_[p].first;
        }
      }

      template < typename GUM_SCALAR >
      INLINE double StrictSearch< GUM_SCALAR >::_outer_cost_(const Pattern* p) {
        try {
          return _map_[p].second;
        } catch (NotFound const&) {
          _compute_costs_(p);
          return _map_[p].second;
        }
      }

      template < typename GUM_SCALAR >
      INLINE std::string
             StrictSearch< GUM_SCALAR >::_str_(const PRMInstance< GUM_SCALAR >*  i,
                                           const PRMAttribute< GUM_SCALAR >* a) const {
        return i->name() + _dot_ + a->safeName();
      }

      template < typename GUM_SCALAR >
      INLINE std::string
             StrictSearch< GUM_SCALAR >::_str_(const PRMInstance< GUM_SCALAR >*  i,
                                           const PRMAttribute< GUM_SCALAR >& a) const {
        return i->name() + _dot_ + a.safeName();
      }

      template < typename GUM_SCALAR >
      INLINE std::string
             StrictSearch< GUM_SCALAR >::_str_(const PRMInstance< GUM_SCALAR >*  i,
                                           const PRMSlotChain< GUM_SCALAR >& a) const {
        return i->name() + _dot_ + a.lastElt().safeName();
      }

      template < typename GUM_SCALAR >
      INLINE void StrictSearch< GUM_SCALAR >::_compute_costs_(const Pattern* p) {
        typename StrictSearch< GUM_SCALAR >::PData data;
        Set< Potential< GUM_SCALAR >* >            pool;
        _buildPatternGraph_(data, pool, *(this->tree_->data(*p).iso_map.begin().val()));
        double inner = std::log(_elimination_cost_(data, pool).first);
        double outer = this->computeCost_(*p);
        _map_.insert(p, std::make_pair(inner, outer));
      }

      // TreeWidthSearch

      template < typename GUM_SCALAR >
      INLINE TreeWidthSearch< GUM_SCALAR >::TreeWidthSearch() : SearchStrategy< GUM_SCALAR >() {
        GUM_CONSTRUCTOR(TreeWidthSearch);
      }

      template < typename GUM_SCALAR >
      INLINE
         TreeWidthSearch< GUM_SCALAR >::TreeWidthSearch(const TreeWidthSearch< GUM_SCALAR >& from) :
          SearchStrategy< GUM_SCALAR >(from) {
        GUM_CONS_CPY(TreeWidthSearch);
      }

      template < typename GUM_SCALAR >
      INLINE TreeWidthSearch< GUM_SCALAR >::~TreeWidthSearch() {
        GUM_DESTRUCTOR(TreeWidthSearch);
      }

      template < typename GUM_SCALAR >
      INLINE TreeWidthSearch< GUM_SCALAR >&
         TreeWidthSearch< GUM_SCALAR >::operator=(const TreeWidthSearch< GUM_SCALAR >& from) {
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE double TreeWidthSearch< GUM_SCALAR >::cost(const Pattern& p) {
        try {
          return _map_[&p];
        } catch (NotFound const&) {
          _map_.insert(&p, this->computeCost_(p));
          return _map_[&p];
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool TreeWidthSearch< GUM_SCALAR >::accept_root(const Pattern* r) {
        Size tree_width = 0;

        for (const auto n: r->nodes())
          tree_width += r->label(n).tree_width;

        return tree_width >= cost(*r);
      }

      template < typename GUM_SCALAR >
      INLINE bool
         TreeWidthSearch< GUM_SCALAR >::accept_growth(const Pattern*                  parent,
                                                      const Pattern*                  child,
                                                      const EdgeGrowth< GUM_SCALAR >& growth) {
        return cost(*parent) >= cost(*child);
      }

      template < typename GUM_SCALAR >
      INLINE bool TreeWidthSearch< GUM_SCALAR >::operator()(gspan::Pattern* i, gspan::Pattern* j) {
        return cost(*i) < cost(*j);
      }

      template < typename GUM_SCALAR >
      INLINE bool TreeWidthSearch< GUM_SCALAR >::operator()(LabelData* i, LabelData* j) {
        return i->tree_width < j->tree_width;
      }

    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */
