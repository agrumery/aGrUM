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
 * @brief the base class for all the scores used for learning (BIC, BDeu, etc)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <cmath>

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter>
    INLINE Counter<IdSetAlloc, CountAlloc>::Counter(
        const RowFilter&         filter,
        const std::vector<Size>& var_modalities,
        Size                     min_range,
        Size                     max_range )
        : _modalities( var_modalities )
        , _record_counter( filter, var_modalities, min_range, max_range ) {
      // for debugging purposes
      GUM_CONSTRUCTOR( Counter );
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    Counter<IdSetAlloc, CountAlloc>::Counter(
        const Counter<IdSetAlloc, CountAlloc>& from )
        : _modalities( from._modalities )
        , _counts_computed( from._counts_computed )
        , _record_counter( from._record_counter ) {
      // copy the target nodesets
      _target_nodesets.reserve( from._target_nodesets.size() );
      for ( const auto set : from._target_nodesets ) {
        if ( set != nullptr ) {
          _target_nodesets.push_back(
              new std::pair<std::vector<Idx, IdSetAlloc>, Idx>( *set ) );
        } else {
          _target_nodesets.push_back( nullptr );
        }
      }

      // copy the conditioning nodesets
      _conditioning_nodesets.reserve( from._conditioning_nodesets.size() );
      for ( const auto set : from._conditioning_nodesets ) {
        if ( set != nullptr ) {
          _conditioning_nodesets.push_back(
              new std::pair<std::vector<Idx, IdSetAlloc>, Idx>( *set ) );
        } else {
          _conditioning_nodesets.push_back( nullptr );
        }
      }

      // now update the __nodesets of the record counter, as this one still
      // points to the nodesets of from
      std::vector<const std::vector<Idx, IdSetAlloc>*>& nodesets =
          _record_counter.__nodesets;
      const std::vector<const std::vector<Idx, IdSetAlloc>*>& from_nodesets =
          from._record_counter.__nodesets;
      Size size = Size( from._target_nodesets.size() );
      for ( Idx i = 0, j = 0; i < size; ++i ) {
        if ( from._target_nodesets[i] != nullptr ) {
          while ( from_nodesets[j] != &( from._target_nodesets[i]->first ) ) {
            ++j;
          }
          nodesets[j] = &( _target_nodesets[i]->first );
        }
      }
      size = Size( from._conditioning_nodesets.size() );
      for ( Idx i = 0, j = 0; i < size; ++i ) {
        if ( from._conditioning_nodesets[i] != nullptr ) {
          while ( from_nodesets[j] !=
                  &( from._conditioning_nodesets[i]->first ) ) {
            ++j;
          }
          nodesets[j] = &( _conditioning_nodesets[i]->first );
        }
      }
      // for debugging purposes
      GUM_CONS_CPY( Counter );
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    Counter<IdSetAlloc, CountAlloc>::Counter(
        Counter<IdSetAlloc, CountAlloc>&& from )
        : _modalities( from._modalities )
        , _counts_computed( std::move( from._counts_computed ) )
        , _record_counter( std::move( from._record_counter ) )
        , _target_nodesets( std::move( from._target_nodesets ) )
        , _conditioning_nodesets( std::move( from._conditioning_nodesets ) ) {
      from._target_nodesets.clear();
      from._conditioning_nodesets.clear();

      // for debugging purposes
      GUM_CONS_CPY( Counter );
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    Counter<IdSetAlloc, CountAlloc>::~Counter() {
      // for debugging purposes
      GUM_DESTRUCTOR( Counter );
      clear();
    }

    /// add an empty variable set to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Idx Counter<IdSetAlloc, CountAlloc>::addEmptyNodeSet() {
      _conditioning_nodesets.push_back( nullptr );
      _target_nodesets.push_back( nullptr );
      _counts_computed = false;

      return Idx( _target_nodesets.size() - 1 );
    }

    /// add a new single variable to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Idx Counter<IdSetAlloc, CountAlloc>::addNodeSet( Idx var ) {
      // create the target nodeset
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_target =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>(
              std::vector<Idx, IdSetAlloc>{var}, 0 );

      // assign to it its record counter index
      new_target->second = _record_counter.addNodeSet( new_target->first );

      // save it in the score's nodeset
      _conditioning_nodesets.push_back( nullptr );
      _target_nodesets.push_back( new_target );

      _counts_computed = false;

      return Idx( _target_nodesets.size() - 1 );
    }

    /// add a new target variable plus some conditioning vars
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Idx Counter<IdSetAlloc, CountAlloc>::addNodeSet(
        Idx var, const std::vector<Idx>& conditioning_ids ) {
      // if the conditioning set is empty, perform the unconditional addNodeSet
      if ( !conditioning_ids.size() ) {
        return Counter<IdSetAlloc, CountAlloc>::addNodeSet( var );
      }

      // create the target nodeset and assign to it its record counter index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_target =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>( conditioning_ids, 0 );
      new_target->first.push_back( var );
      new_target->second = _record_counter.addNodeSet( new_target->first );

      // create the conditioning nodeset and assign to it its record counter
      // index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_cond =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>( conditioning_ids, 0 );
      new_cond->second = _record_counter.addNodeSet( new_cond->first );

      // save it in the score's nodeset
      _conditioning_nodesets.push_back( new_cond );
      _target_nodesets.push_back( new_target );

      _counts_computed = false;

      return Idx( _target_nodesets.size() - 1 );
    }

    /// add a new target variable plus some conditioning vars
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Idx Counter<IdSetAlloc, CountAlloc>::addNodeSet(
        Idx var, std::vector<Idx>&& conditioning_ids ) {
      // if the conditioning set is empty, perform the unconditional addNodeSet
      if ( !conditioning_ids.size() ) {
        return Counter<IdSetAlloc, CountAlloc>::addNodeSet( var );
      }

      // create the target nodeset and assign to it its record counter index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_target =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>( conditioning_ids, 0 );
      new_target->first.push_back( var );
      new_target->second = _record_counter.addNodeSet( new_target->first );

      // create the conditioning nodeset and assign to it its record counter
      // index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_cond =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>(
              std::move( conditioning_ids ), 0 );
      new_cond->second = _record_counter.addNodeSet( new_cond->first );

      // save it in the score's nodeset
      _conditioning_nodesets.push_back( new_cond );
      _target_nodesets.push_back( new_target );

      _counts_computed = false;

      return Idx( _target_nodesets.size() - 1 );
    }

    /// add a new pair of target unconditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Idx Counter<IdSetAlloc, CountAlloc>::addNodeSet( Idx var1, Idx var2 ) {
      // create the target nodeset
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_target =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>(
              std::vector<Idx, IdSetAlloc>{var2, var1}, 0 );

      // assign to it its record counter index
      new_target->second = _record_counter.addNodeSet( new_target->first );

      // create the conditioning nodeset and assign to it its record counter
      // index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_cond =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>(
              std::vector<Idx, IdSetAlloc>{var2}, 0 );
      new_cond->second = _record_counter.addNodeSet( new_cond->first );

      // save it in the score's nodeset
      _conditioning_nodesets.push_back( new_cond );
      _target_nodesets.push_back( new_target );

      _counts_computed = false;

      return Idx( _target_nodesets.size() - 1 );
    }

    /// add a new pair of target unconditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Idx Counter<IdSetAlloc, CountAlloc>::addNodeSet(
        const std::pair<Idx, Idx>& vars ) {
      return addNodeSet( vars.first, vars.second );
    }

    /// add a new pair of target conditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Idx Counter<IdSetAlloc, CountAlloc>::addNodeSet(
        Idx var1, Idx var2, const std::vector<Idx>& conditioning_ids ) {
      // if the conditioning set is empty, perform the unconditional addNodeSet
      if ( !conditioning_ids.size() ) {
        return Counter<IdSetAlloc, CountAlloc>::addNodeSet( var1, var2 );
      }

      // create the target nodeset and assign to it its record counter index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_target =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>( conditioning_ids, 0 );
      new_target->first.push_back( var2 );
      new_target->first.push_back( var1 );
      new_target->second = _record_counter.addNodeSet( new_target->first );

      // create the conditioning nodeset and assign to it its record counter
      // index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_cond =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>( conditioning_ids, 0 );
      new_cond->first.push_back( var2 );
      new_cond->second = _record_counter.addNodeSet( new_cond->first );

      // save it in the score's nodeset
      _conditioning_nodesets.push_back( new_cond );
      _target_nodesets.push_back( new_target );

      _counts_computed = false;

      return Idx( _target_nodesets.size() - 1 );
    }

    /// add a new pair of target conditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Idx Counter<IdSetAlloc, CountAlloc>::addNodeSet(
        Idx var1, Idx var2, std::vector<Idx>&& conditioning_ids ) {
      // if the conditioning set is empty, perform the unconditional addNodeSet
      if ( !conditioning_ids.size() ) {
        return Counter<IdSetAlloc, CountAlloc>::addNodeSet( var1, var2 );
      }

      // create the target nodeset and assign to it its record counter index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_target =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>( conditioning_ids, 0 );
      new_target->first.push_back( var2 );
      new_target->first.push_back( var1 );
      new_target->second = _record_counter.addNodeSet( new_target->first );

      // create the conditioning nodeset and assign to it its record counter
      // index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_cond =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>(
              std::move( conditioning_ids ), 0 );
      new_cond->first.push_back( var2 );
      new_cond->second = _record_counter.addNodeSet( new_cond->first );

      // save it in the score's nodeset
      _conditioning_nodesets.push_back( new_cond );
      _target_nodesets.push_back( new_target );

      _counts_computed = false;

      return Idx( _target_nodesets.size() - 1 );
    }

    /// add a new pair of target conditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Idx Counter<IdSetAlloc, CountAlloc>::addNodeSet(
        const std::pair<Idx, Idx>& vars,
        const std::vector<Idx>& conditioning_ids ) {
      // if the conditioning set is empty, perform the unconditional addNodeSet
      if ( !conditioning_ids.size() ) {
        return Counter<IdSetAlloc, CountAlloc>::addNodeSet( vars.first,
                                                            vars.second );
      }

      // create the target nodeset and assign to it its record counter index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_target =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>( conditioning_ids, 0 );
      new_target->first.push_back( vars.second );
      new_target->first.push_back( vars.first );
      new_target->second = _record_counter.addNodeSet( new_target->first );

      // create the conditioning nodeset and assign to it its record counter
      // index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_cond =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>( conditioning_ids, 0 );
      new_cond->first.push_back( vars.second );
      new_cond->second = _record_counter.addNodeSet( new_cond->first );

      // save it in the score's nodeset
      _conditioning_nodesets.push_back( new_cond );
      _target_nodesets.push_back( new_target );

      _counts_computed = false;

      return Idx( _target_nodesets.size() - 1 );
    }

    /// add a new pair of target conditioned variables to be counted
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE Idx Counter<IdSetAlloc, CountAlloc>::addNodeSet(
        const std::pair<Idx, Idx>& vars, std::vector<Idx>&& conditioning_ids ) {
      // if the conditioning set is empty, perform the unconditional addNodeSet
      if ( !conditioning_ids.size() ) {
        return Counter<IdSetAlloc, CountAlloc>::addNodeSet( vars.first,
                                                            vars.second );
      }

      // create the target nodeset and assign to it its record counter index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_target =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>( conditioning_ids, 0 );
      new_target->first.push_back( vars.second );
      new_target->first.push_back( vars.first );
      new_target->second = _record_counter.addNodeSet( new_target->first );

      // create the conditioning nodeset and assign to it its record counter
      // index
      std::pair<std::vector<Idx, IdSetAlloc>, Idx>* new_cond =
          new std::pair<std::vector<Idx, IdSetAlloc>, Idx>(
              std::move( conditioning_ids ), 0 );
      new_cond->first.push_back( vars.second );
      new_cond->second = _record_counter.addNodeSet( new_cond->first );

      // save it in the score's nodeset
      _conditioning_nodesets.push_back( new_cond );
      _target_nodesets.push_back( new_target );

      _counts_computed = false;

      return Idx( _target_nodesets.size() - 1 );
    }

    /// clears all the data structures from memory
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void Counter<IdSetAlloc, CountAlloc>::clear() {
      _record_counter.clearNodeSets();

      for ( auto set : _target_nodesets )
        if ( set ) delete set;

      for ( auto set : _conditioning_nodesets )
        if ( set ) delete set;

      _target_nodesets.clear();

      _conditioning_nodesets.clear();

      _counts_computed = false;
    }

    /// perform the computation of the countings
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void Counter<IdSetAlloc, CountAlloc>::_count() {
      _record_counter.count();
      _counts_computed = true;
    }

    /// returns the counting vector for a given (conditioned) target set
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<double, CountAlloc>&
    Counter<IdSetAlloc, CountAlloc>::_getAllCounts( Idx index ) {
      if ( !_counts_computed ) _count();
      return _record_counter.getCounts( _target_nodesets[index]->second );
    }

    /// returns the counting vector for a conditioning set
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<double, CountAlloc>&
    Counter<IdSetAlloc, CountAlloc>::_getConditioningCounts( Idx index ) {
      if ( !_counts_computed ) _count();
      return _record_counter.getCounts( _conditioning_nodesets[index]->second );
    }

    /// returns the set of target + conditioning nodes
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<Idx, IdSetAlloc>&
    Counter<IdSetAlloc, CountAlloc>::_getAllNodes( Idx index ) const noexcept {
      return _target_nodesets[index]->first;
    }

    /// returns the set of conditioning nodes
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<Idx, IdSetAlloc>*
    Counter<IdSetAlloc, CountAlloc>::_getConditioningNodes( Idx index ) const
        noexcept {
      if ( _conditioning_nodesets[index] != nullptr ) {
        return &( _conditioning_nodesets[index]->first );
      } else {
        return nullptr;
      }
    }

    /// returns the modalities of the variables
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<Size>&
                 Counter<IdSetAlloc, CountAlloc>::modalities() const noexcept {
      return _modalities;
    }

    /// sets the maximum number of threads used to perform countings
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void
    Counter<IdSetAlloc, CountAlloc>::setMaxNbThreads( Size nb ) noexcept {
      _record_counter.setMaxNbThreads( nb );
    }

    /// returns all the countings performed (both targets and conditioned)
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE std::vector<std::vector<double, CountAlloc>>&
           Counter<IdSetAlloc, CountAlloc>::_getCounts() noexcept {
      if ( !_counts_computed ) _count();
      return _record_counter.__getCounts();
    }

    /// returns all the sets of target + cond nodes, and their counting indices
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<std::pair<std::vector<Idx, IdSetAlloc>, Idx>*>&
                 Counter<IdSetAlloc, CountAlloc>::_getAllNodes() const noexcept {
      return _target_nodesets;
    }

    /// returns all the sets of conditioning nodes
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const std::vector<std::pair<std::vector<Idx, IdSetAlloc>, Idx>*>&
                 Counter<IdSetAlloc, CountAlloc>::_getConditioningNodes() const noexcept {
      return _conditioning_nodesets;
    }


    /// sets the range of records taken into account by the counter
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE void Counter<IdSetAlloc, CountAlloc>::setRange( Size min_range,
                                                           Size max_range ) {
      return _record_counter.setRange( min_range, max_range );
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
