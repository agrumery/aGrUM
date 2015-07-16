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
 * @brief d-separation analysis (as described in Koller & Friedman 2009)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_D_SEPARATION_H
#define GUM_D_SEPARATION_H


#include <agrum/config.h>
#include <agrum/BN/IBayesNet.h>


namespace gum {

  /** @class dSeparation
   * @brief the d-separation algorithm as described in Koller & Friedman (2009)
   * @ingroup bn_group
   */
  class dSeparation {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    dSeparation ();

    /// copy constructor
    dSeparation ( const dSeparation& from );

    /// move constructor
    dSeparation ( dSeparation&& from );

    /// destructor
    ~dSeparation ();

    /// @}

    
    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    dSeparation& operator= ( const dSeparation& from );

    /// move operator
    dSeparation& operator= ( dSeparation&& from );

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /** @brief Fill the 'requisite' nodeset with the requisite nodes in dag
     * given a query and evidence.
     *
     * Requisite nodes are those that are d-connected to at least one of the
     * query nodes given a set of hard and soft evidence
     */
    void requisiteNodes( const DAG& dag,
                         const NodeSet& query,
                         const NodeSet& hardEvidence,
                         const NodeSet& softEvidence,
                         NodeSet& requisite );

    /** @brief update a set of potentials, keeping only those d-connected with
     * query variables given evidence */
    template <typename GUM_SCALAR, template <typename> class TABLE>
    void relevantPotentials( const IBayesNet<GUM_SCALAR>& bn,
                             const NodeSet& query,
                             const NodeSet& hardEvidence,
                             const NodeSet& softEvidence,
                             Set<const TABLE<GUM_SCALAR>*>& potentials );

    /// @}
    
  };
  

} /* namespace gum */


#ifndef GUM_NO_INLINE
#include <agrum/BN/inference/dSeparation.inl>
#endif  // GUM_NO_INLINE

#include <agrum/BN/inference/dSeparation.tcc>


#endif /* GUM_D_SEPARATION_H */
