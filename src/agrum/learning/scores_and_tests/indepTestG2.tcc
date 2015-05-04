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
/** @file
 * @brief the class for computing G2 scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter>
    INLINE IndepTestG2<IdSetAlloc, CountAlloc>::IndepTestG2(
        const RowFilter &filter, const std::vector<unsigned int> &var_modalities)
        : IndependenceTest<IdSetAlloc, CountAlloc>(filter, var_modalities),
          __chi2(var_modalities) {
      // for debugging purposes
      GUM_CONSTRUCTOR(IndepTestG2);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE IndepTestG2<IdSetAlloc, CountAlloc>::~IndepTestG2() {
      // for debugging purposes
      GUM_DESTRUCTOR(IndepTestG2);
    }

    /// returns the score corresponding to a given nodeset
    template <typename IdSetAlloc, typename CountAlloc>
    float IndepTestG2<IdSetAlloc, CountAlloc>::score(unsigned int nodeset_index) {
      // if the score has already been computed, get its value
      if (this->_isInCache(nodeset_index)) {
        return this->_cachedScore(nodeset_index);
      }

      // get the nodes involved in the score as well as their modalities
      const std::vector<unsigned int, IdSetAlloc> &all_nodes =
          this->_getAllNodes(nodeset_index);
      const std::vector<unsigned int, IdSetAlloc> *conditioning_nodes =
          this->_getConditioningNodes(nodeset_index + 1);
      const std::vector<unsigned int> &modals = this->modalities();

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if (conditioning_nodes != nullptr) {
        // indicate to the chi2 distribution the set of conditioning nodes
        __chi2.setConditioningNodes(*conditioning_nodes);

        // now, perform :
        // sum_X sum_Y sum_Z #XYZ * log ( ( #XYZ * #Z ) / ( #XZ * #YZ ) )

        // get the counts for all the targets and for the conditioning nodes
        const std::vector<float, CountAlloc> &Nzyx =
            this->_getAllCounts(nodeset_index);
        const std::vector<float, CountAlloc> &Nzy =
            this->_getConditioningCounts(nodeset_index);
        const std::vector<float, CountAlloc> &Nzx =
            this->_getAllCounts(nodeset_index + 1);
        const std::vector<float, CountAlloc> &Nz =
            this->_getConditioningCounts(nodeset_index + 1);

        const unsigned int Z_size = Nz.size();
        const unsigned int Y_size = modals[all_nodes[all_nodes.size() - 2]];
        const unsigned int X_size = modals[all_nodes[all_nodes.size() - 1]];

        float score = 0;

        for (unsigned int x = 0, beg_zx = 0, zyx = 0; x < X_size;
             ++x, beg_zx += Z_size) {
          for (unsigned int y = 0, zy = 0, zx = beg_zx; y < Y_size;
               ++y, zx = beg_zx) {
            for (unsigned int z = 0; z < Z_size; ++z, ++zy, ++zx, ++zyx) {
              const float tmp1 = Nzyx[zyx] * Nz[z];
              if (tmp1) {
                const float tmp2 = Nzy[zy] * Nzx[zx];
                if (tmp2) {
                  score += Nzyx[zyx] * logf(tmp1 / tmp2);
                }
              }
            }
          }
        }

        // ok, here, score contains the value of the chi2 formula.
        // To get a meaningful score, we shall compute the critical values
        // for the Chi2 distribution and assign as the score of
        // (score - alpha ) / alpha, where alpha is the critical value
        const float alpha = __chi2.criticalValue(all_nodes[all_nodes.size() - 1],
                                                 all_nodes[all_nodes.size() - 2]);
        score = (score - alpha) / alpha;

        // shall we put the score into the cache?
        if (this->_isUsingCache()) {
          this->_insertIntoCache(nodeset_index, score);
        }

        return score;
      } else {
        // here, there is no conditioning set

        // indicate to the chi2 distribution the set of conditioning nodes
        __chi2.setConditioningNodes(__empty_set);

        // now, perform sum_X sum_Y #XY * log ( ( #XY * N ) / ( #X * #Y ) )

        // get the counts for all the targets and for the conditioning nodes
        const std::vector<float, CountAlloc> &Nyx =
            this->_getAllCounts(nodeset_index);
        const std::vector<float, CountAlloc> &Ny =
            this->_getConditioningCounts(nodeset_index);
        const std::vector<float, CountAlloc> &Nx =
            this->_getAllCounts(nodeset_index + 1);

        const unsigned int Y_size = Ny.size();
        const unsigned int X_size = Nx.size();

        // count N
        float N = 0;
        for (unsigned int i = 0; i < Nx.size(); ++i) {
          N += Nx[i];
        }

        float score = 0;

        for (unsigned int x = 0, yx = 0; x < X_size; ++x) {
          const float tmp_Nx = Nx[x];
          for (unsigned int y = 0; y < Y_size; ++y, ++yx) {
            const float tmp = tmp_Nx * Ny[y];
            if (tmp) {
              score += Nyx[yx] * logf((Nyx[yx] * N) / tmp);
            }
          }
        }

        // ok, here, score contains the value of the chi2 formula.
        // To get a meaningful score, we shall compute the critical values
        // for the Chi2 distribution and assign as the score of
        // (score - alpha ) / alpha, where alpha is the critical value
        const float alpha = __chi2.criticalValue(all_nodes[all_nodes.size() - 1],
                                                 all_nodes[all_nodes.size() - 2]);
        score = (score - alpha) / alpha;

        // shall we put the score into the cache?
        if (this->_isUsingCache()) {
          this->_insertIntoCache(nodeset_index, score);
        }

        return score;
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
