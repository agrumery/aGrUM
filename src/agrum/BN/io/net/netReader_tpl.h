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

#  include <agrum/BN/io/net/netReader.h>

namespace gum {

  template < typename GUM_SCALAR >
  NetReader< GUM_SCALAR >::NetReader(BayesNet< GUM_SCALAR >* bn, const std::string& filename) :
      BNReader< GUM_SCALAR >(bn, filename) {
    GUM_CONSTRUCTOR(NetReader);
    _bn_         = bn;
    _streamName_ = filename;
    _parseDone_  = false;

    _factory_ = new BayesNetFactory< GUM_SCALAR >(_bn_);

    _ioerror_ = false;

    try {
      _scanner_ = new net::Scanner(_streamName_.c_str());
      _parser_  = new net::Parser(_scanner_);
      _parser_->setFactory((IBayesNetFactory*)_factory_);
    } catch (IOError const&) { _ioerror_ = true; }
  }

  template < typename GUM_SCALAR >
  NetReader< GUM_SCALAR >::~NetReader() {
    GUM_DESTRUCTOR(NetReader);

    if (!_ioerror_) {
      // this could lead to memory leak !!
      if (_parser_) delete (_parser_);

      if (_scanner_) delete (_scanner_);
    }

    if (_factory_) delete (_factory_);
  }

  template < typename GUM_SCALAR >
  INLINE net::Scanner& NetReader< GUM_SCALAR >::scanner() {
    if (_ioerror_) { GUM_ERROR(gum::IOError, "No such file " + streamName()) }

    return *_scanner_;
  }

  template < typename GUM_SCALAR >
  INLINE const std::string& NetReader< GUM_SCALAR >::streamName() const {
    return _streamName_;
  }

  template < typename GUM_SCALAR >
  INLINE bool NetReader< GUM_SCALAR >::trace() const {
    return _traceScanning_;
  }

  template < typename GUM_SCALAR >
  INLINE void NetReader< GUM_SCALAR >::trace(bool b) {
    _traceScanning_ = b;
    scanner().setTrace(b);
  }

  template < typename GUM_SCALAR >
  Size NetReader< GUM_SCALAR >::proceed() {
    if (_ioerror_) { GUM_ERROR(gum::IOError, "No such file " + streamName()) }

    if (!_parseDone_) {
      try {
        _parser_->Parse();
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        return 1 + _parser_->errors().error_count;
      }

      _parseDone_ = true;
    }

    return (_parser_->errors().error_count);
  }

  // @{
  // publishing Errors API
  template < typename GUM_SCALAR >
  INLINE Idx NetReader< GUM_SCALAR >::errLine(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).line;
    else { GUM_ERROR(OperationNotAllowed, "Net file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE Idx NetReader< GUM_SCALAR >::errCol(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).column;
    else { GUM_ERROR(OperationNotAllowed, "Net file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE bool NetReader< GUM_SCALAR >::errIsError(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).is_error;
    else { GUM_ERROR(OperationNotAllowed, "Net file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE std::string NetReader< GUM_SCALAR >::errMsg(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).msg;
    else { GUM_ERROR(OperationNotAllowed, "Net file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void NetReader< GUM_SCALAR >::showElegantErrors(std::ostream& o) {
    if (_parseDone_) _parser_->errors().elegantErrors(o);
    else { GUM_ERROR(OperationNotAllowed, "Net file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void NetReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& o) {
    if (_parseDone_) _parser_->errors().elegantErrorsAndWarnings(o);
    else { GUM_ERROR(OperationNotAllowed, "Net file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void NetReader< GUM_SCALAR >::showErrorsAndWarnings(std::ostream& o) {
    if (_parseDone_) _parser_->errors().simpleErrorsAndWarnings(o);
    else { GUM_ERROR(OperationNotAllowed, "Net file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE void NetReader< GUM_SCALAR >::showErrorCounts(std::ostream& o) {
    if (_parseDone_) _parser_->errors().syntheticResults(o);
    else { GUM_ERROR(OperationNotAllowed, "Net file not parsed yet") }
  }

  template < typename GUM_SCALAR >
  INLINE Size NetReader< GUM_SCALAR >::errors() {
    return (!_parseDone_) ? (Size)0 : _parser_->errors().error_count;
  }

  template < typename GUM_SCALAR >
  INLINE Size NetReader< GUM_SCALAR >::warnings() {
    return (!_parseDone_) ? (Size)0 : _parser_->errors().warning_count;
  }

  // @}
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
