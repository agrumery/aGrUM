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
 * @brief Lrs wrapper
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 *
 * easy use of lrs lib
 */

#ifndef __LRSWrapper_WRAPPER__H__
#define __LRSWrapper_WRAPPER__H__

#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <chrono>

#include <agrum/core/exceptions.h>
#include <agrum/core/math/rational.h>

// we force MP (not long or GMP)
#undef LONG
#undef GMP
#define MP
// lrs stuff
#include <agrum/core/math/lrslib/lrslib.h>

/* *** from lrs, we need to know BASE to read multiple precision integers *** */
#ifndef B64
/*32 bit machines */
#define FORMAT "%4.4lu"
#define MAXD 2147483647L
#define BASE 10000L
#define BASE_DIG 4
#define INTSIZE 8L
#define BIT "32bit"
#else
/* 64 bit machines */
#define MAXD 9223372036854775807L
#define BASE 1000000000L
#define FORMAT "%9.9lu"
#define BASE_DIG 9
#define INTSIZE 16L
#define BIT "64bit"
#endif
/* ************ */

#define enumStringify(name) #name

namespace gum {
  namespace credal {

    /**
     * @class LRSWrapper LrsWrapper.h <agrum/CN/LrsWrapper.h>
     * @brief Class template acting as a wrapper for Lexicographic Reverse Search by
     * David Avis.
     * @ingroup cn_group
     * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
     * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
     */
    template <typename GUM_SCALAR> class LRSWrapper {
      private:
      /** @brief Shortcut for dynamic matrix using vectors. */
      typedef typename std::vector<std::vector<GUM_SCALAR>> matrix;

      /** @brief Input matrix - either a V-representation or an H-representation. */
      matrix __input;

      /** @brief Output matrix - either a V-representation or an H-representation. */
      matrix __output;

      /** @brief Cardinality of the variable. */
      unsigned int __card;

      /** @brief To keep track of which constraints over modalities have been
       * inserted. When the set is full, the state changes from up to ready. */
      std::unordered_set<int> __insertedModals;

      /** @brief The number of vertices of the polytope. */
      unsigned int __vertices;

      /** @brief To keep track of inserted vertices and total. When set is full, the
       * state changes from up to ready. */
      std::vector<std::vector<GUM_SCALAR>> __insertedVertices;

      /** @brief In case we have lower = upper for all modalities, a point
       * probability, there is no need to use lrs. */
      std::vector<GUM_SCALAR> __vertex;

      /** @enum __states The possible states of the LrsWrapper. Some functions will
       * throw an exception if the state is not correct. It allows the user to avoid
       * making - invisible - mistakes. */
      enum class __states : char {
        none = char(0),
        Hup = char(1),
        Vup = char(2),
        H2Vready = char(3),
        V2Hready = char(4),
      };

      /** @brief The current state of the LrsWrapper. */
      __states __state;

      /** @brief The volume of the polytope, if computed, 0 otherwise. */
      GUM_SCALAR __volume;

      /** @brief To print an enum field name instead of it's value. Used with
       * GUM_ERROR. */
      const char *__setUpStateNames[5] = {
          enumStringify(__states::none),      enumStringify(__states::nHup),
          enumStringify(__states::nVup),      enumStringify(__states::nH2Vready),
          enumStringify(__states::nV2Hready),
      };

      /**
       * @brief File descriptor of standard cout.
       *
       * Lrs writes a lot of stuff on standard cout.
       * __oldCout is used to save the current cout before redirecting it
       * to /dev/null when calling lrs.
       * The standard cout is restored when lrs is done.
       */
      mutable int __oldCout;

      /// @name lrs structs
      /// @{

      /** @brief Structure for holding current dictionary and indices of lrs. */
      lrs_dic *__dic;

      /** @brief Structure for holding static problem data of lrs.*/
      lrs_dat *__dat;

      /** @brief One line of output of lrs : aither a ray, a vertex, a facet or a
       * linearity. */
      lrs_mp_vector __lrsOutput;

      /** @brief Holds lrs input linearities if any are found. */
      lrs_mp_matrix __Lin;

      /// @}

      /// @name flags
      /// @{

      bool __getVolume;

      bool __hull;

      bool __polytope;

      /// @}

      /// @name cout redirection
      /// @{

      /** @brief The function that redirects standard cout to /dev/null. */
      void __coutOff() const;

      /** @brief The function that restores standard cout. */
      void __coutOn() const;

      /// @}

      /// @name lrs datas <-> wrapper datas
      /// @{

      /** @brief Free lrs space. */
      void __freeLrs();

      /** @brief Initialize lrs structs and first basis according to flags. */
      void __initLrs();

      /**
       * @brief Fill lrs_dictionnary and datas from \c __input using integer
       *rationals.
       *
       * Build polyhedron constraints and objective.
       * Rational< GUM_SCALAR >::continuedFrac is the default algorithm used to
       *approximate reals by integer rationals.
       */
      void __fill() const;

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
      void __getLRSWrapperOutput(lrs_mp Nin, lrs_mp Din, std::vector<long int> &Num,
                                 std::vector<long int> &Den) const;

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
       * @return A constant reference to the \c __intput matrix.
       */
      const matrix &getInput() const;

      /**
       * @brief Get the output matrix solution of the problem.
       * @return A constant reference to the \c __output matrix.
       */
      const matrix &getOutput() const;

      /**
       * @brief Get the number of vertices of this polytope.
       * @return A constant reference to the number of vertices \c __vertices.
       */
      const unsigned int &getVerticesNumber() const;

      /**
       * @brief Get the volume of the polytope that has been computed.
       *
       * @warning Volume can only be computed using V-representation.
       *H-representation volume computation is not predictable.
       *
       * @warning Probabilistic polytopes are not full dimensional : they lie in the
       *simplex' hyper plane,
       * therefor a pseudo-volume will be computed by projecting the polytope. The
       *projection used is the
       * lexicographically smallest coordinate subspace.
       *
       * @return A constant reference to the polytope volume.
       */
      const GUM_SCALAR &getVolume() const;

      /// @}

      /// @name setUp / tearDown
      /// @{

      /**
       * @brief %Sets up an H-representation.
       *
       * Initialize input matrix \c __input to correct dimensions and wrapper state
       *\c __state to \c __states::Hup.
       * @param card A constant reference to the cardinality of the variable.
       */
      void setUpH(const unsigned int &card);

      /**
       * @brief %Sets up a V-representation.
       *
       * Initialize input matrix \c __input to correct dimensions and wrapper state
       *\c __state to \c __states::Vup.
       * @param card A constant reference to the cardinality of the variable.
       * @param vertices A constant reference to the number of vertices of the
       *polytope.
       */
      void setUpV(const unsigned int &card, const unsigned int &vertices);

      /**
       * @brief Reset the wrapper as if it was built.
       *
       * Reset wrapper state \c __state to \c __states::none and clear all member
       *datas.
       */
      void tearDown();

      /**
       * @brief Reset the wrapper for next computation for a H-representation with
       *the same
       * variable cardinality and number of inequalities. Usefull when creating
       *credal networks
       * specified as intervals over modalities.
       *
       * Reset wrapper state \c __state to it's previous state and clear output
       *matrix \c __output.
       * Keeps the cardinality \c __card of the variable and therefor the input
       *matrix \c __intput structure.
       */
      void nextHInput();

      /// @}

      /// @name Input filling methods
      /// @{

      /**
       * @brief Creates the H-representation of min <= p(X=modal | .) <= max and add
       *it to the problem input \c __input.
       *
       * @param min The lower value of p(X=modal | .).
       * @param max The upper value of p(X=modal | .).
       * @param modal The modality on which we put constraints.
       */
      void fillH(const GUM_SCALAR &min, const GUM_SCALAR &max,
                 const unsigned int &modal);

      /**
       * @brief Fill the H-representation from the matrix given in argument.
       *
       * @param matrix The H-representation of the polytope of the form 0 <= -b + Ax,
       *A is the matrix, each column the coefficient of the variable in x.
       */
      void fillMatrix(const std::vector<std::vector<GUM_SCALAR>> &matrix);

      /**
       * @brief Creates the V-representation of a polytope by adding a vertex to the
       *problem input \c __input.
       *
       * @param vertex The vertex we wish to add to the V-representation of the
       *polytope.
       */
      void fillV(const std::vector<GUM_SCALAR> &vertex);

      /// @}

      /// @name lrs algorithms
      /// @{

      /**
       * @brief H-representation to V-representation.
       *
       * Computes the V-representation of a polytope, i.e. it's vertices, from it's
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
       * @brief Computes a polytope ( pseudo ) volume from it's V-representation.
       *
       * @warning Volume can only be computed using V-representation.
       *H-representation volume computation is not predictable.
       *
       * @warning Probabilistic polytopes are not full dimensional : they lie in the
       *simplex' hyper plane,
       * therefor a pseudo-volume will be computed by projecting the polytope. The
       *projection used is the
       * lexicographically smallest coordinate subspace.
       */
      void computeVolume();

      /**
       * @brief V-Redundancy elimination.
       *
       * Eliminates redundant vertices from a polytope V-representation input \c
       *__input.
       */
      void elimRedundVrep();

      /**
       * H-redundancy elimination.
       */

      /// @}
    };

  } // end of credal namespace
} // end of gum namespace

#include <agrum/CN/LrsWrapper.tcc>

#endif
