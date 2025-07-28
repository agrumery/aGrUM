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
 * @brief Definition of templatized reader of BIF files for Bayesian networks.
 *
 * how to use it :
 * @code
// OPTIONAL LISTENER CLASS
class aSimpleListener : public gum::Listener {
public:
  void whenLoading(const void *buffer,int percent) {
  // percent goes from 0 to 100 (whenLoading is called at most once for each
integer
between 0 and 100
    // percent=200 recieved when End Of File.
 }
};
// END OF OPTIONAL LISTENER

 gum::BayesNet<double> bn;

 try {
  gum::BIFReader<double> reader(&bn,std::string(args[1]));

  // OPTIONAL SECTION
  aSimpleListener l;
  GUM_CONNECT(reader.scanner(),onLoad,l,aSimpleListener::whenLoading);
  // END OF OPTIONNAL SECTION

  if (reader.proceed()==0) {
   std::cerr<<"Well done !"<<std::endl;
  } else {
   reader.showElegantErrorsAndWarnings();
   reader.showErrorCounts();
  }
 } catch (gum::IOError& e) {GUM_SHOWERROR(e);}

  return 0;

 * @endcode
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef BIF_READER_H
#define BIF_READER_H

#include <fstream>
#include <iostream>
#include <string>

#include <agrum/BN/io/BNReader.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// including coco-generated PARSER and SCANNER
#  include <agrum/BN/io/BIF/cocoR/Parser.h>
#endif   // DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  /**
* @class BIFReader
* @headerfile BIFReader.h <agrum/BN/io/BIF/BIFReader.h>
   * @ingroup bn_io
   * @brief Definition of templatized reader of BIF files for Bayesian networks.
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

   gum::BayesNet<double> bn;

   try {
    gum::BIFReader<double> reader(&bn,std::string(args[1]));

    // OPTIONAL SECTION
    aSimpleListener l;
    GUM_CONNECT(reader.scanner(),onLoad,l,aSimpleListener::whenLoading);
    // END OF OPTIONNAL SECTION

    if (reader.proceed()==0) {
     std::cerr<<"Well done !"<<std::endl;
    } else {
     reader.showElegantErrorsAndWarnings();
     reader.showErrorCounts();
    }
   } catch (gum::IOError& e) {GUM_SHOWERROR(e);}

    return 0;

   * @endcode
   *
   * @author Pierre-Henri WUILLEMIN(_at_LIP6)
   */
  template < typename GUM_SCALAR >
  class BIFReader: public BNReader< GUM_SCALAR > {
    public:
    BIFReader(BayesNet< GUM_SCALAR >* bn, const std::string& filename);

    ~BIFReader();

    /// Direct access to BIF scanner (mandatory for listener connection)
    /// @throws IOError if file not exists
    BIF::Scanner& scanner();

    /// name of read file
    const std::string& streamName() const;

    /// accessor to trace function (just write the number of parser line)
    bool trace() const;
    void trace(bool b);

    /// parse.
    /// @return the number of detected errors
    /// @throws IOError if file not exists
    Size proceed() final;

    /// @{
    /// publishing Errors API

    /// # of errors
    Size errors();
    /// # of errors
    Size warnings();

    /// line of ith error or warning
    Idx errLine(Idx i);
    /// col of ith error or warning
    Idx errCol(Idx i);
    /// type of ith error or warning
    bool errIsError(Idx i);
    /// message of ith error or warning
    std::string errMsg(Idx i);

    /// send on std::cerr the list of errors
    void showElegantErrors(std::ostream& o = std::cerr);

    /// send on std::cerr the list of errors or warnings
    void showElegantErrorsAndWarnings(std::ostream& o = std::cerr);

    /// send on std::cerr the number of errors and the number of warnings
    void showErrorCounts(std::ostream& o = std::cerr);
    /// @}

    private:
    BayesNet< GUM_SCALAR >*        _bn_;
    BayesNetFactory< GUM_SCALAR >* _factory_;
    BIF::Scanner*                  _scanner_;
    BIF::Parser*                   _parser_;

    std::string _streamName_;
    bool        _traceScanning_;
    bool        _parseDone_;

    // a boolean to throw the ioerror not in the constructor but in the
    // proceed()
    bool _ioerror_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class BIFReader< double >;
#endif

}   // namespace gum

#include <agrum/BN/io/BIF/BIFReader_tpl.h>

#endif   // BIF_READER_H
