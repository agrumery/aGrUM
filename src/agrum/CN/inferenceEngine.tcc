#include <agrum/CN/inferenceEngine.h>

namespace gum {
  namespace credal {

    /*template< typename GUM_SCALAR >
    InferenceEngine< GUM_SCALAR >::InferenceEngine () : ApproximationScheme() {
      std::cout << "InferenceEngine construct ()" << std::endl;
      GUM_CONSTRUCTOR ( InferenceEngine );
    }*/

    template <typename GUM_SCALAR>
    InferenceEngine<GUM_SCALAR>::InferenceEngine(
        const CredalNet<GUM_SCALAR> &credalNet)
        : ApproximationScheme() {
      _credalNet = &credalNet;

      _dbnOpt.setCNet(credalNet);

      _initMarginals();

      GUM_CONSTRUCTOR(InferenceEngine);
    }

    template <typename GUM_SCALAR> InferenceEngine<GUM_SCALAR>::~InferenceEngine() {
      GUM_DESTRUCTOR(InferenceEngine);
    }

    template <typename GUM_SCALAR>
    const CredalNet<GUM_SCALAR> &InferenceEngine<GUM_SCALAR>::credalNet() {
      return *_credalNet;
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::eraseAllEvidence() {
      _evidence.clear();
      _query.clear();
      /*
          _marginalMin.clear();
          _marginalMax.clear();
          _oldMarginalMin.clear();
          _oldMarginalMax.clear();
      */
      _initMarginals();
      /*
          _expectationMin.clear();
          _expectationMax.clear();
      */
      _initExpectations();

      //  _marginalSets.clear();
      _initMarginalSets();

      _dynamicExpMin.clear();
      _dynamicExpMax.clear();

      //_modal.clear();

      //_t0.clear();
      //_t1.clear();
    }

    /*
    template< typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::setIterStop ( const int &iter_stop ) {
      _iterStop = iter_stop;
    }*/

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::storeBNOpt(const bool value) {
      _storeBNOpt = value;
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::storeVertices(const bool value) {
      _storeVertices = value;

      if (value)
        _initMarginalSets();
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::setRepetitiveInd(const bool repetitive) {
      bool oldValue = _repetitiveInd;
      _repetitiveInd = repetitive;

      // do not compute clusters more than once
      if (_repetitiveInd && !oldValue)
        _repetitiveInit();
    }

    template <typename GUM_SCALAR>
    bool InferenceEngine<GUM_SCALAR>::repetitiveInd() const {
      return _repetitiveInd;
    }
    /*
        template< typename GUM_SCALAR >
        int InferenceEngine< GUM_SCALAR >::iterStop () const {
          return _iterStop;
        }*/

    template <typename GUM_SCALAR>
    bool InferenceEngine<GUM_SCALAR>::storeVertices() const {
      return _storeVertices;
    }

    template <typename GUM_SCALAR>
    bool InferenceEngine<GUM_SCALAR>::storeBNOpt() const {
      return _storeBNOpt;
    }

    template <typename GUM_SCALAR>
    VarMod2BNsMap<GUM_SCALAR> *InferenceEngine<GUM_SCALAR>::getVarMod2BNsMap() {
      return &_dbnOpt;
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::insertModalsFile(const std::string &path) {
      std::ifstream mod_stream(path.c_str(), std::ios::in);

      if (!mod_stream.good()) {
        GUM_ERROR(OperationNotAllowed, "void InferenceEngine< GUM_SCALAR "
                                       ">::insertModals(const std::string & path) : "
                                       "could not open input file : "
                                           << path);
      }

      if (!_modal.empty())
        _modal.clear();

      std::string line, tmp;
      char *cstr, *p;

      while (mod_stream.good()) {
        getline(mod_stream, line);

        if (line.size() == 0)
          continue;

        cstr = new char[line.size() + 1];
        strcpy(cstr, line.c_str());

        p = strtok(cstr, " ");
        tmp = p;

        std::vector<GUM_SCALAR> values;
        p = strtok(nullptr, " ");

        while (p != nullptr) {
          values.push_back(atof(p));
          p = strtok(nullptr, " ");
        } // end of : line

        _modal.insert(tmp, values); //[tmp] = values;

        delete[] p;
        delete[] cstr;
      } // end of : file

      mod_stream.close();

      _initExpectations();
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::insertModals(
        const std::map<std::string, std::vector<GUM_SCALAR>> &modals) {
      if (!_modal.empty())
        _modal.clear();

      for (auto it = modals.cbegin(), theEnd = modals.cend(); it != theEnd; ++it) {
        NodeId id;

        try {
          id = _credalNet->current_bn().idFromName(it->first);
        } catch (NotFound &err) {
          GUM_SHOWERROR(err);
          continue;
        }

        // check that modals are net compatible
        auto dSize = _credalNet->current_bn().variable(id).domainSize();

        if (dSize != it->second.size())
          continue;

        // GUM_ERROR(OperationNotAllowed, "void InferenceEngine< GUM_SCALAR
        // >::insertModals( const std::map< std::string, std::vector< GUM_SCALAR > >
        // &modals) : modalities does not respect variable cardinality : " <<
        // _credalNet->current_bn().variable( id ).name() << " : " << dSize << " != "
        // << it->second.size());

        _modal.insert(it->first, it->second); //[ it->first ] = it->second;
      }

      //_modal = modals;

      _initExpectations();
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::insertEvidence(
        const std::map<std::string, std::vector<GUM_SCALAR>> &eviMap) {
      if (!_evidence.empty())
        _evidence.clear();

      for (auto it = eviMap.cbegin(), theEnd = eviMap.cend(); it != theEnd; ++it) {
        NodeId id;

        try {
          id = _credalNet->current_bn().idFromName(it->first);
        } catch (NotFound &err) {
          GUM_SHOWERROR(err);
          continue;
        }

        _evidence.insert(id, it->second);
      }
    }

    // check that observed variables DO exists in the network (otherwise Lazy report
    // an error and app crash)
    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::insertEvidence(
        const NodeProperty<std::vector<GUM_SCALAR>> &evidence) {
      if (!_evidence.empty())
        _evidence.clear();

      // use cbegin() to get const_iterator when available in aGrUM hashtables
      for (const auto &elt : evidence) {
        try {
          _credalNet->current_bn().variable(elt.first);
        } catch (NotFound &err) {
          GUM_SHOWERROR(err);
          continue;
        }

        _evidence.insert(elt.first, elt.second);
      }
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::insertEvidenceFile(const std::string &path) {
      std::ifstream evi_stream(path.c_str(), std::ios::in);

      if (!evi_stream.good()) {
        GUM_ERROR(IOError, "void InferenceEngine< GUM_SCALAR "
                           ">::insertEvidence(const std::string & path) : could not "
                           "open input file : "
                               << path);
      }

      if (!_evidence.empty())
        _evidence.clear();

      std::string line, tmp;
      char *cstr, *p;

      while (evi_stream.good() && std::strcmp(line.c_str(), "[EVIDENCE]") != 0) {
        getline(evi_stream, line);
      }

      while (evi_stream.good()) {
        getline(evi_stream, line);

        if (std::strcmp(line.c_str(), "[QUERY]") == 0)
          break;

        if (line.size() == 0)
          continue;

        cstr = new char[line.size() + 1];
        strcpy(cstr, line.c_str());

        p = strtok(cstr, " ");
        tmp = p;

        // if user input is wrong
        NodeId node = -1;

        try {
          node = _credalNet->current_bn().idFromName(tmp);
        } catch (NotFound &err) {
          GUM_SHOWERROR(err);
          continue;
        }

        std::vector<GUM_SCALAR> values;
        p = strtok(nullptr, " ");

        while (p != nullptr) {
          values.push_back(atof(p));
          p = strtok(nullptr, " ");
        } // end of : line

        _evidence.insert(node, values);

        delete[] p;
        delete[] cstr;
      } // end of : file

      evi_stream.close();
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::insertQuery(
        const NodeProperty<std::vector<bool>> &query) {
      if (!_query.empty())
        _query.clear();

      for (const auto &elt : query) {
        try {
          _credalNet->current_bn().variable(elt.first);
        } catch (NotFound &err) {
          GUM_SHOWERROR(err);
          continue;
        }

        _query.insert(elt.first, elt.second);
      }
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::insertQueryFile(const std::string &path) {
      std::ifstream evi_stream(path.c_str(), std::ios::in);

      if (!evi_stream.good()) {
        GUM_ERROR(IOError, "void InferenceEngine< GUM_SCALAR >::insertQuery(const "
                           "std::string & path) : could not open input file : "
                               << path);
      }

      if (!_query.empty())
        _query.clear();

      std::string line, tmp;
      char *cstr, *p;

      while (evi_stream.good() && std::strcmp(line.c_str(), "[QUERY]") != 0) {
        getline(evi_stream, line);
      }

      while (evi_stream.good()) {
        getline(evi_stream, line);

        if (std::strcmp(line.c_str(), "[EVIDENCE]") == 0)
          break;

        if (line.size() == 0)
          continue;

        cstr = new char[line.size() + 1];
        strcpy(cstr, line.c_str());

        p = strtok(cstr, " ");
        tmp = p;

        // if user input is wrong
        NodeId node = -1;

        try {
          node = _credalNet->current_bn().idFromName(tmp);
        } catch (NotFound &err) {
          GUM_SHOWERROR(err);
          continue;
        }

        auto dSize = _credalNet->current_bn().variable(node).domainSize();

        p = strtok(nullptr, " ");

        if (p == nullptr) {
          _query.insert(node, std::vector<bool>(dSize, true));
        } else {
          std::vector<bool> values(dSize, false);

          while (p != nullptr) {
            if ((Size)atoi(p) >= dSize)
              GUM_ERROR(OutOfBounds, "void InferenceEngine< GUM_SCALAR "
                                     ">::insertQuery(const std::string & path) : "
                                     "query modality is higher or equal to "
                                     "cardinality");

            values[atoi(p)] = true;
            p = strtok(nullptr, " ");
          } // end of : line

          _query.insert(node, values);
        }

        delete[] p;
        delete[] cstr;
      } // end of : file

      evi_stream.close();
    }

    template <typename GUM_SCALAR>
    const std::vector<GUM_SCALAR> &
    InferenceEngine<GUM_SCALAR>::marginalMin(const std::string &varName) const {
      try {
        return _marginalMin[_credalNet->current_bn().idFromName(varName)];
      } catch (NotFound &err) {
        throw(err);
      }
    }

    template <typename GUM_SCALAR>
    const std::vector<GUM_SCALAR> &
    InferenceEngine<GUM_SCALAR>::marginalMax(const std::string &varName) const {
      try {
        return _marginalMax[_credalNet->current_bn().idFromName(varName)];
      } catch (NotFound &err) {
        throw(err);
      }
    }

    template <typename GUM_SCALAR>
    const std::vector<GUM_SCALAR> &
    InferenceEngine<GUM_SCALAR>::marginalMin(const NodeId id) const {
      try {
        return _marginalMin[id];
      } catch (NotFound &err) {
        throw(err);
      }
    }

    template <typename GUM_SCALAR>
    const std::vector<GUM_SCALAR> &
    InferenceEngine<GUM_SCALAR>::marginalMax(const NodeId id) const {
      try {
        return _marginalMax[id];
      } catch (NotFound &err) {
        throw(err);
      }
    }

    template <typename GUM_SCALAR>
    const GUM_SCALAR &
    InferenceEngine<GUM_SCALAR>::expectationMin(const std::string &varName) const {
      try {
        return _expectationMin[_credalNet->current_bn().idFromName(varName)];
      } catch (NotFound &err) {
        throw(err);
      }
    }

    template <typename GUM_SCALAR>
    const GUM_SCALAR &
    InferenceEngine<GUM_SCALAR>::expectationMax(const std::string &varName) const {
      try {
        return _expectationMax[_credalNet->current_bn().idFromName(varName)];
      } catch (NotFound &err) {
        throw(err);
      }
    }

    template <typename GUM_SCALAR>
    const GUM_SCALAR &
    InferenceEngine<GUM_SCALAR>::expectationMin(const NodeId id) const {
      try {
        return _expectationMin[id];
      } catch (NotFound &err) {
        throw(err);
      }
    }

    template <typename GUM_SCALAR>
    const GUM_SCALAR &
    InferenceEngine<GUM_SCALAR>::expectationMax(const NodeId id) const {
      try {
        return _expectationMax[id];
      } catch (NotFound &err) {
        throw(err);
      }
    }

    template <typename GUM_SCALAR>
    const std::vector<GUM_SCALAR> &
    InferenceEngine<GUM_SCALAR>::dynamicExpMin(const std::string &varName) const {
      std::string errTxt = "const std::vector< GUM_SCALAR > & InferenceEngine< "
                           "GUM_SCALAR >::dynamicExpMin ( const std::string & "
                           "varName ) const : ";

      if (_dynamicExpMin.empty())
        GUM_ERROR(OperationNotAllowed,
                  errTxt + "_dynamicExpectations() needs to be called before");

      if (!_dynamicExpMin.exists(
              varName) /*_dynamicExpMin.find(varName) == _dynamicExpMin.end()*/)
        GUM_ERROR(NotFound, errTxt + "variable name not found : " << varName);

      return _dynamicExpMin[varName];
    }

    template <typename GUM_SCALAR>
    const std::vector<GUM_SCALAR> &
    InferenceEngine<GUM_SCALAR>::dynamicExpMax(const std::string &varName) const {
      std::string errTxt = "const std::vector< GUM_SCALAR > & InferenceEngine< "
                           "GUM_SCALAR >::dynamicExpMax ( const std::string & "
                           "varName ) const : ";

      if (_dynamicExpMax.empty())
        GUM_ERROR(OperationNotAllowed,
                  errTxt + "_dynamicExpectations() needs to be called before");

      if (!_dynamicExpMax.exists(
              varName) /*_dynamicExpMin.find(varName) == _dynamicExpMin.end()*/)
        GUM_ERROR(NotFound, errTxt + "variable name not found : " << varName);

      return _dynamicExpMax[varName];
    }

    template <typename GUM_SCALAR>
    const std::vector<std::vector<GUM_SCALAR>> &
    InferenceEngine<GUM_SCALAR>::vertices(const NodeId id) const {
      return _marginalSets[id];
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::saveMarginals(const std::string &path) const {
      std::ofstream m_stream(path.c_str(), std::ios::out | std::ios::trunc);

      if (!m_stream.good()) {
        GUM_ERROR(IOError, "void InferenceEngine< GUM_SCALAR >::saveMarginals(const "
                           "std::string & path) const : could not open output file "
                           ": "
                               << path);
      }

      for (const auto &elt : _marginalMin) {
        Size esize = elt.second.size();

        for (Size mod = 0; mod < esize; mod++) {
          m_stream << _credalNet->current_bn().variable(elt.first).name() << " "
                   << mod << " " << (elt.second)[mod] << " "
                   << _marginalMax[elt.first][mod] << std::endl;
        }
      }

      m_stream.close();
    }

    template <typename GUM_SCALAR>
    void
    InferenceEngine<GUM_SCALAR>::saveExpectations(const std::string &path) const {
      if (_dynamicExpMin.empty()) //_modal.empty())
        return;

      // else not here, to keep the const (natural with a saving process)
      // else if(_dynamicExpMin.empty() || _dynamicExpMax.empty())
      //_dynamicExpectations(); // works with or without a dynamic network

      std::ofstream m_stream(path.c_str(), std::ios::out | std::ios::trunc);

      if (!m_stream.good()) {
        GUM_ERROR(IOError, "void InferenceEngine< GUM_SCALAR "
                           ">::saveExpectations(const std::string & path) : could "
                           "not open output file : "
                               << path);
      }

      for (const auto &elt : _dynamicExpMin) {
        m_stream << elt.first; // it->first;

        // iterates over a vector
        for (const auto &elt2 : elt.second) {
          m_stream << " " << elt2;
        }

        m_stream << std::endl;
      }

      for (const auto &elt : _dynamicExpMax) {
        m_stream << elt.first;

        // iterates over a vector
        for (const auto &elt2 : elt.second) {
          m_stream << " " << elt2;
        }

        m_stream << std::endl;
      }

      m_stream.close();
    }

    template <typename GUM_SCALAR>
    std::string InferenceEngine<GUM_SCALAR>::toString() const {
      std::stringstream output;
      output << std::endl;

      // use cbegin() when available
      for (const auto &elt : _marginalMin) {
        Size esize = elt.second.size();

        for (Size mod = 0; mod < esize; mod++) {
          output << "P(" << _credalNet->current_bn().variable(elt.first).name()
                 << "=" << mod << "|e) = [ ";
          output << _marginalMin[elt.first][mod] << ", "
                 << _marginalMax[elt.first][mod] << " ]";

          if (!_query.empty())
            if (_query.exists(elt.first) && _query[elt.first][mod])
              output << " QUERY";

          output << std::endl;
        }

        output << std::endl;
      }

      return output.str();
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::saveVertices(const std::string &path) const {
      std::ofstream m_stream(path.c_str(), std::ios::out | std::ios::trunc);

      if (!m_stream.good()) {
        GUM_ERROR(IOError, "void InferenceEngine< GUM_SCALAR >::saveVertices(const "
                           "std::string & path) : could not open outpul file : "
                               << path);
      }

      for (const auto &elt : _marginalSets) {
        m_stream << _credalNet->current_bn().variable(elt.first).name() << std::endl;

        for (const auto &elt2 : elt.second) {
          m_stream << "[";
          bool first = true;

          for (const auto &elt3 : elt2) {
            if (!first) {
              m_stream << ",";
              first = false;
            }

            m_stream << elt3;
          }

          m_stream << "]\n";
        }
      }

      m_stream.close();
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::_initMarginals() {
      _marginalMin.clear();
      _marginalMax.clear();
      _oldMarginalMin.clear();
      _oldMarginalMax.clear();

      for (auto node : _credalNet->current_bn().nodes()) {
        auto dSize = _credalNet->current_bn().variable(node).domainSize();
        _marginalMin.insert(node, std::vector<GUM_SCALAR>(dSize, 1));
        _oldMarginalMin.insert(node, std::vector<GUM_SCALAR>(dSize, 1));

        _marginalMax.insert(node, std::vector<GUM_SCALAR>(dSize, 0));
        _oldMarginalMax.insert(node, std::vector<GUM_SCALAR>(dSize, 0));
      }
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::_initMarginalSets() {
      _marginalSets.clear();

      if (!_storeVertices)
        return;

      for (auto node : _credalNet->current_bn().nodes())
        _marginalSets.insert(node, std::vector<std::vector<GUM_SCALAR>>());
    }

    // since only monitored variables in _modal will be alble to compute
    // expectations, it is useless to initialize those for all variables
    // _modal variables will always be checked further, so it is not necessary to
    // check it here, but doing so will use less memory
    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::_initExpectations() {
      _expectationMin.clear();
      _expectationMax.clear();

      if (_modal.empty())
        return;

      for (auto node : _credalNet->current_bn().nodes()) {
        std::string var_name, time_step;

        var_name = _credalNet->current_bn().variable(node).name();
        auto delim = var_name.find_first_of("_");
        time_step = var_name.substr(delim + 1, var_name.size());
        var_name = var_name.substr(0, delim);

        if (!_modal.exists(var_name))
          continue;

        _expectationMin.insert(node, _modal[var_name].back());
        _expectationMax.insert(node, _modal[var_name].front());
      }
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::dynamicExpectations() {
      _dynamicExpectations();
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::_dynamicExpectations() {
      // no modals, no expectations computed during inference
      if (_expectationMin.empty() || _modal.empty())
        return;

      // already called by the algorithm or the user
      if (_dynamicExpMax.size() > 0 && _dynamicExpMin.size() > 0)
        return;

      // typedef typename std::map< int, GUM_SCALAR > innerMap;
      typedef typename gum::HashTable<int, GUM_SCALAR> innerMap;

      // typedef typename std::map< std::string, innerMap > outerMap;
      typedef typename gum::HashTable<std::string, innerMap> outerMap;

      // typedef typename std::map< std::string, std::vector< GUM_SCALAR > > mod;

      // si non dynamique, sauver directement _expectationMin et Max (revient au meme
      // mais plus rapide)
      outerMap expectationsMin, expectationsMax;

      for (const auto &elt : _expectationMin) {
        std::string var_name, time_step;

        var_name = _credalNet->current_bn().variable(elt.first).name();
        auto delim = var_name.find_first_of("_");
        time_step = var_name.substr(delim + 1, var_name.size());
        var_name = var_name.substr(0, delim);

        // to be sure (don't store not monitored variables' expectations) although it
        // should be taken care of before this point
        if (!_modal.exists(var_name))
          continue;

        expectationsMin.getWithDefault(var_name, innerMap())
            .getWithDefault(atoi(time_step.c_str()), 0) =
            elt.second; // we iterate with min iterators
        expectationsMax.getWithDefault(var_name, innerMap())
            .getWithDefault(atoi(time_step.c_str()), 0) = _expectationMax[elt.first];
      }

      for (const auto &elt : expectationsMin) {
        typename std::vector<GUM_SCALAR> dynExp(elt.second.size());

        for (const auto &elt2 : elt.second)
          dynExp[elt2.first] = elt2.second;

        _dynamicExpMin.insert(elt.first, dynExp);
      }

      for (const auto &elt : expectationsMax) {
        typename std::vector<GUM_SCALAR> dynExp(elt.second.size());

        for (const auto &elt2 : elt.second) {
          dynExp[elt2.first] = elt2.second;
        }

        _dynamicExpMax.insert(elt.first, dynExp);
      }
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::_repetitiveInit() {
      _timeSteps = 0;
      _t0.clear();
      _t1.clear();

      // t = 0 vars belongs to _t0 as keys
      for (auto node : _credalNet->current_bn().dag().nodes()) {
        std::string var_name = _credalNet->current_bn().variable(node).name();
        auto delim = var_name.find_first_of("_");

        if (delim > var_name.size()) {
          GUM_ERROR(InvalidArgument, "void InferenceEngine< GUM_SCALAR "
                                     ">::_repetitiveInit() : the network does not "
                                     "appear to be dynamic");
        }

        std::string time_step = var_name.substr(delim + 1, 1);

        if (time_step.compare("0") == 0)
          _t0.insert(node, std::vector<NodeId>());
      }

      // t = 1 vars belongs to either _t0 as member value or _t1 as keys
      for (const auto &node : _credalNet->current_bn().dag().nodes()) {
        std::string var_name = _credalNet->current_bn().variable(node).name();
        auto delim = var_name.find_first_of("_");
        std::string time_step = var_name.substr(delim + 1, var_name.size());
        var_name = var_name.substr(0, delim);
        delim = time_step.find_first_of("_");
        time_step = time_step.substr(0, delim);

        if (time_step.compare("1") == 0) {
          bool found = false;

          for (const auto &elt : _t0) {
            std::string var_0_name =
                _credalNet->current_bn().variable(elt.first).name();
            delim = var_0_name.find_first_of("_");
            var_0_name = var_0_name.substr(0, delim);

            if (var_name.compare(var_0_name) == 0) {
              const Potential<GUM_SCALAR> *potential(
                  &_credalNet->current_bn().cpt(node));
              const Potential<GUM_SCALAR> *potential2(
                  &_credalNet->current_bn().cpt(elt.first));

              if (potential->domainSize() == potential2->domainSize())
                _t0[elt.first].push_back(node);
              else
                _t1.insert(node, std::vector<NodeId>());

              found = true;
              break;
            }
          }

          if (!found) {
            _t1.insert(node, std::vector<NodeId>());
          }
        }
      }

      // t > 1 vars belongs to either _t0 or _t1 as member value
      // remember _timeSteps
      for (auto node : _credalNet->current_bn().dag().nodes()) {
        std::string var_name = _credalNet->current_bn().variable(node).name();
        auto delim = var_name.find_first_of("_");
        std::string time_step = var_name.substr(delim + 1, var_name.size());
        var_name = var_name.substr(0, delim);
        delim = time_step.find_first_of("_");
        time_step = time_step.substr(0, delim);

        if (time_step.compare("0") != 0 && time_step.compare("1") != 0) {
          // keep max time_step
          if (atoi(time_step.c_str()) > _timeSteps)
            _timeSteps = atoi(time_step.c_str());

          std::string var_0_name;
          bool found = false;

          for (const auto &elt : _t0) {
            std::string var_0_name =
                _credalNet->current_bn().variable(elt.first).name();
            delim = var_0_name.find_first_of("_");
            var_0_name = var_0_name.substr(0, delim);

            if (var_name.compare(var_0_name) == 0) {
              const Potential<GUM_SCALAR> *potential(
                  &_credalNet->current_bn().cpt(node));
              const Potential<GUM_SCALAR> *potential2(
                  &_credalNet->current_bn().cpt(elt.first));

              if (potential->domainSize() == potential2->domainSize()) {
                _t0[elt.first].push_back(node);
                found = true;
                break;
              }
            }
          }

          if (!found) {
            for (const auto &elt : _t1) {
              std::string var_0_name =
                  _credalNet->current_bn().variable(elt.first).name();
              auto delim = var_0_name.find_first_of("_");
              var_0_name = var_0_name.substr(0, delim);

              if (var_name.compare(var_0_name) == 0) {
                const Potential<GUM_SCALAR> *potential(
                    &_credalNet->current_bn().cpt(node));
                const Potential<GUM_SCALAR> *potential2(
                    &_credalNet->current_bn().cpt(elt.first));

                if (potential->domainSize() == potential2->domainSize()) {
                  _t1[elt.first].push_back(node);
                  break;
                }
              }
            }
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::_updateExpectations(
        const NodeId &id, const std::vector<GUM_SCALAR> &vertex) {
      std::string var_name = _credalNet->current_bn().variable(id).name();
      auto delim = var_name.find_first_of("_");

      var_name = var_name.substr(0, delim);

      if (_modal.exists(var_name) /*_modal.find(var_name) != _modal.end()*/) {
        GUM_SCALAR exp = 0;
        auto vsize = vertex.size();

        for (Size mod = 0; mod < vsize; mod++)
          exp += vertex[mod] * _modal[var_name][mod];

        if (exp > _expectationMax[id])
          _expectationMax[id] = exp;

        if (exp < _expectationMin[id])
          _expectationMin[id] = exp;
      }
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::_updateCredalSetsWithFiles(
        const NodeId &id, const std::vector<GUM_SCALAR> &vertex,
        const bool &elimRedund) {
      auto &nodeCredalSet = _marginalSets[id];

      bool eq = true;

      for (auto it = nodeCredalSet.cbegin(), itEnd = nodeCredalSet.cend();
           it != itEnd; ++it) {
        eq = true;

        for (auto end = (Size)vertex.size(), i = (Size)0; i < end; i++) {
          if (std::fabs(vertex[i] - (*it)[i]) > 1e-6) {
            eq = false;
            break;
          }
        }

        if (eq)
          break;
      }

      if (!eq || nodeCredalSet.size() == 0)
        nodeCredalSet.push_back(vertex);
      else
        return;

      // check that the point and all previously added ones are not inside the actual
      // polytope
      auto itEnd = std::remove_if(nodeCredalSet.begin(), nodeCredalSet.end(),
                                  [&](const std::vector<GUM_SCALAR> &v) -> bool {
        for (auto jt = v.cbegin(), jtEnd = v.cend(),
                  minIt = _marginalMin[id].cbegin(),
                  minItEnd = _marginalMin[id].cend(),
                  maxIt = _marginalMax[id].cbegin(),
                  maxItEnd = _marginalMax[id].cend();
             jt != jtEnd && minIt != minItEnd && maxIt != maxItEnd;
             ++jt, ++minIt, ++maxIt) {
          if ((std::fabs(*jt - *minIt) < 1e-6 || std::fabs(*jt - *maxIt) < 1e-6) &&
              std::fabs(*minIt - *maxIt) > 1e-6)
            return false;
        }
        return true;
      });

      nodeCredalSet.erase(itEnd, nodeCredalSet.end());

      // we need at least 2 points to make a convex combination
      if (!elimRedund || nodeCredalSet.size() <= 2)
        return;

      // there may be points not inside the polytope but on one of it's facet,
      // meaning it's still a convex combination of vertices of this facet. Here we
      // need lrs.

      std::vector<std::vector<GUM_SCALAR>> outputCSet; // final set

      std::string svfile = getUniqueFileName(); // generate unique file name, we need
                                                // to add .ine or .ext for lrs to
                                                // know which input it is (Hrep to
                                                // Vrep or Vrep to Hrep)

      svfile += ".ext";

      std::ofstream v_file(svfile.c_str(), std::ios::out | std::ios::trunc);

      if (!v_file.good())
        GUM_ERROR(IOError, "could not open lrs input file : " << svfile);

      v_file << "V-representation\n";
      v_file << "begin\n";

      auto setSize = nodeCredalSet.size();

      v_file << setSize; // number of vertices
      auto mods = vertex.size();
      v_file << " " << mods + 1 << " rational\n";

      for (Size vtex = 0; vtex < setSize; vtex++) {
        v_file << " 1  ";

        for (Size modal = 0; modal < mods; modal++) {
          long int num, den;
          GUM_SCALAR pr = nodeCredalSet[vtex][modal];

          Rational<GUM_SCALAR>::farey(num, den, pr, 1e6, 1e-6);
          v_file << num << '/' << den << ' ';
        }

        v_file << std::endl;
      }

      v_file << "end\n";
      v_file.close();

      // call lrs
      char *args[3];

      std::string soft_name = "redund";
      std::string extfile(svfile);
      extfile += ".r.ext";

      args[0] = new char[soft_name.size()];
      args[1] = new char[svfile.size()];
      args[2] = new char[extfile.size()];

      strcpy(args[0], soft_name.c_str());
      strcpy(args[1], svfile.c_str());
      strcpy(args[2], extfile.c_str());

      // standard cout to null (avoid lrs flooding)
      int old_cout, new_cout;
      fflush(stdout);
      old_cout = dup(1);

      new_cout = open("/dev/null", O_WRONLY);
      dup2(new_cout, 1);
      close(new_cout);

      redund_main(3, args);

      // restore standard cout
      fflush(stdout);
      dup2(old_cout, 1);
      close(old_cout);

      delete[] args[2];
      delete[] args[1];
      delete[] args[0];

      // read V rep file
      std::ifstream e_file(extfile.c_str() /*extfilename.c_str()*/, std::ios::in);

      if (!e_file.good())
        GUM_ERROR(IOError, "__H2Vlrs : could not open lrs ouput file : " << extfile);

      std::string line, tmp;
      char *cstr, *p;
      GUM_SCALAR probability;

      std::string::size_type pos, end_pos;
      bool keep_going = true;
      // int vertices;

      std::vector<GUM_SCALAR> vtx;

      e_file.ignore(256, 'l');

      while (e_file.good() && keep_going) {
        getline(e_file, line);

        if (line.size() == 0)
          continue;
        else if (line.compare("end") == 0) {
          keep_going = false;
          // this is to get vertices number :
          /*getline ( v_file, line );
            pos = line.find ( "vertices = " );
            end_pos = line.find ( "rays", pos + 9 );
            vertices = atoi ( line.substr ( pos + 9, end_pos - pos - 9 ).c_str() );*/
          break;
        } else if (line[1] != '1') {
          GUM_ERROR(IOError,
                    "reading something other than a vertex from lrs output file : "
                        << extfile);
        }

        line = line.substr(2);
        cstr = new char[line.size() + 1];
        strcpy(cstr, line.c_str());

        p = strtok(cstr, " ");

        while (p != nullptr) {
          tmp = p;

          if (tmp.compare("1") == 0 || tmp.compare("0") == 0)
            probability = atof(tmp.c_str());
          else {
            pos = tmp.find("/");
            probability = atof(tmp.substr(0, pos).c_str()) /
                          atof(tmp.substr(pos + 1, tmp.size()).c_str());
          }

          vtx.push_back(probability);
          p = strtok(nullptr, " ");
        } // end of : for all tokens

        delete[] p;
        delete[] cstr;

        outputCSet.push_back(vtx);
        vtx.clear();

      } // end of file

      e_file.close();

      if (std::remove(svfile.c_str()) != 0)
        GUM_ERROR(IOError, "error removing : " << svfile);

      if (std::remove(extfile.c_str()) != 0)
        GUM_ERROR(IOError, "error removing : " << extfile);

      _marginalSets[id] = outputCSet;
    }

    template <typename GUM_SCALAR>
    void InferenceEngine<GUM_SCALAR>::_updateCredalSets(
        const NodeId &id, const std::vector<GUM_SCALAR> &vertex,
        const bool &elimRedund) {
      auto &nodeCredalSet = _marginalSets[id];
      auto dsize = vertex.size();

      bool eq = true;

      for (auto it = nodeCredalSet.cbegin(), itEnd = nodeCredalSet.cend();
           it != itEnd; ++it) {
        eq = true;

        for (Size i = 0; i < dsize; i++) {
          if (std::fabs(vertex[i] - (*it)[i]) > 1e-6) {
            eq = false;
            break;
          }
        }

        if (eq)
          break;
      }

      if (!eq || nodeCredalSet.size() == 0) {
        nodeCredalSet.push_back(vertex);
        return;
      } else
        return;

      // because of next lambda return condition
      if (nodeCredalSet.size() == 1)
        return;

      // check that the point and all previously added ones are not inside the actual
      // polytope
      auto itEnd = std::remove_if(nodeCredalSet.begin(), nodeCredalSet.end(),
                                  [&](const std::vector<GUM_SCALAR> &v) -> bool {
        for (auto jt = v.cbegin(), jtEnd = v.cend(),
                  minIt = _marginalMin[id].cbegin(),
                  minItEnd = _marginalMin[id].cend(),
                  maxIt = _marginalMax[id].cbegin(),
                  maxItEnd = _marginalMax[id].cend();
             jt != jtEnd && minIt != minItEnd && maxIt != maxItEnd;
             ++jt, ++minIt, ++maxIt) {
          if ((std::fabs(*jt - *minIt) < 1e-6 || std::fabs(*jt - *maxIt) < 1e-6) &&
              std::fabs(*minIt - *maxIt) > 1e-6)
            return false;
        }
        return true;
      });

      nodeCredalSet.erase(itEnd, nodeCredalSet.end());

      // we need at least 2 points to make a convex combination
      if (!elimRedund || nodeCredalSet.size() <= 2)
        return;

      // there may be points not inside the polytope but on one of it's facet,
      // meaning it's still a convex combination of vertices of this facet. Here we
      // need lrs.
      auto setSize = nodeCredalSet.size();

      LRSWrapper<GUM_SCALAR> lrsWrapper;
      lrsWrapper.setUpV(dsize, setSize);

      for (const auto &vtx : nodeCredalSet)
        lrsWrapper.fillV(vtx);

      lrsWrapper.elimRedundVrep();

      _marginalSets[id] = lrsWrapper.getOutput();
    }

    template <typename GUM_SCALAR>
    const typename Property<std::vector<NodeId>>::onNodes &
    InferenceEngine<GUM_SCALAR>::getT0Cluster() const {
      return _t0;
    }

    template <typename GUM_SCALAR>
    const typename Property<std::vector<NodeId>>::onNodes &
    InferenceEngine<GUM_SCALAR>::getT1Cluster() const {
      return _t1;
    }

    template <typename GUM_SCALAR>
    inline const GUM_SCALAR InferenceEngine<GUM_SCALAR>::_computeEpsilon() {
      GUM_SCALAR eps = 0;
#pragma omp parallel
      {
        GUM_SCALAR tEps = 0;
        GUM_SCALAR delta;

        /// int tId = getThreadNumber();
        auto nsize = _marginalMin.size();

#pragma omp for

        for (Size i = 0; i < nsize; i++) {
          auto dSize = _marginalMin[i].size();

          for (Size j = 0; j < dSize; j++) {
            // on min
            delta = _marginalMin[i][j] - _oldMarginalMin[i][j];
            delta = (delta < 0) ? (-delta) : delta;
            tEps = (tEps < delta) ? delta : tEps;

            // on max
            delta = _marginalMax[i][j] - _oldMarginalMax[i][j];
            delta = (delta < 0) ? (-delta) : delta;
            tEps = (tEps < delta) ? delta : tEps;

            _oldMarginalMin[i][j] = _marginalMin[i][j];
            _oldMarginalMax[i][j] = _marginalMax[i][j];
          }
        } // end of : all variables

#pragma omp critical(epsilon_max)
        {
#pragma omp flush(eps)
          eps = (eps < tEps) ? tEps : eps;
        }
      }

      return eps;
    }
  }
} // namespace
