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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  template < typename GUM_SCALAR >
  UAIBNReader< GUM_SCALAR >::UAIBNReader(BayesNet< GUM_SCALAR >* bn, const std::string& filename) :
      BNReader< GUM_SCALAR >(bn, filename) {
    GUM_CONSTRUCTOR(UAIBNReader);
    _bn_         = bn;
    _streamName_ = filename;
    _parseDone_  = false;

    _ioerror_ = false;

    try {
      _scanner_ = new UAIBN::Scanner(_streamName_.c_str());
      _parser_  = new UAIBN::Parser(_scanner_);
    } catch (IOError const&) { _ioerror_ = true; }
  }

  template < typename GUM_SCALAR >
  UAIBNReader< GUM_SCALAR >::~UAIBNReader() {
    GUM_DESTRUCTOR(UAIBNReader);

    if (!_ioerror_) {
      // this could lead to memory leak !!
      if (_parser_) delete (_parser_);

      if (_scanner_) delete (_scanner_);
    }
  }

  template < typename GUM_SCALAR >
  INLINE UAIBN::Scanner& UAIBNReader< GUM_SCALAR >::scanner() {
    if (_ioerror_) { GUM_ERROR(gum::IOError, "No such file " + streamName()) }

    return *_scanner_;
  }

  template < typename GUM_SCALAR >
  INLINE const std::string& UAIBNReader< GUM_SCALAR >::streamName() const {
    return _streamName_;
  }

  template < typename GUM_SCALAR >
  INLINE bool UAIBNReader< GUM_SCALAR >::trace() const {
    return _traceScanning_;
  }

  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::trace(bool b) {
    _traceScanning_ = b;
    scanner().setTrace(b);
  }

  template < typename GUM_SCALAR >
  Size UAIBNReader< GUM_SCALAR >::proceed() {
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
  void UAIBNReader< GUM_SCALAR >::buildFromQuartets(
     std::vector< std::tuple< float, int, int, int > > quartets) {
    Idx  current;
    Size max = quartets.size();
    if (max == 0) {
      _addWarning_(1, 1, "Empty BayesNet");
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
      _bn_->add(gum::LabelizedVariable(std::to_string(i), "", mod));
    }

    incCurrent();
    Size nbrPot = (Size)getInt();
    if (nbrPot != nbrNode)
      _addWarning_(lig(), col(), "Number of CPTs should be the same as number of nodes");

    Set< NodeId > s;
    for (NodeId i = 0; i < nbrPot; i++) {
      incCurrent();
      Size nbrPar = (Size)getInt();
      if (nbrPar == 0) _addError_(lig(), col(), "0 is not possible here");

      std::vector< NodeId > papas;
      for (NodeId j = 1; j < nbrPar; j++) {
        incCurrent();
        NodeId papa = (NodeId)getInt();
        if (papa >= nbrNode) _addError_(lig(), col(), "Not enough variables in the BayesNet");
        papas.push_back(papa);
      }

      incCurrent();
      NodeId nodePot = (Size)getInt();
      if (nodePot >= nbrNode) _addError_(lig(), col(), "Not enough variables in the BayesNet");
      if (s.contains(nodePot)) _addError_(lig(), col(), "Parents already defined");
      s.insert(nodePot);

      for (const auto papa: papas) {
        _bn_->addArc(papa, nodePot);
      }
    }

    std::vector< GUM_SCALAR > v;
    for (NodeId i = 0; i < nbrPot; i++) {
      incCurrent();
      Size nbrParam = (Size)getInt();
      if (nbrParam != _bn_->cpt(i).domainSize())
        _addFatalError_(lig(), col(), "Size does not fit between parents and parameters");
      for (Idx j = 0; j < nbrParam; j++) {
        incCurrent();
        v.push_back(getVal());
      }
      _bn_->cpt(i).fillWith(v);
      v.clear();
    }

    if (current != max - 1) _addError_(lig(), col(), "Too many data in this file");
  }

  // @{
  // publishing Errors API
  template < typename GUM_SCALAR >
  INLINE Idx UAIBNReader< GUM_SCALAR >::errLine(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).line;
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE Idx UAIBNReader< GUM_SCALAR >::errCol(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).column;
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE bool UAIBNReader< GUM_SCALAR >::errIsError(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).is_error;
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE std::string UAIBNReader< GUM_SCALAR >::errMsg(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).msg;
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::showElegantErrors(std::ostream& o) {
    if (_parseDone_) _parser_->errors().elegantErrors(o);
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& o) {
    if (_parseDone_) _parser_->errors().elegantErrorsAndWarnings(o);
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::showErrorsAndWarnings(std::ostream& o) {
    if (_parseDone_) _parser_->errors().simpleErrorsAndWarnings(o);
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::showErrorCounts(std::ostream& o) {
    if (_parseDone_) _parser_->errors().syntheticResults(o);
    else { GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE Size UAIBNReader< GUM_SCALAR >::errors() {
    return (!_parseDone_) ? (Size)0 : _parser_->errors().error_count;
  }

  template < typename GUM_SCALAR >
  INLINE Size UAIBNReader< GUM_SCALAR >::warnings() {
    return (!_parseDone_) ? (Size)0 : _parser_->errors().warning_count;
  }

  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::_addFatalError_(Idx lig, Idx col, const std::string& s) {
    _parser_->errors().addError(s, _streamName_, lig, col);
    GUM_ERROR(gum::OperationNotAllowed, "")
  }
  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::_addError_(Idx lig, Idx col, const std::string& s) {
    _parser_->errors().addError(s, _streamName_, lig, col);
  }
  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::_addWarning_(Idx lig, Idx col, const std::string& s) {
    _parser_->errors().addWarning(s, _streamName_, lig, col);
  }

  // @}
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
