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
 * @brief Class representing Bayesian networks
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#ifndef GUM_BAYES_NET_TPL_H
#define GUM_BAYES_NET_TPL_H

#include <utility>

#include <agrum/gumDebug.h>
#include <agrum/gumDiscreteVariable.h>
#include <agrum/gumGraphElements.h>
#include <agrum/gumExceptions.h>
#include <agrum/utils/gumHashTable.h>
#include <agrum/gumPotential.h>
#include <agrum/gumMultiDimArray.h>
#include <agrum/gumTopologyTpl.h>

/* ============================================================================== */
/* ===                 CLASS FOR REPRESENTING A BAYESIAN NETWORK              === */
/* ============================================================================== */
/** @class gumBayesNetTpl
 * @brief Class representing a Bayesian Network.
 *
 * After a variable is added to the BN, if it's domain size changes, then the
 * data in it's CPT is lost.
 *
 * For simplicity, building should follow these steps:
 *
 * 1) Instanciate your variables and add them to the BN
 * 2) Add all arcs needed
 * 3) Fill the variable's CPT table
 */

template<typename T_DATA>

class gumBayesNetTpl: public gumTopologyTpl {

  public:
// ##############################################################################
/// @name Constructors / Destructors
// ##############################################################################
/// @{

    /**
     * Default constructor.
     */
    gumBayesNetTpl();

    /**
     * Copy constructor.
     */
    gumBayesNetTpl( const gumBayesNetTpl<T_DATA>& source );

    /**
     * Destructor.
     */
    ~gumBayesNetTpl();

/// @}

// ##############################################################################
/// @name Network Manipulation Methods
// ##############################################################################
/// @{

    /**
     * Add a variable, it's associate node and it's CPT.
     *
     * The CPT is not initialized at 0 for all values.
     * @param variable The variable added by copy.
     * @param varId The id of the variable in the topology.
     * @throw gumDuplicateElement Raised if id is already a variable's id.
     * @return the id of the added variable.
     */
    //virtual gumId addVariable( const gumDiscreteVariable * const variable,gumId varId );

    /**
     * Add a variable, it's associate node and it's CPT. The id of the new
     * variable is automatically generated
     *
     * The CPT is not initialized at 0 for all values.
     * @param variable The variable added by copy.
     * @return the id of the added variable.
     */
    virtual gumId addVariable( const gumDiscreteVariable * const variable );

    /**
     * Erase a Variable from the network and update dependent CPTs (by removing
     * arcs).
     *
     * The variable and it's corresponding node are erased from the topology.
     * If no variable matches the id, then nothing is done.
     *
     * @param id The id of the variable to erase.
     */
    virtual void eraseVariable( gumId id );


    /**
     * Returns the CPT of a variable.
     *
     * @warning Do not use addNewDim !
     *
     * @param varId The CPT's variable id.
     * @return Returns a constant reference on the CPT.
     * @throw gumNotFound If no variable's id matches varId.
     */
    const gumPotential<T_DATA>& cpt( gumId varId ) const;

    /**
     * Add an arc in the BN, and update arc.head's CPT.
     *
     * @param head and
     * @param tail as gumId
     * @return Returns a reference on the real copy added.
     * @throw gumInvalidEdge If arc.tail and/or arc.head are not in the BN.
     */
    virtual const gumArc& insertArc( gumId tail, gumId head );

    /**
     * Removes an arc in the BN, and update head's CTP.
     *
     * If (tail, head) doesn't exist, the nothing happens.
     * @param tail The id of the node (variable) at the tail of the arc.
     * @param head The id of the node (variable) at the head of the arc.
     */
    virtual void eraseArc( gumId tail, gumId head );

    /**
     * Clear every variables, nodes, arcs, and CPTs of this BN.
     */
    virtual void clear();

    /**
     * @see gumDiscreteVariableListener
     */
    virtual void variableDomainChanged( gumDiscreteVariableEvent &e );

/// @}

// ##############################################################################
/// @name Operators
// ##############################################################################
/// @{

    /**
     * Copy operator.
     */
    gumBayesNetTpl<T_DATA>& operator=( const gumBayesNetTpl<T_DATA>& source );

/// @}

  private:
    // Mapping between the variable's id and their CPT.
    gumHashTable<gumId, gumPotential<T_DATA>* > __probaMap;

};

#include "gumBayesNetTpl.tcc"

#endif /* GUM_BAYES_NET_TPL_H */

