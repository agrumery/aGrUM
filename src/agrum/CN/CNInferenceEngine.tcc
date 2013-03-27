#include <agrum/core/exceptions.h>
#include <map>

namespace gum {
  template< typename GUM_SCALAR >
  CNInferenceEngine< GUM_SCALAR >::CNInferenceEngine () : ApproximationScheme() {
    GUM_CONSTRUCTOR ( CNInferenceEngine );
  }

  template< typename GUM_SCALAR >
  CNInferenceEngine< GUM_SCALAR >::CNInferenceEngine ( const CredalNet< GUM_SCALAR > & credalNet ) : ApproximationScheme() {
    _credalNet = &credalNet;

    for ( auto id = _credalNet->current_bn().beginNodes(), theEnd = _credalNet->current_bn().endNodes(); id != theEnd; ++id ) {
      auto dSize = _credalNet->current_bn().variable ( *id ).domainSize();
      _marginalMin.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 1 ) );
      _marginalMax.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 0 ) );
      _marginalSets.insert ( *id, std::vector< std::vector< GUM_SCALAR > >() );
    }
    
    GUM_CONSTRUCTOR ( CNInferenceEngine );
  }

  template< typename GUM_SCALAR >
  CNInferenceEngine< GUM_SCALAR >::~CNInferenceEngine() {
    GUM_DESTRUCTOR ( CNInferenceEngine );
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::eraseAllEvidence() {
    _evidence.clear();
    _query.clear();

    _marginalMin.clear();
    _marginalMax.clear();
    _expectationMin.clear();
    _expectationMax.clear();
    _marginalSets.clear();
    _dynamicExpMin.clear();
    _dynamicExpMax.clear();
    _modal.clear();

    _t0.clear();
    _t1.clear();

    for ( auto id = _credalNet->current_bn().beginNodes(), theEnd = _credalNet->current_bn().endNodes(); id != theEnd; ++id ) {
      auto dSize = _credalNet->current_bn().variable ( *id ).domainSize();
      _marginalMin.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 1 ) );
      _marginalMax.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 0 ) );
      _marginalSets.insert ( *id, std::vector< std::vector< GUM_SCALAR > >() );
    }
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::insertModalsFile( const std::string &path) {
    std::ifstream mod_stream ( path.c_str(), std::ios::in );

    if ( !mod_stream.good() ) {
      GUM_ERROR(OperationNotAllowed, "void CNInferenceEngine< GUM_SCALAR >::insertModals(const std::string & path) : could not open input file : " << path );
    }

    if( ! _modal.empty() )
      _modal.clear();

    std::string line, tmp;
    char *cstr, *p;

    while ( mod_stream.good() ) {
      getline ( mod_stream, line );

      if ( line.size() == 0 ) continue;

      cstr = new char [line.size() + 1];
      strcpy ( cstr, line.c_str() );

      p = strtok ( cstr, " " );
      tmp = p;

      std::vector< GUM_SCALAR > values;
      p = strtok ( NULL, " " );

      while ( p != NULL ) {
        values.push_back ( atof ( p ) );
        p = strtok ( NULL, " " );
      } // end of : line

      _modal[tmp] = values;

      delete[] p;
      delete[] cstr;
    } // end of : file

    mod_stream.close();

    _initExpectations();
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::insertModals( const std::map< std::string, std::vector< GUM_SCALAR > > &modals) {
    if( ! _modal.empty() )
      _modal.clear();

    _modal = modals;

    _initExpectations();
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::insertEvidence ( const std::map< std::string, std::vector< GUM_SCALAR > > & eviMap ) {
    if(! _evidence.empty() )
      _evidence.clear();

    for(auto it = eviMap.cbegin(), theEnd = eviMap.cend(); it != theEnd; ++it) {
      gum::NodeId id;
      try {
        id = _credalNet->current_bn().idFromName(it->first);
      } catch ( gum::NotFound & err) {
        GUM_SHOWERROR ( err );
        continue;
      }
      _evidence.insert ( id, it->second );
    }
  }

  // check that observed variables DO exists in the network (otherwise Lazy report an error and app crash)
  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::insertEvidence ( const typename gum::Property< std::vector < GUM_SCALAR > >::onNodes &evidence ) {
    if(! _evidence.empty() )
      _evidence.clear();

    // use cbegin() to get const_iterator when available in aGrUM hashtables
    for ( auto it = evidence.begin(), theEnd = evidence.end(); it != theEnd; ++it ) {
      try {
         _credalNet->current_bn().variable ( it.key() );
      } catch ( gum::NotFound & err ) {
        GUM_SHOWERROR ( err );
        continue;
      }
      _evidence.insert ( it.key(), *it );
    }
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::insertEvidenceFile ( const std::string &path ) {
    std::ifstream evi_stream ( path.c_str(), std::ios::in );

    if ( !evi_stream.good() ) {
      GUM_ERROR(IOError, "void CNInferenceEngine< GUM_SCALAR >::insertEvidence(const std::string & path) : could not open input file : " << path );
    }

    if(! _evidence.empty() )
      _evidence.clear();

    std::string line, tmp;
    char *cstr, *p;

    while ( evi_stream.good() && std::strcmp ( line.c_str(), "[EVIDENCE]" ) != 0 ) {
      getline ( evi_stream, line );
    }

    while ( evi_stream.good() ) {
      getline ( evi_stream, line );

      if ( std::strcmp ( line.c_str(), "[QUERY]" ) == 0 )
        break;

      if ( line.size() == 0 ) continue;

      cstr = new char [line.size() + 1];
      strcpy ( cstr, line.c_str() );

      p = strtok ( cstr, " " );
      tmp = p;

      // if user input is wrong
      gum::NodeId node = -1;
      try {
         node = _credalNet->current_bn().idFromName ( tmp );
      } catch ( gum::NotFound &err ) {
        GUM_SHOWERROR ( err );
        continue;
      }

      std::vector< GUM_SCALAR > values;
      p = strtok ( NULL, " " );

      while ( p != NULL ) {
        values.push_back ( atoi ( p ) );
        p = strtok ( NULL, " " );
      } // end of : line

      _evidence.insert ( node, values );

      delete[] p;
      delete[] cstr;
    } // end of : file

    evi_stream.close();
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::insertQuery ( const typename gum::Property< std::vector < bool > >::onNodes &query ) {
    if(! _query.empty() )
      _query.clear();

    // use cbegin() when available
    for ( auto it = query.begin(), theEnd = query.end(); it != theEnd; ++it ) {
      try {
         _credalNet->current_bn().variable ( it.key() );
      } catch ( gum::NotFound &err ) {
        GUM_SHOWERROR ( err );
        continue;
      }
      _query.insert ( it.key(), *it );
    }
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::insertQueryFile ( const std::string &path ) {
    std::ifstream evi_stream ( path.c_str(), std::ios::in );

    if ( !evi_stream.good() ) {
      GUM_ERROR( IOError, "void CNInferenceEngine< GUM_SCALAR >::insertQuery(const std::string & path) : could not open input file : " << path );
    }

    if(! _query.empty() )
      _query.clear();

    std::string line, tmp;
    char *cstr, *p;

    while ( evi_stream.good() && std::strcmp ( line.c_str(), "[QUERY]" ) != 0 ) {
      getline ( evi_stream, line );
    }

    while ( evi_stream.good() ) {
      getline ( evi_stream, line );

      if ( std::strcmp ( line.c_str(), "[EVIDENCE]" ) == 0 )
        break;

      if ( line.size() == 0 ) continue;

      cstr = new char [line.size() + 1];
      strcpy ( cstr, line.c_str() );

      p = strtok ( cstr, " " );
      tmp = p;

      // if user input is wrong
      gum::NodeId node = -1;
      try {
         node = _credalNet->current_bn().idFromName ( tmp );
      } catch ( gum::NotFound &err ) {
        GUM_SHOWERROR ( err );
        continue;
      }

      auto dSize = _credalNet->current_bn().variable ( node ).domainSize();

      p = strtok ( NULL, " " );

      if ( p == NULL ) {
        _query.insert ( node, std::vector< bool> ( dSize, true ) );
      } else {
        std::vector< bool > values ( dSize, false );

        while ( p != NULL ) {
          if ( atoi ( p ) >= dSize )
            GUM_ERROR(OutOfBounds, "void CNInferenceEngine< GUM_SCALAR >::insertQuery(const std::string & path) : query modality is higher or equal to cardinality");

          values[atoi ( p )] = true;
          p = strtok ( NULL, " " );
        } // end of : line

        _query.insert ( node, values );
      }

      delete[] p;
      delete[] cstr;
    } // end of : file

    evi_stream.close();

  }

  template< typename GUM_SCALAR > 
  const std::vector< GUM_SCALAR > & CNInferenceEngine< GUM_SCALAR >::marginalMin( const std::string & varName ) const {
    try {
      return _marginalMin[_credalNet->current_bn().idFromName(varName)];
    } catch(gum::NotFound & err) {
      throw(err);
    }
  }

  template< typename GUM_SCALAR > 
  const std::vector< GUM_SCALAR > & CNInferenceEngine< GUM_SCALAR >::marginalMax( const std::string & varName ) const {
    try {
      return _marginalMax[_credalNet->current_bn().idFromName(varName)];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const std::vector< GUM_SCALAR > & CNInferenceEngine< GUM_SCALAR >::marginalMin ( const gum::NodeId id ) const {
    try {
      return _marginalMin[id];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const std::vector< GUM_SCALAR > & CNInferenceEngine< GUM_SCALAR >::marginalMax ( const gum::NodeId id ) const {
    try {
      return _marginalMax[id];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }

  }

  template< typename GUM_SCALAR >
  const GUM_SCALAR & CNInferenceEngine< GUM_SCALAR >::expectationMin ( const std::string & varName ) const {
    try {
      return _expectationMin[_credalNet->current_bn().idFromName(varName)];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const GUM_SCALAR & CNInferenceEngine< GUM_SCALAR >::expectationMax ( const std::string & varName ) const {
    try {
      return _expectationMax[_credalNet->current_bn().idFromName(varName)];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const GUM_SCALAR & CNInferenceEngine< GUM_SCALAR >::expectationMin( const gum::NodeId id ) const {
    try {
      return _expectationMin[id];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const GUM_SCALAR & CNInferenceEngine< GUM_SCALAR >::expectationMax( const gum::NodeId id ) const {
    try {
      return _expectationMax[id];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const std::vector< GUM_SCALAR > & CNInferenceEngine< GUM_SCALAR >::dynamicExpMin ( const std::string & varName ) const {
    std::string errTxt = "const std::vector< GUM_SCALAR > & CNInferenceEngine< GUM_SCALAR >::dynamicExpMin ( const std::string & varName ) const : ";

    if( _dynamicExpMin.empty() )
      GUM_ERROR(OperationNotAllowed, errTxt + "_dynamicExpectations() needs to be called before");

    if(_dynamicExpMin.find(varName) == _dynamicExpMin.end())
      GUM_ERROR(NotFound, errTxt + "variable name not found : " << varName);

    return _dynamicExpMin.find(varName)->second;
  }

  template< typename GUM_SCALAR >
  const std::vector< GUM_SCALAR > & CNInferenceEngine< GUM_SCALAR >::dynamicExpMax ( const std::string & varName ) const {
    std::string errTxt = "const std::vector< GUM_SCALAR > & CNInferenceEngine< GUM_SCALAR >::dynamicExpMax ( const std::string & varName ) const : ";

    if( _dynamicExpMax.empty() ) 
       GUM_ERROR(OperationNotAllowed, errTxt + "_dynamicExpectations() needs to be called before");
    
    if(_dynamicExpMin.find(varName) == _dynamicExpMin.end())
      GUM_ERROR(NotFound, errTxt + "variable name not found : " << varName);

    return _dynamicExpMax.find(varName)->second;
  }


  template< typename GUM_SCALAR >
  const std::vector< std::vector< GUM_SCALAR > > & CNInferenceEngine< GUM_SCALAR >::vertices ( const gum::NodeId id ) const {
    return _marginalSets[id];
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::saveMarginals ( const std::string &path ) const {
    std::ofstream m_stream ( path.c_str(), std::ios::out | std::ios::trunc );

    if ( ! m_stream.good() ) {
      GUM_ERROR(IOError, "void CNInferenceEngine< GUM_SCALAR >::saveMarginals(const std::string & path) const : could not open output file : " << path);
    }

    // use cbegin when available
    for ( auto it = _marginalMin.begin(), theEnd = _marginalMin.end(); it != theEnd; ++it ) {
      auto esize = it->size();
      for ( decltype(esize) mod = 0; mod < esize; mod++ ) {
        m_stream << _credalNet->current_bn().variable ( it.key() ).name() << " " << mod << " " << ( *it ) [mod] << " " << _marginalMax[it.key()][mod] << std::endl;
      }
    }

    m_stream.close();
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::saveExpectations (const std::string &path ) const {
    if(_dynamicExpMin.empty())//_modal.empty())
      return;
    // else not here, to keep the const (natural with a saving process)
    //else if(_dynamicExpMin.empty() || _dynamicExpMax.empty())
      //_dynamicExpectations(); // works with or without a dynamic network
    

    std::ofstream m_stream ( path.c_str(), std::ios::out | std::ios::trunc );

    if ( ! m_stream.good() ) {
      GUM_ERROR ( IOError, "void CNInferenceEngine< GUM_SCALAR >::saveExpectations(const std::string & path) : could not open output file : " << path );
    }

    for( auto it = _dynamicExpMin.begin(), theEnd = _dynamicExpMin.end(); it != theEnd; ++it) {
      m_stream << it->first;
      // iterates over a vector
      for( auto it2 = it->second.cbegin(), theEnd2 = it->second.cend(); it2 != theEnd2; ++it2) {
        m_stream << " " << *it2;
      }
      m_stream << "\n";
    }

    for( auto it = _dynamicExpMax.begin(), theEnd = _dynamicExpMax.end(); it != theEnd; ++it) {
      m_stream << it->first;
      // iterates over a vector
      for( auto it2 = it->second.cbegin(), theEnd2 = it->second.cend(); it2 != theEnd2; ++it2) {
        m_stream << " " << *it2;
      }
      m_stream << "\n";
    }

    m_stream.close();
  }

  template< typename GUM_SCALAR >
  std::string CNInferenceEngine< GUM_SCALAR >::toString() const {
    std::stringstream output;
    output << "\n";

    // use cbegin() when available
    for ( auto it = _marginalMin.begin(), theEnd = _marginalMin.end(); it != theEnd; ++it ) {
      auto esize = it->size();
      for ( decltype(esize) mod = 0; mod < esize; mod++ ) {
        output << "P(" << _credalNet->current_bn().variable ( it.key() ).name() << "=" << mod << "|e) = [ ";
        output << _marginalMin[it.key()][mod] << ", " << _marginalMax[it.key()][mod] << " ]";

        if ( ! _query.empty() )
          if ( _query.exists ( it.key() ) && _query[it.key()][mod] )
            output << " QUERY";

        output << "\n";
      }

      output << "\n";
    }

    return output.str();
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::saveVertices ( const std::string &path ) const {
    std::ofstream m_stream ( path.c_str(), std::ios::out | std::ios::trunc );

    if ( ! m_stream.good() ) {
      GUM_ERROR(IOError, "void CNInferenceEngine< GUM_SCALAR >::saveVertices(const std::string & path) : could not open outpul file : " << path);
    }

    // use cbegin() cend() when available
    for ( auto it = _marginalSets.begin(), theEnd = _marginalSets.end(); it != theEnd; ++it ) {
      m_stream << _credalNet->current_bn().variable ( it.key() ).name() << "\n";
      auto esize = _marginalSets[it.key()].size();
      // iterates over vectors from here
      for ( decltype(esize) vertex = 0; vertex < esize; vertex ++ ) {
        m_stream << "[";

        auto dSize = _marginalSets[it.key()][vertex].size();
        for ( decltype(dSize) mod = 0; mod < dSize; mod++ ) {
          if ( mod > 0 )
            m_stream << ",";

          m_stream << _marginalSets[it.key()][vertex][mod];
        }

        m_stream << "]\n";
      }
    }

    m_stream.close();
  }

 

  // since only monitored variables in _modal will be alble to compute expectations, it is useless to initialize those for all variables
  // _modal variables will always be checked further, so it is not necessary to check it here, but doing so will use less memory
  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::_initExpectations() {
    if(_modal.empty())
      return;

    for ( auto id =  _credalNet->current_bn().beginNodes(), theEnd = _credalNet->current_bn().endNodes(); id != theEnd; ++id ) {
      std::string var_name, time_step;

      var_name = _credalNet->current_bn().variable ( *id ).name();
      auto delim = var_name.find_first_of ( "_" );
      time_step = var_name.substr ( delim + 1, var_name.size() );
      var_name = var_name.substr ( 0, delim );

      auto it = _modal.find(var_name);
      if(it == _modal.end())
        continue;

      _expectationMin.insert ( *id, it->second[it->second.size()-1] );
      _expectationMax.insert ( *id, it->second[0] );
    }

  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::dynamicExpectations() {
    _dynamicExpectations();
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::_dynamicExpectations() {
    if(_expectationMin.empty())
      return;

    typedef typename std::map< int, GUM_SCALAR > innerMap;
    //typedef typename std::pair< int, GUM_SCALAR > innerPair;
    typedef typename std::map< std::string, innerMap > outerMap;
    //typedef typename std::pair< std::string, innerMap > outerPair;

    typedef typename std::map< std::string, std::vector< GUM_SCALAR > > mod;

    // si non dynamique, sauver directement _expectationMin et Max (revient au meme mais plus rapide)
    outerMap expectationsMin, expectationsMax;

    // use cbegin() when available
    for ( auto it = _expectationMin.begin(), theEnd = _expectationMin.end(); it != theEnd; ++it ) {
      
      std::string var_name, time_step;

      var_name = _credalNet->current_bn().variable ( it.key() ).name();
      auto delim = var_name.find_first_of ( "_" );
      time_step = var_name.substr ( delim + 1, var_name.size() );
      var_name = var_name.substr ( 0, delim );

      // to be sure (don't store not monitored variables' expectations) although it should be taken care of before this point
      if( _modal.find(var_name) == _modal.end() )
        continue;

      //expectationsMin[var_name].insert(innerPair(atoi(time_step.c_str()), _expectationMin[it.key()]));
      //expectationsMax[var_name].insert(innerPair(atoi(time_step.c_str()), _expectationMax[it.key()]));

      expectationsMin[var_name][atoi(time_step.c_str())] = _expectationMin[it.key()];
      expectationsMax[var_name][atoi(time_step.c_str())] = _expectationMax[it.key()];
    }

    // use cbegin() when available
    for( auto it = expectationsMin.begin(), theEnd = expectationsMin.end(); it != theEnd; ++it) {
      typename std::vector< GUM_SCALAR > dynExp(it->second.size());

      for( auto it2 = it->second.begin(), theEnd2 = it->second.end(); it2 != theEnd2; ++it2) {
        dynExp[it2->first] = it2->second;
      }

      _dynamicExpMin[it->first] = dynExp;
    }

    // use cbegin() when available
    for( auto it = expectationsMax.begin(), theEnd = expectationsMax.end(); it != theEnd; ++it) {
      typename std::vector< GUM_SCALAR > dynExp(it->second.size());

      for( auto it2 = it->second.begin(), theEnd2 = it->second.end(); it2 != theEnd2; ++it2) {
        dynExp[it2->first] = it2->second;
      }

      _dynamicExpMax[it->first] = dynExp;
    }
  }

  template< typename GUM_SCALAR >
  void CNInferenceEngine< GUM_SCALAR >::_repetitiveInit() {
    _timeSteps = 0;
    const gum::DAG &dag = _credalNet->current_bn().dag();

    // t = 0 vars belongs to _t0 as keys
    for ( auto id = dag.beginNodes(), theEnd = dag.endNodes(); id != theEnd; ++id ) {
      std::string var_name = _credalNet->current_bn().variable ( *id ).name();
      auto delim = var_name.find_first_of ( "_" );

      if ( delim > var_name.size() ) {
        GUM_ERROR( InvalidArgument, "void CNInferenceEngine< GUM_SCALAR >::_repetitiveInit() : the network does not appear to be dynamic" );
      }

      std::string time_step = var_name.substr ( delim + 1, 1 );

      if ( time_step.compare ( "0" ) == 0 )
        _t0.insert ( *id, std::vector< gum::NodeId >() );
    }

    // t = 1 vars belongs to either _t0 as member value or _t1 as keys
    for ( auto id = dag.beginNodes(), theEnd = dag.endNodes(); id != theEnd; ++id ) {
      std::string var_name = _credalNet->current_bn().variable ( *id ).name();
      auto delim = var_name.find_first_of ( "_" );
      std::string time_step = var_name.substr ( delim + 1, var_name.size() );
      var_name = var_name.substr ( 0, delim );
      delim = time_step.find_first_of ( "_" );
      time_step = time_step.substr ( 0, delim );

      if ( time_step.compare ( "1" ) == 0 ) {
        bool found = false;

        // use cbegin() when available
        for ( auto it = _t0.begin(), t0End = _t0.end(); it != t0End; ++it ) {
          std::string var_0_name =  _credalNet->current_bn().variable ( it.key() ).name();
          delim = var_0_name.find_first_of ( "_" );
          var_0_name = var_0_name.substr ( 0, delim );

          if ( var_name.compare ( var_0_name ) == 0 ) {
            const gum::Potential< GUM_SCALAR > * potential ( &_credalNet->current_bn().cpt ( *id ) );
            const gum::Potential< GUM_SCALAR > * potential2 ( &_credalNet->current_bn().cpt ( it.key() ) );

            if ( potential->domainSize() == potential2->domainSize() )
              _t0[it.key()].push_back ( *id );
            else
              _t1.insert ( *id, std::vector< gum::NodeId >() );

            found = true;
            break;
          }
        }

        if ( ! found ) {
          _t1.insert ( *id, std::vector< gum::NodeId >() );
        }
      }
    }

    // t > 1 vars belongs to either _t0 or _t1 as member value
    // remember _timeSteps
    for ( auto id = dag.beginNodes(), theEnd = dag.endNodes(); id != theEnd; ++id ) {
      std::string var_name = _credalNet->current_bn().variable ( *id ).name();
      auto delim = var_name.find_first_of ( "_" );
      std::string time_step = var_name.substr ( delim + 1, var_name.size() );
      var_name = var_name.substr ( 0, delim );
      delim = time_step.find_first_of ( "_" );
      time_step = time_step.substr ( 0, delim );



      if ( time_step.compare ( "0" ) != 0 && time_step.compare ( "1" ) != 0 ) {
        // keep max time_step
        if(atoi(time_step.c_str()) > _timeSteps)
          _timeSteps = atoi(time_step.c_str());

        std::string var_0_name;
        bool found = false;

        // use cbegin()
        for ( auto it = _t0.begin(), t0End = _t0.end(); it != t0End; ++it ) {
          std::string var_0_name =  _credalNet->current_bn().variable ( it.key() ).name();
          delim = var_0_name.find_first_of ( "_" );
          var_0_name = var_0_name.substr ( 0, delim );

          if ( var_name.compare ( var_0_name ) == 0 ) {
            const gum::Potential< GUM_SCALAR > * potential ( &_credalNet->current_bn().cpt ( *id ) );
            const gum::Potential< GUM_SCALAR > * potential2 ( &_credalNet->current_bn().cpt ( it.key() ) );

            if ( potential->domainSize() == potential2->domainSize() ) {
              _t0[it.key()].push_back ( *id );
              found = true;
              break;
            }
          }
        }

        if ( !found ) {
          // use cbegin()
          for ( auto it = _t1.begin(), t1End = _t1.end(); it != t1End; ++it ) {
            std::string var_0_name =  _credalNet->current_bn().variable ( it.key() ).name();
            auto delim = var_0_name.find_first_of ( "_" );
            var_0_name = var_0_name.substr ( 0, delim );

            if ( var_name.compare ( var_0_name ) == 0 ) {
              const gum::Potential< GUM_SCALAR > * potential ( &_credalNet->current_bn().cpt ( *id ) );
              const gum::Potential< GUM_SCALAR > * potential2 ( &_credalNet->current_bn().cpt ( it.key() ) );

              if ( potential->domainSize() == potential2->domainSize() ) {
                _t1[it.key()].push_back ( *id );
                break;
              }
            }
          }
        }
      }
    }

    //std::cout << "clusters" << std::endl;
    //std::cout << "_t0 : " << std::endl << _t0 << std::endl;
    //std::cout << "_t1 : " << std::endl << _t1 << std::endl << std::endl;
  }

  template< typename GUM_SCALAR >
  const typename gum::Property< std::vector< gum::NodeId > >::onNodes & CNInferenceEngine< GUM_SCALAR >::getT0Cluster() const {
    return _t0;
  }

  template< typename GUM_SCALAR >
  const typename gum::Property< std::vector< gum::NodeId > >::onNodes & CNInferenceEngine< GUM_SCALAR >::getT1Cluster() const {
    return _t1;
  }

  template < typename GUM_SCALAR >
  inline const GUM_SCALAR CNInferenceEngine< GUM_SCALAR >::_computeEpsilon() {
    GUM_SCALAR eps = 0;
    GUM_SCALAR delta;

    for ( Size i = 0; i < _marginalMin.size(); i++ ) {
      auto dSize = _marginalMin[i].size();
      for ( decltype(dSize) j = 0; j < dSize; j++ ) {
        // on min
        delta = _marginalMin[i][j] - _oldMarginalMin[i][j];
        delta = ( delta < 0 ) ? ( - delta ) : delta;
        eps = ( eps < delta ) ? delta : eps;

        // on max
        delta = _marginalMax[i][j] - _oldMarginalMax[i][j];
        delta = ( delta < 0 ) ? ( - delta ) : delta;
        eps = ( eps < delta ) ? delta : eps;

        _oldMarginalMin[i][j] = _marginalMin[i][j];
        _oldMarginalMax[i][j] = _marginalMax[i][j];
      }
    } // end of : all variables

    return eps;
  }



  ///////////////////////// 2 argument template ////////////////////////
 


  template < typename GUM_SCALAR, class BNInferenceEngine >
  CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::CNInferenceEngine ( const CredalNet< GUM_SCALAR > & credalNet ) {
    this->_credalNet = &credalNet;

    for ( auto id = this->_credalNet->current_bn().beginNodes(), theEnd = this->_credalNet->current_bn().endNodes(); id != theEnd; ++id ) {
      auto dSize = this->_credalNet->current_bn().variable ( *id ).domainSize();
      this->_marginalMin.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 1 ) );
      this->_marginalMax.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 0 ) );
      this->_marginalSets.insert ( *id, std::vector< std::vector< GUM_SCALAR > >() );
    }
    GUM_CONSTRUCTOR ( CNInferenceEngine );
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::~CNInferenceEngine() {
    GUM_DESTRUCTOR ( CNInferenceEngine );
  }


  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline void CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::_initThreadsData( const unsigned int & num_threads, const bool __storeVertices, const bool __storeBNOpt ) {
    _workingSet.clear();
    _workingSet.resize( num_threads, NULL );
    _workingSetE.clear();
    _workingSetE.resize( num_threads, NULL );

    _l_marginalMin.clear();
    _l_marginalMin.resize( num_threads );
    _l_marginalMax.clear();
    _l_marginalMax.resize( num_threads );
    _l_expectationMin.clear();
    _l_expectationMin.resize( num_threads );
    _l_expectationMax.clear();
    _l_expectationMax.resize( num_threads );

    if ( __storeVertices ) {
      _l_marginalSets.clear();
      _l_marginalSets.resize( num_threads );
    }

    if ( __storeBNOpt ) {
      for ( Size ptr = 0; ptr < this->_l_optimalNet.size(); ptr++ )
        if ( this->_l_optimalNet[ptr] != NULL )
          delete _l_optimalNet[ptr];

      _l_optimalNet.clear();
      _l_optimalNet.resize( num_threads );
    }

    _l_modal.clear();
    _l_modal.resize( num_threads );

    this->_oldMarginalMin.clear();
    this->_oldMarginalMin = this->_marginalMin;
    this->_oldMarginalMax.clear();
    this->_oldMarginalMax = this->_marginalMax;
  }
 
  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline bool CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::_updateThread( const gum::NodeId & id, const std::vector< GUM_SCALAR > & vertex, const bool __storeVertices, const bool __storeBNOpt ) {
    int tId = gum_threads::getThreadNumber();
    // save E(X)
    if( ! _l_modal[tId].empty() ) {
      std::string var_name = _workingSet[tId]->variable ( id ).name();
      auto delim = var_name.find_first_of ( "_" );
      std::string time_step = var_name.substr ( delim + 1, var_name.size() );
      var_name = var_name.substr ( 0, delim );

      if( _l_modal[tId].find(var_name) != _l_modal[tId].end() ) {
        GUM_SCALAR exp = 0;
        auto vsize = vertex.size();
        for ( decltype(vsize) mod = 0; mod < vsize; mod++ )
          exp += vertex[mod] * _l_modal[tId][var_name][mod];

        if( exp > _l_expectationMax[tId][id] )
          _l_expectationMax[tId][id] = exp;
        if( exp < _l_expectationMin[tId][id] )
          _l_expectationMin[tId][id] = exp;
      }
    } // end of : if modal (map) not empty

    bool newOne = false;
    bool added = false;
    bool result = false;
    // for burn in, we need to keep checking on local marginals and not global ones (faster inference)
    auto vsize = vertex.size();
    for( decltype(vsize) mod = 0; mod < vsize; mod++ ) {
      if(vertex[mod] < _l_marginalMin[tId][id][mod]) {
        _l_marginalMin[tId][id][mod] = vertex[mod];
        newOne = true;
        if ( __storeBNOpt ) {
          std::vector< unsigned int > key(3);
          key[0] = id; key[1] = mod; key[2] = 0;
          if ( _l_optimalNet[tId]->insert( key, true ) )
            result = true;
        }
      }
      if(vertex[mod] > _l_marginalMax[tId][id][mod]) {
        _l_marginalMax[tId][id][mod] = vertex[mod];
        newOne = true;
        if ( __storeBNOpt ) {
          std::vector< unsigned int > key(3);
          key[0] = id; key[1] = mod; key[2] = 1;
          if ( _l_optimalNet[tId]->insert( key, true ) )
            result = true;
        }
      }
      else if ( vertex[mod] == _l_marginalMin[tId][id][mod] 
              || vertex[mod] == _l_marginalMax[tId][id][mod] ) {
        newOne = true;
        if ( __storeBNOpt && vertex[mod] == _l_marginalMin[tId][id][mod] ) {
          std::vector< unsigned int > key(3);
          key[0] = id; key[1] = mod; key[2] = 0;
          if ( _l_optimalNet[tId]->insert( key, false ) )
            result = true;
        }
        if ( __storeBNOpt && vertex[mod] == _l_marginalMax[tId][id][mod] ) {
          std::vector< unsigned int > key(3);
          key[0] = id; key[1] = mod; key[2] = 1;
          if ( _l_optimalNet[tId]->insert(  key, false ) )
            result = true;
        }
      }
      // store point to compute credal set vertices.
      // check for redundancy at each step or at the end ?
      if( __storeVertices && ! added && newOne ) {
        _l_marginalSets[tId][id].push_back(vertex);
        added = true;
      }
    }

    // if all variables didn't get better marginals, we will delete
    // this->_l_optimalNet[tId]->getCurrentSample() ( the caller will )
    if ( __storeBNOpt && result )
      return true;

    return false;

  }
  
  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline void CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::_updateMarginals() {
    #pragma omp parallel
    {
      int threadId = gum_threads::getThreadNumber();
      auto nsize = _workingSet[threadId]->size();
      #pragma omp for
      for ( decltype(nsize) i = 0; i < nsize; i++ ) {
        auto dSize = _l_marginalMin[threadId][i].size();
        for ( decltype(dSize) j = 0; j < dSize; j++ ) {
          auto tsize = _l_marginalMin.size();
          // go through all threads
          for ( decltype(tsize) tId = 0; tId < tsize; tId++ ) {
            if ( _l_marginalMin[tId][i][j] < this->_marginalMin[i][j] )
              this->_marginalMin[i][j] = _l_marginalMin[tId][i][j];
            if ( _l_marginalMax[tId][i][j] > this->_marginalMax[i][j] )
              this->_marginalMax[i][j] = _l_marginalMax[tId][i][j];
          } // end of : all threads
          
          /*
          // update them
          for ( Size tId = 0; tId < this->_l_marginalMin.size(); tId++ ) {
            //if ( this->_l_marginalMin[tId][i][j] > this->_marginalMin[i][j] )
              this->_l_marginalMin[tId][i][j] = this->_marginalMin[i][j];
            //if ( this->_l_marginalMax[tId][i][j] < this->_oldMarginalMax[i][j] )
              this->_l_marginalMax[tId][i][j] = this->_marginalMax[i][j];
          } // end of : all threads
          */
          
        } // end of : all modalities
      } // end of : all variables
    } // end of : parallel region

  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline const GUM_SCALAR CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::_computeEpsilon() {
    GUM_SCALAR eps = 0;
    #pragma omp parallel
    {
      GUM_SCALAR tEps = 0;
      GUM_SCALAR delta;

      int tId = gum_threads::getThreadNumber();
      auto nsize = _workingSet[tId]->size();

      #pragma omp for
      for ( decltype(nsize) i = 0; i < nsize; i++ ) {
        auto dSize = _l_marginalMin[tId][i].size();
        for ( decltype(dSize) j = 0; j < dSize; j++ ) {
          // on min
          delta = this->_marginalMin[i][j] - this->_oldMarginalMin[i][j];
          delta = ( delta < 0 ) ? ( - delta ) : delta;
          tEps = ( tEps < delta ) ? delta : tEps;

          // on max
          delta = this->_marginalMax[i][j] - this->_oldMarginalMax[i][j];
          delta = ( delta < 0 ) ? ( - delta ) : delta;
          tEps = ( tEps < delta ) ? delta : tEps;

          this->_oldMarginalMin[i][j] = this->_marginalMin[i][j];
          this->_oldMarginalMax[i][j] = this->_marginalMax[i][j];
        }
      } // end of : all variables

      #pragma omp critical(epsilon_max)
      {
        #pragma omp flush(eps)
        eps = ( eps < tEps ) ? tEps : eps;
      }

    } // end of : parallel region
    return eps;
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::_updateOldMarginals() {
    #pragma omp parallel
    {
      int threadId = gum_threads::getThreadNumber();
      auto nsize = _workingSet[threadId]->size();
      
      #pragma omp for
      for ( decltype(nsize) i = 0; i < nsize; i++ ) {
        auto dSize = _l_marginalMin[threadId][i].size();
        for ( decltype(dSize) j = 0; j < dSize; j++ ) {
          auto tsize = _l_marginalMin.size();
          // go through all threads
          for ( decltype(tsize) tId = 0; tId < tsize; tId++ ) {
            if ( _l_marginalMin[tId][i][j] < this->_oldMarginalMin[i][j] )
              this->_oldMarginalMin[i][j] = _l_marginalMin[tId][i][j];
            if ( _l_marginalMax[tId][i][j] > this->_oldMarginalMax[i][j] )
              this->_oldMarginalMax[i][j] = _l_marginalMax[tId][i][j];
          } // end of : all threads
          /*
          // update them
          for ( Size tId = 0; tId < this->_l_marginalMin.size(); tId++ ) {
            //if ( this->_l_marginalMin[tId][i][j] > this->_oldMarginalMin[i][j] )
              this->_l_marginalMin[tId][i][j] = this->_oldMarginalMin[i][j];
            //if ( this->_l_marginalMax[tId][i][j] < this->_oldMarginalMax[i][j] )
              this->_l_marginalMax[tId][i][j] = this->_oldMarginalMax[i][j];
          } // end of : all threads
          */
        } // end of : all modalities
      } // end of : all variables
    } // end of : parallel region

  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::_verticesFusion() {
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::_expFusion() {
    // don't create threads if there are no modalities to compute expectations
    if ( this->_modal.empty() )
      return;

    #pragma omp parallel
    {
      int threadId = gum_threads::getThreadNumber();

      if ( ! this->_l_modal[threadId].empty() ) {
        auto nsize = _workingSet[threadId]->size();

        #pragma omp for
        for ( decltype(nsize) i = 0; i < nsize; i++ ) {
          std::string var_name = _workingSet[threadId]->variable ( i ).name();
          auto delim = var_name.find_first_of ( "_" );
          std::string time_step = var_name.substr ( delim + 1, var_name.size() );
          var_name = var_name.substr ( 0, delim );

          if ( _l_modal[threadId].find(var_name) == _l_modal[threadId].end() )
            continue;

          auto tsize = _l_expectationMax.size();
          for ( decltype(tsize) tId = 0; tId < tsize; tId++ ) {
            if( _l_expectationMax[tId][i] > this->_expectationMax[i] )
              this->_expectationMax[i] = _l_expectationMax[tId][i];
            if( _l_expectationMin[tId][i] < this->_expectationMin[i] )
              this->_expectationMin[i] = _l_expectationMin[tId][i];
          } // end of : each thread
        } // end of : each variable
      } // end of : if modals not empty

    } // end of : parallel region
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::_optFusion() {
    typedef std::vector< bool > dBN;

    std::cout << "thread opt fusion : " << _l_marginalMin.size() << std::endl;

    _threadFusion = OptBN<GUM_SCALAR>( *this->_credalNet );

    auto nsize = _workingSet[0]->size();

    // no parallel insert in hash-tables (OptBN)
    for ( decltype(nsize) i = 0; i < nsize; i++ ) {
      std::cout << _workingSet[0]->variable(i).name() << std::endl;
      auto dSize = _l_marginalMin[0][i].size();
      for ( decltype(dSize) j = 0; j < dSize; j++ ) {
        std::cout << "\t mod : " << j << std::endl;
        // go through all threads
        std::vector< unsigned int > keymin(3);
        keymin[0] = i; keymin[1] = j; keymin[2] = 0;
        std::vector< unsigned int > keymax(keymin);
        keymax[2] = 1;

        auto tsize = _l_marginalMin.size();
        for ( decltype(tsize) tId = 0; tId < tsize; tId++ ) {
          if ( _l_marginalMin[tId][i][j] == this->_marginalMin[i][j] ) {
            std::cout << "\t\t min : ";
            const std::vector< dBN* > & tOpts = _l_optimalNet[tId]->getBNOptsFromKey( keymin );
            std::cout << " size : " << tOpts.size() << std::endl;
            auto osize = tOpts.size();
            for ( decltype(osize) bn = 0; bn < osize; bn++ ) {
              _threadFusion.insert ( *tOpts[bn], keymin );
            }
          }
          if ( _l_marginalMax[tId][i][j] == this->_marginalMax[i][j] ) {
            std::cout << "\t\t max : ";
            const std::vector< dBN* > & tOpts = _l_optimalNet[tId]->getBNOptsFromKey( keymax );
            std::cout << " size : " << tOpts.size() << std::endl;
            auto osize = tOpts.size();
            for ( decltype(osize) bn = 0; bn < osize; bn++ ) {
              _threadFusion.insert ( *tOpts[bn], keymax );
            }
          }
        } // end of : all threads
      } // end of : all modalities
    } // end of : all variables

    std::cout << "fusion size : " << _threadFusion.getEntrySize() << std::endl;
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  OptBN<GUM_SCALAR> * CNInferenceEngine< GUM_SCALAR, BNInferenceEngine >::getOptBN () {
    return &_threadFusion;
  }




}
