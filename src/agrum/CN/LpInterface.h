#ifndef __LP_INTERFACE__H__
#define __LP_INTERFACE__H__

#include <map>
#include <agrum/CN/LrsWrapper.h>

namespace gum {
	namespace cn {
		
		class LpCol {
			public:
				/** @brief Default constructor. */
				LpCol ( unsigned int id ) { __id = id; }
				
				/** @brief Default destructor. */
				~LpCol () {};
				
				/** Column id accessor. */
				unsigned int id () const { return __id; }
				
				bool operator< ( const LpCol & col ) const { return ( __id < col.id() ); }
				bool operator== ( const LpCol & col ) const { return ( __id == col.id() ); }
				bool operator!= ( const LpCol & col ) const { return ( __id != col.id() ); }
				
				std::string toString() const {
					return "col_" + std::to_string( __id );
				};
				
			protected:
				
			private:
				/** Column id. */
				unsigned int __id;
		};
		
		template< typename GUM_SCALAR >
		class LpInterface;
		
		template< typename GUM_SCALAR >
		class LpRow {
			friend class LpInterface< GUM_SCALAR >;
			
			typedef GUM_SCALAR coeff;
			
			public:
				LpRow ( const std::map< LpCol, bool > & cols ) {
					for ( const auto & entry : cols )
						__coeffs.insert( std::pair< LpCol, coeff>( entry.first, 0. ) );
					
					__lower = 0.;
					__upper = 1.;
				};
				
				~LpRow () {};
				
				void setBounds ( const coeff & lower, const coeff & upper ) {
					__lower = lower;
					__upper = upper;
				};
				
				void setColCoeff ( const LpCol & col, const coeff & c, const coeff & lower = -1., const coeff & upper = -1. ) {
					__coeffs.at( col ) = c;
					
					if ( lower != -1. )
						__lower = lower;
					if ( upper != -1. )
						__upper = upper;
				};
				
				void setColsCoeffs ( const std::map< LpCol, coeff > & c, const coeff & lower = -1., const coeff & upper = -1. ) {
					//if ( c.size() != __coeffs.size() )
						//GUM_ERROR ( gum::SizeError, "setColsCoeffs : Sizes mismatch : " << c.size() << " != " << __coeffs.size() );
					
					for ( const auto & entry : c ) {
						__coeffs.at( entry.first ) = entry.second;
					}
					
					if ( lower != -1. )
						__lower = lower;
					if ( upper != -1. )
						__upper = upper;
				};
				
			protected:
				std::vector< std::vector< GUM_SCALAR > > _expandRow () const {
					std::vector< std::vector< GUM_SCALAR > > matrix;
					
					// state <= upper is useless if upper == 1 since we have sum = 1 and positivity constraints, state <= 1 is redundant
					
					std::vector< GUM_SCALAR > boundInf ( __coeffs.size() + 1, 0 );
					
					unsigned int vars = 0;
					for ( const auto & entry : __coeffs ) {
						boundInf[ entry.first.id() + 1 ] = entry.second;
						if ( entry.second != 0. )
							vars++;
					}
					
					// obviously positivity constraints are added once at the end and not for each row
					if ( __lower != 0. || vars > 1 ) {
						boundInf[ 0 ] = -__lower;
					
						matrix.push_back( boundInf );
					}
					
					if ( __upper != 1. ) {
						std::vector< GUM_SCALAR > boundSup ( __coeffs.size() + 1, 0 );
						
						for ( const auto & entry : __coeffs )
							boundSup[ entry.first.id() + 1 ] = -entry.second;
						
						boundSup[ 0 ] = __upper;
						
						matrix.push_back( boundSup );
					}
					
					return matrix;
				};
				
			private:
				std::map< LpCol, coeff > __coeffs;
				coeff __lower, __upper; /// -1 means no bounds other than 0 <= ... <= 1 on states
				
				///unsigned int __id;
				
		}; // end of class LpRow
		
		
		template< typename GUM_SCALAR >
		class LpInterface {
			public:
				/** @brief Default constructor, empty problem. */
				LpInterface () {};
				
				/** @brief Default destructor. */
				~LpInterface () {};
				
				/**
				 * @brief Insert a new column, i.e. a new variable.
				 * @return The column.
				 */
				const LpCol & addCol () {
					__cols.insert( std::pair< LpCol, bool>( LpCol ( __cols.size() ), true ) );
					
					///__cols.push_back( LpCol ( __cols.size() ) );
					///__positivity.insert( std::pair< LpCol, GUM_SCALAR >( __cols[ __cols.size() - 1 ], true ) );
					
					return __cols.rbegin()->first; /// col ids are in increasing order, so this works, but be careful
				};
				
				/**
				 * @brief Adds a row to the linear programm.
				 */
				LpRow< GUM_SCALAR > & addRow () {
					__rows.push_back( LpRow< GUM_SCALAR > ( __cols ) );
					return __rows[ __rows.size() - 1 ];
				};
				
				/**
				 * @brief
				 * @return
				 */
				std::vector< std::vector< GUM_SCALAR > > solve () {
					gum::credal::LRSWrapper< GUM_SCALAR > lrs;
					
					lrs.setUpH( __cols.size() );
					//////////////////////////////////////////
					lrs.fillMatrix( _expandAllRows() ); /// @todo !! redo fillMatrix !!
					/////////////////////////////////////////
					lrs.H2V();
					
					return lrs.getOutput();
				};
				
				
			protected:
				/**
				 * @brief
				 * @return
				 */
				std::vector< std::vector< GUM_SCALAR > > _expandAllRows () {
					std::vector< std::vector< GUM_SCALAR > > matrix;
					
					for ( const auto & row : __rows ) {
						
						if ( row.__coeffs.size() == 1 ) {
							for ( const auto & entry : row.__coeffs ) {
								if ( entry.second > 0 ) {
									//__positivity.at( entry.first ) = false;
									__cols.at( entry.first ) = false;
									break;
								}
							}
						}
						
						std::vector< std::vector< GUM_SCALAR > > expandedRow = row._expandRow();

						std::cout << expandedRow << std::endl;
						matrix.insert( matrix.end(), expandedRow.begin(), expandedRow.end() );
					}
					
					// positivity - some may be redundant if lower <= state is NOT present in a row BUT state <= state' is; 0 <= state' is generated
					for ( const auto & col : __cols ) {
						if ( ! col.second )
							continue;
						
						std::vector< GUM_SCALAR > boundInf ( __cols.size() + 1, 0 );
						boundInf[ col.first.id() + 1 ] = 1;
						matrix.push_back( boundInf );
						std::cout << boundInf << std::endl;
					}
					
					// sum is one
					std::vector< GUM_SCALAR > sumInf ( __cols.size() + 1, 1 );
					sumInf[ 0 ] = -1;
					
					std::vector< GUM_SCALAR > sumSup ( __cols.size() + 1, -1 );
					sumSup[ 0 ] = 1;
					
					matrix.push_back( sumInf );
					matrix.push_back( sumSup );
					
					std::cout << sumInf << std::endl;
					std::cout << sumSup << std::endl;
					
					return matrix;
				};
				
			private:
				///std::vector< LpCol > __cols;
				/** Rows of the problem. */
				std::vector< LpRow< GUM_SCALAR > > __rows;
				
				/** Columns of the problem with boolean flag for positivity constraint ( true if must be generated, false otherwise ). */
				std::map< LpCol, bool > __cols; /// boolean flag for positivity constraint to be generated ( or not )
				
		}; // end of class LpInterface
		
		template< typename GUM_SCALAR >
		class LpExpr {
			public:
				LpExpr () {
					__lValue = 0.;
					__rValue = 0.;
					__mValue = 0.;
					__ileft = false;
					__iright = false;
				};
				
				LpExpr ( const std::map< LpCol, GUM_SCALAR > & coeffs ) {
					__lValue = 0.;
					__rValue = 0.;
					__mValue = 0.;
					__ileft = false;
					__iright = false;
					__mCoeffs = coeffs;
				};
				
				~LpExpr () {};
				
				LpExpr operator= ( const GUM_SCALAR & f ) {
					__mCoeffs.clear();
					__lValue = 0.;
					__rValue = 0.;
					__mValue = 0.;
					__mValue += f;
					return *this;
				};
				
				LpExpr operator= ( const LpCol & col ) {
					__mCoeffs.clear();
					__lValue = 0.;
					__rValue = 0.;
					__mValue = 0.;
					__mCoeffs[ col ] += 1.;
					return *this;
				};
				
				/*LpExpr operator<= ( const LpExpr & expr ) {
					
					return *this;
				};*/
				
				LpExpr operator+ ( const GUM_SCALAR & f ) {
					__mValue += f;
					return *this;
				};
				
				LpExpr operator+ ( const LpCol & col ) {
					__mCoeffs[ col ] += 1.;
					return *this;
				};
				
				LpExpr operator+ ( const LpExpr & expr ) {
					for ( const auto & entry : expr.__mCoeffs )
						__mCoeffs[ entry.first ] += entry.second;
					
					return *this;
				};
				
				LpExpr operator- ( const GUM_SCALAR & f ) {
					__mValue -= f;
					return *this;
				};
				
				LpExpr operator- ( const LpCol & col ) {
					__mCoeffs[ col ] += -1.;
					return *this;
				};
				
				LpExpr operator- ( const LpExpr & expr ) {
					for ( const auto & entry : expr.__mCoeffs )
						__mCoeffs[ entry.first ] -= entry.second;
					
					return *this;
				};
				
				template< typename G_S >
				friend LpExpr< G_S > operator<= ( const LpExpr< G_S > & expra, const LpExpr< G_S > & exprb );
				
				template< typename G_S >
				friend LpExpr< G_S > operator<= ( const LpCol & col, const LpExpr< G_S > & expr );
				
				template< typename G_S >
				friend LpExpr< G_S > operator<= ( const LpExpr< G_S > & expr, const LpCol & col );
				
				template< typename G_S >
				friend LpExpr< G_S > operator<= ( const G_S & f, const LpExpr< G_S > & expr );
				
				template< typename G_S >
				friend LpExpr< G_S > operator<= ( const LpExpr< G_S > & expr, const G_S & f );
				
				friend LpExpr< double > operator<= ( const LpCol & cola, const LpCol & colb );
				
				template< typename G_S >
				friend LpExpr< G_S > operator<= ( const G_S & f, const LpCol & col );
				
				template< typename G_S >
				friend LpExpr< G_S > operator<= ( const LpCol & col, const G_S & f );
				
				template< typename G_S >
				friend LpExpr< G_S > operator* ( const G_S & f, const LpCol & col );
				
				template< typename G_S >
				friend LpExpr< G_S > operator* ( const LpCol & col, const G_S & f );
				
				friend LpExpr< double > operator+ ( const LpCol & cola, const LpCol & colb );
				
				template< typename G_S >
				friend LpExpr< G_S > operator+ ( const G_S & f, const LpCol & col );
				
				template< typename G_S >
				friend LpExpr< G_S > operator+ ( const LpCol & col, const G_S & f );
				
				friend LpExpr< double > operator- ( const LpCol & cola, const LpCol & colb );
				
				template< typename G_S >
				friend LpExpr< G_S > operator- ( const G_S & f, const LpCol & col );
				
				template< typename G_S >
				friend LpExpr< G_S > operator- ( const LpCol & col, const G_S & f );
				
				void print () {
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
					
					std::cout << "mvalue : " << __mValue << std::endl;
					std::cout << "lvalue : " << __lValue << std::endl;
					std::cout << "rvalue : " << __rValue << std::endl;
					
					std::cout << std::endl;
				};
				
			private:
				bool __ileft, __iright;
				
				GUM_SCALAR __lValue;
				GUM_SCALAR __mValue;
				GUM_SCALAR __rValue;
				
				std::map< LpCol, GUM_SCALAR > __lCoeffs;
				std::map< LpCol, GUM_SCALAR > __mCoeffs;
				std::map< LpCol, GUM_SCALAR > __rCoeffs;
		};
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator<= ( const LpExpr< GUM_SCALAR > & expra, const LpExpr< GUM_SCALAR > & exprb ) {
			LpExpr< GUM_SCALAR > expr;
			/*
			bool al = expra.__ileft;
			bool ar = expra.__iright;
			bool bl = exprb.__ileft;
			bool br = exprb.__iright;
			
			if (  1  ) {
				for ( const auto & entry : expra.__mCoeffs )
					expr.__lCoeffs[ entry.first ] = entry.second;
				
				expr.__ileft = true;
				
				for ( const auto & entry : exprb.__mCoeffs )
					expr.__mCoeffs[ entry.first ] = entry.second;
			}
			else if ( ! expr.__iright ) {
				expr = expra;
				
				for ( const auto & entry : exprb.__mCoeffs )
					expr.__rCoeffs[ entry.first ] = entry.second;
			}
			else
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			*/
			return expr;
		}
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator<= ( const LpCol & col, const LpExpr< GUM_SCALAR > & expr ) {
			LpExpr< GUM_SCALAR > exprb = expr;
			
			exprb.__ileft = true;
			exprb.__lCoeffs[ col ] += 1;
			
			return exprb;
		}
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator<= ( const LpExpr< GUM_SCALAR > & expr, const LpCol & col ) {
			LpExpr< GUM_SCALAR > exprb;
			
			if ( ! expr.__ileft ) {
				exprb.__ileft = true;
				
				for ( const auto & entry : expr.__mCoeffs )
					exprb.__lCoeffs[ entry.first ] = entry.second;
				
				exprb.__lValue = expr.__mValue;
				
				exprb.__mCoeffs[ col ] += 1.;
			}
			else if ( ! expr.__iright ) {
				exprb = expr;
				exprb.__iright = true;
				exprb.__rCoeffs[ col ] += 1.;
			}
			else
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			return exprb;
		}
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator<= ( const GUM_SCALAR & f, const LpExpr< GUM_SCALAR > & expr ) {
			LpExpr< GUM_SCALAR > exprb = expr;
			
			if ( ! exprb.__ileft ) {
				exprb.__ileft = true;
				exprb.__lValue += f;
			}
			else if ( ! exprb.__iright ) {
				exprb.__iright = true;
				exprb.__mValue += f;
			}
			else
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			return exprb;
		}
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator<= ( const LpExpr< GUM_SCALAR > & expr, const GUM_SCALAR & f ) {
			LpExpr< GUM_SCALAR > exprb = expr;
			
			if ( ! exprb.__ileft ) {
				exprb.__ileft = true;
				exprb.__mValue += f;
			}
			else if ( ! exprb.__iright ) {
				exprb.__iright = true;
				exprb.__rValue += f;
			}
			else
				GUM_ERROR ( OperationNotAllowed, " too many symbols <= " );
			
			return exprb;
		}
		
		LpExpr< double > operator<= ( const LpCol & cola, const LpCol & colb ) {
			LpExpr< double > expr;
			expr.__lCoeffs[ cola ] += 1.;
			expr.__mCoeffs[ cola ] += 1.;
			expr.__ileft = true;
			return expr;
		}
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator<= ( const GUM_SCALAR & f, const LpCol & col ) {
			LpExpr< GUM_SCALAR > expr;
			expr.__mCoeffs[ col ] += 1.;
			expr.__ileft = true;
			expr.__lValue += f;
			return expr;
		}
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator<= ( const LpCol & col, const GUM_SCALAR & f ) {
			LpExpr< GUM_SCALAR > expr;
			expr.__lCoeffs[ col ] += 1.;
			expr.__ileft = true;
			expr.__mValue += f;
			return expr;
		}

		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator* ( const GUM_SCALAR & f, const LpCol & col ) {
			LpExpr< GUM_SCALAR > expr;
			expr.__mCoeffs[ col ] += f;
			return expr;
		}
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator* ( const LpCol & col, const GUM_SCALAR & f ) {
			return operator* ( f, col );
		}
		
		LpExpr< double > operator+ ( const LpCol & cola, const LpCol & colb ) {
			LpExpr< double > expr;
			expr.__mCoeffs[ cola ] += 1.;
			expr.__mCoeffs[ colb ] += 1.;
			return expr;
		}
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator+ ( const GUM_SCALAR & f, const LpCol & col ) {
			LpExpr< GUM_SCALAR > expr;
			expr.__mValue += f;
			expr.__mCoeffs[ col ] += 1.;
			return expr;
		}
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator+ ( const LpCol & col, const GUM_SCALAR & f ) {
			return operator+ ( f, col );
		}
		
		LpExpr< double > operator- ( const LpCol & cola, const LpCol & colb ) {
			LpExpr< double > expr;
			expr.__mCoeffs[ cola ] += 1.;
			expr.__mCoeffs[ colb ] += -1.;
			return expr;
		}
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator- ( const GUM_SCALAR & f, const LpCol & col ) {
			LpExpr< GUM_SCALAR > expr;
			expr.__mValue += f;
			expr.__mCoeffs[ col ] += -1.;
			return expr;
		}
		
		template< typename GUM_SCALAR >
		LpExpr< GUM_SCALAR > operator- ( const LpCol & col, const GUM_SCALAR & f ) {
			LpExpr< GUM_SCALAR > expr;
			expr.__mValue -= f;
			expr.__mCoeffs[ col ] += 1.;
			return expr;
		}
		
		
	}	// end of namespace cn
} // end of namespace gum

#endif