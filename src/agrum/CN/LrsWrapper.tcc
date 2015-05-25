#include <string.h>

#include "LrsWrapper.h"

namespace gum {
  namespace credal {

    template <typename GUM_SCALAR> LRSWrapper<GUM_SCALAR>::LRSWrapper() {
      __state = __states::none;

      __vertices = 0;
      __card = 0;

      __volume = 0;

      __getVolume = false;
      __hull = false;
      __polytope = false;

      GUM_CONSTRUCTOR(LRSWrapper);
    }

    template <typename GUM_SCALAR> LRSWrapper<GUM_SCALAR>::~LRSWrapper() {
      GUM_DESTRUCTOR(LRSWrapper);
    }

    template <typename GUM_SCALAR>
    auto LRSWrapper<GUM_SCALAR>::getInput() const -> const matrix & {
      return __input;
    }

    template <typename GUM_SCALAR>
    auto LRSWrapper<GUM_SCALAR>::getOutput() const -> const matrix & {
      return __output;
    }

    template <typename GUM_SCALAR>
    const unsigned int &LRSWrapper<GUM_SCALAR>::getVerticesNumber() const {
      return __vertices;
    }

    template <typename GUM_SCALAR>
    const GUM_SCALAR &LRSWrapper<GUM_SCALAR>::getVolume() const {
      if (__volume != 0)
        return __volume;
      else
        GUM_ERROR(OperationNotAllowed, "LRSWrapper< GUM_SCALAR >::getVolume () : "
                                       "volume computation was not asked for this "
                                       "credal set, call computeVolume() from a "
                                       "V-representation.");
    }

    template <typename GUM_SCALAR>
    void LRSWrapper<GUM_SCALAR>::setUpH(const unsigned int &card) {
      if (card < 2)
        GUM_ERROR(
            OperationNotAllowed,
            "LRSWrapper< GUM_SCALAR >::setUpH : cardinality must be at least 2");

      tearDown();

      __input = std::vector<std::vector<GUM_SCALAR>>(
          card * 2 + 2, std::vector<GUM_SCALAR>(card + 1, 0));

      __input[card * 2] = std::vector<GUM_SCALAR>(card + 1, -1);
      __input[card * 2][0] = 1;

      __input[card * 2 + 1] = std::vector<GUM_SCALAR>(card + 1, 1);
      __input[card * 2 + 1][0] = -1;

      __output = std::vector<std::vector<GUM_SCALAR>>();

      __vertex = std::vector<GUM_SCALAR>(card);

      __state = __states::Hup;

      __card = card;
    }

    template <typename GUM_SCALAR>
    void LRSWrapper<GUM_SCALAR>::setUpV(const unsigned int &card,
                                        const unsigned int &vertices) {
      if (card < 2)
        GUM_ERROR(
            OperationNotAllowed,
            "LRSWrapper< GUM_SCALAR >::setUpV : cardinality must be at least 2");

      if (vertices < 2)
        GUM_ERROR(OperationNotAllowed, "LRSWrapper< GUM_SCALAR >::setUpV : vertices "
                                       "must be at least 2 to build a polytope");

      tearDown();

      __input = std::vector<std::vector<GUM_SCALAR>>(
          vertices, std::vector<GUM_SCALAR>(card + 1, 1));

      __output = std::vector<std::vector<GUM_SCALAR>>();

      __state = __states::Vup;

      __card = card;
      __vertices = vertices;
    }

    template <typename GUM_SCALAR> void LRSWrapper<GUM_SCALAR>::tearDown() {
      __input.clear();
      __output.clear();
      __vertex.clear();
      __insertedModals.clear();

      __insertedVertices.clear();
      __vertices = 0;

      __volume = 0;

      __state = __states::none;
      __card = 0;

      __getVolume = false;
      __hull = false;
      __polytope = false;
    }

    template <typename GUM_SCALAR> void LRSWrapper<GUM_SCALAR>::nextHInput() {
      __insertedModals.clear();
      __insertedVertices.clear();
      __output.clear();
      __vertex.clear();
      __vertex.resize(__card, 0);

      __volume = 0;
      __vertices = 0;

      __getVolume = false;
      __hull = false;
      __polytope = false;

      if (__state == __states::H2Vready)
        __state = __states::Hup;
      else if (__state == __states::V2Hready) {
        __state = __states::Vup;
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::nextHInput : only for H-representation "
                  "as input. Previous state was : "
                      << __setUpStateNames[static_cast<int>(__state)]);
      } else {
        __input.clear();
        __state = __states::none;
        __card = 0;
        __vertex.clear();
      }
    }

    template <typename GUM_SCALAR>
    void LRSWrapper<GUM_SCALAR>::fillH(const GUM_SCALAR &min, const GUM_SCALAR &max,
                                       const unsigned int &modal) {
      if (__state != __states::Hup)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::fillH : setUpH or nextInput has not "
                  "been called or H-representation is complete, current state is : "
                      << __setUpStateNames[static_cast<int>(__state)]);

      if (modal >= __card)
        GUM_ERROR(OutOfBounds, "LRSWrapper< GUM_SCALAR >::fillH : modality is "
                               "greater or equal than cardinality : "
                                   << modal << " >= " << __card);

      __input[modal * 2][0] = -min;
      __input[modal * 2][modal + 1] = 1;

      __input[modal * 2 + 1][0] = max;
      __input[modal * 2 + 1][modal + 1] = -1;

      __vertex[modal] = max;

      __insertedModals.insert(modal);

      if (__insertedModals.size() == __card)
        __state = __states::H2Vready;
    }

    template <typename GUM_SCALAR>
    void LRSWrapper<GUM_SCALAR>::fillMatrix(
        const std::vector<std::vector<GUM_SCALAR>> &matrix) {
      if (__state != __states::Hup)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::fillH : setUpH or nextInput has not "
                  "been called or H-representation is complete, current state is : "
                      << __setUpStateNames[static_cast<int>(__state)]);

      if (matrix[0].size() - 1 != __card)
        GUM_ERROR(OutOfBounds, "LRSWrapper< GUM_SCALAR >::fillMatrix : size is "
                               "different than cardinality : "
                                   << (matrix[0].size() - 1) << " != " << __card);

      __input = matrix;

      for (unsigned int modal = 0; modal < __card; modal++) {
        __insertedModals.insert(modal);
      }

      __state = __states::H2Vready;
    }

    template <typename GUM_SCALAR>
    void LRSWrapper<GUM_SCALAR>::fillV(const std::vector<GUM_SCALAR> &vertex) {
      if (__state != __states::Vup)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::fillV : setUpV or nextInput has not "
                  "been called or V-representation is complete, current state is : "
                      << __setUpStateNames[static_cast<int>(__state)]);

      if (__insertedVertices.size() == __vertices)
        GUM_ERROR(OutOfBounds,
                  "LRSWrapper< GUM_SCALAR >::fillV : input is already full with "
                      << __vertices << " vertices.");

      bool eq = true;

      for (const auto &v : __insertedVertices) {
        eq = true;

        for (decltype(__card) mod = 0; mod < __card; mod++)
          if (std::fabs(v[mod] - vertex[mod]) > 1e-6) {
            eq = false;
            break;
          }

        if (eq) {
          __vertices--;
          return;
          // GUM_ERROR ( DuplicateElement, "LRSWrapper< GUM_SCALAR >::fillV : vertex
          // already present : " << vertex );
        }
      }

      auto row = __insertedVertices.size();

      for (decltype(__card) mod = 0; mod < __card; mod++)
        __input[row][mod + 1] = vertex[mod];

      __insertedVertices.push_back(vertex);

      if (__insertedVertices.size() == __vertices)
        __state = __states::V2Hready;
    }

    template <typename GUM_SCALAR> void LRSWrapper<GUM_SCALAR>::H2V() {
      if (__state != __states::H2Vready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::H2V : fillH has not been called with "
                  "all modalities, current state is still : "
                      << __setUpStateNames[static_cast<int>(__state)]);

      // check that we have a credal set and not a precise point probability, i.e.
      // sum of vertex elements is close to one ( floating type precision )
      GUM_SCALAR sum = 0;

      for (const auto elem : __vertex)
        sum += elem;

      if (std::fabs(sum - 1) < 1e-6) {
        __output = std::vector<std::vector<GUM_SCALAR>>(1, __vertex);
        return;
      }

      // not precise point probability, initialize lrs

      __coutOff();

      __initLrs();

      /* We initiate reverse search from this dictionary       */
      /* getting new dictionaries until the search is complete */
      /* User can access each output line from output which is */
      /* a vertex/ray/facet from the lrs_mp_vector output      */

      std::vector<long int> Num; /* numerators of all vertices */
      std::vector<long int> Den; /* denominators of all vertices */

      do {
        for (decltype(__dic->d) col = 0, end = __dic->d; col <= end; col++)
          if (lrs_getsolution(__dic, __dat, __lrsOutput, col)) {

            if (__dat->hull ||
                ((((__lrsOutput[0])[0] == 2 || (__lrsOutput[0])[0] == -2) &&
                  (__lrsOutput[0])[1] == 0)
                     ? 1L
                     : 0L)) {
              __coutOn();
              /*for ( decltype(Q->n) i = 0; i < Q->n; i++ )
                pmp ("", output[i]);*/
              GUM_ERROR(FatalError, "LRSWrapper< GUM_SCALAR >::H2V : asked for "
                                    "Q-hull computation or not reading a vertex !");
            } else
              for (decltype(__dat->n) i = 1, end = __dat->n; i < end; i++)
                __getLRSWrapperOutput(__lrsOutput[i], __lrsOutput[0], Num, Den);
          }
      } while (lrs_getnextbasis(&__dic, __dat, 0L));

      auto vtx = Num.size();
      std::vector<GUM_SCALAR> vertex(__card);

      for (decltype(vtx) i = 1; i <= vtx; i++) {
        vertex[(i - 1) % __card] = (Num[i - 1] * 1.0 / Den[i - 1]);

        if (i % __card == 0) {
          __output.push_back(vertex);
          __vertices++;
        }
      }

      __freeLrs();

      __coutOn();
    }

    template <typename GUM_SCALAR> void LRSWrapper<GUM_SCALAR>::V2H() {
      if (!__state == __states::V2Hready)
        GUM_ERROR(OperationNotAllowed, "LRSWrapper< GUM_SCALAR >::V2H : fillV has "
                                       "not been called with all vertices, current "
                                       "state is still : "
                                           << __setUpStateNames[__state]);
    }

    template <typename GUM_SCALAR> void LRSWrapper<GUM_SCALAR>::computeVolume() {
      if (!__state == __states::V2Hready)
        GUM_ERROR(OperationNotAllowed, "LRSWrapper< GUM_SCALAR >::computeVolume : "
                                       "volume is only for V-representation or "
                                       "fillV has not been called with all "
                                       "vertices, current state is still : "
                                           << __setUpStateNames[__state]);

      __coutOff();

      __getVolume = true;

      __initLrs();

      do {
        for (decltype(__dic->d) col = 0, end = __dic->d; col <= end; col++)
          lrs_getsolution(__dic, __dat, __lrsOutput, col);
      } while (lrs_getnextbasis(&__dic, __dat, 0L));

      long int Nsize =
          (__dat->Nvolume[0] > 0) ? __dat->Nvolume[0] : -__dat->Nvolume[0];
      long int Dsize =
          (__dat->Dvolume[0] > 0) ? __dat->Dvolume[0] : -__dat->Dvolume[0];

      long int num = 0L, den = 0L;
      long int tmp;

      for (decltype(Nsize) i = Nsize - 1; i > 0; i--) {
        tmp = __dat->Nvolume[i];

        for (decltype(i) j = 1; j < i; j++)
          tmp *= BASE;

        num += tmp;
      }

      for (decltype(Dsize) i = Dsize - 1; i > 0; i--) {
        tmp = __dat->Dvolume[i];

        for (decltype(i) j = 1; j < i; j++)
          tmp *= BASE;

        den += tmp;
      }

      __volume = num * 1.0 / den;

      __freeLrs();

      __coutOn();
    }

    template <typename GUM_SCALAR> void LRSWrapper<GUM_SCALAR>::elimRedundVrep() {
      if (__state != __states::V2Hready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::elimRedundVrep : only for "
                  "V-representation or fillV has not been called with all vertices, "
                  "current state is still : "
                      << __setUpStateNames[static_cast<int>(__state)]);

      __coutOff();

      __initLrs();

      long *redineq; /* redineq[i]=0 if ineq i non-red,1 if red,2 linearity  */

      /*********************************************************************************/
      /* Test each row of the dictionary to see if it is redundant */
      /*********************************************************************************/

      /* note some of these may have been changed in getting initial dictionary */
      auto m = __dic->m_A;
      auto d = __dic->d;
      /* number of linearities in input */ /* should be 0 ! */
      auto nlinearity = __dat->nlinearity;
      auto lastdv = __dat->lastdv;

      /* linearities are not considered for redundancy */
      redineq = (long int *)calloc((m + 1), sizeof(long));

      for (decltype(nlinearity) i = 0; i < nlinearity; i++)
        redineq[__dat->linearity[i]] = 2L;

      /* rows 0..lastdv are cost, decision variables, or linearities  */
      /* other rows need to be tested                                */

      for (decltype(m + d) index = lastdv + 1, end = m + d; index <= end; index++) {
        /* input inequality number of current index             */
        auto ineq =
            __dat->inequality[index - lastdv]; /* the input inequality number corr.
                                                  to this index */

        redineq[ineq] = checkindex(__dic, __dat, index);
      }

      /* linearities */
      if (nlinearity > 0)
        GUM_ERROR(FatalError, "LRSWrapper< GUM_SCALAR >::elimRedundVrep : not "
                              "reading a vertex but a linearity !");

      /* count number of non-redundant inequalities */
      /*
      auto nredund = nlinearity;
      for ( decltype ( m ) i = 1; i <= m; i++ )
        if ( redineq[ i ] == 0 )
          nredund++;
      */

      //__vertices = nredund;
      //__output = std::vector< std::vector< GUM_SCALAR > > ( nredund, std::vector<
      // GUM_SCALAR > ( __dat->n - 1 ) );

      for (decltype(m) i = 1; i <= m; i++)
        if (redineq[i] == 0)
          __output.push_back(std::vector<GUM_SCALAR>(++__input[i - 1].begin(),
                                                     __input[i - 1].end()));

      __vertices = __output.size();

      __freeLrs();

      __coutOn();
    }

    template <typename GUM_SCALAR>
    void
    LRSWrapper<GUM_SCALAR>::__getLRSWrapperOutput(lrs_mp Nin, lrs_mp Din,
                                                  std::vector<long int> &Num,
                                                  std::vector<long int> &Den) const {

      long int Nsize = (Nin[0] > 0) ? Nin[0] : -Nin[0];
      long int Dsize = (Din[0] > 0) ? Din[0] : -Din[0];

      long int num = 0L;
      long int den = 0L;

      long int tmp;

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

      long int Nsign = ((Nin[0] < 0) ? -1L : 1L);
      long int Dsign = ((Din[0] < 0) ? -1L : 1L);

      if ((Nsign * Dsign) == -1L)
        num = -num;

      Num.push_back(num);
      Den.push_back(den);
    }

    /*
    void pmp (char name[], lrs_mp a) {
         long i;
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

    template <typename GUM_SCALAR> void LRSWrapper<GUM_SCALAR>::__fill() const {
      auto cols = __input[0].size();

      long int *num = new long int[cols]; // ISO C++ forbids variable length array,
                                          // we need to do this instead
      long int *den = new long int[cols];

      auto rows = __input.size();

      long int numerator, denominator;

      for (decltype(rows) row = 0; row < rows; row++) {
        for (decltype(cols) col = 0; col < cols; col++) {
          Rational<GUM_SCALAR>::continuedFracFirst(numerator, denominator,
                                                   __input[row][col]);

          num[col] = numerator;
          den[col] = denominator;
        }

        /* GE is inequality, EQ is equation */
        /* 1L, 0L respectively */
        lrs_set_row(__dic, __dat, row + 1, num, den,
                    1L); // do NOT forget this + 1 on row
      }

      delete[] num;
      delete[] den;
    }

    template <typename GUM_SCALAR> void LRSWrapper<GUM_SCALAR>::__initLrs() {
      if (__state != __states::H2Vready && __state != __states::V2Hready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::__initLrs : not ready, current state "
                  "is still : "
                      << __setUpStateNames[static_cast<int>(__state)]);

      //__coutOff();

      std::string name = "\n*LrsWrapper:";
      std::vector<char> chars(name.c_str(), name.c_str() + name.size() + 1u);
      // use &chars[0] as a char*

      if (!lrs_init(&chars[0])) {
        __coutOn();
        GUM_ERROR(FatalError,
                  "LRSWrapper< GUM_SCALAR >::__initLrs : failed lrs_init");
      }

      name = "LRSWrapper globals";
      chars = std::vector<char>(name.c_str(), name.c_str() + name.size() + 1u);

      __dat = lrs_alloc_dat(&chars[0]);

      if (__dat == nullptr) {
        __coutOn();
        GUM_ERROR(FatalError,
                  "LRSWrapper< GUM_SCALAR >::__initLrs : failed lrs_alloc_dat");
      }

      __dat->n = __input[0].size();
      __dat->m = __input.size();

      __dat->getvolume = (__getVolume) ? 1L : 0L;
      __dat->hull = (__hull) ? 1L : 0L;
      __dat->polytope = (__polytope) ? 1L : 0L;

      __lrsOutput = lrs_alloc_mp_vector_wrapper(__dat->n);

      __dic = lrs_alloc_dic(__dat);

      if (__dic == nullptr) {
        __coutOn();
        GUM_ERROR(FatalError,
                  "LRSWrapper< GUM_SCALAR >::__initLrs : failed lrs_alloc_dic");
      }

      __fill();

      /* Pivot to a starting dictionary */
      if (!lrs_getfirstbasis(&__dic, __dat, &__Lin, 0L)) {
        __coutOn();
        GUM_ERROR(FatalError,
                  "LRSWrapper< GUM_SCALAR >::__initLrs : failed lrs_getfirstbasis");
      }

      /* There may have been column redundancy */
      /* If so the linearity space is obtained and redundant */
      /* columns are removed. User can access linearity space */
      /* from lrs_mp_matrix Lin dimensions nredundcol x d+1  */

      if (__dat->nredundcol > 0) {
        __coutOn();

        for (decltype(__dat->nredundcol) col = 0, end = __dat->nredundcol; col < end;
             col++)
          lrs_printoutput(__dat, __Lin[col]);

        GUM_ERROR(FatalError,
                  "LRSWrapper< GUM_SCALAR >::__initLrs : redundant columns !");
      }
    }

    template <typename GUM_SCALAR> void LRSWrapper<GUM_SCALAR>::__freeLrs() {
      /* free space : do not change order of next 3 lines! */

      lrs_clear_mp_vector_wrapper(__lrsOutput, __dat->n);
      lrs_free_dic(__dic, __dat);
      lrs_free_dat(__dat);

      std::string name = "LrsWrapper:";
      std::vector<char> chars(name.c_str(), name.c_str() + name.size() + 1u);

      lrs_close(&chars[0]);

      __coutOn();
    }

    template <typename GUM_SCALAR> void LRSWrapper<GUM_SCALAR>::__coutOff() const {
      fflush(stdout);
      __oldCout = dup(1);

      int new_cout = open("/dev/null", O_WRONLY);
      dup2(new_cout, 1);
      close(new_cout);
    }

    template <typename GUM_SCALAR> void LRSWrapper<GUM_SCALAR>::__coutOn() const {
      fflush(stdout);
      dup2(__oldCout, 1);
      close(__oldCout);
    }

  } // end of credal namespace
} // end of gum namespace
