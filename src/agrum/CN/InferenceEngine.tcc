#include <agrum/core/exceptions.h>
#include <map>
#include <agrum/CN/InferenceEngine.h>

namespace gum {

  template< typename GUM_SCALAR >
  InferenceEngine< GUM_SCALAR >::InferenceEngine ( const CredalNet< GUM_SCALAR > & credalNet ) : ApproximationScheme() {
    GUM_CONSTRUCTOR ( InferenceEngine );
    _credalNet = &credalNet;

    for ( gum::DAG::NodeIterator id = _credalNet->current_bn().beginNodes(); id != _credalNet->current_bn().endNodes(); ++id ) {
      int dSize = _credalNet->current_bn().variable ( *id ).domainSize();
      _marginalMin.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 1 ) );
      _marginalMax.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 0 ) );
      _marginalSets.insert ( *id, std::vector< std::vector< GUM_SCALAR > >() );
    }
  }

  template< typename GUM_SCALAR >
  InferenceEngine< GUM_SCALAR >::~InferenceEngine() {
    GUM_DESTRUCTOR ( InferenceEngine );
  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::eraseAllEvidence() {
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

    for ( gum::DAG::NodeIterator id = _credalNet->current_bn().beginNodes(); id != _credalNet->current_bn().endNodes(); ++id ) {
      int dSize = _credalNet->current_bn().variable ( *id ).domainSize();
      _marginalMin.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 1 ) );
      _marginalMax.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 0 ) );
      _marginalSets.insert ( *id, std::vector< std::vector< GUM_SCALAR > >() );
    }
  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::insertModals( const std::string &path) {
    std::ifstream mod_stream ( path.c_str(), std::ios::in );

    if ( !mod_stream.good() ) {
      GUM_ERROR(OperationNotAllowed, "void InferenceEngine< GUM_SCALAR >::insertModals(const std::string & path) : could not open input file : " << path );
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
  void InferenceEngine< GUM_SCALAR >::insertModals( const std::map< std::string, std::vector< GUM_SCALAR > > &modals) {
    if( ! _modal.empty() )
      _modal.clear();

    _modal = modals;

    _initExpectations();
  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::insertEvidence ( const std::map< std::string, std::vector< GUM_SCALAR > > & eviMap ) {
    if(! _evidence.empty() )
      _evidence.clear();

    typedef std::map< std::string, std::vector< GUM_SCALAR > > evidenceMap;

    for(typename evidenceMap::const_iterator it = eviMap.begin(); it != eviMap.end(); ++it) {
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
  void InferenceEngine< GUM_SCALAR >::insertEvidence ( const typename gum::Property< std::vector < GUM_SCALAR > >::onNodes &evidence ) {
    if(! _evidence.empty() )
      _evidence.clear();

    for ( typename gum::Property< std::vector < GUM_SCALAR > >::onNodes::const_iterator it = evidence.begin(); it != evidence.end(); ++it ) {
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
  void InferenceEngine< GUM_SCALAR >::insertEvidence ( const std::string &path ) {
    std::ifstream evi_stream ( path.c_str(), std::ios::in );

    if ( !evi_stream.good() ) {
      GUM_ERROR(IOError, "void InferenceEngine< GUM_SCALAR >::insertEvidence(const std::string & path) : could not open input file : " << path );
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

      //gum::NodeId node = _credalNet->current_bn().idFromName ( tmp );

      std::vector< GUM_SCALAR > values;
      p = strtok ( NULL, " " );

      while ( p != NULL ) {
        //tmp = p;
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
  void InferenceEngine< GUM_SCALAR >::insertQuery ( const typename gum::Property< std::vector < bool > >::onNodes &query ) {
    if(! _query.empty() )
      _query.clear();

    for ( typename gum::Property< std::vector < bool > >::onNodes::const_iterator it = query.begin(); it != query.end(); ++it ) {
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
  void InferenceEngine< GUM_SCALAR >::insertQuery ( const std::string &path ) {
    std::ifstream evi_stream ( path.c_str(), std::ios::in );

    if ( !evi_stream.good() ) {
      GUM_ERROR( IOError, "void InferenceEngine< GUM_SCALAR >::insertQuery(const std::string & path) : could not open input file : " << path );
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
      
      //gum::NodeId node = _credalNet->current_bn().idFromName ( tmp );

      int dSize = _credalNet->current_bn().variable ( node ).domainSize();

      p = strtok ( NULL, " " );

      if ( p == NULL ) {
        _query.insert ( node, std::vector< bool> ( dSize, true ) );
      } else {
        std::vector< bool > values ( dSize, false );

        while ( p != NULL ) {
          //tmp = p;
          if ( atoi ( p ) >= dSize )
            GUM_ERROR(OutOfBounds, "void InferenceEngine< GUM_SCALAR >::insertQuery(const std::string & path) : query modality is higher or equal to cardinality");

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
  const std::vector< GUM_SCALAR > & InferenceEngine< GUM_SCALAR >::marginalMin( const std::string & varName ) const {
    try {
      return _marginalMin[_credalNet->current_bn().idFromName(varName)];
    } catch(gum::NotFound & err) {
      throw(err);
    }
  }

  template< typename GUM_SCALAR > 
  const std::vector< GUM_SCALAR > & InferenceEngine< GUM_SCALAR >::marginalMax( const std::string & varName ) const {
    try {
      return _marginalMax[_credalNet->current_bn().idFromName(varName)];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const std::vector< GUM_SCALAR > & InferenceEngine< GUM_SCALAR >::marginalMin ( const gum::NodeId id ) const {
    try {
      return _marginalMin[id];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const std::vector< GUM_SCALAR > & InferenceEngine< GUM_SCALAR >::marginalMax ( const gum::NodeId id ) const {
    try {
      return _marginalMax[id];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }

  }

  template< typename GUM_SCALAR >
  const GUM_SCALAR & InferenceEngine< GUM_SCALAR >::expectationMin ( const std::string & varName ) const {
    try {
      return _expectationMin[_credalNet->current_bn().idFromName(varName)];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const GUM_SCALAR & InferenceEngine< GUM_SCALAR >::expectationMax ( const std::string & varName ) const {
    try {
      return _expectationMax[_credalNet->current_bn().idFromName(varName)];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const GUM_SCALAR & InferenceEngine< GUM_SCALAR >::expectationMin( const gum::NodeId id ) const {
    try {
      return _expectationMin[id];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const GUM_SCALAR & InferenceEngine< GUM_SCALAR >::expectationMax( const gum::NodeId id ) const {
    try {
      return _expectationMax[id];
    } catch ( gum::NotFound & err ) {
      throw ( err );
    }
  }

  template< typename GUM_SCALAR >
  const std::vector< GUM_SCALAR > & InferenceEngine< GUM_SCALAR >::dynamicExpMin ( const std::string & varName ) const {
    std::string errTxt = "const std::vector< GUM_SCALAR > & InferenceEngine< GUM_SCALAR >::dynamicExpMin ( const std::string & varName ) const : ";

    if( _dynamicExpMin.empty() )
      GUM_ERROR(OperationNotAllowed, errTxt + "_dynamicExpectations() needs to be called before");

    if(_dynamicExpMin.find(varName) == _dynamicExpMin.end())
      GUM_ERROR(NotFound, errTxt + "variable name not found : " << varName);

    return _dynamicExpMin.find(varName)->second;//[varName];
  }

  template< typename GUM_SCALAR >
  const std::vector< GUM_SCALAR > & InferenceEngine< GUM_SCALAR >::dynamicExpMax ( const std::string & varName ) const {
    std::string errTxt = "const std::vector< GUM_SCALAR > & InferenceEngine< GUM_SCALAR >::dynamicExpMax ( const std::string & varName ) const : ";

    if( _dynamicExpMax.empty() ) 
       GUM_ERROR(OperationNotAllowed, errTxt + "_dynamicExpectations() needs to be called before");
    
    if(_dynamicExpMin.find(varName) == _dynamicExpMin.end())
      GUM_ERROR(NotFound, errTxt + "variable name not found : " << varName);

    return _dynamicExpMax.find(varName)->second;//[varName];
  }


  template< typename GUM_SCALAR >
  const std::vector< std::vector< GUM_SCALAR > > & InferenceEngine< GUM_SCALAR >::vertices ( const gum::NodeId id ) const {
    return _marginalSets[id];
  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::saveMarginals ( const std::string &path ) const {
    std::ofstream m_stream ( path.c_str(), std::ios::out | std::ios::trunc );

    if ( ! m_stream.good() ) {
      GUM_ERROR(IOError, "void InferenceEngine< GUM_SCALAR >::saveMarginals(const std::string & path) const : could not open output file : " << path);
    }

    for ( typename gum::Property< std::vector< GUM_SCALAR > >::onNodes::const_iterator it = _marginalMin.begin(); it != _marginalMin.end(); ++it ) {
      for ( int mod = 0; mod < it->size(); mod++ ) {
        m_stream << _credalNet->current_bn().variable ( it.key() ).name() << " " << mod << " " << ( *it ) [mod] << " " << _marginalMax[it.key()][mod] << std::endl;
      }
    }

    m_stream.close();
  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::saveExpectations (const std::string &path ) const {
    if(_dynamicExpMin.empty())//_modal.empty())
      return;
    // else not here, to keep the const (natural with a saving process)
    //else if(_dynamicExpMin.empty() || _dynamicExpMax.empty())
      //_dynamicExpectations(); // works with or without a dynamic network
    

    std::ofstream m_stream ( path.c_str(), std::ios::out | std::ios::trunc );

    if ( ! m_stream.good() ) {
      GUM_ERROR ( IOError, "void InferenceEngine< GUM_SCALAR >::saveExpectations(const std::string & path) : could not open output file : " << path );
    }

    typedef typename std::map< std::string, std::vector< GUM_SCALAR > > myType;

    for(typename myType::const_iterator it = _dynamicExpMin.begin(); it != _dynamicExpMin.end(); ++it) {
      m_stream << it->first;
      for(typename std::vector< GUM_SCALAR >::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
        m_stream << " " << *it2;
      }
      m_stream << "\n";
    }

    for(typename myType::const_iterator it = _dynamicExpMax.begin(); it != _dynamicExpMax.end(); ++it) {
      m_stream << it->first;
      for(typename std::vector< GUM_SCALAR >::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
        m_stream << " " << *it2;
      }
      m_stream << "\n";
    }

    m_stream.close();
  }

  template< typename GUM_SCALAR >
  std::string InferenceEngine< GUM_SCALAR >::toString() const {
    std::stringstream output;
    output << "\n";

    for ( typename gum::Property< std::vector< GUM_SCALAR > >::onNodes::const_iterator it = this->_marginalMin.begin(); it != this->_marginalMin.end(); ++it ) {
      for ( int mod = 0; mod < it->size(); mod++ ) {
        output << "P(" << this->_credalNet->current_bn().variable ( it.key() ).name() << "=" << mod << "|e) = [ ";
        output << this->_marginalMin[it.key()][mod] << ", " << this->_marginalMax[it.key()][mod] << " ]";

        if ( ! this->_query.empty() )
          if ( this->_query.exists ( it.key() ) && this->_query[it.key()][mod] )
            output << " QUERY";

        output << "\n";
      }

      output << "\n";
    }

    return output.str();
  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::saveVertices ( const std::string &path ) const {
    std::ofstream m_stream ( path.c_str(), std::ios::out | std::ios::trunc );

    if ( ! m_stream.good() ) {
      GUM_ERROR(IOError, "void InferenceEngine< GUM_SCALAR >::saveVertices(const std::string & path) : could not open outpul file : " << path);
    }

    for ( typename gum::Property< std::vector< std::vector< GUM_SCALAR > > >::onNodes::const_iterator it = _marginalSets.begin(); it != _marginalSets.end(); ++it ) {
      m_stream << _credalNet->current_bn().variable ( it.key() ).name() << "\n";

      for ( int vertex = 0; vertex < _marginalSets[it.key()].size(); vertex ++ ) {
        m_stream << "[";

        for ( int mod = 0; mod < _marginalSets[it.key()][vertex].size(); mod++ ) {
          if ( mod > 0 )
            m_stream << ",";

          m_stream << _marginalSets[it.key()][vertex][mod];
        }

        m_stream << "]\n";
      }
    }

    m_stream.close();
  }


  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::_initThreadsData( const unsigned int & num_threads, const bool __storeVertices ) {
    this->_workingSet.clear();
    this->_workingSet.resize( num_threads );
    this->_workingSetE.clear();
    this->_workingSetE.resize( num_threads );

    this->_l_marginalMin.resize( num_threads );
    this->_l_marginalMax.resize( num_threads );
    this->_l_expectationMin.resize( num_threads );
    this->_l_expectationMax.resize( num_threads );

    if ( __storeVertices )
      this->_l_marginalSets.resize( num_threads );

    this->_l_modal.resize( num_threads );

    this->_oldMarginalMin = this->_marginalMin;
    this->_oldMarginalMax = this->_marginalMax;
  }

  template< typename GUM_SCALAR >
  inline void InferenceEngine< GUM_SCALAR >::_updateThread( const gum::NodeId & id, const std::vector< GUM_SCALAR > & vertex, const bool __storeVertices ) {
    int this_thread = omp_get_thread_num();
    // save E(X)
    if( ! this->_l_modal[this_thread].empty() ) {
      std::string var_name, time_step;
      size_t delim;

      var_name = this->_workingSet[this_thread]->variable ( id ).name();
      delim = var_name.find_first_of ( "_" );
      time_step = var_name.substr ( delim + 1, var_name.size() );
      var_name = var_name.substr ( 0, delim );

      if( this->_l_modal[this_thread].find(var_name) != this->_l_modal[this_thread].end() ) {
        GUM_SCALAR exp = 0;
        for ( Size mod = 0; mod < vertex.size(); mod++ )
          exp += vertex[mod] * this->_l_modal[this_thread][var_name][mod];

        if( exp > this->_l_expectationMax[this_thread][id] )
          this->_l_expectationMax[this_thread][id] = exp;
        if( exp < this->_l_expectationMin[this_thread][id] )
          this->_l_expectationMin[this_thread][id] = exp;
      }
    } // end of : if modal (map) not empty

    bool newOne = false;
    bool added = false;
    // for burn in, we need to keep checking on local marginals and not global ones (faster inference)
    for( Size mod = 0; mod < vertex.size(); mod++ ) {
      if(vertex[mod] <= this->_l_marginalMin[this_thread][id][mod]) {
        this->_l_marginalMin[this_thread][id][mod] = vertex[mod];
        newOne = true;
      }
      if(vertex[mod] >= this->_l_marginalMax[this_thread][id][mod]) {
        this->_l_marginalMax[this_thread][id][mod] = vertex[mod];
        newOne = true;
      }
      // store point to compute credal set vertices.
      // check for redundancy at each step or at the end ?
      if( __storeVertices && ! added && newOne ) {
        this->_l_marginalSets[this_thread][id].push_back(vertex);
        added = true;
      }
    }

  }


  // since only monitored variables in _modal will be alble to compute expectations, it is useless to initialize those for all variables
  // _modal variables will always be checked further, so it is not necessary to check it here, but doing so will use less memory
  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::_initExpectations() {
    if(_modal.empty())
      return;

    for ( gum::DAG::NodeIterator id =  _credalNet->current_bn().beginNodes(); id !=  _credalNet->current_bn().endNodes(); ++id ) {
      std::string var_name, time_step;
      size_t delim;

      var_name = _credalNet->current_bn().variable ( *id ).name();
      delim = var_name.find_first_of ( "_" );
      time_step = var_name.substr ( delim + 1, var_name.size() );
      var_name = var_name.substr ( 0, delim );

      typename std::map< std::string, std::vector< GUM_SCALAR > >::const_iterator it = _modal.find(var_name);
      if(it == _modal.end())
        continue;

      _expectationMin.insert ( *id, it->second[it->second.size()-1] ); // should be max modal
      _expectationMax.insert ( *id, it->second[0] ); // should be min modal
    }

  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::dynamicExpectations() {
    _dynamicExpectations();
  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::_dynamicExpectations() {
    if(_expectationMin.empty())//_modal.empty())
      return;

    typedef typename std::map< int, GUM_SCALAR > innerMap;
    //typedef typename std::pair< int, GUM_SCALAR > innerPair;
    typedef typename std::map< std::string, innerMap > outerMap;
    //typedef typename std::pair< std::string, innerMap > outerPair;

    typedef typename std::map< std::string, std::vector< GUM_SCALAR > > mod;

    // si non dynamique, sauver directement _expectationMin et Max (revient au meme mais plus rapide)
    outerMap expectationsMin, expectationsMax;


/*
    for(typename mod::const_iterator it = _modal.begin(); it != _modal.end(); ++it) {
      // we should check that input modalities and variables DO exist in the network (don't store stuff with empty lines if user input is wrong)
      try {
         _credalNet->current_bn().idFromName ( it->first );
      } catch ( gum::NotFound &err ) {
        GUM_SHOWERROR ( err );
        continue;
      }
      //expectationsMin.insert(outerPair(it->first, innerMap()));
      //expectationsMax.insert(outerPair(it->first, innerMap()));
      expectationsMin[it->first];
      expectationsMax[it->first];
    }*/

    for ( typename gum::Property< GUM_SCALAR >::onNodes::const_iterator it = _expectationMin.begin(); it != _expectationMin.end(); ++it ) {
      
      std::string var_name, time_step;
      size_t delim;

      var_name = _credalNet->current_bn().variable ( it.key() ).name();
      delim = var_name.find_first_of ( "_" );
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

    for(typename outerMap::const_iterator it = expectationsMin.begin(); it != expectationsMin.end(); ++it) {
      //std::cout << it->first << std::endl;
      typename std::vector< GUM_SCALAR > dynExp(it->second.size());
      for(typename innerMap::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
        dynExp[it2->first] = it2->second;
        //std::cout << it2->first << " => " << it2->second << std::endl;
      }
      _dynamicExpMin[it->first] = dynExp;
    }
    for(typename outerMap::const_iterator it = expectationsMax.begin(); it != expectationsMax.end(); ++it) {
      //std::cout << it->first << std::endl;
      typename std::vector< GUM_SCALAR > dynExp(it->second.size());
      for(typename innerMap::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
        dynExp[it2->first] = it2->second;
        //std::cout << it2->first << " => " << it2->second << std::endl;
      }
      _dynamicExpMax[it->first] = dynExp;
    }
  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::_repetitiveInit() {
    _timeSteps = 0;
    std::string var_name, time_step;
    size_t delim;
    const gum::DAG &dag = this->_credalNet->current_bn().dag();

    // t = 0 vars belongs to _t0 as keys
    for ( gum::DAG::NodeIterator id = dag.beginNodes(); id != dag.endNodes(); ++id ) {
      var_name = this->_credalNet->current_bn().variable ( *id ).name();
      delim = var_name.find_first_of ( "_" );

      if ( delim > var_name.size() ) {
        GUM_ERROR( InvalidArgument, "void InferenceEngine< GUM_SCALAR >::_repetitiveInit() : the network does not appear to be dynamic" );
      }

      time_step = var_name.substr ( delim + 1, 1 );

      if ( time_step.compare ( "0" ) == 0 )
        _t0.insert ( *id, std::vector< gum::NodeId >() );
    }

    // t = 1 vars belongs to either _t0 as member value or _t1 as keys
    for ( gum::DAG::NodeIterator id = dag.beginNodes(); id != dag.endNodes(); ++id ) {
      var_name = this->_credalNet->current_bn().variable ( *id ).name();
      delim = var_name.find_first_of ( "_" );
      time_step = var_name.substr ( delim + 1, var_name.size() );
      var_name = var_name.substr ( 0, delim );
      delim = time_step.find_first_of ( "_" );
      time_step = time_step.substr ( 0, delim );

      if ( time_step.compare ( "1" ) == 0 ) {
        std::string var_0_name;
        bool found = false;

        for ( typename gum::Property< std::vector< gum::NodeId > >::onNodes::const_iterator it = _t0.begin(); it != _t0.end(); ++it ) {
          var_0_name =  this->_credalNet->current_bn().variable ( it.key() ).name();
          delim = var_0_name.find_first_of ( "_" );
          var_0_name = var_0_name.substr ( 0, delim );

          if ( var_name.compare ( var_0_name ) == 0 ) {
            const gum::Potential< GUM_SCALAR > * potential ( &this->_credalNet->current_bn().cpt ( *id ) );
            const gum::Potential< GUM_SCALAR > * potential2 ( &this->_credalNet->current_bn().cpt ( it.key() ) );

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
    for ( gum::DAG::NodeIterator id = dag.beginNodes(); id != dag.endNodes(); ++id ) {
      var_name = this->_credalNet->current_bn().variable ( *id ).name();
      delim = var_name.find_first_of ( "_" );
      time_step = var_name.substr ( delim + 1, var_name.size() );
      var_name = var_name.substr ( 0, delim );
      delim = time_step.find_first_of ( "_" );
      time_step = time_step.substr ( 0, delim );



      if ( time_step.compare ( "0" ) != 0 && time_step.compare ( "1" ) != 0 ) {
        // keep max time_step
        if(atoi(time_step.c_str()) > _timeSteps)
          _timeSteps = atoi(time_step.c_str());

        std::string var_0_name;
        bool found = false;

        for ( typename gum::Property< std::vector< gum::NodeId > >::onNodes::const_iterator it = _t0.begin(); it != _t0.end(); ++it ) {
          var_0_name =  this->_credalNet->current_bn().variable ( it.key() ).name();
          delim = var_0_name.find_first_of ( "_" );
          var_0_name = var_0_name.substr ( 0, delim );

          if ( var_name.compare ( var_0_name ) == 0 ) {
            const gum::Potential< GUM_SCALAR > * potential ( &this->_credalNet->current_bn().cpt ( *id ) );
            const gum::Potential< GUM_SCALAR > * potential2 ( &this->_credalNet->current_bn().cpt ( it.key() ) );

            if ( potential->domainSize() == potential2->domainSize() ) {
              _t0[it.key()].push_back ( *id );
              found = true;
              break;
            }
          }
        }

        if ( !found ) {
          for ( typename gum::Property< std::vector< gum::NodeId > >::onNodes::const_iterator it = _t1.begin(); it != _t1.end(); ++it ) {
            var_0_name =  this->_credalNet->current_bn().variable ( it.key() ).name();
            delim = var_0_name.find_first_of ( "_" );
            var_0_name = var_0_name.substr ( 0, delim );

            if ( var_name.compare ( var_0_name ) == 0 ) {
              const gum::Potential< GUM_SCALAR > * potential ( &this->_credalNet->current_bn().cpt ( *id ) );
              const gum::Potential< GUM_SCALAR > * potential2 ( &this->_credalNet->current_bn().cpt ( it.key() ) );

              if ( potential->domainSize() == potential2->domainSize() ) {
                _t1[it.key()].push_back ( *id );
                break;
              }
            }
          }
        }
      }
    }

    std::cout << "clusters" << std::endl;
    std::cout << "_t0 : " << std::endl << _t0 << std::endl;
    std::cout << "_t1 : " << std::endl << _t1 << std::endl << std::endl;
  }

}
