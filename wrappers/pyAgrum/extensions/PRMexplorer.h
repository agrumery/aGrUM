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
 * @brief This file contains definition for a PRMexplorer (read-only access to
 *PRM)
 *
 * @author Pierre-Henri WUILLEMIN
 */
#include <string>
#include <Python.h>

#include <agrum/PRM/PRM.h>
#include <agrum/PRM/o3prm/O3prmReader.h>

class PRMexplorer {
  public:
  /**
   * Create an explorer from a filename and a classpath
   * @param filename
   * @param classpath
   * @param verbose to print warnings and errors
   */
  PRMexplorer( std::string filename,
               std::string classpath = "",
               bool verbose = false ) {

    std::stringstream stream;
    gum::prm::o3prm::O3prmReader<double> reader;

    reader.setClassPath( classpath );

    auto nbErr = reader.readFile( filename );

    reader.showElegantErrorsAndWarnings( stream );
    if ( nbErr > 0 ) {
      reader.showErrorCounts( stream );
      GUM_ERROR( gum::FatalError, stream.str() );
    }
    if (verbose) {
      std::cout << stream.str() << std::endl;
    }

    __prm = reader.prm();
  };

  /**
   * @return a list of class names
   */
  PyObject* classes() {
    PyObject* q = PyList_New( 0 );

    for ( auto c : __prm->classes() )
      PyList_Append( q, PyString_FromString( c->name().c_str() ) );

    return q;
  };

  /**
   * @return a list of attribute names from a class
   * @param classname : the name of the class
   * @param allAttributes : even automatically generated attributes
   */
  PyObject* attributes( std::string classname, bool allAttributes = false ) {
    PyObject* q = PyList_New( 0 );

    for ( auto c : __prm->getClass( classname ).attributes() )
      if ( allAttributes ) {
        PyList_Append( q, PyString_FromString( c->safeName().c_str() ) );
      } else if ( &( __prm->getClass( classname ).get( c->name() ) ) ==
                  c )  // remove automatically created attributes
                       // (cast-descendant)
        PyList_Append( q, PyString_FromString( c->name().c_str() ) );

    return q;
  }
  /**
   * @return a list of attribute names from a class
   * @param classname : the name of the class
   * @param allAttributes : even automatically generated attributes
   */
  PyObject* references( std::string classname) {
    PyObject* q = PyList_New( 0 );

    for ( auto r : __prm->getClass( classname ).referenceSlots() )
      PyList_Append( q, PyString_FromString( r->name().c_str() ) );

    return q;
  }



  /**
   * @return a list of parameters from a class
   * @param classname : the name of the class
   */
  PyObject* parameters( std::string classname) {
    PyObject* q = PyList_New( 0 );

    for ( auto c : __prm->getClass( classname ).parameters() )
      PyList_Append( q, PyString_FromString( c->safeName().c_str() ) );

    return q;
  }



  /**
   * @return the potential of an attribute in a class
   * @param classname : the name of the class   *
   * @param attribute : the name of the attribute
   */
  const gum::Potential<double>& cpf( std::string classname,
                                     std::string attribute ) {
    return __prm->getClass( classname ).get( attribute ).cpf();
  }



  private:
  gum::prm::PRM<double>* __prm;
};
