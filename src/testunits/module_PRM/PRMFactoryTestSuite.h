
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/PRM/PRMFactory.h>
#include <agrum/PRM/elements/PRMClassElement.h>
#include <agrum/PRM/elements/PRMParameter.h>

namespace gum_tests {
  class PRMFactoryTestSuite : public CxxTest::TestSuite {
    public:
    void testInit() {
      gum::prm::PRMFactory< double > f;
      delete f.prm();
    }

    void testAddParameter() {
      try {
        gum::prm::PRMFactory< double > f;
        auto                           prm = f.prm();

        f.startClass("MyClass");
        f.addParameter("real", "lambda", 0.001);
        f.endClass();

        const auto& c = prm->getClass("MyClass");
        TS_ASSERT(1 == c.parameters().size());
        const auto& elt = c.get("lambda");
        TS_ASSERT(gum::prm::PRMClassElement< double >::isParameter(elt));
        const auto& lambda =
           static_cast< const gum::prm::PRMParameter< double >& >(elt);
        TS_ASSERT(lambda.valueType() == gum::prm::PRMParameter< double >::REAL);
        TS_ASSERT(lambda.value() == 0.001);

        delete prm;

      } catch (gum::Exception& e) {
        std::cout << e.errorType() << std::endl;
        std::cout << e.errorContent() << std::endl;
        std::cout << e.errorCallStack() << std::endl;

        TS_ASSERT(false);
      }
    }

    void testParameterSubClass() {
      try {
        gum::prm::PRMFactory< double > f;
        auto                           prm = f.prm();

        f.startClass("MyClass");
        f.addParameter("real", "lambda", 0.001);
        f.endClass();

        TS_ASSERT(prm->classes().size() == 1);

        f.startSystem("MySystem");
        f.addInstance("MyClass", "i");
        f.endSystem();

        TS_ASSERT(prm->classes().size() == 2);

        const auto& super_c = prm->getClass("MyClass");
        const auto& c = prm->getClass("MyClass<lambda=0.001>");

        TS_ASSERT(c.isSubTypeOf(super_c));

        TS_ASSERT(1 == c.parameters().size());
        const auto& elt = c.get("lambda");
        TS_ASSERT(gum::prm::PRMClassElement< double >::isParameter(elt));
        const auto& lambda =
           static_cast< const gum::prm::PRMParameter< double >& >(elt);
        TS_ASSERT(lambda.valueType() == gum::prm::PRMParameter< double >::REAL);
        TS_ASSERT(lambda.value() == 0.001);

        delete prm;

      } catch (gum::Exception& e) {
        std::cout << e.errorType() << std::endl;
        std::cout << e.errorContent() << std::endl;
        std::cout << e.errorCallStack() << std::endl;

        TS_ASSERT(false);
      }
    }

    void testParameterInstantiation() {
      try {
        gum::prm::PRMFactory< double > f;
        auto                           prm = f.prm();

        f.startClass("MyClass");
        f.addParameter("real", "lambda", 0.001);
        f.endClass();

        f.startSystem("MySystem");
        f.addInstance("MyClass", "i");
        f.endSystem();

        const auto& s = prm->getSystem("MySystem");

        TS_ASSERT(s.exists("i"));

        const auto& i = s.get("i");

        const auto& c = prm->getClass("MyClass<lambda=0.001>");
        TS_ASSERT(c == i.type());

        delete prm;

      } catch (gum::Exception& e) {
        std::cout << e.errorType() << std::endl;
        std::cout << e.errorContent() << std::endl;
        std::cout << e.errorCallStack() << std::endl;

        TS_ASSERT(false);
      }
    }

    void testParameterSpecificInstantiation() {
      try {
        gum::prm::PRMFactory< double > f;
        auto                           prm = f.prm();

        f.startClass("MyClass");
        f.addParameter("real", "lambda", 0.001);
        f.endClass();

        f.startSystem("MySystem");
        gum::HashTable< std::string, double > params;
        params.insert("lambda", 0.009);
        f.addInstance("MyClass", "i", params);
        f.addInstance("MyClass", "j");
        f.addInstance("MyClass", "k", params);

        TS_ASSERT(prm->classes().size() == 3);

        f.endSystem();
        const auto& s = prm->getSystem("MySystem");

        TS_ASSERT(s.exists("i"));

        const auto& i = s.get("i");
        const auto& j = s.get("j");
        const auto& k = s.get("k");

        const auto& super_c = prm->getClass("MyClass");
        const auto& c = prm->getClass("MyClass<lambda=0.009>");

        const auto& c_default = prm->getClass("MyClass<lambda=0.001>");

        TS_ASSERT(c.isSubTypeOf(super_c));
        TS_ASSERT(c == i.type());
        TS_ASSERT(c == k.type());

        TS_ASSERT(c_default.isSubTypeOf(super_c));
        TS_ASSERT(c_default == j.type());

        delete prm;

      } catch (gum::Exception& e) {
        std::cout << e.errorType() << std::endl;
        std::cout << e.errorContent() << std::endl;
        std::cout << e.errorCallStack() << std::endl;

        TS_ASSERT(false);
      }
    }
  };
}   // namespace gum_tests
