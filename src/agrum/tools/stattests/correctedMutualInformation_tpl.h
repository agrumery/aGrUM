/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Quentin FALCAND, Christophe GONZALES(@AMU) and Pierre-Henri
 * WUILLEMIN(@LIP6).
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// returns the allocator used by the score
    template < template < typename > class ALLOC >
    typename CorrectedMutualInformation< ALLOC >::allocator_type
       CorrectedMutualInformation< ALLOC >::getAllocator() const {
      return _NH_.getAllocator();
    }


    /// default constructor
    template < template < typename > class ALLOC >
    CorrectedMutualInformation< ALLOC >::CorrectedMutualInformation(
       const DBRowGeneratorParser< ALLOC >&                                 parser,
       const Apriori< ALLOC >&                                              apriori,
       const std::vector< std::pair< std::size_t, std::size_t >,
                          ALLOC< std::pair< std::size_t, std::size_t > > >& ranges,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&        nodeId2columns,
       const typename CorrectedMutualInformation< ALLOC >::allocator_type&  alloc) :
        _NH_(parser, apriori, ranges, nodeId2columns, alloc),
        _k_NML_(parser, apriori, ranges, nodeId2columns, alloc),
        _score_MDL_(parser, apriori, ranges, nodeId2columns, alloc), _ICache_(alloc),
        _KCache_(alloc) {
      GUM_CONSTRUCTOR(CorrectedMutualInformation);
    }


    /// default constructor
    template < template < typename > class ALLOC >
    CorrectedMutualInformation< ALLOC >::CorrectedMutualInformation(
       const DBRowGeneratorParser< ALLOC >&                                parser,
       const Apriori< ALLOC >&                                             apriori,
       const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&       nodeId2columns,
       const typename CorrectedMutualInformation< ALLOC >::allocator_type& alloc) :
        _NH_(parser, apriori, nodeId2columns, alloc),
        _k_NML_(parser, apriori, nodeId2columns, alloc),
        _score_MDL_(parser, apriori, nodeId2columns, alloc), _ICache_(alloc), _KCache_(alloc) {
      GUM_CONSTRUCTOR(CorrectedMutualInformation);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    CorrectedMutualInformation< ALLOC >::CorrectedMutualInformation(
       const CorrectedMutualInformation< ALLOC >&                          from,
       const typename CorrectedMutualInformation< ALLOC >::allocator_type& alloc) :
        _NH_(from._NH_, alloc),
        _k_NML_(from._k_NML_, alloc), _score_MDL_(from._score_MDL_, alloc), _kmode_(from._kmode_),
        _use_ICache_(from._use_ICache_), _use_HCache_(from._use_HCache_),
        _use_KCache_(from._use_KCache_), _use_CnrCache_(from._use_CnrCache_),
        _ICache_(from._ICache_, alloc), _KCache_(from._KCache_, alloc) {
      GUM_CONS_CPY(CorrectedMutualInformation);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    CorrectedMutualInformation< ALLOC >::CorrectedMutualInformation(
       const CorrectedMutualInformation< ALLOC >& from) :
        CorrectedMutualInformation(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    CorrectedMutualInformation< ALLOC >::CorrectedMutualInformation(
       CorrectedMutualInformation< ALLOC >&&                               from,
       const typename CorrectedMutualInformation< ALLOC >::allocator_type& alloc) :
        _NH_(std::move(from._NH_), alloc),
        _k_NML_(std::move(from._k_NML_), alloc), _score_MDL_(std::move(from._score_MDL_), alloc),
        _kmode_(from._kmode_), _use_ICache_(from._use_ICache_), _use_HCache_(from._use_HCache_),
        _use_KCache_(from._use_KCache_), _use_CnrCache_(from._use_CnrCache_),
        _ICache_(std::move(from._ICache_), alloc), _KCache_(std::move(from._KCache_), alloc) {
      GUM_CONS_MOV(CorrectedMutualInformation);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    CorrectedMutualInformation< ALLOC >::CorrectedMutualInformation(
       CorrectedMutualInformation< ALLOC >&& from) :
        CorrectedMutualInformation(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    CorrectedMutualInformation< ALLOC >* CorrectedMutualInformation< ALLOC >::clone(
       const typename CorrectedMutualInformation< ALLOC >::allocator_type& alloc) const {
      ALLOC< CorrectedMutualInformation< ALLOC > > allocator(alloc);
      CorrectedMutualInformation< ALLOC >*         new_score = allocator.allocate(1);
      try {
        allocator.construct(new_score, *this, alloc);
      } catch (...) {
        allocator.deallocate(new_score, 1);
        throw;
      }

      return new_score;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    CorrectedMutualInformation< ALLOC >* CorrectedMutualInformation< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    CorrectedMutualInformation< ALLOC >::~CorrectedMutualInformation() {
      // for debugging purposes
      GUM_DESTRUCTOR(CorrectedMutualInformation);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    CorrectedMutualInformation< ALLOC >& CorrectedMutualInformation< ALLOC >::operator=(
       const CorrectedMutualInformation< ALLOC >& from) {
      if (this != &from) {
        _NH_           = from._NH_;
        _k_NML_        = from._k_NML_;
        _score_MDL_    = from._score_MDL_;
        _kmode_        = from._kmode_;
        _use_ICache_   = from._use_ICache_;
        _use_HCache_   = from._use_HCache_;
        _use_KCache_   = from._use_KCache_;
        _use_CnrCache_ = from._use_CnrCache_;
        _ICache_       = from._ICache_;
        _KCache_       = from._KCache_;
      }
      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    CorrectedMutualInformation< ALLOC >&
       CorrectedMutualInformation< ALLOC >::operator=(CorrectedMutualInformation< ALLOC >&& from) {
      if (this != &from) {
        _NH_           = std::move(from._NH_);
        _k_NML_        = std::move(from._k_NML_);
        _score_MDL_    = std::move(from._score_MDL_);
        _kmode_        = from._kmode_;
        _use_ICache_   = from._use_ICache_;
        _use_HCache_   = from._use_HCache_;
        _use_KCache_   = from._use_KCache_;
        _use_CnrCache_ = from._use_CnrCache_;
        _ICache_       = std::move(from._ICache_);
        _KCache_       = std::move(from._KCache_);
      }
      return *this;
    }


    /// turn on/off the use of all the caches
    template < template < typename > class ALLOC >
    INLINE void CorrectedMutualInformation< ALLOC >::useCache(bool on_off) {
      useICache(on_off);
      useHCache(on_off);
      useKCache(on_off);
      useCnrCache(on_off);
    }


    /// turn on/off the use of the I cache
    template < template < typename > class ALLOC >
    INLINE void CorrectedMutualInformation< ALLOC >::useICache(bool on_off) {
      if (!on_off) _ICache_.clear();
      _use_ICache_ = on_off;
    }


    /// turn on/off the use of the H cache
    template < template < typename > class ALLOC >
    INLINE void CorrectedMutualInformation< ALLOC >::useHCache(bool on_off) {
      if (!on_off) _NH_.clearCache();
      _use_HCache_ = on_off;
      _NH_.useCache(on_off);
    }


    /// turn on/off the use of the K cache
    template < template < typename > class ALLOC >
    INLINE void CorrectedMutualInformation< ALLOC >::useKCache(bool on_off) {
      if (!on_off) _KCache_.clear();
      _use_KCache_ = on_off;
    }


    /// turn on/off the use of the Cnr cache
    template < template < typename > class ALLOC >
    INLINE void CorrectedMutualInformation< ALLOC >::useCnrCache(bool on_off) {
      if (!on_off) _k_NML_.clearCache();
      _use_CnrCache_ = on_off;
      _k_NML_.useCache(on_off);
    }


    /// clears all the data structures from memory
    template < template < typename > class ALLOC >
    INLINE void CorrectedMutualInformation< ALLOC >::clear() {
      _NH_.clear();
      _k_NML_.clear();
      _score_MDL_.clear();
      clearCache();
    }


    /// clears the current cache (clear nodesets as well)
    template < template < typename > class ALLOC >
    INLINE void CorrectedMutualInformation< ALLOC >::clearCache() {
      _NH_.clearCache();
      _k_NML_.clearCache();
      _ICache_.clear();
      _KCache_.clear();
    }


    /// clears the ICache (the mutual information  cache)
    template < template < typename > class ALLOC >
    INLINE void CorrectedMutualInformation< ALLOC >::clearICache() {
      _ICache_.clear();
    }


    /// clears the HCache (the cache for the entropies)
    template < template < typename > class ALLOC >
    INLINE void CorrectedMutualInformation< ALLOC >::clearHCache() {
      _NH_.clearCache();
    }


    /// clears the KCache (the cache for the penalties)
    template < template < typename > class ALLOC >
    INLINE void CorrectedMutualInformation< ALLOC >::clearKCache() {
      _KCache_.clear();
    }


    /// clears the CnrCache (the cache for the Cnr formula)
    template < template < typename > class ALLOC >
    INLINE void CorrectedMutualInformation< ALLOC >::clearCnrCache() {
      _k_NML_.clearCache();
    }


    /// changes the max number of threads used to parse the database
    template < template < typename > class ALLOC >
    void CorrectedMutualInformation< ALLOC >::setMaxNbThreads(std::size_t nb) const {
      _NH_.setMaxNbThreads(nb);
      _k_NML_.setMaxNbThreads(nb);
      _score_MDL_.setMaxNbThreads(nb);
    }


    /// returns the number of threads used to parse the database
    template < template < typename > class ALLOC >
    std::size_t CorrectedMutualInformation< ALLOC >::nbThreads() const {
      return _NH_.nbThreads();
    }


    /** @brief changes the number min of rows a thread should process in a
     * multithreading context */
    template < template < typename > class ALLOC >
    void CorrectedMutualInformation< ALLOC >::setMinNbRowsPerThread(const std::size_t nb) const {
      _NH_.setMinNbRowsPerThread(nb);
      _k_NML_.setMinNbRowsPerThread(nb);
      _score_MDL_.setMinNbRowsPerThread(nb);
    }


    /// returns the minimum of rows that each thread should process
    template < template < typename > class ALLOC >
    INLINE std::size_t CorrectedMutualInformation< ALLOC >::minNbRowsPerThread() const {
      return _NH_.minNbRowsPerThread();
    }


    /// sets new ranges to perform the countings used by the score
    /** @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
     * indices. The countings are then performed only on the union of the
     * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
     * cross validation tasks, in which part of the database should be ignored.
     * An empty set of ranges is equivalent to an interval [X,Y) ranging over
     * the whole database. */
    template < template < typename > class ALLOC >
    template < template < typename > class XALLOC >
    void CorrectedMutualInformation< ALLOC >::setRanges(
       const std::vector< std::pair< std::size_t, std::size_t >,
                          XALLOC< std::pair< std::size_t, std::size_t > > >& new_ranges) {
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
         old_ranges = ranges();

      _NH_.setRanges(new_ranges);
      _k_NML_.setRanges(new_ranges);
      _score_MDL_.setRanges(new_ranges);

      if (old_ranges != ranges()) clear();
    }


    /// reset the ranges to the one range corresponding to the whole database
    template < template < typename > class ALLOC >
    void CorrectedMutualInformation< ALLOC >::clearRanges() {
      std::vector< std::pair< std::size_t, std::size_t >,
                   ALLOC< std::pair< std::size_t, std::size_t > > >
         old_ranges = ranges();
      _NH_.clearRanges();
      _k_NML_.clearRanges();
      _score_MDL_.clearRanges();
      if (old_ranges != ranges()) clear();
    }


    /// returns the current ranges
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::pair< std::size_t, std::size_t >,
                              ALLOC< std::pair< std::size_t, std::size_t > > >&
                 CorrectedMutualInformation< ALLOC >::ranges() const {
      return _NH_.ranges();
    }


    /// use the MDL penalty function
    template < template < typename > class ALLOC >
    void CorrectedMutualInformation< ALLOC >::useMDL() {
      clearCache();
      _kmode_ = KModeTypes::MDL;
    }


    /// use the kNML penalty function
    template < template < typename > class ALLOC >
    void CorrectedMutualInformation< ALLOC >::useNML() {
      clearCache();
      _kmode_ = KModeTypes::NML;
    }


    /// use no correction/penalty function
    template < template < typename > class ALLOC >
    void CorrectedMutualInformation< ALLOC >::useNoCorr() {
      clearCache();
      _kmode_ = KModeTypes::NoCorr;
    }


    /// returns the 2-point mutual information corresponding to a given nodeset
    template < template < typename > class ALLOC >
    INLINE double CorrectedMutualInformation< ALLOC >::score(NodeId var1, NodeId var2) {
      return score(var1, var2, _empty_conditioning_set_);
    }


    /// returns the 2-point mutual information corresponding to a given nodeset
    template < template < typename > class ALLOC >
    INLINE double CorrectedMutualInformation< ALLOC >::score(
       NodeId                                        var1,
       NodeId                                        var2,
       const std::vector< NodeId, ALLOC< NodeId > >& conditioning_ids) {
      return _NI_score_(var1, var2, conditioning_ids) - _K_score_(var1, var2, conditioning_ids);
    }


    /// returns the 3-point mutual information corresponding to a given nodeset
    template < template < typename > class ALLOC >
    INLINE double
       CorrectedMutualInformation< ALLOC >::score(NodeId var1, NodeId var2, NodeId var3) {
      return score(var1, var2, var3, _empty_conditioning_set_);
    }


    /// returns the 3-point mutual information corresponding to a given nodeset
    template < template < typename > class ALLOC >
    INLINE double CorrectedMutualInformation< ALLOC >::score(
       NodeId                                        var1,
       NodeId                                        var2,
       NodeId                                        var3,
       const std::vector< NodeId, ALLOC< NodeId > >& conditioning_ids) {
      return _NI_score_(var1, var2, var3, conditioning_ids)
           + _K_score_(var1, var2, var3, conditioning_ids);
    }


    /// return N times the mutual information for conditioned pairs of variables
    template < template < typename > class ALLOC >
    double CorrectedMutualInformation< ALLOC >::_NI_score_(
       NodeId                                        var_x,
       NodeId                                        var_y,
       const std::vector< NodeId, ALLOC< NodeId > >& vars_z) {
      /*
       * We have a few partial entropies to compute in order to have the
       * 2-point mutual information:
       * I(x;y) = H(x) + H(y) - H(x,y)
       * correspondingly
       * I(x;y) = Hx + Hy - Hxy
       * or
       * I(x;y|z) = H(x,z) + H(y,z) - H(z) - H(x,y,z)
       * correspondingly
       * I(x;y|z) = Hxz + Hyz - Hz - Hxyz
       * Note that Entropy H is equal to 1/N times the log2Likelihood,
       * where N is the size of the database.
       * Remember that we return N times I(x;y|z)
       */

      // if the score has already been computed, get its value
      const IdCondSet< ALLOC > idset_xyz(var_x, var_y, vars_z, false, false);
      if (_use_ICache_) {
        try {
          return _ICache_.score(idset_xyz);
        } catch (const NotFound&) {}
      }

      // compute the score

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      double score;
      if (!vars_z.empty()) {
        std::vector< NodeId, ALLOC< NodeId > > vars(vars_z);
        // std::sort(vars.begin(), vars.end());
        vars.push_back(var_x);
        vars.push_back(var_y);
        const double NHxyz = -_NH_.score(IdCondSet< ALLOC >(vars, false, true));

        vars.pop_back();
        const double NHxz = -_NH_.score(IdCondSet< ALLOC >(vars, false, true));

        vars.pop_back();
        vars.push_back(var_y);
        const double NHyz = -_NH_.score(IdCondSet< ALLOC >(vars, false, true));

        vars.pop_back();
        const double NHz = -_NH_.score(IdCondSet< ALLOC >(vars, false, true));

        const double NHxz_NHyz = NHxz + NHyz;
        double       NHz_NHxyz = NHz + NHxyz;

        // avoid numeric instability due to rounding errors
        double ratio = 1;
        if (NHxz_NHyz > 0) {
          ratio = (NHxz_NHyz - NHz_NHxyz) / NHxz_NHyz;
        } else if (NHz_NHxyz > 0) {
          ratio = (NHxz_NHyz - NHz_NHxyz) / NHz_NHxyz;
        }
        if (ratio < 0) ratio = -ratio;
        if (ratio < _threshold_) {
          NHz_NHxyz = NHxz_NHyz;   // ensure that the score is equal to 0
        }

        score = NHxz_NHyz - NHz_NHxyz;
      } else {
        const double NHxy
           = -_NH_.score(IdCondSet< ALLOC >(var_x, var_y, _empty_conditioning_set_, true, false));
        const double NHx = -_NH_.score(var_x);
        const double NHy = -_NH_.score(var_y);

        double NHx_NHy = NHx + NHy;

        // avoid numeric instability due to rounding errors
        double ratio = 1;
        if (NHx_NHy > 0) {
          ratio = (NHx_NHy - NHxy) / NHx_NHy;
        } else if (NHxy > 0) {
          ratio = (NHx_NHy - NHxy) / NHxy;
        }
        if (ratio < 0) ratio = -ratio;
        if (ratio < _threshold_) {
          NHx_NHy = NHxy;   // ensure that the score is equal to 0
        }

        score = NHx_NHy - NHxy;
      }


      // shall we put the score into the cache?
      if (_use_ICache_) { _ICache_.insert(idset_xyz, score); }

      return score;
    }


    /// return N times the mutual information for conditioned triples of variables
    template < template < typename > class ALLOC >
    INLINE double CorrectedMutualInformation< ALLOC >::_NI_score_(
       NodeId                                        var_x,
       NodeId                                        var_y,
       NodeId                                        var_z,
       const std::vector< NodeId, ALLOC< NodeId > >& ui_ids) {
      // conditional 3-point mutual information formula:
      // I(x;y;z|{ui}) = I(x;y|{ui}) - I(x;y|z,{ui})
      std::vector< NodeId, ALLOC< NodeId > > uiz_ids = ui_ids;
      uiz_ids.push_back(var_z);
      return _NI_score_(var_x, var_y, ui_ids) - _NI_score_(var_x, var_y, uiz_ids);
    }


    /// 2pt penalty
    template < template < typename > class ALLOC >
    double CorrectedMutualInformation< ALLOC >::_K_score_(
       NodeId                                        var1,
       NodeId                                        var2,
       const std::vector< NodeId, ALLOC< NodeId > >& conditioning_ids) {
      // if no penalty, return 0
      if (_kmode_ == KModeTypes::NoCorr) return 0.0;


      // If using the K cache, verify whether the set isn't already known
      IdCondSet< ALLOC > idset;
      if (_use_KCache_) {
        idset = std::move(IdCondSet< ALLOC >(var1, var2, conditioning_ids, false));
        try {
          return _KCache_.score(idset);
        } catch (const NotFound&) {}
      }

      // compute the score
      double score;
      size_t rx, ry, rui;
      switch (_kmode_) {
        case KModeTypes::MDL: {
          const auto& database  = _NH_.database();
          const auto& node2cols = _NH_.nodeId2Columns();

          rui = 1;
          if (!node2cols.empty()) {
            rx = database.domainSize(node2cols.second(var1));
            ry = database.domainSize(node2cols.second(var2));
            for (const NodeId i: conditioning_ids) {
              rui *= database.domainSize(node2cols.second(i));
            }
          } else {
            rx = database.domainSize(var1);
            ry = database.domainSize(var2);
            for (const NodeId i: conditioning_ids) {
              rui *= database.domainSize(i);
            }
          }

          // compute the size of the database, including the a priori
          if (!_use_KCache_) {
            idset = std::move(IdCondSet< ALLOC >(var1, var2, conditioning_ids, false));
          }
          const double N = _score_MDL_.N(idset);

          score = 0.5 * (rx - 1) * (ry - 1) * rui * std::log2(N);
        } break;

        case KModeTypes::NML:
          score = _k_NML_.score(var1, var2, conditioning_ids);
          break;

        default:
          GUM_ERROR(NotImplementedYet,
                    "CorrectedMutualInformation mode does "
                    "not support yet this correction");
      }

      // shall we put the score into the cache?
      if (_use_KCache_) { _KCache_.insert(idset, score); }
      return score;
    }


    /// 3pt penalty
    template < template < typename > class ALLOC >
    INLINE double CorrectedMutualInformation< ALLOC >::_K_score_(
       NodeId                                        var1,
       NodeId                                        var2,
       NodeId                                        var3,
       const std::vector< NodeId, ALLOC< NodeId > >& ui_ids) {
      // k(x;y;z|ui) = k(x;y|ui,z) - k(x;y|ui)
      std::vector< NodeId, ALLOC< NodeId > > uiz_ids = ui_ids;
      uiz_ids.push_back(var3);
      return _K_score_(var1, var2, uiz_ids) - _K_score_(var1, var2, ui_ids);
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
