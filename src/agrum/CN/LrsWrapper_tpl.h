/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <string.h>

#include <agrum/CN/LrsWrapper.h>
#include <agrum/agrum.h>

namespace gum {
  namespace credal {

    template < typename GUM_SCALAR >
    LRSWrapper< GUM_SCALAR >::LRSWrapper() {
      state__ = states__::none;

      vertices__ = 0;
      card__ = 0;

      volume__ = 0;

      getVolume__ = false;
      hull__ = false;
      polytope__ = false;

      GUM_CONSTRUCTOR(LRSWrapper);
    }

    template < typename GUM_SCALAR >
    LRSWrapper< GUM_SCALAR >::~LRSWrapper() {
      GUM_DESTRUCTOR(LRSWrapper);
    }

    template < typename GUM_SCALAR >
    auto LRSWrapper< GUM_SCALAR >::getInput() const -> const matrix& {
      return input__;
    }

    template < typename GUM_SCALAR >
    auto LRSWrapper< GUM_SCALAR >::getOutput() const -> const matrix& {
      return output__;
    }

    template < typename GUM_SCALAR >
    const unsigned int& LRSWrapper< GUM_SCALAR >::getVerticesNumber() const {
      return vertices__;
    }

    template < typename GUM_SCALAR >
    const GUM_SCALAR& LRSWrapper< GUM_SCALAR >::getVolume() const {
      if (volume__ != 0)
        return volume__;
      else
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::getVolume () : "
                  "volume computation was not asked for this "
                  "credal set, call computeVolume() from a "
                  "V-representation.");
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::setUpH(const Size& card) {
      if (card < 2)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::setUpH : "
                  "cardinality must be at least 2");

      tearDown();

      input__ = std::vector< std::vector< GUM_SCALAR > >(
         card * 2 + 2, std::vector< GUM_SCALAR >(card + 1, 0));

      input__[card * 2] = std::vector< GUM_SCALAR >(card + 1, -1);
      input__[card * 2][0] = 1;

      input__[card * 2 + 1] = std::vector< GUM_SCALAR >(card + 1, 1);
      input__[card * 2 + 1][0] = -1;

      output__ = std::vector< std::vector< GUM_SCALAR > >();

      vertex__ = std::vector< GUM_SCALAR >(card);

      state__ = states__::Hup;

      card__ = (unsigned int)card;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::setUpV(const Size& card, const Size& vertices) {
      if (card < 2)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::setUpV : "
                  "cardinality must be at least 2");

      if (vertices < 2)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::setUpV : vertices "
                  "must be at least 2 to build a polytope");

      tearDown();

      input__ = std::vector< std::vector< GUM_SCALAR > >(
         vertices, std::vector< GUM_SCALAR >(card + 1, 1));

      output__ = std::vector< std::vector< GUM_SCALAR > >();

      state__ = states__::Vup;

      card__ = (unsigned int)card;
      vertices__ = (unsigned int)vertices;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::tearDown() {
      input__.clear();
      output__.clear();
      vertex__.clear();
      insertedModals__.clear();

      insertedVertices__.clear();
      vertices__ = 0;

      volume__ = 0;

      state__ = states__::none;
      card__ = 0;

      getVolume__ = false;
      hull__ = false;
      polytope__ = false;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::nextHInput() {
      insertedModals__.clear();
      insertedVertices__.clear();
      output__.clear();
      vertex__.clear();
      vertex__.resize(card__, 0);

      volume__ = 0;
      vertices__ = 0;

      getVolume__ = false;
      hull__ = false;
      polytope__ = false;

      if (state__ == states__::H2Vready)
        state__ = states__::Hup;
      else if (state__ == states__::V2Hready) {
        state__ = states__::Vup;
        GUM_ERROR(
           OperationNotAllowed,
           "LRSWrapper< GUM_SCALAR >::nextHInput : only for H-representation "
           "as input. Previous state was : "
              << setUpStateNames__[static_cast< int >(state__)]);
      } else {
        input__.clear();
        state__ = states__::none;
        card__ = 0;
        vertex__.clear();
      }
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::fillH(const GUM_SCALAR& min,
                                         const GUM_SCALAR& max,
                                         const Size&       modal) {
      if (state__ != states__::Hup)
        GUM_ERROR(
           OperationNotAllowed,
           "LRSWrapper< GUM_SCALAR >::fillH : setUpH or nextInput has not "
           "been called or H-representation is complete, current state is : "
              << setUpStateNames__[static_cast< int >(state__)]);

      if (modal >= card__)
        GUM_ERROR(OutOfBounds,
                  "LRSWrapper< GUM_SCALAR >::fillH : modality is "
                  "greater or equal than cardinality : "
                     << modal << " >= " << card__);

      input__[modal * 2][0] = -min;
      input__[modal * 2][modal + 1] = 1;

      input__[modal * 2 + 1][0] = max;
      input__[modal * 2 + 1][modal + 1] = -1;

      vertex__[modal] = max;

      insertedModals__.insert(int(modal));

      if (insertedModals__.size() == card__) state__ = states__::H2Vready;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::fillMatrix(
       const std::vector< std::vector< GUM_SCALAR > >& matrix) {
      if (state__ != states__::Hup)
        GUM_ERROR(
           OperationNotAllowed,
           "LRSWrapper< GUM_SCALAR >::fillH : setUpH or nextInput has not "
           "been called or H-representation is complete, current state is : "
              << setUpStateNames__[static_cast< int >(state__)]);

      if (matrix[0].size() - 1 != card__)
        GUM_ERROR(OutOfBounds,
                  "LRSWrapper< GUM_SCALAR >::fillMatrix : size is "
                  "different than cardinality : "
                     << (matrix[0].size() - 1) << " != " << card__);

      input__ = matrix;

      for (unsigned int modal = 0; modal < card__; modal++) {
        insertedModals__.insert(modal);
      }

      state__ = states__::H2Vready;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::fillV(const std::vector< GUM_SCALAR >& vertex) {
      if (state__ != states__::Vup)
        GUM_ERROR(
           OperationNotAllowed,
           "LRSWrapper< GUM_SCALAR >::fillV : setUpV or nextInput has not "
           "been called or V-representation is complete, current state is : "
              << setUpStateNames__[static_cast< int >(state__)]);

      if (insertedVertices__.size() == vertices__)
        GUM_ERROR(OutOfBounds,
                  "LRSWrapper< GUM_SCALAR >::fillV : input is already full with "
                     << vertices__ << " vertices.");

      bool eq = true;

      for (const auto& v: insertedVertices__) {
        eq = true;

        for (decltype(card__) mod = 0; mod < card__; mod++)
          if (std::fabs(v[mod] - vertex[mod]) > 1e-6) {
            eq = false;
            break;
          }

        if (eq) {
          vertices__--;
          return;
          // GUM_ERROR ( DuplicateElement, "LRSWrapper< GUM_SCALAR >::fillV :
          // vertex
          // already present : " << vertex );
        }
      }

      auto row = insertedVertices__.size();

      for (decltype(card__) mod = 0; mod < card__; mod++)
        input__[row][mod + 1] = vertex[mod];

      insertedVertices__.push_back(vertex);

      if (insertedVertices__.size() == vertices__) state__ = states__::V2Hready;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::H2V() {
      if (state__ != states__::H2Vready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::H2V : fillH has not been called with "
                  "all modalities, current state is still : "
                     << setUpStateNames__[static_cast< int >(state__)]);

      // check that we have a credal set and not a precise point probability,
      // i.e.
      // sum of vertex elements is close to one ( floating type precision )
      GUM_SCALAR sum = 0;

      for (const auto elem: vertex__)
        sum += elem;

      if (std::fabs(sum - 1) < 1e-6) {
        output__ = std::vector< std::vector< GUM_SCALAR > >(1, vertex__);
        return;
      }

      // not precise point probability, initialize lrs

      // coutOff__();

      initLrs__();

      /* We initiate reverse search from this dictionary       */
      /* getting new dictionaries until the search is complete */
      /* User can access each output line from output which is */
      /* vertex/ray/facet from the lrs_mp_vector output         */
      /* prune is TRUE if tree should be pruned at current node */

      // pruning is not used

      std::vector< int64_t > Num; /* numerators of all vertices */
      std::vector< int64_t > Den; /* denominators of all vertices */

      do {
        for (decltype(dic__->d) col = 0, end = dic__->d; col <= end; col++)
          if (lrs_getsolution(dic__, dat__, lrsOutput__, col)) {
            // iszero macro could be used here for the test on right
            if (dat__->hull
                || ((((lrsOutput__[0])[0] == 2 || (lrsOutput__[0])[0] == -2)
                     && (lrsOutput__[0])[1] == 0)
                       ? 1L
                       : 0L)) {
              // coutOn__();
              /*for ( decltype(Q->n) i = 0; i < Q->n; i++ )
                pmp ("", output[i]);*/
              GUM_ERROR(FatalError,
                        "LRSWrapper< GUM_SCALAR >::H2V : asked for "
                        "Q-hull computation or not reading a vertex !");
            } else
              for (decltype(dat__->n) i = 1, end = dat__->n; i < end; i++)
                getLRSWrapperOutput__(lrsOutput__[i], lrsOutput__[0], Num, Den);
          }
      } while (lrs_getnextbasis(&dic__, dat__, 0L));

      auto                      vtx = Num.size();
      std::vector< GUM_SCALAR > vertex(card__);

      for (decltype(vtx) i = 1; i <= vtx; i++) {
        vertex[(i - 1) % card__] = GUM_SCALAR(Num[i - 1] * 1.0 / Den[i - 1]);

        if (i % card__ == 0) {
          output__.push_back(vertex);
          vertices__++;
        }
      }

      freeLrs__();

      // coutOn__();
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::V2H() {
      if (!state__ == states__::V2Hready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::V2H : fillV has "
                  "not been called with all vertices, current "
                  "state is still : "
                     << setUpStateNames__[state__]);
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::computeVolume() {
      if (!state__ == states__::V2Hready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::computeVolume : "
                  "volume is only for V-representation or "
                  "fillV has not been called with all "
                  "vertices, current state is still : "
                     << setUpStateNames__[state__]);

      // coutOff__();

      getVolume__ = true;

      initLrs__();

      do {
        for (decltype(dic__->d) col = 0, end = dic__->d; col <= end; col++)
          lrs_getsolution(dic__, dat__, lrsOutput__, col);
      } while (lrs_getnextbasis(&dic__, dat__, 0L));

      int64_t Nsize =
         (dat__->Nvolume[0] > 0) ? dat__->Nvolume[0] : -dat__->Nvolume[0];
      int64_t Dsize =
         (dat__->Dvolume[0] > 0) ? dat__->Dvolume[0] : -dat__->Dvolume[0];

      int64_t num = 0L, den = 0L;
      int64_t tmp;

      for (decltype(Nsize) i = Nsize - 1; i > 0; i--) {
        tmp = dat__->Nvolume[i];

        for (decltype(i) j = 1; j < i; j++)
          tmp *= BASE;

        num += tmp;
      }

      for (decltype(Dsize) i = Dsize - 1; i > 0; i--) {
        tmp = dat__->Dvolume[i];

        for (decltype(i) j = 1; j < i; j++)
          tmp *= BASE;

        den += tmp;
      }

      volume__ = num * 1.0 / den;

      freeLrs__();

      // coutOn__();
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::elimRedundVrep() {
      if (state__ != states__::V2Hready)
        GUM_ERROR(
           OperationNotAllowed,
           "LRSWrapper< GUM_SCALAR >::elimRedundVrep : only for "
           "V-representation or fillV has not been called with all vertices, "
           "current state is still : "
              << setUpStateNames__[static_cast< int >(state__)]);

      // coutOff__();

      initLrs__();

      int64_t* redineq; /* redineq[i]=0 if ineq i non-red,1 if red,2 linearity  */

      /*********************************************************************************/
      /* Test each row of the dictionary to see if it is redundant */
      /*********************************************************************************/

      /* note some of these may have been changed in getting initial dictionary
       */
      auto m = dic__->m_A;
      auto d = dic__->d;
      /* number of linearities in input */ /* should be 0 ! */
      auto nlinearity = dat__->nlinearity;
      auto lastdv = dat__->lastdv;

      /* linearities are not considered for redundancy */
      redineq = (int64_t*)calloc(std::size_t(m + 1), sizeof(int64_t));

      for (decltype(nlinearity) i = 0; i < nlinearity; i++)
        redineq[dat__->linearity[i]] = 2L;

      /* rows 0..lastdv are cost, decision variables, or linearities  */
      /* other rows need to be tested                                */

      for (decltype(m + d) index = lastdv + 1, end = m + d; index <= end;
           index++) {
        /* input inequality number of current index             */
        auto ineq =
           dat__->inequality[index - lastdv]; /* the input inequality number corr.
                                                 to this index */

        redineq[ineq] = checkindex(dic__, dat__, index);
      }

      /* linearities */
      if (nlinearity > 0)
        GUM_ERROR(FatalError,
                  "LRSWrapper< GUM_SCALAR >::elimRedundVrep : not "
                  "reading a vertex but a linearity !");

      /* count number of non-redundant inequalities */
      /*
      auto nredund = nlinearity;
      for ( decltype ( m ) i = 1; i <= m; i++ )
        if ( redineq[ i ] == 0 )
          nredund++;
      */

      //__vertices = nredund;
      //__output = std::vector< std::vector< GUM_SCALAR > > ( nredund,
      // std::vector<
      // GUM_SCALAR > ( dat__->n - 1 ) );

      for (decltype(m) i = 1; i <= m; i++)
        if (redineq[i] == 0)
          output__.push_back(
             std::vector< GUM_SCALAR >(++input__[std::size_t(i - 1)].begin(),
                                       input__[std::size_t(i - 1)].end()));

      vertices__ = (unsigned int)output__.size();

      freeLrs__();

      // coutOn__();
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::getLRSWrapperOutput__(
       lrs_mp                  Nin,
       lrs_mp                  Din,
       std::vector< int64_t >& Num,
       std::vector< int64_t >& Den) const {
      int64_t Nsize = (Nin[0] > 0) ? Nin[0] : -Nin[0];
      int64_t Dsize = (Din[0] > 0) ? Din[0] : -Din[0];

      int64_t num = 0L;
      int64_t den = 0L;

      int64_t tmp;

      for (decltype(Nsize) i = Nsize - 1; i > 0; i--) {
        tmp = Nin[i];

        for (decltype(i) j = 1; j < i; j++)
          tmp *= BASE;

        num += tmp;
      }

      if (!(Din[0] == 2L && Din[1] == 1L)) { /* rational */
        for (decltype(Dsize) i = Dsize - 1; i > 0; i--) {
          tmp = Din[i];

          for (decltype(i) j = 1; j < i; j++)
            tmp *= BASE;

          den += tmp;
        }
      } else {
        den = 1L;
      }

      int64_t Nsign = ((Nin[0] < 0) ? -1L : 1L);
      int64_t Dsign = ((Din[0] < 0) ? -1L : 1L);

      if ((Nsign * Dsign) == -1L) num = -num;

      Num.push_back(num);
      Den.push_back(den);
    }

    /*
    void pmp (char name[], lrs_mp a) {
         int64_t i;
         fprintf (lrs_ofp, "%s", name);
         if (sign (a) == NEG)
           fprintf (lrs_ofp, "-");
         else
           fprintf (lrs_ofp, " ");
         fprintf (lrs_ofp, "%lu", a[length (a) - 1]);
         for (i = length (a) - 2; i >= 1; i--)
           fprintf (lrs_ofp, FORMAT, a[i]);
         fprintf (lrs_ofp, " ");
    }*/

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::fill__() const {
      std::size_t cols = input__[0].size();

      int64_t* num = new int64_t[cols];   // ISO C++ forbids variable length array,
                                          // we need to do this instead
      int64_t* den = new int64_t[cols];

      int64_t rows = int64_t(input__.size());

      int64_t numerator, denominator;

      for (int64_t row = 0; row < rows; row++) {
        for (std::size_t col = 0; col < cols; col++) {
          Rational< GUM_SCALAR >::continuedFracFirst(
             numerator, denominator, input__[std::size_t(row)][col]);

          num[col] = numerator;
          den[col] = denominator;
        }

        /* GE is inequality, EQ is equation */
        /* 1L, 0L respectively */
        lrs_set_row(dic__,
                    dat__,
                    int64_t(row + 1),
                    num,
                    den,
                    1L);   // do NOT forget this + 1 on row
      }

      delete[] num;
      delete[] den;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::initLrs__() {
      if (state__ != states__::H2Vready && state__ != states__::V2Hready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::initLrs__ : not ready, current state "
                  "is still : "
                     << setUpStateNames__[static_cast< int >(state__)]);

      //__coutOff();

      std::string         name = "\n*LrsWrapper:";
      std::vector< char > chars(name.c_str(), name.c_str() + name.size() + 1u);
      // use &chars[0] as a char*

      if (!lrs_init(&chars[0])) {
        // coutOn__();
        GUM_ERROR(FatalError,
                  "LRSWrapper< GUM_SCALAR >::initLrs__ : failed lrs_init");
      }

      name = "LRSWrapper globals";
      chars = std::vector< char >(name.c_str(), name.c_str() + name.size() + 1u);

      dat__ = lrs_alloc_dat(&chars[0]);

      if (dat__ == nullptr) {
        // coutOn__();
        GUM_ERROR(FatalError,
                  "LRSWrapper< GUM_SCALAR >::initLrs__ : failed lrs_alloc_dat");
      }

      dat__->n = Size(input__[0].size());
      dat__->m = Size(input__.size());

      dat__->getvolume = (getVolume__) ? 1L : 0L;
      dat__->hull = (hull__) ? 1L : 0L;
      dat__->polytope = (polytope__) ? 1L : 0L;

      lrsOutput__ = lrs_alloc_mp_vector(dat__->n);

      dic__ = lrs_alloc_dic(dat__);

      if (dic__ == nullptr) {
        // coutOn__();
        GUM_ERROR(FatalError,
                  "LRSWrapper< GUM_SCALAR >::initLrs__ : failed lrs_alloc_dic");
      }

      fill__();

      /* Pivot to a starting dictionary */
      if (!lrs_getfirstbasis(&dic__, dat__, &Lin__, 0L)) {
        // coutOn__();
        GUM_ERROR(
           FatalError,
           "LRSWrapper< GUM_SCALAR >::initLrs__ : failed lrs_getfirstbasis");
      }

      /* There may have been column redundancy */
      /* If so the linearity space is obtained and redundant */
      /* columns are removed. User can access linearity space */
      /* from lrs_mp_matrix Lin dimensions nredundcol x d+1  */

      decltype(dat__->nredundcol) startcol = 0;

      if (dat__->homogeneous && dat__->hull) {
        startcol++; /* col zero not treated as redundant   */

        if (!dat__->restart) {
          // coutOn__();

          for (decltype(dat__->nredundcol) col = startcol; col < dat__->nredundcol;
               col++)
            lrs_printoutput(dat__, Lin__[col]);

          GUM_ERROR(FatalError,
                    "LRSWrapper< GUM_SCALAR >::initLrs__ : redundant columns !");
        }
      }
      /*
  if ( dat__->nredundcol > 0 ) {
    coutOn__();

            for ( decltype( dat__->nredundcol ) col = 0, end =
  dat__->nredundcol;
          col < end;
          col++ )
      lrs_printoutput( dat__, Lin__[col] );

    GUM_ERROR(
        FatalError,
        "LRSWrapper< GUM_SCALAR >::initLrs__ : redundant columns !" );
  }
  */
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::freeLrs__() {
      /* free space : do not change order of next lines! */

      lrs_clear_mp_vector(lrsOutput__, dat__->n);

      if (dat__->nredundcol > 0)
        lrs_clear_mp_matrix(Lin__, dat__->nredundcol, dat__->n);

      if (dat__->runs > 0) {
        free(dat__->isave);
        free(dat__->jsave);
      }

      auto savem = dic__->m; /* need this to clear dat__*/

      lrs_free_dic(dic__, dat__); /* deallocate lrs_dic */

      dat__->m = savem;
      lrs_free_dat(dat__);

      std::string         name = "LrsWrapper:";
      std::vector< char > chars(name.c_str(), name.c_str() + name.size() + 1u);

      lrs_close(&chars[0]);

      // coutOn__();
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::coutOff__() const {
      fflush(stdout);
#ifdef _MSC_VER
      freopen("NUL", "w", stdout);
#else   // _MSC_VER
      oldCout__ = dup(1);

      int new_cout = open("/dev/null", O_WRONLY);
      dup2(new_cout, 1);
      close(new_cout);
#endif   // _MSC_VER
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::coutOn__() const {
      fflush(stdout);
#ifdef _MSC_VER
      freopen("CON", "w", stdout);
#else   // _MSC_VER
      dup2(oldCout__, 1);
      close(oldCout__);
#endif   // _MSC_VER
    }

  }   // namespace credal
}   // namespace gum
