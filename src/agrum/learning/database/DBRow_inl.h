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
 * @brief The class representing rows in tabular database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/database/DBRow.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE DBRow::DBRow() {}

    /// constructor with a given size for the row
    INLINE
    DBRow::DBRow( Size size, const DBCell& default_cell, double weight )
        : _row( size, default_cell )
        , _weight( weight ) {}

    /// initializer list constructor
    INLINE DBRow::DBRow( std::initializer_list<DBCell>& list )
        : _row( list ) {}

    /// initializer from a vector of cells
    INLINE DBRow::DBRow( const std::vector<DBCell>& cells, double weight )
        : _row( cells )
        , _weight( weight ) {}

    /// initializer from a vector of cells
    INLINE DBRow::DBRow( std::vector<DBCell>&& cells, double weight )
        : _row( std::move( cells ) )
        , _weight( weight ) {}

    /// copy operator
    INLINE DBRow::DBRow( const DBRow& from )
        : _row( from._row )
        , _weight( from._weight ) {}

    /// move operator
    INLINE DBRow::DBRow( DBRow&& from )
        : _row( std::move( from._row ) )
        , _weight( from._weight ) {}

    /// destructor
    INLINE DBRow::~DBRow() {}

    /// copy operator
    INLINE DBRow& DBRow::operator=( const DBRow& from ) {
      if ( this != &from ) {
        _row = from._row;
        _weight = from._weight;
      }
      return *this;
    }

    /// move operator
    INLINE DBRow& DBRow::operator=( DBRow&& from ) {
      if ( this != &from ) {
        _row = std::move( from._row );
        _weight = from._weight;
      }
      return *this;
    }

    /// returns the ith DBCell of the row
    INLINE DBCell& DBRow::operator[]( Idx i ) { return _row[i]; }

    /// returns the ith DBCell of the row
    INLINE const DBCell& DBRow::operator[]( Idx i ) const { return _row[i]; }

    /// returns the current row
    INLINE const std::vector<DBCell>& DBRow::row() const noexcept { return _row; }

    /// returns the current row
    INLINE std::vector<DBCell>& DBRow::row() noexcept { return _row; }

    /// returns the weight
    INLINE const double& DBRow::weight() const noexcept { return _weight; }

    /// returns the weight
    INLINE double& DBRow::weight() noexcept { return _weight; }

    /// sets a new row
    INLINE void DBRow::setRow( const std::vector<DBCell>& new_row ) {
      _row = new_row;
    }

    /// sets a new row
    INLINE void DBRow::setRow( std::vector<DBCell>&& new_row ) {
      _row = std::move( new_row );
    }

    /// sets a new weight
    INLINE void DBRow::setWeight( double new_weight ) { _weight = new_weight; }

    /// returns the size of the row
    INLINE Size DBRow::size() const noexcept { return Size( _row.size() ); }

    /// resize a given row
    INLINE void DBRow::resize( Size new_size ) { _row.resize( new_size ); }

  } /* namespace learning */

} /* namespace gum */
