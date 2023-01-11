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


#include <agrum/CN/tools/varMod2BNsMap.h>

namespace gum {
  namespace credal {

    template < typename GUM_SCALAR >
    VarMod2BNsMap< GUM_SCALAR >::VarMod2BNsMap() {
      cnet = nullptr;

      GUM_CONSTRUCTOR(VarMod2BNsMap);
    }

    template < typename GUM_SCALAR >
    VarMod2BNsMap< GUM_SCALAR >::VarMod2BNsMap(const CredalNet< GUM_SCALAR >& cn) {
      setCNet(cn);

      GUM_CONSTRUCTOR(VarMod2BNsMap);
    }

    template < typename GUM_SCALAR >
    VarMod2BNsMap< GUM_SCALAR >::~VarMod2BNsMap() {
      GUM_DESTRUCTOR(VarMod2BNsMap);
    }

    template < typename GUM_SCALAR >
    void VarMod2BNsMap< GUM_SCALAR >::setCNet(const CredalNet< GUM_SCALAR >& cn) {
      auto* cpt    = &cn.credalNet_currentCpt();
      auto  nNodes = cpt->size();
      sampleDef_.resize(nNodes);

      for (NodeId node = 0; node < nNodes; node++) {
        auto pConfs = (*cpt)[node].size();
        sampleDef_[node].resize(pConfs);

        for (Size pconf = 0; pconf < pConfs; pconf++) {
          Size          nVertices = Size((*cpt)[node][pconf].size());
          unsigned long b, c;   // needed by superiorPow
          superiorPow(static_cast< unsigned long >(nVertices), b, c);
          Size nBits = Size(b);
          sampleDef_[node][pconf].resize(nBits);
        }
      }

      cnet = &cn;
    }

    template < typename GUM_SCALAR >
    bool VarMod2BNsMap< GUM_SCALAR >::insert(const std::vector< bool >& bn,
                                             const std::vector< Size >& key) {
      currentHash_            = Size(vectHash_(bn));
      std::list< Size >& nets = myVarHashs_.getWithDefault(key, std::list< Size >());   //[ key ];

      for (std::list< Size >::iterator it = nets.begin(); it != nets.end(); ++it) {
        if (*it == currentHash_) return false;
      }

      // add it
      myHashNet_.set(currentHash_, bn);   //[currentHash_] = bn;
      // insert net hash in our key net list
      nets.push_back(currentHash_);
      // insert out key in the hash key list
      myHashVars_.getWithDefault(currentHash_,
                                 std::list< varKey >()) /*[currentHash_]*/.push_back(key);
      return true;
    }

    template < typename GUM_SCALAR >
    bool VarMod2BNsMap< GUM_SCALAR >::insert(const std::vector< Size >& key, const bool isBetter) {
      if (isBetter) {
        // get all nets of this key (maybe entry does not exists)
        std::list< Size >& old_nets
           = myVarHashs_.getWithDefault(key, std::list< Size >());   //[ key ];

        // for each one
        for (std::list< Size >::iterator it = old_nets.begin(); it != old_nets.end(); ++it) {
          // get all keys associated to this net
          std::list< varKey >& netKeys
             = myHashVars_.getWithDefault(*it, std::list< varKey >());   //[ *it ];

          // if we are the sole user, delete the net entry
          if (netKeys.size() == 1) {
            myHashVars_.erase(*it);
          }
          // other keys use the net, delete our key from list
          else {
            for (std::list< varKey >::iterator it2 = netKeys.begin(); it2 != netKeys.end(); ++it2) {
              if (*it2 == key) {
                netKeys.erase(it2);
                break;
              }
            }
          }
        }   // end of : for each old_net

        // clear all old_nets
        old_nets.clear();
        // insert new net with it's hash
        myHashNet_.set(currentHash_,
                       currentSample_);   //[currentHash_] = currentSample_;
        // insert net hash in our key net list
        old_nets.push_back(currentHash_);
        // insert out key in the hash key list
        myHashVars_
           .getWithDefault(currentHash_, std::list< varKey >()) /*[currentHash_]*/
           .push_back(key);
        return true;

      }   // end of isBetter
      // another opt net
      else {
        // check that we didn't add it for this key
        std::list< Size >& nets = myVarHashs_.getWithDefault(key, std::list< Size >());   //[ key ];

        for (std::list< Size >::iterator it = nets.begin(); it != nets.end(); ++it) {
          if (*it == currentHash_) return false;
        }

        // add it
        myHashNet_.set(currentHash_, currentSample_);
        // insert net hash in our key net list
        nets.push_back(currentHash_);
        // insert out key in the hash key list
        myHashVars_.getWithDefault(currentHash_, std::list< varKey >()).push_back(key);

        /*
              // add it
              myHashNet_[currentHash_] = currentSample_;
              // insert net hash in our key net list
              nets.push_back(currentHash_);
              // insert out key in the hash key list
              myHashVars_[currentHash_].push_back(key);
        */
        return true;
      }   // end of ! isBetter
    }

    template < typename GUM_SCALAR >
    void VarMod2BNsMap< GUM_SCALAR >::setCurrentSample(
       const std::vector< std::vector< std::vector< bool > > >& sample) {
      currentSample_.clear();

      for (Size i = 0; i < sample.size(); i++)
        for (Size j = 0; j < sample[j].size(); j++)
          for (Size k = 0; k < sample[i][j].size(); k++)
            currentSample_.push_back(sample[i][j][k]);

      // std::cout << sample << std::endl;
      // std::cout << currentSample_ << std::endl;

      currentHash_ = Size(vectHash_(currentSample_));
    }

    template < typename GUM_SCALAR >
    const std::vector< bool >& VarMod2BNsMap< GUM_SCALAR >::getCurrentSample() {
      return currentSample_;
    }

    template < typename GUM_SCALAR >
    const std::vector< std::vector< std::vector< bool > > >&
       VarMod2BNsMap< GUM_SCALAR >::getSampleDef() {
      return sampleDef_;
    }

    template < typename GUM_SCALAR >
    const std::vector< std::vector< bool >* >
       VarMod2BNsMap< GUM_SCALAR >::getBNOptsFromKey(const std::vector< Size >& key) {
      // return something even if key does not exist
      if (!myVarHashs_.exists(key)) return std::vector< std::vector< bool >* >();

      std::list< Size >& netsHash = myVarHashs_[key];   //.at(key);

      std::vector< dBN* > nets;
      nets.resize(netsHash.size());

      std::list< Size >::iterator it = netsHash.begin();

      for (Size i = 0; i < netsHash.size(); i++, ++it) {
        nets[i] = &myHashNet_ /*.at(*/[*it];   //);
      }

      return nets;
    }

    template < typename GUM_SCALAR >
    std::vector< std::vector< std::vector< std::vector< bool > > > >
       VarMod2BNsMap< GUM_SCALAR >::getFullBNOptsFromKey(const std::vector< Size >& key) {
      if (cnet == nullptr)
        GUM_ERROR(OperationNotAllowed,
                  "No CredalNet associated to me ! Can't get FullBNOptsFromKey : " << key);

      if (!myVarHashs_.exists(key))
        return std::vector< std::vector< std::vector< std::vector< bool > > > >();

      std::list< Size >& netsHash = myVarHashs_[key];   //.at(key);

      std::vector< std::vector< std::vector< std::vector< bool > > > > nets;
      nets.resize(netsHash.size(), sampleDef_);

      std::list< Size >::iterator it = netsHash.begin();

      for (Size i = 0; i < netsHash.size(); i++, ++it) {
        // std::vector< std::vector< std::vector < bool > > > net(sampleDef_);
        dBN::iterator it2 = myHashNet_ /*.at(*/[*it] /*)*/.begin();

        for (Size j = 0; j < sampleDef_.size(); j++) {
          for (Size k = 0; k < sampleDef_[j].size(); k++) {
            for (Size l = 0; l < sampleDef_[j][k].size(); l++) {
              nets[i][j][k][l] = *it2;
              ++it2;
            }
          }
        }
      }

      return nets;
    }

    template < typename GUM_SCALAR >
    Size VarMod2BNsMap< GUM_SCALAR >::getEntrySize() const {
      return myHashNet_.size();
    }

  }   // namespace credal
}   // namespace gum
