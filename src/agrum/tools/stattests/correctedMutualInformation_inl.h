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
 * @brief The class computing n times the corrected mutual information,
 * as used in the 3off2 algorithm
 *
 * @author Quentin FALCAND, Christophe GONZALES(_at_AMU) and Pierre-Henri
 * WUILLEMIN(_at_LIP6).
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// turn on/off the use of all the caches
    INLINE void CorrectedMutualInformation::useCache(bool on_off) {
      useICache(on_off);
      useHCache(on_off);
      useKCache(on_off);
      useCnrCache(on_off);
    }


    /// turn on/off the use of the I cache
    INLINE void CorrectedMutualInformation::useICache(bool on_off) {
      if (!on_off) _ICache_.clear();
      _use_ICache_ = on_off;
    }


    /// turn on/off the use of the H cache
    INLINE void CorrectedMutualInformation::useHCache(bool on_off) {
      if (!on_off) _NH_.clearCache();
      _use_HCache_ = on_off;
      _NH_.useCache(on_off);
    }


    /// turn on/off the use of the K cache
    INLINE void CorrectedMutualInformation::useKCache(bool on_off) {
      if (!on_off) _KCache_.clear();
      _use_KCache_ = on_off;
    }


    /// turn on/off the use of the Cnr cache
    INLINE void CorrectedMutualInformation::useCnrCache(bool on_off) {
      if (!on_off) _k_NML_.clearCache();
      _use_CnrCache_ = on_off;
      _k_NML_.useCache(on_off);
    }


    /// clears all the data structures from memory
    INLINE void CorrectedMutualInformation::clear() {
      _NH_.clear();
      _k_NML_.clear();
      _score_MDL_.clear();
      clearCache();
    }


    /// clears the current cache (clear nodesets as well)
    INLINE void CorrectedMutualInformation::clearCache() {
      _NH_.clearCache();
      _k_NML_.clearCache();
      _ICache_.clear();
      _KCache_.clear();
    }


    /// clears the ICache (the mutual information  cache)
    INLINE void CorrectedMutualInformation::clearICache() { _ICache_.clear(); }


    /// clears the HCache (the cache for the entropies)
    INLINE void CorrectedMutualInformation::clearHCache() { _NH_.clearCache(); }


    /// clears the KCache (the cache for the penalties)
    INLINE void CorrectedMutualInformation::clearKCache() { _KCache_.clear(); }


    /// clears the CnrCache (the cache for the Cnr formula)
    INLINE void CorrectedMutualInformation::clearCnrCache() { _k_NML_.clearCache(); }


    /// changes the max number of threads used to parse the database
    INLINE void CorrectedMutualInformation::setNumberOfThreads(Size nb) {
      _NH_.setNumberOfThreads(nb);
      _k_NML_.setNumberOfThreads(nb);
      _score_MDL_.setNumberOfThreads(nb);
    }


    /// returns the number of threads used to parse the database
    INLINE Size CorrectedMutualInformation::getNumberOfThreads() const {
      return _NH_.getNumberOfThreads();
    }


    /// indicates whether the user set herself the number of threads
    INLINE bool CorrectedMutualInformation::isGumNumberOfThreadsOverriden() const {
      return _NH_.isGumNumberOfThreadsOverriden();
    }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    INLINE void CorrectedMutualInformation::setMinNbRowsPerThread(const std::size_t nb) const {
      _NH_.setMinNbRowsPerThread(nb);
      _k_NML_.setMinNbRowsPerThread(nb);
      _score_MDL_.setMinNbRowsPerThread(nb);
    }


    /// returns the minimum of rows that each thread should process
    INLINE std::size_t CorrectedMutualInformation::minNbRowsPerThread() const {
      return _NH_.minNbRowsPerThread();
    }


    /// returns the current ranges
    INLINE const std::vector< std::pair< std::size_t, std::size_t > >&
                 CorrectedMutualInformation::ranges() const {
      return _NH_.ranges();
    }


    /// use the MDL penalty function
    INLINE void CorrectedMutualInformation::useMDL() {
      clearCache();
      _kmode_ = KModeTypes::MDL;
    }


    /// use the kNML penalty function
    INLINE void CorrectedMutualInformation::useNML() {
      clearCache();
      _kmode_ = KModeTypes::NML;
    }


    /// use no correction/penalty function
    INLINE void CorrectedMutualInformation::useNoCorr() {
      clearCache();
      _kmode_ = KModeTypes::NoCorr;
    }


    /// returns the 2-point mutual information corresponding to a given nodeset
    INLINE double CorrectedMutualInformation::score(NodeId var1, NodeId var2) {
      return score(var1, var2, _empty_conditioning_set_);
    }


    /// returns the 2-point mutual information corresponding to a given nodeset
    INLINE double CorrectedMutualInformation::score(NodeId                       var1,
                                                    NodeId                       var2,
                                                    const std::vector< NodeId >& conditioning_ids) {
      return _NI_score_(var1, var2, conditioning_ids) - _K_score_(var1, var2, conditioning_ids);
    }


    /// returns the 3-point mutual information corresponding to a given nodeset
    INLINE double CorrectedMutualInformation::score(NodeId var1, NodeId var2, NodeId var3) {
      return score(var1, var2, var3, _empty_conditioning_set_);
    }


    /// returns the 3-point mutual information corresponding to a given nodeset
    INLINE double CorrectedMutualInformation::score(NodeId                       var1,
                                                    NodeId                       var2,
                                                    NodeId                       var3,
                                                    const std::vector< NodeId >& conditioning_ids) {
      return _NI_score_(var1, var2, var3, conditioning_ids)
           + _K_score_(var1, var2, var3, conditioning_ids);
    }


    /// return N times the mutual information for conditioned triples of variables
    INLINE double CorrectedMutualInformation::_NI_score_(NodeId                       var_x,
                                                         NodeId                       var_y,
                                                         NodeId                       var_z,
                                                         const std::vector< NodeId >& ui_ids) {
      // conditional 3-point mutual information formula:
      // I(x;y;z|{ui}) = I(x;y|{ui}) - I(x;y|z,{ui})
      std::vector< NodeId > uiz_ids = ui_ids;
      uiz_ids.push_back(var_z);
      return _NI_score_(var_x, var_y, ui_ids) - _NI_score_(var_x, var_y, uiz_ids);
    }


    /// 3pt penalty
    INLINE double CorrectedMutualInformation::_K_score_(NodeId                       var1,
                                                        NodeId                       var2,
                                                        NodeId                       var3,
                                                        const std::vector< NodeId >& ui_ids) {
      // k(x;y;z|ui) = k(x;y|ui,z) - k(x;y|ui)
      std::vector< NodeId > uiz_ids = ui_ids;
      uiz_ids.push_back(var3);
      return _K_score_(var1, var2, uiz_ids) - _K_score_(var1, var2, ui_ids);
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
