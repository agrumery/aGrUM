/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/PRM/PRMFactory.h>


namespace gum_tests {
  class PRMFactoryTestSuite : public CxxTest::TestSuite {
    public:

      void testInit() {
        gum::prm::PRMFactory<double > f;
        delete f.prm();
      }

      void testAddParameter() {
        gum::prm::PRMFactory<double > f;
        auto prm = f.prm();

        f.startClass("MyClass");
        f.addParameter(Parameter::REAL, "lambda", 0.001);
        f.endClass();

        auto c = prm->getClass("MyClass"); 
        TS_ASSERT( 1 == c.parameters().size() );
        auto elt = c.get("lamda");
        TS_ASSERT( elt.type() == prm_parameter );
        auto lambda = static_cast<const Parameter&>(elt);
        TS_ASSERT( lambda.type() == Parameter::REAL );
        TS_ASSERT( lambda.value == 0.0001 );

        delete prm;
      }

      void testParameterSubClass() {
        gum::prm::PRMFactory<double> f;
        auto prm = f.prm();

        f.startClass("MyClass");
        f.addParameter(Parameter::REAL, "lambda", 0.001);
        f.endClass();

        TS_ASSERT( c->classes().size() == 2 );

        auto super_c = prm->getClass("MyClass");
        auto c = prm->getClass("MyClass<lambda=0.001>");

        TS_ASSERT( c.isSubTypeOf(super_c) );

        TS_ASSERT( 1 == c.parameters().size() );
        auto elt = c.get("lamda");
        TS_ASSERT( elt.type() == prm_parameter );
        auto lambda = static_cast<const Parameter&>(elt);
        TS_ASSERT( lambda.type() == Parameter::REAL );
        TS_ASSERT( lambda.value == 0.0001 );

        delete prm;
      }

      void testParameterInstantiation() {
        gum::prm::PRMFactory<double> f;
        auto prm = f.prm();

        f.startClass("MyClass");
        f.addParameter(Parameter::REAL, "lambda", 0.001);
        f.endClass();

        f.startSystem("MySystem");
        f.addInstance("MyClass", "i");
        f.endSystem();

        auto s = prm->system("MySystem");

        TS_ASSERT( s.exists("i") );
        
        auto i = s.get("i");

        auto c = prm->getClass("MyClass<lambda=0.001>");
        TS_ASSERT( c == i.type() );
      }

      void testParameterSpecificInstantiation() {
        gum::prm::PRMFactory<double> f;
        auto prm = f.prm();

        f.startClass("MyClass");
        f.addParameter(Parameter::REAL, "lambda", 0.001);
        f.endClass();

        TS_ASSERT( c->classes().size() == 3 );

        f.startSystem("MySystem");
        Hashtable<std::string, double> params;
        params.insert("lambda", 0.009);
        f.addInstance("MyClass", "i", params);
        f.endSystem();

        auto s = prm->system("MySystem");

        TS_ASSERT( s.exists("i") );
        
        auto i = s.get("i");

        auto super_c = prm->getClass("MyClass");
        auto c = prm->getClass("MyClass<lambda=0.009>");

        TS_ASSERT( c.isSubTypeOf(super_c) );
        TS_ASSERT( c == i.type() );
      }
  };
}//gum_tests
