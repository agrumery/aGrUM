/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
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
// ============================================================================
#include <cxxtest/AgrumTestSuite.h>
// ============================================================================
#include <agrum/prm/skoor/SkoorInterpreter.h>
// ============================================================================
#include <iostream>
#include <string>

namespace gum {
namespace tests {

using namespace gum;
using namespace gum::prm;
using namespace gum::prm::skoor;

class SkoorTestSuite: public CxxTest::TestSuite {
  public:
    void setUp() {
      //std::cerr << std::endl;
    }

    void tearDown() {
    }

    void testParseFile1() {
      SkoorInterpreter * si = new SkoorInterpreter();
      si->setSyntaxMode(true);
      si->addPath("../../../src/testunits/ressources/skoor/");
      
      TS_GUM_ASSERT_THROWS_NOTHING( si->interpretFile("../../../src/testunits/ressources/skoor/requests/query1.skoor") );
      
      TS_ASSERT_EQUALS( si->errors(), 0 );
      TS_ASSERT_EQUALS( si->warnings(), 0 );  
      
      delete si;
    }
    
    
    void testParseFile2() {
      SkoorInterpreter * si = new SkoorInterpreter();
      si->setSyntaxMode(true);
      si->addPath("../../../src/testunits/ressources/skoor/");
      
      TS_GUM_ASSERT_THROWS_NOTHING( si->interpretFile("../../../src/testunits/ressources/skoor/requests/query2.skoor") );
      
      TS_ASSERT_EQUALS( si->errors(), 1 );
      TS_ASSERT_EQUALS( si->warnings(), 0 );  
      
      delete si;
    }
    

    void testInference() {
      SkoorInterpreter * si = new SkoorInterpreter();
      si->setSyntaxMode(false);
      si->addPath("../../../src/testunits/ressources/skoor/");
      
      TS_GUM_ASSERT_THROWS_NOTHING( si->interpretFile("../../../src/testunits/ressources/skoor/requests/query1.skoor") );
      
      TS_ASSERT_EQUALS( si->errors(), 0 );
      TS_ASSERT_EQUALS( si->warnings(), 0 );
      
      delete si;
    }
    
    
    void testObserve() {
      SkoorInterpreter * si = new SkoorInterpreter();
      si->setSyntaxMode(false);
      si->addPath("../../../src/testunits/ressources/skoor/");
      
      TS_GUM_ASSERT_THROWS_NOTHING( si->interpretFile("../../../src/testunits/ressources/skoor/requests/queryObserveTest.skoor") );

      TS_ASSERT_EQUALS( si->errors(), 0 );
      TS_ASSERT_EQUALS( si->warnings(), 0 );
      
      // Observe correctly
      const Instance& c1 = si->prm()->system("systems.MyKickAssSystem").get("c1");
      const Instance& c2 = si->prm()->system("systems.MyKickAssSystem").get("c2");
      
      TS_ASSERT(   si->inference()->hasEvidence( PRMInference::Chain(&c1, &c1.get("can_print"))) );
      TS_ASSERT(   si->inference()->hasEvidence( PRMInference::Chain(&c2, &c2.get("equipState"))) );
      TS_ASSERT( ! si->inference()->hasEvidence( PRMInference::Chain(&c1, &c1.get("equipState"))) );
      TS_ASSERT( ! si->inference()->hasEvidence( PRMInference::Chain(&c2, &c2.get("can_print"))) ); 
      
      const PRMInference::EMap & e = si->inference()->evidence( c2 );
      const Potential<prm_float> * p = e[c1.get("equipState").id()];
      
      gum::Instantiation j( *p );
      const Attribute & c2_equipState = c2.get("equipState");
      for ( j.setFirst(); ! j.end(); j.inc() ) {
        if ( c2_equipState.type().variable().label( j.val(c2_equipState.type().variable()) ) == "Dysfunctional" ) {
          TS_ASSERT_EQUALS( p->get(j), 1.0 );
        } else {
          TS_ASSERT_EQUALS( p->get(j), 0.0 );
        }
      }
      
      delete si;
    }
    
    void testUnobserve() {
      SkoorInterpreter * si = new SkoorInterpreter();
      si->setSyntaxMode(false);
      si->addPath("../../../src/testunits/ressources/skoor/");
      
      TS_GUM_ASSERT_THROWS_NOTHING( si->interpretFile("../../../src/testunits/ressources/skoor/requests/queryUnobserveTest.skoor") );

      TS_ASSERT_EQUALS( si->errors(), 0 );
      TS_ASSERT_EQUALS( si->warnings(), 0 );
      
      // Unobserve correctly
      const Instance& c1 = si->prm()->system("systems.MyKickAssSystem").get("c1");
      const Instance& c2 = si->prm()->system("systems.MyKickAssSystem").get("c2");
      
      TS_ASSERT( ! si->inference()->hasEvidence( PRMInference::Chain(&c1, &c1.get("can_print"))) );
      TS_ASSERT( ! si->inference()->hasEvidence( PRMInference::Chain(&c2, &c2.get("equipState"))) );
      TS_ASSERT( ! si->inference()->hasEvidence( PRMInference::Chain(&c1, &c1.get("equipState"))) );
      TS_ASSERT( ! si->inference()->hasEvidence( PRMInference::Chain(&c2, &c2.get("can_print"))) );
      
      delete si;
    }
    
    void testQuery() {
      SkoorInterpreter * si = new SkoorInterpreter();
      si->setSyntaxMode(false);
      si->addPath("../../../src/testunits/ressources/skoor/");
      
      TS_GUM_ASSERT_THROWS_NOTHING( si->interpretFile("../../../src/testunits/ressources/skoor/requests/query1.skoor") );
      
      si->showElegantErrorsAndWarnings();
      TS_ASSERT_EQUALS( si->errors(), 0 );
      TS_ASSERT_EQUALS( si->warnings(), 0 );
      
      const Instance& c1 = si->prm()->system("systems.MyKickAssSystem").get("c1");
      const Instance& c2 = si->prm()->system("systems.MyKickAssSystem").get("c2");
      
      TS_ASSERT( ! si->inference()->hasEvidence( PRMInference::Chain(&c1, &c1.get("can_print"))) );
      TS_ASSERT(   si->inference()->hasEvidence( PRMInference::Chain(&c2, &c2.get("equipState"))) );
      TS_ASSERT( ! si->inference()->hasEvidence( PRMInference::Chain(&c1, &c1.get("equipState"))) );
      TS_ASSERT( ! si->inference()->hasEvidence( PRMInference::Chain(&c2, &c2.get("can_print"))) );
      
      // Est-ce que la valeur Dysfonctionnal de l'attribut can_print de l'instance c1 est à 1.0
      
      const PRMInference::EMap & e = si->inference()->evidence( c2 );
      const Potential<prm_float> * p = e[c1.get("equipState").id()];
      
      gum::Instantiation j( *p );
      const Attribute & c2_equipState = c2.get("equipState");
      for ( j.setFirst(); ! j.end(); j.inc() ) {
        if ( c2_equipState.type().variable().label( j.val(c2_equipState.type().variable()) ) == "Dysfunctional" ) {
          TS_ASSERT_EQUALS( p->get(j), 1.0 );
        } else {
          TS_ASSERT_EQUALS( p->get(j), 0.0 );
        }
      }  
      
      delete si;
    }
};

} // namespace tests
} // namespace gum
// ============================================================================
