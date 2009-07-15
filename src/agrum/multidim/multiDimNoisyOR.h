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
 * @brief Abstract base class for all multi dimensionnal aggregator
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#ifndef GUM_MULTI_DIM_NOISY_OR_H
#define GUM_MULTI_DIM_NOISY_OR_H

#include <agrum/multidim/multiDimReadOnly.h>


namespace gum {

  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                     GUM_MULTI_DIM_AGGREGATOR                     === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class MultiDimNoisyOR
  * @brief Noisy OR representation
  * @ingroup multidim_group
  *
  * Noisy-OR as described by Henrion (UAI-3, 1989, pp161-173)
  *
  * @warning
  *   - The first variable is assumed to be the NOISY-OR. The latter are the causes.
  *   - This code give probabilities for BINARY VARIABLES (other values are assumed to be of probability 0). But for optimization reason, we will never check if it is the case.
  */
  /* ============================================================================ */
  template<typename T_DATA>

  class MultiDimNoisyOR : public MultiDimReadOnly<T_DATA> {
    public:
      // ############################################################################
      /// @name Constructors / Destructors
      // ############################################################################
      /// @{
      // ============================================================================
      /// Default constructor.
      // ============================================================================
      MultiDimNoisyOR( T_DATA external_weight,T_DATA default_weight=( T_DATA )1.0 );
      MultiDimNoisyOR( const MultiDimNoisyOR<T_DATA>& from );

      // ============================================================================
      /// Destructor.
      // ============================================================================
      virtual ~MultiDimNoisyOR();

      /// @}


      // ############################################################################
      /// @name Accessors / Modifiers
      // ############################################################################
      /// @{
      // ============================================================================
    public:
      virtual T_DATA get( const Instantiation &i ) const;

      const std::string toString( void ) const;

      // @todo : optimisation with a always up-to-date value associated to each instantiation
      virtual void changeNotification( gum::Instantiation&, const gum::DiscreteVariable*, const gum::Idx&, const gum::Idx& ) {};

      virtual void setFirstNotification( gum::Instantiation& ) {};

      virtual void setLastNotification( gum::Instantiation& ) {};

      virtual void setIncNotification( gum::Instantiation& ) {};

      virtual void setDecNotification( gum::Instantiation& ) {};

      virtual void setChangeNotification( gum::Instantiation& ) {};

      const std::string toString( const gum::Instantiation* i ) const {return i->toString();};

      /// @return the real number of parameters used for this table. This function is used for compute @see compressionRatio()
      virtual Size realSize() const {return 0;};

      T_DATA causalWeight( const DiscreteVariable& v ) const;
      void causalWeight( const DiscreteVariable& v,T_DATA w );
      T_DATA externalWeight() const;
      void externalWeight( T_DATA w );


      /// @}
    protected:
      /// \f$ p_0 \f$ in Henrion (89).
      T_DATA __external_weight;

      /// @name causal weights
      /// \f$ P(e | c_i) \f$ in Henrion (89) in a hashtable with a default_value.
      /// @{
      T_DATA __default_weight;
      HashTable<const DiscreteVariable *,T_DATA> __causal_weights;
      /// @}
  };

  // ==============================================================================
  /// For friendly displaying the content of the array.
  // ==============================================================================
  template<typename T_DATA>
  std::ostream& operator<< ( std::ostream& s,
                             const MultiDimNoisyOR<T_DATA>& ag );


} /* namespace gum */

#include <agrum/multidim/multiDimNoisyOR.tcc>


#endif /* GUM_MULTI_DIM_NOISY_OR_H */

