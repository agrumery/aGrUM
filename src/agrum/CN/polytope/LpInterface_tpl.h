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


#include "LpInterface.h"

namespace gum {
  namespace credal {
    namespace lp {

      /**
       * class LpExpr
       */
      template < typename SCALAR >
      LpExpr& LpExpr::operator=(const SCALAR& rhs) {
        clear();

        _mValue_  = rhs;
        _imiddle_ = true;

        return *this;
      }

      template < typename T >
      LpExpr& LpExpr::operator+=(const T& rhs) {
        if (_ileft_ || _iright_)
          GUM_ERROR(OperationNotAllowed, "expr::operator+= (expr) : <= present on one side of expr")

        if (!_imiddle_) _imiddle_ = true;

        _mValue_ += rhs;

        return *this;
      }

      template < typename T >
      LpExpr& LpExpr::operator-=(const T& rhs) {
        if (_ileft_ || _iright_)
          GUM_ERROR(OperationNotAllowed, "expr::operator-= (rhs) : <= present in one of expr")

        if (!_imiddle_) _imiddle_ = true;

        _mValue_ -= rhs;

        return *this;
      }

      template < typename SCALAR >
      void LpExpr::_addSide_(const SCALAR& from) {
        if (!_ileft_) {
          _lValue_ = from;
          _ileft_  = true;
        } else if (!_imiddle_) {
          _mValue_  = from;
          _imiddle_ = true;
        } else if (!_iright_) {
          _rValue_ = from;
          _iright_ = true;
        } else
          GUM_ERROR(OperationNotAllowed,
                    "LpExpr::setSide ( const LpCol & from "
                    ") : too many <= ; no free side");
      }

      /**
       * class LpInterface
       */
      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::LpInterface() {
        _positivity_ = false;
        _sumIsOne_   = false;
        GUM_CONSTRUCTOR(LpInterface);
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::LpInterface(const LpInterface< GUM_SCALAR >& from) :
          _cols_(from._cols_), _positivity_(from._positivity_), _sumIsOne_(from._sumIsOne_) {
        _rows_.resize(from._rows_.size());

        for (unsigned int i = 0, end = from._rows_.size(); i < end; i++)
          _rows_[i] = new LpRow(*from._rows_[i]);

        GUM_CONS_CPY(LpInterface);
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::LpInterface(LpInterface< GUM_SCALAR >&& from) :
          _positivity_(from._positivity_), _sumIsOne_(from._sumIsOne_) {
        _rows_.swap(from._rows_);
        _cols_.swap(from._cols_);
        GUM_CONS_CPY(LpInterface);
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::~LpInterface() {
        for (const auto row: _rows_)
          delete row;

        GUM_DESTRUCTOR(LpInterface);
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >&
         LpInterface< GUM_SCALAR >::operator=(const LpInterface< GUM_SCALAR >& from) {
        /// faster than clear (), delete only rows
        for (const auto& row: _rows_)
          delete row;

        _rows_.clear();
        _rows_.shrink_to_fit();

        _rows_.resize(from._rows_.size());

        for (unsigned int i = 0, end = from._rows_.size(); i < end; i++)
          _rows_[i] = new LpRow(*from._rows_[i]);

        _cols_       = from._cols_;
        _positivity_ = from._positivity_;
        _sumIsOne_   = from._sumIsOne_;

        return *this;
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >&
         LpInterface< GUM_SCALAR >::operator=(LpInterface< GUM_SCALAR >&& from) {
        _rows_.swap(from._rows_);
        _cols_.swap(from._cols_);

        _positivity_ = from._positivity_;
        _sumIsOne_   = from._sumIsOne_;

        return *this;
      }

      template < typename T >
      std::ostream& operator<<(std::ostream& out, const LpInterface< T >& lpi) {
        out << lpi.toString();
        return out;
      }

      template < typename GUM_SCALAR >
      LpCol LpInterface< GUM_SCALAR >::addCol() {
        LpCol col((unsigned int)_cols_.size());

        _cols_.push_back(col);

        return col;
      }

      template < typename GUM_SCALAR >
      std::vector< LpCol > LpInterface< GUM_SCALAR >::addCols(const unsigned int& cols) {
        if (cols < 1)
          GUM_ERROR(OperationNotAllowed,
                    "LpInterface::addCols ( cols ) : cols "
                    "needs must be equal or greater than 1 : "
                       << cols << " < 1");

        for (unsigned int i = 0; i < cols; i++) {
          _cols_.push_back(LpCol((unsigned int)_cols_.size()));
        }

        return _cols_;
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addRow(const LpExpr& expr) {
        if (!expr._ileft_ && !expr._iright_)
          GUM_ERROR(OperationNotAllowed,
                    "addRow ( const LpExpr & expr ) : expr : " << expr.toString()
                                                               << "is not an inequality.");

        if ((expr._ileft_ && !expr._iright_) || (!expr._ileft_ && expr._iright_)) {
          _rows_.push_back(new LpRow(expr, _cols_));
        } else {
          LpExpr lexpr(expr, true, true, false);
          LpExpr rexpr(expr, false, true, true);

          _rows_.push_back(new LpRow(std::move(lexpr),
                                     _cols_));   /// lexpr not used anymore, use move constructor
          _rows_.push_back(new LpRow(std::move(rexpr),
                                     _cols_));   /// rexpr not used anymore, use move constructor
        }
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addRow(LpExpr&& expr) {
        if (!expr._ileft_ && !expr._iright_)
          GUM_ERROR(OperationNotAllowed,
                    "addRow ( const LpExpr & expr ) : expr : " << expr.toString()
                                                               << "is not an inequality.");

        if ((expr._ileft_ && !expr._iright_) || (!expr._ileft_ && expr._iright_)) {
          _rows_.push_back(new LpRow(std::move(expr), _cols_));
        } else {
          LpExpr lexpr(std::move(expr), true, true, false);

          /// expr pointers on maps now are nullptr except right side
          LpExpr rexpr(std::move(expr), false, false, true);

          /// rexpr miss middle side, copy it from lexpr

          *rexpr._mCoeffs_ = *lexpr._mCoeffs_;
          rexpr._mValue_   = lexpr._mValue_;
          rexpr._imiddle_  = true;

          _rows_.push_back(new LpRow(std::move(lexpr),
                                     _cols_));   /// lexpr not used anymore, use move constructor
          _rows_.push_back(new LpRow(std::move(rexpr),
                                     _cols_));   /// rexpr not used anymore, use move constructor
        }
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addPositivity() {
        if (_positivity_) return;

        for (const auto& col: _cols_)
          addRow(0 <= col);

        _positivity_ = true;
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addSumIsOne() {
        if (_sumIsOne_) return;

        LpExpr expr;

        for (const auto& col: _cols_)
          expr += col;

        addRow(1 <= std::move(expr) <= 1);

        _sumIsOne_ = true;
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addProba() {
        if (_positivity_ && _sumIsOne_) {
          return;
        } else if (_positivity_ && !_sumIsOne_) {
          addSumIsOne();
          return;
        } else if (!_positivity_ && _sumIsOne_) {
          addPositivity();
          return;
        }

        // we can do both with one loop, don't call the above functions.
        // addPositivity();
        // addSumIsOne();
        LpExpr expr;

        for (const auto& col: _cols_) {
          addRow(0 <= col);
          expr += col;
        }

        addRow(1 <= std::move(expr) <= 1);

        _sumIsOne_   = true;
        _positivity_ = true;
      }

      template < typename GUM_SCALAR >
      std::vector< std::vector< GUM_SCALAR > > LpInterface< GUM_SCALAR >::solve() {
        LRSWrapper< GUM_SCALAR > lrs;

        lrs.setUpH((unsigned int)_cols_.size());

        std::vector< std::vector< GUM_SCALAR > > lrsMatrix;

        for (const auto& row: _rows_) {
          std::vector< GUM_SCALAR > expandedRow(_cols_.size() + 1, 0);

          expandedRow[0] = row->_cste_;

          for (const auto& elt: *row->_coeffs_)
            expandedRow[elt.first.id() + 1] = elt.second;

          lrsMatrix.push_back(expandedRow);
        }

        lrs.fillMatrix(lrsMatrix);

        lrs.H2V();

        return lrs.getOutput();
      }

      template < typename GUM_SCALAR >
      std::vector< LpCol > LpInterface< GUM_SCALAR >::getCols() const {
        return _cols_;
      }

      template < typename GUM_SCALAR >
      std::string LpInterface< GUM_SCALAR >::toString() const {
        std::ostringstream s;

        s << std::endl << std::endl << "Variables : " << std::endl;

        for (const auto& col: _cols_)
          s << " " << col.toString();

        s << std::endl;

        for (const auto& row: _rows_)
          s << std::endl << row->toString();

        s << std::endl << std::endl;

        return s.str();
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::clear() {
        for (const auto& row: _rows_)
          delete row;

        _rows_.clear();
        _rows_.shrink_to_fit();   /// to really clear content memory, otherwise
                                  /// we have
                                  /// to wait for (*this) destruction ???
        /// checked with sizeof(  _rows_ ) + sizeof( LpRow ) *  _rows_.capacity()

        _cols_.clear();
        _cols_.shrink_to_fit();

        _positivity_ = false;
        _sumIsOne_   = false;
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::clearRows() {
        for (const auto& row: _rows_)
          delete row;

        _rows_.clear();
        _rows_.shrink_to_fit();

        _positivity_ = false;
        _sumIsOne_   = false;
      }

      ///////////////////////////////////////////////////////
      template < typename T2 >
      LpExpr operator+(LpExpr&& lhs, const T2& rhs) {
        LpExpr expr = std::move(lhs);
        expr += rhs;

        return expr;
      }

      template < typename T2 >
      LpExpr operator+(const LpExpr& lhs, const T2& rhs) {
        LpExpr expr(lhs);
        expr += rhs;

        return expr;
      }

      template < typename T1, forbidden_type< T1, LpExpr > >
      LpExpr operator+(const T1& lhs, LpExpr&& rhs) {
        LpExpr expr = std::move(rhs);
        ;
        expr += lhs;

        return expr;
      }

      template < typename T1, forbidden_type< T1, LpExpr > >
      LpExpr operator+(const T1& lhs, LpExpr& rhs) {
        LpExpr expr(rhs);
        expr += lhs;

        return expr;
      }

      template < typename T2, forbidden_type< T2, LpExpr > >
      LpExpr operator+(const LpCol& lhs, const T2& rhs) {
        LpExpr expr;
        expr += lhs;
        expr += rhs;

        return expr;
      }

      template < typename T1, forbidden_type< T1, LpExpr >, forbidden_type< T1, LpCol > >
      LpExpr operator+(const T1& lhs, const LpCol& rhs) {
        LpExpr expr;
        expr += rhs;
        expr += lhs;

        return expr;
      }

      ///////////////////////////////////////////////////////
      template < typename T2 >
      LpExpr operator-(LpExpr&& lhs, const T2& rhs) {
        LpExpr expr = std::move(lhs);
        expr -= rhs;

        return expr;
      }

      template < typename T2 >
      LpExpr operator-(const LpExpr& lhs, const T2& rhs) {
        LpExpr expr(lhs);
        expr -= rhs;

        return expr;
      }

      template < typename T1, forbidden_type< T1, LpExpr > >
      LpExpr operator-(const T1& lhs, LpExpr&& rhs) {
        LpExpr expr;
        expr += std::move(rhs);
        ;
        expr -= lhs;

        return expr;
      }

      template < typename T1, forbidden_type< T1, LpExpr > >
      LpExpr operator-(const T1& lhs, LpExpr& rhs) {
        LpExpr expr;
        expr += rhs;
        expr -= lhs;

        return expr;
      }

      template < typename T2, forbidden_type< T2, LpExpr > >
      LpExpr operator-(const LpCol& lhs, const T2& rhs) {
        LpExpr expr;
        expr += lhs;
        expr -= rhs;

        return expr;
      }

      template < typename T1, forbidden_type< T1, LpExpr >, forbidden_type< T1, LpCol > >
      LpExpr operator-(const T1& lhs, const LpCol& rhs) {
        LpExpr expr;
        expr += rhs;
        expr -= lhs;

        return expr;
      }

      ///////////////////////////////////////////////////////
      template < typename SCALAR >
      INLINE LpExpr LpExpr::multiply(const SCALAR& lhs, const LpCol& rhs) {
        LpExpr expr;
        expr._mCoeffs_->insert(rhs, lhs);
        expr._imiddle_ = true;
        return expr;
      }

      template < typename SCALAR >
      LpExpr operator*(const SCALAR& lhs, const LpCol& rhs) {
        return LpExpr::multiply(lhs, rhs);
      }

      template < typename SCALAR >
      LpExpr operator*(const LpCol& lhs, const SCALAR& rhs) {
        return LpExpr::multiply(rhs, lhs);
      }

      ///////////////////////////////////////////////////////
      template < typename T1, typename T2 >
      INLINE LpExpr LpExpr::lessThan(T1&& lhs, T2&& rhs) {
        LpExpr expr;
        expr._addSide_(std::forward< T1 >(lhs));
        expr._addSide_(std::forward< T2 >(rhs));
        return expr;
      }

      // const lvalue
      template < typename T2 >
      LpExpr operator<=(const LpExpr& lhs, T2&& rhs) {
        return LpExpr::lessThan(lhs, std::forward< T2 >(rhs));
      }

      template < typename T2 >
      LpExpr operator<=(const LpCol& lhs, T2&& rhs) {
        return LpExpr::lessThan(lhs, std::forward< T2 >(rhs));
      }

      template < typename T1, forbidden_type< T1, LpExpr& >, forbidden_type< T1, LpCol& > >
      LpExpr operator<=(T1&& lhs, const LpExpr& rhs) {
        return LpExpr::lessThan(std::forward< T1 >(lhs), rhs);
      }

      template < typename T1, forbidden_type< T1, LpExpr& >, forbidden_type< T1, LpCol& > >
      LpExpr operator<=(T1&& lhs, const LpCol& rhs) {
        return LpExpr::lessThan(std::forward< T1 >(lhs), rhs);
      }

      // rvaue
      template < typename T2 >
      LpExpr operator<=(LpExpr&& lhs, T2&& rhs) {
        return LpExpr::lessThan(std::move(lhs), std::forward< T2 >(rhs));
      }

      template < typename T2 >
      LpExpr operator<=(LpCol&& lhs, T2&& rhs) {
        return LpExpr::lessThan(std::move(lhs), std::forward< T2 >(rhs));
      }

      template < typename T1, forbidden_type< T1, LpExpr >, forbidden_type< T1, LpCol > >
      LpExpr operator<=(T1&& lhs, LpExpr&& rhs) {
        return LpExpr::lessThan(std::forward< T1 >(lhs), std::move(rhs));
      }

      template < typename T1, forbidden_type< T1, LpExpr >, forbidden_type< T1, LpCol > >
      LpExpr operator<=(T1&& lhs, LpCol&& rhs) {
        return LpExpr::lessThan(std::forward< T1 >(lhs), std::move(rhs));
      }
    }   // namespace lp

  }   // namespace credal

}   // namespace gum
