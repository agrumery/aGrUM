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
 * @brief Definition of templatized reader of dat files for Factored Markov Decision
 Process.
 *
 * how to use it :
 * @code
  // OPTIONAL LISTENER CLASS
  class aSimpleListener : public gum::Listener {
  public:
    void whenLoading(const void *buffer,int percent) {
      // percent goes from 0 to 100 (whenLoading is called at most once for each
 integer between 0 and 100
      // percent=200 recieved when End Of File.
    }
  };
  // END OF OPTIONAL LISTENER

  gum::FactoredMarkovDecisionProcess<double> fmdp;

  try {
    gum::FMDPDatReader<double> reader( &fmdp, std::string( args[1] ) );

    // OPTIONAL SECTION
    aSimpleListener l;
    GUM_CONNECT( reader.scanner(), onLoad, l, aSimpleListener::whenLoading );
    // END OF OPTIONNAL SECTION

    if ( reader.proceed() == 0 ) {
      std::cerr << "Well done !" << std::endl;
    } else {
      reader.showElegantErrorsAndWarnings();
      reader.showErrorCounts();
    }
  } catch ( gum::IOError& e ) { GUM_SHOWERROR( e ); }

  return 0;

 * @endcode
 *
 * @author Pierre-Henri WUILLEMIN
 */

#ifndef FMDP_DAT_READER_H
#define FMDP_DAT_READER_H

#include <iostream>
#include <string>
#include <fstream>

#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/FMDPFactory.h>
#include <agrum/FMDP/io/FMDPReader.h>

#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// including coco-generated PARSER and SCANNER
#include <agrum/FMDP/io/dat/cocoR/Parser.h>
#endif // DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  /**
   * @class FMDPDatReader
   * @brief Definition of templatized reader of FMDPDat files for Bayesian Networks.
   * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
   */
  template <typename GUM_SCALAR>
  class FMDPDatReader : public FMDPReader<GUM_SCALAR> {
    public:
    FMDPDatReader(FactoredMarkovDecisionProcess<GUM_SCALAR> *fmdp,
                  const std::string &filename,
                  MultiDimDecisionDiagramFactoryBase<GUM_SCALAR> *ddf);
    ~FMDPDatReader();

    /// Direct access to FMDPDat scanner (mandatory for listener connection)
    /// @throws IOError if file not exists
    MDPDAT::Scanner &scanner();

    /// name of readen file
    const std::string &streamName() const;

    /// accessor to trace function (just write the number of parser line)
    bool trace(void) const;
    void trace(bool b);

    /// parse.
    /// @return the number of detected errors
    /// @throws IOError if file not exists
    int proceed(void);

    /// @{
    /// publishing Errors API

    /// # of errors
    Size errors();
    /// # of errors
    Size warnings();

    /// line of ith error or warning
    unsigned int errLine(unsigned int i);
    /// col of ith error or warning
    unsigned int errCol(unsigned int i);
    /// type of ith error or warning
    bool errIsError(unsigned int i);
    /// message of ith error or warning
    std::string errMsg(unsigned int i);

    /// send on std::cerr the list of errors
    void showElegantErrors(std::ostream &o = std::cerr);

    /// send on std::cerr the list of errors or warnings
    void showElegantErrorsAndWarnings(std::ostream &o = std::cerr);

    /// send on std::cerr the number of errors and the number of warnings
    void showErrorCounts(std::ostream &o = std::cerr);
    /// @}

    protected:
    FactoredMarkovDecisionProcess<GUM_SCALAR> *__fmdp;
    FMDPFactory<GUM_SCALAR> *__factory;
    MDPDAT::Scanner *__scanner;
    MDPDAT::Parser *__parser;

    std::string __streamName;
    bool __traceScanning;
    bool __parseDone;

    // a boolean to throw the ioerror not in the constructor but in the proceed()
    bool __ioerror;
  };

  extern template class FMDPDatReader<float>;
  extern template class FMDPDatReader<double>;
} // namespace gum

#include <agrum/FMDP/io/dat/FMDPDatReader.tcc>

#endif // FMDP_DAT_READER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
