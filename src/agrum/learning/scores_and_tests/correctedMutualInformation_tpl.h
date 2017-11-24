/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/core/timer.h>
namespace gum {

  namespace learning {

    // ##########################################################################
    // Constructors and destructors
    // ##########################################################################
    /// default constructor
    template < typename IdSetAlloc, typename CountAlloc >
    template < typename RowFilter >
    INLINE CorrectedMutualInformation< IdSetAlloc, CountAlloc >::
      CorrectedMutualInformation(const RowFilter&           filter,
                                 const std::vector< Size >& var_modalities)
        : _H(filter, var_modalities)
        , __k_NML(filter, var_modalities)
        , __modalities(var_modalities) {
      //_H.useCache(false);
      GUM_CONSTRUCTOR(CorrectedMutualInformation);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    INLINE
      CorrectedMutualInformation< IdSetAlloc,
                                  CountAlloc >::~CorrectedMutualInformation() {
      // for debugging purposes
      GUM_DESTRUCTOR(CorrectedMutualInformation);
    }

    // ##########################################################################
    // Public member functions
    // ##########################################################################
    /// turn on/off the use of a cache of the previously computed score
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void CorrectedMutualInformation< IdSetAlloc, CountAlloc >::useCache(
      bool on_off) noexcept {
      if (!on_off) clear();
      __use_cache = on_off;
    }

    /// turn on/off the use of a cache of the previously computed Kscore
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void CorrectedMutualInformation< IdSetAlloc, CountAlloc >::useKCache(
      bool on_off) noexcept {
      if (!on_off) clear();
      __use_Kcache = on_off;
    }

    /// clears all the data structures from memory
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void CorrectedMutualInformation< IdSetAlloc, CountAlloc >::clear() {
      this->_H.clear();
      __k_NML.clear();
    }

    /// clears the current cache (clear nodesets as well)
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void
    CorrectedMutualInformation< IdSetAlloc, CountAlloc >::clearCache() {
      clear();
      __cache_2pt.clear();
      __cache_3pt.clear();
      __cache_K2pt.clear();
      __cache_K3pt.clear();
    }

    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::score(
      const std::pair< Idx, Idx >& vars,
      const std::vector< Idx >& conditioning_ids) {
      return this->_score(vars.first, vars.second, conditioning_ids);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::score(
      const std::pair< Idx, Idx >& vars) {
      return this->_score(vars.first, vars.second, __empty_conditioning_set);
    }


    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::score(Idx var1,
                                                                       Idx var2) {
      return this->_score(var1, var2, __empty_conditioning_set);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::score(
      Idx var1, Idx var2, const std::vector< Idx >& conditioning_ids) {
      return this->_score(var1, var2, conditioning_ids);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::score(
      const std::tuple< Idx, Idx, Idx >& vars,
      const std::vector< Idx >& conditioning_ids) {
      return this->_score(std::get< 0 >(vars),
                          std::get< 1 >(vars),
                          std::get< 2 >(vars),
                          conditioning_ids);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::score(
      const std::tuple< Idx, Idx, Idx >& vars) {
      return this->_score(std::get< 0 >(vars),
                          std::get< 1 >(vars),
                          std::get< 2 >(vars),
                          __empty_conditioning_set);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::score(Idx var1,
                                                                       Idx var2,
                                                                       Idx var3) {
      return this->_score(var1, var2, var3, __empty_conditioning_set);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::score(
      Idx var1, Idx var2, Idx var3, const std::vector< Idx >& conditioning_ids) {
      return this->_score(var1, var2, var3, conditioning_ids);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    void CorrectedMutualInformation< IdSetAlloc, CountAlloc >::useMDL() {
      clearCache();
      __kmode = KModeTypes::MDL;
    }

    template < typename IdSetAlloc, typename CountAlloc >
    void CorrectedMutualInformation< IdSetAlloc, CountAlloc >::useNML() {
      clearCache();
      __kmode = KModeTypes::NML;
      // the object used for NML K has its own cache
      this->useKCache(false);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    void CorrectedMutualInformation< IdSetAlloc, CountAlloc >::useNoCorr() {
      clearCache();
      __kmode = KModeTypes::NoCorr;
      // No need for cache as K always =0
      this->useKCache(false);
    }

    template < typename IdSetAlloc, typename CountAlloc >
    const Size CorrectedMutualInformation< IdSetAlloc, CountAlloc >::N() {
      return _H._N();
    }

    // ##########################################################################
    // Protected members _score
    // ##########################################################################

    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::_score(
      Idx var1, Idx var2, const std::vector< Idx >& conditioning_ids) {
      /*std::cout << "    Recherche I de " << var1 << var2 << conditioning_ids <<
       * std::endl;*/
      double I = this->_I_score(var1, var2, conditioning_ids);
      /*std::cout << "    I " << I << std::endl;*/
      double K = _K(var1, var2, conditioning_ids);

      Size N = this->N();
      /*std::cout << "    I " << I << " K " << K << " N " << N << std::endl;*/

      double score = I - K / N;
      /*std::cout << "    " << score << std::endl;*/
      return score;
    }

    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::_score(
      Idx var1, Idx var2, Idx var3, const std::vector< Idx >& conditioning_ids) {
      // Timer time;
      // double tps;
      double I = this->_I_score(var1, var2, var3, conditioning_ids);
      // tps = time.step();
      // std::cout << "          I3pt I: " << tps << "s" << std::endl;
      // time.reset();
      double K = _K(var1, var2, var3, conditioning_ids);
      // tps = time.step();
      // std::cout << "          I3pt K: " << tps << "s" << std::endl;

      Size N = this->N();

      double score = I + K / N;

      return score;
    }

    // ##########################################################################
    // Protected members _I_score and _K
    // ##########################################################################

    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::_I_score(
      Idx var1, Idx var2, const std::vector< Idx >& conditioning_ids) {
      /*
       * We have a few partial entropies to compute in order to have the
       * 2-points mutual information :
       * I(x;y) = H(x) + H(y) - H(x,y)
       * correspondingly
       * I(x;y) = Hx + Hy - Hxy
       * or
       * I(x;y|z) = H(x,z) + H(y,z) - H(z) - H(x,y,z)
       * correspondingly
       * I(x;y|z) = Hxz + Hyz - Hz - Hxyz
       */
      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      double score = 0.0;
      if (!conditioning_ids.empty()) {
        // if the score has already been computed, get its value

        // If using cache, verify if the set isn't already known
        if (__use_cache) {
          try {
            double score = __cache_2pt.score(var1, var2, conditioning_ids);
            return score;
          } catch (const NotFound&) {
          }
        }

        std::vector< Idx > vec_xz, vec_yz, vec_xyz;
        vec_xz = conditioning_ids;
        vec_xz.push_back(var1);
        vec_yz = conditioning_ids;
        vec_yz.push_back(var2);
        vec_xyz = vec_yz;
        vec_xyz.push_back(var1);

        _H.clear();

        Idx id_z, id_yz, id_xz, id_xyz;
        id_z = _H.addNodeSet(conditioning_ids);
        id_xz = _H.addNodeSet(vec_xz);
        id_yz = _H.addNodeSet(vec_yz);
        id_xyz = _H.addNodeSet(vec_xyz);

        double Hz = _H.score(id_z);
        double Hxz = _H.score(id_xz);
        double Hyz = _H.score(id_yz);
        double Hxyz = _H.score(id_xyz);

        score = Hxz + Hyz - Hz - Hxyz;

        // shall we put the score into the cache?
        if (this->_isUsingCache()) {
          this->_insertIntoCache(var1, var2, conditioning_ids, score);
        }
        return score;


      } else {
        // if the score has already been computed, get its value

        // If using cache, verify if the set isn't already known
        if (__use_cache) {
          try {
            double score = __cache_2pt.score(var1, var2, __empty_conditioning_set);
            return score;
          } catch (const NotFound&) {
          }
        }

        _H.clear();

        Idx id_x, id_y, id_xy;
        id_x = _H.addNodeSet(var1);
        id_y = _H.addNodeSet(var2);
        id_xy = _H.addNodeSet(var1, var2);

        double Hx = _H.score(id_x);
        double Hy = _H.score(id_y);
        double Hxy = _H.score(id_xy);

        score = Hx + Hy - Hxy;

        // shall we put the score into the cache?
        if (this->_isUsingCache()) {
          this->_insertIntoCache(var1, var2, __empty_conditioning_set, score);
        }
        return score;
      }
    }
    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::_K(
      Idx var1, Idx var2, const std::vector< Idx >& conditioning_ids) {
      // If using cache, verify if the set isn't already known
      if (this->__use_Kcache) {
        try {
          double score = __cache_K2pt.score(var1, var2, conditioning_ids);
          return score;
        } catch (const NotFound&) {
        }
      }
      double score;
      Size   rx, ry, rui, N;
      switch (__kmode) {

        case KModeTypes::MDL:
          rx = __modalities[var1];
          ry = __modalities[var2];
          rui = 1;
          for (Idx i : conditioning_ids) {
            rui *= __modalities[i];
          }
          N = this->N();
          score = 0.5 * (rx - 1) * (ry - 1) * rui * std::log(N);
          break;

        case KModeTypes::NML:
          __k_NML.clear();
          score = __k_NML.score(__k_NML.addNodeSet(var1, var2, conditioning_ids));
          break;

        case KModeTypes::NoCorr:
          score = 0.0;
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "CorrectedMutualInformation mode does "
                    "not support yet this correction");
      }
      // shall we put the score into the cache?
      if (this->__use_Kcache) {
        this->_insertIntoKCache(var1, var2, conditioning_ids, score);
      }
      /*std::cout << "K de " << var1 << var2 << conditioning_ids << " vaut " <<
       * score << std::endl;*/
      return score;
    }

    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::_I_score(
      Idx var1, Idx var2, Idx var3, const std::vector< Idx >& conditioning_ids) {
      // conditional 3-point mutual information formula :
      // I(x;y;z|{ui}) = H(x,y,z,{ui}) - H(x, y, {ui}) - H(x, z, {ui})
      //                 - H(y, z, {ui}) + H(x, {ui}) + H(y, {ui})
      //                 + H(z, {ui}) - H({ui})

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      double score = 0.0;
      if (!conditioning_ids.empty()) {

        // if the score has already been computed, get its value

        // If using cache, verify if the set isn't already known

        if (__use_cache) {
          try {
            double score = __cache_3pt.score(var1, var2, var3, conditioning_ids);
            return score;
          } catch (const NotFound&) {
          }
        }

        std::vector< Idx > vec_xyui, vec_xzui, vec_zyui, vec_xyzui;
        std::vector< Idx > vec_xui, vec_zui, vec_yui;
        vec_xui = conditioning_ids;
        vec_xui.push_back(var1);

        vec_yui = conditioning_ids;
        vec_yui.push_back(var2);

        vec_zui = conditioning_ids;
        vec_zui.push_back(var3);

        vec_xyui = vec_yui;
        vec_xyui.push_back(var1);

        vec_xzui = vec_zui;
        vec_xzui.push_back(var1);

        vec_zyui = vec_yui;
        vec_zyui.push_back(var3);

        vec_xyzui = vec_zyui;
        vec_xyzui.push_back(var1);

        _H.clear();

        Idx id_ui, id_xui, id_yui, id_zui, id_xyui, id_xzui, id_zyui, id_xyzui;
        id_ui = _H.addNodeSet(conditioning_ids);
        id_xui = _H.addNodeSet(vec_xui);
        id_yui = _H.addNodeSet(vec_yui);
        id_zui = _H.addNodeSet(vec_zui);
        id_xyui = _H.addNodeSet(vec_xyui);
        id_xzui = _H.addNodeSet(vec_xzui);
        id_zyui = _H.addNodeSet(vec_zyui);
        id_xyzui = _H.addNodeSet(vec_xyzui);

        double Hui = _H.score(id_ui);
        double Hxui = _H.score(id_xui);
        double Hyui = _H.score(id_yui);
        double Hzui = _H.score(id_zui);
        double Hxyui = _H.score(id_xyui);
        double Hxzui = _H.score(id_xzui);
        double Hzyui = _H.score(id_zyui);
        double Hxyzui = _H.score(id_xyzui);

        score = Hxyzui - Hxyui - Hxzui - Hzyui + Hxui + Hyui + Hzui - Hui;

        // shall we put the score into the cache?
        if (this->_isUsingCache()) {
          this->_insertIntoCache(var1, var2, var3, conditioning_ids, score);
        }

        return score;

      } else {
        // if the score has already been computed, get its value

        // If using cache, verify if the set isn't already known

        if (__use_cache) {
          try {
            double score =
              __cache_3pt.score(var1, var2, var3, __empty_conditioning_set);
            return score;
          } catch (const NotFound&) {
          }
        }

        _H.clear();
        // 3-point mutual information formula :
        // I(x;y;z) = H(x,y,z) - H(x, y) - H(x, z) - H(y, z) + H(x) + H(y) + H(z)
        Idx id_x, id_y, id_z, id_xy, id_xz, id_zy, id_xyz;
        id_x = _H.addNodeSet(var1);
        id_y = _H.addNodeSet(var2);
        id_z = _H.addNodeSet(var3);
        id_xy = _H.addNodeSet(var1, var2);
        id_xz = _H.addNodeSet(var1, var3);
        id_zy = _H.addNodeSet(var3, var2);
        id_xyz = _H.addNodeSet(std::vector< Idx >{var1, var2, var3});

        double Hx = _H.score(id_x);
        double Hy = _H.score(id_y);
        double Hz = _H.score(id_z);
        double Hxy = _H.score(id_xy);
        double Hxz = _H.score(id_xz);
        double Hzy = _H.score(id_zy);
        double Hxyz = _H.score(id_xyz);

        score = Hx + Hy + Hz - Hxy - Hxz - Hzy + Hxyz;

        // shall we put the score into the cache?

        if (this->_isUsingCache()) {
          this->_insertIntoCache(
            var1, var2, var3, __empty_conditioning_set, score);
        }
        return score;
      }
    }


    template < typename IdSetAlloc, typename CountAlloc >
    double CorrectedMutualInformation< IdSetAlloc, CountAlloc >::_K(
      Idx var1, Idx var2, Idx var3, const std::vector< Idx >& conditioning_ids) {
      // If using cache, verify if the set isn't already known
      if (this->__use_Kcache) {
        try {
          double score = __cache_K3pt.score(var1, var2, var3, conditioning_ids);
          return score;
        } catch (const NotFound&) {
        }
      }
      // k(x;y;z|ui) = k(x;y|ui,z) - k(x;y|ui)
      std::vector< Idx > uiz = conditioning_ids;
      uiz.push_back(var3);
      double score = _K(var1, var2, uiz) - _K(var1, var2, conditioning_ids);

      // shall we put the score into the cache?
      if (this->__use_Kcache) {
        this->_insertIntoKCache(var1, var2, var3, conditioning_ids, score);
      }

      return score;
    }

    // ##########################################################################
    // Protected members utils
    // ##########################################################################

    /// inserts a new score into the cache
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void
    CorrectedMutualInformation< IdSetAlloc, CountAlloc >::_insertIntoCache(
      Idx                       var1,
      Idx                       var2,
      const std::vector< Idx >& conditioning_ids,
      double                    score) {
      /*
    const std::vector<Idx, IdSetAlloc>& all_nodes =
        _getAllNodes( nodeset_index );
    std::vector<Idx, IdSetAlloc> conditioning_nodes =
        *( _getConditioningNodes( nodeset_index ) );
    conditioning_nodes.pop_back();*/

      if (!conditioning_ids.empty()) {
        try {
          __cache_2pt.insert(var1, var2, conditioning_ids, score);
        } catch (const gum::DuplicateElement&) {
        }
      } else {
        try {
          __cache_2pt.insert(var1, var2, __empty_conditioning_set, score);
        } catch (const gum::DuplicateElement&) {
        }
      }
    }

    /// inserts a new score into the cache
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void
    CorrectedMutualInformation< IdSetAlloc, CountAlloc >::_insertIntoCache(
      Idx                       var1,
      Idx                       var2,
      Idx                       var3,
      const std::vector< Idx >& conditioning_ids,
      double                    score) {
      /*
    const std::vector<Idx, IdSetAlloc>& all_nodes =
        _getAllNodes( nodeset_index );
    std::vector<Idx, IdSetAlloc> conditioning_nodes =
        *( _getConditioningNodes( nodeset_index ) );
    conditioning_nodes.pop_back();*/

      if (!conditioning_ids.empty()) {
        try {
          __cache_3pt.insert(var1, var2, var3, conditioning_ids, score);
        } catch (const gum::DuplicateElement&) {
        }
      } else {
        try {
          __cache_3pt.insert(var1, var2, var3, __empty_conditioning_set, score);
        } catch (const gum::DuplicateElement&) {
        }
      }
    }

    /// inserts a new score into the cache
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void
    CorrectedMutualInformation< IdSetAlloc, CountAlloc >::_insertIntoKCache(
      Idx                       var1,
      Idx                       var2,
      const std::vector< Idx >& conditioning_ids,
      double                    score) {
      if (!conditioning_ids.empty()) {
        try {
          __cache_K2pt.insert(var1, var2, conditioning_ids, score);
        } catch (const gum::DuplicateElement&) {
        }
      } else {
        try {
          __cache_K2pt.insert(var1, var2, this->__empty_conditioning_set, score);
        } catch (const gum::DuplicateElement&) {
        }
      }
    }

    /// inserts a new score into the cache
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE void
    CorrectedMutualInformation< IdSetAlloc, CountAlloc >::_insertIntoKCache(
      Idx                       var1,
      Idx                       var2,
      Idx                       var3,
      const std::vector< Idx >& conditioning_ids,
      double                    score) {
      if (!conditioning_ids.empty()) {
        try {
          __cache_K3pt.insert(var1, var2, var3, conditioning_ids, score);
        } catch (const gum::DuplicateElement&) {
        }
      } else {
        try {
          __cache_K3pt.insert(
            var1, var2, var3, this->__empty_conditioning_set, score);
        } catch (const gum::DuplicateElement&) {
        }
      }
    }

    /// indicates whether we use the cache or not
    template < typename IdSetAlloc, typename CountAlloc >
    INLINE bool
    CorrectedMutualInformation< IdSetAlloc, CountAlloc >::_isUsingCache() const
      noexcept {
      return __use_cache;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
