/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Class used to store optimum IBayesNet during some inference
 * algorithms.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef __VAR_MOD_2_BNS_MAP__H__
#define __VAR_MOD_2_BNS_MAP__H__

#include <list>

// #include <tr1/functional>


#include <agrum/CN/credalNet.h>

namespace gum {
  namespace credal {

    /**
     * @class VarMod2BNsMap
     * @headerfile varMod2BNsMap.h <agrum/CN/varMod2BNsMap.h>
     * @brief Class used to store optimum IBayesNet during some inference
     * algorithms.
     * @ingroup cn_group
     * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
     * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
     */

    template < typename GUM_SCALAR >
    class VarMod2BNsMap {
      private:
      using dBN      = std::vector< bool >;
      using varKey   = std::vector< Size >;
      using hashNet  = gum::HashTable< Size, dBN >;
      using varHashs = gum::HashTable< varKey, std::list< Size > >;
      using hashVars = gum::HashTable< Size, std::list< varKey > >;

      protected:
      /** Map id - dBN : get a net from it's id. */
      hashNet myHashNet_;
      /** Map varKey - list(id) : get all optimal nets id from the given key :
       * variable, modality and min=0 ( or max=1 ). */
      varHashs myVarHashs_;
      /** Map id - list(varKey) : get all variables, modalities associated to
       * this
       * optimal net id. */
      hashVars myHashVars_;

      /** Since all samples have the same structure, this will be used as
       * default
       * initialization (copy constructor) for any sample. */
      std::vector< std::vector< std::vector< bool > > > sampleDef_;

      /** The current sampled IBayesNet. */
      dBN currentSample_;
      /** The current sampled IBayesNet hash. */
      Size currentHash_;
      /** The hash fuction. Requires c++11/0x. */
      std::hash< std::vector< bool > > vectHash_;

      /** A pointer to the CredalNet to be used. */
      const CredalNet< GUM_SCALAR >* cnet;

      public:
      /// @name Constructors / Destructors
      /// @{

      /**
       * Thread constructor.
       * Initialize  _sampleDef_ from the CredalNet.
       *
       * @param cn The CredalNet to be used.
       */
      explicit VarMod2BNsMap(const CredalNet< GUM_SCALAR >& cn);

      /** Default constructor that should be used only by InferenceEngine since
       * it
       * has a member variable. */
      VarMod2BNsMap();

      /** Destructor. */
      ~VarMod2BNsMap();

      /// @}

      /**
       * Insert for thread fusion. All inserted nets are optimums and none will
       * be
       * deleted because a better one is being inserted.
       * @param bn The constant reference to the net to be inserted.
       * @param key The constant reference to the key at which we will insert
       * the
       * net.
       */
      bool insert(const std::vector< bool >& bn, const std::vector< Size >& key);

      /**
       * Thread insert.
       * @param key The key at which the net will be inserted.
       * @param isBetter \c True if the net is a better one, i.e. it gave better
       * marginals.
       * @return \c True if the net was inserted, \c false otherwise.
       */
      bool insert(const std::vector< Size >& key, const bool isBetter);

      /// @name Getters and setters
      /// @{

      /**
       * Initialize  _sampleDef_ from the CredalNet.
       *
       * Shoud only be used by InferenceEngine to initialize it's member
       *variable.
       * @param cn The CredalNet to be used.
       */
      void setCNet(const CredalNet< GUM_SCALAR >& cn);

      /**
       * %Set the current thread sample and it's hash.
       * @param sample The constant reference to the sample with structure.
       */
      void setCurrentSample(const std::vector< std::vector< std::vector< bool > > >& sample);

      /**
       * Get the current sample as a vector of bits without structure.
       * @return The constant reference to the sampled IBayesNet without
       * structure.
       */
      const dBN& getCurrentSample();

      /**
       * Get the sample structure.
       * @return The constant reference to the sample structure.
       */
      const std::vector< std::vector< std::vector< bool > > >& getSampleDef();

      /**
       * Get optimum IBayesNet (s) without structure of the given variable,
       * modality
       * for min or max.
       * @param key The constant reference to the variable, modality, min or
       * max.
       * @return The constant vector of not yet constant pointers to the nets.
       */
      const std::vector< dBN* > getBNOptsFromKey(const std::vector< Size >& key);

      /**
       * Get optimum IBayesNet (s) with structure of the given variable, modality
       * for
       * min or max.
       * @param key The constant reference to the variable, modality, min or max.
       * @return The vector of not yet constant pointers to the nets.
       */
      std::vector< std::vector< std::vector< std::vector< bool > > > >
         getFullBNOptsFromKey(const std::vector< Size >& key);

      /** Get the number of IBayesNet stored. */
      Size getEntrySize() const;

      /// @}

    };   // end of OptBN


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class VarMod2BNsMap< double >;
#endif

  }   // namespace credal
}   // namespace gum

#include <agrum/CN/tools/varMod2BNsMap_tpl.h>

#endif
