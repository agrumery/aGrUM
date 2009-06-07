/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Inline implementation of SkoobDriver.
 *
 * @author Lionel Torti
 */
// ============================================================================
namespace gum_bif {

// Parse a stream using a BIFParser.
INLINE
bool
BIFDriver::parseStream(std::istream& in, const std::string& sname)
{
  __streamName = sname;
  BIFScanner scanner(&in);
  scanner.set_debug(traceScanning());
  __lexer = &scanner;
  BIFParser parser(*this);
  parser.set_debug_level(traceParsing());
  return (parser.parse() == 0);
}

// Parse a string using a BIFParser.
INLINE
bool
BIFDriver::parseString(const std::string& input, const std::string& sname)
{
  std::istringstream iss(input);
  return parseStream(iss, sname);
}

// Parse a file using a BIFScanner.
INLINE
bool
BIFDriver::parseFile(const std::string& filename)
{
  std::ifstream in(filename.c_str());
  if (!in.good()) {
    return false;
  }
  return parseStream(in, filename);
}

//  Returns the lexer.
INLINE
BIFScanner&
BIFDriver::lexer() { return *__lexer; }

// Returns the PRM factory.
INLINE
gum::BayesNetFactory<double>&
BIFDriver::factory() { return *__factory; }

// Returns the stream name.
INLINE
const std::string&
BIFDriver::streamName() const { return __streamName; }

// Returns the trace scanning flag.
INLINE
bool 
BIFDriver::traceScanning() const { return __traceScanning; }

// Sets the trace scanning flat.
INLINE
void 
BIFDriver::traceScanning(bool b) { __traceScanning = b; }

// Returns the trace parsing flag.
INLINE
bool 
BIFDriver::traceParsing() const { return __traceParsing; }

// Sets the trace parsing flag.
INLINE
void 
BIFDriver::traceParsing(bool b) { __traceParsing = b; }

// Prints an error.
INLINE
void
BIFDriver::error(const class location& l, const std::string& m)
{
  std::cerr << streamName()  << "|" << l.begin.line;
  std::cerr << "| Error: " << m << std::endl;
}

// Prints an error.
INLINE
void
BIFDriver::error(const std::string& m)
{
  std::cerr << m << std::endl;
}

} /* namespace gum_bif */
// ============================================================================
