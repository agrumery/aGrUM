/***************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
*   {prenom.nom}_at_lip6.fr   *
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

#include <iostream>

#include <clocale>

#define GUM_TRACE_ON

#include <agrum/config.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/generator/defaultBayesNetGenerator.h>
#include <agrum/BN/inference/lazyPropagation.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace gum {
  template<typename GUM_SCALAR>
  class LoopyBeliefPropagation : BayesNetInference<GUM_SCALAR> {
    private:

      Property<Potential<float> *>::onArcs __lArcs;
      Property<Potential<float> *>::onArcs __pArcs;

    public:
      LoopyBeliefPropagation( const BayesNet<GUM_SCALAR>& bn ):BayesNetInference<GUM_SCALAR>( bn ) {

        const ArcSet& arcs=this->bn().dag().asArcs();

        for( ArcSet::const_iterator arc=arcs.begin(); arc!=arcs.end(); ++arc ) {
          NodeId j=arc->head();
          __lArcs.insert( *arc,new Potential<float>() );
          ( *__lArcs[*arc] )<<this->bn().variable( j );
          __pArcs.insert( *arc,new Potential<float>() );
          ( *__pArcs[*arc] )<<this->bn().variable( j );
        }
      }

      virtual void insertEvidence( const List<const Potential<GUM_SCALAR>*>& pot_list ) {};
      virtual void eraseEvidence( const Potential<GUM_SCALAR>* e ) {};
      virtual void eraseAllEvidence() {};
  };
}

int main( void ) {
  gum::BayesNet<float> bn;

  gum::BIFReader<float> reader( &bn, "tree.bif" ) );
  reader.trace( true );

  if( ! reader.proceed() ) {
    reader.showElegantErrorsAndWarnings();
    reader.showErrorCounts();
    return false;
  }

  GUM_TRACE_VAR( bn );

  gum::LoopyBeliefPropagation<float> inf( bn );
  // inf .makeInference();
//   for( gum::NodeId i=0; i<bn.size(); i++ ) {
//     std::cout<<i<<" : "<<bn.variable( i ).name()<<" => "<<inf.marginal( i )<<std::endl;
//   }
  gum::LazyPropagation<float> inf1( bn );
  inf1.makeInference();

  for( gum::NodeId i=0; i<bn.size(); i++ ) {
    std::cout<<i<<" : "<<bn.variable( i ).name()<<" => "<<inf1.marginal( i )<<std::endl;
  }

  gum::__atexit();

//     const gum::NodeSet& p=bn.dag().parents(i);
//     for (gum::NodeSet::const_iterator it=p.begin() ;;))
//         *it <-
}



#endif // DOXYGEN_SHOULD_SKIP_THIS
