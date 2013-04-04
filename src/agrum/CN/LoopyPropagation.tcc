namespace gum {

  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::saveInference ( const std::string &path )
  {
    std::string path_name = path.substr ( 0, path.size() - 4 );
    path_name = path_name + ".res";

    std::ofstream res( path_name.c_str(), std::ios::out | std::ios::trunc );

    if ( ! res.good() )
      GUM_ERROR ( NotFound, "LoopyPropagation<GUM_SCALAR>::saveInference(std::string & path) : could not open file : " + path_name )

    std::string ext = path.substr( path.size() - 3, path.size() );

    if ( std::strcmp( ext.c_str(), "evi" ) == 0 )
    {
      std::ifstream evi( path.c_str(), std::ios::in );
      std::string ligne;
      if ( ! evi.good() )
        GUM_ERROR ( NotFound, "LoopyPropagation<GUM_SCALAR>::saveInference(std::string & path) : could not open file : " + ext )

      while ( evi.good() )
      {
        getline( evi, ligne );
        res << ligne << "\n";
      }
      evi.close();
    }

    res << "[RESULTATS]" << "\n";

    for ( auto it = bnet->beginNodes(), theEnd = bnet->endNodes(); it != theEnd; ++it )
    {	
      // calcul distri posteriori
      GUM_SCALAR msg_p_min = 1.0;
      GUM_SCALAR msg_p_max = 0.0;

      // cas evidence, calcul immediat
      if ( infE::_evidence.exists( *it ) )
      {
        if ( infE::_evidence[ *it ][1] == 0. )
          msg_p_min = 0.;
        else if ( infE::_evidence[ *it ][1] == 1. )
          msg_p_min = 1.;

        msg_p_max = msg_p_min;
      }
      // sinon depuis node P et node L
      else
      {
        GUM_SCALAR min = _NodesP_min[ *it ];
        GUM_SCALAR max;

        if ( _NodesP_max.exists(*it) )
          max = _NodesP_max[ *it ];
        else
          max = min;

        GUM_SCALAR lmin = _NodesL_min[ *it ];
        GUM_SCALAR lmax;

        if ( _NodesL_max.exists( *it ) )
          lmax = _NodesL_max[ *it ];
        else
          lmax = lmin;

        // cas limites sur min
        if ( min == _INF && lmin == 0. )
          std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;

        if ( lmin == _INF  ) // cas infini
          msg_p_min = 1.;
        else if ( min == 0. || lmin == 0.)
          msg_p_min = 0;
        else
          msg_p_min = 1. / ( 1. + ( (1. / min - 1.) * 1. / lmin ));

        // cas limites sur max	
        if ( max == _INF && lmax == 0. )
          std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;

        if ( lmax == _INF  ) // cas infini
          msg_p_max = 1.;
        else if ( max == 0. || lmax == 0.)
          msg_p_max = 0;
        else
          msg_p_max = 1. / ( 1. + ( (1. / max - 1.) * 1. / lmax ));
      }

      if ( msg_p_min != msg_p_min && msg_p_max == msg_p_max )
        msg_p_min = msg_p_max;
      if ( msg_p_max != msg_p_max && msg_p_min == msg_p_min )
        msg_p_max = msg_p_min;
      if ( msg_p_max != msg_p_max && msg_p_min != msg_p_min )
      {
        std::cout << std::endl;
        std::cout << "pas de proba calculable (verifier observations)" << std::endl;
      }

      res << "P(" << bnet->variable( *it ).name() << " | e) = ";

      if ( infE::_evidence.exists( *it ) )
        res << "(observe)" << std::endl;
      else
        res << std::endl;

      res << "\t\t" << bnet->variable( *it ).label( 0 ) << "  [ " << (GUM_SCALAR) 1. - msg_p_max;

      if ( msg_p_min != msg_p_max )
        res << ", " << (GUM_SCALAR) 1. - msg_p_min <<" ] | ";
      else
        res<<" ] | ";    

      res << bnet->variable( *it ).label( 1 ) << "  [ "<< msg_p_min;
      if ( msg_p_min != msg_p_max )
        res << ", " << msg_p_max << " ]" << std::endl;
      else
        res << " ]" << std::endl;
    } // end of : for each node

    res.close();
  }


  /**
   * pour les fonctions suivantes, les GUM_SCALAR min/max doivent etre initialises
   * (min a 1 et max a 0) pour comparer avec les resultats intermediaires
   */

  /**
   * une fois les cpts marginalises sur X et Ui, on calcul le min/max,
   */
  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_compute_ext (
    GUM_SCALAR & msg_l_min,
    GUM_SCALAR & msg_l_max,
    std::vector<GUM_SCALAR> & lx,
    GUM_SCALAR & num_min,
    GUM_SCALAR & num_max,
    GUM_SCALAR & den_min,
    GUM_SCALAR & den_max 
  )
  {
    GUM_SCALAR old_msg_min = msg_l_min;
    GUM_SCALAR old_msg_max = msg_l_max;

    GUM_SCALAR num_min_tmp = 1.;
    GUM_SCALAR den_min_tmp = 1.;
    GUM_SCALAR num_max_tmp = 1.;
    GUM_SCALAR den_max_tmp = 1.;

    GUM_SCALAR res_min = 1.0, res_max = 0.0;

    auto lsize = lx.size();
    for ( decltype( lsize ) i = 0; i < lsize; i++ )
    {
      bool non_defini_min = false;
      bool non_defini_max = false;

      if ( lx[ i ] == _INF )
      {
        num_min_tmp = num_min;
        den_min_tmp = den_max;
        num_max_tmp = num_max;
        den_max_tmp = den_min;
      }
      else if ( lx[ i ] == (GUM_SCALAR) 1. )
      {
        num_min_tmp = 1.;
        den_min_tmp = 1.;
        num_max_tmp = 1.;	
        den_max_tmp = 1.;
      }
      else if ( lx[ i ] > (GUM_SCALAR) 1. )
      {	
        GUM_SCALAR li = 1. / (lx[ i ] - 1.);
        num_min_tmp = num_min + li;
        den_min_tmp = den_max + li;
        num_max_tmp = num_max + li;
        den_max_tmp = den_min + li;
      }    
      else if ( lx[ i ] < (GUM_SCALAR) 1. )
      {
        GUM_SCALAR li = 1. / (lx[ i ] - 1.);
        num_min_tmp = num_max + li;
        den_min_tmp = den_min + li;
        num_max_tmp = num_min + li;
        den_max_tmp = den_max + li;
      }

      if ( den_min_tmp == 0. && num_min_tmp == 0. )
        non_defini_min = true;
      else if ( den_min_tmp == 0. && num_min_tmp != 0.)
        res_min = _INF;
      else if ( den_min_tmp != _INF || num_min_tmp != _INF )
        res_min = num_min_tmp / den_min_tmp;

      if ( den_max_tmp == 0. && num_max_tmp == 0.)
        non_defini_max = true;
      else if ( den_max_tmp == 0. && num_max_tmp != 0. )
        res_max = _INF;
      else if ( den_max_tmp != _INF || num_max_tmp != _INF )
        res_max = num_max_tmp / den_max_tmp;

      if ( non_defini_max && non_defini_min ) {
        std::cout << "undefined msg" << std::endl;
        continue;
      }
      else if ( non_defini_min && ! non_defini_max )
        res_min = res_max;
      else if ( non_defini_max && ! non_defini_min )
        res_max = res_min;

      if ( res_min < 0. )
        res_min = 0.;
      if ( res_max < 0. )
        res_max = 0.;

      if ( msg_l_min == msg_l_max && msg_l_min == -2. )
      {
        msg_l_min = res_min;
        msg_l_max = res_max;
      }

      if ( res_max > msg_l_max )
        msg_l_max = res_max;
      if ( res_min < msg_l_min )
        msg_l_min = res_min;

    } // end of : for each lx

  }

  /**
   * extremes pour une combinaison des parents, message vers parent
   */
  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_compute_ext ( 
    std::vector< std::vector<GUM_SCALAR> > & combi_msg_p,
    const gum::NodeId & id,
    GUM_SCALAR & msg_l_min,
    GUM_SCALAR & msg_l_max,
    std::vector<GUM_SCALAR> & lx,
    const gum::Idx & pos
  )
  {  
    GUM_SCALAR num_min = 0.;
    GUM_SCALAR num_max = 0.;
    GUM_SCALAR den_min = 0.;
    GUM_SCALAR den_max = 0.;

    auto taille = combi_msg_p.size();

    std::vector< typename std::vector<GUM_SCALAR>::iterator > it( taille );
    
    for ( decltype( taille ) i = 0; i < taille; i++ )
      it[ i ] = combi_msg_p[ i ].begin();

    int pas = 2;
    int pp = pos;
    cn->intPow( pas, pp );

    int combi_den = 0;
    int combi_num = pp;

    // marginalisation
    while ( it[ taille - 1 ] != combi_msg_p[ taille - 1 ].end() ) {
      GUM_SCALAR prod = 1.;

      for ( decltype( taille ) k = 0; k < taille; k++ )
        prod *= *it[ k ];

      den_min += ( cn->get_CPT_min()[ id ][ combi_den ] * prod );
      den_max += ( cn->get_CPT_max()[ id ][ combi_den ] * prod );

      num_min += ( cn->get_CPT_min()[ id ][ combi_num ] * prod );
      num_max += ( cn->get_CPT_max()[ id ][ combi_num ] * prod );

      combi_den++;
      combi_num++;

      if ( combi_den % pp == 0 )
      {
        combi_den += pp;
        combi_num += pp;
      }

      // incrementation 
      ++it[ 0 ];
      for ( decltype( taille ) i = 0; ( i < taille - 1 ) && ( it[ i ] == combi_msg_p[ i ].end() ); ++i )
      {
        it[ i ] = combi_msg_p[ i ].begin();
        ++it[ i + 1 ];
      }
    } // end of : marginalisation

    _compute_ext( msg_l_min, msg_l_max, lx, num_min, num_max, den_min, den_max );

  }

  /**
   * extremes pour une combinaison des parents, message vers enfant
   * marginalisation cpts
   */
  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_compute_ext (
    std::vector< std::vector<GUM_SCALAR> > & combi_msg_p,
    const gum::NodeId & id,
    GUM_SCALAR & msg_p_min,
    GUM_SCALAR & msg_p_max
  )
  {  
    GUM_SCALAR min = 0.;
    GUM_SCALAR max = 0.;

    auto taille = combi_msg_p.size();

    std::vector< typename std::vector<GUM_SCALAR>::iterator > it( taille );

    for ( decltype( taille ) i = 0; i < taille; i++)
      it[ i ] = combi_msg_p[ i ].begin();

    int combi = 0;
    auto theEnd = combi_msg_p[ taille - 1 ].end();

    while( it[ taille - 1 ] != theEnd )
    {
      GUM_SCALAR prod = 1.;

      for ( decltype( taille ) k = 0; k < taille; k++ )
        prod *= *it[ k ];

      min += ( cn->get_CPT_min()[ id ][ combi ] * prod );
      max += ( cn->get_CPT_max()[ id ][ combi ] * prod );

      combi++;

      // incrementation
      ++it[ 0 ];
      for ( decltype( taille ) i = 0; ( i < taille - 1 ) && ( it[ i ] == combi_msg_p[ i ].end() ); ++i )
      {
        it[ i ] = combi_msg_p[ i ].begin();
        ++it[ i + 1 ];
      }
    }

    if ( min < msg_p_min )
      msg_p_min = min;
    if ( max > msg_p_max )
      msg_p_max = max;
  }

  /**
   * enumere combinaisons messages parents, pour message vers enfant
   */
  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_enum_combi (
    std::vector< std::vector< std::vector<GUM_SCALAR> > > & msgs_p,
    const gum::NodeId & id,
    GUM_SCALAR & msg_p_min,
    GUM_SCALAR & msg_p_max
  )
  {
    auto taille = msgs_p.size();

    // source node
    if ( taille == 0 )
    {
      msg_p_min = cn->get_CPT_min()[ id ][ 0 ];
      msg_p_max = cn->get_CPT_max()[ id ][ 0 ];

      return;
    }

    decltype( taille ) msgPerm = 1;
    #pragma omp parallel
    {
      GUM_SCALAR msg_pmin = msg_p_min;
      GUM_SCALAR msg_pmax = msg_p_max;

      std::vector< std::vector<GUM_SCALAR> > combi_msg_p( taille );

      decltype( taille ) confs = 1;
      #pragma omp for
      for ( decltype( taille ) i = 0; i < taille; i++ )
        confs *= msgs_p[ i ].size();

      #pragma omp atomic
      msgPerm *= confs;
      #pragma omp barrier
      #pragma omp flush(msgPerm)

      #pragma omp for
      for ( decltype( msgPerm ) j = 0; j < msgPerm; j++ ) {
        // get jth msg :
        auto jvalue = j;

        for ( decltype( taille ) i = 0; i < taille; i++ ) {
          if ( msgs_p[ i ].size() == 2 ) {
            combi_msg_p[ i ] = ( jvalue & 1 ) ? msgs_p[ i ][ 1 ] : msgs_p[ i ][ 0 ];
            jvalue /= 2;
          }
          else
            combi_msg_p[ i ] = msgs_p[ i ][ 0 ];
        }

        _compute_ext( combi_msg_p, id, msg_pmin, msg_pmax );
      }

      // since min is _INF and max is 0 at init, there is no issue having more threads here than during for loop
      #pragma omp critical(msgpminmax)
      {
        #pragma omp flush(msg_p_min)
        #pragma omp flush(msg_p_max)
        if ( msg_p_min > msg_pmin )
          msg_p_min = msg_pmin;
        if ( msg_p_max < msg_pmax )
          msg_p_max = msg_pmax;
      }
    }
    return;

    // old mono threaded version with iterators //

    std::vector< typename std::vector< std::vector<GUM_SCALAR> >::iterator > it( taille );

    for ( decltype( taille ) i = 0; i < taille; i++ )
      it[ i ] = msgs_p[ i ].begin();

    auto theEnd = msgs_p[ 0 ].end();
    while( it[ 0 ] != theEnd )
    {
      std::vector< std::vector<GUM_SCALAR> > combi_msg_p( taille );

      for ( decltype( taille ) i = 0; i < taille; i++ )
        combi_msg_p[ i ] = *it[ i ];

      _compute_ext( combi_msg_p, id, msg_p_min, msg_p_max );

      if ( msg_p_min == (GUM_SCALAR)0. && msg_p_max == (GUM_SCALAR)1. )
        return;

      combi_msg_p.clear();

      ++it[ taille - 1 ];
      for ( decltype( taille ) i = taille - 1; ( i > 0 ) && ( it[ i ] == msgs_p[ i ].end() ); --i )
      {
        it[ i ] = msgs_p[ i ].begin();
        ++it[ i - 1 ];
      }
    }

  }

  /**
   * comme precedemment mais pour message parent, vraisemblance prise en compte
   */
  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_enum_combi (
    std::vector< std::vector< std::vector<GUM_SCALAR> > > & msgs_p,
    const gum::NodeId & id,
    GUM_SCALAR & msg_l_min,
    GUM_SCALAR & msg_l_max,
    std::vector<GUM_SCALAR> & lx,
    const gum::Idx & pos
  )
  {
    auto taille = msgs_p.size();

    // one parent node, the one receiving the message
    if ( taille == 0 )
    {
      GUM_SCALAR num_min = cn->get_CPT_min()[ id ][ 1 ];
      GUM_SCALAR num_max = cn->get_CPT_max()[ id ][ 1 ];
      GUM_SCALAR den_min = cn->get_CPT_min()[ id ][ 0 ];
      GUM_SCALAR den_max = cn->get_CPT_max()[ id ][ 0 ];

      _compute_ext( msg_l_min, msg_l_max, lx, num_min, num_max, den_min, den_max );

      return;
    }

    decltype( taille ) msgPerm = 1;
    #pragma omp parallel
    {
      GUM_SCALAR msg_lmin = msg_l_min;
      GUM_SCALAR msg_lmax = msg_l_max;
      std::vector< std::vector<GUM_SCALAR> > combi_msg_p( taille );

      decltype( taille ) confs = 1;
      #pragma omp for
      for ( decltype( taille ) i = 0; i < taille; i++ )
        confs *= msgs_p[ i ].size();

      #pragma omp atomic
      msgPerm *= confs;
      #pragma omp barrier
      #pragma omp flush(msgPerm)

      // direct binary representation of config, no need for iterators
      #pragma omp for
      for ( decltype( msgPerm ) j = 0; j < msgPerm; j++ ) {
        // get jth msg :
        auto jvalue = j;

        for ( decltype( taille ) i = 0; i < taille; i++ ) {
          if ( msgs_p[ i ].size() == 2 ) {
            combi_msg_p[ i ] = ( jvalue & 1 ) ? msgs_p[ i ][ 1 ] : msgs_p[ i ][ 0 ];
            jvalue /= 2;
          }
          else
            combi_msg_p[ i ] = msgs_p[ i ][ 0 ];
        }

        _compute_ext( combi_msg_p, id, msg_lmin, msg_lmax, lx, pos );
      }

      // there may be more threads here than in the for loop, therefor positive test is NECESSARY (init is -2)
      #pragma omp critical(msglminmax)
      {
        #pragma omp flush(msg_l_min)
        #pragma omp flush(msg_l_max)
        if ( ( msg_l_min > msg_lmin || msg_l_min == -2 ) && msg_lmin > 0 )
          msg_l_min = msg_lmin;
        if ( ( msg_l_max < msg_lmax || msg_l_max == -2 ) && msg_lmax > 0 )
          msg_l_max = msg_lmax;
      }

    }

    return;

    // old mono threaded version with iterators //

    std::vector< typename std::vector< std::vector<GUM_SCALAR> >::iterator > it( taille );

    for ( decltype( taille ) i = 0; i < taille; i++ )
      it[ i ] = msgs_p[ i ].begin();

    auto theEnd = msgs_p[0].end();

    while( it[ 0 ] != theEnd )
    {
      std::vector< std::vector<GUM_SCALAR> > combi_msg_p( taille );

      for ( decltype( taille ) i = 0; i < taille; i++ )
        combi_msg_p[ i ] = *it[ i ];

      _compute_ext( combi_msg_p, id, msg_l_min, msg_l_max, lx, pos );

      if ( msg_l_min == (GUM_SCALAR)0. && msg_l_max == _INF )
        return;

      combi_msg_p.clear();

      ++it[ taille - 1 ];
      for ( decltype( taille ) i = taille - 1; (i > 0) && ( it[ i ] == msgs_p[ i ].end() ); --i )
      {
        it[ i ] = msgs_p[ i ].begin();
        ++it[ i - 1 ];
      }
    }

  }

  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::makeInference ( )
  {
    if ( _InferenceUpToDate )
      return;

    _initialize();

    infE::initApproximationScheme();

    switch ( __inferenceType )
    {
      case nodeToNeighbours:
        _makeInferenceNodeToNeighbours();
        break;

      case ordered:
        _makeInferenceByOrderedArcs();
        break;

      case randomOrder:
        _makeInferenceByRandomOrder();
        break;
    }

    _updateMarginals();

    _computeExpectations();

    _InferenceUpToDate = true;
  }

  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::eraseAllEvidence ( ) 
  {
    infE::eraseAllEvidence();

    _ArcsL_min.clear(); _ArcsL_max.clear();
    _ArcsP_min.clear(); _ArcsP_max.clear();
    _NodesL_min.clear(); _NodesL_max.clear();
    _NodesP_min.clear(); _NodesP_max.clear();

    _InferenceUpToDate = false;

    if ( _msg_l_sent.size() > 0 )
    {
      for ( auto it = bnet->beginNodes(), theEnd = bnet->endNodes(); it != theEnd; ++it )
        delete _msg_l_sent[ *it ];
    }

    _msg_l_sent.clear();
    _update_l.clear();
    _update_p.clear();

    active_nodes_set.clear();
    next_active_nodes_set.clear();
  }

  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_initialize ( )
  {
    gum::DAG graphe = bnet->dag();

    const Sequence<NodeId> & topoNodes = bnet->topologicalOrder();

    // use const iterators with cbegin when available
    for ( auto it = topoNodes.begin(), theEnd = topoNodes.end(); it != theEnd; ++it )
    {
      _update_p.set( *it, false );
      _update_l.set( *it, false );

      gum::NodeSet * _parents = new gum::NodeSet();
      _msg_l_sent.set( *it , _parents );

      // accelerer init pour evidences
      if ( infE::_evidence.exists( *it ) )
      {
        active_nodes_set.insert( *it );
        _update_l.set( *it, true );
        _update_p.set( *it, true );

        if ( infE::_evidence[ *it ][ 1 ] == (GUM_SCALAR) 1. )
        {
          _NodesL_min.set( *it, _INF );
          _NodesP_min.set( *it, (GUM_SCALAR) 1.);
        }
        else if ( infE::_evidence[ *it ][ 1 ] == (GUM_SCALAR) 0. )
        {
          _NodesL_min.set( *it, (GUM_SCALAR) 0. );
          _NodesP_min.set( *it, (GUM_SCALAR) 0. );
        }

        std::vector<GUM_SCALAR> marg( 2 );
        marg[ 1 ] = _NodesP_min[ *it ];
        marg[ 0 ] = 1 - marg[ 1 ];
        
        infE::_oldMarginalMin.set( *it, marg );
        infE::_oldMarginalMax.set( *it, marg );

        continue;
      }

      gum::NodeSet _par = graphe.parents( *it );
      gum::NodeSet _enf = graphe.children( *it );

      if ( _par.size() == 0 )
      {
        active_nodes_set.insert( *it );
        _update_p.set( *it, true );
        _update_l.set( *it, true );
      }

      if ( _enf.size() == 0 ) 
      {
        active_nodes_set.insert( *it );  
        _update_p.set( *it, true );
        _update_l.set( *it, true );
      }

      /**
       * messages and so parents need to be read in order of appearance
       * use potentials instead of dag
       */		
      const gum::Potential<GUM_SCALAR> * parents = &bnet->cpt( *it );

      std::vector< std::vector< std::vector<GUM_SCALAR> > > msgs_p;
      std::vector< std::vector<GUM_SCALAR> > msg_p;
      std::vector<GUM_SCALAR> distri( 2 );

      // +1 from start to avoid counting itself
      // use const iterators when available with cbegin
      for ( auto jt = ++parents->begin(), theEnd = parents->end(); jt != theEnd; ++jt )
      {
        // compute probability distribution to avoid doing it multiple times (at each combination of messages)
        distri[ 1 ] = _NodesP_min[ bnet->nodeId( **jt ) ];
        distri[ 0 ] = (GUM_SCALAR) 1. - distri[ 1 ];
        msg_p.push_back( distri );

        if ( _NodesP_max.exists( bnet->nodeId( **jt ) ) )
        {    
          distri[ 1 ] = _NodesP_max[ bnet->nodeId( **jt ) ];
          distri[ 0 ] = (GUM_SCALAR) 1. - distri[ 1 ];
          msg_p.push_back( distri );
        }
        msgs_p.push_back( msg_p );
        msg_p.clear();
      }

      GUM_SCALAR msg_p_min = 1.;
      GUM_SCALAR msg_p_max = 0.;

      _enum_combi( msgs_p, *it, msg_p_min, msg_p_max );

      if ( msg_p_min <= (GUM_SCALAR) 0. )
        msg_p_min =  (GUM_SCALAR) 0.;
      if ( msg_p_max <= (GUM_SCALAR) 0. )
        msg_p_max =  (GUM_SCALAR) 0.;


      _NodesP_min.set( *it, msg_p_min );
      std::vector<GUM_SCALAR> marg( 2 );
      marg[ 1 ] = msg_p_min;
      marg[ 0 ] = 1 - msg_p_min;

      infE::_oldMarginalMin.set( *it, marg );

      if ( msg_p_min != msg_p_max )
      {
        marg[ 1 ] = msg_p_max;
        marg[ 0 ] = 1 - msg_p_max;
        _NodesP_max.insert( *it, msg_p_max );
      }
      infE::_oldMarginalMax.set( *it, marg );

      _NodesL_min.set( *it, (GUM_SCALAR) 1. );
    }

    for ( auto it = bnet->beginArcs(), theEnd = bnet->endArcs(); it != theEnd; ++it ) 
    {		
      _ArcsP_min.set( *it, _NodesP_min[ it->tail() ] );
      
      if ( _NodesP_max.exists( it->tail() ) )
        _ArcsP_max.set( *it, _NodesP_max[ it->tail() ] );

      _ArcsL_min.set( *it, _NodesL_min[ it->tail() ] );
    }
    
  }


  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_makeInferenceNodeToNeighbours ( )
  {	
    gum::DAG graphe = bnet->dag();

    GUM_SCALAR eps;
    // to validate TestSuite
    infE::continueApproximationScheme ( 1., false );

    do	
    {
      for ( auto it = active_nodes_set.begin(), theEnd = active_nodes_set.end(); it != theEnd; ++it )
      {
        gum::NodeSet _enfants = graphe.children( *it );

        for ( auto jt = _enfants.begin(), theEnd2 = _enfants.end(); jt != theEnd2; ++jt )
          _msgP( *it, *jt );

        gum::NodeSet _parents = graphe.parents( *it );

        for ( auto kt = _parents.begin(), theEnd2 = _parents.end(); kt != theEnd2; ++kt )
          _msgL(*it,*kt);
      }

      eps = _calculateEpsilon();

      infE::updateApproximationScheme();

      active_nodes_set.clear();
      active_nodes_set = next_active_nodes_set;
      next_active_nodes_set.clear();

    } while( infE::continueApproximationScheme ( eps, false/*, false*/ ) && active_nodes_set.size() > 0 );

  }


  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_makeInferenceByRandomOrder ( )
  {	
    auto nbrArcs = bnet->dag().sizeArcs();

    std::vector< cArcP > seq;
    seq.reserve( nbrArcs );

    for ( auto it = bnet->beginArcs(), theEnd = bnet->endArcs(); it != theEnd; ++it )
      seq.push_back( &( *it ) );

    GUM_SCALAR eps;
    // validate TestSuite
    infE::continueApproximationScheme ( 1., false );

    do
    {
      for ( decltype(nbrArcs) j = 0, theEnd = nbrArcs / 2; j < theEnd; j++ )
      {
        auto w1 = rand() % nbrArcs, w2 = rand() % nbrArcs;
        if ( w1 == w2 ) continue;
        std::swap( seq[ w1 ], seq[ w2 ] );
      }

      for ( auto it = seq.begin(), theEnd = seq.end(); it != theEnd; ++it )
      {
        _msgP( (*it)->tail(), (*it)->head() );
        _msgL( (*it)->head(), (*it)->tail() );
      }

      eps = _calculateEpsilon();

      infE::updateApproximationScheme();

    } while( infE::continueApproximationScheme( eps, false ) );

  }

  // gives slightly worse results for some variable/modalities than other inference types (node D on 2U network loose 0.03 precision)
  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_makeInferenceByOrderedArcs ( )
  {
    auto nbrArcs = bnet->dag().sizeArcs();

    std::vector< cArcP > seq;
    seq.reserve( nbrArcs );

    for ( auto it = bnet->beginArcs(), theEnd = bnet->endArcs(); it != theEnd; ++it )
      seq.push_back( &( *it ) );

    GUM_SCALAR eps;
    // validate TestSuite
    infE::continueApproximationScheme ( 1., false );

    do
    {
      for ( auto it = seq.begin(), theEnd = seq.end(); it != theEnd; ++it )
      {
        _msgP( (*it)->tail(), (*it)->head() );
        _msgL( (*it)->head(), (*it)->tail() );
      }

      eps = _calculateEpsilon();

      infE::updateApproximationScheme();

    } while( infE::continueApproximationScheme( eps, false ) );

  }


  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_msgL ( const NodeId Y, const NodeId X )
  {
    NodeSet const & children = bnet->dag().children( Y );
    NodeSet const & _parents = bnet->dag().parents( Y );

    const gum::Potential<GUM_SCALAR> * parents = &bnet->cpt( Y );


    if ( ( ( children.size() + parents->nbrDim() - 1 ) == 1 ) && ( ! infE::_evidence.exists( Y ) ) )
      return;

    bool update_l = _update_l[ Y ];
    bool update_p = _update_p[ Y ];

    if ( ! update_p && ! update_l )
      return;

    _msg_l_sent[ Y ]->insert( X );

    // for future refresh LM/PI
    if ( _msg_l_sent[ Y ]->size() == _parents.size() )
    {
      _msg_l_sent[ Y ]->clear();
      _update_l[ Y ] = false;
    }

    // refresh LM_part
    if ( update_l )
    {
      if ( ! children.empty() && ! infE::_evidence.exists( Y ) ) 
      {
        GUM_SCALAR lmin = 1.;
        GUM_SCALAR lmax = 1.;

        for ( auto it = children.begin(), theEnd = children.end(); it != theEnd; ++it )
        {
          lmin *= _ArcsL_min[ Arc( Y, *it ) ];

          if ( _ArcsL_max.exists( Arc( Y, *it ) ) )
            lmax *= _ArcsL_max[ Arc( Y, *it ) ];
          else
            lmax *= _ArcsL_min[ Arc( Y, *it ) ];
        }

        if ( lmin != lmin && lmax == lmax )
          lmin = lmax;
        if ( lmax != lmax && lmin == lmin )
          lmax = lmin;
        if ( lmax != lmax && lmin != lmin )
          std::cout << "pas de vraisemblance definie [lmin, lmax] (observations incompatibles ?)" << std::endl;

        if ( lmin < 0. )
          lmin = 0.;
        if ( lmax < 0. )
          lmax = 0.;

        // no need to update nodeL if evidence since nodeL will never be used

        _NodesL_min[ Y ] = lmin;
        if ( lmin != lmax )
          _NodesL_max.set( Y, lmax );
        else
          if ( _NodesL_max.exists( Y ) )
            _NodesL_max.erase( Y );

      } // end of : node has children & no evidence

    } // end of : if update_l 


    GUM_SCALAR lmin = _NodesL_min[ Y ];
    GUM_SCALAR lmax;

    if ( _NodesL_max.exists( Y ) )
      lmax =  _NodesL_max[ Y ];
    else
      lmax = lmin;

    /**
     *  lmin == lmax == 1  => sends 1 as message to parents
     */

    if ( lmin == lmax && lmin == 1. )
    {
      _ArcsL_min[ Arc( X, Y ) ] = lmin;

      if ( _ArcsL_max.exists( Arc( X, Y ) ) )
        _ArcsL_max.erase( Arc( X, Y ) );

      return;
    }


    // garder pour chaque noeud un table des parents maj, une fois tous maj, stop jusque notification msg L ou P

    if (  update_p || update_l )
    {
      std::vector< std::vector< std::vector<GUM_SCALAR> > > msgs_p;
      std::vector< std::vector<GUM_SCALAR> > msg_p;
      std::vector<GUM_SCALAR> distri(2);

      gum::Idx pos;

      // +1 from start to avoid counting itself
      // use const iterators with cbegin when available
      for ( auto jt = ++parents->begin(), theEnd = parents->end(); jt != theEnd; ++jt )
      {
        if ( bnet->nodeId( **jt ) == X )
        {
          pos = parents->pos( **jt ) - 1; // retirer la variable courante de la taille
          continue;
        }

        // compute probability distribution to avoid doing it multiple times (at each combination of messages)
        distri[ 1 ] = _ArcsP_min[ Arc( bnet->nodeId( **jt ), Y ) ];
        distri[ 0 ] = 1. - distri[ 1 ];
        msg_p.push_back( distri );

        if ( _ArcsP_max.exists( Arc( bnet->nodeId( **jt ), Y ) ) )
        {
          distri[ 1 ] = _ArcsP_max[ Arc( bnet->nodeId( **jt ), Y ) ];
          distri[ 0 ] = 1. - distri[ 1 ];
          msg_p.push_back( distri );
        }
        msgs_p.push_back( msg_p );
        msg_p.clear();
      }

      GUM_SCALAR min = -2.;
      GUM_SCALAR max = -2.;

      std::vector< GUM_SCALAR > lx;
      lx.push_back( lmin );

      if ( lmin != lmax )
        lx.push_back( lmax );

      _enum_combi( msgs_p, Y, min, max, lx, pos );

      if ( min == -2. || max == -2. )
      {
        if ( min != -2. )
          max = min;
        else if ( max != -2. )
          min = max;
        else
        {
          std::cout << std::endl;
          std::cout << "!!!! pas de message L calculable !!!!" << std::endl;
          return;
        }
      }

      if ( min < 0. )
        min = 0.;
      if ( max < 0. )
        max = 0.;

      bool update = false;

      if ( min != _ArcsL_min[ Arc( X, Y ) ] )
      {
        _ArcsL_min[ Arc( X, Y ) ] = min;
        update = true;
      }

      if ( _ArcsL_max.exists( Arc( X, Y ) ) )
      {
        if ( max != _ArcsL_max[ Arc( X, Y ) ] )
        {
          if ( max != min )
            _ArcsL_max[ Arc( X, Y ) ] = max;
          else //if ( max == min )
            _ArcsL_max.erase( Arc( X, Y ) );

          update = true;
        }
      }
      else
      {
        if ( max != min)
        {
          _ArcsL_max.insert( Arc( X, Y ), max );
          update = true;
        }
      }

      if ( update )
      {
        _update_l.set( X, true );
        next_active_nodes_set.insert( X );
      }

    } // end of update_p || update_l

  }


  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_msgP ( const NodeId X, const NodeId demanding_child )
  {
    NodeSet const & children = bnet->dag().children( X );
    
    const gum::Potential<GUM_SCALAR> * parents = &bnet->cpt( X );

    if ( ( ( children.size() + parents->nbrDim() - 1) == 1) && ( ! infE::_evidence.exists( X ) ) )
      return;

    // LM_part ---- from all children but one --- the lonely one will get the message 

    if ( infE::_evidence.exists( X ) ) 
    {
      _ArcsP_min[ Arc( X, demanding_child ) ] = infE::_evidence[ X ][ 1 ];
      
      if ( _ArcsP_max.exists( Arc( X, demanding_child ) ) )
        _ArcsP_max.erase( Arc( X, demanding_child ) );

      return;
    }


    bool update_l = _update_l[ X ];
    bool update_p = _update_p[ X ];

    if ( ! update_p && ! update_l )
      return;

    GUM_SCALAR lmin = 1.;
    GUM_SCALAR lmax = 1.;

    // use cbegin if available
    for ( auto it = children.begin(), theEnd = children.end(); it != theEnd; ++it )
    {
      if ( *it == demanding_child ) continue;

      lmin *= _ArcsL_min[ Arc( X, *it ) ];

      if ( _ArcsL_max.exists( Arc( X, *it ) ) )
        lmax *= _ArcsL_max[ Arc( X, *it ) ];
      else
        lmax *= _ArcsL_min[ Arc( X, *it ) ];
    }

    if ( lmin != lmin && lmax == lmax )
      lmin = lmax;
    if (lmax != lmax && lmin == lmin)
      lmax = lmin;
    if (lmax != lmax && lmin != lmin) {
      std::cout << "pas de vraisemblance definie [lmin, lmax] (observations incompatibles ?)" << std::endl;
      return;
    }


    if ( lmin < 0. )
      lmin = 0.;
    if ( lmax < 0. )
      lmax = 0.;

   
    // refresh PI_part 
    GUM_SCALAR min = _INF;
    GUM_SCALAR max = 0.;

    if ( update_p )
    {
      std::vector< std::vector< std::vector<GUM_SCALAR> > > msgs_p;
      std::vector< std::vector<GUM_SCALAR> > msg_p;
      std::vector<GUM_SCALAR> distri( 2 );

      // +1 from start to avoid counting itself
      // use const_iterators if available
      for ( auto jt = ++parents->begin(), theEnd = parents->end(); jt != theEnd; ++jt )
      {
        // compute probability distribution to avoid doing it multiple times (at each combination of messages)
        distri[ 1 ] = _ArcsP_min[ Arc( bnet->nodeId( **jt ), X ) ];
        distri[ 0 ] = 1. - distri[ 1 ];
        msg_p.push_back( distri );

        if ( _ArcsP_max.exists( Arc( bnet->nodeId( **jt ), X ) ) )
        {    
          distri[ 1 ] = _ArcsP_max[ Arc( bnet->nodeId( **jt ), X ) ];
          distri[ 0 ] = 1. - distri[ 1 ];
          msg_p.push_back( distri );
        }
        msgs_p.push_back( msg_p );
        msg_p.clear();
      }

      _enum_combi( msgs_p, X, min, max );

      if (min < 0.)
        min = 0.;
      if (max < 0.)
        max = 0.;

      if ( min == _INF || max == _INF )
      {
        std::cout << " ERREUR msg P min = max = INF " <<std::endl; std::cout.flush();
        return;
      }

      _NodesP_min[ X ] = min;

      if ( min != max )
        _NodesP_max.set( X, max );
      else
        if ( _NodesP_max.exists( X ) )
          _NodesP_max.erase( X );

      _update_p.set( X, false );

    } // end of update_p
    else
    {
      min = _NodesP_min[ X ];
      if ( _NodesP_max.exists( X ) )
        max = _NodesP_max[ X ];
      else
        max = min;
    }

    if (  update_p ||  update_l )
    {
      GUM_SCALAR msg_p_min;
      GUM_SCALAR msg_p_max;

      // cas limites sur min	
      if ( min == _INF && lmin == 0. )
        std::cout << "MESSAGE P ERR (negatif) : pi = inf, l = 0" << std::endl;

      if ( lmin == _INF ) // cas infini
        msg_p_min = 1.;
      else if ( min == 0. || lmin == 0. )
        msg_p_min = 0;
      else
        msg_p_min = 1. / ( 1. + ( (1. / min - 1.) * 1. / lmin ));

      // cas limites sur max	
      if ( max == _INF && lmax == 0. )
        std::cout << "MESSAGE P ERR (negatif) : pi = inf, l = 0" << std::endl;

      if ( lmax == _INF ) // cas infini
        msg_p_max = 1.;	
      else if ( max == 0. || lmax == 0. )
        msg_p_max = 0;
      else
        msg_p_max = 1. / ( 1. + ( (1. / max - 1.) * 1. / lmax ));

      if ( msg_p_min != msg_p_min && msg_p_max == msg_p_max )
      {
        msg_p_min = msg_p_max;
        std::cout << std::endl;
        std::cout << "msg_p_min is NaN" << std::endl;
      }
      if ( msg_p_max != msg_p_max && msg_p_min == msg_p_min )
      {
        msg_p_max = msg_p_min;
        std::cout << std::endl;
        std::cout << "msg_p_max is NaN" << std::endl;

      }
      if ( msg_p_max != msg_p_max && msg_p_min != msg_p_min )
      {
        std::cout << std::endl;
        std::cout << "pas de message P calculable (verifier observations)" << std::endl;
        return;
      }

      if ( msg_p_min < 0. )
        msg_p_min = 0.;
      if ( msg_p_max < 0. )
        msg_p_max = 0.;


      bool update = false;
      if ( msg_p_min != _ArcsP_min[ Arc( X, demanding_child ) ] )
      {
        _ArcsP_min[ Arc( X, demanding_child ) ] = msg_p_min;
        update = true;
      }

      if ( _ArcsP_max.exists( Arc( X, demanding_child ) ) )
      {
        if ( msg_p_max != _ArcsP_max[ Arc( X, demanding_child ) ] )
        {
          if ( msg_p_max != msg_p_min )
            _ArcsP_max[ Arc( X, demanding_child ) ] = msg_p_max;
          else //if ( msg_p_max == msg_p_min )
            _ArcsP_max.erase( Arc( X, demanding_child ) );

          update = true;
        }
      }
      else
      {
        if ( msg_p_max != msg_p_min )
        {
          _ArcsP_max.insert( Arc( X, demanding_child ), msg_p_max );
          update = true;
        }
      }

      if ( update )
      {
        _update_p.set( demanding_child, true );
        next_active_nodes_set.insert( demanding_child );
      }

    } // end of : update_l || update_p

  }

  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_refreshLMsPIs ( ) 
  {
    for ( auto itX = bnet->beginNodes(), theEnd = bnet->endNodes(); itX != theEnd; ++itX )
    {
      NodeSet const & children = bnet->dag().children( *itX );

      const gum::Potential<GUM_SCALAR> * parents = &bnet->cpt( *itX );

      if ( _update_l[ *itX ] )
      {
        GUM_SCALAR lmin = 1.;		  
        GUM_SCALAR lmax = 1.;
        
        if ( ! children.empty() && ! infE::_evidence.exists( *itX ) )
        {
          for (NodeSet::const_iterator it = children.begin(); it != children.end(); ++it)
          {
            lmin *= _ArcsL_min[ Arc( *itX, *it ) ];

            if ( _ArcsL_max.exists( Arc( *itX, *it ) ) )
              lmax *= _ArcsL_max[ Arc( *itX, *it ) ];
            else
              lmax *= _ArcsL_min[ Arc( *itX, *it ) ];
          }

          if ( lmin != lmin && lmax == lmax )
            lmin = lmax;
          if ( lmax != lmax && lmin == lmin )
            lmax = lmin;
          if ( lmax != lmax && lmin != lmin ) {
            std::cout << "pas de vraisemblance definie [lmin, lmax] (observations incompatibles ?)" << std::endl;
            return;
          }

          if (lmin < 0.)
            lmin = 0.;
          if (lmax < 0.)
            lmax = 0.;
 
          _NodesL_min[ *itX ] = lmin;

          if ( lmin != lmax )
            _NodesL_max.set( *itX, lmax );
          else
            if ( _NodesL_max.exists( *itX ) )
              _NodesL_max.erase( *itX );
        }

      } // end of : update_l
      
      
      if (_update_p[ *itX ])
      {
        if ( (parents->nbrDim() - 1) > 0  && ! infE::_evidence.exists( *itX ) )
        {
          std::vector< std::vector< std::vector<GUM_SCALAR> > > msgs_p;
          std::vector< std::vector<GUM_SCALAR> > msg_p;
          std::vector<GUM_SCALAR> distri( 2 );

          // +1 from start to avoid counting itself
          // cbegin
          for ( auto jt = ++parents->begin(), theEnd = parents->end(); jt != theEnd; ++jt )
          {
            // compute probability distribution to avoid doing it multiple times (at each combination of messages)
            distri[ 1 ] = _ArcsP_min[ Arc( bnet->nodeId( **jt ), *itX ) ];
            distri[ 0 ] = 1. - distri[ 1 ];
            msg_p.push_back( distri );

            if ( _ArcsP_max.exists( Arc( bnet->nodeId( **jt ), *itX ) ) )
            {    
              distri[ 1 ] = _ArcsP_max[ Arc( bnet->nodeId( **jt ), *itX ) ];
              distri[ 0 ] = 1. - distri[ 1 ];
              msg_p.push_back( distri );
            }
            msgs_p.push_back( msg_p );
            msg_p.clear();
          }

          GUM_SCALAR min = _INF;
          GUM_SCALAR max = 0.;

          _enum_combi( msgs_p, *itX, min, max );


          if (min < 0.)
            min = 0.;
          if (max < 0.)
            max = 0.;

          _NodesP_min[ *itX ] = min;

          if ( min != max )
            _NodesP_max.set( *itX, max );
          else
            if ( _NodesP_max.exists( *itX ) )
              _NodesP_max.erase( *itX );

          _update_p[ *itX ] = false;
        }
      } // end of update_p

    } // end of : for each node

  }

  template< typename GUM_SCALAR >
  void LoopyPropagation<GUM_SCALAR>::_updateMarginals ( )
  {
    for ( auto it = bnet->beginNodes(), theEnd = bnet->endNodes(); it != theEnd; ++it)
    {	
      GUM_SCALAR msg_p_min = 1.;
      GUM_SCALAR msg_p_max = 0.;

      if ( infE::_evidence.exists( *it ) )
      {
        if ( infE::_evidence[ *it ][ 1 ] == 0. )
          msg_p_min = (GUM_SCALAR) 0.;
        else if ( infE::_evidence[ *it ][ 1 ] == 1. )
          msg_p_min = 1.;

        msg_p_max = msg_p_min;
      }
      else
      {
        GUM_SCALAR min = _NodesP_min[ *it ];
        GUM_SCALAR max;

        if ( _NodesP_max.exists( *it ) )
          max = _NodesP_max[ *it ];
        else
          max = min;

        GUM_SCALAR lmin = _NodesL_min[ *it ];
        GUM_SCALAR lmax;

        if ( _NodesL_max.exists( *it ) )
          lmax = _NodesL_max[ *it ];
        else
          lmax = lmin;

        if ( min == _INF || max == _INF )
        {
          std::cout << " min ou max === _INF !!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
          return;
        }

        if ( min == _INF && lmin == 0. ) {
          std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;
          return;
        }

        if ( lmin == _INF  ) 
          msg_p_min = 1.;
        else if ( min == 0. || lmin == 0.)
          msg_p_min = 0;
        else
          msg_p_min = 1. / ( 1. + ( (1. / min - 1.) * 1. / lmin ));

        if ( max == _INF && lmax == 0. ) {
          std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;
          return;
        }

        if ( lmax == _INF )        
          msg_p_max = 1.;
        else if ( max == 0. || lmax == 0.)
          msg_p_max = 0;	 
        else
          msg_p_max = 1. / ( 1. + ( (1. / max - 1.) * 1. / lmax ));
      }

      if ( msg_p_min != msg_p_min && msg_p_max == msg_p_max )
      {
        msg_p_min = msg_p_max;
        std::cout << std::endl;
        std::cout << "msg_p_min is NaN" << std::endl;
      }
      if ( msg_p_max != msg_p_max && msg_p_min == msg_p_min )
      {
        msg_p_max = msg_p_min;
        std::cout << std::endl;
        std::cout << "msg_p_max is NaN" << std::endl;

      }
      if ( msg_p_max != msg_p_max && msg_p_min != msg_p_min )
      {
        std::cout << std::endl;
        std::cout << "pas de proba calculable (verifier observations)" << std::endl;
        return;
      }

      if (msg_p_min < 0.)
        msg_p_min = 0.;
      if (msg_p_max < 0.)
        msg_p_max = 0.;

      infE::_marginalMin[ *it ][ 0 ] = 1 - msg_p_max;
      infE::_marginalMax[ *it ][ 0 ] = 1 - msg_p_min;
      infE::_marginalMin[ *it ][ 1 ] = msg_p_min;
      infE::_marginalMax[ *it ][ 1 ] = msg_p_max;

    }
  }


  template<typename GUM_SCALAR>
  GUM_SCALAR LoopyPropagation<GUM_SCALAR>::_calculateEpsilon ( ) 
  {
    _refreshLMsPIs();
    _updateMarginals();

    return infE::_computeEpsilon();
  }

  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::_computeExpectations ( )
  {
    if ( infE::_modal.empty() )
      return;

    std::vector< std::vector< GUM_SCALAR > > vertices ( 2, std::vector< GUM_SCALAR > (2) );
    for ( auto it = bnet->beginNodes(), theEnd = bnet->endNodes(); it != theEnd; ++it )
    {
      vertices[ 0 ] = infE::_marginalMin[ *it ];
      vertices[ 1 ] = infE::_marginalMax[ *it ];

      for ( auto vertex = 0, vend = 2; vertex != vend; vertex++ )
        infE::_updateExpectations( *it, vertices[ vertex ] );
    }
  }


  template<typename GUM_SCALAR>
  LoopyPropagation<GUM_SCALAR>::LoopyPropagation ( const CredalNet<GUM_SCALAR> &cnet ) : CNInferenceEngine< GUM_SCALAR >::CNInferenceEngine ( cnet )
  {
    if ( ! cnet.isSeparatelySpecified() )
      GUM_ERROR (OperationNotAllowed, "LoopyPropagation is only available with separately specified nets");

    // test for binary cn
    for ( auto it = cnet.current_bn().beginNodes(), theEnd = cnet.current_bn().endNodes(); it != theEnd; ++it )
      if ( cnet.current_bn().variable( *it ).domainSize() != 2 )
        GUM_ERROR (OperationNotAllowed, "LoopyPropagation is only available with binary credal networks");

    cn = &cnet;
    bnet = &cnet.current_bn();

    __inferenceType = nodeToNeighbours;
    _InferenceUpToDate = false;

    GUM_CONSTRUCTOR( LoopyPropagation );
  }


  template<typename GUM_SCALAR>
  LoopyPropagation<GUM_SCALAR>::~LoopyPropagation ( )
  {
    /*_ArcsL_min.clear(); _ArcsL_max.clear();
    _ArcsP_min.clear(); _ArcsP_max.clear();
    _NodesL_min.clear(); _NodesL_max.clear();
    _NodesP_min.clear(); _NodesP_max.clear();*/

    _InferenceUpToDate = false;

    if (_msg_l_sent.size() > 0)
    {
      for (DAG::NodeIterator it = bnet->beginNodes(); it != bnet->endNodes(); ++it)
        delete _msg_l_sent[*it];
    }
    //_msg_l_sent.clear();
    //_update_l.clear();
    //_update_p.clear();

    GUM_DESTRUCTOR( LoopyPropagation );
  }

  template<typename GUM_SCALAR>
  void LoopyPropagation<GUM_SCALAR>::inferenceType ( InferenceType inft )
  {
    __inferenceType = inft;
  }

  template<typename GUM_SCALAR>
  typename LoopyPropagation<GUM_SCALAR>::InferenceType LoopyPropagation<GUM_SCALAR>::inferenceType ( )
  {
    return __inferenceType;
  }


} // end of namespace gum


