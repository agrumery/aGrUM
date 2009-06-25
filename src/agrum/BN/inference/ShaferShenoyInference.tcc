/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Implementations of the classes defined in
 * bns/inference/ShaferShenoyInference.h.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  /*******************************************************************************
   *                          CONSTRUCTOR & DESTRUCTOR                           *
   *******************************************************************************/
  // Default constructor
  template<typename T_DATA> INLINE
  ShaferShenoyInference<T_DATA>::ShaferShenoyInference( const BayesNet<T_DATA> &bayesNet ):
    BayesNetInference<T_DATA>( bayesNet ),
    __triangulation( NULL )
  {
    GUM_CONSTRUCTOR( ShaferShenoyInference );
    // Builds a hashTable where the keys are the id of the variable,
    // and the values the variable's domain size.

    for (NodeSetIterator iter = this->bn().beginNodes(); iter != this->bn().endNodes(); ++iter)
      __modalitiesMap.insert(*iter,  this->bn().variable(*iter).domainSize());

    // Instantiates the triangulation algorithm
    __triangulation = new DefaultTriangulation( this->bn().moralGraph(), __modalitiesMap );

    // Compute the junction tree" << std::endl;
    __triangulation->junctionTree();

    // Builds the cliques tables" << std::endl;
    __buildCliquesTables();
  }

  // Destructor
  template<typename T_DATA> INLINE
  ShaferShenoyInference<T_DATA>::~ShaferShenoyInference() {
    GUM_DESTRUCTOR( ShaferShenoyInference );
    delete __triangulation;

    for ( HashTableIterator<Id, Potential<T_DATA>* > iter = __cliqueMap.begin();
          iter != __cliqueMap.end();
          ++iter ) {
      delete *iter;
    }

    __cliqueMap.clear();

    for ( HashTableIterator<Id, List<const Potential<T_DATA>*>*> iter_1 =
            __clique_evidences.begin();
          iter_1 != __clique_evidences.end();
          ++iter_1 ) {
      ( *iter_1 )->clear();
      delete *iter_1;
    }

    __clique_evidences.clear();

    for ( typename Property< Potential<T_DATA>* >::onArcs::iterator iter =
            __messagesMap.begin();
          iter != __messagesMap.end();
          ++iter ) {
      delete *iter;
    }

    __messagesMap.clear();
  }

  /*******************************************************************************
   *                             PUBLIC METHODS                                  *
   *******************************************************************************/
  // Makes the inference
  template<typename T_DATA> INLINE
  void
  ShaferShenoyInference<T_DATA>::makeInference() {
    // Setting all collect flags at false
    for ( Property< bool >::onNodes::iterator iter = __collected_cliques.begin();
          iter != __collected_cliques.end();
          ++iter ) {
      __collected_cliques[iter.key()] = false;
    }
    for ( HashTableIterator<Id, bool> iter = __collected_cliques.begin();
          iter != __collected_cliques.end();
          ++iter ) {
      if ( __collected_cliques[iter.key()] == false ) {
        try {__collectFromClique( iter.key() );}
        catch (Exception e) {std::cout << "1" << std::endl; throw e;}
        try {__diffuseFromClique( iter.key() );}
        catch (Exception e) {std::cout << "2" << std::endl; throw e;}
      }
    }
  }

  // Returns the probability of the variable.
  // @param id The variable's id.
  // @throw NotFound Raised if no variable matches id.
  // @throw OperationNotAllowed Raised if the inference haven't be done.
  template<typename T_DATA> INLINE
  void
  ShaferShenoyInference<T_DATA>::_fillMarginal( Id id,Potential<T_DATA>& marginal ) {
    try {
      Id cliqueId = __node2CliqueMap[id];
      const EdgeSet &neighbours = __getneighbours( cliqueId );

      try { __collectFromClique( cliqueId ); }
      catch ( Exception e ) {
        throw e;
      }

      Potential<T_DATA> temp;

      temp=( *__cliqueMap[cliqueId] );

      for ( EdgeSetIterator iter = neighbours.begin();
            iter != neighbours.end(); ++iter ) {
        /*
          if ( marginal == NULL ) {
          try {
          marginal = new Potential<T_DATA>();
          marginal->multiplicate( , *__messagesMap[*iter] );
          } catch ( Exception e ) {
          throw e;
          }
          } else {
          try {
          Potential<T_DATA> *temp = marginal;
          marginal = new Potential<T_DATA>();
          marginal->multiplicate( *temp, *__messagesMap[*iter] );
          delete temp;
          } catch ( Exception e ) {
          throw e;
          }
          } // End if*/
        temp.multiplicateBy( *__messagesMap[Arc( iter->other( cliqueId ),cliqueId )] );
      } // End for

      marginal << this->bn().variable( id );

      marginal.marginalize( temp ).normalize();
    } catch ( Exception e ) {
      throw e;
    }
  }

  // insert new evidence in the graph
  template<typename T_DATA>
  void
  ShaferShenoyInference<T_DATA>::insertEvidence
  ( const List<const Potential<T_DATA>*>& pot_list ) {
    Id varId, cliqueId;
    List<const Potential<T_DATA>*>* list = NULL;
    bool calledRemoveDifMess;

    for ( ListConstIterator<const Potential<T_DATA>*> iter = pot_list.begin();
          iter != pot_list.end();
          ++iter ) {
      // check that the evidence is given w.r.t.only one random variable
      if (( *iter )->nbrDim() != 1 )
        GUM_ERROR( IdError,
                   "Evidence can only be giben w.r.t. one random variable" );

      calledRemoveDifMess = false;

      // remove already existing evidence w.r.t. iter's node
      varId = this->bn().nodeId(( *iter )->variable( 0 ) );

      try {
        list = __clique_evidences[__node2CliqueMap[varId]];
      } catch ( NotFound ) {
        list = new List<const Potential<T_DATA>* >();
        __clique_evidences.insert( __node2CliqueMap[varId], list );
      }

      for ( ListConstIterator<const Potential<T_DATA>* > iter2 = list->begin();
            iter2 != list->end();
            ++iter2 ) {
        if ( varId == this->bn().nodeId(( *iter2 )->variable( 0 ) ) ) {
          eraseEvidence( *iter2 );
          calledRemoveDifMess = true;
          break;
        }
      }

      // insert the evidence
      cliqueId = __node2CliqueMap[varId];

      __clique_evidences[cliqueId]->insert( *iter );

      if ( ! calledRemoveDifMess )
        __removeDiffusedMessages( cliqueId );
    }
  }

  // remove a given evidence from the graph
  template <typename T_DATA>
  void
  ShaferShenoyInference<T_DATA>::eraseEvidence( const Potential<T_DATA>* e ) {
    try {
      Id varId = this->bn().nodeId( e->variable( 0 ) );
      Id cliqueId = __node2CliqueMap[varId];
      List<const Potential<T_DATA>* >& e_list = * ( __clique_evidences[cliqueId] );

      for ( ListConstIterator<const Potential<T_DATA>*> iter = e_list.begin();
            iter != e_list.end();
            ++iter ) {
        Id iterId = this->bn().nodeId(( *iter )->variable( 0 ) );

        if ( iterId == varId ) {
          e_list.eraseByVal( *iter );
          __removeDiffusedMessages( cliqueId );
          return;
        }
      }
    } catch ( NotFound ) {
      // e doesn't concerns one of the variable in the network
      return;
    }
  }

  // remove all evidence from the graph
  template <typename T_DATA>
  void
  ShaferShenoyInference<T_DATA>::eraseAllEvidence() {
    for ( typename Property< List<const Potential<T_DATA>*>* >::onNodes::iterator iter =
            __clique_evidences.begin();
          iter != __clique_evidences.end();
          ++iter ) {
      __removeDiffusedMessages( iter.key() );
      ( *iter )->clear();
      delete *iter;
    }
  }

  /*******************************************************************************
   *                            PRIVATE METHODS                                  *
   *******************************************************************************/
  // @return Returns the number of neighbours of a given clique
  template<typename T_DATA> INLINE
  Size
  ShaferShenoyInference<T_DATA>::__getNbrneighbours( Id cliqueId ) {
    return __triangulation->junctionTree().neighbours( cliqueId ).size();
  }

  // @return Returns the list of neighbours of a given clique
  template<typename T_DATA> INLINE
  const EdgeSet&
  ShaferShenoyInference<T_DATA>::__getneighbours( Id cliqueId ) {
    return __triangulation->junctionTree().neighbours( cliqueId );
  }

  //// @return Returns the neighbour's Id given an iterator on __neighbourList
  //template<typename T_DATA> INLINE
  //const Id&
  //ShaferShenoyInference<T_DATA>::__getneighbour(const ListIterator< RefPtr<Arc> > &iter, Id other)
  //{
  //  return (*iter)->other(other);
  //}
  // @return Returns a separator given two adjacent cliques
  template<typename T_DATA> INLINE
  const NodeSet&
  ShaferShenoyInference<T_DATA>::__getSeparator( Id clique_1, Id clique_2 ) {
    return __triangulation->junctionTree().separator( clique_1, clique_2 );
  }

  // @return Returns the clique in which the node's cpt must be stored
  template<typename T_DATA> INLINE
  Id
  ShaferShenoyInference<T_DATA>::__getClique
  ( const std::vector<Id> &eliminationOrder, Id id ) {
    List<Id> idList;
    const ArcSet &parents=this->bn().dag().parents( id );

    for ( ArcSetIterator iter = parents.begin();
          iter != parents.end();
          ++iter ) {
      idList.insert( iter->tail() );
    }

    idList.insert( id );

    for ( unsigned int i = 0; i < eliminationOrder.size(); i++ ) {
      if ( idList.exists( eliminationOrder[i] ) )
        return __triangulation->createdClique( eliminationOrder[i] );
    }

    GUM_ERROR( NotFound, "Node not found in elimination order." );
  }

  // Builds the cliques tables
  template<typename T_DATA> INLINE
  void
  ShaferShenoyInference<T_DATA>::__buildCliquesTables() {
    const std::vector<Id> &eliminationOrder = __triangulation->eliminationOrder();
    Id cliqueId, createdClique;
    const Potential<T_DATA> *cpt = NULL;
    const Potential<T_DATA> *temp = NULL;

    for ( unsigned int i = 0; i < eliminationOrder.size(); i++ ) {
      createdClique = __triangulation->createdClique( eliminationOrder[i] );

      if ( ! __collected_cliques.exists( createdClique ) )
        __collected_cliques.insert( createdClique, false );

      cliqueId = __getClique( eliminationOrder, eliminationOrder[i] );

      cpt = &( this->bn().cpt( eliminationOrder[i] ) );

      __node2CliqueMap.insert( eliminationOrder[i], cliqueId );

      if ( ! __cliqueMap.exists( cliqueId ) ) {
        __cliqueMap.insert( cliqueId,
                            new Potential<T_DATA>( new MultiDimArray<T_DATA>(),
                                                   *cpt ) ); //__copyPrXX(*cpt));
      } else {
        temp = __cliqueMap[cliqueId];
        __cliqueMap[cliqueId] = new Potential<T_DATA>();
        ( __cliqueMap[cliqueId] )->multiplicate( *temp, *cpt );
        delete temp;
      }
    }
  }

  // Calls a collect with a node has source
  template<typename T_DATA> INLINE
  void
  ShaferShenoyInference<T_DATA>::__collectFromClique( Id source ) {
    __collected_cliques[source] = true;

    try {
      for ( EdgeSetIterator iter = __getneighbours( source ).begin();
            iter != __getneighbours( source ).end();
            ++iter ) {
        __collect( source, iter->other( source ) );
      }
    } catch (Exception e) {
      // Raised if source has no neighbours
    }
  }

  // Collecting phase of the inference
  template<typename T_DATA> INLINE
  bool
  ShaferShenoyInference<T_DATA>::__collect( Id source, Id current ) {
    __collected_cliques[current] = true;
    bool recomputeMessage = false;

    for ( EdgeSetIterator iter = __getneighbours( current ).begin();
          iter != __getneighbours( current ).end();
          ++iter ) {
      NodeId other=iter->other( current );

      if ( other!= source ) {
        recomputeMessage = recomputeMessage || __collect( current, other );
      }
    }

    if ( recomputeMessage ) {
      // I need to recompute current's message, so no need to check for new evidence
      __removeDiffusedMessages( current );
      __sendMessage( current, source );
      return true;
    }

    // If there is no new evidence, then no need to compute message
    // unless there is new evidence on current
    else if ( ! __messageExists( current, source ) ) {
      // There is new evidence
      __sendMessage( current, source );
      return true;
    } else {
      return false;
    }
  }

  template<typename T_DATA> INLINE
  void
  ShaferShenoyInference<T_DATA>::__diffuseFromClique( Id source ) {
    try {
      for ( EdgeSetIterator iter = __getneighbours( source ).begin();
            iter != __getneighbours( source ).end();
            ++iter ) {
        if ( __messageExists( source, iter->other( source ) ) ) {
          __diffuse( source, iter->other( source ), false );
        } else {
          __sendMessage( source, iter->other( source ) );
          __diffuse( source, iter->other( source ), true );
        }
      }
    } catch (Exception e) {
      // Raised if source has no neighbours
    }
  }

  // Diffusing phase of the inference
  template<typename T_DATA> INLINE
  void
  ShaferShenoyInference<T_DATA>::__diffuse( Id source, Id current, bool recompute ) {
    for ( EdgeSetIterator iter = __getneighbours( current ).begin();
          iter != __getneighbours( current ).end();
          ++iter ) {
      if ( iter->other( current ) != source ) {
        if ( recompute || ! __messageExists( current, iter->other( current ) ) ) {
          __sendMessage( current, iter->other( current ) );
          __diffuse( current, iter->other( current ), true );
        } else {
          __diffuse( current, iter->other( current ), false );
        }
      }
    }
  }

  template<typename T_DATA> INLINE
  void
  ShaferShenoyInference<T_DATA>::__sendMessage( Id tail, Id head ) {
    // Building the message's table held by the separator
    Potential<T_DATA> *message = new Potential<T_DATA>();
    const NodeSet &separator = __getSeparator( tail, head );

    for ( __Separator::iterator iter = separator.begin();
          iter != separator.end(); ++iter ) {
      ( *message ) << this->bn().variable( *iter );
    }

    Potential<T_DATA> *joint = NULL;

    // Check if the clique was initialized

    try {
      joint = new Potential<T_DATA>( new MultiDimArray<T_DATA>(),
                                     *__cliqueMap[tail] ); //__copyPrXX(*__cliqueMap[tail]);
    } catch ( NotFound ) {
      __cliqueMap.insert( tail,new Potential<T_DATA>() );
      joint = new Potential<T_DATA>();
    }

    const EdgeSet &neighbours = __getneighbours( tail );

    // First, add evidences to the clique's table
    try {
      List<const Potential<T_DATA>*>& evidenceList = *__clique_evidences[tail];

      for ( ListConstIterator<const Potential<T_DATA>*> iter = evidenceList.begin();
            iter != evidenceList.end(); ++iter ) {
        Potential<T_DATA>* temp = joint;
        joint = new Potential<T_DATA>();
        joint->multiplicate( *temp, *( *iter ) );
        delete temp;
      }
    } catch ( NotFound ) {
      // No evidence to add
    }

    // Second, add message from tail's neighbours
    for ( EdgeSetIterator iter = neighbours.begin(); iter != neighbours.end();
          ++iter ) {
      NodeId other=iter->other( tail );

      if ( other != head ) {
        Arc neighbourMessage( other, tail );
        joint->multiplicateBy( *__messagesMap[neighbourMessage] );
      } // End if
    } // End for

    Arc p( tail, head );

    // Deleting previous message
    if ( __messagesMap.exists( p ) )
      delete __messagesMap[p];

    message->marginalize( *joint );

    __messagesMap.insert( p, message );

    delete joint;
  }

  template<typename T_DATA> INLINE bool
  ShaferShenoyInference<T_DATA>::__messageExists( Id source, Id dest ) {
    return __messagesMap.exists( Arc( source,dest ) );
  }

  template <typename T_DATA> void
  ShaferShenoyInference<T_DATA>::__removeDiffusedMessages( Id cliqueId ) {
    const __neighbourList& neighbours = __getneighbours( cliqueId );

    for ( __neighbourList::iterator iter = neighbours.begin();
          iter != neighbours.end();
          ++iter ) {
      Arc message( cliqueId, iter->other( cliqueId ) );

      if ( __messagesMap.exists( message ) ) {
        delete __messagesMap[message];
        __messagesMap.erase( message );
      }
    }
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
