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
/** @file
 * @brief Template implementation of RecordCounters and RecordCounterThreads
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {

      
    /// default constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE
    RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>::RecordCounterThread
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities ) :
      __modalities ( &var_modalities ),
      __filter ( filter ) {
      GUM_CONSTRUCTOR ( RecordCounterThread );
    }


    /// copy constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE
    RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>::RecordCounterThread
    ( const RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>& from ) :
      __modalities ( from.__modalities ),
      __filter ( from.__filter ) {
      GUM_CONS_CPY ( RecordCounterThread );
    }


    /// move operator
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE
    RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>::RecordCounterThread
    ( RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>&& from ) :
      __modalities ( from.__modalities ),
      __nodesets  ( std::move ( from.__nodesets ) ),
      __filter    ( std::move ( from.__filter ) ),
      __countings ( std::move ( from.__countings ) ){
      GUM_CONS_MOV ( RecordCounterThread );
    }


    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE
    RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>::~RecordCounterThread () {
      GUM_DESTRUCTOR ( RecordCounterThread );
    }
    
     
    /// adds a new target nodeset to be counted
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE unsigned int
    RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( const std::vector<unsigned int,IdSetAlloc>& ids ) {
      // adds the nodeset
      unsigned int nodeset_id = __nodesets.size ();
      __nodesets.push_back ( &ids );

      // compute the size of the nodeset and allocate a counting correspondingly
      unsigned int size = 1;
      for ( const auto& id : ids ) {
        size *= __modalities->operator[] ( id );
      }

      // allocate the counting set
      __countings.push_back ( std::vector<float,CountAlloc> ( size, 0 ) );
      
      return nodeset_id;
    }


    /// remove all the current target nodesets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE void
    RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>::clearNodeSets ()
      noexcept {
      __nodesets.clear ();
      __countings.clear ();
    }


    /// update all the countings of all the nodesets by parsing the database
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE void
    RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>::count () {
      __filter.reset ();
      while ( __filter.hasRows () ) {
        // get the observed filtered rows
        FilteredRow& row = __filter.row ();
        
        // fill the counts for the ith nodeset
        for ( unsigned int i = 0, size =  __nodesets.size (); i < size; ++i ) {
          const std::vector<unsigned int>& var_ids = *( __nodesets[i] );
          unsigned int offset = 0;
          unsigned int dim = 1;
          for ( unsigned int j = 0, vsize = var_ids.size (); j < vsize; ++j ) {
            offset += row[var_ids[j]] * dim;
            dim *= __modalities->operator[] ( var_ids[j] );
          }
          __countings[i][offset] += row.weight ();
        }
      }
    }


    /// returns the countings for the nodeset specified in argument
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE const
    std::vector<float,CountAlloc>&
    RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>::getCounts
    ( unsigned int nodeset_id ) const noexcept {
      return __countings[ nodeset_id];
    }

      
    /// returns the filter used for the countings
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE RowFilter&
    RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>::filter () noexcept {
      return __filter;
    }
      




    /// default constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::RecordCounter
    ( const RowFilter& filter,
      const std::vector<unsigned int>& var_modalities ) :
      __row_filter ( filter ),
      __modalities ( &var_modalities ) {
      GUM_CONSTRUCTOR ( RecordCounter );
    }

    
    /// copy constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::RecordCounter
    ( const RecordCounter<RowFilter,IdSetAlloc,CountAlloc>& from ) :
      __row_filter ( from.__row_filter ),
      __modalities ( from.__modalities ),
      __idsets ( from.__idsets ),
      __is_id_subset ( from.__is_id_subset ),
      __countings ( from.__countings ),
      __thread_counters ( from.__thread_counters ),
      __nb_thread_counters ( from.__nb_thread_counters ) {
      GUM_CONS_CPY ( RecordCounter );
      
      __nodesets.resize ( __idsets.size () );
      __set2thread_id.resize ( __idsets.size () );
      for ( auto iter = __idsets.cbegin (); iter != __idsets.cend (); ++iter ) {
        const auto& idset = iter.key ();
        __idset2index.insert ( &idset, iter.val () );
        __nodesets[iter.val()] = &idset;
        __set2thread_id[iter.val()].first = &idset;
        __set2thread_id[iter.val()].second =
          from.__set2thread_id[iter.val()].second;

        // for each variable in ids, indicate that ids contain this variable
        const std::vector<unsigned int,IdSetAlloc>& ids = idset.ids();
        for ( auto id : ids ) {
          try {
            __var2idsets[id].push_back ( &idset );
          }
          catch ( NotFound& ) {
            __var2idsets.insert ( id, std::vector<const IdSet<IdSetAlloc>*> () );
            __var2idsets[id].push_back ( &idset );
          }
        }
      }
    }


    /// move constructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::RecordCounter
    ( RecordCounter<RowFilter,IdSetAlloc,CountAlloc>&& from ) :
      __row_filter ( std::move ( from.__row_filter ) ),
      __modalities ( from.__modalities ),
      __idsets ( from.__idsets ),
      __nodesets ( std::move ( from.__nodesets ) ),
      __var2idsets ( std::move ( from.__var2idsets ) ),
      __is_id_subset ( std::move ( from.__is_id_subset ) ),
      __countings ( std::move ( from.__countings ) ),
      __idset2index ( std::move ( from.__idset2index ) ),
      __set2thread_id ( std::move ( from.__set2thread_id ) ),
      __thread_counters ( std::move ( from.__thread_counters ) ),
      __nb_thread_counters ( from.__nb_thread_counters ) {
      GUM_CONS_MOV ( RecordCounter );
    }

    
    /// destructor
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::~RecordCounter () {
      GUM_DESTRUCTOR ( RecordCounter );
    }

    
    /// add a new nodeset to count
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    unsigned int
    RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::addNodeSet
    ( const std::vector<unsigned int,IdSetAlloc>& ids ) {
      // allocate a count vector of the domain size of the idset
      unsigned int set_size = 1;
      for ( const auto& id : ids ) {
        set_size *= __modalities->operator[] ( id );
      }
      __countings.push_back
        ( std::vector<float,CountAlloc> ( set_size, 0.0f ) );

      // a priori, the idset is not a subset
      __is_id_subset.push_back ( false );
      
      // create the corresponding idset
      const IdSet<IdSetAlloc>& idset =
        __idsets.insert ( IdSet<IdSetAlloc> ( ids ), __idsets.size () ).first;
      __idset2index.insert ( &idset, __idset2index.size () );
      __set2thread_id.push_back
        ( std::pair<const IdSet<IdSetAlloc>*,unsigned int>
          ( &idset, __set2thread_id.size () ) );
      __nodesets.push_back ( &ids );
      
      // for each variable in ids, indicate that ids contain this variable
      for ( auto id : ids ) {
        try {
          __var2idsets[id].push_back ( &idset );
        }
        catch ( NotFound& ) {
          __var2idsets.insert ( id, std::vector<const IdSet<IdSetAlloc>*> () );
          __var2idsets[id].push_back ( &idset );
        }
      }
      
      return __idset2index.size () - 1;
    }

    
    // an algorithm to parse a subset of the database
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    void RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::countOnSubDatabase () {
      // now, for all the non-subsets, compute their countings
      // start parallel ThreadCounters
      #pragma omp parallel num_threads ( omp_get_max_threads() )
      {
        // create ThreadCounters if needed
        int num_threads = gum::getNumberOfRunningThreads();
        
        #pragma omp single
        {
          while ( __thread_counters.size () < num_threads ) {
            __thread_counters.push_back
              ( RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>
                ( __row_filter, *__modalities ) );
          }
          __nb_thread_counters = num_threads;
        }

        // initialize the thread counters
        int this_thread = getThreadNumber();
        RecordCounterThread<RowFilter,IdSetAlloc,CountAlloc>&
          thread_counter = __thread_counters[this_thread];
        thread_counter.clearNodeSets ();
        for ( unsigned int i = 0, size = __is_id_subset.size(); i < size; ++i ) {
          if ( ! __is_id_subset[i] ) {
            thread_counter.addNodeSet ( *( __nodesets[i] ) );
          }
        }

        // indicate to the filter which part of the database it must parse
        unsigned long
          db_size = thread_counter.filter().handler().DBSize ();
        unsigned long
          size_per_thread = ( db_size + num_threads - 1 ) / num_threads;
        unsigned long min_range = size_per_thread * this_thread;
        unsigned long max_range = std::min ( min_range + size_per_thread,
                                             db_size );
        thread_counter.filter().handler().setRange ( min_range, max_range );
          
        // compute the counts
        thread_counter.count ();
          
      } // omp parallel

        
      // perform the aggregation of the countings
      #pragma omp parallel num_threads ( __nb_thread_counters )
      {
        int this_thread = getThreadNumber ();
        auto& counter = __thread_counters[this_thread];
        unsigned int size_per_thread, min_range, max_range;

        for ( unsigned int i = 0, size = __countings.size (); i < size; ++i ) {
          if ( ! __is_id_subset[i] ) {
            // get the ith idset countings computed by the curent thread
            std::vector<float,CountAlloc>& vect =
              const_cast<std::vector<float,CountAlloc>&>
              ( counter.getCounts ( __set2thread_id[i].second ) );
            size_per_thread =
              ( vect.size () + __nb_thread_counters - 1 ) / __nb_thread_counters;

            // add to vect the countings of the other threads
            min_range = size_per_thread * this_thread;
            max_range = std::min ( min_range + size_per_thread,
                                   (unsigned int) vect.size () ); 
            for ( unsigned int j = 0; j < __nb_thread_counters; ++j ) {
              if ( j != this_thread ) {
                const std::vector<float,CountAlloc>& othervect =
                  __thread_counters[j].getCounts ( __set2thread_id[i].second );
                for ( unsigned int k = min_range; k < max_range; ++k ) {
                  vect[k] += othervect[k];
                }
              }
            }

            // now copy what we just computed into the countings of the
            // current object
            for ( unsigned int k = min_range; k < max_range; ++k ) {
              std::vector<float,CountAlloc>& final_vect = __countings[i];
              for ( unsigned int k = min_range; k < max_range; ++k ) {
                final_vect[k] = vect[k];
              }
            }
          }
        }
      }
    }


    // computes the countings of the subsets from those of their supersets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    void RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::countSubsets () {
        // perform the countings for the subsets
        //#pragma omp parallel for
        for ( unsigned int i = 0; i < __is_id_subset.size (); ++i ) {
           if ( __is_id_subset[i] &&
               ( __set2thread_id[i].second !=
                 std::numeric_limits<unsigned int>::max () ) ) {
            // get the subset and its superset
            const auto& subset_ids = *( __nodesets[i] );
            const auto& superset_ids = *( __nodesets[__set2thread_id[i].second] );
            auto& subset_vect = __countings[i];
            const auto& superset_vect = __countings[ __set2thread_id[i].second ];
                         
            // Compute the variables that belong to both the (projection) subset
            // and its superset. Store the number of increments in the computation
            // loops at the end of the function before which the variables of the
            // projection set need be incremented (vector before_incr).
            std::vector<unsigned int> table_and_result_offset;
            std::vector<unsigned int> table_and_result_domain;
            std::vector<unsigned int> before_incr;
            unsigned int nb_positive_before_incr = 0;
            unsigned int table_alone_domain_size = 1;
            unsigned int result_domain_size = 1;
            unsigned int table_domain_size = superset_vect.size ();
            {
              unsigned int tmp_before_incr = 1;
              bool has_before_incr = false;
              unsigned int subset_size = subset_ids.size ();

              for ( unsigned int h = 0, j = 0; h < superset_ids.size (); ++h ) {
                if ( j < subset_size ) {
                  if ( subset_ids[j] == superset_ids[h] ) {
                    if ( has_before_incr ) {
                      before_incr.push_back ( tmp_before_incr - 1 );
                      has_before_incr = false;
                      ++nb_positive_before_incr;
                    }
                    else {
                      before_incr.push_back ( 0 );
                    }

                    unsigned int modality =
                      __modalities->operator[] ( subset_ids[j] ); 
                    table_and_result_domain.push_back ( modality );
                    table_and_result_offset.push_back ( result_domain_size );
                    result_domain_size *= modality;
                    tmp_before_incr = 1;
                    
                    ++j;
                  }
                  else {
                    unsigned int modality =
                      __modalities->operator[] ( subset_ids[j] ); 
                    tmp_before_incr *= modality;
                    has_before_incr = true;
                    table_alone_domain_size *= modality;
                  }
                }
                else {
                  table_alone_domain_size *=
                    __modalities->operator[] ( superset_ids[h] ); 
                }
              }
            }
 
            std::vector<unsigned int>
              table_and_result_value = table_and_result_domain;
            std::vector<unsigned int>
              current_incr = before_incr;
            std::vector<unsigned int>
              table_and_result_down = table_and_result_offset;

            for ( unsigned int j = 0; j < table_and_result_down.size(); ++j ) {
              table_and_result_down[j] *= ( table_and_result_domain[j] - 1 );
            }
 
            // now, fill the subset counting vector: first loop over the variables
            // X's in table that do not belong to result and, for each value of
            // these X's, loop over the variables in both table and result. As
            // such, in the internal loop, the offsets of "result" need only be
            // incremented as usually to parse appropriately this table. For
            // result, the problem is slightly more complicated: in the outer for
            // loop, we shall always reset resul_offset to 0. For the inner loop,
            // result_offset should be incremented (++) only when t1
            // before_incr[xxx] steps in the loop have already been made. but
            // before doing so, check whether there exist positive_before_incr. If
            // this is not the case, optimize by not using before_incr at all
            if ( ! nb_positive_before_incr ) {
              for ( unsigned int h = 0, k = 0; h < table_alone_domain_size; ++h ) {
                for ( unsigned int j = 0; j < result_domain_size; ++j, k++ ) {
                  subset_vect[j] += superset_vect[k];
                }
              }
            }
            else {
              // here there are positive before_incr and we should use them
              //to know when result_offset needs be changed
              unsigned int result_offset = 0;

              for ( unsigned int h = 0; h < table_domain_size; ++h ) {
                subset_vect[result_offset] += superset_vect[h];

                // update the offset of result
                for ( unsigned int k = 0; k < current_incr.size(); ++k ) {
                  // check if we need modify result_offset
                  if ( current_incr[k] ) {
                    --current_incr[k];
                    break;
                  }

                  current_incr[k] = before_incr[k];

                  // here we shall modify result_offset
                  --table_and_result_value[k];

                  if ( table_and_result_value[k] ) {
                    result_offset += table_and_result_offset[k];
                    break;
                  }

                  table_and_result_value[k] = table_and_result_domain[k];
                  result_offset -= table_and_result_down[k];
                }
              }
            }
          }
        }
      }
            
      
    /// perform the countings of all the sets of ids
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    void RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::count () {
      // first, we examine the nodesets to determine which ones are subsets
      // of other nodesets. For those sets, we will derive their countings
      // directly from their supersets
      __computeSubsets (); 

      // divide the countings among threads w.r.t. the database, i.e., each
      // thread will compute all the countings but only on a subset of the
      // database
      countOnSubDatabase ();
        
      // perform the countings on the subsets
      countSubsets ();

      // deallocate the memory used by the thread counters
      for ( auto& counter : __thread_counters ) {
        counter.clearNodeSets ();
      }
    }


    /// returns the filter used for the countings
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    RowFilter& RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::filter () noexcept {
      return __row_filter;
    }
      

    /// returns the counts performed for a given idSet
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    INLINE const
    std::vector<float,CountAlloc>&
    RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::getCounts
    ( unsigned int idset ) const noexcept {
      return __countings[ idset ];
    }
      

    /// resets the counter, i.e., remove all its sets of ids and counting vectors
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    void
    RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::clearNodeSets () noexcept {
      for ( auto& counter : __thread_counters ) {
        counter.clearNodeSets ();
      }
      __idsets.clear ();
      __var2idsets.clear ();
      __is_id_subset.clear ();
      __countings.clear ();
      __set2thread_id.clear ();
      __idset2index.clear ();
      __nodesets.clear ();
    }


    /// determine which sets are subsets
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc>
    void RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::__computeSubsets () {
      // sort the idSets by increasing size order for each variable
      for ( auto iter = __var2idsets.begin ();
            iter != __var2idsets.end (); ++iter ) {
        std::sort ( iter.val().begin(), iter.val().end(), __sortIdSetBySize );
      }

      // now, for each IdSet, determine if it is a subset of another IdSet
      for ( unsigned int i = 0, j = 0, size = __set2thread_id.size ();
            i< size; ++i ) {
        // get the IdSet to determine
        const IdSet<IdSetAlloc>& ids = *( __set2thread_id[i].first );
        const unsigned int ids_size = ids.ids ().size ();
        
        // empty sets are always subsets
        if ( ! ids_size ) {
          __is_id_subset[i] = true;
          __set2thread_id[i].second = std::numeric_limits<unsigned int>::max ();
          continue;
        }

        // get all the sets that contain its first variable and parse them
        const std::vector<const IdSet<IdSetAlloc>*>&
          sets = __var2idsets[ ids[0] ];
        bool subset = false;
        unsigned int index;
        for ( const auto set : sets ) {
          if ( ( set->ids().size () > ids_size ) && ids.isSubset ( *set ) ) {
            subset = true;
            index = __idset2index[set];
            break;
          }
        }
        
        if ( subset ) {
          // assign the superset to the subset and allocate the subset's counting
          __is_id_subset[i] = true;
          __set2thread_id[i].second = index;
        }
        else {
          __set2thread_id[i].second = j;
          ++j;
        }
      }
    }

      
    /// the function used to sort vectors of IdSets by increasing size order
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    bool
    RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::__sortIdSetBySize
    ( const IdSet<IdSetAlloc>* set1,
      const IdSet<IdSetAlloc>* set2 ) {
      return set1->ids().size () < set2->ids().size ();
    }


    /// returns the modalities of the variables in the database
    template <typename RowFilter, typename IdSetAlloc, typename CountAlloc> INLINE
    const std::vector<unsigned int>&
    RecordCounter<RowFilter,IdSetAlloc,CountAlloc>::modalities () const {
      if ( __modalities ) return *__modalities;
      else {
        GUM_ERROR ( NullElement,
                    "The record Counter does not have any modalities stored yet" );
      }
    }
 

  } /* namespace learning */

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
