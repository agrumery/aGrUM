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
#ifndef DSLREADER_H
#define DSLREADER_H

#include <iostream>
#include <string>
#include <agrum/core/debug.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BNReader.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
//including coco-generated PARSER and SCANNER
#undef COCO_PARSER_H__
#undef COCO_SCANNER_H__
#include <agrum/BN/io/DSL/cocoR/Parser.h>
#endif //DOXYGEN_SHOULD_SKIP_THIS


namespace gum {
  /* ============================================================================ */
  /* ===                               READERS                                === */
  /* ============================================================================ */
  /**
   * @class DSLReader
   * @brief Pure virtual class for reading a BN from a file.
   *
   * Every class used to read the content of a Bayesian Network from a stream,
   * or a file must be a subclass of DSLReader.
   */
	template<typename T_DATA>
	class DSLReader : public BNReader<T_DATA> {


public:
    /**
     * Constructor
		 * A reader is defined for reading a defined file. Hence the 2 args of the constructor.
		 * Note that the BN has to be built outside the reader. There is no delegation to create/destroy
		 * the BN from inside the reader.
     */
	DSLReader(BayesNet<T_DATA>* bn,const std::string& filename );

    /**
     * Default destructor.
     */
	~DSLReader();

	
	/// Direct access to DSL scanner (mandatory for listener connection)
	/// @throws IOError if file not exists
	DSL::Scanner& scanner();

	/// name of readen file
	const std::string& streamName() const;

	/// accessor to trace function (just write the number of parser line)
	bool trace( void ) const;
	void trace( bool b );

	/// parse.
	/// @return the number of detected errors
	/// @throws IOError if file not exists
	int proceed( void );

	/// @{
	/// publishing Errors API

	/// # of errors
	Size errors();
	/// # of errors
	Size warnings();

	/// line of ith error or warning
	unsigned int errLine( unsigned int i );
	/// col of ith error or warning
	unsigned int errCol( unsigned int i );
	/// type of ith error or warning
	bool errIsError( unsigned int i );
	/// message of ith error or warning
	std::string errMsg( unsigned int i );

	/// send on std::cerr the list of errorswith contents
	void showElegantErrors();

	/// send on std::cerr the list of errors or warnings with contents
	void showElegantErrorsAndWarnings();

  /// send on std::cerr the list of errors or warnings
  void showErrorsAndWarnings();

	/// send on std::cerr the number of errors and the number of warnings
	void showErrorCounts();
	/// @}

    
    
protected:
	BayesNet<T_DATA>* __bn;
	BayesNetFactory<T_DATA>* __factory;
	DSL::Scanner* __scanner;
	DSL::Parser* __parser;

	std::string __streamName;
	bool __traceScanning;
	bool __parseDone;

	// a boolean to throw the ioerror not in the constructor but in the proceed()
	bool __ioerror;
      
  };


} /* namespace gum */

#include "DSLReader.tcc"

#endif //DSLREADER_H


// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 


