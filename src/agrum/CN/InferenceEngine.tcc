#include <agrum/core/exceptions.h>

#include <agrum/CN/InferenceEngine.h>

namespace gum {

  template< typename GUM_SCALAR >
  InferenceEngine< GUM_SCALAR >::InferenceEngine ( const CredalNet< GUM_SCALAR > & credalNet ) {
    GUM_CONSTRUCTOR ( InferenceEngine );
    _credalNet = &credalNet;

    for ( gum::DAG::NodeIterator id = _credalNet->current_bn().beginNodes(); id != _credalNet->current_bn().endNodes(); ++id ) {
      int dSize = _credalNet->current_bn().variable ( *id ).domainSize();
      _marginalMin.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 1 ) );
      _marginalMax.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 0 ) );
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

    for ( gum::DAG::NodeIterator id = _credalNet->current_bn().beginNodes(); id != _credalNet->current_bn().endNodes(); ++id ) {
      int dSize = _credalNet->current_bn().variable ( *id ).domainSize();
      _marginalMin.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 1 ) );
      _marginalMax.insert ( *id, std::vector< GUM_SCALAR > ( dSize, 0 ) );
    }
  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::insertEvidence ( const typename gum::Property< std::vector < GUM_SCALAR > >::onNodes &evidence ) {
    for ( typename gum::Property< std::vector < GUM_SCALAR > >::onNodes::const_iterator it = evidence.begin(); it != evidence.end(); ++it ) {
      _evidence.insert ( it.key(), *it );
    }
  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::insertEvidence ( const std::string &path ) {
    std::ifstream evi_stream ( path.c_str(), std::ios::in );

    if ( !evi_stream.good() ) {
      GUM_ERROR(OperationNotAllowed, "void InferenceEngine< GUM_SCALAR >::insertEvidence(const std::string & path) : could not open input file : " << path );
    }

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

      gum::NodeId node = _credalNet->current_bn().idFromName ( tmp );

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
    for ( typename gum::Property< std::vector < bool > >::onNodes::const_iterator it = query.begin(); it != query.end(); ++it ) {
      _query.insert ( it.key(), *it );
    }
  }

  template< typename GUM_SCALAR >
  void InferenceEngine< GUM_SCALAR >::insertQuery ( const std::string &path ) {
    std::ifstream evi_stream ( path.c_str(), std::ios::in );

    if ( !evi_stream.good() ) {
      GUM_ERROR( IOError, "void InferenceEngine< GUM_SCALAR >::insertQuery(const std::string & path) : could not open input file : " << path );
    }

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

      gum::NodeId node = _credalNet->current_bn().idFromName ( tmp );
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
  const std::vector< GUM_SCALAR > & InferenceEngine< GUM_SCALAR >::marginalMin ( const gum::NodeId id ) const {
    return _marginalMin[id];
  }

  template< typename GUM_SCALAR >
  const std::vector< GUM_SCALAR > & InferenceEngine< GUM_SCALAR >::marginalMax ( const gum::NodeId id ) const {
    return _marginalMax[id];
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
  void InferenceEngine< GUM_SCALAR >::_repetitiveInit() {
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
    for ( gum::DAG::NodeIterator id = dag.beginNodes(); id != dag.endNodes(); ++id ) {
      var_name = this->_credalNet->current_bn().variable ( *id ).name();
      delim = var_name.find_first_of ( "_" );
      time_step = var_name.substr ( delim + 1, var_name.size() );
      var_name = var_name.substr ( 0, delim );
      delim = time_step.find_first_of ( "_" );
      time_step = time_step.substr ( 0, delim );

      if ( time_step.compare ( "0" ) != 0 && time_step.compare ( "1" ) != 0 ) {
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
