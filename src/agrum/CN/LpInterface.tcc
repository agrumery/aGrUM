#include "LpInterface.h"

namespace gum {
	namespace credal {
		
		/**
		 * class LpCol
		 */
		
		LpCol::LpCol ( unsigned int id ) { 
			__id = id; 
			
			GUM_CONSTRUCTOR ( LpCol );
		}
		
		LpCol::LpCol ( const LpCol & col ) {
			__id = col.__id;
			
			GUM_CONS_CPY ( LpCol );
		}

		
		LpCol & LpCol::operator= ( const LpCol & col ) {
			__id = col.__id;
			
			return *this;
		}
		
		LpCol::~LpCol () {
			GUM_DESTRUCTOR ( LpCol );
		}
		
		unsigned int LpCol::id () const {
			return __id;
		}
		
		bool LpCol::operator< ( const LpCol & col ) const {
			return ( __id < col.id() );
		}
		
		bool LpCol::operator== ( const LpCol & col ) const {
			return ( __id == col.id() );
		}
		
		bool LpCol::operator!= ( const LpCol & col ) const {
			return ( __id != col.id() );
		}
		
		void LpCol::print () const {
			std::cout << "col_" << std::to_string ( __id );
		}
		
		std::string LpCol::toString () const {
			return "col_" + std::to_string ( __id );
		}
		
		LpExpr LpCol::operator- () const {
			LpExpr expr;
			// we could call expr -= *this but we would test for left or right hand side, which ( uselessly ) slows the operation
			// we will never call expr += unless one argument is already a LpExpr and/or we don't need to check lhs or rhs right away
			// i.e. we gain either copy or test operations
			expr.__mCoeffs.insert ( *this, -1. );
			
			return expr;
		}
		
		template< typename SCALAR >
		LpExpr LpCol::operator+ ( const SCALAR & f ) const {
			LpExpr expr;
			
			expr.__mCoeffs.insert ( *this, 1. );
			expr.__mValue = f;
			
			return expr;
		}
		
		template< typename SCALAR >
		LpExpr LpCol::operator- ( const SCALAR & f ) const {					
			return this->operator+< SCALAR > ( - f );
		}
		
		LpExpr LpCol::operator+ ( const LpCol & col ) const {
			LpExpr expr;
			
			// getWithDefault and += are important, we can have *this == col
			expr.__mCoeffs.insert ( *this, 1. );
			expr.__mCoeffs.getWithDefault ( col, 0. ) += 1.;
			
			return expr;
		}
		
		LpExpr LpCol::operator- ( const LpCol & col ) const {
			/// we could call this.op+ ( - col ) but -col gives an expression and will chain calls with col.op+( expr ) 
			/// which will result in more calls to the copy constructor of LpExpr
			/// += , -= do useless tests
			LpExpr expr;
			
			// getWithDefault and -= are important, we can have *this == col
			expr.__mCoeffs.insert ( *this, 1. );
			expr.__mCoeffs.getWithDefault ( col, 0. ) -= 1.;
			
			return expr;
		}
		
		LpExpr LpCol::operator+ ( const LpExpr & expr ) const {
			return expr.operator+ ( *this );
		}
		
		LpExpr LpCol::operator- ( const LpExpr & expr ) const {
			/// do not call ( - expr ).op+( col ) or col.op+( - expr ) which will result in more copies of (-) expr
			///if ( expr.__ileft || expr.__iright )
				///GUM_ERROR ( gum::OperationNotAllowed, " col::operator-( expr ) : <= present on a side ! " );
			
			LpExpr expra ( - expr ); /// will do the if test, result in no more copies using - expr than expr and manually inverting signs
			
			/*expra.__mValue = - expra.__mValue;
			
			for ( auto it = expra.__mCoeffs.begin (), end = expra.__mCoeffs.end (); it != end; ++it )
				*it = - *it;
			*/
			expra.__mCoeffs.getWithDefault ( *this, 0. ) += 1.; /// don't call +=, it will do another redundant if test
			
			return expra;
		}
		
		template< typename SCALAR >
		LpExpr LpCol::operator* ( const SCALAR & f ) const {
			LpExpr expr;
			
			expr.__mCoeffs.insert ( *this, f );
			
			return expr;
		}
		
		template< typename SCALAR >
		LpExpr LpCol::operator<= ( const SCALAR & f ) const {
			LpExpr expr;
			
			expr.__ileft = true;
			expr.__lCoeffs.insert ( *this, 1. );
			expr.__mValue = f;
			
			return expr;
		}
		
		LpExpr LpCol::operator<= ( const LpCol & col ) const {
			LpExpr expr;
			
			expr.__ileft = true;
			expr.__lCoeffs.insert ( *this, 1. );
			expr.__mCoeffs.insert ( col, 1. );
			
			return expr;
		}
		
		LpExpr LpCol::operator<= ( const LpExpr & expr ) const {
			if ( expr.__ileft && expr.__iright )
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			if ( expr.__ileft /*&& ! expr.__iright*/ ) GUM_ERROR ( gum::OperationNotAllowed, " col <= expr, left but no right ")
			///LpExpr expra;
			
			//if ( ! expr.__ileft ) {
				LpExpr expra ( /*=*/ expr );
				
				expra.__lCoeffs.insert ( *this, 1. );
				expra.__ileft = true;
			//}
				
			/*
			else if ( ! expr.__iright ) {
				std::cout << " col::operator<= ( expr ) :: move expr to right, something on the left side !" << std::endl;
				/// move right
				for ( auto it = expr.__lCoeffs.begin (), end = expr.__lCoeffs.end (); it != end; ++it )
					expra.__mCoeffs.insert ( it.key (), *it );
				
				for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
					expra.__rCoeffs.insert ( it.key (), *it );
				
				expra.__mValue = expr.__lValue;
				expra.__rValue = expr.__mValue;
				
				expra.__lCoeffs.insert ( *this, 1. );
				
				expra.__iright = true;
				expra.__ileft = true;
			}
			*/
			
			return expra;
		}
		
		template< typename SCALAR >
		LpExpr operator+ ( const SCALAR & f, const LpCol & col ) {
			return col.operator+< SCALAR > ( f );
		}
		
		template< typename SCALAR >
		LpExpr operator- ( const SCALAR & f, const LpCol & col ) {
			LpExpr expr;
			
			expr.__mCoeffs.insert ( col, -1. );
			expr.__mValue = f;
			
			return expr;
			///return ( - col ).operator+=< SCALAR > ( f ); /// more copies
		}
		
		template< typename SCALAR >
		LpExpr operator* ( const SCALAR & f, const LpCol & col ) {
			return col.operator*< SCALAR > ( f );
		}
		
		template< typename SCALAR >
		LpExpr operator<= ( const SCALAR & f, const LpCol & col ) {
			LpExpr expr;
			
			expr.__ileft = true;
			expr.__mCoeffs.insert ( col, 1. );
			expr.__lValue = f;
			
			return expr;
		}
		
	}
	
	INLINE Size 
	HashFunc< credal::LpCol >::operator() ( const credal::LpCol & key ) const {
		return ( ( Size ) key.id () );
	}
	
	namespace credal {
		
		/**
		 * class LpExpr
		 */
		
		LpExpr::LpExpr () {
			__lValue = 0.;
			__rValue = 0.;
			__mValue = 0.;
			
			__ileft = false;
			__iright = false;
			
			GUM_CONSTRUCTOR ( LpExpr );
		}
		
		LpExpr::LpExpr ( const LpExpr & expr ) {
			__copy ( expr );
			
			GUM_CONSTRUCTOR ( LpExpr );
		}
		
		LpExpr::~LpExpr () {
			GUM_DESTRUCTOR ( LpExpr );
		}
		
		LpExpr LpExpr::operator- () const {
			if ( __ileft || __iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator- : <= present on one side of expr" );
			
			LpExpr expr ( *this );
			
			for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
				*it = - *it;

			expr.__mValue = - expr.__mValue;
			
			return expr;
		}
		
		LpExpr LpExpr::operator+ ( const LpCol & col ) const {
			///if ( __ileft || __iright )
				///GUM_ERROR ( OperationNotAllowed, "expr::operator+ (expr) : <= present in one of expr" );
			
			LpExpr expr ( *this );
			expr += col;
			///expr.__mCoeffs.getWithDefault ( col, 0. ) += 1.;
			
			return expr;
			///return ( LpExpr ( *this ) += col ); /// way to many copies
		}
		
		LpExpr LpExpr::operator+ ( const LpExpr & expr ) const {
			///if ( __ileft || __iright || expr.__ileft || expr.__iright )
				///GUM_ERROR ( OperationNotAllowed, "expr::operator+ (expr) : <= present in one of expr" );
			
			LpExpr expra ( *this );
			expra += expr;
			///for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
				///expra.__mCoeffs.getWithDefault ( it.key (), 0. ) += *it;

			///expra.__mValue += expr.__mValue;
			
			return expra;
		}
		
		
		template< typename SCALAR >
		LpExpr LpExpr::operator+ ( const SCALAR & f ) const {
			///if ( __ileft || __iright )
				///GUM_ERROR ( OperationNotAllowed, "expr::operator+ (expr) : <= present in one of expr" );
			
			LpExpr expr ( *this );
			expr += f;
			///expr.__mValue += f;
			
			return expr;
		}
		
		template< typename SCALAR >
		LpExpr LpExpr::operator- ( const SCALAR & f ) const {
			///if ( __ileft || __iright )
				///GUM_ERROR ( OperationNotAllowed, "expr::operator- (expr) : <= present in one of expr" );
			
			LpExpr expr ( *this );
			expr -= f;
			///expr.__mValue -= f;
			
			return expr;
		}
		
		LpExpr LpExpr::operator- ( const LpCol & col ) const {
			///if ( __ileft || __iright )
				///GUM_ERROR ( OperationNotAllowed, "expr::operator- (expr) : <= present in one of expr" );
			
			LpExpr expr ( *this );
			expr -= col;
			///expr.__mCoeffs.getWithDefault ( col, 0. ) -= 1.;
			
			return expr;
		}
		
		LpExpr LpExpr::operator- ( const LpExpr & expr ) const {
			///if ( __ileft || __iright || expr.__ileft || expr.__iright )
				///GUM_ERROR ( OperationNotAllowed, "expr::operator- (expr) : <= present in one of expr" );
			
			LpExpr expra ( *this );
			expra -= expr;
			
			///for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
				///expra.__mCoeffs.getWithDefault ( it.key (), 0 ) -= *it;
			
			///expra.__mValue -= expr.__mValue;
			
			return expra;
		}
		
		template< typename SCALAR >
		LpExpr LpExpr::operator<= ( const SCALAR & f ) const {
			if ( __ileft && __iright )
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			LpExpr expr ( *this );
			
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
		
		LpExpr LpExpr::operator<= ( const LpCol & col ) const {
			if ( __ileft && __iright )
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			LpExpr expr ( *this );
			
			if ( ! __ileft ) {
				expr.__moveLeft ();
				expr.__mCoeffs.getWithDefault ( col, 0. ) = 1.;
			}
			else /*if ( ! __iright )*/ {
				expr.__iright = true;
				expr.__rCoeffs.getWithDefault ( col, 0. ) = 1.;
			}
			
			return expr;
		}
		
		LpExpr LpExpr::operator<= ( const LpExpr & expr ) const {
			bool al = __ileft;
			bool ar = __iright;
			bool bl = expr.__ileft;
			bool br = expr.__iright;
			
			if ( ( al || ar ) && ( bl || br ) )
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			if ( ( al && ar ) || ( bl && br ) )
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			LpExpr expra ( *this );
			
			if ( al ) {
				for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
					expra.__rCoeffs.insert ( it.key (), *it );

				expra.__rValue = expr.__mValue;
				
				expra.__iright = true;
			}
			else if ( ar ) {
				std::cout << "move left from right on first expr : expr <= expr" << std::endl;
				expra.__moveLeft();
				
				for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
					expra.__rCoeffs.insert ( it.key (), *it );

				expra.__rValue = expr.__mValue;
			}
			else if ( bl ) {
				std::cout << "move left from middle on first expr : expr <= expr" << std::endl;
				std::cout << " second expr has left member " << std::endl;
				expra.__moveLeft ();
				
				for ( auto it = expr.__lCoeffs.begin (), end = expr.__lCoeffs.end (); it != end; ++it )
					expra.__mCoeffs.insert ( it.key (), *it );

				expra.__mValue = expr.__lValue;
				
				for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
					expra.__rCoeffs.insert ( it.key (), *it );

				expra.__rValue = expr.__mValue;
				
				expra.__iright = true;
			}
			else if ( br ) {
				std::cout << "move left from middle on first expr : expr <= expr" << std::endl;
				std::cout << " second expr has right member " << std::endl;
				expra.__moveLeft ();
				
				for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
					expra.__mCoeffs.insert ( it.key (), *it );

				expra.__mValue = expr.__mValue;
				
				for ( auto it = expr.__rCoeffs.begin (), end = expr.__rCoeffs.end (); it != end; ++it )
					expra.__rCoeffs.insert ( it.key (), *it );

				expra.__rValue = expr.__rValue;
				
				expra.__iright = true;
			}
			else {
				expra.__moveLeft ();
				
				for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
					expra.__mCoeffs.insert ( it.key (), *it );

				expra.__mValue = expr.__mValue;
				
				expra.__ileft = true;
			}
			
			return expra;
		}
		
		template< typename SCALAR >
		LpExpr & LpExpr::operator+= ( const SCALAR & f ) {
			if ( __ileft || __iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator+= (expr) : <= present in one of expr" );
			
			__mValue += f;
			
			return *this;
		}
		
		LpExpr & LpExpr::operator+= ( const LpCol & col ) {
			if ( __ileft || __iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator+= (expr) : <= present in one of expr" );
			
			__mCoeffs.getWithDefault ( col, 0. ) += 1.;
			
			return *this;
		}
		
		LpExpr & LpExpr::operator+= ( const LpExpr & expr ) {
			if ( __ileft || __iright || expr.__ileft || expr.__iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator+= (expr) : <= present in one of expr" );
			
			for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
				__mCoeffs.getWithDefault ( it.key (), 0. ) += *it;

			__mValue += expr.__mValue;
			
			return *this;
		}
		
		template< typename SCALAR >
		LpExpr & LpExpr::operator-= ( const SCALAR & f ) {
			if ( __ileft || __iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator-= (expr) : <= present in one of expr" );
			
			__mValue -= f;
			
			return *this;
		}
		
		LpExpr & LpExpr::operator-= ( const LpCol & col ) {
			if ( __ileft || __iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator-= (expr) : <= present in one of expr" );
			
			__mCoeffs.getWithDefault ( col, 0. ) -= 1.;
			
			return *this;
		}
		
		LpExpr & LpExpr::operator-= ( const LpExpr & expr ) {
			if ( __ileft || __iright || expr.__ileft || expr.__iright )
				GUM_ERROR ( OperationNotAllowed, "expr::operator-= (expr) : <= present in one of expr" );
			
			for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
				__mCoeffs.getWithDefault ( it.key (), 0. ) -= *it;

			__mValue -= expr.__mValue;
			
			return *this;
		}
		
		template< typename SCALAR >
		LpExpr & LpExpr::operator= ( const SCALAR & f ) {
			clear();
			
			__mValue = f;
			
			return *this;
		}
		
		LpExpr & LpExpr::operator= ( const LpCol & col ) {
			clear ();
			
			__mCoeffs.insert ( col, 1. );
			
			return *this;
		}
		
		LpExpr & LpExpr::operator= ( const LpExpr & expr ) {
			/// self assignment check to avoid hashTable copies
			if ( this == & expr )
				return *this;
			
			__copy ( expr );
			
			return *this;
		}
		
		void LpExpr::clear () {
			__lCoeffs.clear ();
			__mCoeffs.clear ();
			__rCoeffs.clear ();
			
			__lValue = 0.;
			__mValue = 0.;
			__rValue = 0.;
			
			__ileft = false;
			__iright = false;
		}
		
		
		void LpExpr::print () const {
			std::cout << std::endl;
			
			std::cout << "left side : " << std::endl;
			
			for ( auto it = __lCoeffs.begin (), end = __lCoeffs.end (); it != end; ++it )
				std::cout << it.key ().toString () << " " << *it << " | ";

			std::cout << std::endl;
			
			std::cout << "middle side : " << std::endl;
			for ( auto it = __mCoeffs.begin (), end = __mCoeffs.end (); it != end; ++it )
				std::cout << it.key ().toString () << " " << *it << " | ";

			std::cout << std::endl;
			
			std::cout << "right side : " << std::endl;
			for ( auto it = __rCoeffs.begin (), end = __rCoeffs.end (); it != end; ++it )
				std::cout << it.key ().toString () << " " << *it << " | ";

			std::cout << std::endl;
			
			std::cout << "lvalue : " << __lValue << std::endl;
			std::cout << "mvalue : " << __mValue << std::endl;
			std::cout << "rvalue : " << __rValue << std::endl;
			
			std::cout << std::endl;
		}
		
		std::string LpExpr::toString () const {
			std::ostringstream s;
			
			s << "\nleft side : \n";
			for ( auto it = __lCoeffs.begin (), end = __lCoeffs.end (); it != end; ++it )
				s << it.key ().toString () << " " << *it << " | ";

			s << "\n";
			
			s << "middle side : \n";
			for ( auto it = __mCoeffs.begin (), end = __mCoeffs.end (); it != end; ++it )
				s << it.key ().toString () << " " << *it << " | ";

			s << "\n";
			
			s << "right side : \n";
			for ( auto it = __rCoeffs.begin (), end = __rCoeffs.end (); it != end; ++it )
				s << it.key ().toString () << " " << *it << " | ";

			s << "\n";
			
			s << "lvalue : " << __lValue << "\n";
			s << "mvalue : " << __mValue << "\n";
			s << "rvalue : " << __rValue << "\n";
			
			s << "\n";
			
			return s.str ();
		}
		
		void LpExpr::__moveLeft () {
			if ( ! __ileft ) {
				__lValue = __mValue;
				__mValue = 0.;
				
				/*for ( auto it = __mCoeffs.begin (), end = __mCoeffs.end (); it != end; ++it )
					__lCoeffs.insert ( it.key (), *it );*/
				__lCoeffs = __mCoeffs;

				__mCoeffs.clear ();
				
				__ileft = true;
				
				if ( __iright ) {
					std::cout << "move left, with right present" << std::endl;
					
					__mValue = __rValue;
					__rValue = 0.;
					
					///for ( auto it = __rCoeffs.begin (), end = __rCoeffs.end (); it != end; ++it )
						///__mCoeffs.insert ( it.key (), *it );
					__mCoeffs = __rCoeffs;

					__rCoeffs.clear ();
					
					__iright = false;
				}
			}
			else
				GUM_ERROR ( OperationNotAllowed, "moveLeft : cannot move left, something already at left side" );
		}
		
		void LpExpr::__copy ( const LpExpr & from ) {
			__copyLeft ( from );
			__copyMiddle ( from );
			__copyRight ( from );
		}
		
		void LpExpr::__copyLeft ( const LpExpr & from ) {
			if ( from.__ileft ) {
				__lCoeffs = from.__lCoeffs;
				__lValue = from.__lValue;
				__ileft = from.__ileft;
			}
			else {
				__lCoeffs.clear();
				__lValue = 0;
				__ileft = false;
			}
		}
		
		void LpExpr::__copyMiddle ( const LpExpr & from ) {
			__mCoeffs = from.__mCoeffs;
			__mValue = from.__mValue;
		}
		
		void LpExpr::__copyRight ( const LpExpr & from ) {
			if ( from.__iright ) {
				__rCoeffs = from.__rCoeffs;
				__rValue = from.__rValue;
				__iright = from.__iright;
			}
			else {
				__rCoeffs.clear();
				__rValue = 0;
				__iright = false;
			}
		}
		
		template< typename SCALAR >
		LpExpr operator+ ( const SCALAR & f, const LpExpr & expr ) {
			return expr.operator+< SCALAR > ( f );
		}
		
		template< typename SCALAR >
		LpExpr operator- ( const SCALAR & f, const LpExpr & expr ) {
			return ( - expr ).operator+=< SCALAR > ( f ); // - expr gives a new (non-const) LpExpr object, so += can be called
		}
		
		template< typename SCALAR >
		LpExpr operator<= ( const SCALAR & f, const LpExpr & expr ) {
			LpExpr expra;
			
			if ( ! expr.__ileft ) {
				expra = expr;
				expra.__lValue = f;
				expra.__ileft = true;
			}
			else if ( ! expr.__iright ) {
				std::cout << "LpExpr operator<= ( const SCALAR & f, const LpExpr & expr ) : nothing at right but something at left side" << std::endl;
				/// move right
				/// we should never go here
				for ( auto it = expr.__lCoeffs.begin (), end = expr.__lCoeffs.end (); it != end; ++it )
					expra.__mCoeffs.getWithDefault ( it.key (), 0. ) = *it; /// replace by insert ?
				
				for ( auto it = expr.__mCoeffs.begin (), end = expr.__mCoeffs.end (); it != end; ++it )
					expra.__rCoeffs.getWithDefault ( it.key (), 0. ) = *it;
				
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
		
		LpRow::LpRow ( const LpExpr & expr, const std::vector< LpCol > & cols ) {
			/// we write 0 <= Ax + b from Ex + f <= Cx + d
			if ( expr.__ileft ) {
				for ( const auto & col : cols ) {
					double col_coeff = 0.;
					
					if ( expr.__lCoeffs.exists ( col ) )
						col_coeff -= expr.__lCoeffs [ col ];
					
					if ( expr.__mCoeffs.exists ( col ) )
						col_coeff += expr.__mCoeffs [ col ];
					
					__coeffs.insert ( col, col_coeff );
				}
				
				__cste = expr.__mValue - expr.__lValue;
			}
			else if ( expr.__iright ) {
				for ( const auto & col : cols ) {
					double col_coeff = 0;
					
					if ( expr.__mCoeffs.exists ( col ) )
						col_coeff -= expr.__mCoeffs [ col ];
					
					if ( expr.__rCoeffs.exists ( col ) )
						col_coeff += expr.__rCoeffs [ col ];
					
					__coeffs.insert ( col, col_coeff );
				}
				
				__cste = expr.__rValue - expr.__mValue;
			}
			else {
				GUM_ERROR ( OperationNotAllowed, "expr : " << expr.toString () << "is not a valid inequality; no <= detected" );
			}
			
			if ( __coeffs.size() == 0 )
				GUM_ERROR ( OperationNotAllowed, "expr : " << expr.toString () << "is not a valid inequality; no variable in inequality, only constants" );
			
			GUM_CONSTRUCTOR ( LpRow );
		}
		
		
		LpRow::LpRow ( const LpRow & row ) {
			__coeffs = row.__coeffs;
			__cste = row.__cste;
			
			GUM_CONSTRUCTOR ( LpRow );
		}
		
		
		LpRow::~LpRow () {
			GUM_DESTRUCTOR ( LpRow );
		}
		
		
		void LpRow::print () const {
			std::cout << "0 <= " << __cste;
			
			for ( auto it = __coeffs.begin (), end = __coeffs.end (); it != end; ++it )
				std::cout << " + " << *it << " * " << it.key ().toString ();
		}
		
		
		std::string LpRow::toString () const {
			std::ostringstream s;
			
			s << "0 <= " << __cste;
			
			for ( auto it = __coeffs.begin (), end = __coeffs.end (); it != end; ++it )
				s << " + " << *it << " * " << it.key ().toString ();
			
			return s.str();
		}
		
		
		/**
		 * class LpInterface
		 */
		
		LpInterface::LpInterface () {
			__positivity = false;
			__sumIsOne = false;
			
			GUM_CONSTRUCTOR ( LpInterface );
		}
		
		LpInterface::~LpInterface () {
			GUM_DESTRUCTOR ( LpInterface );
		}
		
		LpCol LpInterface::addCol () {
			LpCol col ( __cols.size () );
			
			__cols.push_back ( col );
			
			return col;
		}
		
		std::vector< LpCol > LpInterface::addCols ( const unsigned int & cols ) {
			if ( cols < 1 )
				GUM_ERROR ( OperationNotAllowed, "LpInterface::addCols ( cols ) : cols needs must be equal or greater than 1 : " << cols << " < 1");
				
			for ( unsigned int i = 0; i < cols; i++ ) {
				__cols.push_back ( LpCol ( __cols.size () ) );
			}
			
			return __cols;
		}
		
		void LpInterface::addRow ( const LpExpr & expr ) {
			if ( ! expr.__ileft && ! expr.__iright )
				GUM_ERROR ( OperationNotAllowed, "addRow ( const LpExpr & expr ) : expr : " << expr.toString() << "is not an inequality." );
			
			if ( ( expr.__ileft && ! expr.__iright ) || ( ! expr.__ileft && expr.__iright ) ) {
				__rows.push_back ( LpRow ( expr, __cols ) );
			}
			else {
				LpExpr lexpr ( expr );
				LpExpr rexpr ( expr );
				
				lexpr.__rCoeffs.clear ();
				lexpr.__rValue = 0;
				lexpr.__iright = false;
				
				rexpr.__lCoeffs.clear ();
				rexpr.__lValue = 0;
				rexpr.__ileft = false;
				
				__rows.push_back ( LpRow ( lexpr, __cols ) );
				__rows.push_back ( LpRow ( rexpr, __cols ) );
			}
			
		}
		
		
		void LpInterface::addPositivity () {
			if ( __positivity )
				return;
			
			for ( const auto & col : __cols )
				addRow ( 0 <= col );
			
			__positivity = true;
		}
		
		void LpInterface::addSumIsOne () {
			if ( __sumIsOne )
				return;
			
			LpExpr exprInf;
			
			for ( const auto & col : __cols )
				exprInf += col;
			
			/// faster than taking left hand side and right hand side in addRow ( LpExpr )
			/// we can add 2 rows already processed, which result in less copies
			LpExpr exprSup ( exprInf );
			
			exprInf = 1 <= exprInf;
			exprSup = exprSup <= 1;
			
			addRow ( exprInf );
			addRow ( exprSup );
			
			__sumIsOne = true;
		}

		void LpInterface::addProba () {
			if ( __positivity && __sumIsOne ) {
				return;
			}
			else if ( __positivity && ! __sumIsOne ) {
				addSumIsOne ();
				return;
			}
			else if ( ! __positivity && __sumIsOne ) {
				addPositivity ();
				return;
			}
			
			// we can do both with one loop, don't call the above functions.
			//addPositivity();
			//addSumIsOne();
			LpExpr exprInf;
			
			for ( const auto & col : __cols ) {
				addRow ( 0 <= col );
				exprInf += col;
			}
			
			LpExpr exprSup ( exprInf );
			
			exprInf = 1 <= exprInf;
			exprSup = exprSup <= 1;
			
			addRow ( exprInf );
			addRow ( exprSup );
			
			__sumIsOne = true;
			__positivity = true;
		}
		
		std::vector< std::vector< double > > LpInterface::solve () {
			/*
			if ( ! __positivity && ! __sumIsOne ) {
				addProba();
			}
			else if ( __positivity && ! __sumIsOne ) {
				addSumIsOne();
			}
			else if ( ! __positivity && __sumIsOne ) {
				addPositivity();
			}
			*/
			
			LRSWrapper< double > lrs;
			
			lrs.setUpH ( __cols.size () );
			
			std::vector< std::vector< double > > lrsMatrix;
			
			for ( const auto & row : __rows ) {
				std::vector< double > expandedRow ( __cols.size () + 1, 0 );
				
				expandedRow [ 0 ] = row.__cste;
				
				for ( auto it = row.__coeffs.begin (), end = row.__coeffs.end (); it != end; ++it )
					expandedRow [ it.key ().id () + 1 ] = *it;
				
				lrsMatrix.push_back ( expandedRow );
			}
			
			lrs.fillMatrix ( lrsMatrix ); /// @todo !! redo fillMatrix !!
			
			lrs.H2V ();
			
			return lrs.getOutput ();
		}
		
		std::vector< LpCol > LpInterface::getCols () const {
			return __cols;
		}
		
		void LpInterface::print () const {
			std::cout << "\n\nVariables : \n";
			
			for ( const auto & col : __cols ) {
				std::cout << " ";
				col.print ();
			}
			std::cout << std::endl;
			
			for ( const auto & row : __rows ) {
				std::cout << "\n";
				row.print ();
			}
			std::cout << std::endl << std::endl;
		}
		
		
		std::string LpInterface::toString () const {
			std::ostringstream s;
			
			s << "\n\nVariables : \n";
			
			for ( const auto & col : __cols ) {
				s << " ";
				s << col.toString ();
			}
			s << "\n";
			
			for ( const auto & row : __rows ) {
				s << "\n";
				s << row.toString ();
			}
			s << "\n\n";
			
			return s.str();
		}
		
		void LpInterface::clear () {
			__rows.clear ();
			__rows.shrink_to_fit (); /// to really clear content memory, otherwise we have to wait for (*this) destruction ???
			/// checked with sizeof( __rows ) + sizeof( LpRow ) * __rows.capacity()
			
			__cols.clear ();
			__cols.shrink_to_fit ();
			
			__positivity = false;
			__sumIsOne = false;
		}
		
		void LpInterface::clearRows () {
			__rows.clear ();
			__rows.shrink_to_fit ();
			
			__positivity = false;
			__sumIsOne = false;
		}
		
	} // namespace cn
	
} // namespace gum