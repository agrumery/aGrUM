#include "LpInterface.h"

namespace gum {
  namespace credal {
    namespace lp {
      /**
      * class LpCol
      */

      LpCol::LpCol ( unsigned int id ) : __id ( id ) {
        GUM_CONSTRUCTOR ( LpCol );
      }

      LpCol::LpCol ( const LpCol& col ) : __id ( col.__id ) {
        GUM_CONS_CPY ( LpCol );
      }

      LpCol::~LpCol() {
        GUM_DESTRUCTOR ( LpCol );
      }

      unsigned int LpCol::id() const {
        return __id;
      }

      bool LpCol::operator< ( const LpCol& col ) const {
        return ( __id < col.id() );
      }

      bool LpCol::operator== ( const LpCol& col ) const {
        return ( __id == col.id() );
      }

      bool LpCol::operator!= ( const LpCol& col ) const {
        return ( __id != col.id() );
      }

      LpCol& LpCol::operator= ( const LpCol& col ) {
        __id = col.__id;

        return *this;
      }

      std::ostream& operator<< ( std::ostream& out, const LpCol& col ) {
        out << col.toString();
        return out;
      }

      void LpCol::print() const {
        std::cout << "col_" << std::to_string ( __id );
      }

      std::string LpCol::toString() const {
        return "V" + std::to_string ( __id );
      }
    } // end of lp

  } // end of credal

  INLINE Size
  HashFunc< credal::lp::LpCol >::operator() ( const credal::lp::LpCol& key ) const {
    return ( ( ( Size ) key.id() ) * gum::HashFuncConst::gold ) & _hash_mask;
  }

  namespace credal {
    namespace lp {

      /**
      * class LpExpr
      */

      LpExpr::LpExpr() :
        __ileft ( false ),
        __imiddle ( false ),
        __iright ( false ),
        __lValue ( 0. ),
        __mValue ( 0. ),
        __rValue ( 0. ),
        __lCoeffs ( new HashTable< LpCol, double > () ),
        __mCoeffs ( new HashTable< LpCol, double > () ),
        __rCoeffs ( new HashTable< LpCol, double > () ) {
        GUM_CONSTRUCTOR ( LpExpr );
      }

      LpExpr::LpExpr ( const LpExpr& expr ) :
        __ileft ( expr.__ileft ),
        __imiddle ( expr.__imiddle ),
        __iright ( expr.__iright ),
        __lValue ( expr.__lValue ),
        __mValue ( expr.__mValue ),
        __rValue ( expr.__rValue ),
        __lCoeffs ( new HashTable< LpCol, double > ( * expr.__lCoeffs ) ),
        __mCoeffs ( new HashTable< LpCol, double > ( * expr.__mCoeffs ) ),
        __rCoeffs ( new HashTable< LpCol, double > ( * expr.__rCoeffs ) ) {
        GUM_CONS_CPY ( LpExpr );
      }

      LpExpr::LpExpr ( const LpExpr& expr, bool copyLeft, bool copyMiddle, bool copyRight ) :
        __ileft ( false ),
        __imiddle ( false ),
        __iright ( false ),
        __lValue ( 0. ),
        __mValue ( 0. ),
        __rValue ( 0. ),
        __lCoeffs ( nullptr ),
        __mCoeffs ( nullptr ),
        __rCoeffs ( nullptr ) {
        if ( copyLeft ) {
          __lCoeffs = new HashTable< LpCol, double > ( * expr.__lCoeffs );
          __lValue = expr.__lValue;
          __ileft = true;
        } else
          __lCoeffs = new HashTable< LpCol, double > ();

        if ( copyMiddle ) {
          __mCoeffs = new HashTable< LpCol, double > ( * expr.__mCoeffs );
          __mValue = expr.__mValue;
          __imiddle = true;
        } else
          __mCoeffs = new HashTable< LpCol, double > ();

        if ( copyRight ) {
          __rCoeffs = new HashTable< LpCol, double > ( * expr.__rCoeffs );
          __rValue = expr.__rValue;
          __iright = true;
        } else
          __rCoeffs = new HashTable< LpCol, double > ();

        GUM_CONS_CPY ( LpExpr );
      }

      LpExpr::LpExpr ( LpExpr && expr ) :
        __ileft ( expr.__ileft ),
        __imiddle ( expr.__imiddle ),
        __iright ( expr.__iright ),
        __lValue ( expr.__lValue ),
        __mValue ( expr.__mValue ),
        __rValue ( expr.__rValue ),
        __lCoeffs ( expr.__lCoeffs ),
        __mCoeffs ( expr.__mCoeffs ),
        __rCoeffs ( expr.__rCoeffs ) {
        expr.__lCoeffs = nullptr;
        expr.__mCoeffs = nullptr;
        expr.__rCoeffs = nullptr;

        GUM_CONS_CPY ( LpExpr );
      }

      LpExpr::LpExpr ( LpExpr && expr, bool copyLeft, bool copyMiddle, bool copyRight ) :
        __ileft ( false ),
        __imiddle ( false ),
        __iright ( false ),
        __lValue ( 0. ),
        __mValue ( 0. ),
        __rValue ( 0. ),
        __lCoeffs ( nullptr ),
        __mCoeffs ( nullptr ),
        __rCoeffs ( nullptr ) {
        if ( copyLeft ) {
          swap ( __lCoeffs, expr.__lCoeffs );
          __lValue = expr.__lValue;
          __ileft = true;
        } else
          __lCoeffs = new HashTable< LpCol, double > ();

        if ( copyMiddle ) {
          swap ( __mCoeffs, expr.__mCoeffs );
          __mValue = expr.__mValue;
          __imiddle = true;
        } else
          __mCoeffs = new HashTable< LpCol, double > ();

        if ( copyRight ) {
          swap ( __rCoeffs, expr.__rCoeffs );
          __rValue = expr.__rValue;
          __iright = true;
        } else
          __rCoeffs = new HashTable< LpCol, double > ();

        GUM_CONS_CPY ( LpExpr );
      }

      LpExpr::~LpExpr() {
        delete __lCoeffs;
        delete __mCoeffs;
        delete __rCoeffs;

        GUM_DESTRUCTOR ( LpExpr );
      }

      LpExpr& LpExpr::operator= ( const LpCol& rhs ) {
        clear();

        __mCoeffs->insert ( rhs, 1. );
        __imiddle = true;

        return *this;
      }

      LpExpr& LpExpr::operator= ( const LpExpr& rhs ) {
        /// self assignment check to avoid hashTable copies
        if ( this == & rhs )
          return *this;

        * __lCoeffs = * rhs.__lCoeffs;
        * __mCoeffs = * rhs.__mCoeffs;
        * __rCoeffs = * rhs.__rCoeffs;

        __lValue = rhs.__lValue;
        __mValue = rhs.__mValue;
        __rValue = rhs.__rValue;

        __ileft = rhs.__ileft;
        __imiddle = rhs.__imiddle;
        __iright = rhs.__iright;

        return *this;
      }

      LpExpr& LpExpr::operator= ( LpExpr && rhs ) {
        /// self assignment check to avoid hashTable copies
        if ( this == & rhs )
          return *this;

        swap ( __lCoeffs, rhs.__lCoeffs );
        swap ( __mCoeffs, rhs.__mCoeffs );
        swap ( __rCoeffs, rhs.__rCoeffs );

        __lValue = rhs.__lValue;
        __mValue = rhs.__mValue;
        __rValue = rhs.__rValue;

        __ileft = rhs.__ileft;
        __imiddle = rhs.__imiddle;
        __iright = rhs.__iright;

        return *this;
      }

      template< typename SCALAR >
      LpExpr& LpExpr::operator= ( const SCALAR& rhs ) {
        clear();

        __mValue = rhs;
        __imiddle = true;

        return *this;
      }

      LpExpr& LpExpr::operator+= ( const LpCol& rhs ) {
        if ( __ileft || __iright )
          GUM_ERROR ( OperationNotAllowed, "expr::operator+= (expr) : <= present on one side of expr" );

        if ( ! __imiddle )
          __imiddle = true;

        __mCoeffs->getWithDefault ( rhs, 0. ) += 1.;

        return *this;
      }

      LpExpr& LpExpr::operator+= ( const LpExpr& rhs ) {
        if ( __ileft || __iright || rhs.__ileft || rhs.__iright )
          GUM_ERROR ( OperationNotAllowed, "expr::operator+= (rhs) : <= present on one side of rhs and/or expr" );

        if ( ! __imiddle )
          __imiddle = true;

        for ( auto it = rhs.__mCoeffs->beginSafe(), end = rhs.__mCoeffs->endSafe(); it != end; ++it )
          __mCoeffs->getWithDefault ( it.key(), 0. ) += it.val();

        __mValue += rhs.__mValue;

        return *this;
      }

      LpExpr& LpExpr::operator+= ( LpExpr && rhs ) {
        if ( __ileft || __iright || rhs.__ileft || rhs.__iright )
          GUM_ERROR ( OperationNotAllowed, "expr::operator+= (rhs) : <= present on one side of rhs and/or expr" );

        if ( ! __imiddle ) {
          __imiddle = true;

          __mValue = rhs.__mValue;

          swap ( __mCoeffs, rhs.__mCoeffs );

          return *this;
        }

        for ( auto it = rhs.__mCoeffs->beginSafe(), end = rhs.__mCoeffs->endSafe(); it != end; ++it )
          __mCoeffs->getWithDefault ( it.key(), 0. ) += it.val();

        __mValue += rhs.__mValue;

        return *this;
      }

      template< typename T >
      LpExpr& LpExpr::operator+= ( const T& rhs ) {
        if ( __ileft || __iright )
          GUM_ERROR ( OperationNotAllowed, "expr::operator+= (expr) : <= present on one side of expr" );

        if ( ! __imiddle )
          __imiddle = true;

        __mValue += rhs;

        return *this;
      }

      LpExpr& LpExpr::operator-= ( const LpCol& rhs ) {
        if ( __ileft || __iright )
          GUM_ERROR ( OperationNotAllowed, "expr::operator-= (rhs) : <= present in one of expr" );

        if ( ! __imiddle )
          __imiddle = true;

        __mCoeffs->getWithDefault ( rhs, 0. ) -= 1.;

        return *this;
      }

      LpExpr& LpExpr::operator-= ( const LpExpr& rhs ) {
        if ( __ileft || __iright || rhs.__ileft || rhs.__iright )
          GUM_ERROR ( OperationNotAllowed, "expr::operator-= (rhs) : <= present in one of rhs and/or expr" );

        if ( ! __imiddle )
          __imiddle = true;

        for ( auto it = rhs.__mCoeffs->beginSafe(), end = rhs.__mCoeffs->endSafe(); it != end; ++it )
          __mCoeffs->getWithDefault ( it.key(), 0. ) -= it.val();

        __mValue -= rhs.__mValue;

        return *this;
      }

      template< typename T >
      LpExpr& LpExpr::operator-= ( const T& rhs ) {
        if ( __ileft || __iright )
          GUM_ERROR ( OperationNotAllowed, "expr::operator-= (rhs) : <= present in one of expr" );

        if ( ! __imiddle )
          __imiddle = true;

        __mValue -= rhs;

        return *this;
      }

      std::ostream& operator<< ( std::ostream& out, const LpExpr& expr ) {
        out << expr.toString();
        return out;
      }

      void LpExpr::__addSide ( const LpCol& from ) {
        if ( ! __ileft ) {
          __lCoeffs->insert ( from, 1. );
          __ileft = true;
        } else if ( ! __imiddle ) {
          __mCoeffs->insert ( from, 1. );
          __imiddle = true;
        } else if ( ! __iright ) {
          __rCoeffs->insert ( from, 1. );
          __iright = true;
        } else
          GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );
      }


      void LpExpr::__addSide ( const LpExpr& from ) {
        if ( __ileft && __iright && from.__imiddle )
          GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );

        /// nothing to do, from is an empty LpExpr
        if ( ! from.__imiddle )
          return;

        /// from only has middle side : this should be empty or has left side, or has left and middle side
        if ( ! from.__ileft && ! from.__iright ) {
          if ( ! __ileft ) {

            * __lCoeffs = * from.__mCoeffs;
            __lValue = from.__mValue;
            __ileft = true;

            return;
          } else if ( ! __imiddle ) {

            * __mCoeffs = * from.__mCoeffs;
            __mValue = from.__mValue;
            __imiddle = true;

            return;
          } else if ( ! __iright ) {

            * __rCoeffs = * from.__mCoeffs;
            __rValue = from.__mValue;
            __iright = true;

            return;
          } else
            GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );
        }
        /// from has left and middle side : this should be empty or has left side
        else if ( from.__ileft && ! from.__iright ) {
          if ( ! __ileft ) {

            * __lCoeffs = * from.__lCoeffs;
            __lValue = from.__lValue;
            __ileft = true;

            * __mCoeffs = * from.__mCoeffs;
            __mValue = from.__mValue;
            __imiddle = true;

            return;
          } else if ( ! __imiddle && ! __iright ) {
            * __mCoeffs = * from.__lCoeffs;
            __mValue = from.__lValue;
            __imiddle = true;

            * __rCoeffs = * from.__mCoeffs;
            __rValue = from.__mValue;
            __iright = true;

            return;
          } else
            GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );
        }
        /// from has left, middle and right side : this should be empty
        /// to be exhaustive
        else if ( from.__ileft && from.__iright ) {
          if ( __ileft || __imiddle || __iright )
            GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );

          *this = from;

          return;
        } else
          GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );
      }


      void LpExpr::__addSide ( LpExpr && from ) {
        ///std::cout << from;
        if ( __ileft && __iright && from.__imiddle )
          GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );

        /// nothing to do, from is an empty LpExpr
        if ( ! from.__imiddle )
          return;

        /// from only has middle side : this should be empty or has left side, or has left and middle side
        if ( ! from.__ileft && ! from.__iright ) {
          if ( ! __ileft ) {
            ///* __lCoeffs = * from.__mCoeffs;
            swap ( __lCoeffs, from.__mCoeffs );
            __lValue = from.__mValue;
            __ileft = true;

            return;
          } else if ( ! __imiddle ) {
            ///* __mCoeffs = * from.__mCoeffs;
            swap ( __mCoeffs, from.__mCoeffs );
            __mValue = from.__mValue;
            __imiddle = true;

            return;
          } else if ( ! __iright ) {

            ///* __rCoeffs = * from.__mCoeffs;
            swap ( __rCoeffs, from.__mCoeffs );
            __rValue = from.__mValue;
            __iright = true;

            return;
          } else
            GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );
        }
        /// from has left and middle side : this should be empty or has left side
        else if ( from.__ileft && ! from.__iright ) {
          if ( ! __ileft ) {
            ///* __lCoeffs = * from.__lCoeffs;
            swap ( __lCoeffs, from.__lCoeffs );
            __lValue = from.__lValue;
            __ileft = true;

            ///* __mCoeffs = * from.__mCoeffs;
            swap ( __mCoeffs, from.__mCoeffs );
            __mValue = from.__mValue;
            __imiddle = true;

            return;
          } else if ( ! __imiddle && ! __iright ) {

            ///* __mCoeffs = * from.__lCoeffs;
            swap ( __mCoeffs, from.__lCoeffs );
            __mValue = from.__lValue;
            __imiddle = true;

            ///* __rCoeffs = * from.__mCoeffs;
            swap ( __rCoeffs, from.__mCoeffs );
            __rValue = from.__mValue;
            __iright = true;

            return;
          } else
            GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );
        }
        /// from has left, middle and right side : this should be empty
        /// to be exhaustive
        else if ( from.__ileft && from.__iright ) {
          if ( __ileft || __imiddle || __iright )
            GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );

          *this = std::move ( from );

          return;
        } else
          GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );
      }


      template< typename SCALAR >
      void LpExpr::__addSide ( const SCALAR& from ) {
        if ( ! __ileft ) {
          __lValue = from;
          __ileft = true;
        } else if ( ! __imiddle ) {
          __mValue = from;
          __imiddle = true;
        } else if ( ! __iright ) {
          __rValue = from;
          __iright = true;
        } else
          GUM_ERROR ( OperationNotAllowed, "LpExpr::setSide ( const LpCol & from ) : too many <= ; no free side" );
      }


      void LpExpr::clear() {
        __lCoeffs->clear();
        __mCoeffs->clear();
        __rCoeffs->clear();

        __lValue = 0.;
        __mValue = 0.;
        __rValue = 0.;

        __ileft = false;
        __imiddle = false;
        __iright = false;
      }


      void LpExpr::print() const {
        std::cout << std::endl;

        std::cout << "left side : " << std::endl;

        if ( __lCoeffs != nullptr )
          for ( auto it = __lCoeffs->beginSafe(), end = __lCoeffs->endSafe(); it != end; ++it )
            std::cout << it.key().toString() << " " << it.val () << " | ";

        std::cout << std::endl;

        std::cout << "middle side : " << std::endl;

        if ( __mCoeffs != nullptr )
          for ( auto it = __mCoeffs->beginSafe(), end = __mCoeffs->endSafe(); it != end; ++it )
            std::cout << it.key().toString() << " " << it.val() << " | ";

        std::cout << std::endl;

        std::cout << "right side : " << std::endl;

        if ( __rCoeffs != nullptr )
          for ( auto it = __rCoeffs->beginSafe(), end = __rCoeffs->endSafe(); it != end; ++it )
            std::cout << it.key().toString() << " " << it.val() << " | ";

        std::cout << std::endl;

        std::cout << "lvalue : " << __lValue << std::endl;
        std::cout << "mvalue : " << __mValue << std::endl;
        std::cout << "rvalue : " << __rValue << std::endl;

        std::cout << std::endl;
      }

      std::string LpExpr::toString() const {
        std::ostringstream s;

        s << "\nleft side : \n";

        if ( __lCoeffs != nullptr )
          for ( auto it = __lCoeffs->beginSafe(), end = __lCoeffs->endSafe(); it != end; ++it )
            s << it.key().toString() << " " << it.val() << " | ";

        s << "\n";

        s << "middle side : \n";

        if ( __mCoeffs != nullptr )
          for ( auto it = __mCoeffs->beginSafe(), end = __mCoeffs->endSafe(); it != end; ++it )
            s << it.key().toString() << " " << it.val() << " | ";

        s << "\n";

        s << "right side : \n";

        if ( __rCoeffs != nullptr )
          for ( auto it = __rCoeffs->beginSafe(), end = __rCoeffs->endSafe(); it != end; ++it )
            s << it.key().toString() << " " << it.val() << " | ";

        s << "\n";

        s << "lvalue : " << __lValue << "\n";
        s << "mvalue : " << __mValue << "\n";
        s << "rvalue : " << __rValue << "\n";

        s << "\n";

        return s.str();
      }


      /**
      * class LpRow
      */

      LpRow::LpRow ( const LpExpr& expr, const std::vector< LpCol >& cols ) : __coeffs ( nullptr ) {
        // we write 0 <= Ax + b from Ex + f <= Cx + d
        if ( expr.__ileft && ! expr.__iright ) {
          __coeffs = new HashTable< LpCol, double > ( * expr.__mCoeffs );

          for ( const auto & col : cols ) {
            double col_coeff = 0.;

            // from left side to middle side : 0 <= middle - left
            if ( expr.__lCoeffs->exists ( col ) )
              col_coeff = expr.__lCoeffs->operator[] ( col );

            __coeffs->getWithDefault ( col, 0. ) -= col_coeff;
          }

          __cste = expr.__mValue - expr.__lValue;
        } else if ( expr.__iright && ! expr.__ileft ) {
          __coeffs = new HashTable< LpCol, double > ( * expr.__rCoeffs );

          for ( const auto & col : cols ) {
            double col_coeff = 0;

            // from middle side to right side : 0 <= right - middle
            if ( expr.__mCoeffs->exists ( col ) )
              col_coeff = expr.__mCoeffs->operator[] ( col );

            __coeffs->getWithDefault ( col, 0. ) -= col_coeff;
          }

          __cste = expr.__rValue - expr.__mValue;
        } else
          GUM_ERROR ( OperationNotAllowed, "expr : " << expr.toString() << "is not a valid inequality; no <= detected" );

        if ( __coeffs->size() == 0 )
          GUM_ERROR ( OperationNotAllowed, "expr : " << expr.toString() << "is not a valid inequality; no variable in inequality, only constants" );

        GUM_CONSTRUCTOR ( LpRow );
      }

      LpRow::LpRow ( LpExpr && expr, const std::vector< LpCol >& cols ) : __coeffs ( nullptr ) {
        /// we write 0 <= Ax + b from Ex + f <= Cx + d
        if ( expr.__ileft && ! expr.__iright ) {
          swap ( __coeffs, expr.__mCoeffs );

          for ( const auto & col : cols ) {
            double col_coeff = 0;

            if ( expr.__lCoeffs->exists ( col ) )
              col_coeff = expr.__lCoeffs->operator[] ( col );

            __coeffs->getWithDefault ( col, 0. ) -= col_coeff;
          }

          __cste = expr.__mValue - expr.__lValue;
        } else if ( expr.__iright && ! expr.__ileft ) {
          swap ( __coeffs, expr.__rCoeffs );

          for ( const auto & col : cols ) {
            double col_coeff = 0;

            if ( expr.__mCoeffs->exists ( col ) )
              col_coeff = expr.__mCoeffs->operator[] ( col );

            __coeffs->getWithDefault ( col, 0. ) -= col_coeff;
          }

          __cste = expr.__rValue - expr.__mValue;
        } else
          GUM_ERROR ( OperationNotAllowed, "expr : " << expr.toString() << "is not a valid inequality; no <= detected" );

        if ( __coeffs->size() == 0 )
          GUM_ERROR ( OperationNotAllowed, "expr : " << expr.toString() << "is not a valid inequality; no variable in inequality, only constants" );

        GUM_CONSTRUCTOR ( LpRow );
      }


      LpRow::LpRow ( const LpRow& row ) :
        __cste ( row.__cste ),
        __coeffs ( new HashTable< LpCol, double > ( * row.__coeffs ) ) {
        GUM_CONS_CPY ( LpRow );
      }


      LpRow::LpRow ( LpRow && row ) :
        __cste ( row.__cste ),
        __coeffs ( row.__coeffs ) {
        row.__coeffs = nullptr;

        GUM_CONS_CPY ( LpRow );
      }


      LpRow::~LpRow() {
        delete __coeffs;

        GUM_DESTRUCTOR ( LpRow );
      }

      LpRow& LpRow::operator= ( const LpRow& row ) {
        __cste = row.__cste;
        *__coeffs = * row.__coeffs;
        return *this;
      }

      LpRow& LpRow::operator= ( LpRow && row ) {
        __cste = row.__cste;
        swap ( __coeffs, row.__coeffs );
        return *this;
      }

      std::ostream& operator<< ( std::ostream& out, const LpRow& row ) {
        out << row.toString();
        return out;
      }

      void LpRow::print() const {
        std::cout << toString();
      }


      std::string LpRow::toString() const {
        std::ostringstream s;

        s << "0 <= " << __cste;

        if ( __coeffs != nullptr )
          for ( auto it = __coeffs->beginSafe(), end = __coeffs->endSafe(); it != end; ++it )
            if ( it.val() > 0 ) {
              if ( it.val() != 1 ) {
                s << " +" << it.val() << "*" << it.key().toString();
              } else {
                s << " +" << it.key().toString();
              }
            } else if ( it.val() < 0 ) {
              if ( it.val() != -1 ) {
                s << " " << it.val() << "*" << it.key().toString();
              } else {
                s << " -" << it.key().toString();
              }
            }

        return s.str();
      }


      /**
      * class LpInterface
      */
      template< typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::LpInterface() {
        __positivity = false;
        __sumIsOne = false;
        GUM_CONSTRUCTOR ( LpInterface );
      }

      template< typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::LpInterface ( const LpInterface< GUM_SCALAR >& from ) :
        __cols ( from.__cols ),
        __positivity ( from.__positivity ),
        __sumIsOne ( from.__sumIsOne ) {
        __rows.resize ( from.__rows.size() );

        for ( unsigned int i = 0, end = from.__rows.size(); i < end; i++ )
          __rows[ i ] = new LpRow ( * from.__rows[ i ] );

        GUM_CONS_CPY ( LpInterface );
      }

      template< typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::LpInterface ( LpInterface< GUM_SCALAR > && from ) :
        __positivity ( from.__positivity ),
        __sumIsOne ( from.__sumIsOne ) {
        __rows.swap ( from.__rows );
        __cols.swap ( from.__cols );
        GUM_CONS_CPY ( LpInterface );
      }

      template< typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >::~LpInterface() {
        for ( const auto row : __rows )
          delete row;

        GUM_DESTRUCTOR ( LpInterface );
      }

      template < typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >& LpInterface< GUM_SCALAR >::operator= ( const LpInterface < GUM_SCALAR >& from ) {
        /// faster than clear (), delete only rows
        for ( const auto & row : __rows )
          delete row;

        __rows.clear();
        __rows.shrink_to_fit();

        __rows.resize ( from.__rows.size() );

        for ( unsigned int i = 0, end = from.__rows.size(); i < end; i++ )
          __rows[ i ] = new LpRow ( * from.__rows[ i ] );

        __cols = from.__cols;
        __positivity = from.__positivity;
        __sumIsOne = from.__sumIsOne;

        return *this;
      }

      template< typename GUM_SCALAR >
      LpInterface< GUM_SCALAR >& LpInterface< GUM_SCALAR >::operator= ( LpInterface < GUM_SCALAR > && from ) {
        __rows.swap ( from.__rows );
        __cols.swap ( from.__cols );

        __positivity = from.__positivity;
        __sumIsOne = from.__sumIsOne;

        return *this;
      }

      template< typename T >
      std::ostream& operator<< ( std::ostream& out, const LpInterface< T >& lpi ) {
        out << lpi.toString();
        return out;
      }

      template< typename GUM_SCALAR >
      LpCol LpInterface< GUM_SCALAR >::addCol() {
        LpCol col ( __cols.size() );

        __cols.push_back ( col );

        return col;
      }

      template< typename GUM_SCALAR >
      std::vector< LpCol > LpInterface< GUM_SCALAR >::addCols ( const unsigned int& cols ) {
        if ( cols < 1 )
          GUM_ERROR ( OperationNotAllowed, "LpInterface::addCols ( cols ) : cols needs must be equal or greater than 1 : " << cols << " < 1" );

        for ( unsigned int i = 0; i < cols; i++ ) {
          __cols.push_back ( LpCol ( __cols.size() ) );
        }

        return __cols;
      }

      template< typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addRow ( const LpExpr& expr ) {
        if ( ! expr.__ileft && ! expr.__iright )
          GUM_ERROR ( OperationNotAllowed, "addRow ( const LpExpr & expr ) : expr : " << expr.toString() << "is not an inequality." );

        if ( ( expr.__ileft && ! expr.__iright ) || ( ! expr.__ileft && expr.__iright ) ) {
          __rows.push_back ( new LpRow ( expr, __cols ) );
        } else {
          LpExpr lexpr ( expr, true, true, false );
          LpExpr rexpr ( expr, false, true, true );

          __rows.push_back ( new LpRow ( std::move ( lexpr ), __cols ) ); /// lexpr not used anymore, use move constructor
          __rows.push_back ( new LpRow ( std::move ( rexpr ), __cols ) ); /// rexpr not used anymore, use move constructor
        }

      }

      template< typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addRow ( LpExpr && expr ) {
        if ( ! expr.__ileft && ! expr.__iright )
          GUM_ERROR ( OperationNotAllowed, "addRow ( const LpExpr & expr ) : expr : " << expr.toString() << "is not an inequality." );

        if ( ( expr.__ileft && ! expr.__iright ) || ( ! expr.__ileft && expr.__iright ) ) {
          __rows.push_back ( new LpRow ( std::move ( expr ), __cols ) );
        } else {
          LpExpr lexpr ( std::move ( expr ), true, true, false );

          /// expr pointers on maps now are nullptr except right side
          LpExpr rexpr ( std::move ( expr ), false, false, true );

          /// rexpr miss middle side, copy it from lexpr

          * rexpr.__mCoeffs = * lexpr.__mCoeffs;
          rexpr.__mValue = lexpr.__mValue;
          rexpr.__imiddle = true;

          __rows.push_back ( new LpRow ( std::move ( lexpr ), __cols ) ); /// lexpr not used anymore, use move constructor
          __rows.push_back ( new LpRow ( std::move ( rexpr ), __cols ) ); /// rexpr not used anymore, use move constructor
        }

      }

      template< typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addPositivity() {
        if ( __positivity )
          return;

        for ( const auto & col : __cols )
          addRow ( 0 <= col );

        __positivity = true;
      }

      template< typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addSumIsOne() {
        if ( __sumIsOne )
          return;

        LpExpr expr;

        for ( const auto & col : __cols )
          expr += col;

        addRow ( 1 <= std::move ( expr ) <= 1 );

        __sumIsOne = true;
      }

      template< typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::addProba() {
        if ( __positivity && __sumIsOne ) {
          return;
        } else if ( __positivity && ! __sumIsOne ) {
          addSumIsOne();
          return;
        } else if ( ! __positivity && __sumIsOne ) {
          addPositivity();
          return;
        }

        // we can do both with one loop, don't call the above functions.
        //addPositivity();
        //addSumIsOne();
        LpExpr expr;

        for ( const auto & col : __cols ) {
          addRow ( 0 <= col );
          expr += col;
        }

        addRow ( 1 <= std::move ( expr ) <= 1 );

        __sumIsOne = true;
        __positivity = true;
      }

      template< typename GUM_SCALAR >
      std::vector< std::vector< GUM_SCALAR > > LpInterface< GUM_SCALAR >::solve() {
        /*
        if ( ! __positivity&& ! __sumIsOne ) {
          addProba();
        }
        else if ( __positivity&& ! __sumIsOne ) {
          addSumIsOne();
        }
        else if ( ! __positivity&& __sumIsOne ) {
          addPositivity();
        }
        */

        LRSWrapper< GUM_SCALAR > lrs;

        lrs.setUpH ( __cols.size() );

        std::vector< std::vector< GUM_SCALAR > > lrsMatrix;

        for ( const auto & row : __rows ) {
          std::vector< GUM_SCALAR > expandedRow ( __cols.size() + 1, 0 );

          expandedRow [ 0 ] = row->__cste;

          for ( auto it = row->__coeffs->beginSafe(), end = row->__coeffs->endSafe(); it != end; ++it )
            expandedRow [ it.key().id() + 1 ] = it.val();

          lrsMatrix.push_back ( expandedRow );
        }

        lrs.fillMatrix ( lrsMatrix );

        lrs.H2V();

        return lrs.getOutput();
      }

      template< typename GUM_SCALAR >
      std::vector< LpCol > LpInterface< GUM_SCALAR >::getCols() const {
        return __cols;
      }

      template< typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::print() const {
        std::cout << "\n\nVariables : \n";

        for ( const auto & col : __cols ) {
          std::cout << " ";
          col.print();
        }

        std::cout << std::endl;

        for ( const auto & row : __rows ) {
          std::cout << "\n";
          row->print();
        }

        std::cout << std::endl << std::endl;
      }

      template< typename GUM_SCALAR >
      std::string LpInterface< GUM_SCALAR >::toString() const {
        std::ostringstream s;

        s << "\n\nVariables : \n";

        for ( const auto & col : __cols ) {
          s << " ";
          s << col.toString();
        }

        s << "\n";

        for ( const auto & row : __rows ) {
          s << "\n";
          s << row->toString();
        }

        s << "\n\n";

        return s.str();
      }

      template< typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::clear() {
        for ( const auto & row : __rows )
          delete row;

        __rows.clear();
        __rows.shrink_to_fit();  /// to really clear content memory, otherwise we have to wait for (*this) destruction ???
        /// checked with sizeof( __rows ) + sizeof( LpRow ) * __rows.capacity()

        __cols.clear();
        __cols.shrink_to_fit();

        __positivity = false;
        __sumIsOne = false;
      }

      template< typename GUM_SCALAR >
      void LpInterface< GUM_SCALAR >::clearRows() {
        for ( const auto & row : __rows )
          delete row;

        __rows.clear();
        __rows.shrink_to_fit();

        __positivity = false;
        __sumIsOne = false;
      }

      ///////////////////////////////////////////////////////
      void swap ( HashTable< LpCol, double >*& a, HashTable< LpCol, double >*& b ) {
        HashTable< LpCol, double >* tmp ( a );
        a = b;
        b = tmp;
      }

      ///////////////////////////////////////////////////////
      template < typename T2>
      LpExpr operator+ ( LpExpr && lhs, const T2& rhs ) {
        LpExpr expr = std::move ( lhs );
        expr += rhs;

        return expr;
      }

      template < typename T2>
      LpExpr operator+ ( const LpExpr& lhs, const T2& rhs ) {
        LpExpr expr ( lhs );
        expr += rhs;

        return expr;
      }

      template< typename T1, forbidden_type<T1, LpExpr> = 0 >
      LpExpr operator+ ( const T1& lhs, LpExpr && rhs ) {
        LpExpr expr = std::move ( rhs );;
        expr += lhs;

        return expr;
      }


      template< typename T1, forbidden_type<T1, LpExpr> = 0 >
      LpExpr operator+ ( const T1& lhs, LpExpr& rhs ) {
        LpExpr expr ( rhs );
        expr += lhs;

        return expr;
      }


      template <typename T2, forbidden_type<T2, LpExpr> = 0>
      LpExpr operator+ ( const LpCol& lhs , const  T2& rhs ) {
        LpExpr expr;
        expr += lhs ;
        expr += rhs ;

        return expr;
      }

      template < typename T1,
               forbidden_type<T1, LpExpr> = 0,
               forbidden_type<T1, LpCol> = 0 >
      LpExpr operator+ ( const T1& lhs, const LpCol& rhs ) {
        LpExpr expr;
        expr += rhs;
        expr += lhs;

        return expr;
      }

      ///////////////////////////////////////////////////////
      template < typename T2>
      LpExpr operator- ( LpExpr && lhs, const T2& rhs ) {
        LpExpr expr = std::move ( lhs );
        expr -= rhs;

        return expr;
      }

      template < typename T2>
      LpExpr operator- ( const LpExpr& lhs, const T2& rhs ) {
        LpExpr expr ( lhs );
        expr -= rhs;

        return expr;
      }

      template< typename T1, forbidden_type<T1, LpExpr> = 0 >
      LpExpr operator- ( const T1& lhs, LpExpr && rhs ) {
        LpExpr expr;
        expr += std::move ( rhs );;
        expr -= lhs;

        return expr;
      }


      template< typename T1, forbidden_type<T1, LpExpr> = 0 >
      LpExpr operator- ( const T1& lhs, LpExpr& rhs ) {
        LpExpr expr;
        expr += rhs ;
        expr -= lhs;

        return expr;
      }


      template <typename T2, forbidden_type<T2, LpExpr> = 0>
      LpExpr operator- ( const LpCol& lhs , const  T2& rhs ) {
        LpExpr expr;
        expr += lhs ;
        expr -= rhs ;

        return expr;
      }

      template < typename T1,
               forbidden_type<T1, LpExpr> = 0,
               forbidden_type<T1, LpCol> = 0 >
      LpExpr operator- ( const T1& lhs, const LpCol& rhs ) {
        LpExpr expr;
        expr += rhs;
        expr -= lhs;

        return expr;
      }

      ///////////////////////////////////////////////////////
      template<typename SCALAR> INLINE
      LpExpr LpExpr::multiply ( const SCALAR& lhs, const LpCol& rhs ) {
        LpExpr expr;
        expr.__mCoeffs->insert ( rhs, lhs );
        expr.__imiddle = true;
        return expr;
      }

      template< typename SCALAR >
      LpExpr operator* ( const SCALAR& lhs, const LpCol& rhs ) {
        return LpExpr::multiply ( lhs, rhs );
      }

      template< typename SCALAR >
      LpExpr operator* ( const LpCol& lhs, const SCALAR& rhs ) {
        return LpExpr::multiply ( rhs, lhs );
      }

      ///////////////////////////////////////////////////////
      template< typename T1, typename T2 > INLINE
      LpExpr LpExpr::lessThan ( T1 && lhs,  T2 && rhs ) {
        LpExpr expr;
        expr.__addSide ( std::forward<T1> ( lhs ) );
        expr.__addSide ( std::forward<T2> ( rhs ) );
        return expr;
      }

      // const lvalue
      template< typename T2 >
      LpExpr operator<= ( const LpExpr& lhs,  T2 && rhs ) {
        return LpExpr::lessThan ( lhs , std::forward<T2> ( rhs ) );
      }

      template< typename T2 >
      LpExpr operator<= ( const LpCol& lhs,  T2 && rhs ) {
        return LpExpr::lessThan ( lhs , std::forward<T2> ( rhs ) );
      }

      template < typename T1,
               forbidden_type<T1, LpExpr&> = 0  ,
               forbidden_type<T1, LpCol&> = 0 >
      LpExpr operator<= ( T1 && lhs,  const LpExpr& rhs ) {
        return LpExpr::lessThan ( std::forward<T1> ( lhs ), rhs );
      }

      template < typename T1,
               forbidden_type<T1, LpExpr&> = 0 ,
               forbidden_type<T1, LpCol&> = 0 >
      LpExpr operator<= ( T1 && lhs,  const LpCol& rhs ) {
        return LpExpr::lessThan ( std::forward<T1> ( lhs ), rhs );
      }

      // rvaue
      template< typename T2 >
      LpExpr operator<= ( LpExpr && lhs,  T2 && rhs ) {
        return LpExpr::lessThan ( std::move ( lhs ) , std::forward<T2> ( rhs ) );
      }

      template< typename T2 >
      LpExpr operator<= ( LpCol && lhs,  T2 && rhs ) {
        return LpExpr::lessThan ( std::move ( lhs ) , std::forward<T2> ( rhs ) );
      }

      template < typename T1,
               forbidden_type<T1, LpExpr> = 0  ,
               forbidden_type<T1, LpCol> = 0 >
      LpExpr operator<= ( T1 && lhs,  LpExpr && rhs ) {
        return LpExpr::lessThan ( std::forward<T1> ( lhs ), std::move ( rhs ) );
      }

      template < typename T1,
               forbidden_type<T1, LpExpr> = 0 ,
               forbidden_type<T1, LpCol> = 0 >
      LpExpr operator<= ( T1 && lhs,  LpCol && rhs ) {
        return LpExpr::lessThan ( std::forward<T1> ( lhs ), std::move ( rhs ) );
      }
    } // namespace lp

  } // namespace cn

} // namespace gum
