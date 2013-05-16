#include "LpInterface.h"

namespace gum {
	namespace credal {
		
		/**
		 * class LpCol
		 */
		
		gum::credal::LpCol::LpCol ( unsigned int id ) { 
			__id = id; 
			GUM_CONSTRUCTOR ( gum::credal::LpCol );
		}
		
		gum::credal::LpCol::~LpCol () {
			GUM_DESTRUCTOR ( gum::credal::LpCol );
		}
		
		unsigned int gum::credal::LpCol::id () const {
			return __id;
		}
		
		bool gum::credal::LpCol::operator< ( const gum::credal::LpCol & col ) const {
			return ( __id < col.id() );
		}
		
		bool gum::credal::LpCol::operator== ( const gum::credal::LpCol & col ) const {
			return ( __id == col.id() );
		}
		
		bool gum::credal::LpCol::operator!= ( const gum::credal::LpCol & col ) const {
			return ( __id != col.id() );
		}
		
		void gum::credal::LpCol::print () const {
			std::cout << "col_" << std::to_string( __id );
		}
		
		std::string gum::credal::LpCol::toString () const {
			return "col_" + std::to_string( __id );
		}
		
		gum::credal::LpExpr gum::credal::LpCol::operator- () const {
			gum::credal::LpExpr expr;
			
			expr.__mCoeffs[ *this ] = -1;
			
			return expr;
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr gum::credal::LpCol::operator+ ( const SCALAR & f ) const {
			gum::credal::LpExpr expr;
			
			expr.__mCoeffs[ *this ] = 1;
			expr.__mValue = f;
			
			return expr;
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr gum::credal::LpCol::operator- ( const SCALAR & f ) const {					
			return this->operator+< SCALAR >( -f );
		}
		
		gum::credal::LpExpr gum::credal::LpCol::operator+ ( const gum::credal::LpCol & col ) const {
			gum::credal::LpExpr expr;
			
			// += is important, we can have *this == col
			expr.__mCoeffs[ *this ] += 1;
			expr.__mCoeffs[ col ] += 1;
			
			return expr;
		}
		
		gum::credal::LpExpr gum::credal::LpCol::operator- ( const gum::credal::LpCol & col ) const {
			gum::credal::LpExpr expr;
			
			// += and -= are important, we can have *this == col
			expr.__mCoeffs[ *this ] += 1;
			expr.__mCoeffs[ col ] -= 1;
			
			return expr;
		}
		
		gum::credal::LpExpr gum::credal::LpCol::operator+ ( const gum::credal::LpExpr & expr ) const {
			gum::credal::LpExpr expra = expr;
			
			expra.__mCoeffs[ *this ] += 1;
			
			return expra;
		}
		
		gum::credal::LpExpr gum::credal::LpCol::operator- ( const gum::credal::LpExpr & expr ) const {
			if ( expr.__ileft || expr.__iright )
				GUM_ERROR ( gum::OperationNotAllowed, " col::operator-( expr ) : <= present on a side ! " );
			
			gum::credal::LpExpr expra = expr;
			
			expra.__lValue = - expra.__lValue;
			expra.__mValue = - expra.__mValue;
			expra.__rValue = - expra.__rValue;
			
			for ( auto & entry : expra.__lCoeffs )
				entry.second = - entry.second;
			
			for ( auto & entry : expra.__mCoeffs )
				entry.second = - entry.second;
			
			for ( auto & entry : expra.__rCoeffs )
				entry.second = - entry.second;
			
			expra.__mCoeffs[ *this ] += 1;
			
			return expra;
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr gum::credal::LpCol::operator* ( const SCALAR & f ) const {
			gum::credal::LpExpr expr;
			
			expr.__mCoeffs[ *this ] = f;
			
			return expr;
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr gum::credal::LpCol::operator<= ( const SCALAR & f ) const {
			gum::credal::LpExpr expr;
			
			expr.__ileft = true;
			expr.__lCoeffs[ *this ] = 1;
			expr.__mValue = f;
			
			return expr;
		}
		
		gum::credal::LpExpr gum::credal::LpCol::operator<= ( const gum::credal::LpCol & col ) const {
			gum::credal::LpExpr expr;
			
			expr.__ileft = true;
			expr.__lCoeffs[ *this ] = 1;
			expr.__mCoeffs[ col ] = 1;
			
			return expr;
		}
		
		gum::credal::LpExpr gum::credal::LpCol::operator<= ( const gum::credal::LpExpr & expr ) const {
			if ( expr.__ileft && expr.__iright )
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			gum::credal::LpExpr expra;
			
			if ( ! expr.__ileft ) {
				expra = expr;
				expra.__ileft = true;
				expra.__lCoeffs[ *this ] = 1;
			}
			else if ( ! expr.__iright ) {
				std::cout << " col::operator<= ( expr ) :: move expr to right, something on the left side !" << std::endl;
				// move right
				for ( const auto & entry : expr.__lCoeffs )
					expra.__mCoeffs[ entry.first ] = entry.second;
				
				for ( const auto & entry : expr.__mCoeffs )
					expra.__rCoeffs[ entry.first ] = entry.second;
				
				expra.__mValue = expr.__lValue;
				expra.__rValue = expr.__mValue;
				
				expra.__lCoeffs[ *this ] = 1;
				
				expra.__iright = true;
				expra.__ileft = true;
			}
			
			return expra;
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr operator+ ( const SCALAR & f, const LpCol & col ) {
			return col.operator+< SCALAR >( f );
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr operator- ( const SCALAR & f, const LpCol & col ) {
			return ( - col ).operator+< SCALAR >( f );
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr operator* ( const SCALAR & f, const LpCol & col ) {
			return col.operator*< SCALAR >( f );
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr operator<= ( const SCALAR & f, const LpCol & col ) {
			gum::credal::LpExpr expr;
			
			expr.__ileft = true;
			expr.__mCoeffs[ col ] = 1;
			expr.__lValue = f;
			
			return expr;
		}
		
		/**
		 * class LpExpr
		 */
		
		gum::credal::LpExpr::LpExpr () {
			__lValue = 0.;
			__rValue = 0.;
			__mValue = 0.;
			__ileft = false;
			__iright = false;
			
			GUM_CONSTRUCTOR ( gum::credal::LpExpr );
		}
		
		gum::credal::LpExpr::~LpExpr () {
			GUM_DESTRUCTOR ( gum::credal::LpExpr );
		}
		
		gum::credal::LpExpr gum::credal::LpExpr::operator- () const {			
			if ( __ileft || __iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator- : <= present in one of expr");
			
			gum::credal::LpExpr expr = *this;
			
			for ( auto & entry : expr.__mCoeffs )
				entry.second = - entry.second;
			
			expr.__mValue = - expr.__mValue;
			
			return expr;
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr gum::credal::LpExpr::operator+ ( const SCALAR & f ) const {
			if ( __ileft || __iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator- (expr) : <= present in one of expr");
			
			gum::credal::LpExpr expr = *this;
			
			expr.__mValue += f;
			
			return expr;
		}
		
		gum::credal::LpExpr gum::credal::LpExpr::operator+ ( const gum::credal::LpCol & col ) const {
			if ( __ileft || __iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator- (expr) : <= present in one of expr");
			
			gum::credal::LpExpr expr = *this;
			
			expr.__mCoeffs[ col ] += 1.;
			
			return expr;
		}
		
		gum::credal::LpExpr gum::credal::LpExpr::operator+ ( const gum::credal::LpExpr & expr ) const {
			if ( __ileft || __iright || expr.__ileft || expr.__iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator- (expr) : <= present in one of expr");
			
			gum::credal::LpExpr expra = *this;
			
			for ( const auto & entry : expr.__mCoeffs )
				expra.__mCoeffs[ entry.first ] += entry.second;
			
			expra.__mValue += expr.__mValue;
			
			return expra;
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr gum::credal::LpExpr::operator- ( const SCALAR & f ) const {
			if ( __ileft || __iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator- (expr) : <= present in one of expr");
			
			gum::credal::LpExpr expr = *this;
			
			expr.__mValue -= f;
			
			return expr;
		}
		
		gum::credal::LpExpr gum::credal::LpExpr::operator- ( const gum::credal::LpCol & col ) const {
			if ( __ileft || __iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator- (expr) : <= present in one of expr");
			
			gum::credal::LpExpr expr = *this;
			
			expr.__mCoeffs[ col ] -= 1.;
			
			return expr;
		}
		
		gum::credal::LpExpr gum::credal::LpExpr::operator- ( const gum::credal::LpExpr & expr ) const {
			if ( __ileft || __iright || expr.__ileft || expr.__iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator- (expr) : <= present in one of expr");
			
			gum::credal::LpExpr expra = *this;
			
			expra.__mValue -= expr.__mValue;
			
			for ( const auto & entry : expr.__mCoeffs )
				expra.__mCoeffs[ entry.first ] -= entry.second;
			
			return expra;
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr gum::credal::LpExpr::operator<= ( const SCALAR & f ) const {
			if ( __ileft && __iright )
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			gum::credal::LpExpr expr = *this;
			
			if ( ! __ileft ) {
				expr.__moveLeft ();
				expr.__mValue = f;
			}
			else /*if ( ! __iright )*/ {
				expr.__iright = true;
				expr.__rValue = f;
			}
			
			return expr;
		}
		
		gum::credal::LpExpr gum::credal::LpExpr::operator<= ( const gum::credal::LpCol & col ) const {
			if ( __ileft && __iright )
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			gum::credal::LpExpr expr = *this;
			
			if ( ! __ileft ) {
				expr.__moveLeft();
				expr.__mCoeffs[ col ] = 1.;
			}
			else /*if ( ! __iright )*/ {
				expr.__iright = true;
				expr.__rCoeffs[ col ] = 1.;
			}
			
			return expr;
		}
		
		gum::credal::LpExpr gum::credal::LpExpr::operator<= ( const gum::credal::LpExpr & expr ) const {
			bool al = __ileft;
			bool ar = __iright;
			bool bl = expr.__ileft;
			bool br = expr.__iright;
			
			if ( ( al || ar ) && ( bl || br ) )
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			if ( ( al && ar ) || ( bl && br ) )
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			gum::credal::LpExpr expra = *this;
			
			if ( al ) {
				for ( const auto & entry : expr.__mCoeffs )
					expra.__rCoeffs[ entry.first ] = entry.second;
				
				expra.__rValue = expr.__mValue;
				
				expra.__iright = true;
			}
			else if ( ar ) {
				std::cout << "move left from right on first expr : expr <= expr" << std::endl;
				expra.__moveLeft();
				
				for ( const auto & entry : expr.__mCoeffs )
					expra.__rCoeffs[ entry.first ] = entry.second;
				
				expra.__rValue = expr.__mValue;
			}
			else if ( bl ) {
				std::cout << "move left from middle on first expr : expr <= expr" << std::endl;
				std::cout << " second expr has left member " << std::endl;
				expra.__moveLeft();
				
				for ( const auto & entry : expr.__lCoeffs )
					expra.__mCoeffs[ entry.first ] = entry.second;
				
				expra.__mValue = expr.__lValue;
				
				for ( const auto & entry : expr.__mCoeffs )
					expra.__rCoeffs[ entry.first ] = entry.second;
				
				expra.__rValue = expr.__mValue;
				
				expra.__iright = true;
			}
			else if ( br ) {
				std::cout << "move left from middle on first expr : expr <= expr" << std::endl;
				std::cout << " second expr has right member " << std::endl;
				expra.__moveLeft();
				
				for ( const auto & entry : expr.__mCoeffs )
					expra.__mCoeffs[ entry.first ] = entry.second;
				
				expra.__mValue = expr.__mValue;
				
				for ( const auto & entry : expr.__rCoeffs )
					expra.__rCoeffs[ entry.first ] = entry.second;
				
				expra.__rValue = expr.__rValue;
				
				expra.__iright = true;
			}
			else {
				expra.__moveLeft();
				
				for ( const auto & entry : expr.__mCoeffs )
					expra.__mCoeffs[ entry.first ] += entry.second;
				
				expra.__mValue = expr.__mValue;
				
				expra.__ileft = true;
			}
			
			return expra;
		}
		
		
		void gum::credal::LpExpr::print () const {
			std::cout << std::endl;
			
			std::cout << "left side : " << std::endl;
			for ( const auto & entry : __lCoeffs )
				std::cout << entry.first.toString() << " " << entry.second << " | ";
			std::cout << std::endl;
			
			std::cout << "middle side : " << std::endl;
			for ( const auto & entry : __mCoeffs )
				std::cout << entry.first.toString() << " " << entry.second << " | ";
			std::cout << std::endl;
			
			std::cout << "right side : " << std::endl;
			for ( const auto & entry : __rCoeffs )
				std::cout << entry.first.toString() << " " << entry.second << " | ";
			std::cout << std::endl;
			
			std::cout << "lvalue : " << __lValue << std::endl;
			std::cout << "mvalue : " << __mValue << std::endl;
			std::cout << "rvalue : " << __rValue << std::endl;
			
			std::cout << std::endl;
		}
		
		std::string gum::credal::LpExpr::toString () const {
			std::ostringstream s;
			
			s << "\nleft side : \n";
			for ( const auto & entry : __lCoeffs )
				s << entry.first.toString() << " " << entry.second << " | ";
			
			s << "\n";
			
			s << "middle side : \n";
			for ( const auto & entry : __mCoeffs )
				s << entry.first.toString() << " " << entry.second << " | ";
			s << "\n";
			
			s << "right side : \n";
			for ( const auto & entry : __rCoeffs )
				s << entry.first.toString() << " " << entry.second << " | ";
			s << "\n";
			
			s << "lvalue : " << __lValue << "\n";
			s << "mvalue : " << __mValue << "\n";
			s << "rvalue : " << __rValue << "\n";
			
			s << "\n";
			
			return s.str();
		}
		
		void gum::credal::LpExpr::__moveLeft () {
			if ( ! __ileft ) {
				__ileft = true;
				
				__lValue = __mValue;
				__mValue = 0.;
				
				for ( const auto & entry : __mCoeffs )
					__lCoeffs[ entry.first ] = entry.second;
				
				__mCoeffs.clear();
				
				if ( __iright ) {
					std::cout << "move left, with right present" << std::endl;
					
					__mValue = __rValue;
					__rValue = 0.;
					
					for ( const auto & entry : __rCoeffs )
						__mCoeffs[ entry.first ] = entry.second;
					
					__rCoeffs.clear();
				}
			}
			else
				GUM_ERROR ( OperationNotAllowed, "moveLeft : cannot move left, something already at left side");
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr operator+ ( const SCALAR & f, const gum::credal::LpExpr & expr ) {
			return expr.operator+< SCALAR > ( f );
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr operator- ( const SCALAR & f, const gum::credal::LpExpr & expr ) {
			return ( - expr ).operator+< SCALAR > ( f );
		}
		
		template< typename SCALAR >
		gum::credal::LpExpr operator<= ( const SCALAR & f, const gum::credal::LpExpr & expr ) {
			gum::credal::LpExpr expra;
			
			if ( ! expr.__ileft ) {
				expra = expr;
				expra.__lValue = f;
				expra.__ileft = true;
			}
			else if ( ! expr.__iright ) {
				/// move right
				for ( const auto & entry : expr.__lCoeffs )
					expra.__mCoeffs[ entry.first ] = entry.second;
				
				for ( const auto & entry : expr.__mCoeffs )
					expra.__rCoeffs[ entry.first ] = entry.second;
				
				expra.__mValue = expr.__lValue;
				expra.__rValue = expr.__mValue;
				
				expra.__iright = true;
				expra.__ileft = true;
				
				expra.__lValue = f;
			}
			else
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			return expra;
		}
		
		
		
		/**
		 * class LpRow
		 */
		
		gum::credal::LpRow::LpRow ( const std::vector< gum::credal::LpCol > & cols ) {
			for ( const auto & col : cols )
				__coeffs.insert( std::pair< gum::credal::LpCol, double>( col, 0. ) );
			
			__cste = 0.;
			
			GUM_CONSTRUCTOR ( gum::credal::LpRow );
		}
		
		gum::credal::LpRow::LpRow ( const gum::credal::LpExpr & expr, const std::vector< gum::credal::LpCol > & cols ) {
			/// we write 0 <= Ax + b from Ex + f <= Cx + d
			/// __lower is b in this setting
			if ( expr.__ileft ) {
				for ( const auto & col : cols ) {
					double col_coeff = 0;
					
					auto itl = expr.__lCoeffs.find( col );
					
					if ( itl != expr.__lCoeffs.end() )
						col_coeff -= itl->second;
					
					auto itm = expr.__mCoeffs.find( col );
					
					if ( itm != expr.__mCoeffs.end() )
						col_coeff += itm->second;
					
					__coeffs.insert( std::pair< gum::credal::LpCol, double>( col, col_coeff ) );
				}
				
				__cste = expr.__mValue - expr.__lValue;
			}
			else if ( expr.__iright ) {
				for ( const auto & col : cols ) {
					double col_coeff = 0;
					
					auto itm = expr.__mCoeffs.find( col );
					
					if ( itm != expr.__mCoeffs.end() )
						col_coeff -= itm->second;
					
					auto itr = expr.__rCoeffs.find( col );
					
					if ( itr != expr.__rCoeffs.end() )
						col_coeff += itr->second;
					
					__coeffs.insert( std::pair< gum::credal::LpCol, double>( col, col_coeff ) );
				}
				
				__cste = expr.__rValue - expr.__mValue;
			}
			else {
				GUM_ERROR ( OperationNotAllowed, "expr : " << expr.toString() << "is not a valid inequality; no <= detected" );
			}
			
			GUM_CONSTRUCTOR ( gum::credal::LpRow );
		}
		
		gum::credal::LpRow::~LpRow () {
			GUM_DESTRUCTOR ( gum::credal::LpRow );
		}
		
		
		void gum::credal::LpRow::print () const {
			std::cout << "0 <= " << __cste;
			
			for ( const auto & entry : __coeffs )
				std::cout << " + " << entry.second << " * " << entry.first.toString();
		}
		
		
		std::string gum::credal::LpRow::toString () const {
			std::ostringstream s;
			
			s << "0 <= " << __cste;
			
			for ( const auto & entry : __coeffs )
				s << " + " << entry.second << " * " << entry.first.toString();
			
			return s.str();
		}
		
		
		/**
		 * class LpInterface
		 */
		
		gum::credal::LpInterface::LpInterface () {
			GUM_CONSTRUCTOR ( gum::credal::LpInterface );
		}
		
		gum::credal::LpInterface::~LpInterface () {
			GUM_DESTRUCTOR( gum::credal::LpInterface );
		}
		
		gum::credal::LpCol gum::credal::LpInterface::addCol () {
			__cols.push_back( gum::credal::LpCol ( __cols.size() ) );
			return __cols[ __cols.size() - 1 ];
		}
		
		void gum::credal::LpInterface::addRow ( const gum::credal::LpExpr & expr ) {
			if ( ! expr.__ileft && ! expr.__iright )
				GUM_ERROR ( OperationNotAllowed, "addRow ( const gum::credal::LpExpr & expr ) : expr : " << expr.toString() << "is not an inequality." );
			
			if ( ( expr.__ileft && ! expr.__iright ) || ( ! expr.__ileft && expr.__iright ) ) {
				__rows.push_back( gum::credal::LpRow ( expr, __cols ) );
			}
			else {
				gum::credal::LpExpr lexpr = expr;
				gum::credal::LpExpr rexpr = expr;
				
				lexpr.__rCoeffs.clear();
				lexpr.__rValue = 0;
				lexpr.__iright = false;
				
				rexpr.__lCoeffs.clear();
				rexpr.__lValue = 0;
				rexpr.__ileft = false;
				
				__rows.push_back( gum::credal::LpRow ( lexpr, __cols ) );
				__rows.push_back( gum::credal::LpRow ( rexpr, __cols ) );
			}
			
		}
		
		std::vector< std::vector< double > > gum::credal::LpInterface::solve () {
			// vacuous case, only positivity constraints and sum constraint
			if ( __rows.size() == 0 ) {
				std::vector< std::vector< double > > vacuous( __cols.size() );
				std::vector< double > vertex ( __cols.size() );
				
				for ( const auto & col : __cols ) {
					vertex[ col.id() ] = 1;
					vacuous[ col.id() ] = vertex;
					vertex[ col.id() ] = 0;
				}
				
				return vacuous;
			}
			
			// positivity constraints & sum constraint
			gum::credal::LpExpr exprInf;

			for ( const auto & col : __cols ) {
				this->addRow( 0 <= col );
				exprInf = exprInf + col;
			}
			
			gum::credal::LpExpr exprSup = exprInf;
			
			exprInf = 1 <= exprInf;
			exprSup = exprSup <= 1;
			
			this->addRow ( exprInf );
			this->addRow ( exprSup );
			
			gum::credal::LRSWrapper< double > lrs;
			
			lrs.setUpH( __cols.size() );
			
			std::vector< std::vector< double > > lrsMatrix;
			
			for ( const auto & row : __rows ) {
				std::vector< double > expandedRow ( __cols.size() + 1, 0 );
				
				expandedRow[ 0 ] = row.__cste;
				
				for ( const auto & entry : row.__coeffs )
					expandedRow[ entry.first.id() + 1 ] = entry.second;
				
				lrsMatrix.push_back ( expandedRow );
			}
			
			lrs.fillMatrix( lrsMatrix ); /// @todo !! redo fillMatrix !!
			
			lrs.H2V();
			
			return lrs.getOutput();
		}
		
		
		void gum::credal::LpInterface::print () const {
			std::cout << "\n\nVariables : \n";
			
			for ( const auto & col : __cols ) {
				std::cout << " ";
				col.print();
			}
			std::cout << std::endl;
			
			for ( const auto & row : __rows ) {
				std::cout << "\n";
				row.print();
			}
			std::cout << std::endl;
		}
		
		
		std::string gum::credal::LpInterface::toString () const {
			std::ostringstream s;
			
			s << "\n\nVariables : \n";
			
			for ( const auto & col : __cols ) {
				s << " ";
				s << col.toString();
			}
			s << "\n";
			
			for ( const auto & row : __rows ) {
				s << "\n";
				s << row.toString();
			}
			s << "\n";
			
			return s.str();
		}
		
		void gum::credal::LpInterface::clear () {
			__rows.clear();
			__cols.clear();
		}
		
	} // namespace cn
} // namespace gum