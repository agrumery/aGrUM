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
 *   (gumSize) 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.   *
 ***************************************************************************/
#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/maxParentsMCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/variables/labelizedVariable.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

class Size;
namespace gum_tests {
  int intest = 0;
  class BayesNetTestSuite : public CxxTest::TestSuite {

    private:
    void fillTopo( gum::BayesNet<float>& bn, gum::List<gum::NodeId>& idList ) {
      try {
        idList.insert( bn.add( *var1 ) );
        idList.insert( bn.add( *var2 ) );
        idList.insert( bn.add( *var3 ) );
        idList.insert( bn.add( *var4 ) );
        idList.insert( bn.add( *var5 ) );

        bn.addArc( idList[0], idList[2] );
        bn.addArc( idList[2], idList[4] );
        bn.addArc( idList[1], idList[3] );
        bn.addArc( idList[0], idList[3] );
        bn.addArc( idList[3], idList[4] );
        bn.addArc( idList[1], idList[4] );

      } catch ( gum::Exception& e ) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void fill( gum::BayesNet<float>& bn, gum::List<gum::NodeId>& idList ) {
      fillTopo( bn, idList );

      try {

        bn.cpt( idList[0] ).fillWith( {0.2f, 0.8f} );

        bn.cpt( idList[1] ).fillWith( {0.3f, 0.7f} );

        bn.cpt( idList[2] ).fillWith( {0.1f, 0.9f, 0.9f, 0.1f} );

        bn.cpt( idList[3] )
            .fillWith(  // clang-format off
                           {0.4f, 0.6f,
                            0.5f, 0.5f,
                            0.5f, 0.5f,
                            1.0f, 0.0f} );  // clang-format on

        bn.cpt( idList[4] )
            .fillWith(  // clang-format off
                      {0.3f,0.6f,0.1f,
                       0.5f,0.5f,0.0f,
                       0.5f,0.5f,0.0f,
                       1.0f,0.0f,0.0f,
                       0.4f,0.6f,0.0f,
                       0.5f,0.5f,0.0f,
                       0.5f,0.5f,0.0f,
                       0.0f,0.0f,1.0f} );  // clang-format on

      } catch ( gum::Exception& e ) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    gum::UndiGraph getRealMoralGraph( const gum::BayesNet<float>&   bn,
                                      const gum::List<gum::NodeId>& idList ) {
      gum::UndiGraph graph;
      graph.populateNodes( bn.dag() );

      graph.addEdge( idList[0], idList[2] );
      graph.addEdge( idList[0], idList[3] );
      graph.addEdge( idList[0], idList[1] );
      graph.addEdge( idList[2], idList[4] );
      graph.addEdge( idList[3], idList[4] );
      graph.addEdge( idList[2], idList[3] );
      graph.addEdge( idList[1], idList[3] );
      graph.addEdge( idList[1], idList[4] );
      graph.addEdge( idList[1], idList[2] );

      return graph;
    }

    public:
    gum::LabelizedVariable *var1, *var2, *var3, *var4, *var5;

    void setUp() {
      var1 = new gum::LabelizedVariable( "var1", "1", 2 );
      var2 = new gum::LabelizedVariable( "var2", "2", 2 );
      var3 = new gum::LabelizedVariable( "var3", "3", 2 );
      var4 = new gum::LabelizedVariable( "var4", "4", 2 );
      var5 = new gum::LabelizedVariable( "var5", "(gum::Size) 3", 3 );
    }

    void tearDown() {
      delete var1;
      delete var2;
      delete var3;
      delete var4;
      delete var5;
    }

    void testConstructor() {
      gum::BayesNet<float>* topology = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING( topology = new gum::BayesNet<float>() );
      TS_ASSERT_EQUALS( topology->size(), (gum::Idx)0 );
      TS_ASSERT_EQUALS( topology->sizeArcs(), (gum::Idx)0 );
      TS_ASSERT_EQUALS( topology->dim(), (gum::Idx)0 );

      TS_ASSERT_THROWS( topology->addArc( 1, 2 ), gum::InvalidNode );
      gum::List<gum::NodeId> idList;
      TS_GUM_ASSERT_THROWS_NOTHING( fill( *topology, idList ) );

      TS_ASSERT_THROWS( topology->add( *var1 ), gum::DuplicateLabel );

      TS_ASSERT_EQUALS( topology->toString(),
                        "BN{nodes: 5, arcs: 6, domainSize: 48, "
                        "parameters: 40, compression ratio: "
                        "16% }" );
      TS_ASSERT_EQUALS( topology->size(), (gum::Idx)5 );
      TS_ASSERT_EQUALS( topology->sizeArcs(), (gum::Idx)6 );
      TS_ASSERT_EQUALS( topology->dim(), (gum::Idx)24 );

      TS_GUM_ASSERT_THROWS_NOTHING( delete topology );
    }
    void testCopyConstructor() {
      gum::BayesNet<float>   source;
      gum::List<gum::NodeId> idList;
      TS_GUM_ASSERT_THROWS_NOTHING( fill( source, idList ) );

      gum::BayesNet<float>* copy = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING( copy = new gum::BayesNet<float>( source ) );

      TS_ASSERT_EQUALS( source.dag().size(), copy->dag().size() );
      TS_ASSERT_EQUALS( source.dag().sizeArcs(), copy->dag().sizeArcs() );
      // const gum::NodeSet& nodes=source.nodes();
      const gum::DAG dag = source.dag();

      for ( const auto node : dag.nodes() ) {
        TS_ASSERT( copy->dag().exists( node ) );

        const gum::DiscreteVariable& srcVar = source.variable( node );
        const gum::DiscreteVariable& cpVar = copy->variable( node );
        TS_ASSERT_EQUALS( srcVar.name(), cpVar.name() );

        if ( srcVar.domainSize() == cpVar.domainSize() ) {
          for ( gum::Idx i = 0; i < srcVar.domainSize(); i++ ) {
            TS_ASSERT_EQUALS( srcVar.label( i ), cpVar.label( i ) );
          }

        } else {
          TS_ASSERT( false );
        }

        for ( const auto parent : source.dag().parents( node ) ) {
          TS_ASSERT( copy->dag().existsArc( gum::Arc( parent, node ) ) );
        }

        const gum::Potential<float>& srcCPT = source.cpt( node );
        const gum::Potential<float>& cpCPT = copy->cpt( node );

        gum::Instantiation srcInst( srcCPT );
        gum::Instantiation cpInst( cpCPT );

        for ( cpInst.setFirst(); !cpInst.end(); cpInst.inc() ) {
          for ( gum::Idx i = 0; i < cpInst.nbrDim(); i++ ) {
            gum::NodeId id = copy->nodeId( cpInst.variable( i ) );
            srcInst.chgVal( source.variable( id ), cpInst.val( i ) );
          }

          TS_ASSERT_EQUALS( cpCPT[cpInst], srcCPT[srcInst] );
        }
      }

      if ( copy ) {
        delete copy;
      }
    }

    void testCopyOperator() {
      gum::BayesNet<float>   source;
      gum::BayesNet<float>   copy;
      gum::List<gum::NodeId> idList;
      TS_GUM_ASSERT_THROWS_NOTHING( fill( source, idList ) );

      TS_GUM_ASSERT_THROWS_NOTHING( copy = source );

      TS_ASSERT_EQUALS( source.dag().size(), copy.dag().size() );
      TS_ASSERT_EQUALS( source.dag().sizeArcs(), copy.dag().sizeArcs() );
      // const gum::NodeSet& nodes=source.nodes();
      const gum::DAG dag = source.dag();

      for ( const auto node : dag.nodes() ) {
        TS_ASSERT( copy.dag().exists( node ) );

        const gum::DiscreteVariable& srcVar = source.variable( node );
        const gum::DiscreteVariable& cpVar = copy.variable( node );
        TS_ASSERT_EQUALS( srcVar.name(), cpVar.name() );

        if ( srcVar.domainSize() == cpVar.domainSize() ) {
          for ( gum::Idx i = 0; i < srcVar.domainSize(); i++ ) {
            TS_ASSERT_EQUALS( srcVar.label( i ), cpVar.label( i ) );
          }

        } else {
          TS_ASSERT( false );
        }

        for ( const auto parent : source.dag().parents( node ) ) {
          TS_ASSERT( copy.dag().existsArc( gum::Arc( parent, node ) ) );
        }

        const gum::Potential<float>& srcCPT = source.cpt( node );
        const gum::Potential<float>& cpCPT = copy.cpt( node );
        gum::Instantiation           srcInst( srcCPT );
        gum::Instantiation           cpInst( cpCPT );

        for ( cpInst.setFirst(); !cpInst.end(); cpInst.inc() ) {
          for ( gum::Idx i = 0; i < cpInst.nbrDim(); i++ ) {
            gum::NodeId id = copy.nodeId( cpInst.variable( i ) );
            srcInst.chgVal( source.variable( id ), cpInst.val( i ) );
          }

          TS_ASSERT_EQUALS( cpCPT[cpInst], srcCPT[srcInst] );
        }
      }

      for ( const auto node : copy.nodes() ) {
        std::stringstream c_str;
        std::stringstream s_str;

        for ( const auto var : source.cpt( node ).variablesSequence() ) {
          s_str << *var << ",";
        }

        for ( const auto var : copy.cpt( node ).variablesSequence() ) {
          c_str << *var << ",";
        }

        TS_ASSERT_EQUALS( c_str.str(), s_str.str() );
      }
    }

    void testInsertion_1() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.add( *var1 ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.add( *var2 ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.add( *var3 ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.add( *var4 ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.add( *var5 ) ) );

      TS_ASSERT_EQUALS( bn.size(), (gum::Size)5 );
      TS_ASSERT_EQUALS( bn.dag().size(), (gum::Size)5 );

      // Test for uniqueness of the ids

      for ( gum::Idx i = 0; i < idList.size() - 1; i++ ) {
        for ( gum::Idx j = i + 1; j < idList.size(); j++ ) {
          if ( idList[i] != idList[j] ) {
            TS_ASSERT( true );

          } else {
            TS_ASSERT( false );
          }
        }
      }

      gum::LabelizedVariable* varPtr = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          varPtr = (gum::LabelizedVariable*)&bn.variable( idList[0] ) );
      TS_ASSERT_EQUALS( *varPtr, *var1 );

      TS_ASSERT_EQUALS( *( (gum::LabelizedVariable*)&bn.variable( idList[0] ) ),
                        *var1 );
      TS_ASSERT_EQUALS( *( (gum::LabelizedVariable*)&bn.variable( idList[1] ) ),
                        *var2 );
      TS_ASSERT_EQUALS( *( (gum::LabelizedVariable*)&bn.variable( idList[2] ) ),
                        *var3 );
      TS_ASSERT_EQUALS( *( (gum::LabelizedVariable*)&bn.variable( idList[3] ) ),
                        *var4 );
      TS_ASSERT_EQUALS( *( (gum::LabelizedVariable*)&bn.variable( idList[4] ) ),
                        *var5 );
    }

    void testInsertion_2() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.add( *var1 ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.add( *var2 ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.add( *var3 ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.add( *var4 ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( bn.add( *var5 ) ) );

      TS_ASSERT_EQUALS( bn.size(), (gum::Size)5 );
      TS_ASSERT_EQUALS( bn.dag().size(), (gum::Size)5 );

      gum::LabelizedVariable* varPtr = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
          varPtr = (gum::LabelizedVariable*)&bn.variable( idList[0] ) );
      TS_ASSERT_EQUALS( *varPtr, *var1 );

      TS_ASSERT_EQUALS( *( (gum::LabelizedVariable*)&bn.variable( idList[0] ) ),
                        *var1 );
      TS_ASSERT_EQUALS( *( (gum::LabelizedVariable*)&bn.variable( idList[1] ) ),
                        *var2 );
      TS_ASSERT_EQUALS( *( (gum::LabelizedVariable*)&bn.variable( idList[2] ) ),
                        *var3 );
      TS_ASSERT_EQUALS( *( (gum::LabelizedVariable*)&bn.variable( idList[3] ) ),
                        *var4 );
      TS_ASSERT_EQUALS( *( (gum::LabelizedVariable*)&bn.variable( idList[4] ) ),
                        *var5 );
    }

    void testIterations() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;
      TS_GUM_ASSERT_THROWS_NOTHING( fill( bn, idList ) );

      gum::Size cpt = (gum::Size)0;

      for ( const auto node : bn.nodes() ) {
        GUM_UNUSED( node );
        cpt++;
      }

      TS_ASSERT_EQUALS( cpt, bn.size() );

      cpt = (gum::Size)0;

      for ( const auto arc : bn.arcs() ) {
        cpt++;
      }

      TS_ASSERT_EQUALS( cpt, bn.sizeArcs() );
    }

    void testArcInsertion() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      idList.insert( bn.add( *var1 ) );
      idList.insert( bn.add( *var2 ) );
      idList.insert( bn.add( *var3 ) );
      idList.insert( bn.add( *var4 ) );
      idList.insert( bn.add( *var5 ) );

      TS_GUM_ASSERT_THROWS_NOTHING( bn.addArc( idList[0], idList[2] ) );
      TS_GUM_ASSERT_THROWS_NOTHING( bn.addArc( idList[2], idList[4] ) );
      TS_GUM_ASSERT_THROWS_NOTHING( bn.addArc( idList[1], idList[3] ) );
      TS_GUM_ASSERT_THROWS_NOTHING( bn.addArc( idList[0], idList[3] ) );
      TS_GUM_ASSERT_THROWS_NOTHING( bn.addArc( idList[3], idList[4] ) );
      TS_GUM_ASSERT_THROWS_NOTHING( bn.addArc( idList[1], idList[4] ) );

      TS_ASSERT_EQUALS( bn.dag().sizeArcs(), (gum::Size)6 );
    }

    void testEraseVar() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      TS_ASSERT( bn.empty() );
      TS_ASSERT( bn.dag().emptyArcs() );

      fill( bn, idList );

      TS_ASSERT( !bn.empty() );
      TS_ASSERT( !bn.dag().emptyArcs() );

      TS_ASSERT_EQUALS( bn.size(), (gum::Size)5 );
      TS_ASSERT_EQUALS( bn.dag().size(), (gum::Size)5 );
      TS_ASSERT_EQUALS( bn.dag().sizeArcs(), (gum::Size)6 );

      bn.erase( idList[0] );

      for ( const auto i : idList ) {
        bn.erase( i );
      }

      TS_ASSERT( bn.empty() );

      TS_ASSERT( bn.dag().emptyArcs() );

      TS_ASSERT_EQUALS( bn.size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bn.dag().size(), (gum::Size)0 );
      TS_ASSERT_EQUALS( bn.dag().sizeArcs(), (gum::Size)0 );

      idList.clear();
      TS_GUM_ASSERT_THROWS_NOTHING( fill( bn, idList ) );

      TS_ASSERT( !bn.empty() );
      TS_ASSERT( !bn.dag().emptyArcs() );

      TS_ASSERT_EQUALS( bn.size(), (gum::Size)5 );
      TS_ASSERT_EQUALS( bn.dag().size(), (gum::Size)5 );
      TS_ASSERT_EQUALS( bn.dag().sizeArcs(), (gum::Size)6 );
    }

    void testEraseArc() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      TS_ASSERT( bn.empty() );
      TS_ASSERT( bn.dag().emptyArcs() );

      fill( bn, idList );

      TS_ASSERT( !bn.empty() );
      TS_ASSERT( !bn.dag().emptyArcs() );

      TS_ASSERT_EQUALS( bn.size(), (gum::Size)5 );
      TS_ASSERT_EQUALS( bn.dag().size(), (gum::Size)5 );
      TS_ASSERT_EQUALS( bn.dag().sizeArcs(), (gum::Size)6 );

      TS_GUM_ASSERT_THROWS_NOTHING(
          bn.eraseArc( gum::Arc( idList[0], idList[2] ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING(
          bn.eraseArc( gum::Arc( idList[2], idList[4] ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING(
          bn.eraseArc( gum::Arc( idList[1], idList[3] ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING(
          bn.eraseArc( gum::Arc( idList[0], idList[3] ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING(
          bn.eraseArc( gum::Arc( idList[3], idList[4] ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING(
          bn.eraseArc( gum::Arc( idList[1], idList[4] ) ) );

      TS_ASSERT( !bn.empty() );
      TS_ASSERT( bn.dag().emptyArcs() );
    }

    void testStringAccessors() {
      gum::BayesNet<float> bn;
      for ( const auto& x : {"A", "B", "C"} ) {
        bn.add( gum::LabelizedVariable( x, x, 2 ) );
      }
      for ( const auto& a :
            {std::make_pair( "A", "C" ), std::make_pair( "B", "C" )} ) {
        bn.addArc( a.first, a.second );
      }
      TS_ASSERT_EQUALS( bn.toString(),
                        "BN{nodes: 3, arcs: 2, domainSize: 8, "
                        "parameters: 12, compression ratio: "
                        "-50% }" );

      bn.cpt( "A" ).fillWith( 1.0f ).normalize();
      bn.generateCPT( "B" );
      bn.generateCPT( "C" );
      TS_ASSERT_THROWS( bn.cpt( "XXX" ), gum::NotFound );

      bn.reverseArc( "A", "C" );
      TS_ASSERT_EQUALS( bn.toString(),
                        "BN{nodes: 3, arcs: 3, domainSize: 8, "
                        "parameters: 14, compression ratio: "
                        "-75% }" );

      TS_ASSERT_THROWS( bn.reverseArc( "A", "C" ), gum::InvalidArc );
      TS_ASSERT_THROWS( bn.reverseArc( "A", "C" ), gum::GraphError );
      TS_ASSERT_THROWS( bn.reverseArc( "A", "X" ), gum::NotFound );

      bn.erase( "A" );
      TS_ASSERT_EQUALS( bn.toString(),
                        "BN{nodes: 2, arcs: 1, domainSize: 4, "
                        "parameters: 6, compression ratio: -50% "
                        "}" );

      TS_ASSERT_THROWS( bn.erase( "A" ), gum::NotFound );

      bn.eraseArc( "B", "C" );
      TS_ASSERT_EQUALS( bn.toString(),
                        "BN{nodes: 2, arcs: 0, domainSize: 4, "
                        "parameters: 4, compression ratio: 0% "
                        "}" );

      TS_ASSERT_THROWS( bn.eraseArc( "B", "C" ), gum::NotFound );
    }


    void testIterator() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      for ( const auto node : bn.nodes() ) {
        TS_ASSERT( idList.exists( node ) );
      }
    }

    void testRandomlyFilled() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      fill( bn, idList );

      for ( const auto node : bn.nodes() ) {
        std::stringstream s1, s2;
        s1 << bn.cpt( node );

        bn.generateCPTs();
        s2 << bn.cpt( node );
        TS_ASSERT_DIFFERS( s1.str(), s2.str() );
      }
    }

    void testMoralGraph() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      fill( bn, idList );

      gum::UndiGraph graph;
      TS_GUM_ASSERT_THROWS_NOTHING( graph = bn.moralGraph() );
      TS_ASSERT_EQUALS( graph, getRealMoralGraph( bn, idList ) );
    }

    void testTopologicalOrder() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      fill( bn, idList );

      gum::Sequence<gum::NodeId> topoOrder;
      TS_GUM_ASSERT_THROWS_NOTHING( topoOrder = bn.topologicalOrder() );

      TS_ASSERT_EQUALS( topoOrder.size(), (gum::Size)5 );
    }

    void testcpt() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      fill( bn, idList );
      TS_ASSERT_EQUALS( bn.cpt( idList[0] ).domainSize(), (gum::Size)2 );
      TS_ASSERT_EQUALS( bn.cpt( idList[1] ).domainSize(), (gum::Size)2 );
      TS_ASSERT_EQUALS( bn.cpt( idList[2] ).domainSize(), (gum::Size)4 );
      TS_ASSERT_EQUALS( bn.cpt( idList[3] ).domainSize(), (gum::Size)8 );
      TS_ASSERT_EQUALS( bn.cpt( idList[4] ).domainSize(), (gum::Size)24 );
    }

    void testCPTCoherencyVarRemoval() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      fill( bn, idList );

      TS_ASSERT_EQUALS( bn.dag().parents( idList[3] ).size(), (gum::Size)2 );
      TS_ASSERT( bn.dag().existsArc( idList[0], idList[3] ) );
      TS_ASSERT( bn.dag().existsArc( idList[1], idList[3] ) );

      gum::Size dmnSize_1 = bn.variable( idList[0] ).domainSize() *
                            bn.variable( idList[1] ).domainSize();
      dmnSize_1 *= bn.variable( idList[3] ).domainSize();
      TS_ASSERT_EQUALS( bn.cpt( idList[3] ).domainSize(), dmnSize_1 );

      TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[0] ) ) );
      TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[1] ) ) );
      TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[3] ) ) );

      TS_GUM_ASSERT_THROWS_NOTHING( bn.erase( idList[1] ) );

      TS_ASSERT_EQUALS( bn.dag().parents( idList[3] ).size(), (gum::Size)1 );
      TS_ASSERT( bn.dag().existsArc( idList[0], idList[3] ) );
      TS_ASSERT( !bn.dag().existsArc( idList[1], idList[3] ) );

      gum::Size dmnSize_2 = bn.variable( idList[0] ).domainSize();
      dmnSize_2 *= bn.variable( idList[3] ).domainSize();
      TS_ASSERT_EQUALS( bn.cpt( idList[3] ).domainSize(), dmnSize_2 );

      TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[0] ) ) );
      TS_ASSERT_THROWS_ANYTHING(
          bn.cpt( idList[3] ).contains( bn.variable( idList[1] ) ) );
      TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[3] ) ) );
    }

    void testCPTCoherencyArcRemoval() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      fill( bn, idList );

      TS_ASSERT_EQUALS( bn.dag().parents( idList[3] ).size(), (gum::Size)2 );
      TS_ASSERT( bn.dag().existsArc( idList[0], idList[3] ) );
      TS_ASSERT( bn.dag().existsArc( idList[1], idList[3] ) );

      gum::Size dmnSize_1 = bn.variable( idList[0] ).domainSize() *
                            bn.variable( idList[1] ).domainSize();
      dmnSize_1 *= bn.variable( idList[3] ).domainSize();
      TS_ASSERT_EQUALS( bn.cpt( idList[3] ).domainSize(), dmnSize_1 );

      TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[0] ) ) );
      TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[1] ) ) );
      TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[3] ) ) );

      TS_GUM_ASSERT_THROWS_NOTHING(
          bn.eraseArc( gum::Arc( idList[1], idList[3] ) ) );

      TS_ASSERT_EQUALS( bn.dag().parents( idList[3] ).size(), (gum::Size)1 );
      TS_ASSERT( bn.dag().existsArc( idList[0], idList[3] ) );
      TS_ASSERT( !bn.dag().existsArc( idList[1], idList[3] ) );

      gum::Size dmnSize_2 = bn.variable( idList[0] ).domainSize();
      dmnSize_2 *= bn.variable( idList[3] ).domainSize();
      TS_ASSERT_EQUALS( bn.cpt( idList[3] ).domainSize(), dmnSize_2 );

      TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[0] ) ) );
      TS_ASSERT( !bn.cpt( idList[3] ).contains( bn.variable( idList[1] ) ) );
      TS_ASSERT( bn.cpt( idList[3] ).contains( bn.variable( idList[3] ) ) );
    }

    void testStreamOperator() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      fill( bn, idList );
      std::stringstream sBuff;
      TS_GUM_ASSERT_THROWS_NOTHING( sBuff << bn );
    }

    void testAccessorByName() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      fill( bn, idList );

      for ( const auto node : bn.nodes() ) {
        TS_ASSERT_EQUALS( bn.idFromName( bn.variable( node ).name() ), node );
        TS_ASSERT_EQUALS( &bn.variableFromName( bn.variable( node ).name() ),
                          &bn.variable( node ) );
      }

      TS_ASSERT_THROWS( bn.idFromName( "choucroute" ), gum::NotFound );

      TS_ASSERT_THROWS( bn.variableFromName( "choucroute" ), gum::NotFound );

      TS_GUM_ASSERT_THROWS_NOTHING( bn.idFromName( "var1" ) );
      bn.erase( bn.idFromName( "var1" ) );
      TS_ASSERT_THROWS( bn.idFromName( "var1" ), gum::NotFound );
    }

    void testCopyAndEqualityOperators() {
      gum::BayesNet<float>*               bn_1 = new gum::BayesNet<float>();
      gum::SimpleBayesNetGenerator<float> generator( 20, 30, 4 );
      generator.generateBN( *bn_1 );

      gum::BayesNet<float>* bn_2 = new gum::BayesNet<float>();

      generator.generateBN( *bn_2 );

      gum::BayesNet<float> bn_cpy_1 = *bn_1;
      gum::BayesNet<float> bn_cpy_2 = *bn_2;

      TS_ASSERT( bn_cpy_1 == bn_cpy_1 );
      TS_ASSERT( bn_cpy_1 == ( *bn_1 ) );
      TS_ASSERT( bn_cpy_2 == bn_cpy_2 );
      TS_ASSERT( bn_cpy_2 == ( *bn_2 ) );
      TS_ASSERT( ( *bn_1 ) != ( *bn_2 ) );
      TS_ASSERT( bn_cpy_1 != bn_cpy_2 );

      TS_ASSERT_EQUALS( bn_cpy_1, bn_cpy_1 );
      TS_ASSERT_EQUALS( bn_cpy_1, ( *bn_1 ) );
      TS_ASSERT_EQUALS( bn_cpy_2, bn_cpy_2 );
      TS_ASSERT_EQUALS( bn_cpy_2, ( *bn_2 ) );
      TS_ASSERT_DIFFERS( ( *bn_1 ), ( *bn_2 ) );
      TS_ASSERT_DIFFERS( bn_cpy_1, bn_cpy_2 );

      gum::BayesNet<float> cpy = *bn_1;
      TS_ASSERT( cpy == ( *bn_1 ) );
      TS_ASSERT( cpy != ( *bn_2 ) );

      TS_GUM_ASSERT_THROWS_NOTHING( cpy = *bn_2 );

      TS_ASSERT_EQUALS( cpy, ( *bn_2 ) );

      TS_ASSERT( cpy == ( *bn_2 ) );
      TS_ASSERT_DIFFERS( cpy, ( *bn_1 ) );
      TS_ASSERT( cpy != ( *bn_1 ) );

      TS_ASSERT_THROWS_NOTHING( cpy = cpy; );

      TS_ASSERT_EQUALS( cpy, ( *bn_2 ) );

      TS_ASSERT( cpy == ( *bn_2 ) );
      TS_ASSERT_DIFFERS( cpy, ( *bn_1 ) );
      TS_ASSERT( cpy != ( *bn_1 ) );

      std::string s1 = cpy.toString();
      delete bn_2;
      delete bn_1;
      std::string s2 = cpy.toString();
      TS_ASSERT_EQUALS( s1, s2 );
    }

    void testAggregatorNodes() {
      {
        gum::BayesNet<float> bn;
        gum::Idx             i1 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING( i1 = bn.addOR( *var1 ) );
        TS_ASSERT_THROWS( bn.addOR( *var5 ), gum::SizeError );

        bn.addArc( bn.add( *var3 ), i1 );
        bn.addArc( bn.add( *var4 ), i1 );
        bn.addArc( bn.add( *var5 ), i1 );
      }
      {
        gum::BayesNet<float> bn;
        gum::Idx             i2 = 0;

        TS_GUM_ASSERT_THROWS_NOTHING( i2 = bn.addAND( *var2 ) );
        TS_ASSERT_THROWS( bn.addAND( *var5 ), gum::SizeError );

        bn.addArc( bn.add( *var3 ), i2 );
        bn.addArc( bn.add( *var4 ), i2 );
        bn.addArc( bn.add( *var5 ), i2 );
      }
    }

    void testJointProbability() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;

      fill( bn, idList );

      gum::Instantiation i;
      bn.completeInstantiation( i );
      TS_ASSERT_EQUALS( i.nbrDim(), bn.size() );

      TS_ASSERT_EQUALS( i.domainSize(), ( gum::Size )( 2 * 2 * 2 * 2 * 3 ) );

      float a = 0.0;
      TS_ASSERT_THROWS_NOTHING( a = bn.jointProbability( i ) );

      TS_ASSERT_DELTA( a, 0.00072, 1e-4 );

      gum::Instantiation j;
      float              b = 0.0;
      TS_ASSERT_THROWS_NOTHING( b = bn.jointProbability( j ) );

      TS_ASSERT_EQUALS( a, b );
    }

    void testArcReversal() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;
      fill( bn, idList );

      std::vector<float> joint;
      joint.reserve( 2 * bn.dim() );
      gum::Instantiation i;
      bn.completeInstantiation( i );

      for ( i.setFirst(); !i.end(); i.inc() ) {
        joint.push_back( bn.jointProbability( i ) );
      }

      bn.reverseArc( 0, 2 );
      bn.reverseArc( gum::Arc( 3, 4 ) );
      TS_ASSERT_THROWS( bn.reverseArc( gum::Arc( 3, 4 ) ), gum::InvalidArc );
      TS_ASSERT_THROWS( bn.reverseArc( gum::Arc( 3, 5 ) ), gum::InvalidArc );
      TS_ASSERT_THROWS( bn.reverseArc( gum::Arc( 2, 4 ) ), gum::InvalidArc );

      float        epsilon = 1e-5f;
      unsigned int j;

      for ( j = 0, i.setFirst(); !i.end(); i.inc(), ++j ) {
        TS_ASSERT( fabs( bn.jointProbability( i ) - joint[j] ) <= epsilon );
      }
    }

    void testChangeLabelOfVariable() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;
      fill( bn, idList );

      TS_ASSERT_EQUALS( bn.variable( 0 ).toString(), "var1<0,1>" );
      TS_ASSERT_THROWS_NOTHING(
          dynamic_cast<const gum::LabelizedVariable&>( bn.variable( 0 ) )
              .changeLabel( 0, "x" ) );
      TS_ASSERT_EQUALS( bn.variable( 0 ).toString(), "var1<x,1>" );
    }

    void testShortCutAddLabelized() {
      gum::BayesNet<float> bn;

      gum::NodeId i1, i2, i3;

      TS_ASSERT_THROWS_NOTHING( i1 = bn.add( "A", 2 ) );
      TS_ASSERT_THROWS_NOTHING( i2 = bn.add( "B", 3 ) );
      TS_ASSERT_EQUALS( i1, gum::NodeId( 0 ) );
      TS_ASSERT_EQUALS( i2, gum::NodeId( 1 ) );

      TS_ASSERT_THROWS( i3 = bn.add( "A", 5 ), gum::DuplicateLabel );
      TS_ASSERT_THROWS( i3 = bn.add( "C", 1 ), gum::OperationNotAllowed );
      GUM_UNUSED( i3 );

      TS_ASSERT_THROWS_NOTHING( bn.addArc( i1, i2 ) );
      TS_ASSERT_EQUALS( bn.log10DomainSize(), std::log10( 2.0 * 3.0 ) );
    }

    void testSomeFunctions() {
      gum::BayesNet<float>   bn;
      gum::List<gum::NodeId> idList;
      TS_GUM_ASSERT_THROWS_NOTHING( fill( bn, idList ) );

      TS_ASSERT_EQUALS( bn.maxVarDomainSize(), (gum::Size)3 );
      TS_ASSERT_EQUALS( bn.minParam(), 0.0f );
      TS_ASSERT_EQUALS( bn.maxParam(), 1.0f );
      TS_ASSERT_EQUALS( bn.minNonZeroParam(), 0.1f );
      TS_ASSERT_EQUALS( bn.maxNonOneParam(), 0.9f );
    }

    void testMinimalCondSet() {
      /*
       A   B        0   1
        \ / \        \ / \
      H  C   D     7  2   3
       \ |   |      \ |   |
         E   |        4   |
          \ /          \ /
           F            5
           |            |
           G            6
       */
      gum::BayesNet<int> bn;

      auto a = bn.add( "A", 2 );
      auto b = bn.add( "B", 2 );
      auto c = bn.add( "C", 2 );
      auto d = bn.add( "D", 2 );
      auto e = bn.add( "E", 2 );
      auto f = bn.add( "F", 2 );
      auto g = bn.add( "G", 2 );
      auto h = bn.add( "H", 2 );

      bn.addArc( a, c );
      bn.addArc( b, c );
      bn.addArc( b, d );
      bn.addArc( c, e );
      bn.addArc( d, f );
      bn.addArc( e, f );
      bn.addArc( f, g );
      bn.addArc( h, e );

      TS_ASSERT_EQUALS( bn.minimalCondSet( c, {a, b, c, d, e, f, g, h} ),
                        gum::NodeSet( {c} ) );
      TS_ASSERT_EQUALS( bn.minimalCondSet( c, {a, b, d, e, f, g, h} ),
                        gum::NodeSet( {a, b, e, h} ) );
      TS_ASSERT_EQUALS( bn.minimalCondSet( c, {e, f, g} ),
                        gum::NodeSet( {e, f} ) );
      TS_ASSERT_EQUALS( bn.minimalCondSet( c, {b, e, f, g} ),
                        gum::NodeSet( {e, b} ) );
      TS_ASSERT_EQUALS( bn.minimalCondSet( c, {a, e, f, g} ),
                        gum::NodeSet( {a, e, f} ) );

      // for set of targets
      TS_ASSERT_EQUALS( bn.minimalCondSet( {e, d}, {a, b, c, d, e, f, g, h} ),
                        gum::NodeSet( {d, e} ) );
      TS_ASSERT_EQUALS( bn.minimalCondSet( {e, d}, {a, b, c, d, f, g, h} ),
                        gum::NodeSet( {c, d, h, f} ) );
      TS_ASSERT_EQUALS( bn.minimalCondSet( {e, d}, {a, b, c, f, g, h} ),
                        gum::NodeSet( {b, c, f, h} ) );
    }

    void testMinimalCondSet2() {
      /*
      F  A
      \ / \
       B   |
       |   E
       C   |
      / \ /
      H  D
      */
      gum::BayesNet<int> bn;
      //                         0    1    2    3    4    5    6
      for ( const auto& item : {"A", "B", "C", "D", "E", "F", "H"} )
        bn.add( item, 2 );
      bn.addArc( "A", "B" );
      bn.addArc( "A", "E" );
      bn.addArc( "B", "C" );
      bn.addArc( "C", "D" );
      bn.addArc( "C", "H" );
      bn.addArc( "E", "D" );
      bn.addArc( "F", "B" );

      TS_ASSERT_EQUALS( bn.minimalCondSet( 4, gum::NodeSet( {0, 1, 2, 3, 5, 6} ) ),
                        gum::NodeSet( {0, 2, 3} ) );

      TS_ASSERT_EQUALS( bn.minimalCondSet( 4, gum::NodeSet( {0, 1, 5} ) ),
                        gum::NodeSet( {0} ) );

      TS_ASSERT_EQUALS( bn.minimalCondSet( 4, gum::NodeSet( {0, 1, 3, 5} ) ),
                        gum::NodeSet( {0, 1, 3} ) );
    }

    void testFastPrototype() {
      auto bn = gum::BayesNet<int>::fastPrototype( "a->b->c;a->c" );
      TS_ASSERT_EQUALS( bn.size(), gum::Size( 3 ) );
      TS_ASSERT_EQUALS( bn.sizeArcs(), gum::Size( 3 ) );
      TS_ASSERT_EQUALS(
          bn.dim(),
          gum::Size( ( 2 - 1 ) + ( 2 * ( 2 - 1 ) ) + ( 2 * 2 * ( 2 - 1 ) ) ) );

      bn = gum::BayesNet<int>::fastPrototype( "a->b->c;a->c", 3 );
      TS_ASSERT_EQUALS( bn.size(), gum::Size( 3 ) );
      TS_ASSERT_EQUALS( bn.sizeArcs(), gum::Size( 3 ) );
      TS_ASSERT_EQUALS(
          bn.dim(),
          gum::Size( ( 3 - 1 ) + ( 3 * ( 3 - 1 ) ) + ( 3 * 3 * ( 3 - 1 ) ) ) );

      bn = gum::BayesNet<int>::fastPrototype( "a->b[5]->c;a->c" );
      TS_ASSERT_EQUALS( bn.size(), gum::Size( 3 ) );
      TS_ASSERT_EQUALS( bn.sizeArcs(), gum::Size( 3 ) );
      TS_ASSERT_EQUALS(
          bn.dim(),
          gum::Size( ( 2 - 1 ) + ( 2 * ( 5 - 1 ) ) + ( 5 * 2 * ( 2 - 1 ) ) ) );

      bn = gum::BayesNet<int>::fastPrototype( "a->b->c;a[1000]->c" );
      TS_ASSERT_EQUALS( bn.size(), gum::Size( 3 ) );
      TS_ASSERT_EQUALS( bn.sizeArcs(), gum::Size( 3 ) );
      TS_ASSERT_EQUALS(
          bn.dim(),
          gum::Size( ( 2 - 1 ) + ( 2 * ( 2 - 1 ) ) + ( 2 * 2 * ( 2 - 1 ) ) ) );

      TS_ASSERT_THROWS( bn = gum::BayesNet<int>::fastPrototype( "a->b->c->a" ),
                        gum::InvalidDirectedCycle );
    }
  };

}  // tests
