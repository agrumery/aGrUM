
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
  UAIReader< GUM_SCALAR >::UAIReader(BayesNet< GUM_SCALAR >* bn,
                                     const std::string&      filename) :
      BNReader< GUM_SCALAR >(bn, filename) {
    GUM_CONSTRUCTOR(UAIReader);
    __bn = bn;
    __streamName = filename;
    __parseDone = false;

    __ioerror = false;

    try {
      __scanner = new UAI::Scanner(__streamName.c_str());
      __parser = new UAI::Parser(__scanner);
    } catch (IOError&) { __ioerror = true; }
  }

  template < typename GUM_SCALAR >
  UAIReader< GUM_SCALAR >::~UAIReader() {
    GUM_DESTRUCTOR(UAIReader);

    if (!__ioerror) {
      // this could lead to memory leak !!
      if (__parser) delete (__parser);

      if (__scanner) delete (__scanner);
    }
  }

  template < typename GUM_SCALAR >
  INLINE UAI::Scanner& UAIReader< GUM_SCALAR >::scanner() {
    if (__ioerror) { GUM_ERROR(gum::IOError, "No such file " + streamName()); }

    return *__scanner;
  }

  template < typename GUM_SCALAR >
  INLINE const std::string& UAIReader< GUM_SCALAR >::streamName() const {
    return __streamName;
  }

  template < typename GUM_SCALAR >
  INLINE bool UAIReader< GUM_SCALAR >::trace() const {
    return __traceScanning;
  }

  template < typename GUM_SCALAR >
  INLINE void UAIReader< GUM_SCALAR >::trace(bool b) {
    __traceScanning = b;
    scanner().setTrace(b);
  }

  template < typename GUM_SCALAR >
  Size UAIReader< GUM_SCALAR >::proceed() {
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
  void UAIReader< GUM_SCALAR >::buildFromQuartets(
     std::vector< std::tuple< float, int, int, int > > quartets) {
    Idx  current;
    Size max = quartets.size();
    if (max == 0) {
      __addWarning(1, 1, "Empty BayesNet");
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
      __bn->add(gum::LabelizedVariable(std::to_string(i), "", mod));
    }

    incCurrent();
    Size nbrPot = (Size)getInt();
    if (nbrPot != nbrNode)
      __addWarning(
         lig(), col(), "Number of CPTs should be the same as number of nodes");

    Set< NodeId > s;
    for (NodeId i = 0; i < nbrPot; i++) {
      incCurrent();
      Size nbrPar = (Size)getInt();
      if (nbrPar == 0) __addError(lig(), col(), "0 is not possible here");

      std::vector< NodeId > papas;
      for (NodeId j = 1; j < nbrPar; j++) {
        incCurrent();
        NodeId papa = (NodeId)getInt();
        if (papa >= nbrNode)
          __addError(lig(), col(), "Not enough variables in the BayesNet");
        papas.push_back(papa);
      }

      incCurrent();
      NodeId nodePot = (Size)getInt();
      if (nodePot >= nbrNode)
        __addError(lig(), col(), "Not enough variables in the BayesNet");
      if (s.contains(nodePot)) __addError(lig(), col(), "Parents already defined");
      s.insert(nodePot);

      for (const auto papa : papas) {
        __bn->addArc(papa, nodePot);
      }
    }

    std::vector< GUM_SCALAR > v;
    for (NodeId i = 0; i < nbrPot; i++) {
      incCurrent();
      Size nbrParam = (Size)getInt();
      if (nbrParam != __bn->cpt(i).domainSize())
        __addFatalError(
           lig(), col(), "Size does not fit between parents and parameters");
      for (Idx j = 0; j < nbrParam; j++) {
        incCurrent();
        v.push_back(getVal());
      }
      __bn->cpt(i).fillWith(v);
      v.clear();
    }

    if (current != max - 1) __addError(lig(), col(), "Too many data in this file");
  }

  // @{
  // publishing Errors API
  template < typename GUM_SCALAR >
  INLINE Idx UAIReader< GUM_SCALAR >::errLine(Idx i) {
    if (__parseDone)
      return __parser->errors().error(i).line;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE Idx UAIReader< GUM_SCALAR >::errCol(Idx i) {
    if (__parseDone)
      return __parser->errors().error(i).column;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE bool UAIReader< GUM_SCALAR >::errIsError(Idx i) {
    if (__parseDone)
      return __parser->errors().error(i).is_error;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE std::string UAIReader< GUM_SCALAR >::errMsg(Idx i) {
    if (__parseDone)
      return __parser->errors().error(i).msg;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIReader< GUM_SCALAR >::showElegantErrors(std::ostream& o) {
    if (__parseDone)
      __parser->errors().elegantErrors(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void
     UAIReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& o) {
    if (__parseDone)
      __parser->errors().elegantErrorsAndWarnings(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIReader< GUM_SCALAR >::showErrorsAndWarnings(std::ostream& o) {
    if (__parseDone)
      __parser->errors().simpleErrorsAndWarnings(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIReader< GUM_SCALAR >::showErrorCounts(std::ostream& o) {
    if (__parseDone)
      __parser->errors().syntheticResults(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE Size UAIReader< GUM_SCALAR >::errors() {
    return (!__parseDone) ? (Size)0 : __parser->errors().error_count;
  }

  template < typename GUM_SCALAR >
  INLINE Size UAIReader< GUM_SCALAR >::warnings() {
    return (!__parseDone) ? (Size)0 : __parser->errors().warning_count;
  }

  template < typename GUM_SCALAR >
  INLINE void UAIReader< GUM_SCALAR >::__addFatalError(Idx                lig,
                                                       Idx                col,
                                                       const std::string& s) {
    __parser->errors().addError(s, __streamName, lig, col);
    GUM_ERROR(gum::OperationNotAllowed, "");
  }
  template < typename GUM_SCALAR >
  INLINE void
     UAIReader< GUM_SCALAR >::__addError(Idx lig, Idx col, const std::string& s) {
    __parser->errors().addError(s, __streamName, lig, col);
  }
  template < typename GUM_SCALAR >
  INLINE void UAIReader< GUM_SCALAR >::__addWarning(Idx                lig,
                                                    Idx                col,
                                                    const std::string& s) {
    __parser->errors().addWarning(s, __streamName, lig, col);
  }

  // @}
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
