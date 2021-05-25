/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

      LpCol::LpCol(unsigned int id) : _id_(id) {
        GUM_CONSTRUCTOR(LpCol);
        ;
      }

      LpCol::LpCol(const LpCol& col) : _id_(col._id_) {
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
          _ileft_(false), _imiddle_(false), _iright_(false), _lValue_(0.), _mValue_(0.),
          _rValue_(0.), _lCoeffs_(new HashTable< LpCol, double >()),
          _mCoeffs_(new HashTable< LpCol, double >()), _rCoeffs_(new HashTable< LpCol, double >()) {
        GUM_CONSTRUCTOR(LpExpr);
      }

      LpExpr::LpExpr(const LpExpr& expr) :
          _ileft_(expr._ileft_), _imiddle_(expr._imiddle_), _iright_(expr._iright_),
          _lValue_(expr._lValue_), _mValue_(expr._mValue_), _rValue_(expr._rValue_),
          _lCoeffs_(new HashTable< LpCol, double >(*expr._lCoeffs_)),
          _mCoeffs_(new HashTable< LpCol, double >(*expr._mCoeffs_)),
          _rCoeffs_(new HashTable< LpCol, double >(*expr._rCoeffs_)) {
        GUM_CONS_CPY(LpExpr);
      }

      LpExpr::LpExpr(const LpExpr& expr, bool copyLeft, bool copyMiddle, bool copyRight) :
          _ileft_(false), _imiddle_(false), _iright_(false), _lValue_(0.), _mValue_(0.),
          _rValue_(0.), _lCoeffs_(nullptr), _mCoeffs_(nullptr), _rCoeffs_(nullptr) {
        if (copyLeft) {
          _lCoeffs_ = new HashTable< LpCol, double >(*expr._lCoeffs_);
          _lValue_  = expr._lValue_;
          _ileft_   = true;
        } else
          _lCoeffs_ = new HashTable< LpCol, double >();

        if (copyMiddle) {
          _mCoeffs_ = new HashTable< LpCol, double >(*expr._mCoeffs_);
          _mValue_  = expr._mValue_;
          _imiddle_ = true;
        } else
          _mCoeffs_ = new HashTable< LpCol, double >();

        if (copyRight) {
          _rCoeffs_ = new HashTable< LpCol, double >(*expr._rCoeffs_);
          _rValue_  = expr._rValue_;
          _iright_  = true;
        } else
          _rCoeffs_ = new HashTable< LpCol, double >();

        GUM_CONS_CPY(LpExpr);
      }

      LpExpr::LpExpr(LpExpr&& expr) :
          _ileft_(expr._ileft_), _imiddle_(expr._imiddle_), _iright_(expr._iright_),
          _lValue_(expr._lValue_), _mValue_(expr._mValue_), _rValue_(expr._rValue_),
          _lCoeffs_(expr._lCoeffs_), _mCoeffs_(expr._mCoeffs_), _rCoeffs_(expr._rCoeffs_) {
        expr._lCoeffs_ = nullptr;
        expr._mCoeffs_ = nullptr;
        expr._rCoeffs_ = nullptr;

        GUM_CONS_CPY(LpExpr);
      }

      LpExpr::LpExpr(LpExpr&& expr, bool copyLeft, bool copyMiddle, bool copyRight) :
          _ileft_(false), _imiddle_(false), _iright_(false), _lValue_(0.), _mValue_(0.),
          _rValue_(0.), _lCoeffs_(nullptr), _mCoeffs_(nullptr), _rCoeffs_(nullptr) {
        if (copyLeft) {
          swap(_lCoeffs_, expr._lCoeffs_);
          _lValue_ = expr._lValue_;
          _ileft_  = true;
        } else
          _lCoeffs_ = new HashTable< LpCol, double >();

        if (copyMiddle) {
          swap(_mCoeffs_, expr._mCoeffs_);
          _mValue_  = expr._mValue_;
          _imiddle_ = true;
        } else
          _mCoeffs_ = new HashTable< LpCol, double >();

        if (copyRight) {
          swap(_rCoeffs_, expr._rCoeffs_);
          _rValue_ = expr._rValue_;
          _iright_ = true;
        } else
          _rCoeffs_ = new HashTable< LpCol, double >();

        GUM_CONS_CPY(LpExpr);
      }

      LpExpr::~LpExpr() {
        delete _lCoeffs_;
        delete _mCoeffs_;
        delete _rCoeffs_;

        GUM_DESTRUCTOR(LpExpr);
      }

      LpExpr& LpExpr::operator=(const LpCol& rhs) {
        clear();

        _mCoeffs_->insert(rhs, 1.);
        _imiddle_ = true;

        return *this;
      }

      LpExpr& LpExpr::operator=(const LpExpr& rhs) {
        /// self assignment check to avoid hashTable copies
        if (this == &rhs) return *this;

        *_lCoeffs_ = *rhs._lCoeffs_;
        *_mCoeffs_ = *rhs._mCoeffs_;
        *_rCoeffs_ = *rhs._rCoeffs_;

        _lValue_ = rhs._lValue_;
        _mValue_ = rhs._mValue_;
        _rValue_ = rhs._rValue_;

        _ileft_   = rhs._ileft_;
        _imiddle_ = rhs._imiddle_;
        _iright_  = rhs._iright_;

        return *this;
      }

      LpExpr& LpExpr::operator=(LpExpr&& rhs) {
        /// self assignment check to avoid hashTable copies
        if (this == &rhs) return *this;

        swap(_lCoeffs_, rhs._lCoeffs_);
        swap(_mCoeffs_, rhs._mCoeffs_);
        swap(_rCoeffs_, rhs._rCoeffs_);

        _lValue_ = rhs._lValue_;
        _mValue_ = rhs._mValue_;
        _rValue_ = rhs._rValue_;

        _ileft_   = rhs._ileft_;
        _imiddle_ = rhs._imiddle_;
        _iright_  = rhs._iright_;

        return *this;
      }


      LpExpr& LpExpr::operator+=(const LpCol& rhs) {
        if (_ileft_ || _iright_)
          GUM_ERROR(OperationNotAllowed, "expr::operator+= (expr) : <= present on one side of expr")

        if (!_imiddle_) _imiddle_ = true;

        _mCoeffs_->getWithDefault(rhs, 0.) += 1.;

        return *this;
      }

      LpExpr& LpExpr::operator+=(const LpExpr& rhs) {
        if (_ileft_ || _iright_ || rhs._ileft_ || rhs._iright_)
          GUM_ERROR(OperationNotAllowed,
                    "expr::operator+= (rhs) : <= present "
                    "on one side of rhs and/or expr");

        if (!_imiddle_) _imiddle_ = true;

        for (const auto& elt: *rhs._mCoeffs_)
          _mCoeffs_->getWithDefault(elt.first, 0.) += elt.second;

        _mValue_ += rhs._mValue_;

        return *this;
      }

      LpExpr& LpExpr::operator+=(LpExpr&& rhs) {
        if (_ileft_ || _iright_ || rhs._ileft_ || rhs._iright_)
          GUM_ERROR(OperationNotAllowed,
                    "expr::operator+= (rhs) : <= present "
                    "on one side of rhs and/or expr");

        if (!_imiddle_) {
          _imiddle_ = true;
          _mValue_  = rhs._mValue_;
          swap(_mCoeffs_, rhs._mCoeffs_);

          return *this;
        }

        for (const auto& elt: *rhs._mCoeffs_)
          _mCoeffs_->getWithDefault(elt.first, 0.) += elt.second;
        _mValue_ += rhs._mValue_;

        return *this;
      }

      LpExpr& LpExpr::operator-=(const LpCol& rhs) {
        if (_ileft_ || _iright_)
          GUM_ERROR(OperationNotAllowed, "expr::operator-= (rhs) : <= present in one of expr")

        if (!_imiddle_) _imiddle_ = true;

        _mCoeffs_->getWithDefault(rhs, 0.) -= 1.;

        return *this;
      }

      LpExpr& LpExpr::operator-=(const LpExpr& rhs) {
        if (_ileft_ || _iright_ || rhs._ileft_ || rhs._iright_)
          GUM_ERROR(OperationNotAllowed,
                    "expr::operator-= (rhs) : <= present in one of rhs and/or expr");

        if (!_imiddle_) _imiddle_ = true;

        for (const auto& elt: *rhs._mCoeffs_)
          _mCoeffs_->getWithDefault(elt.first, 0.) -= elt.second;

        _mValue_ -= rhs._mValue_;

        return *this;
      }


      void LpExpr::_addSide_(const LpCol& from) {
        if (!_ileft_) {
          _lCoeffs_->insert(from, 1.);
          _ileft_ = true;
        } else if (!_imiddle_) {
          _mCoeffs_->insert(from, 1.);
          _imiddle_ = true;
        } else if (!_iright_) {
          _rCoeffs_->insert(from, 1.);
          _iright_ = true;
        } else
          GUM_ERROR(OperationNotAllowed,
                    "LpExpr::setSide ( const LpCol & from "
                    ") : too many <= ; no free side");
      }

      void LpExpr::_addSide_(const LpExpr& from) {
        if (_ileft_ && _iright_ && from._imiddle_)
          GUM_ERROR(OperationNotAllowed,
                    "LpExpr::setSide ( const LpCol & from "
                    ") : too many <= ; no free side");

        /// nothing to do, from is an empty LpExpr
        if (!from._imiddle_) return;

        /// from only has middle side : this should be empty or has left side,
        /// or has
        /// left and middle side
        if (!from._ileft_ && !from._iright_) {
          if (!_ileft_) {
            *_lCoeffs_ = *from._mCoeffs_;
            _lValue_   = from._mValue_;
            _ileft_    = true;

            return;
          } else if (!_imiddle_) {
            *_mCoeffs_ = *from._mCoeffs_;
            _mValue_   = from._mValue_;
            _imiddle_  = true;

            return;
          } else if (!_iright_) {
            *_rCoeffs_ = *from._mCoeffs_;
            _rValue_   = from._mValue_;
            _iright_   = true;

            return;
          } else
            GUM_ERROR(OperationNotAllowed,
                      "LpExpr::setSide ( const LpCol & from ) "
                      ": too many <= ; no free side");
        }
        /// from has left and middle side : this should be empty or has left
        /// side
        else if (from._ileft_ && !from._iright_) {
          if (!_ileft_) {
            *_lCoeffs_ = *from._lCoeffs_;
            _lValue_   = from._lValue_;
            _ileft_    = true;

            *_mCoeffs_ = *from._mCoeffs_;
            _mValue_   = from._mValue_;
            _imiddle_  = true;

            return;
          } else if (!_imiddle_ && !_iright_) {
            *_mCoeffs_ = *from._lCoeffs_;
            _mValue_   = from._lValue_;
            _imiddle_  = true;

            *_rCoeffs_ = *from._mCoeffs_;
            _rValue_   = from._mValue_;
            _iright_   = true;

            return;
          } else
            GUM_ERROR(OperationNotAllowed,
                      "LpExpr::setSide ( const LpCol & from ) "
                      ": too many <= ; no free side");
        }
        /// from has left, middle and right side : this should be empty
        /// to be exhaustive
        else if (from._ileft_ && from._iright_) {
          if (_ileft_ || _imiddle_ || _iright_)
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

      void LpExpr::_addSide_(LpExpr&& from) {
        /// std::cout << from;
        if (_ileft_ && _iright_ && from._imiddle_)
          GUM_ERROR(OperationNotAllowed,
                    "LpExpr::setSide ( const LpCol & from "
                    ") : too many <= ; no free side");

        /// nothing to do, from is an empty LpExpr
        if (!from._imiddle_) return;

        /// from only has middle side : this should be empty or has left side,
        /// or has
        /// left and middle side
        if (!from._ileft_ && !from._iright_) {
          if (!_ileft_) {
            ///*  _lCoeffs_ = * from. _mCoeffs_;
            swap(_lCoeffs_, from._mCoeffs_);
            _lValue_ = from._mValue_;
            _ileft_  = true;

            return;
          } else if (!_imiddle_) {
            ///*  _mCoeffs_ = * from. _mCoeffs_;
            swap(_mCoeffs_, from._mCoeffs_);
            _mValue_  = from._mValue_;
            _imiddle_ = true;

            return;
          } else if (!_iright_) {
            ///*  _rCoeffs_ = * from. _mCoeffs_;
            swap(_rCoeffs_, from._mCoeffs_);
            _rValue_ = from._mValue_;
            _iright_ = true;

            return;
          } else
            GUM_ERROR(OperationNotAllowed,
                      "LpExpr::setSide ( const LpCol & from ) "
                      ": too many <= ; no free side");
        }
        /// from has left and middle side : this should be empty or has left
        /// side
        else if (from._ileft_ && !from._iright_) {
          if (!_ileft_) {
            ///*  _lCoeffs_ = * from. _lCoeffs_;
            swap(_lCoeffs_, from._lCoeffs_);
            _lValue_ = from._lValue_;
            _ileft_  = true;

            ///*  _mCoeffs_ = * from. _mCoeffs_;
            swap(_mCoeffs_, from._mCoeffs_);
            _mValue_  = from._mValue_;
            _imiddle_ = true;

            return;
          } else if (!_imiddle_ && !_iright_) {
            ///*  _mCoeffs_ = * from. _lCoeffs_;
            swap(_mCoeffs_, from._lCoeffs_);
            _mValue_  = from._lValue_;
            _imiddle_ = true;

            ///*  _rCoeffs_ = * from. _mCoeffs_;
            swap(_rCoeffs_, from._mCoeffs_);
            _rValue_ = from._mValue_;
            _iright_ = true;

            return;
          } else
            GUM_ERROR(OperationNotAllowed,
                      "LpExpr::setSide ( const LpCol & from ) "
                      ": too many <= ; no free side");
        }
        /// from has left, middle and right side : this should be empty
        /// to be exhaustive
        else if (from._ileft_ && from._iright_) {
          if (_ileft_ || _imiddle_ || _iright_)
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
        _lCoeffs_->clear();
        _mCoeffs_->clear();
        _rCoeffs_->clear();

        _lValue_ = 0.;
        _mValue_ = 0.;
        _rValue_ = 0.;

        _ileft_   = false;
        _imiddle_ = false;
        _iright_  = false;
      }

      std::string LpExpr::toString() const {
        std::ostringstream s;

        s << std::endl << "left side : " << std::endl;

        if (_lCoeffs_ != nullptr)
          for (const auto& elt: *_lCoeffs_)
            s << elt.first.toString() << " " << elt.second << " | ";

        s << std::endl << "middle side : " << std::endl;

        if (_mCoeffs_ != nullptr)
          for (const auto& elt: *_mCoeffs_)
            s << elt.first.toString() << " " << elt.second << " | ";

        s << std::endl << "right side : " << std::endl;

        if (_rCoeffs_ != nullptr)
          for (const auto& elt: *_rCoeffs_)
            s << elt.first.toString() << " " << elt.second << " | ";

        s << std::endl
          << "lvalue : " << _lValue_ << std::endl
          << "mvalue : " << _mValue_ << std::endl
          << "rvalue : " << _rValue_ << std::endl;

        s << std::endl;

        return s.str();
      }


      /**
       * class LpRow
       */


      LpRow::LpRow(const LpExpr& expr, const std::vector< LpCol >& cols) : _coeffs_(nullptr) {
        // we write 0 <= Ax + b from Ex + f <= Cx + d
        if (expr._ileft_ && !expr._iright_) {
          _coeffs_ = new HashTable< LpCol, double >(*expr._mCoeffs_);

          for (const auto& col: cols) {
            double col_coeff = 0.;

            // from left side to middle side : 0 <= middle - left
            if (expr._lCoeffs_->exists(col)) col_coeff = expr._lCoeffs_->operator[](col);

            _coeffs_->getWithDefault(col, 0.) -= col_coeff;
          }

          _cste_ = expr._mValue_ - expr._lValue_;
        } else if (expr._iright_ && !expr._ileft_) {
          _coeffs_ = new HashTable< LpCol, double >(*expr._rCoeffs_);

          for (const auto& col: cols) {
            double col_coeff = 0;

            // from middle side to right side : 0 <= right - middle
            if (expr._mCoeffs_->exists(col)) col_coeff = expr._mCoeffs_->operator[](col);

            _coeffs_->getWithDefault(col, 0.) -= col_coeff;
          }

          _cste_ = expr._rValue_ - expr._mValue_;
        } else
          GUM_ERROR(OperationNotAllowed,
                    "expr : " << expr.toString() << "is not a valid inequality; no <= detected");

        if (_coeffs_->size() == 0)
          GUM_ERROR(OperationNotAllowed,
                    "expr : " << expr.toString()
                              << "is not a valid inequality; "
                                 "no variable in inequality, "
                                 "only constants");

        GUM_CONSTRUCTOR(LpRow);
      }

      LpRow::LpRow(LpExpr&& expr, const std::vector< LpCol >& cols) : _coeffs_(nullptr) {
        /// we write 0 <= Ax + b from Ex + f <= Cx + d
        if (expr._ileft_ && !expr._iright_) {
          swap(_coeffs_, expr._mCoeffs_);

          for (const auto& col: cols) {
            double col_coeff = 0;

            if (expr._lCoeffs_->exists(col)) col_coeff = expr._lCoeffs_->operator[](col);

            _coeffs_->getWithDefault(col, 0.) -= col_coeff;
          }

          _cste_ = expr._mValue_ - expr._lValue_;
        } else if (expr._iright_ && !expr._ileft_) {
          swap(_coeffs_, expr._rCoeffs_);

          for (const auto& col: cols) {
            double col_coeff = 0;

            if (expr._mCoeffs_->exists(col)) col_coeff = expr._mCoeffs_->operator[](col);

            _coeffs_->getWithDefault(col, 0.) -= col_coeff;
          }

          _cste_ = expr._rValue_ - expr._mValue_;
        } else
          GUM_ERROR(OperationNotAllowed,
                    "expr : " << expr.toString() << "is not a valid inequality; no <= detected");

        if (_coeffs_->size() == 0)
          GUM_ERROR(OperationNotAllowed,
                    "expr : " << expr.toString()
                              << "is not a valid inequality; "
                                 "no variable in inequality, "
                                 "only constants");

        GUM_CONSTRUCTOR(LpRow);
      }

      LpRow::LpRow(const LpRow& row) :
          _cste_(row._cste_), _coeffs_(new HashTable< LpCol, double >(*row._coeffs_)) {
        GUM_CONS_CPY(LpRow);
      }

      LpRow::LpRow(LpRow&& row) : _cste_(row._cste_), _coeffs_(row._coeffs_) {
        row._coeffs_ = nullptr;

        GUM_CONS_CPY(LpRow);
      }

      LpRow::~LpRow() {
        delete _coeffs_;

        GUM_DESTRUCTOR(LpRow);
      }

      LpRow& LpRow::operator=(const LpRow& row) {
        _cste_    = row._cste_;
        *_coeffs_ = *row._coeffs_;
        return *this;
      }

      LpRow& LpRow::operator=(LpRow&& row) {
        _cste_ = row._cste_;
        swap(_coeffs_, row._coeffs_);
        return *this;
      }

      std::ostream& operator<<(std::ostream& out, const LpRow& row) {
        out << row.toString();
        return out;
      }

      std::string LpRow::toString() const {
        std::ostringstream s;

        s << "0 <= " << _cste_;

        if (_coeffs_ != nullptr) {
          for (const auto& elt: *_coeffs_) {
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