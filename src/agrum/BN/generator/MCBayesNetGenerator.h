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
 * @brief Class for generating bayesian networks.using MC algorithm
 * cf. [Ide and Cozman, 2002]
 *
 * @author Ariele MAESANO
 */

#ifndef MCBAYESNETGENERATOR
#define MCBAYESNETGENERATOR

#include <set>
#include <vector>
#include <iostream>
#include <fstream>

#include <sstream>

#include <dirent.h>
#include <sys/time.h>


#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/CPTGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>

#include <agrum/multidim/labelizedVariable.h>

namespace gum {

  template<typename T_DATA> class MCBayesNetGenerator {

    public:
      MCBayesNetGenerator( Size nbNodes,Size max_modality=2, Size max_parents=2, double max_density=1.0 );
      MCBayesNetGenerator( CPTGenerator* cptGenerator,Size nbNodes, Size max_modality=2, Size max_parents=2, double max_density=1.0 );
      ~MCBayesNetGenerator();



      BayesNet<T_DATA>* generateBN( gum::Idx iteration, gum::Idx p,gum::Idx q );

      BayesNet<T_DATA>* modifyingExistingBN( BayesNet<T_DATA> * bayesNetinit, gum::Idx iteration, gum::Idx p,gum::Idx q );
    private:

      gum::Idx c;
      CPTGenerator* __cptGenerator;
      gum::BayesNet<double> * __bayesNet;
      Size __nbNodes;
      Size __max_parents;
      double __max_density;
      Size __max_modality;

      void __PMMx( gum::Idx iteration, gum::Idx p,gum::Idx q ) ;
      bool __isPolytree();
      bool __connect( const gum::NodeId i, const gum::NodeId j );
      bool __directedPath( const gum::NodeId i, const gum::NodeId j );
      void __PMMx_poly( gum::Idx iteration, gum::Idx p,gum::Idx q ) ;
      void __PMMx_multi( gum::Idx iteration, gum::Idx p,gum::Idx q );
      void __jump_poly() ;
      void __jump_multi() ;
      void __AorR();
      void __AR() ;
      std::vector<gum::Idx> * __createPartDAG( gum::Size BNSize, gum::Size iniRoot );
      void __createDAG( gum::Size BNSize, gum::Size iniRoot );
      bool __connect( const gum::NodeId i, const gum::NodeId j, gum::NodeSet &excluded );

      bool __directedPath( const gum::NodeId tail, const gum::NodeId head, gum::NodeSet &excluded );
      void __choosenodes( gum::NodeId &i,gum::NodeId &j );
      void __chooseclosenodes( gum::NodeId &i,gum::NodeId &j );
      void __transformPoly( gum::Idx nbiter );


      void __createTree( gum::Size BNSize ) ;
      NodeId __createPartTree( gum::Size BNSize ) ;


  };
}

#include <agrum/BN/generator/MCBayesNetGenerator.tcc>
#endif // MCBAYESNETGENERATOR
