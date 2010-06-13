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

#define GET_PATH_STR(x) "/home/phw/Documents/svn/agrum/trunk/src/testunits/ressources/" #x

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <agrum/BN/inference/ShaferShenoyInference.h>
//#include <agrum/BN/inference/FastSSInference.h>
#include <agrum/BN/inference/Gibbs.h>
#include <agrum/BN/inference/lazyPropagation.h>
//#include <agrum/BN/inference/fastLazyPropagation.h>

#include <agrum/core/timer.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define TEST_INFERENCE(x,i) {                                     \
std::cout<<"==================="<<std::endl<<#x<<std::endl;     \
    t.reset();                                                  \
    gum::x<float> inf( *net );                                  \
    inf.makeInference();                                        \
    std::cout<<(inf.marginal(i))<<std::endl                     \
             <<"Timing : "<<t.step()<<std::endl                 \
             <<"==================="<<std::endl<<std::endl;     \
    }
    
namespace run_demo {
  void f() {    
    std::cout<<"\n\n==========\nAlarm.bif\n";
    gum::Timer t;
    std::string file = GET_PATH_STR( alarm.bif );
    gum::BayesNet<float> *net = new gum::BayesNet<float>();
    gum::BIFReader<float> reader( net, file );
    
    t.reset();
    reader.proceed();
    GUM_TRACE_VAR(*net);
    std::cout<<"reading BN : "<<t.step()<<std::endl;

    int i=0;
    //TEST_INFERENCE(Gibbs);
//    for(gum::NodeId i=0;i<10;++i) {
      TEST_INFERENCE(Gibbs,i);
      TEST_INFERENCE(LazyPropagation,i);
      TEST_INFERENCE(ShaferShenoyInference,i);
//    }
  }
  void g() {    
    std::cout<<"\n\n========\nLink.bif\n";
    gum::Timer t;
    std::string file = GET_PATH_STR( Link.bif );
    gum::BayesNet<float> *net = new gum::BayesNet<float>();
    gum::BIFReader<float> reader( net, file );
    
    t.reset();
    reader.proceed();
    GUM_TRACE_VAR(*net);
    std::cout<<"reading BN : "<<t.step()<<std::endl;

    int i=0;
    //TEST_INFERENCE(Gibbs);
//    for(gum::NodeId i=0;i<10;++i) {
      TEST_INFERENCE(Gibbs,i);
      TEST_INFERENCE(LazyPropagation,i);
      TEST_INFERENCE(ShaferShenoyInference,i);
//    }
  }
}

int main( void ) {
  run_demo::f();
  run_demo::g();

  gum::__atexit();
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
