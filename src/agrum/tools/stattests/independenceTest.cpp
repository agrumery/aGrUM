/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Implementation of gum::learning::IndependenceTest
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/stattests/independenceTest.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/stattests/independenceTest_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// copy operator
    IndependenceTest& IndependenceTest::operator=(const IndependenceTest& from) {
      if (this != &from) {
        Prior*        new_prior = from.prior_->clone();
        RecordCounter new_counter = from.counter_;
        ScoringCache  new_cache   = from.cache_;

        if (prior_ != nullptr) delete prior_;

        prior_ = new_prior;
        counter_ = std::move(new_counter);
        cache_   = std::move(new_cache);

        use_cache_ = from.use_cache_;
      }
      return *this;
    }


    /// move operator
    IndependenceTest& IndependenceTest::operator=(IndependenceTest&& from) {
      if (this != &from) {
        std::swap(prior_, from.prior_);

        counter_   = std::move(from.counter_);
        cache_     = std::move(from.cache_);
        use_cache_ = from.use_cache_;
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
    void IndependenceTest::setRanges(
       const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges) {
      std::vector< std::pair< std::size_t, std::size_t > > old_ranges = ranges();
      counter_.setRanges(new_ranges);
      if (old_ranges != ranges()) clear();
    }


    /// reset the ranges to the one range corresponding to the whole database
    void IndependenceTest::clearRanges() {
      std::vector< std::pair< std::size_t, std::size_t > > old_ranges = ranges();
      counter_.clearRanges();
      if (old_ranges != ranges()) clear();
    }


    /// returns a counting vector where variables are marginalized from N_xyz
    /** @param node_2_marginalize indicates which node(s) shall be marginalized:
     * - 0 means that X should be marginalized
     * - 1 means that Y should be marginalized
     * - 2 means that Z should be marginalized
     */
    std::vector< double > IndependenceTest::marginalize_(const std::size_t node_2_marginalize,
                                                         const std::size_t X_size,
                                                         const std::size_t Y_size,
                                                         const std::size_t Z_size,
                                                         const std::vector< double >& N_xyz) const {
      // determine the size of the output vector
      std::size_t out_size = Z_size;
      if (node_2_marginalize == std::size_t(0))
        out_size *= Y_size;
      else if (node_2_marginalize == std::size_t(1))
        out_size *= X_size;

      // allocate the output vector
      std::vector< double > res(out_size, 0.0);

      // fill the vector:
      if (node_2_marginalize == std::size_t(0)) {   // marginalize X
        for (std::size_t yz = std::size_t(0), xyz = std::size_t(0); yz < out_size; ++yz) {
          for (std::size_t x = std::size_t(0); x < X_size; ++x, ++xyz) {
            res[yz] += N_xyz[xyz];
          }
        }
      } else if (node_2_marginalize == std::size_t(1)) {   // marginalize Y
        for (std::size_t z = std::size_t(0), xyz = std::size_t(0), beg_xz = std::size_t(0);
             z < Z_size;
             ++z, beg_xz += X_size) {
          for (std::size_t y = std::size_t(0); y < Y_size; ++y) {
            for (std::size_t x = std::size_t(0), xz = beg_xz; x < X_size; ++x, ++xz, ++xyz) {
              res[xz] += N_xyz[xyz];
            }
          }
        }
      } else if (node_2_marginalize == std::size_t(2)) {   // marginalize X and Y
        const std::size_t XY_size = X_size * Y_size;
        for (std::size_t z = std::size_t(0), xyz = std::size_t(0); z < out_size; ++z) {
          for (std::size_t xy = std::size_t(0); xy < XY_size; ++xy, ++xyz) {
            res[z] += N_xyz[xyz];
          }
        }
      } else {
        GUM_ERROR(NotImplementedYet,
                  "_marginalize not implemented for nodeset " << node_2_marginalize);
      }

      return res;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
