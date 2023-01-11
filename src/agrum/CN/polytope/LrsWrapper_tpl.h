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


#include <string.h>

#include <agrum/CN/polytope/LrsWrapper.h>
#include <agrum/agrum.h>

namespace gum {
  namespace credal {

    template < typename GUM_SCALAR >
    LRSWrapper< GUM_SCALAR >::LRSWrapper() {
      _state_ = _states_::none;

      _vertices_ = 0;
      _card_     = 0;

      _volume_ = 0;

      _getVolume_ = false;
      _hull_      = false;
      _polytope_  = false;

      GUM_CONSTRUCTOR(LRSWrapper);
    }

    template < typename GUM_SCALAR >
    LRSWrapper< GUM_SCALAR >::~LRSWrapper() {
      GUM_DESTRUCTOR(LRSWrapper);
    }

    template < typename GUM_SCALAR >
    auto LRSWrapper< GUM_SCALAR >::getInput() const -> const matrix& {
      return _input_;
    }

    template < typename GUM_SCALAR >
    auto LRSWrapper< GUM_SCALAR >::getOutput() const -> const matrix& {
      return _output_;
    }

    template < typename GUM_SCALAR >
    const unsigned int& LRSWrapper< GUM_SCALAR >::getVerticesNumber() const {
      return _vertices_;
    }

    template < typename GUM_SCALAR >
    const GUM_SCALAR& LRSWrapper< GUM_SCALAR >::getVolume() const {
      if (_volume_ != 0) return _volume_;
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

      _input_ = std::vector< std::vector< GUM_SCALAR > >(card * 2 + 2,
                                                         std::vector< GUM_SCALAR >(card + 1, 0));

      _input_[card * 2]    = std::vector< GUM_SCALAR >(card + 1, -1);
      _input_[card * 2][0] = 1;

      _input_[card * 2 + 1]    = std::vector< GUM_SCALAR >(card + 1, 1);
      _input_[card * 2 + 1][0] = -1;

      _output_ = std::vector< std::vector< GUM_SCALAR > >();

      _vertex_ = std::vector< GUM_SCALAR >(card);

      _state_ = _states_::Hup;

      _card_ = (unsigned int)card;
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

      _input_ = std::vector< std::vector< GUM_SCALAR > >(vertices,
                                                         std::vector< GUM_SCALAR >(card + 1, 1));

      _output_ = std::vector< std::vector< GUM_SCALAR > >();

      _state_ = _states_::Vup;

      _card_     = (unsigned int)card;
      _vertices_ = (unsigned int)vertices;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::tearDown() {
      _input_.clear();
      _output_.clear();
      _vertex_.clear();
      _insertedModals_.clear();

      _insertedVertices_.clear();
      _vertices_ = 0;

      _volume_ = 0;

      _state_ = _states_::none;
      _card_  = 0;

      _getVolume_ = false;
      _hull_      = false;
      _polytope_  = false;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::nextHInput() {
      _insertedModals_.clear();
      _insertedVertices_.clear();
      _output_.clear();
      _vertex_.clear();
      _vertex_.resize(_card_, 0);

      _volume_   = 0;
      _vertices_ = 0;

      _getVolume_ = false;
      _hull_      = false;
      _polytope_  = false;

      if (_state_ == _states_::H2Vready) _state_ = _states_::Hup;
      else if (_state_ == _states_::V2Hready) {
        _state_ = _states_::Vup;
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::nextHInput : only for H-representation "
                  "as input. Previous state was : "
                     << _setUpStateNames_[static_cast< int >(_state_)]);
      } else {
        _input_.clear();
        _state_ = _states_::none;
        _card_  = 0;
        _vertex_.clear();
      }
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::fillH(const GUM_SCALAR& min,
                                         const GUM_SCALAR& max,
                                         const Size&       modal) {
      if (_state_ != _states_::Hup)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::fillH : setUpH or nextInput has not "
                  "been called or H-representation is complete, current state is : "
                     << _setUpStateNames_[static_cast< int >(_state_)]);

      if (modal >= _card_)
        GUM_ERROR(OutOfBounds,
                  "LRSWrapper< GUM_SCALAR >::fillH : modality is "
                  "greater or equal than cardinality : "
                     << modal << " >= " << _card_);

      _input_[modal * 2][0]         = -min;
      _input_[modal * 2][modal + 1] = 1;

      _input_[modal * 2 + 1][0]         = max;
      _input_[modal * 2 + 1][modal + 1] = -1;

      _vertex_[modal] = max;

      _insertedModals_.insert(int(modal));

      if (_insertedModals_.size() == _card_) _state_ = _states_::H2Vready;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::fillMatrix(
       const std::vector< std::vector< GUM_SCALAR > >& matrix) {
      if (_state_ != _states_::Hup)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::fillH : setUpH or nextInput has not "
                  "been called or H-representation is complete, current state is : "
                     << _setUpStateNames_[static_cast< int >(_state_)]);

      if (matrix[0].size() - 1 != _card_)
        GUM_ERROR(OutOfBounds,
                  "LRSWrapper< GUM_SCALAR >::fillMatrix : size is "
                  "different than cardinality : "
                     << (matrix[0].size() - 1) << " != " << _card_);

      _input_ = matrix;

      for (unsigned int modal = 0; modal < _card_; modal++) {
        _insertedModals_.insert(modal);
      }

      _state_ = _states_::H2Vready;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::fillV(const std::vector< GUM_SCALAR >& vertex) {
      if (_state_ != _states_::Vup)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::fillV : setUpV or nextInput has not "
                  "been called or V-representation is complete, current state is : "
                     << _setUpStateNames_[static_cast< int >(_state_)]);

      if (_insertedVertices_.size() == _vertices_)
        GUM_ERROR(OutOfBounds,
                  "LRSWrapper< GUM_SCALAR >::fillV : input is already full with " << _vertices_
                                                                                  << " vertices.");

      bool eq = true;

      for (const auto& v: _insertedVertices_) {
        eq = true;

        for (decltype(_card_) mod = 0; mod < _card_; mod++)
          if (std::fabs(v[mod] - vertex[mod]) > 1e-6) {
            eq = false;
            break;
          }

        if (eq) {
          _vertices_--;
          return;
          // GUM_ERROR ( DuplicateElement, "LRSWrapper< GUM_SCALAR >::fillV :
          // vertex
          // already present : " << vertex );
        }
      }

      auto row = _insertedVertices_.size();

      for (decltype(_card_) mod = 0; mod < _card_; mod++)
        _input_[row][mod + 1] = vertex[mod];

      _insertedVertices_.push_back(vertex);

      if (_insertedVertices_.size() == _vertices_) _state_ = _states_::V2Hready;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::H2V() {
      if (_state_ != _states_::H2Vready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::H2V : fillH has not been called with "
                  "all modalities, current state is still : "
                     << _setUpStateNames_[static_cast< int >(_state_)]);

      // check that we have a credal set and not a precise point probability,
      // i.e.
      // sum of vertex elements is close to one ( floating type precision )
      GUM_SCALAR sum = 0;

      for (const auto elem: _vertex_)
        sum += elem;

      if (std::fabs(sum - 1) < 1e-6) {
        _output_ = std::vector< std::vector< GUM_SCALAR > >(1, _vertex_);
        return;
      }

      // not precise point probability, initialize lrs

      //  _coutOff_();

      _initLrs_();

      /* We initiate reverse search from this dictionary       */
      /* getting new dictionaries until the search is complete */
      /* User can access each output line from output which is */
      /* vertex/ray/facet from the lrs_mp_vector output         */
      /* prune is TRUE if tree should be pruned at current node */

      // pruning is not used

      std::vector< int64_t > Num; /* numerators of all vertices */
      std::vector< int64_t > Den; /* denominators of all vertices */

      do {
        for (decltype(_dic_->d) col = 0, end = _dic_->d; col <= end; col++)
          if (lrs_getsolution(_dic_, _dat_, _lrsOutput_, col)) {
            // iszero macro could be used here for the test on right
            if (_dat_->hull
                || ((((_lrsOutput_[0])[0] == 2 || (_lrsOutput_[0])[0] == -2)
                     && (_lrsOutput_[0])[1] == 0)
                       ? 1L
                       : 0L)) {
              //  _coutOn_();
              /*for ( decltype(Q->n) i = 0; i < Q->n; i++ )
                pmp ("", output[i]);*/
              GUM_ERROR(FatalError,
                        "LRSWrapper< GUM_SCALAR >::H2V : asked for "
                        "Q-hull computation or not reading a vertex !");
            } else
              for (decltype(_dat_->n) i = 1, end = _dat_->n; i < end; i++)
                _getLRSWrapperOutput_(_lrsOutput_[i], _lrsOutput_[0], Num, Den);
          }
      } while (lrs_getnextbasis(&_dic_, _dat_, 0L));

      auto                      vtx = Num.size();
      std::vector< GUM_SCALAR > vertex(_card_);

      for (decltype(vtx) i = 1; i <= vtx; i++) {
        vertex[(i - 1) % _card_] = GUM_SCALAR(Num[i - 1] * 1.0 / Den[i - 1]);

        if (i % _card_ == 0) {
          _output_.push_back(vertex);
          _vertices_++;
        }
      }

      _freeLrs_();

      //  _coutOn_();
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::V2H() {
      if (!_state_ == _states_::V2Hready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::V2H : fillV has "
                  "not been called with all vertices, current "
                  "state is still : "
                     << _setUpStateNames_[_state_]);
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::computeVolume() {
      if (!_state_ == _states_::V2Hready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::computeVolume : "
                  "volume is only for V-representation or "
                  "fillV has not been called with all "
                  "vertices, current state is still : "
                     << _setUpStateNames_[_state_]);

      //  _coutOff_();

      _getVolume_ = true;

      _initLrs_();

      do {
        for (decltype(_dic_->d) col = 0, end = _dic_->d; col <= end; col++)
          lrs_getsolution(_dic_, _dat_, _lrsOutput_, col);
      } while (lrs_getnextbasis(&_dic_, _dat_, 0L));

      int64_t Nsize = (_dat_->Nvolume[0] > 0) ? _dat_->Nvolume[0] : -_dat_->Nvolume[0];
      int64_t Dsize = (_dat_->Dvolume[0] > 0) ? _dat_->Dvolume[0] : -_dat_->Dvolume[0];

      int64_t num = 0L, den = 0L;
      int64_t tmp;

      for (decltype(Nsize) i = Nsize - 1; i > 0; i--) {
        tmp = _dat_->Nvolume[i];

        for (decltype(i) j = 1; j < i; j++)
          tmp *= BASE;

        num += tmp;
      }

      for (decltype(Dsize) i = Dsize - 1; i > 0; i--) {
        tmp = _dat_->Dvolume[i];

        for (decltype(i) j = 1; j < i; j++)
          tmp *= BASE;

        den += tmp;
      }

      _volume_ = num * 1.0 / den;

      _freeLrs_();

      //  _coutOn_();
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::elimRedundVrep() {
      if (_state_ != _states_::V2Hready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >::elimRedundVrep : only for "
                  "V-representation or fillV has not been called with all vertices, "
                  "current state is still : "
                     << _setUpStateNames_[static_cast< int >(_state_)]);

      //  _coutOff_();

      _initLrs_();

      int64_t* redineq; /* redineq[i]=0 if ineq i non-red,1 if red,2 linearity  */

      /*********************************************************************************/
      /* Test each row of the dictionary to see if it is redundant */
      /*********************************************************************************/

      /* note some of these may have been changed in getting initial dictionary
       */
      auto m = _dic_->m_A;
      auto d = _dic_->d;
      /* number of linearities in input */ /* should be 0 ! */
      auto nlinearity = _dat_->nlinearity;
      auto lastdv     = _dat_->lastdv;

      /* linearities are not considered for redundancy */
      redineq = (int64_t*)calloc(std::size_t(m + 1), sizeof(int64_t));

      for (decltype(nlinearity) i = 0; i < nlinearity; i++)
        redineq[_dat_->linearity[i]] = 2L;

      /* rows 0..lastdv are cost, decision variables, or linearities  */
      /* other rows need to be tested                                */

      for (decltype(m + d) index = lastdv + 1, end = m + d; index <= end; index++) {
        /* input inequality number of current index             */
        auto ineq = _dat_->inequality[index - lastdv]; /* the input inequality number
                                                         corr. to this index */

        redineq[ineq] = checkindex(_dic_, _dat_, index);
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

      // __vertices = nredund;
      // __output = std::vector< std::vector< GUM_SCALAR > > ( nredund,
      // std::vector<
      // GUM_SCALAR > (  _dat_->n - 1 ) );

      for (decltype(m) i = 1; i <= m; i++)
        if (redineq[i] == 0)
          _output_.push_back(std::vector< GUM_SCALAR >(++_input_[std::size_t(i - 1)].begin(),
                                                       _input_[std::size_t(i - 1)].end()));

      _vertices_ = (unsigned int)_output_.size();

      _freeLrs_();

      //  _coutOn_();
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::_getLRSWrapperOutput_(lrs_mp                  Nin,
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
    void LRSWrapper< GUM_SCALAR >::_fill_() const {
      std::size_t cols = _input_[0].size();

      int64_t* num = new int64_t[cols];   // ISO C++ forbids variable length array,
                                          // we need to do this instead
      int64_t* den = new int64_t[cols];

      int64_t rows = int64_t(_input_.size());

      int64_t numerator, denominator;

      for (int64_t row = 0; row < rows; row++) {
        for (std::size_t col = 0; col < cols; col++) {
          Rational< GUM_SCALAR >::continuedFracFirst(numerator,
                                                     denominator,
                                                     _input_[std::size_t(row)][col]);

          num[col] = numerator;
          den[col] = denominator;
        }

        /* GE is inequality, EQ is equation */
        /* 1L, 0L respectively */
        lrs_set_row(_dic_,
                    _dat_,
                    int64_t(row + 1),
                    num,
                    den,
                    1L);   // do NOT forget this + 1 on row
      }

      delete[] num;
      delete[] den;
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::_initLrs_() {
      if (_state_ != _states_::H2Vready && _state_ != _states_::V2Hready)
        GUM_ERROR(OperationNotAllowed,
                  "LRSWrapper< GUM_SCALAR >:: _initLrs_ : not ready, current state "
                  "is still : "
                     << _setUpStateNames_[static_cast< int >(_state_)]);

      // __coutOff();

      std::string         name = "\n*LrsWrapper:";
      std::vector< char > chars(name.c_str(), name.c_str() + name.size() + 1u);
      // use &chars[0] as a char*

      if (!lrs_init(&chars[0])) {
        //  _coutOn_();
        GUM_ERROR(FatalError, "LRSWrapper< GUM_SCALAR >:: _initLrs_ : failed lrs_init")
      }

      name  = "LRSWrapper globals";
      chars = std::vector< char >(name.c_str(), name.c_str() + name.size() + 1u);

      _dat_ = lrs_alloc_dat(&chars[0]);

      if (_dat_ == nullptr) {
        //  _coutOn_();
        GUM_ERROR(FatalError, "LRSWrapper< GUM_SCALAR >:: _initLrs_ : failed lrs_alloc_dat")
      }

      _dat_->n = Size(_input_[0].size());
      _dat_->m = Size(_input_.size());

      _dat_->getvolume = (_getVolume_) ? 1L : 0L;
      _dat_->hull      = (_hull_) ? 1L : 0L;
      _dat_->polytope  = (_polytope_) ? 1L : 0L;

      _lrsOutput_ = lrs_alloc_mp_vector(_dat_->n);

      _dic_ = lrs_alloc_dic(_dat_);

      if (_dic_ == nullptr) {
        //  _coutOn_();
        GUM_ERROR(FatalError, "LRSWrapper< GUM_SCALAR >:: _initLrs_ : failed lrs_alloc_dic")
      }

      _fill_();

      /* Pivot to a starting dictionary */
      if (!lrs_getfirstbasis(&_dic_, _dat_, &_Lin_, 0L)) {
        //  _coutOn_();
        GUM_ERROR(FatalError, "LRSWrapper< GUM_SCALAR >:: _initLrs_ : failed lrs_getfirstbasis");
      }

      /* There may have been column redundancy */
      /* If so the linearity space is obtained and redundant */
      /* columns are removed. User can access linearity space */
      /* from lrs_mp_matrix Lin dimensions nredundcol x d+1  */

      decltype(_dat_->nredundcol) startcol = 0;

      if (_dat_->homogeneous && _dat_->hull) {
        startcol++; /* col zero not treated as redundant   */

        if (!_dat_->restart) {
          //  _coutOn_();

          for (decltype(_dat_->nredundcol) col = startcol; col < _dat_->nredundcol; col++)
            lrs_printoutput(_dat_, _Lin_[col]);

          GUM_ERROR(FatalError, "LRSWrapper< GUM_SCALAR >:: _initLrs_ : redundant columns !")
        }
      }
      /*
  if (  _dat_->nredundcol > 0 ) {
     _coutOn_();

            for ( decltype(  _dat_->nredundcol ) col = 0, end =
   _dat_->nredundcol;
          col < end;
          col++ )
      lrs_printoutput(  _dat_,  _Lin_[col] );

    GUM_ERROR(
        FatalError,
        "LRSWrapper< GUM_SCALAR >:: _initLrs_ : redundant columns !" );
  }
  */
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::_freeLrs_() {
      /* free space : do not change order of next lines! */

      lrs_clear_mp_vector(_lrsOutput_, _dat_->n);

      if (_dat_->nredundcol > 0) lrs_clear_mp_matrix(_Lin_, _dat_->nredundcol, _dat_->n);

      if (_dat_->runs > 0) {
        free(_dat_->isave);
        free(_dat_->jsave);
      }

      auto savem = _dic_->m; /* need this to clear  _dat_*/

      lrs_free_dic(_dic_, _dat_); /* deallocate lrs_dic */

      _dat_->m = savem;
      lrs_free_dat(_dat_);

      std::string         name = "LrsWrapper:";
      std::vector< char > chars(name.c_str(), name.c_str() + name.size() + 1u);

      lrs_close(&chars[0]);

      //  _coutOn_();
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::_coutOff_() const {
      fflush(stdout);
#ifdef _MSC_VER
      freopen("NUL", "w", stdout);
#else    // _MSC_VER
      _oldCout_ = dup(1);

      int new_cout = open("/dev/null", O_WRONLY);
      dup2(new_cout, 1);
      close(new_cout);
#endif   // _MSC_VER
    }

    template < typename GUM_SCALAR >
    void LRSWrapper< GUM_SCALAR >::_coutOn_() const {
      fflush(stdout);
#ifdef _MSC_VER
      freopen("CON", "w", stdout);
#else    // _MSC_VER
      dup2(_oldCout_, 1);
      close(_oldCout_);
#endif   // _MSC_VER
    }

  }   // namespace credal
}   // namespace gum
