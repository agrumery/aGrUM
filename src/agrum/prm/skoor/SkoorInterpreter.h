
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
 * @brief Headers of SkoolInterpreter
 *
 * @author Pierre-Henri WUILLEMIN, Ni NI, Lionel TORTI & Vincent RENAUDINEAU
 */

#ifndef SKOORINTERPRETER_H
#define SKOORINTERPRETER_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <agrum/prm/skoor/SkoorContext.h>
#include <agrum/prm/skool/SkoolReader.h>
#include <agrum/prm/PRMInference.h>
#include <agrum/core/errorsContainer.h>
#endif //DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  namespace prm {

    namespace skoor {

typedef std::vector< std::pair<std::string,float> > QueryResult;

/**
 * Permet de manipuler un contexte skoor
 * et d'interpréter celui-ci.
 */
class SkoorInterpreter {
  
public:
  /// This constructor create an empty context.
  SkoorInterpreter();
  /// Destructor. Delete current context.
  ~SkoorInterpreter();
  
  /**
   * Interpret the file or the command line.
   * If syntax mode is activated, check only syntax and semantic.
   * If errors occured, return false. Errors messages can be retrieve be 
   * getErrorsContainer() methods.
   * If any errors occured, return true.
   * Requests results can be retrieve be results() methods.
   * */
  bool interpretFile( const string & filename );
  bool interpretLine( const string & line );
  
  /// Retrieve prm object.
  const gum::prm::PRM* prm() const;
  /// Retrieve inference motor object.
  const gum::prm::PRMInference* inference() const;
  /// Return a vector of pair query/QueryResults.
  /// Each QueryResults is a vector of pair label/value.
  const std::vector< std::pair<std::string,QueryResult> > & results() const;
  /// Return container with all errors.
  ErrorsContainer getErrorsContainer() const;
  
  /// Getter and setter for the context.
  SkoorContext * getContext() const;
  void setContext( SkoorContext * context );
  
  /// Root paths to search from there packages.
  /// Default are working dir, request file dir if any 
  /// and one is calculate from package if any.
  std::vector<std::string> getPaths() const;
  void addPath( const std::string & path );
  void clearPaths();
  
  /// syntax mode don't process anything, just check syntax. Default is false.
  bool isInSyntaxMode() const;
  void setSyntaxMode ( bool f );
  
  /// verbose mode show more details on the program execution. Default is false.
  bool isVerboseMode() const;
  void setVerboseMode ( bool f );
  
  /**
   * En cas d'échec, l'API de gestion d'erreurs est présente.
   * */
  /// # of errors + warnings
  int count() const;
  /// # of errors
  int errors() const;
  /// # of warnings
  int warnings() const;
  /// throw a string error if i >= count
  ParseError getError( int i ) const;
  /// send on std::cerr the list of errors
  void showElegantErrors() const;
  /// send on std::cerr the list of errors or warnings
  void showElegantErrorsAndWarnings() const;
  /// send on std::cerr the number of errors and the number of warnings
  void showErrorCounts() const;
  
private:
  bool checkSemantic( SkoorContext * context );
  bool checkSetEngine( SetEngineCommand * command );
  bool checkSetGndEngine( SetGndEngineCommand * command );
  bool checkObserve( ObserveCommand * command );
  bool checkUnobserve( UnobserveCommand * command );
  bool checkQuery( QueryCommand * command );

  bool interpret( SkoorContext * c );
  bool import( SkoorContext * context, std::string import );
  bool observe( const ObserveCommand * command );
  bool unobserve( const UnobserveCommand * command );
  void query( const QueryCommand * command );
  void setEngine( const SetEngineCommand * command );
  void setGndEngine( const SetGndEngineCommand * command );

  std::string findSystemName ( std::string & s );
  std::string findInstanceName ( std::string& s, const gum::prm::System& sys );
  std::string findAttributeName ( const std::string& s, const gum::prm::Instance& instance );
  const System & getSystem( string & ident );
  void generateInfEngine(const gum::prm::System& sys);
  
  void addError( std::string msg );
  void addWarning( std::string msg );
  
  SkoorContext * m_context;
  gum::prm::skool::SkoolReader * m_reader; 
  std::vector<std::string> m_paths;
  gum::prm::PRMInference* m_inf;
  std::string m_engine;
  std::string m_bn_engine;
  std::vector< std::pair<std::string,QueryResult> > m_results;
  gum::ErrorsContainer m_errors;
  bool m_syntax_flag;
  bool m_verbose;
  std::ostream& m_log;
  int m_current_line;
  
  //// We need this when using grounded inference
  //gum::BayesNetInference<gum::prm::prm_float>* bn;
          
}; // class SkoorInterpreter

    } // namespace skoor
  } // namespace prm
} // namespace gum

#endif //GUM_SKOORINTERPRETER_H
