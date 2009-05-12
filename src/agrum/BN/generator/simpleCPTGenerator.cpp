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
 * @brief Source implementation of SimpleCPTGenerator.
 *
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI
 *
 */
#include <agrum/BN/generator/simpleCPTGenerator.h>


namespace gum {


  // Default constructor.
  SimpleCPTGenerator::SimpleCPTGenerator() {
    GUM_CONSTRUCTOR( SimpleCPTGenerator );
  }

  // Destructor.
  SimpleCPTGenerator::~SimpleCPTGenerator() {
    GUM_DESTRUCTOR( SimpleCPTGenerator );
  }

  // Generates a CPT using floats.
  // @param varID The variable id of the CPT owner.
  // @param cpt A reference on the CPT to fill.
  void
  SimpleCPTGenerator::generateCPT( const Idx& varId,
                                   const Potential<float>& cpt ) {
    std::vector<float> v;
    srand( time( NULL ) );

    for ( Size i = 0; i < cpt.domainSize();  ++i ) {
      v.push_back(( float ) rand() );
    }

    cpt.fillWith( v );

    Instantiation varInst;
    varInst.add( cpt.variable( varId ) );
    Instantiation cptInst( cpt );

    for ( cptInst.setFirstOut( varInst ); !cptInst.end(); cptInst.incOut( varInst ) ) {
      float sum = ( float ) 0;

      for ( cptInst.setFirstIn( varInst ); !cptInst.end(); cptInst.incIn( varInst ) ) {
        sum += cpt[cptInst];
      }

      for ( cptInst.setFirstIn( varInst ); !cptInst.end(); cptInst.incIn( varInst ) ) {
        cpt.set( cptInst,cpt[cptInst] / sum );
      }

      cptInst.unsetEnd();
    }
  }

  // Generates a CPT using doubles.
  // @param varID The variable id of the CPT owner.
  // @param cpt A reference on the CPT to fill.
  void
  SimpleCPTGenerator::generateCPT( const Idx& varId,
                                   const Potential<double>& cpt ) {
    std::vector<double> v;
    srand( time( NULL ) );

    for ( Size i = 0; i < cpt.domainSize();  ++i ) {
      v.push_back(( double ) rand() );
    }

    cpt.fillWith( v );

    Instantiation varInst;
    varInst.add( cpt.variable( varId ) );
    Instantiation cptInst( cpt );

    for ( cptInst.setFirstOut( varInst ); !cptInst.end(); cptInst.incOut( varInst ) ) {
      double sum = ( double ) 0;

      for ( cptInst.setFirstIn( varInst ); !cptInst.end(); cptInst.incIn( varInst ) ) {
        sum += cpt[cptInst];
      }

      for ( cptInst.setFirstIn( varInst ); !cptInst.end(); cptInst.incIn( varInst ) ) {
        cpt.set(cptInst,cpt[cptInst] / sum);
      }

      cptInst.unsetEnd();
    }
  }


} /* namespace gum */
