#include <agrum/CN/LrsWrapper.h>
#include <agrum/CN/lrslib/lrslib.h>

namespace gum {  
  namespace credal {

    template < typename GUM_SCALAR >
    LRS< GUM_SCALAR >::LRS () {
      __state = __states::none;
      __card = 0;
    }


    template < typename GUM_SCALAR >
    LRS< GUM_SCALAR >::~LRS () {
    }
    

    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::setUpH ( const int & card ) {
			if ( card < 2 )
        GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::setUpH : cardinality must be at least 2" );

      __input = std::vector< std::vector< GUM_SCALAR > > ( card * 2 + 2, std::vector< GUM_SCALAR > ( card + 1, 0 ) );

      __input[ card * 2 ] = std::vector< GUM_SCALAR > ( card + 1, -1 );
      __input[ card * 2 ][ 0 ] = 1;

      __input[ card * 2 + 1 ] = std::vector< GUM_SCALAR > ( card + 1, 1 );
      __input[ card * 2 + 1 ][ 0 ] = -1;

      __output = std::vector< std::vector< GUM_SCALAR > > ();

      __vertex = std::vector< GUM_SCALAR > ( card );

      __state = __states::Hup;
			
      __card = card;
    }


    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::tearDown () {
      __input.clear ();
      __output.clear ();
      __vertex.clear ();

      __state = __states::none;
      __card = 0;
    }


    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::fillH ( const GUM_SCALAR & min, const GUM_SCALAR & max, const int & modal ) {
      if ( ! __state == __states::Hup )
        GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::fillH : setUpH has not been called, current state is : " << __setUpStateNames[ __state ] );

      if ( modal >= __card )
        GUM_ERROR ( OutOfBounds, "LRS< GUM_SCALAR >::fillH : modality is greater or equal than cardinality : " << modal << " >= " << __card );

      __input[ modal * 2 ][ 0 ] = - min;
      __input[ modal * 2 ][ modal + 1 ] = 1;

      __input[ modal * 2 + 1 ][ 0 ] = max;
      __input[ modal * 2 + 1 ][ modal + 1 ] = -1;

      __vertex[ modal ] = max;
			
      if ( ( modal - __vertex.size () + 1 ) == 0 )
        __state = __states::H2Vready;
    }


    template < typename GUM_SCALAR >
    auto LRS< GUM_SCALAR >::H2V () -> matrix {
      if ( ! __state == __states::H2Vready )
        GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::H2V : fillH has not been called with all modalities, current state is still : " << __setUpStateNames[ __state ] );

      // check than we have a credal set and not a precise point probability, i.e. sum of vertex elements is close to one ( floating type precision )
      GUM_SCALAR sum = 0;
      for ( auto & elem : __vertex )
        sum += elem;

      if ( fabs ( sum - 1 ) < 1e-6 )
        return std::vector< std::vector< GUM_SCALAR > > ( 1, __vertex );

      // not precise point probability, initialize lrs

      lrs_dic *P;	/* structure for holding current dictionary and indices */
      lrs_dat *Q;	/* structure for holding static problem data */
      lrs_mp_vector output;	/* one line of output:ray,vertex,facet,linearity */
      lrs_mp_matrix Lin; /* holds input linearities if any are found */

      long i;
      long col; /* output column index for dictionary */

      if ( ! lrs_init ("\n*LrsWrapper:") )
        GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::H2V : failed lrs_init" );

      /* allocate and init structure for static problem data */
      Q = lrs_alloc_dat ("LRS globals");

      if ( Q == nullptr )
        GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::H2V : failed lrs_alloc_dat" );

      /* now flags in lrs_dat can be set */
      Q->n = __card + 1; /* number of input columns (dimension + 1 ) */
      Q->m = 2 * __card + 2; /* number of input rows = number of inequalities */ 

      output = lrs_alloc_mp_vector_wrapper ( Q->n );

      P = lrs_alloc_dic ( Q ); /* allocate and initialize lrs_dic */

      if ( P == nullptr )
        GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::H2V : failed lrs_alloc_dic" );

      /* Build polyhedron : constraints and objective */
      /* no objective here */
      __fill( P, Q );

      /* code from here is borrowed from lrs_main */

      /* Pivot to a starting dictionary */
      if ( ! lrs_getfirstbasis ( &P, Q, &Lin, FALSE ) )
        GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::H2V : failed lrs_getfirstbasis" );

      /* There may have been column redundancy */
      /* If so the linearity space is obtained and redundant */
      /* columns are removed. User can access linearity space */
      /* from lrs_mp_matrix Lin dimensions nredundcol x d+1  */

      if ( Q->nredundcol > 0 ) {
        for ( col = 0L; col < Q->nredundcol; col++ ) /* print linearity space */
          lrs_printoutput ( Q, Lin[col] ); /* Array Lin[][] holds the coeffs. */

        GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::H2V : redundant columns !" );
      }

      /* We initiate reverse search from this dictionary       */
      /* getting new dictionaries until the search is complete */
      /* User can access each output line from output which is */
      /* a vertex/ray/facet from the lrs_mp_vector output      */

			std::stringstream istring;
			
			

      do
      {
        for ( col = 0; col <= P->d; col++ )
          if ( lrs_getsolution ( P, Q, output, col ) ) {
            istring. << lrs_printoutput ( Q, output );
						//std::cout << istring << std::endl;
					}
      }
      while ( lrs_getnextbasis ( &P, Q, FALSE ) );
			
      lrs_printtotals (P, Q); /* print final totals */


      /* free space : do not change order of next 3 lines! */

      lrs_clear_mp_vector_wrapper ( output, Q->n );
      lrs_free_dic ( P, Q ); /* deallocate lrs_dic */
      lrs_free_dat ( Q ); /* deallocate lrs_dat */

      lrs_close ("LrsWrapper:");

      return __output;
    }


    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::__fill ( lrs_dic * P, lrs_dat * Q ) const {
      long num[ __card + 1 ];
      long den[ __card + 1 ];

      int rows = __card * 2 + 2;

      int64_t numerator, denominator;

      for ( int row = 0; row < rows; row++ ) {
        for ( decltype ( __card ) col = 0; col < __card + 1; col++ ) {
          gum::Rational< GUM_SCALAR >::farey ( numerator, denominator, __input [ row ][ col ] );
          num [ col ] = numerator;
          den [ col ] = denominator;
        }
        /* GE is inequality, EQ is equation */
        lrs_set_row ( P, Q, row, num, den, GE );
      }
    }
    

    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::__coutOff() const {
      fflush ( stdout );
      __oldCout = dup ( 1 );

      int new_cout = open ( "/dev/null", O_WRONLY );
      dup2 ( new_cout, 1 );
      close ( new_cout );
    }


    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::__coutOn() const {
      fflush ( stdout );
      dup2 ( __oldCout, 1 );
      close ( __oldCout );
    }

  } // end of credal namespace
} // end of gum namespace
