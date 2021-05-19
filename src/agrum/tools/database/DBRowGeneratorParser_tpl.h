/***************************************************************************
 *   Copyright (c) 2005-2020 by Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)  *
 *   info_at_agrum_dot_org                                               *
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
 * @brief Template implementation of the DBRowGeneratorParser class
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <template<typename> class ALLOC>
    DBRowGeneratorParser<ALLOC>::DBRowGeneratorParser(
      const typename DatabaseTable<ALLOC>::Handler& handler,
      const DBRowGeneratorSet<ALLOC>& generator_set,
      const typename DBRowGeneratorParser<ALLOC>::allocator_type& alloc )
      : handler__(handler)
      , generator_set__(generator_set, alloc)
      , generator_size__ ( generator_set.size () ) {
      GUM_CONSTRUCTOR(DBRowGeneratorParser);
    }
    

    /// copy constructor with a given allocator
    template <template<typename> class ALLOC>
    DBRowGeneratorParser<ALLOC>::DBRowGeneratorParser(
      const DBRowGeneratorParser<ALLOC>& from,
      const typename DBRowGeneratorParser<ALLOC>::allocator_type& alloc )
      : handler__(from.handler__)
      , generator_set__(from.generator_set__)
      , generator_size__(from.generator_size__) {
      GUM_CONS_CPY(DBRowGeneratorParser);
    }

    
    /// copy constructor
    template <template<typename> class ALLOC>
    INLINE DBRowGeneratorParser<ALLOC>::DBRowGeneratorParser(
      const DBRowGeneratorParser<ALLOC>& from)
      : DBRowGeneratorParser<ALLOC> ( from, from.getAllocator () ) {}

    
    /// move constructor with a given allocator
    template <template<typename> class ALLOC>
    DBRowGeneratorParser<ALLOC>::DBRowGeneratorParser(
      DBRowGeneratorParser<ALLOC>&& from,
      const typename DBRowGeneratorParser<ALLOC>::allocator_type& alloc )
      : handler__ ( std::move(from.handler__) )
      , generator_set__(std::move(from.generator_set__))
      , generator_size__(from.generator_size__) {
      GUM_CONS_MOV(DBRowGeneratorParser);
    }
    

    /// move constructor
    template <template<typename> class ALLOC>
    INLINE DBRowGeneratorParser<ALLOC>::DBRowGeneratorParser(
      DBRowGeneratorParser<ALLOC>&& from)
      : DBRowGeneratorParser<ALLOC> ( std::move(from), from.getAllocator() ) {}


    /// virtual copy constructor with a given allocator
    template <template<typename> class ALLOC>
    DBRowGeneratorParser<ALLOC>* DBRowGeneratorParser<ALLOC>::clone (
      const typename DBRowGeneratorParser<ALLOC>::allocator_type& alloc) const {
      ALLOC<DBRowGeneratorParser<ALLOC>> allocator ( alloc );
      DBRowGeneratorParser<ALLOC>* new_parser = allocator.allocate(1);
      try {
        allocator.construct ( new_parser, *this, alloc );
      }
      catch ( ... ) {
        allocator.deallocate ( new_parser, 1 );
        throw;
      }
      return new_parser;
    }


    /// virtual copy constructor
    template <template<typename> class ALLOC>
    INLINE DBRowGeneratorParser<ALLOC>*
    DBRowGeneratorParser<ALLOC>::clone () const {
      return clone ( this->getAllocator () );
    }

   
    /// destructor
    template <template<typename> class ALLOC>
    DBRowGeneratorParser<ALLOC>::~DBRowGeneratorParser() {
      GUM_DESTRUCTOR(DBRowGeneratorParser);
    }


    /// copy operator
    template <template<typename> class ALLOC>
    INLINE DBRowGeneratorParser<ALLOC>&
    DBRowGeneratorParser<ALLOC>::operator=(
      const DBRowGeneratorParser<ALLOC>& from) {
      if (this != &from) {
        handler__ = from.handler__;
        generator_set__ = from.generator_set__;
        generator_size__ = from.generator_size__;
      }
      return *this;
    }

    
    /// move operator
    template <template<typename> class ALLOC>
    INLINE DBRowGeneratorParser<ALLOC>&
    DBRowGeneratorParser<ALLOC>::operator=(DBRowGeneratorParser<ALLOC>&& from) {
      if (this != &from) {
        handler__ = std::move(from.handler__);
        generator_set__ = std::move(from.generator_set__);
        generator_size__ = from.generator_size__;
      }
      return *this;
    }

    
    /// returns true if there are still rows that can be output by the RowFilter
    template <template<typename> class ALLOC>
    INLINE bool DBRowGeneratorParser<ALLOC>::hasRows() {
      if ( ! generator_size__ ) return handler__.hasRows();

      while ( ! generator_set__.hasRows() ) {
        if ( ! handler__.hasRows () ) return false;
        generator_set__.setInputRow( *handler__ );
        ++handler__;
      }
      return true;
    }

    
    /// returns a new output row with its corresponding weight
    template <template<typename> class ALLOC>
    INLINE const DBRow<DBTranslatedValue,ALLOC>&
    DBRowGeneratorParser<ALLOC>::row() {
      // if there exists no generator in the generator set, we just return
      // the row pointed to by the handler
      if ( ! generator_size__ ) {
        const auto& pointed_row = *handler__;
        ++handler__;
        return pointed_row;
      }
      else {
        return generator_set__.generate();
      }
    }
    

    /// resets the filter
    template <template<typename> class ALLOC>
    INLINE void DBRowGeneratorParser<ALLOC>::reset() {
      handler__.reset();
      generator_set__.reset();
    }
    

    // returns the handler used by the filter
    template <template<typename> class ALLOC>
    INLINE typename DatabaseTable<ALLOC>::Handler&
    DBRowGeneratorParser<ALLOC>::handler() {
      return handler__;
    }
    

    // returns the handler used by the filter
    template <template<typename> class ALLOC>
    INLINE const typename DatabaseTable<ALLOC>::Handler&
    DBRowGeneratorParser<ALLOC>::handler() const {
      return handler__;
    }

    
    /// returns a reference on the database
    template <template<typename> class ALLOC>
    INLINE const DatabaseTable< ALLOC >&
    DBRowGeneratorParser<ALLOC>::database () const {
      return static_cast<const DatabaseTable< ALLOC >&>
        (handler__.database ());
    }


    /// returns the generator set that is actually used
    template <template<typename> class ALLOC>
    INLINE DBRowGeneratorSet<ALLOC>&
    DBRowGeneratorParser<ALLOC>::generatorSet() {
      return generator_set__;
    }

    
    /// sets the area in the database the handler will handle
    template <template<typename> class ALLOC>
    INLINE void DBRowGeneratorParser<ALLOC>::setRange(std::size_t begin,
                                                      std::size_t end) {
      handler__.setRange (begin,end);
    }
    
    
    /// returns the generator set that is actually used
    template <template<typename> class ALLOC>
    INLINE const DBRowGeneratorSet<ALLOC>&
    DBRowGeneratorParser<ALLOC>::generatorSet() const {
      return generator_set__;
    }

    
    /** @brief sets the columns of interest: the output DBRow needs only 
     * contain values fot these columns */
    template <template<typename> class ALLOC>
    INLINE void DBRowGeneratorParser<ALLOC>::setColumnsOfInterest (
      const std::vector<std::size_t,ALLOC<std::size_t>>& cols_of_interest ) {
      generator_set__.setColumnsOfInterest ( cols_of_interest );
    }

    
    /** @brief sets the columns of interest: the output DBRow needs only 
     * contain values fot these columns */
    template <template<typename> class ALLOC>
    INLINE void DBRowGeneratorParser<ALLOC>::setColumnsOfInterest (
      std::vector<std::size_t,ALLOC<std::size_t>>&& cols_of_interest ) {
      generator_set__.setColumnsOfInterest ( std::move ( cols_of_interest ) );
    }


    /// assign a new Bayes net to all the generators that depend on a BN
    template <template<typename> class ALLOC>
    template < typename GUM_SCALAR >
    INLINE void DBRowGeneratorParser<ALLOC>::setBayesNet (
                const BayesNet<GUM_SCALAR>& new_bn) {
      generator_set__.setBayesNet(new_bn);
    }
    
      
    /// returns the allocator used
    template <template<typename> class ALLOC>
    INLINE typename DBRowGeneratorParser<ALLOC>::allocator_type
    DBRowGeneratorParser<ALLOC>::getAllocator () const {
      return generator_set__.getAllocator ();
    }
    

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
