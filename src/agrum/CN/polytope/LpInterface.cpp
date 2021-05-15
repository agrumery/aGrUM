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


/**
 * @file
 * @brief Class to include at least once this header
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 */

#include <agrum/CN/polytope/LpInterface.h>

#ifdef GUM_NO_INLINE
#  include <agrum/CN/polytope/LpInterface_inl.h>
#endif /* GUM_NO_INLINE */


namespace gum {
  namespace credal {
    namespace lp {

      /**
       * class LpCol
       */

      LpCol::LpCol(unsigned int id) : id__(id) {
        GUM_CONSTRUCTOR(LpCol);
        ;
      }

      LpCol::LpCol(const LpCol& col) : id__(col.id__) {
        GUM_CONS_CPY(LpCol);
        ;
      }

      LpCol::~LpCol() {
        GUM_DESTRUCTOR(LpCol);
        ;
      }

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
          lValue__  = expr.lValue__;
          ileft__   = true;
        } else
          lCoeffs__ = new HashTable< LpCol, double >();

        if (copyMiddle) {
          mCoeffs__ = new HashTable< LpCol, double >(*expr.mCoeffs__);
          mValue__  = expr.mValue__;
          imiddle__ = true;
        } else
          mCoeffs__ = new HashTable< LpCol, double >();

        if (copyRight) {
          rCoeffs__ = new HashTable< LpCol, double >(*expr.rCoeffs__);
          rValue__  = expr.rValue__;
          iright__  = true;
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
          ileft__  = true;
        } else
          lCoeffs__ = new HashTable< LpCol, double >();

        if (copyMiddle) {
          swap(mCoeffs__, expr.mCoeffs__);
          mValue__  = expr.mValue__;
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

        ileft__   = rhs.ileft__;
        imiddle__ = rhs.imiddle__;
        iright__  = rhs.iright__;

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

        ileft__   = rhs.ileft__;
        imiddle__ = rhs.imiddle__;
        iright__  = rhs.iright__;

        return *this;
      }


      LpExpr& LpExpr::operator+=(const LpCol& rhs) {
        if (ileft__ || iright__)
          GUM_ERROR(OperationNotAllowed,
                    "expr::operator+= (expr) : <= present on one side of expr")

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
          mValue__  = rhs.mValue__;
          swap(mCoeffs__, rhs.mCoeffs__);

          return *this;
        }

        for (const auto& elt: *rhs.mCoeffs__)
          mCoeffs__->getWithDefault(elt.first, 0.) += elt.second;
        mValue__ += rhs.mValue__;

        return *this;
      }

      LpExpr& LpExpr::operator-=(const LpCol& rhs) {
        if (ileft__ || iright__)
          GUM_ERROR(OperationNotAllowed,
                    "expr::operator-= (rhs) : <= present in one of expr")

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
            lValue__   = from.mValue__;
            ileft__    = true;

            return;
          } else if (!imiddle__) {
            *mCoeffs__ = *from.mCoeffs__;
            mValue__   = from.mValue__;
            imiddle__  = true;

            return;
          } else if (!iright__) {
            *rCoeffs__ = *from.mCoeffs__;
            rValue__   = from.mValue__;
            iright__   = true;

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
            lValue__   = from.lValue__;
            ileft__    = true;

            *mCoeffs__ = *from.mCoeffs__;
            mValue__   = from.mValue__;
            imiddle__  = true;

            return;
          } else if (!imiddle__ && !iright__) {
            *mCoeffs__ = *from.lCoeffs__;
            mValue__   = from.lValue__;
            imiddle__  = true;

            *rCoeffs__ = *from.mCoeffs__;
            rValue__   = from.mValue__;
            iright__   = true;

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
            ileft__  = true;

            return;
          } else if (!imiddle__) {
            ///* mCoeffs__ = * from.mCoeffs__;
            swap(mCoeffs__, from.mCoeffs__);
            mValue__  = from.mValue__;
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
            ileft__  = true;

            ///* mCoeffs__ = * from.mCoeffs__;
            swap(mCoeffs__, from.mCoeffs__);
            mValue__  = from.mValue__;
            imiddle__ = true;

            return;
          } else if (!imiddle__ && !iright__) {
            ///* mCoeffs__ = * from.lCoeffs__;
            swap(mCoeffs__, from.lCoeffs__);
            mValue__  = from.lValue__;
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


      void LpExpr::clear() {
        lCoeffs__->clear();
        mCoeffs__->clear();
        rCoeffs__->clear();

        lValue__ = 0.;
        mValue__ = 0.;
        rValue__ = 0.;

        ileft__   = false;
        imiddle__ = false;
        iright__  = false;
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
        cste__    = row.cste__;
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

    }   // namespace lp
  }     // namespace credal
}   // namespace gum