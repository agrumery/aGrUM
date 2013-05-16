#ifndef __LP_INTERFACE__H__
#define __LP_INTERFACE__H__

#include <map>
#include <string>       /// std::string
#include <iostream>     /// std::cout
#include <sstream>      /// std::ostringstream

#include <agrum/CN/LrsWrapper.h>


namespace gum {
	namespace credal {
		
		/* forward declaration */
		
		class LpCol;
		class LpRow;
		class LpInterface;
		class LpExpr;
		
		/** */
		
		class LpCol {
			public:
				/** @brief Default constructor. */
				LpCol ( unsigned int id );
				
				/** @brief Default destructor. */
				~LpCol ();
				
				/** Column id accessor. */
				unsigned int id () const;
				
				bool operator< ( const LpCol & col ) const;
				bool operator== ( const LpCol & col ) const;
				bool operator!= ( const LpCol & col ) const;
				
				void print () const;
				std::string toString () const;
				
				LpExpr operator- () const;
				
				template< typename SCALAR >
				LpExpr operator+ ( const SCALAR & f ) const;
				
				template< typename SCALAR >
				LpExpr operator- ( const SCALAR & f ) const;
				
				LpExpr operator+ ( const LpCol & col ) const;
				
				LpExpr operator- ( const LpCol & col ) const;
				
				LpExpr operator+ ( const LpExpr & expr ) const;
				
				LpExpr operator- ( const LpExpr & expr ) const;
				
				template< typename SCALAR >
				LpExpr operator* ( const SCALAR & f ) const;
				
				template< typename SCALAR >
				LpExpr operator<= ( const SCALAR & f ) const;
				
				LpExpr operator<= ( const LpCol & col ) const;
				
				LpExpr operator<= ( const LpExpr & expr ) const;
				
			protected:
				
			private:
				/** Column id. */
				unsigned int __id;
		};
		
		
		class LpRow {
			friend class LpInterface;
			
			public:
				LpRow ( const std::vector< LpCol > & cols );
				LpRow ( const LpExpr & expr, const std::vector< LpCol > & cols );
				
				~LpRow ();

				void print () const;
				
				std::string toString () const;
				
			protected:
				
			private:
				std::map< LpCol, double > __coeffs;
				double __cste;
				
		}; // end of class LpRow
		
		
		class LpInterface {
			
			public:
				/** @brief Default constructor, empty problem. */
				LpInterface ();
				
				/** @brief Default destructor. */
				~LpInterface ();
				
				/**
				 * @brief Insert a new column, i.e. a new variable.
				 * @return The column copy.
				 */
				LpCol addCol ();
				
				/**
				 * @brief Add rows to the linear programm according to expr which must be an inequality.
				 */
				void addRow ( const LpExpr & expr );
				
				/**
				 * @brief
				 * @return
				 */
				std::vector< std::vector< double > > solve ();
				
				void print () const;
				
				std::string toString () const;
				
				void clear();
				
			protected:
				
			private:
				/** Rows of the problem. */
				std::vector< LpRow > __rows;
				/** Variables of the problem. */
				std::vector< LpCol > __cols;
				
		}; // end of class LpInterface
		
		
		class LpExpr {
			friend class LpCol;
			friend class LpRow;
			friend class LpInterface;
			
			public:
				LpExpr ();
				
				~LpExpr ();
				
				LpExpr operator- () const;
				
				template< typename SCALAR >
				LpExpr operator+ ( const SCALAR & f ) const;
				
				template< typename SCALAR >
				friend LpExpr operator+ ( const SCALAR & f, const LpExpr & expr );
				
				LpExpr operator+ ( const LpCol & col ) const;
				
				LpExpr operator+ ( const LpExpr & expr ) const;
				
				template< typename SCALAR >
				LpExpr operator- ( const SCALAR & f ) const;
				
				template< typename SCALAR >
				friend LpExpr operator- ( const SCALAR & f, const LpExpr & expr );
				
				LpExpr operator- ( const LpCol & col ) const;
				
				LpExpr operator- ( const LpExpr & expr ) const;
				
				template< typename SCALAR >
				LpExpr operator<= ( const SCALAR & f ) const;
				
				template< typename SCALAR >
				friend LpExpr operator<= ( const SCALAR & f, const LpExpr & expr );
				
				LpExpr operator<= ( const LpCol & col ) const;
				
				LpExpr operator<= ( const LpExpr & expr ) const;
				
				void print () const;
				
				std::string toString () const;
				
				/// ////////////////////////////////////////////////////////////
				
				template< typename SCALAR >
				friend LpExpr operator+ ( const SCALAR & f, const LpCol & col );
				
				template< typename SCALAR >
				friend LpExpr operator- ( const SCALAR & f, const LpCol & col );
				
				template< typename SCALAR >
				friend LpExpr operator* ( const SCALAR & f, const LpCol & col );
				
				template< typename SCALAR >
				friend LpExpr operator<= ( const SCALAR & f, const LpCol & col );
				
				/// ////////////////////////////////////////////////////////////
				
			private:
				bool __ileft;
				bool __iright;
				
				double __lValue;
				double __mValue;
				double __rValue;
				
				std::map< LpCol, double > __lCoeffs;
				std::map< LpCol, double > __mCoeffs;
				std::map< LpCol, double > __rCoeffs;
				
				void __moveLeft ();
		};
		
		
		
	}	// end of namespace cn
} // end of namespace gum

#include <agrum/CN/LpInterface.tcc>

#endif