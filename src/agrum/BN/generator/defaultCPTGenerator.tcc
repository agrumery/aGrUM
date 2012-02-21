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
 * @brief Source implementation of DefaultCPTGenerator.
 *
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI and Ariele-Paolo MAESANO
 *
 */
#include <agrum/BN/generator/defaultCPTGenerator.h>


namespace gum {


// Default constructor.
template <typename T_DATA> INLINE
DefaultCPTGenerator<T_DATA>::DefaultCPTGenerator() :
        AbstractCPTGenerator<T_DATA>() {
    GUM_CONSTRUCTOR( DefaultCPTGenerator );
}

// Destructor.
template <typename T_DATA> INLINE
DefaultCPTGenerator<T_DATA>::~DefaultCPTGenerator() {
    GUM_DESTRUCTOR( DefaultCPTGenerator );
}

// Generates a CPT using floats.
// @param varID The variable id of the CPT owner.
// @param cpt A reference on the CPT to fill.


template <typename T_DATA> void
DefaultCPTGenerator<T_DATA>::generateCPT( const Idx& varId, const Potential<T_DATA>& cpt ) {
    std::vector<T_DATA> v;
    srand( time( NULL ) );
    for ( Size i = 0; i < cpt.domainSize();  ++i ) {
        v.push_back(( T_DATA ) rand() );
    }
    cpt.fillWith( v );
    Instantiation varInst;
    varInst.add( cpt.variable( varId ) );
    Instantiation cptInst( cpt );
    for ( cptInst.setFirstOut( varInst ); !cptInst.end(); cptInst.incOut( varInst ) ) {
        T_DATA sum = ( T_DATA ) 0;
        for ( cptInst.setFirstIn( varInst ); !cptInst.end(); cptInst.incIn( varInst ) ) {
            sum += cpt[cptInst];
        }
        for ( cptInst.setFirstIn( varInst ); !cptInst.end(); cptInst.incIn( varInst ) ) {
            cpt.set( cptInst,cpt[cptInst] / sum );
        }
        cptInst.unsetEnd();
    }
}




} /* namespace gum */
