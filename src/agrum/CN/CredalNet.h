#ifndef __CREDAL_NET__H__
#define __CREDAL_NET__H__

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cstdlib>

//we force MP (not long or GMP)
#define MP
#undef long
#undef GMP
#include "lrslib/lrslib.h"
#include <agrum/core/exceptions.h>

// replace #include <omp.h>
#include <agrum/CN/OMPThreads.h>

namespace gum {

  template< typename GUM_SCALAR >
  class CredalNet {
    public:
      enum nodeType {
        PRECISE = 0,
        CREDAL = 1,
        VACUOUS = 2,
        DNODE = 3
        /*,
        BARREN_PRECISE = 3,
        BARREN_CREDAL = 4,
        BARREN_VACUOUS = 5,

        SOURCE_PRECISE = 6,
        SOURCE_CREDAL = 7,
        SOURCE_VACUOUS = 8,

        EVIDENCE = 9
        */
      };

    private:
      // member attributes (initialisation in CredalNet< GUM_SCALAR >::__initCNNets
      // // min, max and average perturbation of the network ( if asked )
      double __epsilonMin, __epsilonMax, __epsilonMoy;

      double __epsRedund;//= 1e-4;

      // farley algorithm
      double __epsF;// = 1e-6;
      double __denMax;// = 1e6; // beware LRS

      // continued fractions, beware LRS
      // decimal paces (__epsC * __precisionC == 1)
      double __precisionC;// = 1e6;
      double __deltaC;// = 5;

      // old custom algorithm
      int __precision;// = 1e6; // beware LRS

      // original bn (no transformation done on it)
      gum::BayesNet< GUM_SCALAR > __src_bn;

      // bns to store intervals
      gum::BayesNet< GUM_SCALAR > __src_bn_min;
      gum::BayesNet< GUM_SCALAR > __src_bn_max;

      // up to date bn (dag) to perform L2U inference
      gum::BayesNet< GUM_SCALAR > * __current_bn;// = NULL;

      typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes __credalNet_src_cpt;

      typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *__credalNet_current_cpt;// =  NULL;

      typename gum::Property< std::vector< gum::NodeId > >::onNodes __var_bits;

      // see enum in public
      typename gum::Property< nodeType >::onNodes __nodeType;
      typename gum::Property< nodeType >::onNodes *__current_nodeType;// = NULL;

      typename std::vector< std::vector< GUM_SCALAR > > binCptMin;
      typename std::vector< std::vector< GUM_SCALAR > > binCptMax;


      void __intervalToCredal();
      
      void __initParams();
      void __initCNNets ( const std::string &src_min_num, const std::string &src_max_den );
      void __initCNNets ( const gum::BayesNet< GUM_SCALAR > & src_min_num, const gum::BayesNet< GUM_SCALAR > & src_max_den );

      // member methods
      void __bnCopy ( gum::BayesNet< GUM_SCALAR > & bn_dest );

      void __intPow ( const int &base, int &exponent ) const;
      void __int2Pow ( int &exponent ) const;
      void __superiorPow ( const int &card, int &num_bits, int &new_card ) const;

      // lrs call
      void __H2V ( const std::vector< std::vector< GUM_SCALAR > > & h_rep, std::vector< std::vector< GUM_SCALAR > > & v_rep, const bool rationals ) const;

      // custom algorithm
      void __frac ( int64_t &numerator, int64_t &denominator, const GUM_SCALAR &number ) const;

      // farley algorithm
      void __farley ( int64_t &numerator, int64_t &denominator, const GUM_SCALAR &number, const int &den_max ) const;

      // another algorithm (continued fractions)
      void __fracC ( int64_t &numerator, int64_t &denominator, const int &alpha_num, const int &d_num, const int &denum ) const;

      bool __less ( const double &a, const double &b, const int &c, const int &d ) const;
      bool __leq ( const double &a, const double &b, const int &c, const int &d ) const;

      int __matches ( const double &a, const double &b, const int &alpha_num, const int &d_num, const int &denum ) const;
      void __find_exact_left ( const double &p_a, const double &q_a, const double &p_b, const double &q_b, int64_t &num, int64_t &den, const int &alpha_num, const int &d_num, const int &denum ) const;
      void __find_exact_right ( const double &p_a, const double &q_a, const double &p_b, const double &q_b, int64_t &num, int64_t &den, const int &alpha_num, const int &d_num, const int &denum ) const;

      // enum
      void __sort_varType();

      int __find_dNode_card ( const std::vector< std::vector< std::vector< GUM_SCALAR > > > & var_cpt ) const;

    protected:

    public:
      // constructors
      CredalNet ( const std::string &src_min_num, const std::string &src_max_den = "" );
      CredalNet ( const gum::BayesNet< GUM_SCALAR > & src_min_num, const gum::BayesNet< GUM_SCALAR > & src_max_den = gum::BayesNet< GUM_SCALAR >() );

      // destructor
      ~CredalNet();

      // getter methods
      const gum::BayesNet< GUM_SCALAR > & src_bn() const;
      const gum::BayesNet< GUM_SCALAR > & current_bn() const;
      const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes &credalNet_cpt() const;
      //PH const std::vector< std::vector< gum::NodeId > > & var_bits() const;
      //PH const std::vector< std::vector< GUM_SCALAR > > & credalNet_cpt_min() const;
      //PH const std::vector< std::vector< GUM_SCALAR > > & credalNet_cpt_max() const;
      nodeType getNodeType ( const gum::NodeId &id ) const;

      // transformation member methods
      void bnToCredal ( GUM_SCALAR beta, bool rational );
      
      void intervalToCredal ( const int s = 0 );
      void dts(); // for L2U, no arcs between decision node bits
      // not complete / to be removed
      void bin();
      //PH void indic();
      //PH void perState();

      // other utility member methods
      //PH void saveCNet ( const std::string &cn_path ) const;
      //PH void loadCNet ( const std::string &src_cn_path );

      void saveBNsMinMax( const std::string & min_path, const std::string & max_path ) const;

      //PH void vacants ( int &result ) const;
      //PH void notVacants ( int &result ) const;
      //PH void averageVertices ( double &result ) const;

      std::string toString() const;
      //PH void toPNG ( const std::string &png_path ) const;

      // test functions
      // to be removed ?
      void testFrac() const;
      void testPow() const;

      void intPow ( const int &base, int &exponent ) const;
      void int2Pow ( int &exponent ) const;
      void superiorPow ( const int &card, int &num_bits, int &new_card ) const;

      void computeCPTMinMax();
      const std::vector< std::vector< GUM_SCALAR > > & get_CPT_min() const;
      const std::vector< std::vector< GUM_SCALAR > > & get_CPT_max() const;

      const double & getEpsilonMin() const;
      const double & getEpsilonMax() const;
      const double & getEpsilonMoy() const;

  }; // CredalNet

} // namespace cn

#include <agrum/CN/CredalNet.tcc>

#endif // __CREDAL_NET__H__
