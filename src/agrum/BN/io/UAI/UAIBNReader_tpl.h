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

namespace gum {

  template < typename GUM_SCALAR >
  UAIBNReader< GUM_SCALAR >::UAIBNReader(BayesNet< GUM_SCALAR >* bn,
                                         const std::string&      filename) :
      BNReader< GUM_SCALAR >(bn, filename) {
    GUM_CONSTRUCTOR(UAIBNReader);
    bn__         = bn;
    streamName__ = filename;
    parseDone__  = false;

    ioerror__ = false;

    try {
      scanner__ = new UAIBN::Scanner(streamName__.c_str());
      parser__  = new UAIBN::Parser(scanner__);
    } catch (IOError&) { ioerror__ = true; }
  }

  template < typename GUM_SCALAR >
  UAIBNReader< GUM_SCALAR >::~UAIBNReader() {
    GUM_DESTRUCTOR(UAIBNReader);

    if (!ioerror__) {
      // this could lead to memory leak !!
      if (parser__) delete (parser__);

      if (scanner__) delete (scanner__);
    }
  }

  template < typename GUM_SCALAR >
  INLINE UAIBN::Scanner& UAIBNReader< GUM_SCALAR >::scanner() {
    if (ioerror__) { GUM_ERROR(gum::IOError, "No such file " + streamName()); }

    return *scanner__;
  }

  template < typename GUM_SCALAR >
  INLINE const std::string& UAIBNReader< GUM_SCALAR >::streamName() const {
    return streamName__;
  }

  template < typename GUM_SCALAR >
  INLINE bool UAIBNReader< GUM_SCALAR >::trace() const {
    return traceScanning__;
  }

  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::trace(bool b) {
    traceScanning__ = b;
    scanner().setTrace(b);
  }

  template < typename GUM_SCALAR >
  Size UAIBNReader< GUM_SCALAR >::proceed() {
    if (ioerror__) { GUM_ERROR(gum::IOError, "No such file " + streamName()); }

    if (!parseDone__) {
      try {
        parser__->Parse();
        parseDone__ = true;
        buildFromQuartets(parser__->getQuartets());
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        return 1 + parser__->errors().error_count;
      }
    }

    return (parser__->errors().error_count);
  }

  template < typename GUM_SCALAR >
  void UAIBNReader< GUM_SCALAR >::buildFromQuartets(
     std::vector< std::tuple< float, int, int, int > > quartets) {
    Idx  current;
    Size max = quartets.size();
    if (max == 0) {
      addWarning__(1, 1, "Empty BayesNet");
      return;
    }

    auto isInt = [&]() -> bool {
      return (std::get< 0 >(quartets[current]) == -1);
    };
    auto lig = [&]() -> int {
      return std::get< 2 >(quartets[current]);
    };
    auto col = [&]() -> int {
      return std::get< 3 >(quartets[current]);
    };

    auto getInt = [&]() -> int {
      if (!isInt()) this->addFatalError__(lig(), col(), "int expected");
      return std::get< 1 >(quartets[current]);
    };
    auto getVal = [&]() -> GUM_SCALAR {
      return (isInt()) ? (std::get< 1 >(quartets[current]))
                       : (std::get< 0 >(quartets[current]));
    };
    auto incCurrent = [&]() {
      current += 1;
      if (current >= max)
        this->addFatalError__(lig(), col(), "Not enough data in UAI file");
    };

    current      = 0;
    Size nbrNode = (Size)getInt();

    for (NodeId i = 0; i < nbrNode; i++) {
      incCurrent();
      int mod = getInt();
      if (mod < 2)
        addError__(lig(), col(), "Number of modalities should be greater than 2.");
      bn__->add(gum::LabelizedVariable(std::to_string(i), "", mod));
    }

    incCurrent();
    Size nbrPot = (Size)getInt();
    if (nbrPot != nbrNode)
      addWarning__(lig(),
                   col(),
                   "Number of CPTs should be the same as number of nodes");

    Set< NodeId > s;
    for (NodeId i = 0; i < nbrPot; i++) {
      incCurrent();
      Size nbrPar = (Size)getInt();
      if (nbrPar == 0) addError__(lig(), col(), "0 is not possible here");

      std::vector< NodeId > papas;
      for (NodeId j = 1; j < nbrPar; j++) {
        incCurrent();
        NodeId papa = (NodeId)getInt();
        if (papa >= nbrNode)
          addError__(lig(), col(), "Not enough variables in the BayesNet");
        papas.push_back(papa);
      }

      incCurrent();
      NodeId nodePot = (Size)getInt();
      if (nodePot >= nbrNode)
        addError__(lig(), col(), "Not enough variables in the BayesNet");
      if (s.contains(nodePot)) addError__(lig(), col(), "Parents already defined");
      s.insert(nodePot);

      for (const auto papa: papas) {
        bn__->addArc(papa, nodePot);
      }
    }

    std::vector< GUM_SCALAR > v;
    for (NodeId i = 0; i < nbrPot; i++) {
      incCurrent();
      Size nbrParam = (Size)getInt();
      if (nbrParam != bn__->cpt(i).domainSize())
        addFatalError__(lig(),
                        col(),
                        "Size does not fit between parents and parameters");
      for (Idx j = 0; j < nbrParam; j++) {
        incCurrent();
        v.push_back(getVal());
      }
      bn__->cpt(i).fillWith(v);
      v.clear();
    }

    if (current != max - 1) addError__(lig(), col(), "Too many data in this file");
  }

  // @{
  // publishing Errors API
  template < typename GUM_SCALAR >
  INLINE Idx UAIBNReader< GUM_SCALAR >::errLine(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).line;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE Idx UAIBNReader< GUM_SCALAR >::errCol(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).column;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE bool UAIBNReader< GUM_SCALAR >::errIsError(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).is_error;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE std::string UAIBNReader< GUM_SCALAR >::errMsg(Idx i) {
    if (parseDone__)
      return parser__->errors().error(i).msg;
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::showElegantErrors(std::ostream& o) {
    if (parseDone__)
      parser__->errors().elegantErrors(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void
     UAIBNReader< GUM_SCALAR >::showElegantErrorsAndWarnings(std::ostream& o) {
    if (parseDone__)
      parser__->errors().elegantErrorsAndWarnings(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::showErrorsAndWarnings(std::ostream& o) {
    if (parseDone__)
      parser__->errors().simpleErrorsAndWarnings(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::showErrorCounts(std::ostream& o) {
    if (parseDone__)
      parser__->errors().syntheticResults(o);
    else {
      GUM_ERROR(OperationNotAllowed, "UAI file not parsed yet");
    }
  }

  template < typename GUM_SCALAR >
  INLINE Size UAIBNReader< GUM_SCALAR >::errors() {
    return (!parseDone__) ? (Size)0 : parser__->errors().error_count;
  }

  template < typename GUM_SCALAR >
  INLINE Size UAIBNReader< GUM_SCALAR >::warnings() {
    return (!parseDone__) ? (Size)0 : parser__->errors().warning_count;
  }

  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::addFatalError__(Idx                lig,
                                                         Idx                col,
                                                         const std::string& s) {
    parser__->errors().addError(s, streamName__, lig, col);
    GUM_ERROR(gum::OperationNotAllowed, "");
  }
  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::addError__(Idx                lig,
                                                    Idx                col,
                                                    const std::string& s) {
    parser__->errors().addError(s, streamName__, lig, col);
  }
  template < typename GUM_SCALAR >
  INLINE void UAIBNReader< GUM_SCALAR >::addWarning__(Idx                lig,
                                                      Idx                col,
                                                      const std::string& s) {
    parser__->errors().addWarning(s, streamName__, lig, col);
  }

  // @}
}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
