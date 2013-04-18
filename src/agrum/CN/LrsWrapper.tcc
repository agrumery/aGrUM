#include <agrum/CN/LrsWrapper.h>
#include <agrum/CN/lrslib/lrslib.h>

namespace gum {  
  namespace credal {

    template < typename GUM_SCALAR >
    LRS< GUM_SCALAR >::LRS () {			
      __state = __states::none;
			
			__vertices = 0;
      __card = 0;
			
			__volume = 0;
    }


    template < typename GUM_SCALAR >
    LRS< GUM_SCALAR >::~LRS () {
    }

		
		template < typename GUM_SCALAR >
		const GUM_SCALAR & LRS< GUM_SCALAR >::getVolume () const {
			if ( __volume != 0 )
				return __volume;
			else
				GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::getVolume () : volume computation was not asked for this credal set, call computeVolume() from a V-representation." );
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
    void LRS< GUM_SCALAR >::setUpV ( const int & card, const int & vertices ) {
			if ( card < 2 )
				GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::setUpV : cardinality must be at least 2" );
			
			if ( vertices < 2 )
				GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::setUpV : vertices must be at least 2 to build a polytope" );
			
			__input = std::vector< std::vector< GUM_SCALAR > > ( vertices, std::vector< GUM_SCALAR > ( card + 1, 1 ) );
			
			__output = std::vector< std::vector< GUM_SCALAR > > ();
			
			__state = __states::Vup;
			
			__card = card;
			__vertices = vertices;
		}


    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::tearDown () {
      __input.clear ();
      __output.clear ();
      __vertex.clear ();
			__insertedModals.clear();
			
			__insertedVertices.clear();
			__vertices = 0;
			
			__volume = 0;

      __state = __states::none;
      __card = 0;
    }
    
    
    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::nextInput () {
			__insertedModals.clear();
			__insertedVertices.clear();
			__output.clear();
			__vertex.clear();
			__volume = 0;
			
			if ( __state == __states::H2Vready )
				__state = __states::Hup;
			else if ( __state == __states::V2Hready )
				__state = __states::Vup;
			else
			{
				__input.clear ();
				__state = __states::none;
				__card = 0;
				__vertices = 0;
			}
		}


    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::fillH ( const GUM_SCALAR & min, const GUM_SCALAR & max, const int & modal ) {
      if ( ! __state == __states::Hup )
        GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::fillH : setUpH or nextInput has not been called or H-representation is complete, current state is : " << __setUpStateNames[ __state ] );

      if ( modal >= __card )
        GUM_ERROR ( OutOfBounds, "LRS< GUM_SCALAR >::fillH : modality is greater or equal than cardinality : " << modal << " >= " << __card );

      __input[ modal * 2 ][ 0 ] = - min;
      __input[ modal * 2 ][ modal + 1 ] = 1;

      __input[ modal * 2 + 1 ][ 0 ] = max;
      __input[ modal * 2 + 1 ][ modal + 1 ] = -1;
			
      __vertex[ modal ] = max;
			
			__insertedModals.insert( modal );
			
      if ( __insertedModals.size() == __card )
        __state = __states::H2Vready;
    }
    
    
    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::fillV ( const std::vector< GUM_SCALAR > & vertex ) {
			if ( ! __state == __states::Vup )
				GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::fillV : setUpV or nextInput has not been called or V-representation is complete, current state is : " << __setUpStateNames[ __state ] );
			
			if ( __insertedVertices.size() == __vertices )
				GUM_ERROR ( OutOfBounds, "LRS< GUM_SCALAR >::fillV : input if already full with " << __vertices << " vertices." );
			
			GUM_SCALAR cptSum = 0;
			for ( auto & p : vertex )
				cptSum += p;
			
			if ( fabs ( 1 - cptSum ) > 1e-6 )
				GUM_ERROR ( CPTNoSumTo1, "LRS< GUM_SCALAR >::fillV : CPT sum is not one : " << cptSum << " from vertex : " << vertex );
			
			bool eq = true;
			for ( auto & v : __insertedVertices ) {
				eq = true;
				for ( int mod = 0; mod < __card; mod++ )
					if ( fabs ( v[ mod ] - vertex [ mod ] ) > 1e-6 ) {
						eq = false;
						break;
					}
				
				if ( eq )
					GUM_ERROR ( gum::DuplicateElement, "LRS< GUM_SCALAR >::fillV : vertex already present : " << vertex );
			}
			
			int row = __insertedVertices.size();
			for ( int mod = 0; mod < __card; mod++ )
				__input[ row ][ mod + 1 ] = vertex[ mod ];
			
			__insertedVertices.push_back( vertex );
			
			if ( __insertedVertices.size() == __vertices )
				__state = __states::V2Hready;
		}
    
    
    template < typename GUM_SCALAR >
    auto LRS< GUM_SCALAR >::getInput () const -> const matrix & {
			return __input;
		}
		
		
		template < typename GUM_SCALAR >
		auto LRS< GUM_SCALAR >::getOutput () const -> const matrix & {
			return __output;
		}


    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::H2V () {
			if ( ! __state == __states::H2Vready )
        GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::H2V : fillH has not been called with all modalities, current state is still : " << __setUpStateNames[ __state ] );

      // check that we have a credal set and not a precise point probability, i.e. sum of vertex elements is close to one ( floating type precision )
      GUM_SCALAR sum = 0;
      for ( auto & elem : __vertex )
        sum += elem;

      if ( fabs ( sum - 1 ) < 1e-6 ) {
				__output = std::vector< std::vector< GUM_SCALAR > > ( 1, __vertex );
				return;
        //return std::vector< std::vector< GUM_SCALAR > > ( 1, __vertex );
			}

      // not precise point probability, initialize lrs
			
			// redirect cout
			__coutOff();

      lrs_dic *P;	/* structure for holding current dictionary and indices */
      lrs_dat *Q;	/* structure for holding static problem data */
			lrs_mp_vector output;	/* one line of output:ray,vertex,facet,linearity */
      lrs_mp_matrix Lin; /* holds input linearities if any are found */

      long i;
      long col; /* output column index for dictionary */

      if ( ! lrs_init ("\n*LrsWrapper:") ) {
				__coutOn();
        GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::H2V : failed lrs_init" );
			}

      /* allocate and init structure for static problem data */
      Q = lrs_alloc_dat ("LRS globals");

      if ( Q == nullptr ) {
				__coutOn();
        GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::H2V : failed lrs_alloc_dat" );
			}

      /* now flags in lrs_dat can be set */
      Q->n = __card + 1; /* number of input columns (dimension + 1 ) */
      Q->m = 2 * __card + 2; /* number of input rows = number of inequalities */
      
      output = lrs_alloc_mp_vector_wrapper ( Q->n );

      P = lrs_alloc_dic ( Q ); /* allocate and initialize lrs_dic */

      if ( P == nullptr ) {
				__coutOn();
        GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::H2V : failed lrs_alloc_dic" );
			}

      /* Build polyhedron : constraints and objective */
      /* no objective here */
      __fill( P, Q );

      /* code from here is borrowed from lrs_main */

      /* Pivot to a starting dictionary */
      if ( ! lrs_getfirstbasis ( &P, Q, &Lin, FALSE ) ) {
				__coutOn();
        GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::H2V : failed lrs_getfirstbasis" );
			}

      /* There may have been column redundancy */
      /* If so the linearity space is obtained and redundant */
      /* columns are removed. User can access linearity space */
      /* from lrs_mp_matrix Lin dimensions nredundcol x d+1  */

      if ( Q->nredundcol > 0 ) {
				__coutOn();
				
        for ( col = 0L; col < Q->nredundcol; col++ ) /* print linearity space */
          lrs_printoutput ( Q, Lin[col] ); /* Array Lin[][] holds the coeffs. */

        GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::H2V : redundant columns !" );
      }

      /* We initiate reverse search from this dictionary       */
      /* getting new dictionaries until the search is complete */
      /* User can access each output line from output which is */
      /* a vertex/ray/facet from the lrs_mp_vector output      */
			
			std::vector< long int > Num; /* numerators of all vertices */
			std::vector< long int > Den; /* denominators of all vertices */
			
      do
      {
        for ( col = 0; col <= P->d; col++ )
					if ( lrs_getsolution ( P, Q, output, col ) ) {
						
						if ( Q->hull || ((((output[0])[0]==2 || (output[0])[0]==-2) && (output[0])[1]==0)?TRUE:FALSE) ) {
							__coutOn();
							GUM_ERROR( gum::FatalError, "LRS< GUM_SCALAR >::H2V : asked for Q-hull computation or not reading a vertex !");
							/*for ( decltype(Q->n) i = 0; i < Q->n; i++ )
								pmp ("", output[i]);*/
						}
						else
							for ( decltype(Q->n) i = 1; i < Q->n; i++ )
								__getLRSOutput( output[i], output[0], Num, Den );
						
						//__coutOn();
						//lrs_printoutput( Q, output );
						//__coutOff();
					}
      }
      while ( lrs_getnextbasis ( &P, Q, FALSE ) );
			
			auto vtx = Num.size();
			std::vector< GUM_SCALAR > vertex;
			
			//__output.clear(); /* to be sure */
			for ( decltype(vtx) i = 1; i <= vtx; i++ ) {
				vertex.push_back( Num[ i-1 ]*1.0 / Den[ i-1 ] );
				if( i % __card == 0 ) {
					__output.push_back( vertex );
					__vertices++;
					vertex.clear();
				}
			}
			
			//__coutOn();
      //lrs_printtotals (P, Q); /* print final totals */
			//__coutOff();

      /* free space : do not change order of next 3 lines! */

      lrs_clear_mp_vector_wrapper ( output, Q->n );
      lrs_free_dic ( P, Q ); /* deallocate lrs_dic */
      lrs_free_dat ( Q ); /* deallocate lrs_dat */

      lrs_close ("LrsWrapper:");
			
			__coutOn();
			
      //return __output;
    }
    
    
    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::computeVolume () {
			if ( ! __state == __states::V2Hready )
				GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::computeVolume : volume is only for V-representation or fillV has not been called with all vertices, current state is still : " << __setUpStateNames[ __state ] );
			
			__coutOff();
			
			lrs_dic *P;
			lrs_dat *Q;
			lrs_mp_vector output;
			lrs_mp_matrix Lin;
			
			long i;
			long col;
			
			if ( ! lrs_init ("\n*LrsWrapper:") ) {
				__coutOn();
				GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::computeVolume : failed lrs_init" );
			}
			
			Q = lrs_alloc_dat ("LRS globals");
			
			if ( Q == nullptr ) {
				__coutOn();
				GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::computeVolume : failed lrs_alloc_dat" );
			}
			
			Q->n = __input[ 0 ].size();
			Q->m = __input.size();
			
			Q->getvolume = true;
			
			output = lrs_alloc_mp_vector_wrapper ( Q->n );
			
			P = lrs_alloc_dic ( Q );
			
			if ( P == nullptr ) {
				__coutOn();
				GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::computeVolume : failed lrs_alloc_dic" );
			}
			
			__fill( P, Q );
			
			if ( ! lrs_getfirstbasis ( &P, Q, &Lin, FALSE ) ) {
				__coutOn();
				GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::computeVolume : failed lrs_getfirstbasis" );
			}
			
			if ( Q->nredundcol > 0 ) {
				__coutOn();
				
				for ( col = 0L; col < Q->nredundcol; col++ )
					lrs_printoutput ( Q, Lin[col] );
					
				GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::computeVolume : redundant columns !" );
			}
			
			do
			{
				for ( col = 0; col <= P->d; col++ )
					lrs_getsolution ( P, Q, output, col );
					/*if ( lrs_getsolution ( P, Q, output, col ) ) {
						__coutOn();
						lrs_printoutput( Q, output );
						__coutOff();
					}*/
			}
			while ( lrs_getnextbasis ( &P, Q, FALSE ) );
			
			//__coutOn();
			//lrs_printtotals (P, Q);
			//__coutOff();
				
			long Nsize = ( Q->Nvolume[0] > 0 ) ? Q->Nvolume[0] : -Q->Nvolume[0];
			long Dsize = ( Q->Dvolume[0] > 0 ) ? Q->Dvolume[0] : -Q->Dvolume[0];
				
			__volume = Q->Nvolume[ Nsize - 1 ] * 1.0 / Q->Dvolume[ Dsize - 1 ];
			
			/* free space : do not change order of next 3 lines! */
			
			lrs_clear_mp_vector_wrapper ( output, Q->n );
			lrs_free_dic ( P, Q );
			lrs_free_dat ( Q );
			
			lrs_close ("LrsWrapper:");
			
			__coutOn();
		}
    
    
    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::V2H () {
			if ( ! __state == __states::V2Hready )
				GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::V2H : fillV has not been called with all vertices, current state is still : " << __setUpStateNames[ __state ] );
			
			
		}
    
    
    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::__getLRSOutput( lrs_mp Nin, lrs_mp Din, std::vector< long int > & Num, std::vector< long int > & Den ) const {
			lrs_mp Nt, Dt;
			long i;

			long Nsize = ( Nin[0] > 0 ) ? Nin[0] : -Nin[0];
			long Dsize = ( Din[0] > 0 ) ? Din[0] : -Din[0];
			
			/*
			for ( i = 0; i <= Nsize; i++ )
				Nt[i] = Nin[i];
			for ( i = 0; i <= Dsize; i++ )
				Dt[i] = Din[i];
			*/
			
			//reduce (Nt, Dt);
				
			long int num = 1;
			long int den = 1;
			
			/* print out       */
			if (sign (Nin) * sign (Din) == NEG)
				//fprintf (lrs_ofp, "-");
				num = -num;
			/*else
				fprintf (lrs_ofp, " ");*/
			//fprintf (lrs_ofp, "%lu", Nt[length (Nt) - 1]);
			
			num *= Nin[ Nsize - 1 ];
			
			//std::cout << "first print : " << Nt[ Nsize - 1 ];
			//std::cout << "\n size : " << Nsize << "\n";
			
			//for (i = Nsize - 2; i >= 1; i--)
				//std::cout << Nt[ i ];
				//fprintf (lrs_ofp, FORMAT, Nt[i]);
			
			
			if (!(Din[0] == 2 && Din[1] == 1))	/* rational */
			{
				//fprintf (lrs_ofp, "/");
				//fprintf (lrs_ofp, "%lu", Dt[length (Dt) - 1]);
				//std::cout << "/" << Dt[ Dsize - 1 ];
				den = Din[ Dsize - 1 ];
				
				//for (i = Dsize - 2; i >= 1; i--)
					//std::cout << Dt[ i ];
					//fprintf (lrs_ofp, FORMAT, Dt[i]);
			}
			//fprintf (lrs_ofp, " ");
			//std::cout << " ";
			Num.push_back(num);
			Den.push_back(den);
		}
		
		
		/*
		void pmp (char name[], lrs_mp a) {
				 long i;
				 fprintf (lrs_ofp, "%s", name);
				 if (sign (a) == NEG)
					 fprintf (lrs_ofp, "-");
				 else
					 fprintf (lrs_ofp, " ");
				 fprintf (lrs_ofp, "%lu", a[length (a) - 1]);
				 for (i = length (a) - 2; i >= 1; i--)
					 fprintf (lrs_ofp, FORMAT, a[i]);
				 fprintf (lrs_ofp, " ");
		}*/


    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::__fill ( lrs_dic * P, lrs_dat * Q ) const {
			int cols = __input[ 0 ].size(); //__card + 1;
			
      long num[ cols /*__card + 1*/ ];
      long den[ cols /*__card + 1*/ ];

      int rows = __input.size();//__card * 2 + 2;

      int64_t numerator, denominator;
			//__coutOn();
			//std::cout << std::endl;
      for ( int row = 0; row < rows; row++ ) {
        for ( int col = 0; col < cols; col++ ) {
          gum::Rational< GUM_SCALAR >::farey ( numerator, denominator, __input [ row ][ col ] );
          num [ col ] = numerator;
          den [ col ] = denominator;
					//std::cout << numerator << "/" << denominator << " ";
        }
        //std::cout << std::endl;
        /* GE is inequality, EQ is equation */
				//if ( __state == __states::H2Vready )
					lrs_set_row ( P, Q, row+1, num, den, GE ); // do NOT forget this + 1 on row
				/*else if ( __state == __states::V2Hready )
					lrs_set_row ( P, Q, row+1, num, den, GE );*/ // do NOT forget this + 1 on row
      }
      //__coutOff();
    }
    
    
    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::__initLrs ( const bool & volume ) {
			if ( ! __state == __states::H2Vready && ! __state == __states::V2Hready )
				GUM_ERROR ( OperationNotAllowed, "LRS< GUM_SCALAR >::__initLrs : not ready, current state is still : " << __setUpStateNames[ __state ] );
			
			__coutOff();
			
			if ( ! lrs_init ("\n*LrsWrapper:") ) {
				__coutOn();
				GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::__initLrs : failed lrs_init" );
			}
			
			__Q = lrs_alloc_dat ("LRS globals");
			
			if ( __Q == nullptr ) {
				__coutOn();
				GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::__initLrs : failed lrs_alloc_dat" );
			}
			
			__Q->n = __input[ 0 ].size();
			__Q->m = __input.size();
			
			__Q->getvolume = volume;
			
			__lrsOutput = lrs_alloc_mp_vector_wrapper ( __Q->n );
			
			__P = lrs_alloc_dic ( __Q );
			
			if ( __P == nullptr ) {
				__coutOn();
				GUM_ERROR ( FatalError, "LRS< GUM_SCALAR >::__initLrs : failed lrs_alloc_dic" );
			}
		}
		
		template < typename GUM_SCALAR >
		void LRS< GUM_SCALAR >::__freeLrs () {
			/* free space : do not change order of next 3 lines! */
			
			lrs_clear_mp_vector_wrapper ( __lrsOutput, __Q->n );
			lrs_free_dic ( __P, __Q );
			lrs_free_dat ( __Q );
			
			lrs_close ("LrsWrapper:");
			
			__coutOn();
		}
    

    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::__coutOff () const {
      fflush ( stdout );
      __oldCout = dup ( 1 );

      int new_cout = open ( "/dev/null", O_WRONLY );
      dup2 ( new_cout, 1 );
      close ( new_cout );
    }


    template < typename GUM_SCALAR >
    void LRS< GUM_SCALAR >::__coutOn () const {
      fflush ( stdout );
      dup2 ( __oldCout, 1 );
      close ( __oldCout );
    }

  } // end of credal namespace
} // end of gum namespace
