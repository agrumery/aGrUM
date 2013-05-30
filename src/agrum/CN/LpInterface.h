#ifndef __LP_INTERFACE__H__
#define __LP_INTERFACE__H__

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
				
				/** @brief Default destructor. */
				~LpCol ();
				
				/** Column id accessor. */
				unsigned int id () const;
				
				bool operator< ( const LpCol & col ) const;
				bool operator== ( const LpCol & col ) const;
				bool operator!= ( const LpCol & col ) const;
				
				LpCol & operator= ( const LpCol & col );
				
				friend std::ostream & operator<< ( std::ostream & out, const LpCol & col );
				
				void print () const;
				std::string toString () const;
				
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
			
			LpExpr ( const LpExpr & expr, bool copyLeft, bool copyMiddle, bool copyRight );
			
			/// move copy constructor
			LpExpr ( LpExpr && expr );
			/// be carefull when chaining moves with this one, as expr will be partially moved !!
			LpExpr ( LpExpr && expr, bool copyLeft, bool copyMiddle, bool copyRight );
			
			~LpExpr ();
			
			/// Operators =
			/// @{
				
			LpExpr & operator= ( const LpCol & rhs );
			
			LpExpr & operator= ( const LpExpr & rhs );
			
			LpExpr & operator= ( LpExpr && rhs );
			
			template< typename SCALAR >
			LpExpr & operator= ( const SCALAR & rhs );
			
			/// @}
			
			/// Unary operator -
			/// @{
				
			///LpExpr operator- ();
			
			/// @}
			
			/// Operators +=
			/// @{
				
			LpExpr & operator+= ( const LpCol & rhs );
			
			LpExpr & operator+= ( const LpExpr & rhs );
			
			LpExpr & operator+= ( LpExpr && rhs );
			
			template< typename T >
			LpExpr & operator+= ( const T & rhs );
			
			/// @}
			
			/// Operators -=
			/// @{
				
			LpExpr & operator-= ( const LpCol & rhs );
			
			LpExpr & operator-= ( const LpExpr & rhs );
			
			template< typename T >
			LpExpr & operator-= ( const T & rhs );
			
			friend std::ostream & operator<< ( std::ostream & out, const LpExpr & expr );
			
			/// @}
			
			template< typename SCALAR >
			friend LpExpr operator* ( const SCALAR & lhs, const LpCol & rhs );
			
			/// Used by operators <=
			/// @{
			
			/// ALWAYS inserted from left to right
			inline void addSide ( const LpCol & from );
			
			/// ALWAYS inserted from left to right
			inline void addSide ( const LpExpr & from );
			
			/// ALWAYS inserted from left to right
			inline void addSide ( LpExpr && from );
			
			/// ALWAYS inserted from left to right
			template< typename SCALAR >
			inline void addSide ( const SCALAR & from );
			
			/// @}
			
			void clear();
			
			void print () const;
			
			std::string toString () const;
			
			
		protected:
			
		private:
			bool __ileft;
			bool __imiddle;
			bool __iright;
			
			double __lValue;
			double __mValue;
			double __rValue;
			
			gum::HashTable< LpCol, double > * __lCoeffs;
			gum::HashTable< LpCol, double > * __mCoeffs;
			gum::HashTable< LpCol, double > * __rCoeffs;
			
		};
		
		
		class LpRow {
			friend class LpInterface;
			
			public:
				/// to use swap trick with vectors
				/// not intended to be used without expressions or by the user
				LpRow ();
				
				LpRow ( const LpExpr & expr, const std::vector< LpCol > & cols );
				
				LpRow ( LpExpr && expr, const std::vector< LpCol > & cols );
				
				LpRow ( const LpRow & row );
				
				LpRow ( LpRow && row );
				
				~LpRow ();
				
				LpRow & operator= ( const LpRow & row );
				LpRow & operator= ( LpRow && row );
				
				friend std::ostream & operator<< ( std::ostream & out, const LpRow & row );

				void print () const;
				
				std::string toString () const;
				
			protected:
				
			private:
				double __cste;
				
				gum::HashTable< LpCol, double > * __coeffs;
				
		}; // end of class LpRow
		
		
		class LpInterface {
			public:
				/** @brief Default constructor, empty problem. */
				LpInterface ();
				
				/** @brief Default destructor. */
				~LpInterface ();
				
				friend std::ostream & operator<< ( std::ostream & out, const LpInterface & lpi );
				
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
				 * @brief Add rows to the linear programm according to expr which must be an inequality.
				 */
				void addRow ( LpExpr && expr );
				
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
				std::vector< LpRow * > __rows;
				/** Variables of the problem. */
				std::vector< LpCol > __cols;
				
				/** \c true if addPositivity() has been called, \c false otherwise. */
				bool __positivity;
				/** \c true if addSumIsOne() has been called, \c false otherwise. */
				bool __sumIsOne;
				
		}; // end of class LpInterface
		
		/// used by move semantics to take ownership of the hashTables of an Rvalue reference ( either a or b, it doesn't matter ).
		/// the Rvalue reference will be deleted shortly after, and it's destructor will take care of the delete operations
		inline void swap ( gum::HashTable< LpCol, double > *& a, gum::HashTable< LpCol, double > *& b );
		
		/// non-members operators +
		/// @{
		
		template< typename T1, typename T2 >
		LpExpr operator+ ( const T1 & lhs, const T2 & rhs );
		
		/// Rvalue reference operators
		
		template< typename T1, typename T2 >
		LpExpr operator+ ( T1 && lhs, const T2 & rhs );
		
		template< typename T1, typename T2 >
		LpExpr operator+ ( const T1 & lhs, T2 && rhs );
		
		template< typename T1, typename T2 >
		LpExpr operator+ ( T1 && lhs, T2 && rhs );
		
		/// @}
		
		/// non-members operators -
		/// @{
		
		template< typename T1, typename T2 >
		LpExpr operator- ( const T1 & lhs, const T2 & rhs );
		
		/// Rvalue reference operators
		
		/// the only operator- that can use move semantics is on lsh. rhs cannot use move semantic because of - rhs
		template< typename T2 >
		LpExpr operator- ( LpExpr && lhs, const T2 & rhs );
		
		/// @}
		
		/// non-members operators *
		/// @{
		/// declared friend by LpExpr
		template< typename SCALAR >
		LpExpr operator* ( const SCALAR & lhs, const LpCol & rhs );
		
		template< typename SCALAR >
		LpExpr operator* ( const LpCol & lhs, const SCALAR & rhs );
		
		/// @}
		
		/// non-members operators <=
		/// @{
			
		template< typename T1, typename T2 >
		LpExpr operator<= ( const T1 & lhs , const T2 & rhs );
		
		/// Rvalue reference operators
		
		template< typename T1, typename T2 >
		LpExpr operator<= ( T1 && lhs , const T2 & rhs );
		
		template< typename T1, typename T2 >
		LpExpr operator<= ( const T1 & lhs , T2 && rhs );
		
		template< typename T1, typename T2 >
		LpExpr operator<= ( T1 && lhs , T2 && rhs );
		
		/// @}
		
	}	// end of namespace cn
	
} // end of namespace gum

#include <agrum/CN/LpInterface.tcc>

#endif