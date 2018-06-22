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
 * @brief The class that computes countings of observations from the database.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/scores_and_tests/recordCounter2.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC > 
    RecordCounter2<ALLOC>::RecordCounter2(
        const DBRowGeneratorParser<ALLOC>& parser,
        const std::vector<std::pair<std::size_t,std::size_t>,
               XALLOC<std::pair<std::size_t,std::size_t>>>& ranges,
        const Bijection<NodeId,std::size_t>& nodeId2columns,
        const allocator_type& alloc ) :
      ALLOC<DBTranslatedValue> ( alloc ),
      __nodeId2columns ( nodeId2columns ) {
      // create the ranges
      __ranges.reserve ( ranges.size () );
      for ( const auto& range : ranges ) __ranges.push_back ( range );
      
      // create the parsers. There should always be at least one parser
      if ( __max_nb_threads < std::size_t(1) )
        __max_nb_threads = std::size_t(1);
      __parsers.reserve ( __max_nb_threads );
      for ( std::size_t i = std::size_t(0); i < __max_nb_threads; ++i )
        __parsers.push_back ( parser );
      
      GUM_CONSTRUCTOR ( RecordCounter2 );
    }
    

    /// default constructor
    template < template < typename > class ALLOC >   
    RecordCounter2<ALLOC>::RecordCounter2(
         const DBRowGeneratorParser<ALLOC>& parser,
         const Bijection<NodeId,std::size_t>& nodeId2columns,
         const allocator_type& alloc ) :
      RecordCounter2<ALLOC> ( parser,
                              std::vector<std::pair<std::size_t,std::size_t>,
                              ALLOC<std::pair<std::size_t,std::size_t>>> (),
                              nodeId2columns,
                              alloc ) {}
                      

    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >   
    RecordCounter2<ALLOC>::RecordCounter2(const RecordCounter2<ALLOC>& from,
                                          const allocator_type& alloc) :
      ALLOC<DBTranslatedValue> ( alloc ),
      __parsers ( from.__parsers ),
      __ranges ( from.__ranges ),
      __nodeId2columns ( from.__nodeId2columns ),
      __last_DB_countings ( from.__last_DB_countings ),
      __last_DB_ids ( from.__last_DB_ids ),
      __last_nonDB_countings ( from.__last_nonDB_countings ),
      __last_nonDB_ids ( from.__last_nonDB_ids ),
      __max_nb_threads ( from.__max_nb_threads ) {
      GUM_CONS_CPY ( RecordCounter2 );
    }

    
    /// copy constructor
    template < template < typename > class ALLOC >   
    RecordCounter2<ALLOC>::RecordCounter2(const RecordCounter2<ALLOC>& from) :
      RecordCounter2<ALLOC> ( from, from.getAllocator () ) {}
      

    /// move constructor with a given allocator
    template < template < typename > class ALLOC >   
    RecordCounter2<ALLOC>::RecordCounter2(RecordCounter2<ALLOC>&& from,
                                          const allocator_type& alloc) :
       ALLOC<DBTranslatedValue> ( alloc ),
      __parsers ( std::move ( from.__parsers ) ),
      __ranges ( std::move ( from.__ranges ) ),
      __nodeId2columns ( std::move ( from.__nodeId2columns ) ),
      __last_DB_countings ( std::move ( from.__last_DB_countings )) ,
      __last_DB_ids ( std::move ( from.__last_DB_ids ) ),
      __last_nonDB_countings ( std::move ( from.__last_nonDB_countings ) ),
      __last_nonDB_ids ( std::move ( from.__last_nonDB_ids ) ),
      __max_nb_threads ( from.__max_nb_threads ) {
      GUM_CONS_MOV ( RecordCounter2 );
    }

    
    /// move constructor
    template < template < typename > class ALLOC >   
    RecordCounter2<ALLOC>::RecordCounter2(RecordCounter2<ALLOC>&& from) :
      RecordCounter2<ALLOC> ( std::move(from), from.getAllocator () ) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >   
    RecordCounter2<ALLOC>*
    RecordCounter2<ALLOC>::clone (const allocator_type& alloc) const {
      ALLOC< RecordCounter2< ALLOC > > allocator(alloc);
      RecordCounter2< ALLOC >* new_counter = allocator.allocate(1);
      try {
        allocator.construct(new_counter, *this, alloc);
      } catch (...) {
        allocator.deallocate(new_counter, 1);
        throw;
      }

      return new_counter;
    }

    
    /// virtual copy constructor
    template < template < typename > class ALLOC >   
    RecordCounter2<ALLOC>* RecordCounter2<ALLOC>::clone () const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >   
    RecordCounter2<ALLOC>::~RecordCounter2() {
      GUM_DESTRUCTOR ( RecordCounter2 );
    }


    /// copy operator
    template < template < typename > class ALLOC >   
    RecordCounter2<ALLOC>&
    RecordCounter2<ALLOC>::operator=(const RecordCounter2<ALLOC>& from ) {
      if ( this != &from ) {
        __parsers              = from.__parsers;
        __ranges               = from.__ranges;
        __nodeId2columns       = from.__nodeId2columns;
        __last_DB_countings    = from.__last_DB_countings;
        __last_DB_ids          = from.__last_DB_ids;
        __last_nonDB_countings = from.__last_nonDB_countings;
        __last_nonDB_ids       = from.__last_nonDB_ids;
        __max_nb_threads       = from.__max_nb_threads;
      }
      return *this;
    }
    

    /// move operator
    template < template < typename > class ALLOC >   
    RecordCounter2<ALLOC>&
    RecordCounter2<ALLOC>::operator=(RecordCounter2<ALLOC>&& from ) {
      if ( this != &from ) {
        __parsers              = std::move ( from.__parsers );
        __ranges               = std::move ( from.__ranges );
        __nodeId2columns       = std::move ( from.__nodeId2columns );
        __last_DB_countings    = std::move ( from.__last_DB_countings );
        __last_DB_ids          = std::move ( from.__last_DB_ids );
        __last_nonDB_countings = std::move ( from.__last_nonDB_countings );
        __last_nonDB_ids       = std::move ( from.__last_nonDB_ids );
        __max_nb_threads       = from.__max_nb_threads;
      }
      return *this;
    }
      

    /// clears all the last database-parsed countings from memory
    template < template < typename > class ALLOC >   
    void RecordCounter2<ALLOC>::clear() {
      __last_DB_countings.clear ();
      __last_DB_ids.clear ();
      __last_nonDB_countings.clear ();
      __last_nonDB_ids.clear ();
    }


    /// changes the max number of threads used to parse the database
    template < template < typename > class ALLOC >   
    void RecordCounter2<ALLOC>::setMaxNbThreads(const std::size_t nb) const {
      if (nb == std::size_t(0))
        __max_nb_threads = std::size_t(1);
      else
        __max_nb_threads = nb;
    }


    /// returns the number of threads used to parse the database
    template < template < typename > class ALLOC >   
    std::size_t RecordCounter2<ALLOC>::nbThreads () const {
      return __max_nb_threads;
    }


    /// returns the counts for a given set of nodes
    template < template < typename > class ALLOC >   
    const std::vector< double, ALLOC<double> >&
    RecordCounter2<ALLOC>::counts( const std::vector<NodeId>& ids ) const {
      return __last_DB_countings;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
