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
 * @brief Implementation of SVED.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/SVED.h>
// ============================================================================
#ifdef GUM_NO_INLINE
#include <agrum/prm/SVED.inl>
#endif // GUM_NO_INLINE
// ============================================================================
namespace gum {
  namespace prm {

    SVED::~SVED() {
      GUM_DESTRUCTOR( SVED );
      typedef HashTable<const Class*, std::vector<NodeId>*>::iterator ElimIter;
      for ( ElimIter i = __elim_orders.begin(); i != __elim_orders.end(); ++i ) {
        delete *i;
      }
      if ( __class_elim_order != 0 ) {
        delete __class_elim_order;
      }
    }

    void
    SVED::__eliminateNodes( const Instance* query, NodeId node, BucketSet& pool, BucketSet& trash ) {
      Set<const Instance*> ignore;
      ignore.insert( query );
      // Extracting required attributes and slotchains
      Set<NodeId>& attr_set = __getAttrSet( query );
      Set<NodeId>& sc_set = __getSCSet( query );
      // Downward elimination
      List<const Instance*> elim_list;
      for ( Set<NodeId>::iterator attr = attr_set.begin(); attr != attr_set.end(); ++attr ) {
        try {
          typedef std::vector< std::pair<Instance*, std::string> >::const_iterator Iter;
          for ( Iter iter = query->getRefAttr( *attr ).begin(); iter != query->getRefAttr( *attr ).end(); ++iter )
            if (( not ignore.exists( iter->first ) ) and( __bb.exists( iter->first ) ) )
              __eliminateNodesDownward( query, iter->first, pool, trash, elim_list, ignore );
        } catch ( NotFound& ) {
          // Ok
        }
      }
      // Eliminating all nodes in query instance, except query
      InstanceBayesNet bn( *query );
      DefaultTriangulation t( &( bn.moralGraph() ), &( bn.modalities() ) );
      std::vector<NodeId> elim_order;
      VariableElimination<prm_float> inf( bn );
      if ( hasEvidence( query ) )
        __insertEvidence( query, pool );
      for ( Set<NodeId>::iterator attr = attr_set.begin(); attr != attr_set.end(); ++attr )
        pool.insert( &( const_cast<Potential<prm_float>&>( query->get( *attr ).cpf() ) ) );
      for ( size_t idx = 0; idx < t.eliminationOrder().size(); ++idx )
        if ( t.eliminationOrder()[idx] != node )
          elim_order.push_back( t.eliminationOrder()[idx] );
      inf.eliminateNodes( elim_order, pool, trash );
      // Eliminating instance in elim_list
      List<const Instance*> tmp_list;
      __reduceElimList( query, elim_list, tmp_list, ignore, pool, trash );
      while ( not elim_list.empty() ) {
        if ( __checkElimOrder( query, elim_list.front() ) ) {
          if (( not ignore.exists( elim_list.front() ) ) and( __bb.exists( elim_list.front() ) ) )
            __eliminateNodesDownward( query, elim_list.front(), pool, trash, elim_list, ignore );
        } else if ( __bb.exists( elim_list.front() ) ) {
          tmp_list.insert( elim_list.front() );
        }
        elim_list.popFront();
      }
      // Upward elimination
      for ( Set<NodeId>::iterator sc = sc_set.begin(); sc != sc_set.end(); ++sc )
        for ( Set<Instance*>::iterator parent = query->getInstances( *sc ).begin(); parent != query->getInstances( *sc ).end(); ++parent )
          if (( not ignore.exists( *parent ) ) and( __bb.exists( *parent ) ) )
            __eliminateNodesUpward( *parent, pool, trash, tmp_list, ignore );
    }

    void
    SVED::__eliminateNodesDownward( const Instance* from, const Instance* i,
                                    BucketSet& pool, BucketSet& trash,
                                    List<const Instance*>& elim_list,
                                    Set<const Instance*>& ignore ) {
      ignore.insert( i );
      // Extracting required attributes and slotchains
      Set<NodeId>& attr_set = __getAttrSet( i );
      Set<NodeId>& sc_set   = __getSCSet( i );
      // Calling elimination over child instance
      List<const Instance*> my_list;
      for ( Set<NodeId>::iterator attr = attr_set.begin(); attr != attr_set.end(); ++attr ) {
        try {
          typedef std::vector< std::pair<Instance*, std::string> >::const_iterator Iter;
          for ( Iter iter = i->getRefAttr( *attr ).begin(); iter != i->getRefAttr( *attr ).end(); ++iter )
            if (( not ignore.exists( iter->first ) ) and( __bb.exists( iter->first ) ) )
              __eliminateNodesDownward( i, iter->first, pool, trash, my_list, ignore );
        } catch ( NotFound& ) {
          // Ok
        }
      }
      // Eliminating all nodes in current instance
      if ( hasEvidence( i ) ) {
        __eliminateNodesWithEvidence( i, pool, trash );
      } else {
        __insertLiftedNodes( i, pool, trash );
        for ( Set<Aggregate*>::iterator agg = i->type().aggregates().begin(); agg != i->type().aggregates().end(); ++agg )
          if ( __bb.requisiteNodes( i ).exists(( **agg ).id() ) )
            pool.insert( __getAggPotential( i, *agg ) );
        try {
          InstanceBayesNet bn( *i );
          VariableElimination<prm_float> inf( bn );
          inf.eliminateNodes( __getElimOrder( i->type() ), pool, trash );
        } catch ( NotFound& ) {
          // Raised if there is no inner nodes to eliminate
        }
      }
      // Calling elimination over child's parents
      while ( not my_list.empty() ) {
        if ( __checkElimOrder( i, my_list.front() ) ) {
          if (( not ignore.exists( my_list.front() ) ) and( __bb.exists( my_list.front() ) ) )
            __eliminateNodesDownward( i, my_list.front(), pool, trash, my_list, ignore );
        } else if ( __bb.exists( my_list.front() ) ) {
          elim_list.insert( my_list.front() );
        }
        my_list.popFront();
      }
      // Adding parents instance to elim_list
      for ( Set<NodeId>::iterator sc = sc_set.begin(); sc != sc_set.end(); ++sc )
        for ( Set<Instance*>::iterator parent = i->getInstances( *sc ).begin(); parent != i->getInstances( *sc ).end(); ++parent )
          if (( not ignore.exists( *parent ) ) and __bb.exists( *parent ) and( *parent != from ) )
            elim_list.insert( *parent );
    }

    void
    SVED::__eliminateNodesUpward( const Instance* i,
                                  BucketSet& pool, BucketSet& trash,
                                  List<const Instance*>& elim_list,
                                  Set<const Instance*>& ignore ) {
      ignore.insert( i );
      // Extracting required attributes and slotchains
      Set<NodeId>& attr_set = __getAttrSet( i );
      Set<NodeId>& sc_set   = __getSCSet( i );
      // Downward elimination
      for ( Set<NodeId>::iterator attr = attr_set.begin(); attr != attr_set.end(); ++attr ) {
        try {
          typedef std::vector< std::pair<Instance*, std::string> >::const_iterator Iter;
          for ( Iter iter = i->getRefAttr( *attr ).begin(); iter != i->getRefAttr( *attr ).end(); ++iter )
            if (( not ignore.exists( iter->first ) ) and( __bb.exists( iter->first ) ) )
              __eliminateNodesDownward( i, iter->first, pool, trash, elim_list, ignore );
        } catch ( NotFound& ) {
          // Ok
        }
      }
      // Eliminating all nodes in i instance
      if ( hasEvidence( i ) ) {
        __eliminateNodesWithEvidence( i, pool, trash );
      } else {
        __insertLiftedNodes( i, pool, trash );
        for ( Set<Aggregate*>::iterator agg = i->type().aggregates().begin(); agg != i->type().aggregates().end(); ++agg )
          if ( __bb.requisiteNodes( i ).exists(( **agg ).id() ) )
            pool.insert( __getAggPotential( i, *agg ) );
        try {
          InstanceBayesNet bn( *i );
          VariableElimination<prm_float> inf( bn );
          inf.eliminateNodes( __getElimOrder( i->type() ), pool, trash );
        } catch ( NotFound& ) {
          // Raised if there is no inner nodes to eliminate
        }
      }
      // Eliminating instance in elim_list
      List<const Instance*> tmp_list;
      while ( not elim_list.empty() ) {
        if ( __checkElimOrder( i, elim_list.front() ) ) {
          if (( not ignore.exists( elim_list.front() ) ) and( __bb.exists( elim_list.front() ) ) )
            __eliminateNodesDownward( i, elim_list.front(), pool, trash, elim_list, ignore );
        } else if ( __bb.exists( elim_list.front() ) ) {
          ignore.insert( elim_list.front() );
        }
        elim_list.popFront();
      }
      // Upward elimination
      for ( Set<NodeId>::iterator sc = sc_set.begin(); sc != sc_set.end(); ++sc )
        for ( Set<Instance*>::iterator parent = i->getInstances( *sc ).begin(); parent != i->getInstances( *sc ).end(); ++parent )
          if (( not ignore.exists( *parent ) ) and( __bb.exists( *parent ) ) )
            __eliminateNodesUpward( *parent, pool, trash, tmp_list, ignore );
    }

    void
    SVED::__eliminateNodesWithEvidence( const Instance* i, BucketSet& pool, BucketSet& trash ) {
      // Adding required evidences
      for ( EMapIterator e = evidence( i ).begin(); e != evidence( i ).end(); ++e )
        if ( __bb.requisiteNodes( i ).exists( e.key() ) )
          pool.insert( const_cast<Potential<prm_float>*>( *e ) );
      // Adding potentials and eliminating the remaining nodes
      for ( Instance::const_iterator a = i->begin(); a != i->end(); ++a )
        if ( __bb.requisiteNodes( i ).exists( a.key() ) )
          pool.insert( &(( **a ).cpf() ) );
      InstanceBayesNet bn( *i );
      DefaultTriangulation t( &( bn.moralGraph() ), &( bn.modalities() ) );
      const std::vector<NodeId>& full_elim_order = t.eliminationOrder();
      for ( std::vector<NodeId>::const_iterator var = full_elim_order.begin(); var != full_elim_order.end(); ++var )
        eliminateNode( &( i->get( *var ).type().variable() ), pool, trash );
    }

    void
    SVED::__insertLiftedNodes( const Instance* i, BucketSet& pool, BucketSet& trash ) {
      BucketSet* lifted_pool = 0;
      try {
        lifted_pool = __lifted_pools[&( __bb.requisiteNodes( i ) )];
      } catch ( NotFound& ) {
        __initLiftedNodes( i, trash );
        lifted_pool = __lifted_pools[&( __bb.requisiteNodes( i ) )];
      }
      for ( SVED::BucketSet::iterator iter = lifted_pool->begin(); iter != lifted_pool->end(); ++iter ) {
        Potential<prm_float>* pot = copyPotential( i->bijection(), **iter );
        pool.insert( pot );
        trash.insert( pot );
      }
    }

//  void
//  SVED::__initLiftedNodes(const Instance* i, BucketSet& trash)
//  {
//    const Class& c = i->type();
//    BucketSet lifted_pool;
//    __lifted_pools.insert(&(__bb.requisiteNodes(i)), new SVED::ArraySet());
//    for (Set<NodeId>::iterator node = __bb.requisiteNodes(i).begin(); node != __bb.requisiteNodes(i).end(); ++node) {
//      if (c.get(*node).elt_type() == ClassElement::prm_attribute) {
//        lifted_pool.insert(const_cast<Potential<prm_float>*>(&(c.get(*node).cpf())));
//      }
//    }
//    ClassBayesNet bn(c);
//    DefaultTriangulation t(&(bn.moralGraph()), &(bn.modalities()));
//    const std::vector<NodeId>& full_elim_order = t.eliminationOrder();
//    VariableElimination<prm_float> inf(bn);
//    // Removing Output nodes of elimination order
//    std::vector<NodeId> inner_elim_order;
//    std::vector<NodeId>* output_elim_order = new std::vector<NodeId>();
//    for (size_t idx = 0; idx < full_elim_order.size(); ++idx) {
//      if ((not c.isOutputNode(c.get(full_elim_order[idx]))) and (not ClassElement::isAggregate(c.get(full_elim_order[idx])))) {
//        inner_elim_order.push_back(full_elim_order[idx]);
//      } else {
//        output_elim_order->push_back(full_elim_order[idx]);
//      }
//    }
//    // We need to put in the output_elim_order aggregator's parents which are innner nodes
//    for (Set<Aggregate*>::const_iterator agg = i->type().aggregates().begin(); agg != i->type().aggregates().end(); ++agg) {
//      if (__bb.requisiteNodes(i).exists((**agg).id())) {
//        for (Set<NodeId>::const_iterator prnt = i->type().dag().parents((**agg).id()).begin(); prnt != i->type().dag().parents((**agg).id()).end(); ++prnt) {
//          if (ClassElement::isAttribute(i->type().get(*prnt)) and i->type().isInnerNode(i->type().get(*prnt))) {
//        }
//      }
//    }
//    // If there is only output nodes and Aggregate we can't lift anything
//    if (not inner_elim_order.empty()) {
//      inf.eliminateNodes(inner_elim_order, lifted_pool, trash);
//    }
//    // Copying buckets in MultiDimArrays
//    for(SVED::BucketSetIterator iter = lifted_pool.begin(); iter != lifted_pool.end(); ++iter) {
//      const MultiDimBucket<prm_float>* b = 0;
//      const MultiDimImplementation<prm_float>* impl = const_cast<const Potential<prm_float>&>((**iter)).getContent();
//      b = dynamic_cast<const MultiDimBucket<prm_float>* >(impl);
//      if (b != 0) {
//        b->compute(true);
//        try {
//          __lifted_pools[&(__bb.requisiteNodes(i))]->insert(new MultiDimArray<prm_float>(b->bucket()));
//        } catch (OperationNotAllowed&) {
//          // Empty bucket
//        }
//      } else {
//        const MultiDimArray<prm_float>* a = dynamic_cast<const MultiDimArray<prm_float>*>(impl);
//        if (a != 0) {
//          __lifted_pools[&(__bb.requisiteNodes(i))]->insert(new MultiDimArray<prm_float>(*a));
//        } else {
//          GUM_ERROR(FatalError, "unknown content");
//        }
//      }
//    }
//    // If there is not only inner and input Attributes
//    if (not output_elim_order->empty()) {
//      __elim_orders.insert(&c, output_elim_order);
//    } else {
//      delete output_elim_order;
//    }
//  }

    void
    SVED::__initLiftedNodes( const Instance* i, BucketSet& trash ) {
      Class& c = const_cast<Class&>( i->type() );
      BucketSet* lifted_pool = new BucketSet();
      __lifted_pools.insert( &( __bb.requisiteNodes( i ) ), lifted_pool );
      for ( Set<NodeId>::iterator node = __bb.requisiteNodes( i ).begin(); node != __bb.requisiteNodes( i ).end(); ++node )
        if ( ClassElement::isAttribute( c.get( *node ) ) )
          lifted_pool->insert( const_cast<Potential<prm_float>*>( &( c.get( *node ).cpf() ) ) );
      NodeSet inners, outers, ignore;
      const Set<NodeId>* parents = 0;
      for ( Instance::const_iterator node = i->begin(); node != i->end(); ++node ) {
        if ( __bb.requisiteNodes( *i ).exists( node.key() ) ) {
          //if (ClassElement::isAttribute(c.get(node.key())) and c.isInnerNode(c.get(node.key()))) {
          if ( ClassElement::isAttribute( c.get( node.key() ) ) ) {
            if ( c.isOutputNode( c.get( node.key() ) ) )
              outers.insert( node.key() );
            else if ( not outers.exists( node.key() ) )
              inners.insert( node.key() );
          } else if ( ClassElement::isAggregate( c.get( node.key() ) ) ) {
            outers.insert( node.key() );
            // We need to put in the output_elim_order aggregator's parents which are innner nodes
            parents = &( c.dag().parents( node.key() ) );
            for ( Set<NodeId>::const_iterator prnt = parents->begin(); prnt != parents->end(); ++prnt ) {
              if ( ClassElement::isAttribute( i->type().get( *prnt ) ) and i->type().isInnerNode( i->type().get( *prnt ) ) and __bb.requisiteNodes( i ).exists( *prnt ) ) {
                inners.erase( *prnt );
                outers.insert( *prnt );
              }
            }
          }
          // else {
          //   // just an outer attribute
          //   outers.insert(node.key());
          // }
        } else {
          ignore.insert( node.key() );
        }
      }
      // Now we proceed with the elimination of inner attributes
      ClassBayesNet bn( c );
      List<NodeSet> partial_ordering;
      if ( inners.size() )
        partial_ordering.push_back( inners );
      if ( outers.size() )
        partial_ordering.push_back( outers );
      if ( ignore.size() )
        partial_ordering.push_back( ignore );
      GUM_ASSERT( inners.size() or outers.size() );
      PartialOrderedTriangulation t( &( bn.moralGraph() ), &( bn.modalities() ), &partial_ordering );
      for ( size_t idx = 0; idx < inners.size(); ++idx )
        eliminateNode( &( c.get( t.eliminationOrder()[idx] ).type().variable() ), *lifted_pool, trash );
      // If there is not only inner and input Attributes
      if ( outers.size() ) {
        __elim_orders.insert( &c,
                              new std::vector<NodeId>( t.eliminationOrder().begin() + inners.size(), t.eliminationOrder().end() ) );
      }
    }

    void
    SVED::__initElimOrder() {
      CDG cdg( *_prm );
      __class_elim_order = new Sequence<const ClassElementContainer*>();
      std::list<NodeId> l;
      for ( DAG::NodeIterator node = cdg.dag().beginNodes(); node != cdg.dag().endNodes(); ++node )
        if ( cdg.dag().parents( *node ).empty() ) l.push_back( *node );
      Set<NodeId> visited_node;
      while ( not l.empty() ) {
        visited_node.insert( l.front() );
        if ( not __class_elim_order->exists( cdg.get( l.front() ).first ) )
          __class_elim_order->insert( cdg.get( l.front() ).first );
        const NodeSet& children = cdg.dag().children( l.front() );
        for ( NodeSetIterator child = children.begin();
              child != children.end(); ++child )
          if ( not visited_node.contains( *child ) ) l.push_back( *child );
        l.pop_front();
      }
    }

    void
    SVED::_marginal( const Chain& chain, Potential<prm_float>& m ) {
      const Instance* i = chain.first;
      const Attribute* elt = chain.second;
      SVED::BucketSet pool, trash;
      __bb.compute( i, elt->id() );
      __eliminateNodes( i, elt->id(), pool, trash );
      m.fill(( prm_float ) 1 );
      for ( SVED::BucketSetIterator iter = pool.begin(); iter != pool.end(); ++iter ) {
        if (( **iter ).contains( *( m.variablesSequence().atPos( 0 ) ) ) ) {
          m.multiplicateBy( **iter );
        }
        if (( **iter ).nbrDim() > 1 ) {
          std::string dot = ".";
          std::string t = "true";
          std::string f = "false";
          std::string none = "NONE";
          GUM_TRACE_VAR(( **iter ).nbrDim() );
          GUM_TRACE( chain.first->name() + dot + chain.second->safeName() );
          for ( System::const_iterator jter = _sys->begin(); jter != _sys->end(); ++jter ) {
            for ( Instance::iterator a = ( **jter ).begin(); a != ( **jter ).end(); ++a ) {
              if (( **iter ).contains(( **a ).type().variable() ) ) {
                GUM_TRACE(( **jter ).name() + dot + ( **a ).safeName() );
                if ( __bb.exists( *jter ) ) {
                  GUM_TRACE( "should be eliminated" );
                } else {
                  GUM_TRACE( "should not be here" );
                }
                // try {
                //   std::cerr << "Instance eliminated in:       " << __debug[*jter] << std::endl;
                // } catch (NotFound&) {
                //   std::cerr << "Instance eliminated in:       " << none << std::endl;
                // }
                try {
                  std::cerr << "Instance has requisite nodes: " << __bb.requisiteNodes( *jter ).size() << std::endl;
                } catch ( NotFound& ) {
                  std::cerr << "Instance has requisite nodes: " << none << std::endl;
                }
                std::cerr << "Attribute has refered attr:   " << (( **jter ).hasRefAttr(( **a ).id() ) ) << std::endl;
                if (( **jter ).hasRefAttr(( **a ).id() ) ) {
                  bool foo = false;
                  for ( std::vector<std::pair<Instance*, std::string> >::const_iterator ref = ( **jter ).getRefAttr(( **a ).id() ).begin(); ref != ( **jter ).getRefAttr(( **a ).id() ).end(); ++ref ) {
                    if ( __bb.exists( ref->first ) and __bb.requisiteNodes( ref->first ).exists( ref->first->type().get( ref->second ).id() ) ) {
                      foo = true;
                      break;
                    }
                  }
                  if ( foo )
                    std::cerr << "Attribute has required child: " << t << std::endl;
                  else
                    std::cerr << "Attribute has required child: " << f << std::endl;
                }
                try {
                  std::cerr << "Attribute is required:        " << ( __bb.requisiteNodes( *jter ).exists(( **a ).id() ) );
                } catch ( NotFound& ) {
                  std::cerr << "Attribute is required:        " << none;
                }
                std::cerr << std::endl;
                std::cerr << "Attribute has evidence:       " << ( hasEvidence( *jter ) );
                std::cerr << std::endl;
                std::cerr << "Attribute parents number:     " << (( **jter ).type().dag().parents(( **a ).id() ).size() ) << std::endl;
                std::cerr << "Attribute children number:    " << (( **jter ).type().dag().children(( **a ).id() ).size() ) << std::endl;
              }
            }
          }
        }
      }
      m.normalize();
      // cleaning up the mess
      for ( SVED::BucketSetIterator iter = trash.begin(); iter != trash.end(); ++iter ) {
        delete *iter;
      }
      typedef HashTable<const Set<NodeId>*, BucketSet*>::iterator LiftedIter;
      for ( LiftedIter iter = __lifted_pools.begin(); iter != __lifted_pools.end(); ++iter ) {
        // for (SVED::ArraySetIterator j = (**iter).begin(); j != (**iter).end(); ++j) {
        //   delete *j;
        // }
        delete *iter;
      }
      __lifted_pools.clear();
      typedef HashTable< const Set<NodeId>*, std::pair<Set<NodeId>*, Set<NodeId>*> >::iterator ReqIter;
      for ( ReqIter iter = __req_set.begin(); iter != __req_set.end(); ++iter ) {
        delete iter->first;
        delete iter->second;
      }
      __req_set.clear();
      typedef HashTable<const Class*, std::vector<NodeId>*>::iterator ElimIter;
      for ( ElimIter iter = __elim_orders.begin(); iter != __elim_orders.end(); ++iter ) {
        delete *iter;
      }
      __elim_orders.clear();
    }

    void
    SVED::_joint( const std::vector<Chain>& queries, Potential<prm_float>& j ) {
      GUM_ERROR( FatalError, "Not implemented." );
    }

    void
    SVED::__initReqSets( const Instance* i ) {
      Set<NodeId>* attr_set = new Set<NodeId>();
      Set<NodeId>* sc_set = new Set<NodeId>();
      for ( Set<NodeId>::iterator iter = __bb.requisiteNodes( i ).begin(); iter != __bb.requisiteNodes( i ).end(); ++iter ) {
        switch ( i->type().get( *iter ).elt_type() ) {
          case ClassElement::prm_aggregate:
          case ClassElement::prm_attribute: {
              attr_set->insert( *iter );
              break;
            }
          case ClassElement::prm_slotchain: {
              sc_set->insert( *iter );
              break;
            }
          default: {
              GUM_ERROR( FatalError, "There should not be elements other"
                         " than Attribute and SlotChain." );
            }
        }
      }
      __req_set.insert( &( __bb.requisiteNodes( i ) ), std::pair<Set<NodeId>*, Set<NodeId>*>( attr_set, sc_set ) );
    }

  } /* namespace prm */
} /* namespace gum */
// ============================================================================
