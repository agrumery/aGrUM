/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
 * @file
 * @brief Class used to store optimum IBayesNet during some inference algorithms.
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

#ifndef __VAR_MOD_2_BNS_MAP__H__
#define __VAR_MOD_2_BNS_MAP__H__

#include <list>

#include <tr1/functional>

#include <agrum/core/math/pow.h>
#include <agrum/core/hashTable.h>

#include <agrum/CN/credalNet.h>

namespace gum {
  namespace credal {

    /**
     * @class VarMod2BNsMap VarMod2BNsMap.h <agrum/CN/VarMod2BNsMap.h>
     * @brief Class used to store optimum IBayesNet during some inference algorithms.
     * @ingroup cn_group
     * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
     * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
     */

    template <typename GUM_SCALAR> class VarMod2BNsMap {
      private:
      typedef std::vector<bool> dBN;

      typedef typename std::vector<unsigned int> varKey;

      typedef typename gum::HashTable<Size, dBN> hashNet;

      typedef typename gum::HashTable<varKey, std::list<Size>> varHashs;

      typedef typename gum::HashTable<Size, std::list<varKey>> hashVars;

      protected:
      /** Map id - dBN : get a net from it's id. */
      hashNet _myHashNet;
      /** Map varKey - list(id) : get all optimal nets id from the given key :
       * variable, modality and min=0 ( or max=1 ). */
      varHashs _myVarHashs;
      /** Map id - list(varKey) : get all variables, modalities associated to this
       * optimal net id. */
      hashVars _myHashVars;

      /** Since all samples have the same structure, this will be used as default
       * initialization (copy constructor) for any sample. */
      std::vector<std::vector<std::vector<bool>>> _sampleDef;

      /** The current sampled IBayesNet. */
      dBN _currentSample;
      /** The current sampled IBayesNet hash. */
      Size _currentHash;
      /** The hash fuction. Requires c++11/0x. */
      std::hash<std::vector<bool>> _vectHash;

      /** A pointer to the CredalNet to be used. */
      const CredalNet<GUM_SCALAR> *cnet;

      public:
      /// @name Constructors / Destructors
      /// @{

      /**
       * Thread constructor.
       * Initialize __sampleDef from the CredalNet.
       *
       * @param cn The CredalNet to be used.
       */
      VarMod2BNsMap(const CredalNet<GUM_SCALAR> &cn);

      /** Default constructor that should be used only by InferenceEngine since it
       * has a member variable. */
      VarMod2BNsMap();

      /** Destructor. */
      ~VarMod2BNsMap();

      /// @}

      /**
       * Insert for thread fusion. All inserted nets are optimums and none will be
       * deleted because a better one is being inserted.
       * @param bn The constant reference to the net to be inserted.
       * @param key The constant reference to the key at which we will insert the
       * net.
       */
      bool insert(const std::vector<bool> &bn, const std::vector<unsigned int> &key);

      /**
       * Thread insert.
       * @param key The key at which the net will be inserted.
       * @param isBetter \c True if the net is a better one, i.e. it gave better
       * marginals.
       * @return \c True if the net was inserted, \c false otherwise.
       */
      bool insert(const std::vector<unsigned int> &key, const bool isBetter);

      /// @name Getters and setters
      /// @{

      /**
       * Initialize __sampleDef from the CredalNet.
       *
       * Shoud only be used by InferenceEngine to initialize it's member variable.
       * @param cn The CredalNet to be used.
       */
      void setCNet(const CredalNet<GUM_SCALAR> &cn);

      /**
       * %Set the current thread sample and it's hash.
       * @param sample The constant reference to the sample with structure.
       */
      void
      setCurrentSample(const std::vector<std::vector<std::vector<bool>>> &sample);

      /**
       * Get the current sample as a vector of bits without structure.
       * @return The constant reference to the sampled IBayesNet without structure.
       */
      const dBN &getCurrentSample();

      /**
       * Get the sample structure.
       * @return The constant reference to the sample structure.
       */
      const std::vector<std::vector<std::vector<bool>>> &getSampleDef();

      /**
       * Get optimum IBayesNet (s) without structure of the given variable, modality
       * for min or max.
       * @param key The constant reference to the variable, modality, min or max.
       * @return The constant vector of not yet constant pointers to the nets.
       */
      const std::vector<dBN *>
      getBNOptsFromKey(const std::vector<unsigned int> &key);

      /**
      * Get optimum IBayesNet (s) with structure of the given variable, modality for
      * min or max.
      * @param key The constant reference to the variable, modality, min or max.
      * @return The vector of not yet constant pointers to the nets.
      */
      std::vector<std::vector<std::vector<std::vector<bool>>>>
      getFullBNOptsFromKey(const std::vector<unsigned int> &key);

      /** Get the number of IBayesNet stored. */
      unsigned int getEntrySize() const;

      /// @}

    }; // end of OptBN

    extern template class VarMod2BNsMap<float>;
    extern template class VarMod2BNsMap<double>;
  } // credal namespace
} // gum namespace

#include <agrum/CN/varMod2BNsMap.tcc>

#endif
