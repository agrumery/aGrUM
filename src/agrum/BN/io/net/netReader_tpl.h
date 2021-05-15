/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
  NetReader< GUM_SCALAR >::NetReader(BayesNet< GUM_SCALAR >* bn,
                                     const std::string&      filename) :
      BNReader< GUM_SCALAR >(bn, filename) {
    GUM_CONSTRUCTOR(NetReader);
    bn__         = bn;
    streamName__ = filename;
    parseDone__  = false;

    factory__ = new BayesNetFactory< GUM_SCALAR >(bn__);

    ioerror__ = false;

    try {
      scanner__ = new net::Scanner(streamName__.c_str());
      parser__  = new net::Parser(scanner__);
      parser__->setFactory((IBayesNetFactory*)factory__);
    } catch (IOError&) { ioerror__ = true; }
  }

  template < typename GUM_SCALAR >
  NetReader< GUM_SCALAR >::~NetReader() {
    GUM_DESTRUCTOR(NetReader);

    if (!ioerror__) {
      // this could lead to memory leak !!
      if (parser__) delete (parser__);

      if (scanner__) delete (scanner__);
    }

    if (factory__) delete (factory__);
  }

  template < typename GUM_SCALAR >
  INLINE net::Scanner& NetReader< GUM_SCALAR >::scanner() {
    if (ioerror__) { GUM_ERROR(gum::IOError, "No such file " + streamName()) }

    return *scanner__;
  }

  template < typename GUM_SCALAR >
  INLINE const std::string& NetReader< GUM_SCALAR >::streamName() const {
    return streamName__;
  }

  template < typename GUM_SCALAR >
  INLINE bool NetReader< GUM_SCALAR >::trace() const {
    return traceScanning__;
  }

  template < typename GUM_SCALAR >
  INLINE void NetReader< GUM_SCALAR >::trace(bool b) {
    traceScanning__ = b;
    scanner().setTrace(b);
  }

  template < typename GUM_SCALAR >
  Size NetReader< GUM_SCALAR >::proceed() {
    if (ioerror__) { GUM_ERROR(gum::IOError, "No such file " + streamName()) }

    if (!parseDone__) {
      try {
        parser__->Parse();
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        return 1 + parser__->errors().error_count;
      }

      parseDone__ = true;
    }

    return (parser__->errors().error_count);
  }

  // @{
  // publishing Errors API
  template < typename GUM_SCALAR >
  INLINE Idx NetReader< GUM_SCALAR >::errLine(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).line;
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet")
    }
  }

  template < typename GUM_SCALAR >
  INLINE Idx NetReader< GUM_SCALAR >::errCol(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).column;
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet")
    }
  }

  template < typename GUM_SCALAR >
  INLINE bool NetReader< GUM_SCALAR >::errIsError(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).is_error;
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet")
    }
  }

  template < typename GUM_SCALAR >
  INLINE std::string NetReader< GUM_SCALAR >::errMsg(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).msg;
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet")
    }
  }

  template < typename GUM_SCALAR >
  INLINE void NetReader< GUM_SCALAR >::showElegantErrors(std::ostream& o) {
    if (parseDone__)
      parser__->errors().elegantErrors(o);
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet")
    }
  }

  template < typename GUM_SCALAR >
  INLINE void
     NetReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& o) {
    if (parseDone__)
      parser__->errors().elegantErrorsAndWarnings(o);
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet")
    }
  }

  template < typename GUM_SCALAR >
  INLINE void NetReader< GUM_SCALAR >::showErrorsAndWarnings(std::ostream& o) {
    if (parseDone__)
      parser__->errors().simpleErrorsAndWarnings(o);
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet")
    }
  }

  template < typename GUM_SCALAR >
  INLINE void NetReader< GUM_SCALAR >::showErrorCounts(std::ostream& o) {
    if (parseDone__)
      parser__->errors().syntheticResults(o);
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet")
    }
  }

  template < typename GUM_SCALAR >
  INLINE Size NetReader< GUM_SCALAR >::errors() {
    return (!parseDone__) ? (Size)0 : parser__->errors().error_count;
  }

  template < typename GUM_SCALAR >
  INLINE Size NetReader< GUM_SCALAR >::warnings() {
    return (!parseDone__) ? (Size)0 : parser__->errors().warning_count;
  }

  // @}
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
