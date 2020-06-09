
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
       * class LpCol
       */

      LpCol::LpCol(unsigned int id) : id__(id) { GUM_CONSTRUCTOR(LpCol); }

      LpCol::LpCol(const LpCol& col) : id__(col.id__) { GUM_CONS_CPY(LpCol); }

      LpCol::~LpCol() { GUM_DESTRUCTOR(LpCol); }

      unsigned int LpCol::id() const { return id__; }

      bool LpCol::operator<(const LpCol& col) const { return (id__ < col.id()); }

      bool LpCol::operator==(const LpCol& col) const { return (id__ == col.id()); }

      bool LpCol::operator!=(const LpCol& col) const { return (id__ != col.id()); }

      LpCol& LpCol::operator=(const LpCol& col) {
        id__ = col.id__;

        return *this;
      }

      std::ostream& operator<<(std::ostream& out, const LpCol& col) {
        out << col.toString();
        return out;
      }

      std::string LpCol::toString() const { return "V" + std::to_string(id__); }
    }   // namespace lp

  }   // namespace credal


  namespace credal {
    namespace lp {

      /**
       * class LpExpr
       */

      LpExpr::LpExpr() :
          ileft__(false), imiddle__(false), iright__(false), lValue__(0.),
          mValue__(0.), rValue__(0.), lCoeffs__(new HashTable< LpCol, double >()),
          mCoeffs__(new HashTable< LpCol, double >()),
          rCoeffs__(new HashTable< LpCol, double >()) {
        GUM_CONSTRUCTOR(LpExpr);
      }

      LpExpr::LpExpr(const LpExpr& expr) :
          ileft__(expr.ileft__), imiddle__(expr.imiddle__),
          iright__(expr.iright__), lValue__(expr.lValue__),
          mValue__(expr.mValue__), rValue__(expr.rValue__),
          lCoeffs__(new HashTable< LpCol, double >(*expr.lCoeffs__)),
          mCoeffs__(new HashTable< LpCol, double >(*expr.mCoeffs__)),
          rCoeffs__(new HashTable< LpCol, double >(*expr.rCoeffs__)) {
        GUM_CONS_CPY(LpExpr);
      }

      LpExpr::LpExpr(const LpExpr& expr,
                     bool          copyLeft,
                     bool          copyMiddle,
                     bool          copyRight) :
          ileft__(false),
          imiddle__(false), iright__(false), lValue__(0.), mValue__(0.),
          rValue__(0.), lCoeffs__(nullptr), mCoeffs__(nullptr),
          rCoeffs__(nullptr) {
        if (copyLeft) {
          lCoeffs__ = new HashTable< LpCol, double >(*expr.lCoeffs__);
          lValue__ = expr.lValue__;
          ileft__ = true;
        } else
          lCoeffs__ = new HashTable< LpCol, double >();

        if (copyMiddle) {
          mCoeffs__ = new HashTable< LpCol, double >(*expr.mCoeffs__);
          mValue__ = expr.mValue__;
          imiddle__ = true;
        } else
          mCoeffs__ = new HashTable< LpCol, double >();

        if (copyRight) {
          rCoeffs__ = new HashTable< LpCol, double >(*expr.rCoeffs__);
          rValue__ = expr.rValue__;
          iright__ = true;
        } else
          rCoeffs__ = new HashTable< LpCol, double >();

        GUM_CONS_CPY(LpExpr);
      }

      LpExpr::LpExpr(LpExpr&& expr) :
          ileft__(expr.ileft__), imiddle__(expr.imiddle__),
          iright__(expr.iright__), lValue__(expr.lValue__),
          mValue__(expr.mValue__), rValue__(expr.rValue__),
          lCoeffs__(expr.lCoeffs__), mCoeffs__(expr.mCoeffs__),
          rCoeffs__(expr.rCoeffs__) {
        expr.lCoeffs__ = nullptr;
        expr.mCoeffs__ = nullptr;
        expr.rCoeffs__ = nullptr;

        GUM_CONS_CPY(LpExpr);
      }

      LpExpr::LpExpr(LpExpr&& expr,
                     bool     copyLeft,
                     bool     copyMiddle,
                     bool     copyRight) :
          ileft__(false),
          imiddle__(false), iright__(false), lValue__(0.), mValue__(0.),
          rValue__(0.), lCoeffs__(nullptr), mCoeffs__(nullptr),
          rCoeffs__(nullptr) {
        if (copyLeft) {
          swap(lCoeffs__, expr.lCoeffs__);
          lValue__ = expr.lValue__;
          ileft__ = true;
        } else
          lCoeffs__ = new HashTable< LpCol, double >();

        if (copyMiddle) {
          swap(mCoeffs__, expr.mCoeffs__);
          mValue__ = expr.mValue__;
          imiddle__ = true;
        } else
          mCoeffs__ = new HashTable< LpCol, double >();

        if (copyRight) {
          swap(rCoeffs__, expr.rCoeffs__);
          rValue__ = expr.rValue__;
          iright__ = true;
        } else
          rCoeffs__ = new HashTable< LpCol, double >();

        GUM_CONS_CPY(LpExpr);
      }

      LpExpr::~LpExpr() {
        delete lCoeffs__;
        delete mCoeffs__;
        delete rCoeffs__;

        GUM_DESTRUCTOR(LpExpr);
      }

      LpExpr& LpExpr::operator=(const LpCol& rhs) {
        clear();

        mCoeffs__->insert(rhs, 1.);
        imiddle__ = true;

        return *this;
      }

      LpExpr& LpExpr::operator=(const LpExpr& rhs) {
        /// self assignment check to avoid hashTable copies
        if (this == &rhs) return *this;

        *lCoeffs__ = *rhs.lCoeffs__;
        *mCoeffs__ = *rhs.mCoeffs__;
        *rCoeffs__ = *rhs.rCoeffs__;

        lValue__ = rhs.lValue__;
        mValue__ = rhs.mValue__;
        rValue__ = rhs.rValue__;

        ileft__ = rhs.ileft__;
        imiddle__ = rhs.imiddle__;
        iright__ = rhs.iright__;

        return *this;
      }

      LpExpr& LpExpr::operator=(LpExpr&& rhs) {
        /// self assignment check to avoid hashTable copies
        if (this == &rhs) return *this;

        swap(lCoeffs__, rhs.lCoeffs__);
        swap(mCoeffs__, rhs.mCoeffs__);
        swap(rCoeffs__, rhs.rCoeffs__);

        lValue__ = rhs.lValue__;
        mValue__ = rhs.mValue__;
        rValue__ = rhs.rValue__;

        ileft__ = rhs.ileft__;
        imiddle__ = rhs.imiddle__;
        iright__ = rhs.iright__;

        return *this;
      }

      template < typename SCALAR >
      LpExpr& LpExpr::operator=(const SCALAR& rhs) {
        clear();

        mValue__ = rhs;
        imiddle__ = true;

        return *this;
      }

      LpExpr& LpExpr::operator+=(const LpCol& rhs) {
        if (ileft__ || iright__)
          GUM_ERROR(OperationNotAllowed,
                    "expr::operator+= (expr) : <= present on one side of expr");

        if (!imiddle__) imiddle__ = true;

        mCoeffs__->getWithDefault(rhs, 0.) += 1.;

        return *this;
      }

      LpExpr& LpExpr::operator+=(const LpExpr& rhs) {
        if (ileft__ || iright__ || rhs.ileft__ || rhs.iright__)
          GUM_ERROR(OperationNotAllowed,
                    "expr::operator+= (rhs) : <= present "
                    "on one side of rhs and/or expr");

        if (!imiddle__) imiddle__ = true;

        for (const auto& elt: *rhs.mCoeffs__)
          mCoeffs__->getWithDefault(elt.first, 0.) += elt.second;

        mValue__ += rhs.mValue__;

        return *this;
      }

      LpExpr& LpExpr::operator+=(LpExpr&& rhs) {
        if (ileft__ || iright__ || rhs.ileft__ || rhs.iright__)
          GUM_ERROR(OperationNotAllowed,
                    "expr::operator+= (rhs) : <= present "
                    "on one side of rhs and/or expr");

        if (!imiddle__) {
          imiddle__ = true;

          mValue__ = rhs.mValue__;

          swap(mCoeffs__, rhs.mCoeffs__);

          return *this;
        }

        for (const auto& elt: *rhs.mCoeffs__)
          mCoeffs__->getWithDefault(elt.first, 0.) += elt.second;

        mValue__ += rhs.mValue__;

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

      LpExpr& LpExpr::operator-=(const LpCol& rhs) {
        if (ileft__ || iright__)
          GUM_ERROR(OperationNotAllowed,
                    "expr::operator-= (rhs) : <= present in one of expr");

        if (!imiddle__) imiddle__ = true;

        mCoeffs__->getWithDefault(rhs, 0.) -= 1.;

        return *this;
      }

      LpExpr& LpExpr::operator-=(const LpExpr& rhs) {
        if (ileft__ || iright__ || rhs.ileft__ || rhs.iright__)
          GUM_ERROR(
             OperationNotAllowed,
             "expr::operator-= (rhs) : <= present in one of rhs and/or expr");

        if (!imiddle__) imiddle__ = true;

        for (const auto& elt: *rhs.mCoeffs__)
          mCoeffs__->getWithDefault(elt.first, 0.) -= elt.second;

        mValue__ -= rhs.mValue__;

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

      std::ostream& operator<<(std::ostream& out, const LpExpr& expr) {
        out << expr.toString();
        return out;
      }

      void LpExpr::addSide__(const LpCol& from) {
        if (!ileft__) {
          lCoeffs__->insert(from, 1.);
          ileft__ = true;
        } else if (!imiddle__) {
          mCoeffs__->insert(from, 1.);
          imiddle__ = true;
        } else if (!iright__) {
          rCoeffs__->insert(from, 1.);
          iright__ = true;
        } else
          GUM_ERROR(OperationNotAllowed,
                    "LpExpr::setSide ( const LpCol & from "
                    ") : too many <= ; no free side");
      }

      void LpExpr::addSide__(const LpExpr& from) {
        if (ileft__ && iright__ && from.imiddle__)
          GUM_ERROR(OperationNotAllowed,
                    "LpExpr::setSide ( const LpCol & from "
                    ") : too many <= ; no free side");

        /// nothing to do, from is an empty LpExpr
        if (!from.imiddle__) return;

        /// from only has middle side : this should be empty or has left side,
        /// or has
        /// left and middle side
        if (!from.ileft__ && !from.iright__) {
          if (!ileft__) {
            *lCoeffs__ = *from.mCoeffs__;
            lValue__ = from.mValue__;
            ileft__ = true;

            return;
          } else if (!imiddle__) {
            *mCoeffs__ = *from.mCoeffs__;
            mValue__ = from.mValue__;
            imiddle__ = true;

            return;
          } else if (!iright__) {
            *rCoeffs__ = *from.mCoeffs__;
            rValue__ = from.mValue__;
            iright__ = true;

            return;
          } else
            GUM_ERROR(OperationNotAllowed,
                      "LpExpr::setSide ( const LpCol & from ) "
                      ": too many <= ; no free side");
        }
        /// from has left and middle side : this should be empty or has left
        /// side
        else if (from.ileft__ && !from.iright__) {
          if (!ileft__) {
            *lCoeffs__ = *from.lCoeffs__;
            lValue__ = from.lValue__;
            ileft__ = true;

            *mCoeffs__ = *from.mCoeffs__;
            mValue__ = from.mValue__;
            imiddle__ = true;

            return;
          } else if (!imiddle__ && !iright__) {
            *mCoeffs__ = *from.lCoeffs__;
            mValue__ = from.lValue__;
            imiddle__ = true;

            *rCoeffs__ = *from.mCoeffs__;
            rValue__ = from.mValue__;
            iright__ = true;

            return;
          } else
            GUM_ERROR(OperationNotAllowed,
                      "LpExpr::setSide ( const LpCol & from ) "
                      ": too many <= ; no free side");
        }
        /// from has left, middle and right side : this should be empty
        /// to be exhaustive
        else if (from.ileft__ && from.iright__) {
          if (ileft__ || imiddle__ || iright__)
            GUM_ERROR(OperationNotAllowed,
                      "LpExpr::setSide ( const LpCol & from ) "
                      ": too many <= ; no free side");

          *this = from;

          return;
        } else
          GUM_ERROR(OperationNotAllowed,
                    "LpExpr::setSide ( const LpCol & from "
                    ") : too many <= ; no free side");
      }

      void LpExpr::addSide__(LpExpr&& from) {
        /// std::cout << from;
        if (ileft__ && iright__ && from.imiddle__)
          GUM_ERROR(OperationNotAllowed,
                    "LpExpr::setSide ( const LpCol & from "
                    ") : too many <= ; no free side");

        /// nothing to do, from is an empty LpExpr
        if (!from.imiddle__) return;

        /// from only has middle side : this should be empty or has left side,
        /// or has
        /// left and middle side
        if (!from.ileft__ && !from.iright__) {
          if (!ileft__) {
            ///* lCoeffs__ = * from.mCoeffs__;
            swap(lCoeffs__, from.mCoeffs__);
            lValue__ = from.mValue__;
            ileft__ = true;

            return;
          } else if (!imiddle__) {
            ///* mCoeffs__ = * from.mCoeffs__;
            swap(mCoeffs__, from.mCoeffs__);
            mValue__ = from.mValue__;
            imiddle__ = true;

            return;
          } else if (!iright__) {
            ///* rCoeffs__ = * from.mCoeffs__;
            swap(rCoeffs__, from.mCoeffs__);
            rValue__ = from.mValue__;
            iright__ = true;

            return;
          } else
            GUM_ERROR(OperationNotAllowed,
                      "LpExpr::setSide ( const LpCol & from ) "
                      ": too many <= ; no free side");
        }
        /// from has left and middle side : this should be empty or has left
        /// side
        else if (from.ileft__ && !from.iright__) {
          if (!ileft__) {
            ///* lCoeffs__ = * from.lCoeffs__;
            swap(lCoeffs__, from.lCoeffs__);
            lValue__ = from.lValue__;
            ileft__ = true;

            ///* mCoeffs__ = * from.mCoeffs__;
            swap(mCoeffs__, from.mCoeffs__);
            mValue__ = from.mValue__;
            imiddle__ = true;

            return;
          } else if (!imiddle__ && !iright__) {
            ///* mCoeffs__ = * from.lCoeffs__;
            swap(mCoeffs__, from.lCoeffs__);
            mValue__ = from.lValue__;
            imiddle__ = true;

            ///* rCoeffs__ = * from.mCoeffs__;
            swap(rCoeffs__, from.mCoeffs__);
            rValue__ = from.mValue__;
            iright__ = true;

            return;
          } else
            GUM_ERROR(OperationNotAllowed,
                      "LpExpr::setSide ( const LpCol & from ) "
                      ": too many <= ; no free side");
        }
        /// from has left, middle and right side : this should be empty
        /// to be exhaustive
        else if (from.ileft__ && from.iright__) {
          if (ileft__ || imiddle__ || iright__)
            GUM_ERROR(OperationNotAllowed,
                      "LpExpr::setSide ( const LpCol & from ) "
                      ": too many <= ; no free side");

          *this = std::move(from);

          return;
        } else
          GUM_ERROR(OperationNotAllowed,
                    "LpExpr::setSide ( const LpCol & from "
                    ") : too many <= ; no free side");
      }

      template < typename SCALAR >
      void LpExpr::addSide__(const SCALAR& from) {
        if (!ileft__) {
          lValue__ = from;
          ileft__ = true;
        } else if (!imiddle__) {
          mValue__ = from;
          imiddle__ = true;
        } else if (!iright__) {
          rValue__ = from;
          iright__ = true;
        } else
          GUM_ERROR(OperationNotAllowed,
                    "LpExpr::setSide ( const LpCol & from "
                    ") : too many <= ; no free side");
      }

      void LpExpr::clear() {
        lCoeffs__->clear();
        mCoeffs__->clear();
        rCoeffs__->clear();

        lValue__ = 0.;
        mValue__ = 0.;
        rValue__ = 0.;

        ileft__ = false;
        imiddle__ = false;
        iright__ = false;
      }

      std::string LpExpr::toString() const {
        std::ostringstream s;

        s << std::endl << "left side : " << std::endl;

        if (lCoeffs__ != nullptr)
          for (const auto& elt: *lCoeffs__)
            s << elt.first.toString() << " " << elt.second << " | ";

        s << std::endl << "middle side : " << std::endl;

        if (mCoeffs__ != nullptr)
          for (const auto& elt: *mCoeffs__)
            s << elt.first.toString() << " " << elt.second << " | ";

        s << std::endl << "right side : " << std::endl;

        if (rCoeffs__ != nullptr)
          for (const auto& elt: *rCoeffs__)
            s << elt.first.toString() << " " << elt.second << " | ";

        s << std::endl
          << "lvalue : " << lValue__ << std::endl
          << "mvalue : " << mValue__ << std::endl
          << "rvalue : " << rValue__ << std::endl;

        s << std::endl;

        return s.str();
      }

      /**
       * class LpRow
       */

      LpRow::LpRow(const LpExpr& expr, const std::vector< LpCol >& cols) :
          coeffs__(nullptr) {
        // we write 0 <= Ax + b from Ex + f <= Cx + d
        if (expr.ileft__ && !expr.iright__) {
          coeffs__ = new HashTable< LpCol, double >(*expr.mCoeffs__);

          for (const auto& col: cols) {
            double col_coeff = 0.;

            // from left side to middle side : 0 <= middle - left
            if (expr.lCoeffs__->exists(col))
              col_coeff = expr.lCoeffs__->operator[](col);

            coeffs__->getWithDefault(col, 0.) -= col_coeff;
          }

          cste__ = expr.mValue__ - expr.lValue__;
        } else if (expr.iright__ && !expr.ileft__) {
          coeffs__ = new HashTable< LpCol, double >(*expr.rCoeffs__);

          for (const auto& col: cols) {
            double col_coeff = 0;

            // from middle side to right side : 0 <= right - middle
            if (expr.mCoeffs__->exists(col))
              col_coeff = expr.mCoeffs__->operator[](col);

            coeffs__->getWithDefault(col, 0.) -= col_coeff;
          }

          cste__ = expr.rValue__ - expr.mValue__;
        } else
          GUM_ERROR(OperationNotAllowed,
                    "expr : " << expr.toString()
                              << "is not a valid inequality; no <= detected");

        if (coeffs__->size() == 0)
          GUM_ERROR(OperationNotAllowed,
                    "expr : " << expr.toString()
                              << "is not a valid inequality; "
                                 "no variable in inequality, "
                                 "only constants");

        GUM_CONSTRUCTOR(LpRow);
      }

      LpRow::LpRow(LpExpr&& expr, const std::vector< LpCol >& cols) :
          coeffs__(nullptr) {
        /// we write 0 <= Ax + b from Ex + f <= Cx + d
        if (expr.ileft__ && !expr.iright__) {
          swap(coeffs__, expr.mCoeffs__);

          for (const auto& col: cols) {
            double col_coeff = 0;

            if (expr.lCoeffs__->exists(col))
              col_coeff = expr.lCoeffs__->operator[](col);

            coeffs__->getWithDefault(col, 0.) -= col_coeff;
          }

          cste__ = expr.mValue__ - expr.lValue__;
        } else if (expr.iright__ && !expr.ileft__) {
          swap(coeffs__, expr.rCoeffs__);

          for (const auto& col: cols) {
            double col_coeff = 0;

            if (expr.mCoeffs__->exists(col))
              col_coeff = expr.mCoeffs__->operator[](col);

            coeffs__->getWithDefault(col, 0.) -= col_coeff;
          }

          cste__ = expr.rValue__ - expr.mValue__;
        } else
          GUM_ERROR(OperationNotAllowed,
                    "expr : " << expr.toString()
                              << "is not a valid inequality; no <= detected");

        if (coeffs__->size() == 0)
          GUM_ERROR(OperationNotAllowed,
                    "expr : " << expr.toString()
                              << "is not a valid inequality; "
                                 "no variable in inequality, "
                                 "only constants");

        GUM_CONSTRUCTOR(LpRow);
      }

      LpRow::LpRow(const LpRow& row) :
          cste__(row.cste__),
          coeffs__(new HashTable< LpCol, double >(*row.coeffs__)) {
        GUM_CONS_CPY(LpRow);
      }

      LpRow::LpRow(LpRow&& row) : cste__(row.cste__), coeffs__(row.coeffs__) {
        row.coeffs__ = nullptr;

        GUM_CONS_CPY(LpRow);
      }

      LpRow::~LpRow() {
        delete coeffs__;

        GUM_DESTRUCTOR(LpRow);
      }

      LpRow& LpRow::operator=(const LpRow& row) {
        cste__ = row.cste__;
        *coeffs__ = *row.coeffs__;
        return *this;
      }

      LpRow& LpRow::operator=(LpRow&& row) {
        cste__ = row.cste__;
        swap(coeffs__, row.coeffs__);
        return *this;
      }

      std::ostream& operator<<(std::ostream& out, const LpRow& row) {
        out << row.toString();
        return out;
      }

      std::string LpRow::toString() const {
        std::ostringstream s;

        s << "0 <= " << cste__;

        if (coeffs__ != nullptr) {
          for (const auto& elt: *coeffs__) {
            if (elt.second > 0) {
              if (elt.second != 1) {
                s << " +" << elt.second << "*" << elt.first.toString();
              } else {
                s << " +" << elt.first.toString();
              }
            } else {
              if (elt.second < 0) {
                if (elt.second != -1) {
                  s << " " << elt.second << "*" << elt.first.toString();
                } else {
                  s << " -" << elt.first.toString();
                }
              }
            }
          }
        }

        return s.str();
      }

      /**
       * class LpInterface
       */
      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::LpInterface() {
        positivity__ = false;
        sumIsOne__ = false;
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

        cols__ = from.cols__;
        positivity__ = from.positivity__;
        sumIsOne__ = from.sumIsOne__;

        return *this;
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >&
         LpInterface< GUM_SCALAR >::operator=(LpInterface< GUM_SCALAR >&& from) {
        rows__.swap(from.rows__);
        cols__.swap(from.cols__);

        positivity__ = from.positivity__;
        sumIsOne__ = from.sumIsOne__;

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
          rexpr.mValue__ = lexpr.mValue__;
          rexpr.imiddle__ = true;

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

        sumIsOne__ = true;
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
        sumIsOne__ = false;
      }

      template < typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::clearRows() {
        for (const auto& row: rows__)
          delete row;

        rows__.clear();
        rows__.shrink_to_fit();

        positivity__ = false;
        sumIsOne__ = false;
      }

      ///////////////////////////////////////////////////////
      void swap(HashTable< LpCol, double >*& a, HashTable< LpCol, double >*& b) {
        HashTable< LpCol, double >* tmp(a);
        a = b;
        b = tmp;
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
