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
 * @brief Definition of abstract classes for file input  manipulation
 * of Factored Markov Decision Process.
 *
 * Every classe used to read a FMDP from a file, must inherit from
 * FMDPReader.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_FMDP_READER_H
#define GUM_FMDP_READER_H

#include <iostream>
#include <string>
#include <agrum/config.h>
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>

namespace gum {
  /* ============================================================================ */
  /* ===                               READERS                                === */
  /* ============================================================================ */
  /**
   * @class FMDPReader
   * @brief Pure virtual class for reading a FMDP from a file.
   * @ingroup fmdp_group
   * Every class used to read the content of a Factored Markov Decision Process from
   * a stream,
   * or a file must be a subclass of FMDPReader.
   */
  template <typename GUM_SCALAR> class FMDPReader {
    public:
    /**
     * Constructor
     * A reader is defined for reading a defined file. Hence the 2 args of the
     * constructor.
     * Note that the FMDP has to be built outside the reader. There is no delegation
     * to create/destroy
     * the FMDP from inside the reader.
     */
    FMDPReader(FactoredMarkovDecisionProcess<GUM_SCALAR> *fmdp,
               const std::string &filename);

    /**
     * Default destructor.
     */
    virtual ~FMDPReader();

    /**
     * Reads a Factored Markov Decision Process from the file referenced by filePath
     * into`
     * parameter fmdp.
     * @return Returns the number of error during the parsing (0 if none).
     */
    virtual int proceed() = 0;
  };

  extern template class FMDPReader<float>;
  extern template class FMDPReader<double>;

} /* namespace gum */

#include <agrum/FMDP/io/FMDPReader.tcc>

#endif // GUM_FMDP_READER_H
