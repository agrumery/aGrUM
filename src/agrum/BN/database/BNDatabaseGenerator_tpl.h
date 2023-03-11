/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief
 *
 * @author Santiago Cortijo and Pierre-Henri Wuillemin
 */

#include <agrum/BN/database/BNDatabaseGenerator.h>

#include <agrum/tools/core/timer.h>


namespace gum::learning {


  /// default constructor
  template < typename GUM_SCALAR >
  BNDatabaseGenerator< GUM_SCALAR >::BNDatabaseGenerator(const BayesNet< GUM_SCALAR >& bn) :
      _bn_(bn) {
    GUM_CONSTRUCTOR(BNDatabaseGenerator)

    // get the node names => they will serve as ids
    NodeId id = 0;
    for (const auto& var: _bn_.dag()) {
      auto name = _bn_.variable(var).name();
      _names2ids_.insert(name, var);
      ++id;
    }
    _nbVars_ = id;
    _varOrder_.resize(_nbVars_);
    std::iota(_varOrder_.begin(), _varOrder_.end(), (Idx)0);
  }

  /// destructor
  template < typename GUM_SCALAR >
  BNDatabaseGenerator< GUM_SCALAR >::~BNDatabaseGenerator() {
    GUM_DESTRUCTOR(BNDatabaseGenerator)
  }


  /// draw instances from  _bn_
  template < typename GUM_SCALAR >
  double BNDatabaseGenerator< GUM_SCALAR >::drawSamples(Size nbSamples) {
    Instantiation inst;
    return drawSamples(nbSamples, inst);
  }

  /// draw instances from  _bn_
  template < typename GUM_SCALAR >
  double BNDatabaseGenerator< GUM_SCALAR >::drawSamples(Size nbSamples, const Instantiation& evs) {
    int progress = 0;

    gum::Timer timer;
    timer.reset();

    if (onProgress.hasListener()) { GUM_EMIT2(onProgress, progress, timer.step()); }

    _database_.clear();
    _database_.resize(nbSamples);
    for (auto& row: _database_) {
      row.resize(_nbVars_);
    }
    // get the order in which the nodes will be sampled
    const auto         topOrder = _bn_.topologicalOrder();
    gum::Instantiation particule;

    // create instantiations in advance
    for (NodeId node = 0; node < _nbVars_; ++node)
      particule.add(_bn_.variable(node));

    // create the random generator
    std::uniform_real_distribution distro(0.0, 1.0);

    // perform the sampling
    _log2likelihood_ = 0;
    Idx idSample     = 0;
    for (Idx step = 0; step < nbSamples; ++step) {
      if (onProgress.hasListener()) {
        auto p = int((step * 100) / nbSamples);
        if (p != progress) {
          progress = p;
          GUM_EMIT2(onProgress, progress, timer.step());
        }
      }
      std::vector< Idx >& sample = _database_.at(idSample);
      bool                reject = false;
      for (Idx rank = 0; rank < _nbVars_; ++rank) {
        const NodeId node = topOrder[rank];
        const auto&  var  = _bn_.variable(node);
        const auto&  cpt  = _bn_.cpt(node);

        const double nb    = gum::randomProba();
        double       cumul = 0.0;
        for (particule.setFirstVar(var); !particule.end(); particule.incVar(var)) {
          cumul += cpt[particule];
          if (cumul >= nb) break;
        }
        if (particule.end()) particule.setLastVar(var);

        if ((!evs.empty()) && evs.contains(var) && (evs.val(var) != particule.val(var))) {
          reject = true;
          break;
        }

        sample.at(node) = particule.val(var);
        _log2likelihood_ += std::log2(_bn_.cpt(node)[particule]);
      }
      if (reject) { continue; }

      idSample++;
    }

    _drawnSamples_ = true;

    if (onProgress.hasListener()) {
      std::stringstream ss;
      ss << "Database of size " << idSample << "(" << nbSamples << ") generated in " << timer.step()
         << " seconds. Log2likelihood : " << _log2likelihood_;
      GUM_EMIT1(onStop, ss.str());
    }

    if (nbSamples > idSample) _database_.resize(idSample);

    return _log2likelihood_;
  }

  template < typename GUM_SCALAR >
  INLINE Size BNDatabaseGenerator< GUM_SCALAR >::samplesNbRows() const {
    if (!_drawnSamples_) { GUM_ERROR(OperationNotAllowed, "drawSamples() must be called first.") }

    return _database_.size();
  }
  template < typename GUM_SCALAR >
  INLINE Size BNDatabaseGenerator< GUM_SCALAR >::samplesNbCols() const {
    if (!_drawnSamples_) { GUM_ERROR(OperationNotAllowed, "drawSamples() must be called first.") }

    return _nbVars_;
  }

  template < typename GUM_SCALAR >
  INLINE Idx BNDatabaseGenerator< GUM_SCALAR >::samplesAt(Idx row, Idx col) const {
    if (!_drawnSamples_) { GUM_ERROR(OperationNotAllowed, "drawSamples() must be called first.") }
    return _database_.at(row).at(_varOrder_.at(col));
  }

  template < typename GUM_SCALAR >
  INLINE std::string BNDatabaseGenerator< GUM_SCALAR >::samplesLabelAt(Idx row, Idx col) const {
    if (!_drawnSamples_) { GUM_ERROR(OperationNotAllowed, "drawSamples() must be called first.") }
    return _bn_.variable(_varOrder_.at(col)).label(_database_.at(row).at(_varOrder_.at(col)));
  }

  /// generates database, and writes csv file
  template < typename GUM_SCALAR >
  void BNDatabaseGenerator< GUM_SCALAR >::toCSV(const std::string& csvFileURL,
                                                bool               useLabels,
                                                bool               append,
                                                std::string        csvSeparator,
                                                bool               checkOnAppend) const {
    if (!_drawnSamples_) { GUM_ERROR(OperationNotAllowed, "drawSamples() must be called first.") }

    if (csvSeparator.find("\n") != std::string::npos) {
      GUM_ERROR(InvalidArgument, "csvSeparator must not contain end-line characters")
    }

    bool includeHeader = true;
    if (append) {
      std::ifstream csvFile(csvFileURL);
      if (csvFile) {
        if (auto varOrder = _varOrderFromCSV_(csvFile, csvSeparator);
            checkOnAppend && varOrder != _varOrder_)
          GUM_ERROR(OperationNotAllowed,
                    "Inconsistent variable order in csvFile when appending. You "
                    "can use setVarOrderFromCSV(url) function to get the right "
                    "order. You could also set parameter checkOnAppend=false if you "
                    "know what you are doing.")
        includeHeader = false;
      }
      csvFile.close();
    }


    auto ofstreamFlag = append ? std::ofstream::app : std::ofstream::out;

    std::ofstream os(csvFileURL, ofstreamFlag);
    bool          firstCol = true;
    if (includeHeader) {
      for (const auto& i: _varOrder_) {
        if (firstCol) {
          firstCol = false;
        } else {
          os << csvSeparator;
        }
        os << _bn_.variable(i).name();
      }
    }
    os << std::endl;

    bool firstRow = true;
    for (const auto& row: _database_) {
      if (firstRow) {
        firstRow = false;
      } else {
        os << std::endl;
      }
      firstCol = true;
      for (const auto& i: _varOrder_) {
        if (firstCol) {
          firstCol = false;
        } else {
          os << csvSeparator;
        }
        if (useLabels) {
          os << _bn_.variable(i).label(row.at(i));
        } else {
          os << row[i];
        }
      }
    }

    os.close();
  }

  /// generates a DatabaseVectInRAM
  template < typename GUM_SCALAR >
  DatabaseTable BNDatabaseGenerator< GUM_SCALAR >::toDatabaseTable(bool useLabels) const {
    if (!_drawnSamples_) GUM_ERROR(OperationNotAllowed, "proceed() must be called first.")

    DatabaseTable              db;
    std::vector< std::string > varNames;
    varNames.reserve(_nbVars_);
    for (const auto& i: _varOrder_) {
      varNames.push_back(_names2ids_.first(i));
    }

    // create the translators
    for (std::size_t i = 0; i < _nbVars_; ++i) {
      const Variable& var = _bn_.variable(_varOrder_[i]);
      db.insertTranslator(var, i);
    }

    if (useLabels) {
      std::vector< std::string > xrow(_nbVars_);
      for (const auto& row: _database_) {
        for (Idx i = 0; i < _nbVars_; ++i) {
          Idx j   = _varOrder_.at(i);
          xrow[i] = _bn_.variable(j).label(row.at(j));
        }
        db.insertRow(xrow);
      }
    } else {
      std::vector< DBTranslatedValueType > translatorType(_nbVars_);
      for (std::size_t i = 0; i < _nbVars_; ++i) {
        translatorType[i] = db.translator(i).getValType();
      }
      DBRow< DBTranslatedValue > xrow(_nbVars_);
      const auto                 xmiss = gum::learning::DatabaseTable::IsMissing::False;
      for (const auto& row: _database_) {
        for (Idx i = 0; i < _nbVars_; ++i) {
          Idx j = _varOrder_.at(i);

          if (translatorType[i] == DBTranslatedValueType::DISCRETE)
            xrow[i].discr_val = std::size_t(row.at(j));
          else xrow[i].cont_val = float(row.at(j));
        }
      }
      db.insertRow(xrow, xmiss);
    }

    return db;
  }


  /// returns database using specified data order
  template < typename GUM_SCALAR >
  std::vector< std::vector< Idx > > BNDatabaseGenerator< GUM_SCALAR >::database() const {
    if (!_drawnSamples_) GUM_ERROR(OperationNotAllowed, "drawSamples() must be called first.")

    auto db(_database_);
    for (Idx i = 0; i < _database_.size(); ++i) {
      for (Idx j = 0; j < _nbVars_; ++j) {
        db.at(i).at(j) = (Idx)_database_.at(i).at(_varOrder_.at(j));
      }
    }
    return db;
  }

  /// change columns order
  template < typename GUM_SCALAR >
  void BNDatabaseGenerator< GUM_SCALAR >::setVarOrder(const std::vector< Idx >& varOrder) {
    if (varOrder.size() != _nbVars_)
      GUM_ERROR(FatalError, "varOrder's size must be equal to the number of variables")

    std::vector< bool > usedVars(_nbVars_, false);
    for (const auto& i: varOrder) {
      if (i >= _nbVars_) GUM_ERROR(FatalError, "varOrder contains invalid variables")
      if (usedVars.at(i)) GUM_ERROR(FatalError, "varOrder must not have repeated variables")
      usedVars.at(i) = true;
    }

    if (std::find(usedVars.begin(), usedVars.end(), false) != usedVars.end()) {
      GUM_ERROR(FatalError, "varOrder must contain all variables")
    }

    _varOrder_ = varOrder;
  }

  /// change columns order using variable names
  template < typename GUM_SCALAR >
  void BNDatabaseGenerator< GUM_SCALAR >::setVarOrder(const std::vector< std::string >& varOrder) {
    std::vector< Idx > varOrderIdx;
    varOrderIdx.reserve(varOrder.size());
    for (const auto& vname: varOrder) {
      varOrderIdx.push_back(_names2ids_.second(vname));
    }
    setVarOrder(varOrderIdx);
  }

  /// change columns order according to a csv file
  template < typename GUM_SCALAR >
  void BNDatabaseGenerator< GUM_SCALAR >::setVarOrderFromCSV(const std::string& csvFileURL,
                                                             const std::string& csvSeparator) {
    setVarOrder(_varOrderFromCSV_(csvFileURL, csvSeparator));
  }

  /// set columns in Topological order
  template < typename GUM_SCALAR >
  void BNDatabaseGenerator< GUM_SCALAR >::setTopologicalVarOrder() {
    std::vector< Idx > varOrder;
    varOrder.reserve(_nbVars_);
    for (const auto& v: _bn_.topologicalOrder()) {
      varOrder.push_back(v);
    }
    setVarOrder(varOrder);
  }

  /// set columns in antiTopological order
  template < typename GUM_SCALAR >
  void BNDatabaseGenerator< GUM_SCALAR >::setAntiTopologicalVarOrder() {
    std::vector< Idx > varOrder;
    varOrder.reserve(_nbVars_);
    for (const auto& v: _bn_.topologicalOrder()) {
      varOrder.push_back(v);
    }
    std::reverse(varOrder.begin(), varOrder.end());
    setVarOrder(varOrder);
  }

  /// set columns in random order
  template < typename GUM_SCALAR >
  void BNDatabaseGenerator< GUM_SCALAR >::setRandomVarOrder() {
    std::vector< std::string > varOrder;
    varOrder.reserve(_bn_.size());
    for (const auto& var: _bn_.dag()) {
      varOrder.push_back(_bn_.variable(var).name());
    }
    std::shuffle(varOrder.begin(), varOrder.end(), gum::randomGenerator());
    setVarOrder(varOrder);
  }


  /// returns variable order indexes
  template < typename GUM_SCALAR >
  std::vector< Idx > BNDatabaseGenerator< GUM_SCALAR >::varOrder() const {
    return _varOrder_;
  }

  /// returns variable order.
  template < typename GUM_SCALAR >
  std::vector< std::string > BNDatabaseGenerator< GUM_SCALAR >::varOrderNames() const {
    std::vector< std::string > varNames;
    varNames.reserve(_nbVars_);
    for (const auto& i: _varOrder_) {
      varNames.push_back(_names2ids_.first(i));
    }

    return varNames;
  }

  /// returns log2Likelihood of generated samples
  template < typename GUM_SCALAR >
  double BNDatabaseGenerator< GUM_SCALAR >::log2likelihood() const {
    if (!_drawnSamples_) { GUM_ERROR(OperationNotAllowed, "drawSamples() must be called first.") }
    return _log2likelihood_;
  }

  /// returns varOrder from a csv file
  template < typename GUM_SCALAR >
  std::vector< Idx >
     BNDatabaseGenerator< GUM_SCALAR >::_varOrderFromCSV_(const std::string& csvFileURL,
                                                          const std::string& csvSeparator) const {
    std::ifstream      csvFile(csvFileURL);
    std::vector< Idx > varOrder;
    if (csvFile) {
      varOrder = _varOrderFromCSV_(csvFile, csvSeparator);
      csvFile.close();
    } else {
      GUM_ERROR(NotFound, "csvFileURL does not exist")
    }

    return varOrder;
  }

  /// returns varOrder from a csv file
  template < typename GUM_SCALAR >
  std::vector< Idx >
     BNDatabaseGenerator< GUM_SCALAR >::_varOrderFromCSV_(std::ifstream&     csvFile,
                                                          const std::string& csvSeparator) const {
    std::string                line;
    std::vector< std::string > header_found;
    header_found.reserve(_nbVars_);
    while (std::getline(csvFile, line)) {
      std::size_t i   = 0;
      auto        pos = line.find(csvSeparator);
      while (pos != std::string::npos) {
        header_found.push_back(line.substr(i, pos - i));
        pos += csvSeparator.length();
        i   = pos;
        pos = line.find(csvSeparator, pos);

        if (pos == std::string::npos) header_found.push_back(line.substr(i, line.length()));
      }
      break;
    }

    std::vector< Size > varOrder;
    varOrder.reserve(_nbVars_);

    for (const auto& hf: header_found) {
      varOrder.push_back(_names2ids_.second(hf));
    }

    return varOrder;
  }
}   // namespace gum::learning