#ifndef __CN_MC_SAMPLING__H__
#define __CN_MC_SAMPLING__H__

#include <limits>
#include <agrum/CN/MultipleInferenceEngines.h>

namespace gum {
  namespace credal {

  /**
   * @class MCSampling MCSampling.h <agrum/CN/MCSampling.h>
   * @brief Basic sampling algorithm (pure random).
   * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
   * @tparam BNInferenceEngine A BayesNet inference engine such as LazyPropagation.
   */
  template< typename GUM_SCALAR, class BNInferenceEngine >
  class MCSampling : public MultipleInferenceEngines< GUM_SCALAR, BNInferenceEngine > {
    private:
      /** To easily acces MultipleInferenceEngines< GUM_SCALAR, BNInferenceEngine > methods. */
      typedef MultipleInferenceEngines< GUM_SCALAR, BNInferenceEngine > infEs;

//////////////////////////////////////////
      /// @name Private initialization methods
//////////////////////////////////////////
      /// @{
      /** Initialize approximation Scheme. */
      void __mcInitApproximationScheme();
      /** Initialize threads data. */
      void __mcThreadDataCopy();
      /// @}

//////////////////////////////////////////
      /// @name Private algorithm methods
//////////////////////////////////////////
      /// @{
      /** Thread samples a BayesNet from the CredalNet. */
      inline void __verticesSampling();

      /** Insert CredalNet evidence into a thread BNInferenceEngine. */
      inline void __insertEvidence();

      /** Thread performs an inference using BNInferenceEngine. Calls __verticesSampling and __insertEvidence. */
      inline void __threadInference();

      /** Update thread data after a BayesNet inference. */
      inline void __threadUpdate();

      /**
       * Get the binary representation of a given value.
       * @param toFill A reference to the bits to fill. Size must be correct before passing argument (i.e. big enough to represent \c value)
       * @param value The constant integer we want to binarize.
       */
      inline void __binaryRep ( std::vector< bool > & toFill,  const unsigned int value ) const;

      /// @}


    protected:

    public:
//////////////////////////////////////////
      /// @name Constructors / Destructors
//////////////////////////////////////////
      /// @{
      /**
       * Constructor.
       * @param credalNet The CredalNet to be used by the algorithm.
       */
      MCSampling ( const CredalNet< GUM_SCALAR > & credalNet );
      /** Destructor. */
      virtual ~MCSampling();
      /// @}

//////////////////////////////////////////
      /// @name Public algorithm methods
//////////////////////////////////////////
      /// @{

      /** Starts the inference. */
      void makeInference();

      /// @}

      /** Erase all inference data (including thread data). */
      //void eraseAllEvidence();

      //// debug /////

      unsigned int notOptDelete;

      ////////////////

    protected:
      bool _repetitiveInd;
  };


  } // namespace cn
}

#include <agrum/CN/MCSampling.tcc>

#endif
