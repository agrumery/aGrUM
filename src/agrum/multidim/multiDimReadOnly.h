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
/** @file
 * @brief Abstract base class for all readonly multi dimensionnal 
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#ifndef GUM_MULTI_DIM_READONLY_H
#define GUM_MULTI_DIM_READONLY_H

#include <agrum/multidim/multiDimImplementation.h>


namespace gum {

  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                     GUM_MULTI_DIM_READONLY                     === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class MultiDimReadOnly
  * @brief Abstract base class for all multi dimensionnal read only structure
  * @ingroup multidim_group
  *
  */
  /* ============================================================================ */
  template<typename T_DATA>

  class MultiDimReadOnly : public MultiDimImplementation<T_DATA> {
    public:
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{
      // ============================================================================
      /// Default constructor.
      // ============================================================================
      MultiDimReadOnly();
      MultiDimReadOnly( const MultiDimReadOnly<T_DATA>& from );

      // ============================================================================
      /// Destructor.
      // ============================================================================
      virtual ~MultiDimReadOnly();

      /// @}


      // ############################################################################
      /// @name Accessors / Modifiers
      // ############################################################################
      /// @{
      // ============================================================================
    protected:
      virtual T_DATA& _get( const Instantiation &i ) const  {GUM_ERROR( OperationNotAllowed,"No (unconst) access to an aggregator" );};

    public:
      virtual void set( const Instantiation& i ,const T_DATA& value ) const {GUM_ERROR( OperationNotAllowed,"Write access to an aggregator" );};

      void fill( const T_DATA& ) const { GUM_ERROR( OperationNotAllowed,"Write access to an aggregator" ); };

      /// @}
  };
} /* namespace gum */

#include <agrum/multidim/multiDimReadOnly.tcc>


#endif /* GUM_MULTI_DIM_READONLY_H */

