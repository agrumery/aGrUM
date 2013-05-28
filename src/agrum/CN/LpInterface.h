#ifndef __LP_INTERFACE__H__
#define __LP_INTERFACE__H__

///#include <map>
#include <string>       /// std::string
#include <iostream>     /// std::cout
#include <sstream>      /// std::ostringstream

#include <agrum/CN/LrsWrapper.h>
#include <agrum/core/hashFunc.h>
#include <agrum/core/hashTable.h>

namespace gum {
	namespace credal {
		
		/* forward declaration */
		
		class LpCol;
		class LpRow;
		class LpInterface;
		class LpExpr;
		
		class LpCol {
			public:
				/** @brief Default constructor. */
				LpCol ( unsigned int id );
				
				LpCol ( const LpCol & col );
				///LpCol ( LpCol && col );
				
				/** @brief Default destructor. */
				~LpCol ();
				
				/** Column id accessor. */
				unsigned int id () const;
				
				bool operator< ( const LpCol & col ) const;
				bool operator== ( const LpCol & col ) const;
				bool operator!= ( const LpCol & col ) const;
				
				/// @warning : SELF assignment !!! test : this == &col -> return *this directly
				/// @warning : return CONST ref to avoid (x=y)=z compiling ... but loose (x=y).nonConstFunc ???
				LpCol & operator= ( const LpCol & col );
				///LpCol & operator= ( LpCol && col );
				
				void print () const;
				std::string toString () const;
				
				
				LpExpr operator- () const;
				
				
				LpExpr operator+ ( const LpCol & col ) const;
				
				LpExpr operator+ ( const LpExpr & expr ) const;
				
				template< typename SCALAR >
				LpExpr operator+ ( const SCALAR & f ) const;
				
				
				LpExpr operator- ( const LpCol & col ) const;
				
				LpExpr operator- ( const LpExpr & expr ) const;
				
				template< typename SCALAR >
				LpExpr operator- ( const SCALAR & f ) const;
				
				
				template< typename SCALAR >
				LpExpr operator* ( const SCALAR & f ) const;
				
				
				LpExpr operator<= ( const LpCol & col ) const;
				
				LpExpr operator<= ( const LpExpr & expr ) const;
				
				template< typename SCALAR >
				LpExpr operator<= ( const SCALAR & f ) const;
				
			protected:
				
			private:
				/** @brief Column (variable) id. */
				unsigned int __id;
		};
		
	} // end of namespace credal
	
	template <> 
	class HashFunc< credal::LpCol > : public HashFuncBase< credal::LpCol > {
	public:
		Size operator() ( const credal::LpCol & key ) const;
	};
		
	namespace credal {
		
		class LpExpr {
			friend class LpCol;
			friend class LpRow;
			friend class LpInterface;
			
		public:
			LpExpr ();
			
			LpExpr ( const LpExpr & expr );
			
			~LpExpr ();
			
			
			LpExpr operator- () const;
			
			
			LpExpr operator+ ( const LpCol & col ) const;
			
			LpExpr operator+ ( const LpExpr & expr ) const;
			
			template< typename SCALAR >
			LpExpr operator+ ( const SCALAR & f ) const;
			
			template< typename SCALAR >
			friend LpExpr operator+ ( const SCALAR & f, const LpCol & col );
			
			template< typename SCALAR >
			friend LpExpr operator+ ( const SCALAR & f, const LpExpr & expr );
			
			
			LpExpr operator- ( const LpCol & col ) const;
			
			LpExpr operator- ( const LpExpr & expr ) const;
			
			template< typename SCALAR >
			LpExpr operator- ( const SCALAR & f ) const;
			
			template< typename SCALAR >
			friend LpExpr operator- ( const SCALAR & f, const LpCol & col );
			
			template< typename SCALAR >
			friend LpExpr operator- ( const SCALAR & f, const LpExpr & expr );
			
			
			template< typename SCALAR >
			friend LpExpr operator* ( const SCALAR & f, const LpCol & col );
			
			
			LpExpr operator<= ( const LpCol & col ) const;
			
			LpExpr operator<= ( const LpExpr & expr ) const;
			
			template< typename SCALAR >
			LpExpr operator<= ( const SCALAR & f ) const;
			
			template< typename SCALAR >
			friend LpExpr operator<= ( const SCALAR & f, const LpCol & col );
			
			template< typename SCALAR >
			friend LpExpr operator<= ( const SCALAR & f, const LpExpr & expr );
			
			
			LpExpr & operator+= ( const LpCol & col );
			
			LpExpr & operator+= ( const LpExpr & expr );
			
			template< typename SCALAR >
			LpExpr & operator+= ( const SCALAR & f );
			
			
			LpExpr & operator-= ( const LpCol & col );
			
			LpExpr & operator-= ( const LpExpr & expr );
			
			template< typename SCALAR >
			LpExpr & operator-= ( const SCALAR & f );
			
			
			LpExpr & operator= ( const LpCol & col );
			
			LpExpr & operator= ( const LpExpr & expr );
			
			template< typename SCALAR >
			LpExpr & operator= ( const SCALAR & f );
			
			
			void print () const;
			
			std::string toString () const;
			
			void clear();
			
		private:
			bool __ileft;
			bool __iright;
			
			double __lValue;
			double __mValue;
			double __rValue;
			
			gum::HashTable< LpCol, double > __lCoeffs;
			gum::HashTable< LpCol, double > __mCoeffs;
			gum::HashTable< LpCol, double > __rCoeffs;
			
			void __moveLeft ();
			
			inline void __copy ( const LpExpr & from );
			inline void __copyLeft ( const LpExpr & from );
			inline void __copyMiddle ( const LpExpr & from );
			inline void __copyRight ( const LpExpr & from );
		};
		
		
		class LpRow {
			friend class LpInterface;
			
			public:
				LpRow ( const LpExpr & expr, const std::vector< LpCol > & cols );
				
				LpRow ( const LpRow & row );
				
				~LpRow ();

				void print () const;
				
				std::string toString () const;
				
			protected:
				
			private:
				gum::HashTable< LpCol, double > __coeffs;
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
				 * @brief Insert new columns, i.e. new variables.
				 * @return The columns copy.
				 */
				std::vector< LpCol > addCols ( const unsigned int & cols );
				
				/**
				 * @brief Add rows to the linear programm according to expr which must be an inequality.
				 */
				void addRow ( const LpExpr & expr );
				
				/**
				 * @brief Add positivity constraints for all variables
				 */
				void addPositivity ();
				
				/**
				 * @brief Add sum of variables is 1 constraints
				 */
				void addSumIsOne ();
				
				/**
				 * Add positivity constraints and sum of variables is 1 ( probability constraints )
				 */
				void addProba ();
				
				/**
				 * @brief Solve the linear program (H-representation of the polytope) by enumeration (of the polytope vertices) using lexicographic reverse search (lrs).
				 * 
				 * Only works with bounded polyhedron (polytopes) and not unbounded ones (i.e. defined by vertices and rays).
				 * 
				 * @return The V-representation (vertices) of the polytope as a vector of vectors (vector of vertices).
				 */
				std::vector< std::vector< double > > solve ();
				
				/**
				 * @brief Get the colums of the LP, i.e. the variables.
				 * @return A copy of the variables as a vector of (copied) variables.
				 */
				std::vector< LpCol > getCols () const;
				
				/**
				 * @brief Print on std out the representation of the LP.
				 */
				void print () const;
				
				/**
				 * @brief Get a string representation of the LP.
				 * @return A string representation of the LP.
				 */
				std::string toString () const;
				
				/**
				 * @brief Reset the rows (inequalities) and columns (variables) of the LP as if it was created.
				 */
				void clear ();
				/**
				 * @brief Reset the rows (inequalities) of the LP but not the columns (variables are kept).
				 */
				void clearRows ();
				
			protected:
				
			private:
				/** Rows of the problem. */
				std::vector< LpRow > __rows;
				/** Variables of the problem. */
				std::vector< LpCol > __cols;
				
				/** \c true if addPositivity() has been called, \c false otherwise. */
				bool __positivity;
				/** \c true if addSumIsOne() has been called, \c false otherwise. */
				bool __sumIsOne;
				
		}; // end of class LpInterface
		
		
	}	// end of namespace cn
	
} // end of namespace gum

#include <agrum/CN/LpInterface.tcc>

#endif