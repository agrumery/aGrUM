#ifndef __CREDAL_NET__H__
#define __CREDAL_NET__H__

/**
 * @file
 * @brief Class representing Credal Networks
 */

#include <vector>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <set>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstdlib>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/core/exceptions.h>

#include <agrum/core/Rational.h> // custom decimal to rational
#include <agrum/core/pow.h> // custom pow functions with integers, faster implementation

#include <agrum/CN/LrsWrapper.h>

// lrs stuff
//we force MP (not long or GMP)
#undef LONG
#undef GMP
#define MP
#include <agrum/CN/lrslib/lrslib.h>

#include <agrum/core/OMPThreads.h>

namespace gum {
namespace credal {

  /**
   * @class CredalNet CredalNet.h <agrum/CN/CredalNet.h>
   * @brief Class template representing a Credal Network.
   *
   * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
   */
  template< typename GUM_SCALAR >
  class CredalNet {
    public:
      /** @brief nodeType to speed-up computations in some algorithms */
      enum nodeType {
        PRECISE = 0,
        CREDAL = 1,
        VACUOUS = 2,
				INDIC = 3
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
//////////////////////////////////////////
      /// @name Constructors / Destructors
//////////////////////////////////////////
      /// @{
      /**
       * Constructor for interval defined credal network which takes 2 BayesNet file path.
       * One can also provide a single BayesNet in order to perturb it's probability distributions into credal sets according to another BayesNet containing the number of cases, for each node, of each parent instanciation met during learning, i.e. \f$ p(X = 0 \mid pa(X) = j) = N_{pa(X) = j} \f$.
       *
       * @param src_min_num The path to a BayesNet which contains lower probabilities.
       * @param src_max_den The ( optional ) path to a BayesNet which contains upper probabilities.
       */
      CredalNet ( const std::string &src_min_num, const std::string &src_max_den = "" );

      /**
       * Constructor for interval defined credal network which takes 2 BayesNet. 
       * One can also provide a single BayesNet in order to perturb it's probability distributions into credal sets according to another BayesNet containing the number of cases, for each node, of each parent instanciation met during learning, i.e. \f$ p(X = 0 \mid pa(X) = j) = N_{pa(X) = j} \f$.
       *
       * @param src_min_num The BayesNet which contains lower probabilities.
       * @param src_max_den The ( optional ) BayesNet which contains upper probabilities.
       */
      CredalNet ( const gum::BayesNet< GUM_SCALAR > & src_min_num, const gum::BayesNet< GUM_SCALAR > & src_max_den = gum::BayesNet< GUM_SCALAR >() );

      /**
       * Destructor.
       */
      ~CredalNet();

      /// @}

//////////////////////////////////////////
      /// @name Public manipulation methods
//////////////////////////////////////////
      /// @{
      /**
       * Perturbates the BayesNet provided as input for this CrealNet by generating intervals instead of point probabilities and then computes each vertex of each credal set.
       *
       * The perturbations are done according to the number of cases met for each node and each of it's parent instanciation, i.e. \f$ \epsilon = \beta^{ln(N_{pa(X) = j} + 1)} \f$ is the imprecision introduced which leads to \f$ \underline{p}(X = i \mid pa(X) = j) = (1 - \epsilon) p(X = i \mid pa(X) = j) \f$ and \f$ \overline{p}(X = i \mid pa(X) = j) = \underline{p}(X = i \mid pa(X) = j) + \epsilon \f$.
       *
       * @param beta The beta used to perturbate the network. \f$ 0 \leq \beta \leq 1 \f$.
       * 
       */
      void bnToCredal ( GUM_SCALAR beta );
      
      /**
			 * @deprecated Use intervalToCredal ( lrsWrapper with no input / output files needed ).
			 * 
       * Computes the vertices of each credal set according to their interval definition (uses lrs).
       */
      void intervalToCredalWithFiles ();
			
			/**
			 * Computes the vertices of each credal set according to their interval definition (uses lrs).
			 */
			void intervalToCredal ();
			
      /**
       * @deprecated
       * Transform this CredalNet using the Decision Theoretic Specification.
       * @warning not complete / definitive
       */
      void dts(); // for L2U, no arcs between decision node bits
      // not complete / to be removed
      /**
       * @deprecated
       * Binarise this CredalNet.
       * @warning not complete / definitive
       */
      void bin();
      //PH void indic();
      //PH void perState();
			
			/**
			 * @warning those are a test
			 */
			void approximatedBinarization();
      
      /// @}

      // other utility member methods
      //PH void saveCNet ( const std::string &cn_path ) const;
      //PH void loadCNet ( const std::string &src_cn_path );

      /**
       * If this CredalNet was built over a perturbed BayesNet, one can save the intervals as two BayesNet.
       *
       * @param min_path The path to save the BayesNet which contains the lower probabilities of each node X, i.e. \f$ \underline{p}(X = i \mid pa(X) = j) \f$.
       * @param max_path The path to save the BayesNet which contains the upper probabilities of each node X, i.e. \f$ \overline{p}(X = i \mid pa(X) = j) \f$.
       */
      void saveBNsMinMax( const std::string & min_path, const std::string & max_path ) const;

      //PH void vacants ( int &result ) const;
      //PH void notVacants ( int &result ) const;
      //PH void averageVertices ( double &result ) const;

      /**
       * @return Returns the string representation of this CredalNet, i.e. it's CPTs (which also represent arcs).
       */
      std::string toString() const;
      //PH void toPNG ( const std::string &png_path ) const;
      
      /**
       * Used with binary networks to speed-up L2U inference.
       *
       * Store the lower and upper probabilities of each node X over the "true" modality, i.e. respectively \f$ \underline{p}(X = 1 \mid pa(X) = j) \f$ and \f$ \overline{p}(X = 1 \mid pa(X) = j) \f$.
       */
      void computeCPTMinMax(); // REDO THIS IN PRIVATE !!!

////////////////////////////////////////// 
      /// @name Getters and setters 
//////////////////////////////////////////
      /// @{
      /**
       * @return Returns a constant reference to the original BayesNet (used as a DAG, it's CPTs does not matter).
       */
      const gum::BayesNet< GUM_SCALAR > & src_bn() const;

      /**
       * @return Returs a constant reference to the actual BayesNet (used as a DAG, it's CPTs does not matter).
       */
      const gum::BayesNet< GUM_SCALAR > & current_bn() const;

      /**
       * @return Returns a constant reference to the ( up-to-date ) CredalNet CPTs.
       */
      const typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes &credalNet_cpt() const;

      /**
       * @param id The constant reference to the choosen NodeId
       * @return Returns the type of the choosen node in the ( up-to-date ) CredalNet.
       */
      nodeType getNodeType ( const gum::NodeId &id ) const;

      /**
       * @return Returns a constant reference to the lowest perturbation of the BayesNet provided as input for this CredalNet.
       */
      const double & getEpsilonMin() const;

      /**
       * @return Returns a constant reference to the highest perturbation of the BayesNet provided as input for this CredalNet.
       */
      const double & getEpsilonMax() const;

      /**
       * @return Returns a constant reference to the average perturbation of the BayesNet provided as input for this CredalNet.
       */
      const double & getEpsilonMoy() const;

      /**
       * @return Returns \c TRUE is this CredalNet is separately and interval specified, \c FALSE otherwise.
       */
      const bool isSeparatelySpecified() const;

      /**
       * Used with binary networks to speed-up L2U inference.
       *
       * @return Returns a constant reference to the lower probabilities of each node X over the "true" modality, i.e. \f$ \underline{p}(X = 1 \mid pa(X) = j) \f$.
       */
      const std::vector< std::vector< GUM_SCALAR > > & get_CPT_min() const;

      /**
       * Used with binary networks to speed-up L2U inference.
       *
       * @return Returns a constant reference to the upper probabilities of each node X over the "true" modality, i.e. \f$ \overline{p}(X = 1 \mid pa(X) = j) \f$.
       */
      const std::vector< std::vector< GUM_SCALAR > > & get_CPT_max() const;

      //PH const std::vector< std::vector< gum::NodeId > > & var_bits() const;
      /// @}
    
    protected:

    private:
      /** @brief 1e6 by default, used by __fracC as precision. */
      double __precisionC;// = 1e6;
      /** @brief 5 by default, used by __fracC as number of decimals. */
      double __deltaC;// = 5;

      /** @brief The lowest perturbation of the BayesNet provided as input for this CredalNet. */
      double __epsilonMin;
      /** @brief The highest perturbation of the BayesNet provided as input for this CredalNet. */
      double __epsilonMax;
      /** @brief The average perturbation of the BayesNet provided as input for this CredalNet. */
      double __epsilonMoy;

      /** @brief Value under which a decimal number is considered to be zero when computing redundant vertices. */
      double __epsRedund;//= 1e-4;

      /** @brief Value under which a decimal number is considered to be zero when using __farey. */
      double __epsF;// = 1e-6;
      /** @brief Highest possible denominator allowed when using __farey. A value too high may lead to lrs being unable to find vertices. */
      double __denMax;// = 1e6; // beware LRS

      /** @brief Precision used by __frac. */
      int __precision;// = 1e6; // beware LRS

      /** @brief \c TRUE if this CredalNet is separately and interval specified. */
      bool __separatelySpecified;

      /** @brief Original BayesNet (used as a DAG). Is never modified. */
      gum::BayesNet< GUM_SCALAR > __src_bn;

      /** @brief BayesNet used to store lower probabilities. */
      gum::BayesNet< GUM_SCALAR > __src_bn_min;
      /** @brief BayesNet used to store upper probabilities. */
      gum::BayesNet< GUM_SCALAR > __src_bn_max;

      /** @brief Up-to-date BayesNet (used as a DAG). */
      gum::BayesNet< GUM_SCALAR > * __current_bn;// = NULL;

      /** @brief This CredalNet original CPTs. */
      typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes __credalNet_src_cpt;

      /** @brief This CredalNet up-to-date CPTs. */
      typename gum::Property< std::vector< std::vector< std::vector< GUM_SCALAR > > > >::onNodes *__credalNet_current_cpt;// =  NULL;

      /** @deprecated @brief Corresponding bits of each variable. */
      typename gum::Property< std::vector< gum::NodeId > >::onNodes __var_bits;

      /** @brief The nodeType of each node from the ORIGINAL network. */
      typename gum::Property< nodeType >::onNodes __nodeType;
      /** @brief The nodeType of each node from the up-to-date network. */
      typename gum::Property< nodeType >::onNodes *__current_nodeType;// = NULL;

      /** 
       * @brief Used with binary networks to speed-up L2U inference. Store the lower probabilities of each node X over the "true" modality, i.e. \f$ \underline{p}(X = 1 \mid pa(X) = j) \f$.
       */
      typename std::vector< std::vector< GUM_SCALAR > > binCptMin;

      /** 
       * @brief Used with binary networks to speed-up L2U inference. Store the upper probabilities of each node X over the "true" modality, i.e. \f$ \overline{p}(X = 1 \mid pa(X) = j) \f$.
       */
      typename std::vector< std::vector< GUM_SCALAR > > binCptMax;

      /** @brief Set the nodeType of each node */
      void __sort_varType();

      /**
       * @deprecated
       * @param var_cpt The reference to a node CPT which may need a Decision Node.
       * @return Returns the cardinality of the Decision Node.
       */
      int __find_dNode_card ( const std::vector< std::vector< std::vector< GUM_SCALAR > > > & var_cpt ) const;

      /**
       * Computes the vertices of each credal set according to their interval definition (does not use lrs). Only works with credal sets defined such that when one modality reach it's upper probability, all others are at their lowest.
       *
       * Called by bnToCredal.
       */
      void __intervalToCredal();
      
      /** @brief Initialize private constant variables after the Constructor has been called */
      void __initParams();

      /** @brief Initialize private BayesNet variables after the Constructor has been called */
      void __initCNNets ( const std::string &src_min_num, const std::string &src_max_den );

      /** @brief Initialize private BayesNet variables after the Constructor has been called */
      void __initCNNets ( const gum::BayesNet< GUM_SCALAR > & src_min_num, const gum::BayesNet< GUM_SCALAR > & src_max_den );

      /**
			 * @deprecated
       * @warning May be useless since the BayesNet copy constructor seems to now work well (parent order is preserved).
       *
       * Copy the up-to-date BayesNet associated with this CredalNet. Since all we care about is the DAG, only arcs are copied. Because the order with which arcs are created is important, the function iterates over the CPTs variables to be sure parent order stays the same from a net to it's copy.
       *
       * @param bn_dest The reference to the new copy
       */
      void __bnCopy ( gum::BayesNet< GUM_SCALAR > & bn_dest );

      //void __H2Vcdd ( const std::vector< std::vector< GUM_SCALAR > > & h_rep, std::vector< std::vector< GUM_SCALAR > > & v_rep ) const;
      /**
			 * @deprecated one should use the LrsWrapper class
       * Computes the V-representation of a credal set, i.e. it's vertices, from it's H-representation, i.e. the hyper-plan inequalities. Uses lrs.
       *
       * @param h_rep A constant reference to the H-representation of a credal set.
       * @param v_rep A reference to the V-representation of the same credal set.
       */
      void __H2Vlrs ( const std::vector< std::vector< GUM_SCALAR > > & h_rep, std::vector< std::vector< GUM_SCALAR > > & v_rep ) const;

  }; // CredalNet

} // namespace cn
}

#include <agrum/CN/CredalNet.tcc>

#endif // __CREDAL_NET__H__
