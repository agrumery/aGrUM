/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Implementation of gum::learning::CorrectedMutualInformation.
 *
 * @author Quentin FALCAND and Pierre-Henri WUILLEMIN(_at_LIP6).
 */

#include <agrum/BN/learning/correctedMutualInformation.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/BN/learning/correctedMutualInformation_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    CorrectedMutualInformation::CorrectedMutualInformation(
        const DBRowGeneratorParser&                                 parser,
        const Prior&                                                prior,
        const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
        const Bijection< NodeId, std::size_t >&                     nodeId2columns) :
        _NH_(parser, prior, ranges, nodeId2columns), _k_NML_(parser, prior, ranges, nodeId2columns),
        _score_MDL_(parser, prior, ranges, nodeId2columns) {
      GUM_CONSTRUCTOR(CorrectedMutualInformation);
    }

    /// default constructor
    CorrectedMutualInformation::CorrectedMutualInformation(
        const DBRowGeneratorParser&             parser,
        const Prior&                            prior,
        const Bijection< NodeId, std::size_t >& nodeId2columns) :
        _NH_(parser, prior, nodeId2columns), _k_NML_(parser, prior, nodeId2columns),
        _score_MDL_(parser, prior, nodeId2columns) {
      GUM_CONSTRUCTOR(CorrectedMutualInformation);
    }

    /// copy constructor
    CorrectedMutualInformation::CorrectedMutualInformation(const CorrectedMutualInformation& from) :
        _NH_(from._NH_), _k_NML_(from._k_NML_), _score_MDL_(from._score_MDL_),
        _kmode_(from._kmode_), _use_ICache_(from._use_ICache_), _use_HCache_(from._use_HCache_),
        _use_KCache_(from._use_KCache_), _use_CnrCache_(from._use_CnrCache_),
        _ICache_(from._ICache_), _KCache_(from._KCache_) {
      GUM_CONS_CPY(CorrectedMutualInformation);
    }

    /// move constructor
    CorrectedMutualInformation::CorrectedMutualInformation(CorrectedMutualInformation&& from) :
        _NH_(std::move(from._NH_)), _k_NML_(std::move(from._k_NML_)),
        _score_MDL_(std::move(from._score_MDL_)), _kmode_(from._kmode_),
        _use_ICache_(from._use_ICache_), _use_HCache_(from._use_HCache_),
        _use_KCache_(from._use_KCache_), _use_CnrCache_(from._use_CnrCache_),
        _ICache_(std::move(from._ICache_)), _KCache_(std::move(from._KCache_)) {
      GUM_CONS_MOV(CorrectedMutualInformation);
    }

    /// virtual copy constructor
    CorrectedMutualInformation* CorrectedMutualInformation::clone() const {
      return new CorrectedMutualInformation(*this);
    }

    /// destructor
    CorrectedMutualInformation::~CorrectedMutualInformation() {
      // for debugging purposes
      GUM_DESTRUCTOR(CorrectedMutualInformation);
    }

    /// copy operator
    CorrectedMutualInformation&
        CorrectedMutualInformation::operator=(const CorrectedMutualInformation& from) {
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
    CorrectedMutualInformation&
        CorrectedMutualInformation::operator=(CorrectedMutualInformation&& from) {
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

    /// sets new ranges to perform the counts used by the score
    /** @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
     * indices. The counts are then performed only on the union of the
     * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
     * cross validation tasks, in which part of the database should be ignored.
     * An empty set of ranges is equivalent to an interval [X,Y) ranging over
     * the whole database. */
    void CorrectedMutualInformation::setRanges(
        const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges) {
      std::vector< std::pair< std::size_t, std::size_t > > old_ranges = ranges();

      _NH_.setRanges(new_ranges);
      _k_NML_.setRanges(new_ranges);
      _score_MDL_.setRanges(new_ranges);

      if (old_ranges != ranges()) clear();
    }

    /// reset the ranges to the one range corresponding to the whole database
    void CorrectedMutualInformation::clearRanges() {
      std::vector< std::pair< std::size_t, std::size_t > > old_ranges = ranges();
      _NH_.clearRanges();
      _k_NML_.clearRanges();
      _score_MDL_.clearRanges();
      if (old_ranges != ranges()) clear();
    }

    /// return N times the mutual information for conditioned pairs of variables
    double CorrectedMutualInformation::_NI_score_(NodeId                       var_x,
                                                  NodeId                       var_y,
                                                  const std::vector< NodeId >& vars_z) {
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
      const IdCondSet idset_xyz(var_x, var_y, vars_z, false, false);
      if (_use_ICache_)
        if (_ICache_.exists(idset_xyz)) return _ICache_.score(idset_xyz);

      // compute the score

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      double score;
      if (!vars_z.empty()) {
        std::vector< NodeId > vars(vars_z);
        vars.push_back(var_x);
        vars.push_back(var_y);
        const double NHxyz = -_NH_.score(IdCondSet(vars, false, true));

        vars.pop_back();
        const double NHxz = -_NH_.score(IdCondSet(vars, false, true));

        vars.pop_back();
        vars.push_back(var_y);
        const double NHyz = -_NH_.score(IdCondSet(vars, false, true));

        vars.pop_back();
        const double NHz = -_NH_.score(IdCondSet(vars, false, true));

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
            = -_NH_.score(IdCondSet(var_x, var_y, _empty_conditioning_set_, true, false));
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

    /// 2pt penalty
    double CorrectedMutualInformation::_K_score_(NodeId                       var1,
                                                 NodeId                       var2,
                                                 const std::vector< NodeId >& conditioning_ids) {
      // if no penalty, return 0
      if (_kmode_ == KModeTypes::NoCorr) return 0.0;


      // If using the K cache, verify whether the set isn't already known
      IdCondSet idset = IdCondSet(var1, var2, conditioning_ids, false);
      if (_use_KCache_)
        if (_KCache_.exists(idset)) return _KCache_.score(idset);


      // compute the score
      double score;
      size_t rx;
      size_t ry;
      size_t rui;
      switch (_kmode_) {
        case KModeTypes::MDL : {
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
          const double N = _score_MDL_.N(idset);

          score = 0.5 * (rx - 1) * (ry - 1) * rui * std::log2(N);
        } break;

        case KModeTypes::NML : score = _k_NML_.score(var1, var2, conditioning_ids); break;

        default :
          GUM_ERROR(NotImplementedYet,
                    "CorrectedMutualInformation mode does "
                    "not support yet this correction");
      }

      // shall we put the score into the cache?
      if (_use_KCache_) { _KCache_.insert(idset, score); }
      return score;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
