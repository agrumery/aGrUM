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
 * @brief Lrs wrapper
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 * easy use of lrs lib
 */

#ifndef __LRSWrapper_WRAPPER__H__
#define __LRSWrapper_WRAPPER__H__

#include <agrum/agrum.h>


#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#else
#  include <agrum/tools/core/mvsc/unistd.h>
#endif

#include <chrono>
#include <cstdio>
#include <fcntl.h>
#include <fstream>
#include <unordered_set>
#include <vector>

#include <agrum/tools/core/math/rational.h>

// we force MP (not long or GMP)
#undef LONG
#undef LRSLONG
#undef GMP
#define MP
// lrs stuff
extern "C" {
#include <agrum/tools/external/lrslib/lrslib.h>
}
/* *** from lrs, we need to know BASE to read multiple precision integers *** */
#ifdef B32
/*32 bit machines */
#  define FORMAT   "%4.4lu"
#  define MAXD     2147483647L
#  define BASE     10000L
#  define BASE_DIG 4
#  define INTSIZE  8L
#  define BIT      "32bit"
#else
/* 64 bit machines */
#  define MAXD     9223372036854775807L
#  define BASE     1000000000L
#  define FORMAT   "%9.9lu"
#  define BASE_DIG 9
#  define INTSIZE  16L
#  define BIT      "64bit"
#endif

// 64 bits for windows (long is 32 bits)
#ifdef _MSC_VER
typedef __int64          int64_t;
typedef unsigned __int64 uint64_t;
#else
#  include <stdint.h>
#endif

/* ************ */

#define enumStringify(name) #name

namespace gum {
  namespace credal {

    /**
     * @class LRSWrapper
     * @headerfile LrsWrapper.h <agrum/CN/LrsWrapper.h>
     * @brief Class template acting as a wrapper for Lexicographic Reverse
     * Search by
     * David Avis.
     * @ingroup cn_group
     * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
     * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
     */
    template < typename GUM_SCALAR >
    class LRSWrapper {
      private:
      /** @brief Shortcut for dynamic matrix using vectors. */
      using matrix = typename std::vector< std::vector< GUM_SCALAR > >;

      /** @brief Input matrix - either a V-representation or an
       * H-representation. */
      matrix _input_;

      /** @brief Output matrix - either a V-representation or an
       * H-representation. */
      matrix _output_;

      /** @brief Cardinality of the variable. */
      unsigned int _card_;

      /** @brief To keep track of which constraints over modalities have been
       * inserted. When the set is full, the state changes from up to ready. */
      std::unordered_set< int > _insertedModals_;

      /** @brief The number of vertices of the polytope. */
      unsigned int _vertices_;

      /** @brief To keep track of inserted vertices and total. When set is full,
       * the
       * state changes from up to ready. */
      std::vector< std::vector< GUM_SCALAR > > _insertedVertices_;

      /** @brief In case we have lower = upper for all modalities, a point
       * probability, there is no need to use lrs. */
      std::vector< GUM_SCALAR > _vertex_;

      /** @enum  _states_ The possible states of the LrsWrapper. Some functions
       * will
       * throw an exception if the state is not correct. It allows the user to
       * avoid
       * making - invisible - mistakes. */
      enum class _states_ : char {
        none     = char(0),
        Hup      = char(1),
        Vup      = char(2),
        H2Vready = char(3),
        V2Hready = char(4),
      };

      /** @brief The current state of the LrsWrapper. */
      _states_ _state_;

      /** @brief The volume of the polytope, if computed, 0 otherwise. */
      GUM_SCALAR _volume_;

      /** @brief To print an enum field name instead of it's value. Used with
       * GUM_ERROR. */
      const char* _setUpStateNames_[5] = {
         enumStringify(_states_::none),
         enumStringify(_states_::nHup),
         enumStringify(_states_::nVup),
         enumStringify(_states_::nH2Vready),
         enumStringify(_states_::nV2Hready),
      };

      /**
       * @brief File descriptor of standard cout.
       *
       * Lrs writes a lot of stuff on standard cout.
       *  _oldCout_ is used to save the current cout before redirecting it
       * to /dev/null when calling lrs.
       * The standard cout is restored when lrs is done.
       */
      mutable int _oldCout_;

      /// @name lrs structs
      /// @{

      /** @brief Structure for holding current dictionary and indices of lrs. */
      lrs_dic* _dic_;

      /** @brief Structure for holding static problem data of lrs.*/
      lrs_dat* _dat_;

      /** @brief One line of output of lrs : aither a ray, a vertex, a facet or
       * a
       * linearity. */
      lrs_mp_vector _lrsOutput_;

      /** @brief Holds lrs input linearities if any are found. */
      lrs_mp_matrix _Lin_;

      /// @}

      /// @name flags
      /// @{

      bool _getVolume_;

      bool _hull_;

      bool _polytope_;

      /// @}

      /// @name cout redirection
      /// @{

      /** @brief The function that redirects standard cout to /dev/null. */
      void _coutOff_() const;

      /** @brief The function that restores standard cout. */
      void _coutOn_() const;

      /// @}

      /// @name lrs datas <-> wrapper datas
      /// @{

      /** @brief Free lrs space. */
      void _freeLrs_();

      /** @brief Initialize lrs structs and first basis according to flags. */
      void _initLrs_();

      /**
       * @brief Fill lrs_dictionnary and datas from \c  _input_ using integer
       *rationals.
       *
       * Build polyhedron constraints and objective.
       * Rational< GUM_SCALAR >::continuedFrac is the default algorithm used to
       *approximate reals by integer rationals.
       */
      void _fill_() const;

      /**
       * @brief Translate a single output from lrs.
       *
       * Only vertices are supposed to be read at this step.
       *
       * @param Nin Input numerators in mp format (returned by lrs).
       * @param Din Input denominators in mp format (returned by lrs).
       * @param Num Output integer numerators.
       * @param Den Output integer denominators.
       */
      void _getLRSWrapperOutput_(lrs_mp                  Nin,
                                 lrs_mp                  Din,
                                 std::vector< int64_t >& Num,
                                 std::vector< int64_t >& Den) const;

      /// @}

      public:
      /// @name Constructors / Destructors
      /// @{

      /**
       * Default Constructor.
       */
      LRSWrapper();

      /**
       * Default Destructor.
       */
      ~LRSWrapper();

      /// @}

      /// @name Getters and setters
      /// @{

      /**
       * @brief Get the intput matrix of the problem.
       * @return A constant reference to the \c  _intput_ matrix.
       */
      const matrix& getInput() const;

      /**
       * @brief Get the output matrix solution of the problem.
       * @return A constant reference to the \c  _output_ matrix.
       */
      const matrix& getOutput() const;

      /**
       * @brief Get the number of vertices of this polytope.
       * @return A constant reference to the number of vertices \c  _vertices_.
       */
      const unsigned int& getVerticesNumber() const;

      /**
       * @brief Get the volume of the polytope that has been computed.
       *
       * @warning Volume can only be computed using V-representation.
       *H-representation volume computation is not predictable.
       *
       * @warning Probabilistic polytopes are not full dimensional : they lie in
       *the
       *simplex' hyper plane,
       * therefor a pseudo-volume will be computed by projecting the polytope.
       *The
       *projection used is the
       * lexicographically smallest coordinate subspace.
       *
       * @return A constant reference to the polytope volume.
       */
      const GUM_SCALAR& getVolume() const;

      /// @}

      /// @name setUp / tearDown
      /// @{

      /**
       * @brief %Sets up an H-representation.
       *
       * Initialize input matrix \c  _input_ to correct dimensions and wrapper
       *state
       *\c  _state_ to \c  _states_::Hup.
       * @param card A constant reference to the cardinality of the variable.
       */
      void setUpH(const Size& card);

      /**
       * @brief %Sets up a V-representation.
       *
       * Initialize input matrix \c  _input_ to correct dimensions and wrapper
       *state
       *\c  _state_ to \c  _states_::Vup.
       * @param card A constant reference to the cardinality of the variable.
       * @param vertices A constant reference to the number of vertices of the
       *polytope.
       */
      void setUpV(const Size& card, const Size& vertices);

      /**
       * @brief Reset the wrapper as if it was built.
       *
       * Reset wrapper state \c  _state_ to \c  _states_::none and clear all
       *member
       *datas.
       */
      void tearDown();

      /**
       * @brief Reset the wrapper for next computation for a H-representation
       *with
       *the same
       * variable cardinality and number of inequalities. Usefull when creating
       *credal networks
       * specified as intervals over modalities.
       *
       * Reset wrapper state \c  _state_ to it's previous state and clear output
       *matrix \c  _output_.
       * Keeps the cardinality \c  _card_ of the variable and therefor the input
       *matrix \c  _intput_ structure.
       */
      void nextHInput();

      /// @}

      /// @name Input filling methods
      /// @{

      /**
       * @brief Creates the H-representation of min <= p(X=modal | .) <= max and
       *add
       *it to the problem input \c  _input_.
       *
       * @param min The lower value of p(X=modal | .).
       * @param max The upper value of p(X=modal | .).
       * @param modal The modality on which we put constraints.
       */
      void fillH(const GUM_SCALAR& min, const GUM_SCALAR& max, const Size& modal);

      /**
       * @brief Fill the H-representation from the matrix given in argument.
       *
       * @param matrix The H-representation of the polytope of the form 0 <= -b
       *+ Ax,
       *A is the matrix, each column the coefficient of the variable in x.
       */
      void fillMatrix(const std::vector< std::vector< GUM_SCALAR > >& matrix);

      /**
       * @brief Creates the V-representation of a polytope by adding a vertex to
       *the
       *problem input \c  _input_.
       *
       * @param vertex The vertex we wish to add to the V-representation of the
       *polytope.
       */
      void fillV(const std::vector< GUM_SCALAR >& vertex);

      /// @}

      /// @name lrs algorithms
      /// @{

      /**
       * @brief H-representation to V-representation.
       *
       * Computes the V-representation of a polytope, i.e. it's vertices, from
       *it's
       *H-representation, i.e. the hyper-plan inequalities.
       */
      void H2V();

      /**
       * @brief V-representation to H-representation.
       *
       * @warning Not complete yet.
       *
       * Computes the H-representation of a polytope from it's V-representation.
       */
      void V2H();

      /**
       * @brief Computes a polytope ( pseudo ) volume from it's
       *V-representation.
       *
       * @warning Volume can only be computed using V-representation.
       *H-representation volume computation is not predictable.
       *
       * @warning Probabilistic polytopes are not full dimensional : they lie in
       *the
       *simplex' hyper plane,
       * therefor a pseudo-volume will be computed by projecting the polytope.
       *The
       *projection used is the
       * lexicographically smallest coordinate subspace.
       */
      void computeVolume();

      /**
       * @brief V-Redundancy elimination.
       *
       * Eliminates redundant vertices from a polytope V-representation input \c
       * _input_.
       */
      void elimRedundVrep();

      /**
       * H-redundancy elimination.
       */

      /// @}
    };

  }   // namespace credal
}   // namespace gum

#include <agrum/CN/polytope/LrsWrapper_tpl.h>

#endif
