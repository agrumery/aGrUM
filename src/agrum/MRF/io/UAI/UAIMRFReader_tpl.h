/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) et Christophe GONZALES(_at_AMU)
 * (_at_AMU) info_at_agrum_dot_org
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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  template < typename GUM_SCALAR >
  UAIMRFReader< GUM_SCALAR >::UAIMRFReader(MarkovRandomField< GUM_SCALAR >* MN,
                                           const std::string&               filename) :
      MRFReader< GUM_SCALAR >(MN, filename) {
    GUM_CONSTRUCTOR(UAIMRFReader);
    _mn_ = MN;
    _mn_->clear();

    _streamName_ = filename;
    _parseDone_  = false;

    _ioerror_ = false;

    try {
      _scanner_ = new UAIMRF::Scanner(_streamName_.c_str());
      _parser_  = new UAIMRF::Parser(_scanner_);
    } catch (IOError const&) { _ioerror_ = true; }
  }

  template < typename GUM_SCALAR >
  UAIMRFReader< GUM_SCALAR >::~UAIMRFReader() {
    GUM_DESTRUCTOR(UAIMRFReader);

    if (!_ioerror_) {
      // this could lead to memory leak !!
      if (_parser_) delete (_parser_);

      if (_scanner_) delete (_scanner_);
    }
  }

  template < typename GUM_SCALAR >
  INLINE UAIMRF::Scanner& UAIMRFReader< GUM_SCALAR >::scanner() {
    if (_ioerror_) { GUM_ERROR(gum::IOError, "No such file " + streamName()) }

    return *_scanner_;
  }

  template < typename GUM_SCALAR >
  INLINE const std::string& UAIMRFReader< GUM_SCALAR >::streamName() const {
    return _streamName_;
  }

  template < typename GUM_SCALAR >
  INLINE bool UAIMRFReader< GUM_SCALAR >::trace() const {
    return _traceScanning_;
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMRFReader< GUM_SCALAR >::trace(bool b) {
    _traceScanning_ = b;
    scanner().setTrace(b);
  }

  template < typename GUM_SCALAR >
  Size UAIMRFReader< GUM_SCALAR >::proceed() {
    if (_ioerror_) { GUM_ERROR(gum::IOError, "No such file " + streamName()) }

    if (!_parseDone_) {
      try {
        _parser_->Parse();
        _parseDone_ = true;
        buildFromQuartets(_parser_->getQuartets());
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        return 1 + _parser_->errors().error_count;
      }
    }

    return (_parser_->errors().error_count);
  }

  template < typename GUM_SCALAR >
  void UAIMRFReader< GUM_SCALAR >::buildFromQuartets(
     std::vector< std::tuple< float, int, int, int > > quartets) {
    Idx  current;
    Size max = quartets.size();
    if (max == 0) {
      _addWarning_(1, 1, "Empty MarkovRandomField");
      return;
    }

    auto isInt = [&]() -> bool { return (std::get< 0 >(quartets[current]) == -1); };
    auto lig   = [&]() -> int { return std::get< 2 >(quartets[current]); };
    auto col   = [&]() -> int { return std::get< 3 >(quartets[current]); };

    auto getInt = [&]() -> int {
      if (!isInt()) this->_addFatalError_(lig(), col(), "int expected");
      return std::get< 1 >(quartets[current]);
    };
    auto getVal = [&]() -> GUM_SCALAR {
      return (isInt()) ? (std::get< 1 >(quartets[current])) : (std::get< 0 >(quartets[current]));
    };
    auto incCurrent = [&]() {
      current += 1;
      if (current >= max) this->_addFatalError_(lig(), col(), "Not enough data in UAI file");
    };

    current      = 0;
    Size nbrNode = (Size)getInt();

    for (NodeId i = 0; i < nbrNode; i++) {
      incCurrent();
      int mod = getInt();
      if (mod < 2) _addError_(lig(), col(), "Number of modalities should be greater than 2.");
      _mn_->add(gum::LabelizedVariable(std::to_string(i), "", mod));
    }

    incCurrent();
    Size nbrFactors = (Size)getInt();

    std::vector< NodeSet > clicks;
    for (NodeId i = 0; i < nbrFactors; i++) {
      incCurrent();
      Size nbrVar = (Size)getInt();
      if (nbrVar == 0) _addError_(lig(), col(), "0 is not possible here");

      NodeSet vars;
      for (NodeId j = 0; j < nbrVar; j++) {
        incCurrent();
        NodeId nod = (NodeId)getInt();
        if (nod >= nbrNode)
          _addError_(lig(), col(), "Not enough variables in the MarkovRandomField");
        vars.insert(nod);
      }
      _mn_->addFactor(vars);
      clicks.push_back(vars);
    }

    for (NodeId i = 0; i < nbrFactors; i++) {
      incCurrent();
      Size nbrParam = (Size)getInt();
      if (nbrParam != _mn_->factor(clicks[i]).domainSize()) {
        _addFatalError_(lig(), col(), "Size does not fit between clique and parameters");
      }
      std::vector< GUM_SCALAR > v;
      for (Idx j = 0; j < nbrParam; j++) {
        incCurrent();
        v.push_back(getVal());
      }
      _mn_->factor(clicks[i]).fillWith(v);
      v.clear();
    }

    if (current != max - 1) _addError_(lig(), col(), "Too many data in this file");
  }

  // @{
  // publishing Errors API
  template < typename GUM_SCALAR >
  INLINE Idx UAIMRFReader< GUM_SCALAR >::errLine(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).line;
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE Idx UAIMRFReader< GUM_SCALAR >::errCol(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).column;
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE bool UAIMRFReader< GUM_SCALAR >::errIsError(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).is_error;
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE std::string UAIMRFReader< GUM_SCALAR >::errMsg(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).msg;
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMRFReader< GUM_SCALAR >::showElegantErrors(std::ostream& o) {
    if (_parseDone_) _parser_->errors().elegantErrors(o);
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMRFReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& o) {
    if (_parseDone_) _parser_->errors().elegantErrorsAndWarnings(o);
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMRFReader< GUM_SCALAR >::showErrorsAndWarnings(std::ostream& o) {
    if (_parseDone_) _parser_->errors().simpleErrorsAndWarnings(o);
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMRFReader< GUM_SCALAR >::showErrorCounts(std::ostream& o) {
    if (_parseDone_) _parser_->errors().syntheticResults(o);
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE Size UAIMRFReader< GUM_SCALAR >::errors() {
    return (!_parseDone_) ? (Size)0 : _parser_->errors().error_count;
  }

  template < typename GUM_SCALAR >
  INLINE Size UAIMRFReader< GUM_SCALAR >::warnings() {
    return (!_parseDone_) ? (Size)0 : _parser_->errors().warning_count;
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMRFReader< GUM_SCALAR >::_addFatalError_(Idx lig, Idx col, const std::string& s) {
    _parser_->errors().addError(s, _streamName_, lig, col);
    GUM_ERROR(gum::OperationNotAllowed, s)
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMRFReader< GUM_SCALAR >::_addError_(Idx lig, Idx col, const std::string& s) {
    _parser_->errors().addError(s, _streamName_, lig, col);
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMRFReader< GUM_SCALAR >::_addWarning_(Idx lig, Idx col, const std::string& s) {
    _parser_->errors().addWarning(s, _streamName_, lig, col);
  }

  // @}
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
