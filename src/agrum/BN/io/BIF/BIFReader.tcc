/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Definition of templatized reader of BIF files for Bayesian Networks.
 *
 * @author Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/BN/io/BIF/BIFReader.h>

namespace gum {

  template <typename GUM_SCALAR>
  BIFReader<GUM_SCALAR>::BIFReader(BayesNet<GUM_SCALAR> *bn,
                                   const std::string &filename)
      : BNReader<GUM_SCALAR>(bn, filename) {
    GUM_CONSTRUCTOR(BIFReader);
    __bn = bn;
    __streamName = filename;
    __parseDone = false;

    __factory = new BayesNetFactory<GUM_SCALAR>(__bn);

    __ioerror = false;

    try {
      __scanner = new BIF::Scanner(__streamName.c_str());
      __parser = new BIF::Parser(__scanner);
      __parser->setFactory((IBayesNetFactory *)__factory);
    } catch (IOError e) {
      __ioerror = true;
    }
  }

  template <typename GUM_SCALAR> BIFReader<GUM_SCALAR>::~BIFReader() {
    GUM_DESTRUCTOR(BIFReader);

    if (!__ioerror) {
      // this could lead to memory leak !!
      if (__parser)
        delete (__parser);

      if (__scanner)
        delete (__scanner);
    }

    if (__factory)
      delete (__factory);
  }

  template <typename GUM_SCALAR>
  INLINE BIF::Scanner &BIFReader<GUM_SCALAR>::scanner() {
    if (__ioerror) {
      GUM_ERROR(gum::IOError, "No such file " + streamName());
    }

    return *__scanner;
  }

  template <typename GUM_SCALAR>
  INLINE const std::string &BIFReader<GUM_SCALAR>::streamName() const {
    return __streamName;
  }

  template <typename GUM_SCALAR>
  INLINE bool BIFReader<GUM_SCALAR>::trace(void) const {
    return __traceScanning;
  }

  template <typename GUM_SCALAR> INLINE void BIFReader<GUM_SCALAR>::trace(bool b) {
    __traceScanning = b;
    scanner().setTrace(b);
  }

  template <typename GUM_SCALAR> int BIFReader<GUM_SCALAR>::proceed(void) {
    if (__ioerror) {
      GUM_ERROR(gum::IOError, "No such file " + streamName());
    }

    if (!__parseDone) {
      try {
        __parser->Parse();
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
        return 1 + __parser->errors().error_count;
      }

      __parseDone = true;
    }

    return (__parser->errors().error_count);
  }

  /// @{
  /// publishing Errors API
  template <typename GUM_SCALAR>
  INLINE unsigned int BIFReader<GUM_SCALAR>::errLine(unsigned int i) {
    if (__parseDone)
      return __parser->errors().line(i);
    else {
      GUM_ERROR(OperationNotAllowed, "BIF file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE unsigned int BIFReader<GUM_SCALAR>::errCol(unsigned int i) {
    if (__parseDone)
      return __parser->errors().col(i);
    else {
      GUM_ERROR(OperationNotAllowed, "BIF file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE bool BIFReader<GUM_SCALAR>::errIsError(unsigned int i) {
    if (__parseDone)
      return __parser->errors().is_error(i);
    else {
      GUM_ERROR(OperationNotAllowed, "BIF file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE std::string BIFReader<GUM_SCALAR>::errMsg(unsigned int i) {
    if (__parseDone)
      return std::string(narrow(__parser->errors().msg(i)));
    else {
      GUM_ERROR(OperationNotAllowed, "BIF file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE void BIFReader<GUM_SCALAR>::showElegantErrors(std::ostream &o) {
    if (__parseDone)
      __parser->errors().elegantErrors(o);
    else {
      GUM_ERROR(OperationNotAllowed, "BIF file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE void BIFReader<GUM_SCALAR>::showElegantErrorsAndWarnings(std::ostream &o) {
    if (__parseDone)
      __parser->errors().elegantErrorsAndWarnings(o);
    else {
      GUM_ERROR(OperationNotAllowed, "BIF file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE void BIFReader<GUM_SCALAR>::showErrorCounts(std::ostream &o) {
    if (__parseDone)
      __parser->errors().syntheticResults(o);
    else {
      GUM_ERROR(OperationNotAllowed, "BIF file not parsed yet");
    }
  }

  template <typename GUM_SCALAR> INLINE Size BIFReader<GUM_SCALAR>::errors() {
    return (!__parseDone) ? (Size)0 : __parser->errors().error_count;
  }

  template <typename GUM_SCALAR> INLINE Size BIFReader<GUM_SCALAR>::warnings() {
    return (!__parseDone) ? (Size)0 : __parser->errors().warning_count;
  }

  /// @}
} // namespace

#endif // DOXYGEN_SHOULD_SKIP_THIS
