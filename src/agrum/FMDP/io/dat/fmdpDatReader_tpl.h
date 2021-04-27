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


/**
 * @file
 * @brief Definition of templatized reader of dat files for Factored Markov
 * Decision Process.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/FMDP/io/dat/fmdpDatReader.h>

namespace gum {

  template < typename GUM_SCALAR >
  FMDPDatReader< GUM_SCALAR >::FMDPDatReader(FMDP< GUM_SCALAR >* fmdp,
                                             const std::string&  filename) :
      FMDPReader< GUM_SCALAR >(fmdp, filename) {
    GUM_CONSTRUCTOR(FMDPDatReader);

    fmdp__       = fmdp;
    streamName__ = filename;
    parseDone__  = false;
    //    ddf->putOnNoVariableCheckMode();
    factory__ = new FMDPFactory< GUM_SCALAR >(fmdp__);
    //~ factory__->setVerbose();
    ioerror__ = false;

    try {
      scanner__ = new MDPDAT::Scanner(streamName__.c_str());
      parser__  = new MDPDAT::Parser(scanner__);
      parser__->setFactory((AbstractFMDPFactory*)factory__);
    } catch (IOError e) { ioerror__ = true; }
  }

  template < typename GUM_SCALAR >
  FMDPDatReader< GUM_SCALAR >::~FMDPDatReader() {
    GUM_DESTRUCTOR(FMDPDatReader);

    if (!ioerror__) {
      // this could lead to memory leak !!
      if (parser__) delete (parser__);

      if (scanner__) delete (scanner__);
    }

    if (factory__) delete (factory__);
  }

  template < typename GUM_SCALAR >
  INLINE MDPDAT::Scanner& FMDPDatReader< GUM_SCALAR >::scanner() {
    if (ioerror__) { GUM_ERROR(gum::IOError, "No such file " + streamName()); }

    return *scanner__;
  }

  template < typename GUM_SCALAR >
  INLINE const std::string& FMDPDatReader< GUM_SCALAR >::streamName() const {
    return streamName__;
  }

  template < typename GUM_SCALAR >
  INLINE bool FMDPDatReader< GUM_SCALAR >::trace() const {
    return traceScanning__;
  }

  template < typename GUM_SCALAR >
  INLINE void FMDPDatReader< GUM_SCALAR >::trace(bool b) {
    traceScanning__ = b;
    scanner().setTrace(b);
  }

  template < typename GUM_SCALAR >
  Size FMDPDatReader< GUM_SCALAR >::proceed() {
    if (ioerror__) { GUM_ERROR(gum::IOError, "No such file " + streamName()); }

    if (!parseDone__) {
      try {
        parser__->Parse();
        parseDone__ = true;
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        return 1 + parser__->errors().error_count;
      }
    }

    return (parser__->errors().error_count);
  }

  /// @{
  /// publishing Errors API
  template < typename GUM_SCALAR >
  INLINE Idx FMDPDatReader< GUM_SCALAR >::errLine(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).line;
    else
      GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet");
  }

  template < typename GUM_SCALAR >
  INLINE Idx FMDPDatReader< GUM_SCALAR >::errCol(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).column;
    else
      GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet");
  }

  template < typename GUM_SCALAR >
  INLINE bool FMDPDatReader< GUM_SCALAR >::errIsError(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).is_error;
    else
      GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet");
  }

  template < typename GUM_SCALAR >
  INLINE std::string FMDPDatReader< GUM_SCALAR >::errMsg(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).msg;
    else
      GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet");
  }

  template < typename GUM_SCALAR >
  INLINE void FMDPDatReader< GUM_SCALAR >::showElegantErrors(std::ostream& o) {
    if (parseDone__)
      parser__->errors().elegantErrors(o);
    else
      GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet");
  }

  template < typename GUM_SCALAR >
  INLINE void
     FMDPDatReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& o) {
    if (parseDone__)
      parser__->errors().elegantErrorsAndWarnings(o);
    else
      GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet");
  }

  template < typename GUM_SCALAR >
  INLINE void FMDPDatReader< GUM_SCALAR >::showErrorCounts(std::ostream& o) {
    if (parseDone__)
      parser__->errors().syntheticResults(o);
    else
      GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet");
  }

  template < typename GUM_SCALAR >
  INLINE Size FMDPDatReader< GUM_SCALAR >::errors() {
    return (!parseDone__) ? (Size)0 : parser__->errors().error_count;
  }

  template < typename GUM_SCALAR >
  INLINE Size FMDPDatReader< GUM_SCALAR >::warnings() {
    return (!parseDone__) ? (Size)0 : parser__->errors().warning_count;
  }

  /// @}
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
