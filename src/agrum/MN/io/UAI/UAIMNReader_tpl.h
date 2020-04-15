
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES
 * (@AMU) info_at_agrum_dot_org
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
  UAIMNReader< GUM_SCALAR >::UAIMNReader(MarkovNet< GUM_SCALAR >* MN,
                                         const std::string&       filename) :
      MNReader< GUM_SCALAR >(MN, filename) {
    GUM_CONSTRUCTOR(UAIMNReader);
    __mn = MN;
    __streamName = filename;
    __parseDone = false;

    __ioerror = false;

    try {
      __scanner = new UAIMN::Scanner(__streamName.c_str());
      __parser = new UAIMN::Parser(__scanner);
    } catch (IOError&) { __ioerror = true; }
  }

  template < typename GUM_SCALAR >
  UAIMNReader< GUM_SCALAR >::~UAIMNReader() {
    GUM_DESTRUCTOR(UAIMNReader);

    if (!__ioerror) {
      // this could lead to memory leak !!
      if (__parser) delete (__parser);

      if (__scanner) delete (__scanner);
    }
  }

  template < typename GUM_SCALAR >
  INLINE UAIMN::Scanner& UAIMNReader< GUM_SCALAR >::scanner() {
    if (__ioerror) { GUM_ERROR(gum::IOError, "No such file " + streamName()); }

    return *__scanner;
  }

  template < typename GUM_SCALAR >
  INLINE const std::string& UAIMNReader< GUM_SCALAR >::streamName() const {
    return __streamName;
  }

  template < typename GUM_SCALAR >
  INLINE bool UAIMNReader< GUM_SCALAR >::trace() const {
    return __traceScanning;
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMNReader< GUM_SCALAR >::trace(bool b) {
    __traceScanning = b;
    scanner().setTrace(b);
  }

  template < typename GUM_SCALAR >
  Size UAIMNReader< GUM_SCALAR >::proceed() {
    if (__ioerror) { GUM_ERROR(gum::IOError, "No such file " + streamName()); }

    if (!__parseDone) {
      try {
        __parser->Parse();
        __parseDone = true;
        buildFromQuartets(__parser->getQuartets());
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        return 1 + __parser->errors().error_count;
      }
    }

    return (__parser->errors().error_count);
  }

  template < typename GUM_SCALAR >
  void UAIMNReader< GUM_SCALAR >::buildFromQuartets(
     std::vector< std::tuple< float, int, int, int > > quartets) {
    Idx  current;
    Size max = quartets.size();
    if (max == 0) {
      __addWarning(1, 1, "Empty MarkovNet");
      return;
    }

    auto isInt = [&]() -> bool {
      return (std::get< 0 >(quartets[current]) == -1);
    };
    auto lig = [&]() -> int { return std::get< 2 >(quartets[current]); };
    auto col = [&]() -> int { return std::get< 3 >(quartets[current]); };

    auto getInt = [&]() -> int {
      if (!isInt()) this->__addFatalError(lig(), col(), "int expected");
      return std::get< 1 >(quartets[current]);
    };
    auto getVal = [&]() -> GUM_SCALAR {
      return (isInt()) ? (std::get< 1 >(quartets[current]))
                       : (std::get< 0 >(quartets[current]));
    };
    auto incCurrent = [&]() {
      current += 1;
      if (current >= max)
        this->__addFatalError(lig(), col(), "Not enough data in UAI file");
    };

    current = 0;
    Size nbrNode = (Size)getInt();

    for (NodeId i = 0; i < nbrNode; i++) {
      incCurrent();
      int mod = getInt();
      if (mod < 2)
        __addError(lig(), col(), "Number of modalities should be greater than 2.");
      __mn->add(gum::LabelizedVariable(std::to_string(i), "", mod));
    }

    incCurrent();
    Size nbrFactors = (Size)getInt();

    std::vector< NodeSet > clicks;
    for (NodeId i = 0; i < nbrFactors; i++) {
      incCurrent();
      Size nbrVar = (Size)getInt();
      if (nbrVar == 0) __addError(lig(), col(), "0 is not possible here");

      NodeSet vars;
      for (NodeId j = 0; j < nbrVar; j++) {
        incCurrent();
        NodeId nod = (NodeId)getInt();
        if (nod >= nbrNode)
          __addError(lig(), col(), "Not enough variables in the MarkovNet");
        vars.insert(nod);
      }
      __mn->addFactor(vars);
      clicks.push_back(vars);
    }

    for (NodeId i = 0; i < nbrFactors; i++) {
      incCurrent();
      Size nbrParam = (Size)getInt();
      if (nbrParam != __mn->factor(clicks[i]).domainSize()) {
        __addFatalError(
           lig(), col(), "Size does not fit between clique and parameters");
      }
      std::vector< GUM_SCALAR > v;
      for (Idx j = 0; j < nbrParam; j++) {
        incCurrent();
        v.push_back(getVal());
      }
      __mn->factor(clicks[i]).fillWith(v);
      v.clear();
    }

    if (current != max - 1) __addError(lig(), col(), "Too many data in this file");
  }

  // @{
  // publishing Errors API
  template < typename GUM_SCALAR >
  INLINE Idx UAIMNReader< GUM_SCALAR >::errLine(Idx i) {
    if (__parseDone)
      return __parser->errors().error(i).line;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE Idx UAIMNReader< GUM_SCALAR >::errCol(Idx i) {
    if (__parseDone)
      return __parser->errors().error(i).column;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE bool UAIMNReader< GUM_SCALAR >::errIsError(Idx i) {
    if (__parseDone)
      return __parser->errors().error(i).is_error;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE std::string UAIMNReader< GUM_SCALAR >::errMsg(Idx i) {
    if (__parseDone)
      return __parser->errors().error(i).msg;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMNReader< GUM_SCALAR >::showElegantErrors(std::ostream& o) {
    if (__parseDone)
      __parser->errors().elegantErrors(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void
     UAIMNReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& o) {
    if (__parseDone)
      __parser->errors().elegantErrorsAndWarnings(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMNReader< GUM_SCALAR >::showErrorsAndWarnings(std::ostream& o) {
    if (__parseDone)
      __parser->errors().simpleErrorsAndWarnings(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMNReader< GUM_SCALAR >::showErrorCounts(std::ostream& o) {
    if (__parseDone)
      __parser->errors().syntheticResults(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE Size UAIMNReader< GUM_SCALAR >::errors() {
    return (!__parseDone) ? (Size)0 : __parser->errors().error_count;
  }

  template < typename GUM_SCALAR >
  INLINE Size UAIMNReader< GUM_SCALAR >::warnings() {
    return (!__parseDone) ? (Size)0 : __parser->errors().warning_count;
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMNReader< GUM_SCALAR >::__addFatalError(Idx                lig,
                                                         Idx                col,
                                                         const std::string& s) {
    __parser->errors().addError(s, __streamName, lig, col);
    GUM_ERROR(gum::OperationNotAllowed, s);
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMNReader< GUM_SCALAR >::__addError(Idx                lig,
                                                    Idx                col,
                                                    const std::string& s) {
    __parser->errors().addError(s, __streamName, lig, col);
  }

  template < typename GUM_SCALAR >
  INLINE void UAIMNReader< GUM_SCALAR >::__addWarning(Idx                lig,
                                                      Idx                col,
                                                      const std::string& s) {
    __parser->errors().addWarning(s, __streamName, lig, col);
  }

  // @}
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
