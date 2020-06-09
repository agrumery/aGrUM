
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
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


/**
 * @file
 * @brief Inference by basic sampling algorithm (pure random) of bnet in credal
 * networks.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef __CN_MC_SAMPLING__H__
#define __CN_MC_SAMPLING__H__

#include <agrum/CN/multipleInferenceEngine.h>
#include <limits>

#include <agrum/BN/inference/lazyPropagation.h>

namespace gum {
  namespace credal {

    /**
     * @class CNMonteCarloSampling CNMonteCarloSampling.h
     *<agrum/CN/CNMonteCarloSampling.h>
     * @brief Inference by basic sampling algorithm (pure random) of bnet in
     *credal
     *networks.
     * @ingroup cn_group
     * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
     * @tparam BNInferenceEngine A IBayesNet inference engine such as
     *LazyPropagation
     *( recommanded ).
     * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(@LIP6)
     *
     * @warning p(e) must be available ( by a call to
     *my_BNInferenceEngine.evidenceMarginal() ) !! the vertices are correct if
     *p(e) >
     *0 for a sample
     * the test is made once
     */
    template < typename GUM_SCALAR,
               class BNInferenceEngine = LazyPropagation< GUM_SCALAR > >
    class CNMonteCarloSampling:
        public MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine > {
      private:
      /** To easily acces MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine
       * >
       * methods. */
      using infEs__ = MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >;

      /// @name Private initialization methods
      /// @{
      /** Initialize approximation Scheme. */
      void mcInitApproximationScheme__();
      /** Initialize threads data. */
      void mcThreadDataCopy__();
      /// @}

      /// @name Private algorithm methods
      /// @{
      /** Thread samples a IBayesNet from the CredalNet. */
      inline void verticesSampling__();

      /** Insert CredalNet evidence into a thread BNInferenceEngine. */
      inline void insertEvidence__();

      /** Thread performs an inference using BNInferenceEngine. Calls
       * verticesSampling__ and insertEvidence__. */
      inline void threadInference__();

      /** Update thread data after a IBayesNet inference. */
      inline void threadUpdate__();

      /**
       * Get the binary representation of a given value.
       * @param toFill A reference to the bits to fill. Size must be correct
       * before
       * passing argument (i.e. big enough to represent \c value)
       * @param value The constant integer we want to binarize.
       */
      inline void binaryRep__(std::vector< bool >& toFill, const Idx value) const;

      /// @}

      protected:
      public:
      /// @name Constructors / Destructors
      /// @{
      /**
       * Constructor.
       * @param credalNet The CredalNet to be used by the algorithm.
       */
      explicit CNMonteCarloSampling(const CredalNet< GUM_SCALAR >& credalNet);
      /** Destructor. */
      virtual ~CNMonteCarloSampling();
      /// @}

      /// @name Public algorithm methods
      /// @{

      /** Starts the inference. */
      void makeInference();

      /// @}

      /// unsigned int notOptDelete;

      virtual void insertEvidenceFile(const std::string& path) {
        InferenceEngine< GUM_SCALAR >::insertEvidenceFile(path);
      };

      protected:
      bool repetitiveInd_;
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class CNMonteCarloSampling< float, LazyPropagation< float > >;

    extern template class CNMonteCarloSampling< double,
                                                LazyPropagation< double > >;
#endif


  }   // namespace credal
}   // namespace gum

#include <agrum/CN/CNMonteCarloSampling_tpl.h>

#endif
