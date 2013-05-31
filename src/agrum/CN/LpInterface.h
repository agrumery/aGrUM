#ifndef __LP_INTERFACE__H__
#define __LP_INTERFACE__H__

/**
 * @file
 * @brief Class representing a polytope ( credal set ) as linear constraints
 */

#include <string>       /// std::string
#include <iostream>     /// std::cout
#include <sstream>      /// std::ostringstream

#include <agrum/CN/LrsWrapper.h>
#include <agrum/core/hashFunc.h>
#include <agrum/core/hashTable.h>

namespace gum {
	namespace credal {
		
		class LpCol;
		class LpRow;
		class LpInterface;
		class LpExpr;
		
		/**
		 * @class LpCol LpInterface.h <agrum/CN/LpInterface.h>
		 * @brief Class representing a variable ( a column ) of a linear program, i.e. a dimension of the problem.
		 */
		class LpCol {
			public:
				/** 
				 * @brief Default constructor.
				 * @param id The id of the variable.
				 */
				LpCol ( unsigned int id );
				
				/** 
				 * @brief Default copy constructor.
				 * @param col The constant reference to the variable to copy.
				 */
				LpCol ( const LpCol & col );
				
				/** @brief Default destructor. */
				~LpCol ();
				
				/** 
				 * @brief Variable id accessor. 
				 * @return The id of the variable.
				 */
				unsigned int id () const;
				
				/** 
				 * @brief Test of ordering \c < between two variables.
				 * 
				 * For use with maps, hashTable etc
				 * @param col The constant reference to the variable to compare to this ( the caller ).
				 * @return \c True if this id is smaller than \c col id, false otherwise.
				 */
				bool operator< ( const LpCol & col ) const;
				
				/** 
				 * @brief Test of equality \c == between two variables.
				 * 
				 * For use with maps, hashTable etc
				 * @param col The constant reference to the variable to compare to this ( the caller ).
				 * @return \c True if this id is equals to \c col id, false otherwise.
				 */
				bool operator== ( const LpCol & col ) const;
				
				/** 
				 * @brief Opposite of equality \c != test between two variables.
				 * 
				 * For use with maps, hashTable etc
				 * @param col The constant reference to the variable to compare to this ( the caller ).
				 * @return \c True if this id is not the same than \c col id, false otherwise.
				 */
				bool operator!= ( const LpCol & col ) const;
				
				/**
				 * @brief Assignment operator \c = by copy.
				 * @param col The constant reference to the variable to copy in this ( the caller ).
				 * @return The address of the caller.
				 */
				LpCol & operator= ( const LpCol & col );
				
				/**
				 * @brief Overload of \c << to use with output streams ( such as std::cout << ).
				 * @param out The reference to the caller, i.e. left side of \c <<.
				 * @param col The constant reference to the variable whose representation we want.
				 * @return The address of the caller.
				 */
				friend std::ostream & operator<< ( std::ostream & out, const LpCol & col );
				
				/**
				 * @brief Print the representation of a calling variable on std::cout.
				 */
				void print () const;
				
				/**
				 * @brief Get the string representation of a calling variable.
				 * @return The string representation of the calling variable.
				 */
				std::string toString () const;
				
			protected:
				
			private:
				/** @brief Variable id. */
				unsigned int __id;
		};
		
	} // end of namespace credal
	
	/**
	 * @class HashFunc LpInterface.h <agrum/CN/LpInterface.h>
	 * @brief Class template representing hashing function of \c LpCol. To use aGrUM hashTable.
	 */
	template <> 
	class HashFunc< credal::LpCol > : public HashFuncBase< credal::LpCol > {
	public:
		/**
		 * @brief Overload of operator () to compute hash of a variable \c LpCol.
		 * @param key The constant reference to the variable whose hash we want to compute.
		 */
		Size operator() ( const credal::LpCol & key ) const;
	};
		
	namespace credal {
		
		/**
		 * @class LpExpr LpInterface.h <agrum/CN/LpInterface.h>
		 * @brief Class representing a linear expression.
		 */
		class LpExpr {
			friend class LpCol;
			friend class LpRow;
			friend class LpInterface;
			
		public:
			/// @name Constructors / Destructors
			/// @{
			
			/** @brief Default constructor. */
			LpExpr ();
			
			/** 
			 * @brief Copy constructor.
			 * @param expr The constant reference to the expression to copy.
			 */
			LpExpr ( const LpExpr & expr );
			
			/** 
			 * @brief Copy constructor. Makes a ( partial ) copy.
			 * @param expr The constant reference to the expression to copy.
			 * @param copyLeft \c True if we want to copy left side of \c expr, \c False otherwise.
			 * @param copyMiddle \c True if we want to copy middle side of \c expr, \c False otherwise.
			 * @param copyRight \c True if we want to copy right side of \c expr, \c False otherwise.
			 */
			LpExpr ( const LpExpr & expr, bool copyLeft, bool copyMiddle, bool copyRight );
			
			/** 
			 * @brief Move copy constructor from temporary.
			 * 
			 * Swap ressources between the temporary \c expr and the caller instead of copying the temporary.
			 * @param expr The temporary expression to move to this.
			 */
			LpExpr ( LpExpr && expr );
			
			/** 
			 * @brief Move copy constructor. Makes ( partial ) moves.
			 * 
			 * @warning Proceed with care; partial moves are dangerous because the object being moved may still briefly exist after being moved and could be used afterward.
			 * 
			 * @param expr The temporary expression to ( partially ) move to this.
			 * @param copyLeft \c True if we want to copy left side of \c expr, \c False otherwise.
			 * @param copyMiddle \c True if we want to copy middle side of \c expr, \c False otherwise.
			 * @param copyRight \c True if we want to copy right side of \c expr, \c False otherwise.
			 */
			LpExpr ( LpExpr && expr, bool copyLeft, bool copyMiddle, bool copyRight );
			
			/** @brief Default destructor. */
			~LpExpr ();
			
			/// @}
			
			/// @name Assignment operators =
			/// @{
			
			/**
			 * @brief Assignment operator \c = with a variable.
			 * @param rhs the constant reference to the variable to form an expression with.
			 * @return The address of the calling expression.
			 */
			LpExpr & operator= ( const LpCol & rhs );
			
			/**
			 * @brief Assignment operator \c = with another expression, by copy.
			 * @param rhs the constant reference to the expression to copy.
			 * @return The address of the calling expression.
			 */
			LpExpr & operator= ( const LpExpr & rhs );
			
			/**
			 * @brief Assignment operator \c = with another expression, by move.
			 * @param rhs the expression to move to this.
			 * @return The address of the calling expression.
			 */
			LpExpr & operator= ( LpExpr && rhs );
			
			/**
			 * @brief Assignment operator \c = with a scalar.
			 * @param rhs the constant reference to the scalar to form an expression with.
			 * @return The address of the calling expression.
			 */
			template< typename SCALAR >
			LpExpr & operator= ( const SCALAR & rhs );
			
			/// @}
			
			
			/// @name Compound assignment operators +=
			/// @{
			
			/**
			 * @brief Compound assignment operator \c += with a variable.
			 * @param rhs the constant reference to the variable to add to the calling expression.
			 * @return The address of the calling expression.
			 */
			LpExpr & operator+= ( const LpCol & rhs );
			
			/**
			 * @brief Compound assignment operator \c += with another expression.
			 * @param rhs the constant reference to the expression to add to the calling expression.
			 * @return The address of the calling expression.
			 */
			LpExpr & operator+= ( const LpExpr & rhs );
			
			/**
			 * @brief Compound assignment operator \c += with another temporary expression.
			 * @param rhs the temporary expression to add to the calling expression.
			 * @return The address of the calling expression.
			 */
			LpExpr & operator+= ( LpExpr && rhs );
			
			/**
			 * @brief Compound assignment operator \c += with a scalar.
			 * @param rhs the constant reference to the scalar to add to the calling expression.
			 * @return The address of the calling expression.
			 */
			template< typename T >
			LpExpr & operator+= ( const T & rhs );
			
			/// @}
			
			/// @name Compound assignment operators -=
			/// @{
				
			/**
			 * @brief Compound assignment operator \c -= with a variable.
			 * @param rhs the constant reference to the variable to substract from the calling expression.
			 * @return The address of the calling expression.
			 */
			LpExpr & operator-= ( const LpCol & rhs );
			
			/**
			 * @brief Compound assignment operator \c -= with another expression.
			 * @param rhs the constant reference to the expression to substract from the calling expression.
			 * @return The address of the calling expression.
			 */
			LpExpr & operator-= ( const LpExpr & rhs );
			
			/**
			 * @brief Compound assignment operator \c -= with a salar.
			 * @param rhs the constant reference to the scalar to substract from the calling expression.
			 * @return The address of the calling expression.
			 */
			template< typename T >
			LpExpr & operator-= ( const T & rhs );
			
			/// @}
			
			/**
			 * @brief Overload of \c << to use with output streams ( such as std::cout << ).
			 * @param out the reference to the caller, i.e. left side of \c <<.
			 * @param expr the constant reference to the expression whose representation we want.
			 * @return The address of the caller.
			 */
			friend std::ostream & operator<< ( std::ostream & out, const LpExpr & expr );
			
			/** @brief Non-member friend operator *. Not detailed here as not a member. */
			template< typename SCALAR >
			friend LpExpr operator* ( const SCALAR & lhs, const LpCol & rhs );
			
			/** @brief Non-member friend operator <=. Not detailed here as not a member. */
			template< typename T1, typename T2 >
			friend LpExpr operator<= ( T1 && lhs, T2 && rhs );
			
			/** @brief Clear all data of the calling expression as if it was constructed. */
			void clear();
			
			/**
			 * @brief Print the representation of a calling expression on std::cout.
			 */
			void print () const;
			
			/**
			 * @brief Get the string representation of a calling expression.
			 * @return The string representation of the calling expression.
			 */
			std::string toString () const;
			
			
		protected:
			
		private:
			/** @brief \c True if this expression has a non-empty left side L : L <= M <= R . \c False otherwise. */
			bool __ileft;
			/** @brief \c True if this expression has a non-empty middle side M ( the default ) : L <= M <= R . \c False otherwise. */
			bool __imiddle;
			/** @brief \c True if this expression has a non-empty right side R : L <= M <= R . \c False otherwise. */
			bool __iright;
			
			/** @brief The constant on the left side L : L <= M <= R */
			double __lValue;
			/** @brief The constant on the middle side L : L <= M <= R */
			double __mValue;
			/** @brief The constant on the right side L : L <= M <= R */
			double __rValue;
			
			/** @brief The coefficients of each variable on the left side L : L <= M <= R. If a variable is not present, it's coefficient is 0. */
			gum::HashTable< LpCol, double > * __lCoeffs;
			/** @brief The coefficients of each variable on the middle side L : L <= M <= R. If a variable is not present, it's coefficient is 0. */
			gum::HashTable< LpCol, double > * __mCoeffs;
			/** @brief The coefficients of each variable on the right side L : L <= M <= R. If a variable is not present, it's coefficient is 0. */
			gum::HashTable< LpCol, double > * __rCoeffs;
			
			/// @name Used by operator <=
			/// @{
			
			/**
			 * @brief Set the side of the calling expression, from LEFT TO RIGHT : L <= M <= R.
			 * @param from the constant reference to the variable ( the side ) to put on the first empty side met, starting at left.
			 */
			inline void __addSide ( const LpCol & from );
			
			/**
			 * @brief Copy an expression to a side of the calling expression, from LEFT TO RIGHT : L <= M <= R.
			 * @param from the constant reference to the expression ( the side ) to copy on the first empty side met, starting at left.
			 */
			inline void __addSide ( const LpExpr & from );
			
			/**
			 * @brief Move an expression to a side of the calling expression, from LEFT TO RIGHT : L <= M <= R.
			 * @param from the expression ( the side ) to move on the first empty side met, starting at left.
			 */
			inline void __addSide ( LpExpr && from );
			
			/**
			 * @brief Set the side of the calling expression, from LEFT TO RIGHT : L <= M <= R.
			 * @param from the constant reference to the scalar ( the side ) to put on the first empty side met, starting at left.
			 */
			template< typename SCALAR >
			inline void __addSide ( const SCALAR & from );
			
			/// @}
			
		};
		
		/**
		 * @class LpRow LpInterface.h <agrum/CN/LpInterface.h>
		 * @brief Class representing a row of the linear program, i.e. an inequality.
		 */
		class LpRow {
			friend class LpInterface;
			
			public:
				/// @name Constructors / Destructors
				/// @{
				
				/**
				 * @brief Constructor from an expression and the address of the vector of variables of the problem.
				 * @param expr the constant reference to the expression to convert into rows ( inequalities ).
				 * @param cols the constant reference to the vector of variables of the problem.
				 */
				LpRow ( const LpExpr & expr, const std::vector< LpCol > & cols );
				
				/**
				 * @brief Move constructor from a temporary expression and the address of the vector of variables of the problem.
				 * @param expr the temporary expression to move into rows ( inequalities ).
				 * @param cols the constant reference to the vector of variables of the problem.
				 */
				LpRow ( LpExpr && expr, const std::vector< LpCol > & cols );
				
				/**
				 * @brief Copy constructor.
				 * @param row The constant reference to the row to be copied.
				 */
				LpRow ( const LpRow & row );
				
				/**
				 * @brief Move copy constructor from temporary.
				 * @param row The temporary row to be copied.
				 */
				LpRow ( LpRow && row );
				
				/** @brief Default destructor. */
				~LpRow ();
				
				/// @}
				
				/// @name Assignment operators
				/// @{
				
				/**
				 * @brief
				 * @param row The constant reference to the row to be copied.
				 */
				LpRow & operator= ( const LpRow & row );
				/**
				 * @brief
				 * @param row The temporary row to be moved to this.
				 */
				LpRow & operator= ( LpRow && row );
				
				/// @}
				
				/**
				 * @brief Overload of \c << to use with output streams ( such as std::cout << ).
				 * @param out the reference to the caller, i.e. left side of \c <<.
				 * @param row the constant reference to the row whose representation we want.
				 * @return The address of the caller.
				 */
				friend std::ostream & operator<< ( std::ostream & out, const LpRow & row );

				/**
				 * @brief Print the representation of a calling row on std::cout.
				 */
				void print () const;
				
				/**
				 * @brief Get the string representation of a calling row.
				 * @return The string representation of the calling row.
				 */
				std::string toString () const;
				
			protected:
				
			private:
				/** @brief The constant of the linear inequality. */
				double __cste;
				
				/** @brief The coefficients of the variables of the linear inequality. */
				gum::HashTable< LpCol, double > * __coeffs;
				
		}; // end of class LpRow
		
		
		class LpInterface {
			public:
				/// @name Constructor / Destructor
				/// @{
					
				/** @brief Default constructor, empty problem. */
				LpInterface ();
				
				/** @brief Default destructor. */
				~LpInterface ();
				
				/// @}
				
				/**
				 * @brief Overload of \c << to use with output streams ( such as std::cout << ).
				 * @param out the reference to the caller, i.e. left side of \c <<.
				 * @param lpi the constant reference to the linear program whose representation we want.
				 * @return The address of the caller.
				 */
				friend std::ostream & operator<< ( std::ostream & out, const LpInterface & lpi );
				
				/**
				 * @brief Insert a new column, i.e. a new variable.
				 * @return A copy of the variable.
				 */
				LpCol addCol ();
				
				/**
				 * @brief Insert new columns, i.e. new variables.
				 * @param cols the constant reference to the number of variables we want.
				 * @return The copy of the vector of all variables of the problem.
				 */
				std::vector< LpCol > addCols ( const unsigned int & cols );
				
				/**
				 * @brief Add rows to the linear program according to a given expression ( which must be at least an inequality ).
				 * @param expr the constant reference to the expression to convert to rows.
				 */
				void addRow ( const LpExpr & expr );
				
				/**
				 * @brief Add rows to the linear program according to a given expression ( which must be at least an inequality ).
				 * @param expr the temporary expression to move to rows.
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
				 * @brief Add positivity constraints and sum of variables is 1 ( probability constraints )
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
				 * @brief Get the variables of the LP.
				 * @return A copy of the variables as a vector of variables.
				 */
				std::vector< LpCol > getCols () const;
				
				/**
				 * @brief Print the representation of a calling linear program on std::cout.
				 */
				void print () const;
				
				/**
				 * @brief Get the string representation of a calling linear program.
				 * @return The string representation of the calling linear program.
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
				/** @brief Rows of the problem. */
				std::vector< LpRow * > __rows;
				/** @brief Variables of the problem. */
				std::vector< LpCol > __cols;
				
				/** @brief \c true if addPositivity() has been called, \c false otherwise. */
				bool __positivity;
				/** @brief \c true if addSumIsOne() has been called, \c false otherwise. */
				bool __sumIsOne;
				
		}; // end of class LpInterface
		
		/**
		 * @brief Swap the addresses of two pointers to hashTables.
		 * 
		 * Used by move operations as the temporary will take care of the delete operations.
		 * @warning DO NOT call this directly ( one of the object owning a pointer must be a temporary, i.e. an "rvalue reference" ).
		 * @param a the address of the first pointer.
		 * @param b the address of the second pointer.
		 */
		inline void swap ( gum::HashTable< LpCol, double > *& a, gum::HashTable< LpCol, double > *& b );
		
		/// @name Non-members operators +
		/// @{
			
		/**
		 * @brief Overload of operator \c + between anything ( a scalar, a variable or an expression ) and a temporary expression.
		 * 
		 * This overload is used because only one side of the operator profit from move operation; the one with a temporary expression. 
		 * If the expression if on the right hand side, we always add it first to profit from move operation. Otherwise another overload is used.
		 * 
		 * Implements both the copy operator \c + and move operator \c + because operator \c+ is implemented on top of operators \c +=.
		 * Because of template type deduction \c lhs is a "universal reference", i.e. either an lvalue or an rvalue reference. 
		 * To force rvalue reference with template deduction one should use const : const T && lhs : lhs is an rvalue reference and WILL be modofied ( not intuitive const ).
		 * Therefor std::forward must be used to forward the type of \c lhs. \c rhs is an rvalue reference and std::move must be used to forward it's type.
		 * @param lhs the universal reference to either a scalar, a variable or an expression on the left hand side of the operator.
		 * @param rhs the rvalue reference to the temporary expression on the right hand side of the operator.
		 * @return An expression which yields the result of \c lhs \c + \c rhs.
		 */
		template< typename T1 >
		LpExpr operator+ ( T1 && lhs, LpExpr && rhs );
		
		/**
		 * @brief Overload of operator \c + between anything ( a scalar, a variable or an expression ) and anything except a temporary expression.
		 * 
		 * Implements both the copy operator \c + and move operator \c + because operator \c+ is implemented on top of operators \c +=.
		 * Because of template type deduction both \c lhs and \c rhs are "universal references", i.e. either an lvalue or an rvalue reference.
		 * Therefor std::forward must be used to forward the type of \c lhs and \c rhs.
		 * @param lhs the universal reference to either a scalar, a variable or an expression on the left hand side of the operator.
		 * @param rhs the universal reference to either a scalar or a variable on the right hand side of the operator.
		 * @return An expression which yields the result of \c lhs \c + \c rhs.
		 */
		template< typename T1, typename T2 >
		LpExpr operator+ ( T1 && lhs, T2 && rhs );
		
		/// @}
		
		/// @name Non-members operators -
		/// @{
		
		/**
		 * @brief Overload of operator \c - between a temporary ( rvalue reference ) expression and anything.
		 * 
		 * This overload is used because only one side of the operator profit from move operation; the left side with a temporary expression.
		 * std::move must be used on \c lhs.
		 * @param lhs the rvalue reference to the temporary expression on the left hand side of the operator.
		 * @param rhs the constant reference to the right hand side of the operator.
		 * @return An expression which yields the result of \c lhs \c - \c rhs.
		 */
		template< typename T2 >
		LpExpr operator- ( LpExpr && lhs, const T2 & rhs );
			
		/**
		 * @brief Overload of operator \c - between anything except a temporary expression and anything. 
		 * 
		 * No move operations since left side is not a temporary expression.
		 * @param lhs the constant reference to the left hand side of the operator.
		 * @param rhs the constant reference to the right hand side of the operator.
		 * @return An expression which yields the result of \c lhs \c - \c rhs.
		 */
		template< typename T1, typename T2 >
		LpExpr operator- ( const T1 & lhs, const T2 & rhs );
		
		/// @}
		
		/// @name Non-members operators *
		/// @{
		
		/**
		 * @brief Overload of operator \c * between a scalar and a variable.
		 * 
		 * @param lhs the constant reference to the scalar on the left hand side of the operator.
		 * @param rhs the constant reference to the variable on the right hand side of the operator.
		 * @return An expression which yields the result of \c lhs \c * \c rhs.
		 */
		template< typename SCALAR >
		LpExpr operator* ( const SCALAR & lhs, const LpCol & rhs );
		
		/**
		 * @brief Overload of operator \c * between a variable and a scalar.
		 * 
		 * @param lhs the constant reference to the variable on the left hand side of the operator.
		 * @param rhs the constant reference to the scalar on the right hand side of the operator.
		 * @return An expression which yields the result of \c lhs \c * \c rhs.
		 */
		template< typename SCALAR >
		LpExpr operator* ( const LpCol & lhs, const SCALAR & rhs );
		
		/// @}
		
		/// @name Non-members operators <=
		/// @{
		
		/**
		 * @brief Overload of operator \c <= between anything and anything.
		 * 
		 * Implements both the copy operator \c <= and move operator \c <=.
		 * Because of template type deduction both \c lhs and \c rhs are "universal references", i.e. either an lvalue or an rvalue reference.
		 * Therefor std::forward must be used to forward the type of \c lhs and \c rhs.
		 * @param lhs the universal reference to either a ( temporary ) scalar, a ( temporary ) variable or a ( temporary ) expression on the left hand side of the operator.
		 * @param rhs the universal reference to either a ( temporary ) scalar, a ( temporary ) variable or a ( temporary ) expression on the right hand side of the operator.
		 * @return An expression which yields the result of \c lhs \c <= \c rhs.
		 */
		template< typename T1, typename T2 >
		LpExpr operator<= ( T1 && lhs, T2 && rhs );
		
		/// @}
		
	}	// end of namespace cn
	
} // end of namespace gum

#include <agrum/CN/LpInterface.tcc>

#endif