/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief Abstract class representing CredalNet inference engines
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */

#include <algorithm>

#include <agrum/CN/inference/inferenceEngine.h>
#include <agrum/agrum.h>

namespace gum {
  namespace credal {

    /*template< typename GUM_SCALAR >
    InferenceEngine< GUM_SCALAR >::InferenceEngine () : ApproximationScheme() {
      std::cout << "InferenceEngine construct ()" << std::endl;
      GUM_CONSTRUCTOR ( InferenceEngine )
    }*/

    template < typename GUM_SCALAR >
    InferenceEngine< GUM_SCALAR >::InferenceEngine(const CredalNet< GUM_SCALAR >& credalNet) :
        ApproximationScheme() {
      credalNet_ = &credalNet;

      dbnOpt_.setCNet(credalNet);

      initMarginals_();

      GUM_CONSTRUCTOR(InferenceEngine);
    }

    template < typename GUM_SCALAR >
    InferenceEngine< GUM_SCALAR >::~InferenceEngine() {
      GUM_DESTRUCTOR(InferenceEngine);
    }

    template < typename GUM_SCALAR >
    const CredalNet< GUM_SCALAR >& InferenceEngine< GUM_SCALAR >::credalNet() const {
      return *credalNet_;
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::eraseAllEvidence() {
      evidence_.clear();
      query_.clear();
      /*
          marginalMin_.clear();
          marginalMax_.clear();
          oldMarginalMin_.clear();
          oldMarginalMax_.clear();
      */
      initMarginals_();
      /*
          expectationMin_.clear();
          expectationMax_.clear();
      */
      initExpectations_();

      //  marginalSets_.clear();
      initMarginalSets_();

      dynamicExpMin_.clear();
      dynamicExpMax_.clear();

      //_modal.clear();

      //_t0.clear();
      //_t1.clear();
    }

    /*
    template< typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::setIterStop ( const int &iter_stop ) {
      iterStop_ = iter_stop;
    }*/

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::storeBNOpt(const bool value) {
      storeBNOpt_ = value;
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::storeVertices(const bool value) {
      storeVertices_ = value;

      if (value) initMarginalSets_();
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::setRepetitiveInd(const bool repetitive) {
      bool oldValue  = repetitiveInd_;
      repetitiveInd_ = repetitive;

      // do not compute clusters more than once
      if (repetitiveInd_ && !oldValue) repetitiveInit_();
    }

    template < typename GUM_SCALAR >
    bool InferenceEngine< GUM_SCALAR >::repetitiveInd() const {
      return repetitiveInd_;
    }
    /*
        template< typename GUM_SCALAR >
        int InferenceEngine< GUM_SCALAR >::iterStop () const {
          return iterStop_;
        }*/

    template < typename GUM_SCALAR >
    bool InferenceEngine< GUM_SCALAR >::storeVertices() const {
      return storeVertices_;
    }

    template < typename GUM_SCALAR >
    bool InferenceEngine< GUM_SCALAR >::storeBNOpt() const {
      return storeBNOpt_;
    }

    template < typename GUM_SCALAR >
    VarMod2BNsMap< GUM_SCALAR >* InferenceEngine< GUM_SCALAR >::getVarMod2BNsMap() {
      return &dbnOpt_;
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::insertModalsFile(const std::string& path) {
      std::ifstream mod_stream(path.c_str(), std::ios::in);

      if (!mod_stream.good()) {
        GUM_ERROR(OperationNotAllowed,
                  "void InferenceEngine< GUM_SCALAR "
                  ">::insertModals(const std::string & path) : "
                  "could not open input file : "
                     << path);
      }

      if (!modal_.empty()) modal_.clear();

      std::string line, tmp;
      char *      cstr, *p;

      while (mod_stream.good()) {
        getline(mod_stream, line);

        if (line.size() == 0) continue;

        cstr = new char[line.size() + 1];
        strcpy(cstr, line.c_str());

        p   = strtok(cstr, " ");
        tmp = p;

        std::vector< GUM_SCALAR > values;
        p = strtok(nullptr, " ");

        while (p != nullptr) {
          values.push_back(GUM_SCALAR(atof(p)));
          p = strtok(nullptr, " ");
        }   // end of : line

        modal_.insert(tmp, values);   //[tmp] = values;

        delete[] p;
        delete[] cstr;
      }   // end of : file

      mod_stream.close();

      initExpectations_();
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::insertModals(
       const std::map< std::string, std::vector< GUM_SCALAR > >& modals) {
      if (!modal_.empty()) modal_.clear();

      for (auto it = modals.cbegin(), theEnd = modals.cend(); it != theEnd; ++it) {
        NodeId id;

        try {
          id = credalNet_->current_bn().idFromName(it->first);
        } catch (NotFound& err) {
          GUM_SHOWERROR(err);
          continue;
        }

        // check that modals are net compatible
        auto dSize = credalNet_->current_bn().variable(id).domainSize();

        if (dSize != it->second.size()) continue;

        // GUM_ERROR(OperationNotAllowed, "void InferenceEngine< GUM_SCALAR
        // >::insertModals( const std::map< std::string, std::vector< GUM_SCALAR
        // > >
        // &modals) : modalities does not respect variable cardinality : " <<
        // credalNet_->current_bn().variable( id ).name() << " : " << dSize << "
        // != "
        // << it->second.size());

        modal_.insert(it->first, it->second);   //[ it->first ] = it->second;
      }

      //_modal = modals;

      initExpectations_();
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::insertEvidence(
       const std::map< std::string, std::vector< GUM_SCALAR > >& eviMap) {
      if (!evidence_.empty()) evidence_.clear();

      for (auto it = eviMap.cbegin(), theEnd = eviMap.cend(); it != theEnd; ++it) {
        NodeId id;

        try {
          id = credalNet_->current_bn().idFromName(it->first);
        } catch (NotFound& err) {
          GUM_SHOWERROR(err);
          continue;
        }

        evidence_.insert(id, it->second);
      }

      // forces the computation of the begin iterator to avoid subsequent data races
      // @TODO make HashTableConstIterator constructors thread safe
      evidence_.begin();
    }

    // check that observed variables DO exists in the network (otherwise Lazy
    // report
    // an error and app crash)
    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::insertEvidence(
       const NodeProperty< std::vector< GUM_SCALAR > >& evidence) {
      if (!evidence_.empty()) evidence_.clear();

      // use cbegin() to get const_iterator when available in aGrUM hashtables
      for (const auto& elt: evidence) {
        try {
          credalNet_->current_bn().variable(elt.first);
        } catch (NotFound& err) {
          GUM_SHOWERROR(err);
          continue;
        }

        evidence_.insert(elt.first, elt.second);
      }

      // forces the computation of the begin iterator to avoid subsequent data races
      // @TODO make HashTableConstIterator constructors thread safe
      evidence_.begin();
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::insertEvidenceFile(const std::string& path) {
      std::ifstream evi_stream(path.c_str(), std::ios::in);

      if (!evi_stream.good()) {
        GUM_ERROR(IOError,
                  "void InferenceEngine< GUM_SCALAR "
                  ">::insertEvidence(const std::string & path) : could not "
                  "open input file : "
                     << path);
      }

      if (!evidence_.empty()) evidence_.clear();

      std::string line, tmp;
      char *      cstr, *p;

      while (evi_stream.good() && std::strcmp(line.c_str(), "[EVIDENCE]") != 0) {
        getline(evi_stream, line);
      }

      while (evi_stream.good()) {
        getline(evi_stream, line);

        if (std::strcmp(line.c_str(), "[QUERY]") == 0) break;

        if (line.size() == 0) continue;

        cstr = new char[line.size() + 1];
        strcpy(cstr, line.c_str());

        p   = strtok(cstr, " ");
        tmp = p;

        // if user input is wrong
        NodeId node = -1;

        try {
          node = credalNet_->current_bn().idFromName(tmp);
        } catch (NotFound& err) {
          GUM_SHOWERROR(err);
          continue;
        }

        std::vector< GUM_SCALAR > values;
        p = strtok(nullptr, " ");

        while (p != nullptr) {
          values.push_back(GUM_SCALAR(atof(p)));
          p = strtok(nullptr, " ");
        }   // end of : line

        evidence_.insert(node, values);

        delete[] p;
        delete[] cstr;
      }   // end of : file

      evi_stream.close();

      // forces the computation of the begin iterator to avoid subsequent data races
      // @TODO make HashTableConstIterator constructors thread safe
      evidence_.begin();
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::insertQuery(
       const NodeProperty< std::vector< bool > >& query) {
      if (!query_.empty()) query_.clear();

      for (const auto& elt: query) {
        try {
          credalNet_->current_bn().variable(elt.first);
        } catch (NotFound& err) {
          GUM_SHOWERROR(err);
          continue;
        }

        query_.insert(elt.first, elt.second);
      }
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::insertQueryFile(const std::string& path) {
      std::ifstream evi_stream(path.c_str(), std::ios::in);

      if (!evi_stream.good()) {
        GUM_ERROR(IOError,
                  "void InferenceEngine< GUM_SCALAR >::insertQuery(const "
                  "std::string & path) : could not open input file : "
                     << path);
      }

      if (!query_.empty()) query_.clear();

      std::string line, tmp;
      char *      cstr, *p;

      while (evi_stream.good() && std::strcmp(line.c_str(), "[QUERY]") != 0) {
        getline(evi_stream, line);
      }

      while (evi_stream.good()) {
        getline(evi_stream, line);

        if (std::strcmp(line.c_str(), "[EVIDENCE]") == 0) break;

        if (line.size() == 0) continue;

        cstr = new char[line.size() + 1];
        strcpy(cstr, line.c_str());

        p   = strtok(cstr, " ");
        tmp = p;

        // if user input is wrong
        NodeId node = -1;

        try {
          node = credalNet_->current_bn().idFromName(tmp);
        } catch (NotFound& err) {
          GUM_SHOWERROR(err);
          continue;
        }

        auto dSize = credalNet_->current_bn().variable(node).domainSize();

        p = strtok(nullptr, " ");

        if (p == nullptr) {
          query_.insert(node, std::vector< bool >(dSize, true));
        } else {
          std::vector< bool > values(dSize, false);

          while (p != nullptr) {
            if ((Size)atoi(p) >= dSize)
              GUM_ERROR(OutOfBounds,
                        "void InferenceEngine< GUM_SCALAR "
                        ">::insertQuery(const std::string & path) : "
                        "query modality is higher or equal to "
                        "cardinality");

            values[atoi(p)] = true;
            p               = strtok(nullptr, " ");
          }   // end of : line

          query_.insert(node, values);
        }

        delete[] p;
        delete[] cstr;
      }   // end of : file

      evi_stream.close();
    }

    template < typename GUM_SCALAR >
    INLINE Potential< GUM_SCALAR >
           InferenceEngine< GUM_SCALAR >::marginalMin(const std::string& varName) const {
      return marginalMin(credalNet_->current_bn().idFromName(varName));
    }

    template < typename GUM_SCALAR >
    INLINE Potential< GUM_SCALAR >
           InferenceEngine< GUM_SCALAR >::marginalMax(const std::string& varName) const {
      return marginalMax(credalNet_->current_bn().idFromName(varName));
    }

    template < typename GUM_SCALAR >
    gum::Potential< GUM_SCALAR > InferenceEngine< GUM_SCALAR >::marginalMin(const NodeId id) const {
      try {
        Potential< GUM_SCALAR > res;
        res.add(credalNet_->current_bn().variable(id));
        res.fillWith(marginalMin_[id]);
        return res;
      } catch (NotFound& err) { throw(err); }
    }

    template < typename GUM_SCALAR >
    gum::Potential< GUM_SCALAR > InferenceEngine< GUM_SCALAR >::marginalMax(const NodeId id) const {
      try {
        Potential< GUM_SCALAR > res;
        res.add(credalNet_->current_bn().variable(id));
        res.fillWith(marginalMax_[id]);
        return res;
      } catch (NotFound& err) { throw(err); }
    }

    template < typename GUM_SCALAR >
    const GUM_SCALAR&
       InferenceEngine< GUM_SCALAR >::expectationMin(const std::string& varName) const {
      try {
        return expectationMin_[credalNet_->current_bn().idFromName(varName)];
      } catch (NotFound& err) { throw(err); }
    }

    template < typename GUM_SCALAR >
    const GUM_SCALAR&
       InferenceEngine< GUM_SCALAR >::expectationMax(const std::string& varName) const {
      try {
        return expectationMax_[credalNet_->current_bn().idFromName(varName)];
      } catch (NotFound& err) { throw(err); }
    }

    template < typename GUM_SCALAR >
    const GUM_SCALAR& InferenceEngine< GUM_SCALAR >::expectationMin(const NodeId id) const {
      try {
        return expectationMin_[id];
      } catch (NotFound& err) { throw(err); }
    }

    template < typename GUM_SCALAR >
    const GUM_SCALAR& InferenceEngine< GUM_SCALAR >::expectationMax(const NodeId id) const {
      try {
        return expectationMax_[id];
      } catch (NotFound& err) { throw(err); }
    }

    template < typename GUM_SCALAR >
    const std::vector< GUM_SCALAR >&
       InferenceEngine< GUM_SCALAR >::dynamicExpMin(const std::string& varName) const {
      std::string errTxt = "const std::vector< GUM_SCALAR > & InferenceEngine< "
                           "GUM_SCALAR >::dynamicExpMin ( const std::string & "
                           "varName ) const : ";

      if (dynamicExpMin_.empty())
        GUM_ERROR(OperationNotAllowed, errTxt + "_dynamicExpectations() needs to be called before")

      if (!dynamicExpMin_.exists(varName) /*dynamicExpMin_.find(varName) == dynamicExpMin_.end()*/)
        GUM_ERROR(NotFound, errTxt + "variable name not found : " << varName)

      return dynamicExpMin_[varName];
    }

    template < typename GUM_SCALAR >
    const std::vector< GUM_SCALAR >&
       InferenceEngine< GUM_SCALAR >::dynamicExpMax(const std::string& varName) const {
      std::string errTxt = "const std::vector< GUM_SCALAR > & InferenceEngine< "
                           "GUM_SCALAR >::dynamicExpMax ( const std::string & "
                           "varName ) const : ";

      if (dynamicExpMax_.empty())
        GUM_ERROR(OperationNotAllowed, errTxt + "_dynamicExpectations() needs to be called before")

      if (!dynamicExpMax_.exists(varName) /*dynamicExpMin_.find(varName) == dynamicExpMin_.end()*/)
        GUM_ERROR(NotFound, errTxt + "variable name not found : " << varName)

      return dynamicExpMax_[varName];
    }

    template < typename GUM_SCALAR >
    const std::vector< std::vector< GUM_SCALAR > >&
       InferenceEngine< GUM_SCALAR >::vertices(const NodeId id) const {
      return marginalSets_[id];
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::saveMarginals(const std::string& path) const {
      std::ofstream m_stream(path.c_str(), std::ios::out | std::ios::trunc);

      if (!m_stream.good()) {
        GUM_ERROR(IOError,
                  "void InferenceEngine< GUM_SCALAR >::saveMarginals(const "
                  "std::string & path) const : could not open output file "
                  ": "
                     << path);
      }

      for (const auto& elt: marginalMin_) {
        Size esize = Size(elt.second.size());

        for (Size mod = 0; mod < esize; mod++) {
          m_stream << credalNet_->current_bn().variable(elt.first).name() << " " << mod << " "
                   << (elt.second)[mod] << " " << marginalMax_[elt.first][mod] << std::endl;
        }
      }

      m_stream.close();
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::saveExpectations(const std::string& path) const {
      if (dynamicExpMin_.empty())   //_modal.empty())
        return;

      // else not here, to keep the const (natural with a saving process)
      // else if(dynamicExpMin_.empty() || dynamicExpMax_.empty())
      //_dynamicExpectations(); // works with or without a dynamic network

      std::ofstream m_stream(path.c_str(), std::ios::out | std::ios::trunc);

      if (!m_stream.good()) {
        GUM_ERROR(IOError,
                  "void InferenceEngine< GUM_SCALAR "
                  ">::saveExpectations(const std::string & path) : could "
                  "not open output file : "
                     << path);
      }

      for (const auto& elt: dynamicExpMin_) {
        m_stream << elt.first;   // it->first;

        // iterates over a vector
        for (const auto& elt2: elt.second) {
          m_stream << " " << elt2;
        }

        m_stream << std::endl;
      }

      for (const auto& elt: dynamicExpMax_) {
        m_stream << elt.first;

        // iterates over a vector
        for (const auto& elt2: elt.second) {
          m_stream << " " << elt2;
        }

        m_stream << std::endl;
      }

      m_stream.close();
    }

    template < typename GUM_SCALAR >
    std::string InferenceEngine< GUM_SCALAR >::toString() const {
      std::stringstream output;
      output << std::endl;

      // use cbegin() when available
      for (const auto& elt: marginalMin_) {
        Size esize = Size(elt.second.size());

        for (Size mod = 0; mod < esize; mod++) {
          output << "P(" << credalNet_->current_bn().variable(elt.first).name() << "=" << mod
                 << "|e) = [ ";
          output << marginalMin_[elt.first][mod] << ", " << marginalMax_[elt.first][mod] << " ]";

          if (!query_.empty())
            if (query_.exists(elt.first) && query_[elt.first][mod]) output << " QUERY";

          output << std::endl;
        }

        output << std::endl;
      }

      return output.str();
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::saveVertices(const std::string& path) const {
      std::ofstream m_stream(path.c_str(), std::ios::out | std::ios::trunc);

      if (!m_stream.good()) {
        GUM_ERROR(IOError,
                  "void InferenceEngine< GUM_SCALAR >::saveVertices(const "
                  "std::string & path) : could not open outpul file : "
                     << path);
      }

      for (const auto& elt: marginalSets_) {
        m_stream << credalNet_->current_bn().variable(elt.first).name() << std::endl;

        for (const auto& elt2: elt.second) {
          m_stream << "[";
          bool first = true;

          for (const auto& elt3: elt2) {
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

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::initMarginals_() {
      marginalMin_.clear();
      marginalMax_.clear();
      oldMarginalMin_.clear();
      oldMarginalMax_.clear();

      for (auto node: credalNet_->current_bn().nodes()) {
        auto dSize = credalNet_->current_bn().variable(node).domainSize();
        marginalMin_.insert(node, std::vector< GUM_SCALAR >(dSize, 1));
        oldMarginalMin_.insert(node, std::vector< GUM_SCALAR >(dSize, 1));

        marginalMax_.insert(node, std::vector< GUM_SCALAR >(dSize, 0));
        oldMarginalMax_.insert(node, std::vector< GUM_SCALAR >(dSize, 0));
      }

      // now that we know the sizes of marginalMin_ and marginalMax_, we can
      // dispatch their processes to the threads
      displatchMarginalsToThreads_();
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::initMarginalSets_() {
      marginalSets_.clear();

      if (!storeVertices_) return;

      for (auto node: credalNet_->current_bn().nodes())
        marginalSets_.insert(node, std::vector< std::vector< GUM_SCALAR > >());
    }

    // since only monitored variables in modal_ will be alble to compute
    // expectations, it is useless to initialize those for all variables
    // modal_ variables will always be checked further, so it is not necessary
    // to
    // check it here, but doing so will use less memory
    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::initExpectations_() {
      expectationMin_.clear();
      expectationMax_.clear();

      if (modal_.empty()) return;

      for (auto node: credalNet_->current_bn().nodes()) {
        std::string var_name, time_step;

        var_name   = credalNet_->current_bn().variable(node).name();
        auto delim = var_name.find_first_of("_");
        var_name   = var_name.substr(0, delim);

        if (!modal_.exists(var_name)) continue;

        expectationMin_.insert(node, modal_[var_name].back());
        expectationMax_.insert(node, modal_[var_name].front());
      }
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::dynamicExpectations() {
      dynamicExpectations_();
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::dynamicExpectations_() {
      // no modals, no expectations computed during inference
      if (expectationMin_.empty() || modal_.empty()) return;

      // already called by the algorithm or the user
      if (dynamicExpMax_.size() > 0 && dynamicExpMin_.size() > 0) return;

      using innerMap = typename gum::HashTable< int, GUM_SCALAR >;

      using outerMap = typename gum::HashTable< std::string, innerMap >;


      // if non dynamic, directly save expectationMin_ et Max (same but faster)
      outerMap expectationsMin, expectationsMax;

      for (const auto& elt: expectationMin_) {
        std::string var_name, time_step;

        var_name   = credalNet_->current_bn().variable(elt.first).name();
        auto delim = var_name.find_first_of("_");
        time_step  = var_name.substr(delim + 1, var_name.size());
        var_name   = var_name.substr(0, delim);

        // to be sure (don't store not monitored variables' expectations)
        // although it
        // should be taken care of before this point
        if (!modal_.exists(var_name)) continue;

        expectationsMin.getWithDefault(var_name, innerMap())
           .getWithDefault(atoi(time_step.c_str()), 0)
           = elt.second;   // we iterate with min iterators
        expectationsMax.getWithDefault(var_name, innerMap())
           .getWithDefault(atoi(time_step.c_str()), 0)
           = expectationMax_[elt.first];
      }

      for (const auto& elt: expectationsMin) {
        typename std::vector< GUM_SCALAR > dynExp(elt.second.size());

        for (const auto& elt2: elt.second)
          dynExp[elt2.first] = elt2.second;

        dynamicExpMin_.insert(elt.first, dynExp);
      }

      for (const auto& elt: expectationsMax) {
        typename std::vector< GUM_SCALAR > dynExp(elt.second.size());

        for (const auto& elt2: elt.second) {
          dynExp[elt2.first] = elt2.second;
        }

        dynamicExpMax_.insert(elt.first, dynExp);
      }
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::repetitiveInit_() {
      timeSteps_ = 0;
      t0_.clear();
      t1_.clear();

      // t = 0 vars belongs to t0_ as keys
      for (auto node: credalNet_->current_bn().dag().nodes()) {
        std::string var_name = credalNet_->current_bn().variable(node).name();
        auto        delim    = var_name.find_first_of("_");

        if (delim > var_name.size()) {
          GUM_ERROR(InvalidArgument,
                    "void InferenceEngine< GUM_SCALAR "
                    ">::repetitiveInit_() : the network does not "
                    "appear to be dynamic");
        }

        std::string time_step = var_name.substr(delim + 1, 1);

        if (time_step.compare("0") == 0) t0_.insert(node, std::vector< NodeId >());
      }

      // t = 1 vars belongs to either t0_ as member value or t1_ as keys
      for (const auto& node: credalNet_->current_bn().dag().nodes()) {
        std::string var_name  = credalNet_->current_bn().variable(node).name();
        auto        delim     = var_name.find_first_of("_");
        std::string time_step = var_name.substr(delim + 1, var_name.size());
        var_name              = var_name.substr(0, delim);
        delim                 = time_step.find_first_of("_");
        time_step             = time_step.substr(0, delim);

        if (time_step.compare("1") == 0) {
          bool found = false;

          for (const auto& elt: t0_) {
            std::string var_0_name = credalNet_->current_bn().variable(elt.first).name();
            delim                  = var_0_name.find_first_of("_");
            var_0_name             = var_0_name.substr(0, delim);

            if (var_name.compare(var_0_name) == 0) {
              const Potential< GUM_SCALAR >* potential(&credalNet_->current_bn().cpt(node));
              const Potential< GUM_SCALAR >* potential2(&credalNet_->current_bn().cpt(elt.first));

              if (potential->domainSize() == potential2->domainSize())
                t0_[elt.first].push_back(node);
              else t1_.insert(node, std::vector< NodeId >());

              found = true;
              break;
            }
          }

          if (!found) { t1_.insert(node, std::vector< NodeId >()); }
        }
      }

      // t > 1 vars belongs to either t0_ or t1_ as member value
      // remember timeSteps_
      for (auto node: credalNet_->current_bn().dag().nodes()) {
        std::string var_name  = credalNet_->current_bn().variable(node).name();
        auto        delim     = var_name.find_first_of("_");
        std::string time_step = var_name.substr(delim + 1, var_name.size());
        var_name              = var_name.substr(0, delim);
        delim                 = time_step.find_first_of("_");
        time_step             = time_step.substr(0, delim);

        if (time_step.compare("0") != 0 && time_step.compare("1") != 0) {
          // keep max time_step
          if (atoi(time_step.c_str()) > timeSteps_) timeSteps_ = atoi(time_step.c_str());

          std::string var_0_name;
          bool        found = false;

          for (const auto& elt: t0_) {
            std::string var_0_name = credalNet_->current_bn().variable(elt.first).name();
            delim                  = var_0_name.find_first_of("_");
            var_0_name             = var_0_name.substr(0, delim);

            if (var_name.compare(var_0_name) == 0) {
              const Potential< GUM_SCALAR >* potential(&credalNet_->current_bn().cpt(node));
              const Potential< GUM_SCALAR >* potential2(&credalNet_->current_bn().cpt(elt.first));

              if (potential->domainSize() == potential2->domainSize()) {
                t0_[elt.first].push_back(node);
                found = true;
                break;
              }
            }
          }

          if (!found) {
            for (const auto& elt: t1_) {
              std::string var_0_name = credalNet_->current_bn().variable(elt.first).name();
              auto        delim      = var_0_name.find_first_of("_");
              var_0_name             = var_0_name.substr(0, delim);

              if (var_name.compare(var_0_name) == 0) {
                const Potential< GUM_SCALAR >* potential(&credalNet_->current_bn().cpt(node));
                const Potential< GUM_SCALAR >* potential2(&credalNet_->current_bn().cpt(elt.first));

                if (potential->domainSize() == potential2->domainSize()) {
                  t1_[elt.first].push_back(node);
                  break;
                }
              }
            }
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    void
       InferenceEngine< GUM_SCALAR >::updateExpectations_(const NodeId&                    id,
                                                          const std::vector< GUM_SCALAR >& vertex) {
      std::string var_name = credalNet_->current_bn().variable(id).name();
      auto        delim    = var_name.find_first_of("_");

      var_name = var_name.substr(0, delim);

      if (modal_.exists(var_name) /*modal_.find(var_name) != modal_.end()*/) {
        GUM_SCALAR exp   = 0;
        auto       vsize = vertex.size();

        for (Size mod = 0; mod < vsize; mod++)
          exp += vertex[mod] * modal_[var_name][mod];

        if (exp > expectationMax_[id]) expectationMax_[id] = exp;

        if (exp < expectationMin_[id]) expectationMin_[id] = exp;
      }
    }

    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::updateCredalSets_(const NodeId&                    id,
                                                          const std::vector< GUM_SCALAR >& vertex,
                                                          const bool& elimRedund) {
      auto& nodeCredalSet = marginalSets_[id];
      auto  dsize         = vertex.size();

      bool eq = true;

      for (auto it = nodeCredalSet.cbegin(), itEnd = nodeCredalSet.cend(); it != itEnd; ++it) {
        eq = true;

        for (Size i = 0; i < dsize; i++) {
          if (std::fabs(vertex[i] - (*it)[i]) > 1e-6) {
            eq = false;
            break;
          }
        }

        if (eq) break;
      }

      if (!eq || nodeCredalSet.size() == 0) {
        nodeCredalSet.push_back(vertex);
        return;
      } else return;

      // because of next lambda return condition
      if (nodeCredalSet.size() == 1) return;

      // check that the point and all previously added ones are not inside the
      // actual
      // polytope
      auto itEnd
         = std::remove_if(nodeCredalSet.begin(),
                          nodeCredalSet.end(),
                          [&](const std::vector< GUM_SCALAR >& v) -> bool {
                            for (auto jt       = v.cbegin(),
                                      jtEnd    = v.cend(),
                                      minIt    = marginalMin_[id].cbegin(),
                                      minItEnd = marginalMin_[id].cend(),
                                      maxIt    = marginalMax_[id].cbegin(),
                                      maxItEnd = marginalMax_[id].cend();
                                 jt != jtEnd && minIt != minItEnd && maxIt != maxItEnd;
                                 ++jt, ++minIt, ++maxIt) {
                              if ((std::fabs(*jt - *minIt) < 1e-6 || std::fabs(*jt - *maxIt) < 1e-6)
                                  && std::fabs(*minIt - *maxIt) > 1e-6)
                                return false;
                            }
                            return true;
                          });

      nodeCredalSet.erase(itEnd, nodeCredalSet.end());

      // we need at least 2 points to make a convex combination
      if (!elimRedund || nodeCredalSet.size() <= 2) return;

      // there may be points not inside the polytope but on one of it's facet,
      // meaning it's still a convex combination of vertices of this facet. Here
      // we
      // need lrs.
      LRSWrapper< GUM_SCALAR > lrsWrapper;
      lrsWrapper.setUpV((unsigned int)dsize, (unsigned int)(nodeCredalSet.size()));

      for (const auto& vtx: nodeCredalSet)
        lrsWrapper.fillV(vtx);

      lrsWrapper.elimRedundVrep();

      marginalSets_[id] = lrsWrapper.getOutput();
    }

    template < typename GUM_SCALAR >
    const NodeProperty< std::vector< NodeId > >&
       InferenceEngine< GUM_SCALAR >::getT0Cluster() const {
      return t0_;
    }

    template < typename GUM_SCALAR >
    const NodeProperty< std::vector< NodeId > >&
       InferenceEngine< GUM_SCALAR >::getT1Cluster() const {
      return t1_;
    }

    template < typename GUM_SCALAR >
    inline const GUM_SCALAR InferenceEngine< GUM_SCALAR >::computeEpsilon_() {
      // compute the number of threads and prepare for the result
      const Size                nb_threads = ThreadExecutor::nbRunningThreadsExecutors() == 0
                                              ? this->threadRanges_.size() - 1
                                              : 1;   // no nested multithreading
      std::vector< GUM_SCALAR > tEps(nb_threads, std::numeric_limits< GUM_SCALAR >::max());

      // create the function to be executed by the threads
      auto threadedEps = [this, &tEps](const std::size_t                              this_thread,
                                       const std::size_t                              nb_threads,
                                       const std::vector< std::pair< NodeId, Idx > >& ranges) {
        auto&      this_tEps = tEps[this_thread];
        GUM_SCALAR delta;

        // below, we will loop over indices i and j of marginalMin_ and
        // marginalMax_. Index i represents nodes and j allow to parse their
        // domain. To parse all the domains of all the nodes, we should theorically
        // use 2 loops. However, here, we will use one loop: we start with node i
        // and parse its domain with index j. When this is done, we move to the
        // next node, and so on. The underlying idea is that, by doing so, we
        // need not parse in this function the whole domain of a node: we can start
        // the loop at a given value of node i and complete the loop on another
        // value of another node. These values are computed in Vector threadRanges_
        // by Method dispatchMarginalsToThreads_(), which dispatches the loops
        // among threads
        auto       i                = ranges[this_thread].first;
        auto       j                = ranges[this_thread].second;
        auto       domain_size      = this->marginalMax_[i].size();
        const auto end_i            = ranges[this_thread + 1].first;
        auto       end_j            = ranges[this_thread + 1].second;
        const auto marginalMax_size = this->marginalMax_.size();

        while ((i < end_i) || (j < end_j)) {
          // on min
          delta     = marginalMin_[i][j] - oldMarginalMin_[i][j];
          delta     = (delta < 0) ? (-delta) : delta;
          this_tEps = (this_tEps < delta) ? delta : this_tEps;

          // on max
          delta     = marginalMax_[i][j] - oldMarginalMax_[i][j];
          delta     = (delta < 0) ? (-delta) : delta;
          this_tEps = (this_tEps < delta) ? delta : this_tEps;

          oldMarginalMin_[i][j] = marginalMin_[i][j];
          oldMarginalMax_[i][j] = marginalMax_[i][j];

          if (++j == domain_size) {
            j = 0;
            ++i;
            if (i < marginalMax_size) domain_size = this->marginalMax_[i].size();
          }
        }
      };

      // launch the threads
      ThreadExecutor::execute(
         nb_threads,
         threadedEps,
         (nb_threads == 1)
            ? std::vector< std::pair< NodeId, Idx > >{{0, 0}, {this->marginalMin_.size(), 0}}
            : this->threadRanges_);

      // aggregate all the results
      GUM_SCALAR eps = tEps[0];
      for (const auto nb: tEps)
        if (eps < nb) eps = nb;

      return eps;
    }

    /*
      // old openMP code:
      GUM_SCALAR eps = 0;
      #pragma omp parallel
      {
        GUM_SCALAR tEps = 0;
        GUM_SCALAR delta;

        /// int tId = getThreadNumber();
        int nsize = int(marginalMin_.size());

        #pragma omp for

        for (int i = 0; i < nsize; i++) {
          auto dSize = marginalMin_[i].size();

          for (Size j = 0; j < dSize; j++) {
            // on min
            delta = marginalMin_[i][j] - oldMarginalMin_[i][j];
            delta = (delta < 0) ? (-delta) : delta;
            tEps  = (tEps < delta) ? delta : tEps;

            // on max
            delta = marginalMax_[i][j] - oldMarginalMax_[i][j];
            delta = (delta < 0) ? (-delta) : delta;
            tEps  = (tEps < delta) ? delta : tEps;

            oldMarginalMin_[i][j] = marginalMin_[i][j];
            oldMarginalMax_[i][j] = marginalMax_[i][j];
          }
        }   // end of : all variables

        #pragma omp critical(epsilon_max)
        {
        #pragma omp flush(eps)
          eps = (eps < tEps) ? tEps : eps;
        }
      }

      return eps;
    }
    */


    template < typename GUM_SCALAR >
    void InferenceEngine< GUM_SCALAR >::displatchMarginalsToThreads_() {
      // we compute the number of elements in the 2 loops (over i,j in marginalMin_[i][j])
      Size       nb_elements      = 0;
      const auto marginalMin_size = this->marginalMin_.size();
      for (const auto& marg_i: this->marginalMin_)
        nb_elements += marg_i.second.size();

      // distribute evenly the elements among the threads
      auto nb_threads = ThreadNumberManager::getNumberOfThreads();
      if (nb_elements < nb_threads) nb_threads = nb_elements;

      // the result that we return is a vector of pairs (NodeId, Idx). For thread number i, the
      // pair at index i is the beginning of the range that the thread will have to process: this
      // is the part of the marginal distribution vector of node NodeId starting at index Idx.
      // The pair at index i+1 is the end of this range (not included)
      threadRanges_.clear();
      threadRanges_.reserve(nb_threads + 1);

      // try to balance the number of elements among the threads
      Idx nb_elts_par_thread = nb_elements / nb_threads;
      Idx rest_elts          = nb_elements - nb_elts_par_thread * nb_threads;

      NodeId current_node         = 0;
      Idx    current_domain_index = 0;
      Size   current_domain_size  = this->marginalMin_[0].size();
      threadRanges_.emplace_back(current_node, current_domain_index);

      for (Idx i = Idx(0); i < nb_threads; ++i) {
        // compute the end of the threads, assuming that the current node has a domain
        // sufficiently large
        current_domain_index += nb_elts_par_thread;
        if (rest_elts != Idx(0)) {
          ++current_domain_index;
          --rest_elts;
        }

        // if the current node is not sufficient to hold all the elements that
        // the current thread should process. So we should add elements of the
        // next nodes
        while (current_domain_index >= current_domain_size) {
          current_domain_index -= current_domain_size;
          ++current_node;
          current_domain_index = 0;
          if (current_node != marginalMin_size) {
            current_domain_size = this->marginalMin_[current_node].size();
          }
        }

        // now we can store the range if elements
        threadRanges_.emplace_back(current_node, current_domain_index);

        // compute the next begin_node
        if (current_domain_index == current_domain_size) {
          ++current_node;
          current_domain_index = 0;
        }
      }
    }

  }   // namespace credal
}   // namespace gum
