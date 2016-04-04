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
#include <cxxtest/testsuite_utils.h>

#include <sstream>

#include <agrum/core/hashTable.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/PRM/elements/system.h>

/**
 * This class is used to test gum::prm::PRMClassElement, since it is an abstrac
 * class, tests defined here should be called by each sub class of
 * gum::prm::PRMClassElement.
 */
namespace gum_tests {

  class PRMSystemTestSuite : public CxxTest::TestSuite {
    private:
    typedef gum::prm::PRMSystem<double> PRMSystem;
    typedef gum::prm::PRMInstance<double> PRMInstance;
    typedef gum::prm::PRMClass<double> PRMClass;
    typedef gum::prm::PRMType<double> PRMType;
    typedef gum::prm::PRMScalarAttribute<double> PRMAttribute;
    typedef gum::prm::PRMReferenceSlot<double> Reference;
    typedef gum::prm::PRMSlotChain<double> PRMSlotChain;

    PRMType* __boolean;
    PRMClass* __asia;
    gum::HashTable<std::string, gum::NodeId>* __nodeMap;

    public:
    void setUp() {
      __boolean = PRMType::boolean();
      __nodeMap = new gum::HashTable<std::string, gum::NodeId>();
      __asia = new PRMClass( "asia" );
      __buildAsiaBN();
    }

    void tearDown() {
      delete __boolean;
      delete __nodeMap;
      delete __asia;
    }

    void __buildAsiaBN() {
      __visitToAsia();
      __tuberculosis();
      __smoking();
      __lungCancer();
      __bronchitis();
      __tubOrCancer();
      __positiveXRay();
      __dyspnea();
    }

    void __visitToAsia() {
      std::string name = "visitToAsia";
      auto attr = new PRMAttribute( name, *__boolean );
      auto id = __asia->add( attr );
      std::vector<double> values{0.99, 0.01};
      attr->cpf().fillWith( values );
      __nodeMap->insert( name, id );
    }

    void __tuberculosis() {
      std::string name = "tuberculosis";
      auto attr = new PRMAttribute( name, *__boolean );
      auto id = __asia->add( attr );
      __asia->addArc( "visitToAsia", "tuberculosis" );
      std::vector<double> values{0.99, 0.95, 0.01, 0.05};
      attr->cpf().fillWith( values );
      __nodeMap->insert( name, id );
    }

    void __smoking() {
      std::string name = "smoking";
      auto attr = new PRMAttribute( name, *__boolean );
      auto id = __asia->add( attr );
      std::vector<double> values{0.50, 0.50};
      attr->cpf().fillWith( values );
      __nodeMap->insert( name, id );
    }

    void __lungCancer() {
      std::string name = "lungCancer";
      auto attr = new PRMAttribute( name, *__boolean );
      auto id = __asia->add( attr );
      __asia->addArc( "smoking", "lungCancer" );
      std::vector<double> values{0.99, 0.90, 0.01, 0.10};
      attr->cpf().fillWith( values );
      __nodeMap->insert( name, id );
    }

    void __bronchitis() {
      std::string name = "bronchitis";
      auto attr = new PRMAttribute( name, *__boolean );
      auto id = __asia->add( attr );
      __asia->addArc( "smoking", "bronchitis" );
      std::vector<double> values{0.70, 0.40, 0.30, 0.60};
      attr->cpf().fillWith( values );
      __nodeMap->insert( name, id );
    }

    void __tubOrCancer() {
      std::string name = "tubOrCancer";
      auto attr = new PRMAttribute( name, *__boolean );
      auto id = __asia->add( attr );
      __asia->addArc( "tuberculosis", "tubOrCancer" );
      __asia->addArc( "lungCancer", "tubOrCancer" );
      std::vector<double> values{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
      attr->cpf().fillWith( values );
      __nodeMap->insert( name, id );
    }

    void __positiveXRay() {
      std::string name = "positiveXRay";
      auto attr = new PRMAttribute( name, *__boolean );
      auto id = __asia->add( attr );
      __asia->addArc( "tubOrCancer", "positiveXRay" );
      std::vector<double> values{0.95, 0.02, 0.05, 0.98};
      attr->cpf().fillWith( values );
      __nodeMap->insert( name, id );
    }

    void __dyspnea() {
      std::string name = "dyspnea";
      auto attr = new PRMAttribute( name, *__boolean );
      auto id = __asia->add( attr );
      __asia->addArc( "tubOrCancer", "dyspnea" );
      __asia->addArc( "bronchitis", "dyspnea" );
      std::vector<double> values{0.9, 0.2, 0.3, 0.1, 0.1, 0.8, 0.7, 0.9};
      attr->cpf().fillWith( values );
      __nodeMap->insert( name, id );
    }

    void testClassConstruction() {
      TS_ASSERT_EQUALS( __asia->attributes().size(), (gum::Size)8 );
    }

    void testAddInstance() {
      // Arrange
      PRMSystem sys( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      // Act
      TS_ASSERT_THROWS_NOTHING( sys.add( inst ) );
      // Assert
      TS_ASSERT( sys.exists( "asia" ) );
      TS_ASSERT( sys.isInstantiated( *__asia ) );
      TS_ASSERT_EQUALS( sys.size(), (gum::Size)1 );
    }

    void testInstantiate() {
      // Arrange
      PRMSystem sys( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      sys.add( inst );
      // Act
      TS_ASSERT_THROWS_NOTHING( sys.instantiate() );
      // Assert
      TS_ASSERT( sys.exists( "asia" ) );
      TS_ASSERT( sys.isInstantiated( *__asia ) );
      TS_ASSERT_EQUALS( sys.size(), (gum::Size)1 );
    }

    void testGroundBN() {
      // Arrange
      std::string x0, y0, x1, y1;
      auto bn = new gum::BayesNet<double>( "asia" );
      {
        PRMSystem sys( "asia" );
        auto inst = new PRMInstance( "asia", *__asia );
        sys.add( inst );
        sys.instantiate();
        gum::BayesNetFactory<double> factory( bn );
        // Act
        TS_ASSERT_THROWS_NOTHING( sys.groundedBN( factory ) );
        x0 = bn->cpt( 0 ).toString();
        y0 = bn->cpt( 1 ).toString();
      }
      // Assert
      TS_ASSERT_EQUALS( bn->size(), (gum::Size)8 );
      TS_ASSERT_EQUALS( bn->sizeArcs(), (gum::Size)8 );
      x1 = bn->cpt( 0 ).toString();
      y1 = bn->cpt( 1 ).toString();
      TS_ASSERT_EQUALS( x0, x1 );
      TS_ASSERT_EQUALS( y0, y1 );
      delete bn;
    }

    void testGroundBNAfterDelete() {
      // Arrange
      PRMSystem* sys = new PRMSystem( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      sys->add( inst );
      sys->instantiate();
      auto bn = new gum::BayesNet<double>( "asia" );
      gum::BayesNetFactory<double> factory( bn );
      sys->groundedBN( factory );
      // Act
      TS_ASSERT_THROWS_NOTHING( delete sys );
      // Assert
      TS_ASSERT_EQUALS( bn->size(), (gum::Size)8 );
      TS_ASSERT_EQUALS( bn->sizeArcs(), (gum::Size)8 );
      for ( auto node : bn->dag() ) {
        const gum::Potential<double>* cpt = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING( cpt = &( bn->cpt( node ) ) );
        gum::Instantiation inst( *cpt );
        for ( inst.begin(); ! inst.end(); inst.inc() ) {
          TS_GUM_ASSERT_THROWS_NOTHING( cpt->get( inst ) );
        }
      }
      delete bn;
    }

    void testVisitToAsiaId() {
      // Arrange
      PRMSystem sys( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      sys.add( inst );
      sys.instantiate();
      auto bn = new gum::BayesNet<double>( "asia" );
      gum::BayesNetFactory<double> factory( bn );
      sys.groundedBN( factory );
      // Act
      TS_ASSERT_THROWS_NOTHING( bn->idFromName( "asia.(boolean)visitToAsia" ) );
      // Assert
      delete bn;
    }

    void testVisitToAsia() {
      // Arrange
      PRMSystem sys( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      sys.add( inst );
      sys.instantiate();
      auto bn = new gum::BayesNet<double>( "asia" );
      gum::BayesNetFactory<double> factory( bn );
      sys.groundedBN( factory );
      auto id = bn->idFromName( "asia.(boolean)visitToAsia" );
      std::vector<double> values;
      const auto& cpf = __asia->get( "visitToAsia" ).cpf();
      gum::Potential<double> const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( cpt = &( bn->cpt( id ) ) );
      // Assert
      gum::Instantiation i( cpf );
      gum::Instantiation j( cpt );
      for ( i.begin(), j.begin(); !( i.end() || j.end() );
            i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( cpf.get( i ), cpt->get( j ) );
      }
      delete bn;
    }

    void testTuberculosis() {
      // Arrange
      PRMSystem sys( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      sys.add( inst );
      sys.instantiate();
      auto bn = new gum::BayesNet<double>( "asia" );
      gum::BayesNetFactory<double> factory( bn );
      sys.groundedBN( factory );
      auto id = bn->idFromName( "asia.(boolean)tuberculosis" );
      std::vector<double> values;
      const auto& cpf = __asia->get( "tuberculosis" ).cpf();
      gum::Potential<double> const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( cpt = &( bn->cpt( id ) ) );
      // Assert
      gum::Instantiation i( cpf );
      gum::Instantiation j( cpt );
      for ( i.begin(), j.begin(); !( i.end() || j.end() );
            i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( cpf.get( i ), cpt->get( j ) );
      }
      delete bn;
    }

    void testSmoking() {
      // Arrange
      PRMSystem sys( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      sys.add( inst );
      sys.instantiate();
      auto bn = new gum::BayesNet<double>( "asia" );
      gum::BayesNetFactory<double> factory( bn );
      sys.groundedBN( factory );
      auto id = bn->idFromName( "asia.(boolean)smoking" );
      std::vector<double> values;
      const auto& cpf = __asia->get( "smoking" ).cpf();
      gum::Potential<double> const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( cpt = &( bn->cpt( id ) ) );
      // Assert
      gum::Instantiation i( cpf );
      gum::Instantiation j( cpt );
      for ( i.begin(), j.begin(); !( i.end() || j.end() );
            i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( cpf.get( i ), cpt->get( j ) );
      }
      delete bn;
    }

    void testLungCancer() {
      // Arrange
      PRMSystem sys( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      sys.add( inst );
      sys.instantiate();
      auto bn = new gum::BayesNet<double>( "asia" );
      gum::BayesNetFactory<double> factory( bn );
      sys.groundedBN( factory );
      auto id = bn->idFromName( "asia.(boolean)lungCancer" );
      std::vector<double> values;
      const auto& cpf = __asia->get( "lungCancer" ).cpf();
      gum::Potential<double> const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( cpt = &( bn->cpt( id ) ) );
      // Assert
      gum::Instantiation i( cpf );
      gum::Instantiation j( cpt );
      for ( i.begin(), j.begin(); !( i.end() || j.end() );
            i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( cpf.get( i ), cpt->get( j ) );
      }
      delete bn;
    }

    void testBronchitis() {
      // Arrange
      PRMSystem sys( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      sys.add( inst );
      sys.instantiate();
      auto bn = new gum::BayesNet<double>( "asia" );
      gum::BayesNetFactory<double> factory( bn );
      sys.groundedBN( factory );
      auto id = bn->idFromName( "asia.(boolean)bronchitis" );
      std::vector<double> values;
      const auto& cpf = __asia->get( "bronchitis" ).cpf();
      gum::Potential<double> const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( cpt = &( bn->cpt( id ) ) );
      // Assert
      gum::Instantiation i( cpf );
      gum::Instantiation j( cpt );
      for ( i.begin(), j.begin(); !( i.end() || j.end() );
            i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( cpf.get( i ), cpt->get( j ) );
      }
      delete bn;
    }

    void testTubOrCancer() {
      // Arrange
      PRMSystem sys( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      sys.add( inst );
      sys.instantiate();
      auto bn = new gum::BayesNet<double>( "asia" );
      gum::BayesNetFactory<double> factory( bn );
      sys.groundedBN( factory );
      auto id = bn->idFromName( "asia.(boolean)tubOrCancer" );
      std::vector<double> values;
      const auto& cpf = __asia->get( "tubOrCancer" ).cpf();
      gum::Potential<double> const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( cpt = &( bn->cpt( id ) ) );
      // Assert
      gum::Instantiation i( cpf );
      gum::Instantiation j( cpt );
      for ( i.begin(), j.begin(); !( i.end() || j.end() );
            i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( cpf.get( i ), cpt->get( j ) );
      }
      delete bn;
    }

    void testPositiveXRay() {
      // Arrange
      PRMSystem sys( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      sys.add( inst );
      sys.instantiate();
      auto bn = new gum::BayesNet<double>( "asia" );
      gum::BayesNetFactory<double> factory( bn );
      sys.groundedBN( factory );
      auto id = bn->idFromName( "asia.(boolean)positiveXRay" );
      std::vector<double> values;
      const auto& cpf = __asia->get( "positiveXRay" ).cpf();
      gum::Potential<double> const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( cpt = &( bn->cpt( id ) ) );
      // Assert
      gum::Instantiation i( cpf );
      gum::Instantiation j( cpt );
      for ( i.begin(), j.begin(); !( i.end() || j.end() );
            i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( cpf.get( i ), cpt->get( j ) );
      }
      delete bn;
    }

    void testDyspnea() {
      // Arrange
      PRMSystem sys( "asia" );
      auto inst = new PRMInstance( "asia", *__asia );
      sys.add( inst );
      sys.instantiate();
      auto bn = new gum::BayesNet<double>( "asia" );
      gum::BayesNetFactory<double> factory( bn );
      sys.groundedBN( factory );
      auto id = bn->idFromName( "asia.(boolean)dyspnea" );
      std::vector<double> values;
      const auto& cpf = __asia->get( "dyspnea" ).cpf();
      gum::Potential<double> const* cpt = nullptr;
      // Act
      TS_ASSERT_THROWS_NOTHING( cpt = &( bn->cpt( id ) ) );
      // Assert
      gum::Instantiation i( cpf );
      gum::Instantiation j( cpt );
      for ( i.begin(), j.begin(); !( i.end() || j.end() );
            i.inc(), j.inc() ) {
        TS_ASSERT_EQUALS( cpf.get( i ), cpt->get( j ) );
      }
      delete bn;
    }
  };

}  // gum_tests
