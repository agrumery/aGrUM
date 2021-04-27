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

        mValue__  = rhs;
        imiddle__ = true;

        return *this;
      }

      template < typename T >
      LpExpr& LpExpr::operator+=(const T& rhs) {
        if (ileft__ || iright__)
          GUM_ERROR(OperationNotAllowed,
                    "expr::operator+= (expr) : <= present on one side of expr");

        if (!imiddle__) imiddle__ = true;

        mValue__ += rhs;

        return *this;
      }

      template < typename T >
      LpExpr& LpExpr::operator-=(const T& rhs) {
        if (ileft__ || iright__)
          GUM_ERROR(OperationNotAllowed,
                    "expr::operator-= (rhs) : <= present in one of expr");

        if (!imiddle__) imiddle__ = true;

        mValue__ -= rhs;

        return *this;
      }

      template < typename SCALAR >
      void LpExpr::addSide__(const SCALAR& from) {
        if (!ileft__) {
          lValue__ = from;
          ileft__  = true;
        } else if (!imiddle__) {
          mValue__  = from;
          imiddle__ = true;
        } else if (!iright__) {
          rValue__ = from;
          iright__ = true;
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
        positivity__ = false;
        sumIsOne__   = false;
        GUM_CONSTRUCTOR(LpInterface);
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::LpInterface(
         const LpInterface< GUM_SCALAR >& from) :
          cols__(from.cols__),
          positivity__(from.positivity__), sumIsOne__(from.sumIsOne__) {
        rows__.resize(from.rows__.size());

        for (unsigned int i = 0, end = from.rows__.size(); i < end; i++)
          rows__[i] = new LpRow(*from.rows__[i]);

        GUM_CONS_CPY(LpInterface);
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::LpInterface(LpInterface< GUM_SCALAR >&& from) :
          positivity__(from.positivity__), sumIsOne__(from.sumIsOne__) {
        rows__.swap(from.rows__);
        cols__.swap(from.cols__);
        GUM_CONS_CPY(LpInterface);
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::~LpInterface() {
        for (const auto row: rows__)
          delete row;

        GUM_DESTRUCTOR(LpInterface);
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >& LpInterface< GUM_SCALAR >::operator=(
         const LpInterface< GUM_SCALAR >& from) {
        /// faster than clear (), delete only rows
        for (const auto& row: rows__)
          delete row;

        rows__.clear();
        rows__.shrink_to_fit();

        rows__.resize(from.rows__.size());

        for (unsigned int i = 0, end = from.rows__.size(); i < end; i++)
          rows__[i] = new LpRow(*from.rows__[i]);

        cols__       = from.cols__;
        positivity__ = from.positivity__;
        sumIsOne__   = from.sumIsOne__;

        return *this;
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >&
         LpInterface< GUM_SCALAR >::operator=(LpInterface< GUM_SCALAR >&& from) {
        rows__.swap(from.rows__);
        cols__.swap(from.cols__);

        positivity__ = from.positivity__;
        sumIsOne__   = from.sumIsOne__;

        return *this;
      }

      template < typename T >
      std::ostream& operator<<(std::ostream& out, const LpInterface< T >& lpi) {
        out << lpi.toString();
        return out;
      }

      template < typename GUM_SCALAR >
      LpCol LpInterface< GUM_SCALAR >::addCol() {
        LpCol col((unsigned int)cols__.size());

        cols__.push_back(col);

        return col;
      }

      template < typename GUM_SCALAR >
      std::vector< LpCol >
         LpInterface< GUM_SCALAR >::addCols(const unsigned int& cols) {
        if (cols < 1)
          GUM_ERROR(OperationNotAllowed,
                    "LpInterface::addCols ( cols ) : cols "
                    "needs must be equal or greater than 1 : "
                       << cols << " < 1");

        for (unsigned int i = 0; i < cols; i++) {
          cols__.push_back(LpCol((unsigned int)cols__.size()));
        }

        return cols__;
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addRow(const LpExpr& expr) {
        if (!expr.ileft__ && !expr.iright__)
          GUM_ERROR(OperationNotAllowed,
                    "addRow ( const LpExpr & expr ) : expr : "
                       << expr.toString() << "is not an inequality.");

        if ((expr.ileft__ && !expr.iright__) || (!expr.ileft__ && expr.iright__)) {
          rows__.push_back(new LpRow(expr, cols__));
        } else {
          LpExpr lexpr(expr, true, true, false);
          LpExpr rexpr(expr, false, true, true);

          rows__.push_back(
             new LpRow(std::move(lexpr),
                       cols__));   /// lexpr not used anymore, use move constructor
          rows__.push_back(
             new LpRow(std::move(rexpr),
                       cols__));   /// rexpr not used anymore, use move constructor
        }
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addRow(LpExpr&& expr) {
        if (!expr.ileft__ && !expr.iright__)
          GUM_ERROR(OperationNotAllowed,
                    "addRow ( const LpExpr & expr ) : expr : "
                       << expr.toString() << "is not an inequality.");

        if ((expr.ileft__ && !expr.iright__) || (!expr.ileft__ && expr.iright__)) {
          rows__.push_back(new LpRow(std::move(expr), cols__));
        } else {
          LpExpr lexpr(std::move(expr), true, true, false);

          /// expr pointers on maps now are nullptr except right side
          LpExpr rexpr(std::move(expr), false, false, true);

          /// rexpr miss middle side, copy it from lexpr

          *rexpr.mCoeffs__ = *lexpr.mCoeffs__;
          rexpr.mValue__   = lexpr.mValue__;
          rexpr.imiddle__  = true;

          rows__.push_back(
             new LpRow(std::move(lexpr),
                       cols__));   /// lexpr not used anymore, use move constructor
          rows__.push_back(
             new LpRow(std::move(rexpr),
                       cols__));   /// rexpr not used anymore, use move constructor
        }
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addPositivity() {
        if (positivity__) return;

        for (const auto& col: cols__)
          addRow(0 <= col);

        positivity__ = true;
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addSumIsOne() {
        if (sumIsOne__) return;

        LpExpr expr;

        for (const auto& col: cols__)
          expr += col;

        addRow(1 <= std::move(expr) <= 1);

        sumIsOne__ = true;
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addProba() {
        if (positivity__ && sumIsOne__) {
          return;
        } else if (positivity__ && !sumIsOne__) {
          addSumIsOne();
          return;
        } else if (!positivity__ && sumIsOne__) {
          addPositivity();
          return;
        }

        // we can do both with one loop, don't call the above functions.
        // addPositivity();
        // addSumIsOne();
        LpExpr expr;

        for (const auto& col: cols__) {
          addRow(0 <= col);
          expr += col;
        }

        addRow(1 <= std::move(expr) <= 1);

        sumIsOne__   = true;
        positivity__ = true;
      }

      template < typename GUM_SCALAR >
      std::vector< std::vector< GUM_SCALAR > > LpInterface< GUM_SCALAR >::solve() {
        LRSWrapper< GUM_SCALAR > lrs;

        lrs.setUpH((unsigned int)cols__.size());

        std::vector< std::vector< GUM_SCALAR > > lrsMatrix;

        for (const auto& row: rows__) {
          std::vector< GUM_SCALAR > expandedRow(cols__.size() + 1, 0);

          expandedRow[0] = row->cste__;

          for (const auto& elt: *row->coeffs__)
            expandedRow[elt.first.id() + 1] = elt.second;

          lrsMatrix.push_back(expandedRow);
        }

        lrs.fillMatrix(lrsMatrix);

        lrs.H2V();

        return lrs.getOutput();
      }

      template < typename GUM_SCALAR >
      std::vector< LpCol > LpInterface< GUM_SCALAR >::getCols() const {
        return cols__;
      }

      template < typename GUM_SCALAR >
      std::string LpInterface< GUM_SCALAR >::toString() const {
        std::ostringstream s;

        s << std::endl << std::endl << "Variables : " << std::endl;

        for (const auto& col: cols__)
          s << " " << col.toString();

        s << std::endl;

        for (const auto& row: rows__)
          s << std::endl << row->toString();

        s << std::endl << std::endl;

        return s.str();
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::clear() {
        for (const auto& row: rows__)
          delete row;

        rows__.clear();
        rows__.shrink_to_fit();   /// to really clear content memory, otherwise
                                  /// we have
                                  /// to wait for (*this) destruction ???
        /// checked with sizeof( rows__ ) + sizeof( LpRow ) * rows__.capacity()

        cols__.clear();
        cols__.shrink_to_fit();

        positivity__ = false;
        sumIsOne__   = false;
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::clearRows() {
        for (const auto& row: rows__)
          delete row;

        rows__.clear();
        rows__.shrink_to_fit();

        positivity__ = false;
        sumIsOne__   = false;
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

      template < typename T1,
                 forbidden_type< T1, LpExpr >,
                 forbidden_type< T1, LpCol > >
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

      template < typename T1,
                 forbidden_type< T1, LpExpr >,
                 forbidden_type< T1, LpCol > >
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
        expr.mCoeffs__->insert(rhs, lhs);
        expr.imiddle__ = true;
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
        expr.addSide__(std::forward< T1 >(lhs));
        expr.addSide__(std::forward< T2 >(rhs));
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

      template < typename T1,
                 forbidden_type< T1, LpExpr& >,
                 forbidden_type< T1, LpCol& > >
      LpExpr operator<=(T1&& lhs, const LpExpr& rhs) {
        return LpExpr::lessThan(std::forward< T1 >(lhs), rhs);
      }

      template < typename T1,
                 forbidden_type< T1, LpExpr& >,
                 forbidden_type< T1, LpCol& > >
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

      template < typename T1,
                 forbidden_type< T1, LpExpr >,
                 forbidden_type< T1, LpCol > >
      LpExpr operator<=(T1&& lhs, LpExpr&& rhs) {
        return LpExpr::lessThan(std::forward< T1 >(lhs), std::move(rhs));
      }

      template < typename T1,
                 forbidden_type< T1, LpExpr >,
                 forbidden_type< T1, LpCol > >
      LpExpr operator<=(T1&& lhs, LpCol&& rhs) {
        return LpExpr::lessThan(std::forward< T1 >(lhs), std::move(rhs));
      }
    }   // namespace lp

  }   // namespace credal

}   // namespace gum
