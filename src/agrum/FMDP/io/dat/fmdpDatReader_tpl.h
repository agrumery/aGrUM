/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Definition of templatized reader of dat files for Factored Markov
 * Decision Process.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/FMDP/io/dat/fmdpDatReader.h>

namespace gum {

  template < typename GUM_SCALAR >
  FMDPDatReader< GUM_SCALAR >::FMDPDatReader(FMDP< GUM_SCALAR >* fmdp,
                                             const std::string&  filename) :
      FMDPReader< GUM_SCALAR >(fmdp, filename) {
    GUM_CONSTRUCTOR(FMDPDatReader);

    _fmdp_       = fmdp;
    _streamName_ = filename;
    _parseDone_  = false;
    //    ddf->putOnNoVariableCheckMode();
    _factory_ = new FMDPFactory< GUM_SCALAR >(_fmdp_);
    //~  _factory_->setVerbose();
    _ioerror_ = false;

    try {
      _scanner_ = new MDPDAT::Scanner(_streamName_.c_str());
      _parser_  = new MDPDAT::Parser(_scanner_);
      _parser_->setFactory((AbstractFMDPFactory*)_factory_);
    } catch (const IOError&) { _ioerror_ = true; }
  }

  template < typename GUM_SCALAR >
  FMDPDatReader< GUM_SCALAR >::~FMDPDatReader() {
    GUM_DESTRUCTOR(FMDPDatReader);

    if (!_ioerror_) {
      // this could lead to memory leak !!
      if (_parser_) delete (_parser_);

      if (_scanner_) delete (_scanner_);
    }

    if (_factory_) delete (_factory_);
  }

  template < typename GUM_SCALAR >
  INLINE MDPDAT::Scanner& FMDPDatReader< GUM_SCALAR >::scanner() {
    if (_ioerror_) { GUM_ERROR(gum::IOError, "No such file " + streamName()) }

    return *_scanner_;
  }

  template < typename GUM_SCALAR >
  INLINE const std::string& FMDPDatReader< GUM_SCALAR >::streamName() const {
    return _streamName_;
  }

  template < typename GUM_SCALAR >
  INLINE bool FMDPDatReader< GUM_SCALAR >::trace() const {
    return _traceScanning_;
  }

  template < typename GUM_SCALAR >
  INLINE void FMDPDatReader< GUM_SCALAR >::trace(bool b) {
    _traceScanning_ = b;
    scanner().setTrace(b);
  }

  template < typename GUM_SCALAR >
  Size FMDPDatReader< GUM_SCALAR >::proceed() {
    if (_ioerror_) { GUM_ERROR(gum::IOError, "No such file " + streamName()) }

    if (!_parseDone_) {
      try {
        _parser_->Parse();
        _parseDone_ = true;
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        return 1 + _parser_->errors().error_count;
      }
    }

    return (_parser_->errors().error_count);
  }

  /// @{
  /// publishing Errors API
  template < typename GUM_SCALAR >
  INLINE Idx FMDPDatReader< GUM_SCALAR >::errLine(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).line;
    else GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet")
  }

  template < typename GUM_SCALAR >
  INLINE Idx FMDPDatReader< GUM_SCALAR >::errCol(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).column;
    else GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet")
  }

  template < typename GUM_SCALAR >
  INLINE bool FMDPDatReader< GUM_SCALAR >::errIsError(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).is_error;
    else GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet")
  }

  template < typename GUM_SCALAR >
  INLINE std::string FMDPDatReader< GUM_SCALAR >::errMsg(Idx i) {
    if (_parseDone_) return _parser_->errors().error(i).msg;
    else GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet")
  }

  template < typename GUM_SCALAR >
  INLINE void FMDPDatReader< GUM_SCALAR >::showElegantErrors(std::ostream& o) {
    if (_parseDone_) _parser_->errors().elegantErrors(o);
    else GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet")
  }

  template < typename GUM_SCALAR >
  INLINE void FMDPDatReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& o) {
    if (_parseDone_) _parser_->errors().elegantErrorsAndWarnings(o);
    else GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet")
  }

  template < typename GUM_SCALAR >
  INLINE void FMDPDatReader< GUM_SCALAR >::showErrorCounts(std::ostream& o) {
    if (_parseDone_) _parser_->errors().syntheticResults(o);
    else GUM_ERROR(OperationNotAllowed, "FMDPDat file not parsed yet")
  }

  template < typename GUM_SCALAR >
  INLINE Size FMDPDatReader< GUM_SCALAR >::errors() {
    return (!_parseDone_) ? (Size)0 : _parser_->errors().error_count;
  }

  template < typename GUM_SCALAR >
  INLINE Size FMDPDatReader< GUM_SCALAR >::warnings() {
    return (!_parseDone_) ? (Size)0 : _parser_->errors().warning_count;
  }

  /// @}
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
