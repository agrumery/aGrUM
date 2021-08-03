/***************************************************************************
 *   Copyright (c) 2005-2020 by Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)  *
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
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
      :  _handler_(handler)
      ,  _generator_set_(generator_set, alloc)
      ,  _generator_size_ ( generator_set.size () ) {
      GUM_CONSTRUCTOR(DBRowGeneratorParser);
    }
    

    /// copy constructor with a given allocator
    template <template<typename> class ALLOC>
    DBRowGeneratorParser<ALLOC>::DBRowGeneratorParser(
      const DBRowGeneratorParser<ALLOC>& from,
      const typename DBRowGeneratorParser<ALLOC>::allocator_type& alloc )
      :  _handler_(from. _handler_)
      ,  _generator_set_(from. _generator_set_)
      ,  _generator_size_(from. _generator_size_) {
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
      :  _handler_ ( std::move(from. _handler_) )
      ,  _generator_set_(std::move(from. _generator_set_))
      ,  _generator_size_(from. _generator_size_) {
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
         _handler_ = from. _handler_;
         _generator_set_ = from. _generator_set_;
         _generator_size_ = from. _generator_size_;
      }
      return *this;
    }

    
    /// move operator
    template <template<typename> class ALLOC>
    INLINE DBRowGeneratorParser<ALLOC>&
    DBRowGeneratorParser<ALLOC>::operator=(DBRowGeneratorParser<ALLOC>&& from) {
      if (this != &from) {
         _handler_ = std::move(from. _handler_);
         _generator_set_ = std::move(from. _generator_set_);
         _generator_size_ = from. _generator_size_;
      }
      return *this;
    }

    
    /// returns true if there are still rows that can be output by the RowFilter
    template <template<typename> class ALLOC>
    INLINE bool DBRowGeneratorParser<ALLOC>::hasRows() {
      if ( !  _generator_size_ ) return  _handler_.hasRows();

      while ( !  _generator_set_.hasRows() ) {
        if ( !  _handler_.hasRows () ) return false;
         _generator_set_.setInputRow( * _handler_ );
        ++ _handler_;
      }
      return true;
    }

    
    /// returns a new output row with its corresponding weight
    template <template<typename> class ALLOC>
    INLINE const DBRow<DBTranslatedValue,ALLOC>&
    DBRowGeneratorParser<ALLOC>::row() {
      // if there exists no generator in the generator set, we just return
      // the row pointed to by the handler
      if ( !  _generator_size_ ) {
        const auto& pointed_row = * _handler_;
        ++ _handler_;
        return pointed_row;
      }
      else {
        return  _generator_set_.generate();
      }
    }
    

    /// resets the filter
    template <template<typename> class ALLOC>
    INLINE void DBRowGeneratorParser<ALLOC>::reset() {
       _handler_.reset();
       _generator_set_.reset();
    }
    

    // returns the handler used by the filter
    template <template<typename> class ALLOC>
    INLINE typename DatabaseTable<ALLOC>::Handler&
    DBRowGeneratorParser<ALLOC>::handler() {
      return  _handler_;
    }
    

    // returns the handler used by the filter
    template <template<typename> class ALLOC>
    INLINE const typename DatabaseTable<ALLOC>::Handler&
    DBRowGeneratorParser<ALLOC>::handler() const {
      return  _handler_;
    }

    
    /// returns a reference on the database
    template <template<typename> class ALLOC>
    INLINE const DatabaseTable< ALLOC >&
    DBRowGeneratorParser<ALLOC>::database () const {
      return static_cast<const DatabaseTable< ALLOC >&>
        ( _handler_.database ());
    }


    /// returns the generator set that is actually used
    template <template<typename> class ALLOC>
    INLINE DBRowGeneratorSet<ALLOC>&
    DBRowGeneratorParser<ALLOC>::generatorSet() {
      return  _generator_set_;
    }

    
    /// sets the area in the database the handler will handle
    template <template<typename> class ALLOC>
    INLINE void DBRowGeneratorParser<ALLOC>::setRange(std::size_t begin,
                                                      std::size_t end) {
       _handler_.setRange (begin,end);
    }
    
    
    /// returns the generator set that is actually used
    template <template<typename> class ALLOC>
    INLINE const DBRowGeneratorSet<ALLOC>&
    DBRowGeneratorParser<ALLOC>::generatorSet() const {
      return  _generator_set_;
    }

    
    /** @brief sets the columns of interest: the output DBRow needs only 
     * contain values fot these columns */
    template <template<typename> class ALLOC>
    INLINE void DBRowGeneratorParser<ALLOC>::setColumnsOfInterest (
      const std::vector<std::size_t,ALLOC<std::size_t>>& cols_of_interest ) {
       _generator_set_.setColumnsOfInterest ( cols_of_interest );
    }

    
    /** @brief sets the columns of interest: the output DBRow needs only 
     * contain values fot these columns */
    template <template<typename> class ALLOC>
    INLINE void DBRowGeneratorParser<ALLOC>::setColumnsOfInterest (
      std::vector<std::size_t,ALLOC<std::size_t>>&& cols_of_interest ) {
       _generator_set_.setColumnsOfInterest ( std::move ( cols_of_interest ) );
    }


    /// assign a new Bayes net to all the generators that depend on a BN
    template <template<typename> class ALLOC>
    template < typename GUM_SCALAR >
    INLINE void DBRowGeneratorParser<ALLOC>::setBayesNet (
                const BayesNet<GUM_SCALAR>& new_bn) {
       _generator_set_.setBayesNet(new_bn);
    }
    
      
    /// returns the allocator used
    template <template<typename> class ALLOC>
    INLINE typename DBRowGeneratorParser<ALLOC>::allocator_type
    DBRowGeneratorParser<ALLOC>::getAllocator () const {
      return  _generator_set_.getAllocator ();
    }
    

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
