#include <cxxtest/testsuite_utils.h>

#include <vector>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/variableElimination.h>


// The graph used for the tests:
/*
    A             0  
     \             \ 
      B   G         1   6
       \ / \         \ / \ 
        C   H         2   7
       / \           / \
      D   E         3   4
     /             /
    F             5
*/

namespace gum_tests {

  
  class LazyPropagationIncrementalTestSuite : public CxxTest::TestSuite {
  public:

    // the function used to combine two tables
    static gum::Potential<double>*
    LPIncrmultiPotential( const gum::Potential<double>& t1,
                          const gum::Potential<double>& t2 ) {
      return new gum::Potential<double>( t1 * t2 );
    }

    // the function used to combine two tables
    static gum::Potential<double>*
    LPIncrprojPotential( const gum::Potential<double>& t1,
                         const gum::Set<const gum::DiscreteVariable*>& del_vars ) {
      return new gum::Potential<double>( t1.margSumOut( del_vars ) );
    }


    

    gum::BayesNet<double> bn;
    std::vector<gum::NodeId> BN_node_index;
    std::vector<const gum::DiscreteVariable*> BN_variable;
    gum::Set<const gum::DiscreteVariable*> BN_variable_set;

    double __epsilon{1e-6};

    gum::Potential<double>* joint;

    gum::MultiDimCombinationDefault<double,gum::Potential>
    combination { LPIncrmultiPotential };

    gum::MultiDimProjection<double,gum::Potential>
    proj { LPIncrprojPotential };

    
    void setUp() {
      // create the BN
      std::vector<gum::LabelizedVariable> variables { 
        gum::LabelizedVariable ( "A", "", 2 ),
        gum::LabelizedVariable ( "B", "", 3 ),
        gum::LabelizedVariable ( "C", "", 2 ),
        gum::LabelizedVariable ( "D", "", 3 ),
        gum::LabelizedVariable ( "E", "", 2 ),
        gum::LabelizedVariable ( "F", "", 3 ),
        gum::LabelizedVariable ( "G", "", 4 ),
        gum::LabelizedVariable ( "H", "", 3 ) };

      for ( unsigned int i = 0; i < variables.size (); ++i ) {
        BN_node_index.push_back ( bn.add( variables[i] ) );
        BN_variable.push_back ( &( bn.variable ( BN_node_index[i] ) ) );
        BN_variable_set.insert ( BN_variable[i] );
      }

      bn.addArc( BN_node_index[0], BN_node_index[1] );
      bn.addArc( BN_node_index[1], BN_node_index[2] );
      bn.addArc( BN_node_index[2], BN_node_index[3] );
      bn.addArc( BN_node_index[2], BN_node_index[4] );
      bn.addArc( BN_node_index[3], BN_node_index[5] );
      bn.addArc( BN_node_index[6], BN_node_index[2] );
      bn.addArc( BN_node_index[6], BN_node_index[7] );

      bn.cpt( BN_node_index[0] ).fillWith( {0.2, 0.8} );
      bn.cpt( BN_node_index[1] ).fillWith( {0.3, 0.4, 0.3,
                                            0.1, 0.3, 0.6} );
      bn.cpt( BN_node_index[2] ).fillWith( {0.1, 0.9,  // 1
                                            0.2, 0.8,  // 2
                                            0.3, 0.7,  // 3
                                            0.4, 0.6,  // 4
                                            0.5, 0.5,  // 5
                                            0.6, 0.4,  // 6
                                            0.7, 0.3,  // 7
                                            0.8, 0.2,  // 8
                                            0.9, 0.1,  // 9
                                            0.8, 0.2,  // 10
                                            0.7, 0.3,  // 11
                                            0.6, 0.4} );  // 12
      bn.cpt( BN_node_index[3] ).fillWith( {0.3, 0.2, 0.5,
                                            0.7, 0.1, 0.2 } ); 
      bn.cpt( BN_node_index[4] ).fillWith( {0.4, 0.6, 0.9, 0.1} );
      bn.cpt( BN_node_index[5] ).fillWith( {0.2, 0.4, 0.4,
                                            0.4, 0.1, 0.5,
                                            0.7, 0.2, 0.1} );
      bn.cpt( BN_node_index[6] ).fillWith( {0.1, 0.4, 0.2, 0.3} );
      bn.cpt( BN_node_index[7] ).fillWith( {0.4, 0.3, 0.3,
                                            0.1, 0.8, 0.1,
                                            0.3, 0.4, 0.3,
                                            0.7, 0.1, 0.2} );

      // create the joint probability
      gum::Set<const gum::Potential<double>*> potset;
      for ( unsigned int i = 0; i < BN_node_index.size (); ++i ) {
        potset.insert ( &( bn.cpt ( i ) ) );
      }

      joint = combination.combine ( potset );
    }

    void tearDown() {
      delete joint;
    }


    // ============================================================================
    gum::Potential<double>*
    create_evidence ( const gum::NodeId node_id,
                      const std::vector<double>& ev ) {
      gum::Potential<double>* proba = new gum::Potential<double>;
      proba->add ( *( BN_variable[node_id] ) );
      proba->fillWith ( ev );

      return proba;
    }
    

    // ============================================================================
    gum::Potential<double>*
    posterior_joint ( const gum::Potential<double>* joint,
                      gum::Set<const gum::Potential<double>*> evidence ) {
      evidence.insert ( joint );
      gum::Potential<double>* joint_pot = combination.combine ( evidence );

      return joint_pot;
    }


    // ============================================================================
    gum::Potential<double>* get_marginal ( const gum::Potential<double>* joint,
                                           const gum::NodeId target_id ) {
      // get the set of variables to erase
      auto myset = BN_variable_set;
      myset.erase ( BN_variable[target_id] );
      return LPIncrprojPotential ( *joint, myset );
    }


    // ============================================================================
    gum::Potential<double>* get_joint ( const gum::Potential<double>* joint,
                                        const gum::NodeSet& target_ids ) {
      // get the set of variables to erase
      auto myset = BN_variable_set;
      for ( auto target_id : target_ids ) 
        myset.erase ( BN_variable[target_id] );
      return proj.project ( *joint, myset );
    }


    // ============================================================================
    bool equalPotentials( const gum::Potential<double>& p1,
                          const gum::Potential<double>& p2 ) {
      gum::Instantiation i1( p1 );
      gum::Instantiation i2( p2 );

      for ( i1.setFirst(), i2.setFirst(); !i1.end(); i1.inc(), i2.inc() ) {
        if ( ( p1[i1] == 0 ) && ( std::fabs( p2[i2] ) > __epsilon ) )
          return false;
        if ( p1[i1] > p2[i2] ) {
          if ( std::fabs( ( p1[i1] - p2[i2] ) / p1[i1] ) > __epsilon )
            return false;
        } else {
          if ( std::fabs( ( p1[i1] - p2[i2] ) / p1[i2] ) > __epsilon )
            return false;
        }
      }

      return true;
    }
    

    // ============================================================================
    // ============================================================================
    void test_prior () {
      gum::LazyPropagation<double> inf ( &bn );
      gum::ShaferShenoyInference<double> inf2( bn );
      TS_ASSERT_THROWS_NOTHING( inf.makeInference() );
      inf2.makeInference ();
      
      // get the marginals of A, C, D, H
      gum::Potential<double>* pa = get_marginal ( joint, BN_node_index[0] );
      gum::Potential<double>* pc = get_marginal ( joint, BN_node_index[2] );
      gum::Potential<double>* pd = get_marginal ( joint, BN_node_index[3] );
      gum::Potential<double>* ph = get_marginal ( joint, BN_node_index[7] );

      std::cout << bn.cpt ( 2 ) << std::endl;

      std::cout << *pa << std::endl
                << inf.posterior ( BN_node_index[0] ) << std::endl
                << inf2.posterior ( BN_node_index[0] ) << std::endl;
      std::cout << *pc << std::endl
                << inf.posterior ( BN_node_index[2] ) << std::endl
                << inf2.posterior ( BN_node_index[2] ) << std::endl;
      std::cout << *pd << std::endl << inf.posterior ( BN_node_index[3] )
                << std::endl;
      std::cout << *ph << std::endl << inf.posterior ( BN_node_index[7] )
                << std::endl;
      
      TS_ASSERT( equalPotentials( inf.posterior( BN_node_index[0] ), *pa ) );
      TS_ASSERT( equalPotentials( inf.posterior( BN_node_index[2] ), *pc ) );
      TS_ASSERT( equalPotentials( inf.posterior( BN_node_index[3] ), *pd ) );
      TS_ASSERT( equalPotentials( inf.posterior( BN_node_index[7] ), *ph ) );

    }
    
  };
  
}

