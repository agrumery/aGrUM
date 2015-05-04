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

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/BN/io/net/netReader.h>

namespace gum {

  template <typename GUM_SCALAR>
  NetReader<GUM_SCALAR>::NetReader(BayesNet<GUM_SCALAR> *bn,
                                   const std::string &filename)
      : BNReader<GUM_SCALAR>(bn, filename) {
    GUM_CONSTRUCTOR(NetReader);
    __bn = bn;
    __streamName = filename;
    __parseDone = false;

    __factory = new BayesNetFactory<GUM_SCALAR>(__bn);

    __ioerror = false;

    try {
      __scanner = new net::Scanner(__streamName.c_str());
      __parser = new net::Parser(__scanner);
      __parser->setFactory((IBayesNetFactory *)__factory);
    } catch (IOError e) {
      __ioerror = true;
    }
  }

  template <typename GUM_SCALAR> NetReader<GUM_SCALAR>::~NetReader() {
    GUM_DESTRUCTOR(NetReader);

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
  INLINE net::Scanner &NetReader<GUM_SCALAR>::scanner() {
    if (__ioerror) {
      GUM_ERROR(gum::IOError, "No such file " + streamName());
    }

    return *__scanner;
  }

  template <typename GUM_SCALAR>
  INLINE const std::string &NetReader<GUM_SCALAR>::streamName() const {
    return __streamName;
  }

  template <typename GUM_SCALAR>
  INLINE bool NetReader<GUM_SCALAR>::trace(void) const {
    return __traceScanning;
  }

  template <typename GUM_SCALAR> INLINE void NetReader<GUM_SCALAR>::trace(bool b) {
    __traceScanning = b;
    scanner().setTrace(b);
  }

  template <typename GUM_SCALAR> int NetReader<GUM_SCALAR>::proceed(void) {
    // FILE* tmpfile();

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

  // @{
  // publishing Errors API
  template <typename GUM_SCALAR>
  INLINE unsigned int NetReader<GUM_SCALAR>::errLine(unsigned int i) {
    if (__parseDone)
      return __parser->errors().line(i);
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE unsigned int NetReader<GUM_SCALAR>::errCol(unsigned int i) {
    if (__parseDone)
      return __parser->errors().col(i);
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE bool NetReader<GUM_SCALAR>::errIsError(unsigned int i) {
    if (__parseDone)
      return __parser->errors().is_error(i);
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE std::string NetReader<GUM_SCALAR>::errMsg(unsigned int i) {
    if (__parseDone)
      return std::string(narrow(__parser->errors().msg(i)));
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE void NetReader<GUM_SCALAR>::showElegantErrors(std::ostream &o) {
    if (__parseDone)
      __parser->errors().elegantErrors(o);
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE void NetReader<GUM_SCALAR>::showElegantErrorsAndWarnings(std::ostream &o) {
    if (__parseDone)
      __parser->errors().elegantErrorsAndWarnings(o);
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE void NetReader<GUM_SCALAR>::showErrorsAndWarnings(std::ostream &o) {
    if (__parseDone)
      __parser->errors().simpleErrorsAndWarnings(o);
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet");
    }
  }

  template <typename GUM_SCALAR>
  INLINE void NetReader<GUM_SCALAR>::showErrorCounts(std::ostream &o) {
    if (__parseDone)
      __parser->errors().syntheticResults(o);
    else {
      GUM_ERROR(OperationNotAllowed, "Net file not parsed yet");
    }
  }

  template <typename GUM_SCALAR> INLINE Size NetReader<GUM_SCALAR>::errors() {
    return (!__parseDone) ? (Size)0 : __parser->errors().error_count;
  }

  template <typename GUM_SCALAR> INLINE Size NetReader<GUM_SCALAR>::warnings() {
    return (!__parseDone) ? (Size)0 : __parser->errors().warning_count;
  }

  // @}
} // namespace

#endif // DOXYGEN_SHOULD_SKIP_THIS
