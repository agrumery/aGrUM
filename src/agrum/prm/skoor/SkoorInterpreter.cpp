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
 * @brief Implementation of SkoolReader.
 *
 * @author Pierre-Henri WUILLEMIN, Ni NI, Lionel TORTI & Vincent RENAUDINEAU
 */
// =============================================================================
#include <agrum/prm/skoor/SkoorInterpreter.h>

#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/inference/VEWithBB.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/prm/SVED.h>
#include <agrum/prm/groundedInference.h>
#include <agrum/prm/SVE.h>
#undef COCO_PARSER_H__
#undef COCO_SCANNER_H__
#include <agrum/prm/skoor/cocoR/Parser.h>

namespace gum {

  namespace prm {

    namespace skoor {
    
using namespace std;
using namespace gum;
using namespace gum::prm;

/* ************************************************************************** */

/// This constructor create an empty context.
SkoorInterpreter::SkoorInterpreter() :
  m_context(new SkoorContext()),
  m_reader(0),
  m_inf(0),
  m_syntax_flag(false),
  m_verbose(false),
  m_log(cout),
  m_current_line(-1)
{
}

/// Destructor. Delete current context.
SkoorInterpreter::~SkoorInterpreter()
{
  delete m_context;
  delete m_inf;
  if ( m_reader )
    delete m_reader->prm();
  delete m_reader;
}

/* ************************************************************************** */
  
/// Getter for the context.
SkoorContext * SkoorInterpreter::getContext() const
{
  return m_context;
}

/// Setter for the context.
void SkoorInterpreter::setContext( SkoorContext * context )
{
  delete m_context;
  if (context == 0)
    m_context = new SkoorContext();
  else
    m_context = context;
}

/// Root paths to search from there packages.
/// Default are './' and one is calculate from request package if any.
vector<std::string> SkoorInterpreter::getPaths() const
{
  return m_paths;
}

/// Root paths to search from there packages.
/// Default are './' and one is calculate from request package if any.
void SkoorInterpreter::addPath( const std::string & path )
{
  m_paths.push_back(path);
}
  
/// Root paths to search from there packages.
/// Default are './' and one is calculate from request package if any.
void SkoorInterpreter::clearPaths()
{
  m_paths.clear();
}

/// syntax mode don't process anything, just check syntax.
bool SkoorInterpreter::isInSyntaxMode() const {
  return m_syntax_flag;
}

/// syntax mode don't process anything, just check syntax.
void SkoorInterpreter::setSyntaxMode ( bool f ) {
  m_syntax_flag = f;
}
    
/// verbose mode show more details on the program execution.
bool SkoorInterpreter::isVerboseMode() const
{
  return m_verbose;
}

/// verbose mode show more details on the program execution.
void SkoorInterpreter::setVerboseMode ( bool f )
{
  m_verbose = f;
}

/// Retrieve prm object.
const gum::prm::PRM* SkoorInterpreter::prm() const
{
    if ( m_reader ) {
        return m_reader->prm();
    } else
        return 0;
}

/// Retrieve inference motor object.
const gum::prm::PRMInference* SkoorInterpreter::inference() const
{
  return m_inf;
}

/// Return a vector of pair query/QueryResults.
/// Each QueryResults is a vector of pair label/value.
const std::vector< std::pair<std::string,QueryResult> > & SkoorInterpreter::results() const
{
  return m_results;
}
  

/**
 * Parse the file or the command line.
 * If errors occured, return false. Errors messages can be retrieve be 
 * getErrorsContainer() methods.
 * If any errors occured, return true.
 * Requests results can be retrieve be results() methods.
 * */
bool SkoorInterpreter::interpretFile( const string & filename )
{    
  m_results.clear();
  
  // Test if filename exist
  std::ifstream file_test;
  file_test.open(filename.c_str());
  if ( ! file_test.is_open() ) {
    addError("file '" + filename + "' not found. Check classPath");
    return false;
  }
  
  file_test.close();

  delete m_context;
  m_context = new SkoorContext(filename);
  SkoorContext c(filename);
  
  // On vérifie la syntaxe
  Scanner s( filename.c_str() );
  Parser p( &s );
  p.setSkoorContext( &c );
  p.Parse();
  
  m_errors = p.errors();
  if ( errors() > 0 )
    return false;
  // On vérifie la sémantique.
  if ( ! checkSemantic( &c ) )
    return false;
  
  if ( isInSyntaxMode() )
    return true;
  else
    return interpret(&c);
}

bool SkoorInterpreter::interpretLine( const string & line )
{  
  m_results.clear();
  
  // On vérifie la syntaxe
  SkoorContext c;
  Scanner s( ( unsigned char* ) line.c_str(), ( int ) line.length() );
  Parser p( &s );
  p.setSkoorContext( &c );
  p.Parse();
  m_errors = p.errors();
  if ( errors() > 0 )
    return false;
  
  // On vérifie la sémantique.
  if ( ! checkSemantic( &c ) )
    return false;
  
  if ( isInSyntaxMode() )
    return true;
  else
    return interpret(&c);
}

/**
 * Crée le prm correspondant au contexte courant.
 * Renvoie true en cas de succès, ou false en cas échéant d'échec
 * de l'interprétation du contexte (import introuvable ou non défini,
 * etc).
 * */
bool SkoorInterpreter::interpret( SkoorContext * c )
{
  if ( isVerboseMode() ) m_log << "## Start interpretation." << endl << flush; 
  
  // Don't parse if any syntax errors.
  if ( errors() > 0 )
    return false;
  
  // For each session
  vector<SkoorSession *> sessions = c->getSessions();
  for ( vector<SkoorSession *>::const_iterator i = sessions.begin() ; i < sessions.end() ; i++ ) {
    
    // For each command
    vector<SkoorCommand *> commands = (*i)->getCommands();
    for ( vector<SkoorCommand *>::const_iterator j = commands.begin() ; j < commands.end() ; j++ ) {
            
      // We process it.
      bool result = true;
      try{ 
      switch ( (*j)->type() ) {
        case SkoorCommand::Observe :
          result = observe( (ObserveCommand*) (*j) );
          break;
        case SkoorCommand::Unobserve :
          result = unobserve( (UnobserveCommand*) (*j) );
          break;
        case SkoorCommand::SetEngine :
          setEngine( (SetEngineCommand*) (*j) );
          break;
        case SkoorCommand::SetGndEngine :
          setGndEngine( (SetGndEngineCommand*) (*j) );
          break;
        case SkoorCommand::Query :
          query( (QueryCommand*) (*j) );
          break;
      } 
      } catch (gum::Exception&err) { result = false;addError(err.getContent()); 
      } catch (string&err) { result = false;addError(err); }
      
      // If there was a problem, skip the rest of this session, 
      // unless syntax mode is activated.
      if ( ! result ) {
        if (m_verbose) m_log << "Errors : skip the rest of this session." << endl;
        break;
      }
      
    }
    
  }
  
  if ( isVerboseMode() ) m_log << "## End interpretation." << endl << flush; 
  
  return errors() == 0;
}


/* ************************************************************************** */

/**
 * Check semantic validity of context.
 * Import first all import, and check that systems, instances, attributes and labels exists.
 * While checking, prepare data structures for interpretation.
 * Return true if all is right, false otherwise.
 * 
 * Note : Stop checking at first error unless syntax mode is activated.
 * */
bool SkoorInterpreter::checkSemantic( SkoorContext * context )
{
  // Don't parse if any syntax errors.
  if ( errors() > 0 )
    return false;
  
  if ( ! m_reader ) {
    // Create the reader
    m_reader = new skool::SkoolReader();
    
    // Set paths to search from.
    for ( size_t i = 0 ; i < m_paths.size() ; i++ )
      m_reader->addClassPath( m_paths[i] );
  }
  
  // On importe tous les systèmes.
  vector<ImportCommand *> imports = context->getImports();
  for ( vector<ImportCommand *>::const_iterator i = imports.begin() ; i < imports.end() ; i++ ) {
    m_current_line = (*i)->line;
    // if import doen't succed stop here unless syntax mode is activated.
    bool succeed = import( context, (*i)->value );
    if ( ! succeed && ! isInSyntaxMode() )
      return false;
    // En cas de succès, on met à jour le contexte global
    if ( succeed )
      m_context->addImport( **i );
  }
  
  // On vérifie chaque session
  vector<SkoorSession *> sessions = context->getSessions();
  if (m_verbose) m_log << "## Check semantic for " << sessions.size() << " sessions" << endl;
  for ( vector<SkoorSession *>::const_iterator i = sessions.begin() ; i < sessions.end() ; i++ ) {
    
    string sessionName = (*i)->getName();
    SkoorSession * session = new SkoorSession(sessionName);
    
    if (m_verbose) m_log << "## Start session '" << sessionName << "'..." << endl << endl;
    
    // For each command
    vector<SkoorCommand *> commands = (*i)->getCommands();
    for ( vector<SkoorCommand *>::const_iterator j = commands.begin() ; j < commands.end() ; j++ ) {
      
      if ( m_verbose ) m_log << "# * Going to check command : " << (*j)->toString() << endl;
      
      // Update the current line (for warnings and errors)
      m_current_line = (*j)->line;
      
      // We check it.
      bool result = true;
      try{ 
      switch ( (*j)->type() ) {
        case SkoorCommand::SetEngine :
          result = checkSetEngine( (SetEngineCommand*) (*j) );
          break;
        case SkoorCommand::SetGndEngine :
          result = checkSetGndEngine( (SetGndEngineCommand*) (*j) );
          break;
        case SkoorCommand::Observe :
          result = checkObserve( (ObserveCommand*) (*j) );
          break;
        case SkoorCommand::Unobserve :
          result = checkUnobserve( (UnobserveCommand*) (*j) );
          break;
        case SkoorCommand::Query :
          result = checkQuery( (QueryCommand*) (*j) );
          break;
        default :
          addError( "Error : Unknow command : " + 
            (*j)->toString() + "\n -> Command not processed.");
          result = false;
      }
      } catch ( gum::Exception & err ) { result = false; addError(err.getContent()); 
      } catch ( string & err ) { result = false; addError(err); }
      
      // If there was a problem, skip the rest of this session, 
      // unless syntax mode is activated.
      if ( ! result && ! isInSyntaxMode() ) {
        if (m_verbose) m_log << "Errors : skip the rest of this session." << endl;
        break;
      }
      
      // On l'ajoute au contexte globale
      if ( result )
        session->addCommand( (const SkoorCommand*) (*j) );
    }
    
    // Ajoute la session au contexte global,
    // ou à la dernière session.
    if ( sessionName == "default" && m_context->getSessions().size() > 0 )
      *(m_context->getSessions().back()) += *session;
    else
      m_context->addSession( session );
    
    if (m_verbose) m_log << endl << "## Session '" << sessionName << "' finished." << endl << endl << endl;
  }
  
  return errors() == 0;
}

bool SkoorInterpreter::checkSetEngine( SetEngineCommand * command )
{
  string name = command->value;
  return name == "SVED" || name == "GRD" || name == "SVE";
}

bool SkoorInterpreter::checkSetGndEngine( SetGndEngineCommand * command )
{
  string name = command->value;
  return name == "VE" || name == "VEBB" || name == "lazy";
}

bool SkoorInterpreter::checkObserve( ObserveCommand * command )
{
  try {
    std::string left_val  = command->leftValue;
    const std::string right_val = command->rightValue;
    
    // Contruct the pair (instance,attribut)
    const gum::prm::System& sys = getSystem( left_val );
    const gum::prm::Instance& instance = sys.get ( findInstanceName ( left_val, sys ) );
    const gum::prm::Attribute& attr = instance.get ( findAttributeName ( left_val, instance ) );
    gum::prm::PRMInference::Chain chain = std::make_pair ( &instance, &attr );
    
    command->system = &sys;
    command->chain = std::make_pair ( &instance, &attr );
    
    // Check label exists for this type.
    gum::Potential<gum::prm::prm_float> e;
    e.add ( chain.second->type().variable() );
    gum::Instantiation i ( e );
    bool found = false;
    for ( i.setFirst(); not i.end(); i.inc() ) {
      if ( chain.second->type().variable().label ( i.val ( chain.second->type().variable() ) ) == right_val ) {
        e.set ( i, ( gum::prm::prm_float ) 1.0 );
        found = true;
      } else {
        e.set ( i, ( gum::prm::prm_float ) 0.0 );
      }
    }

    if ( ! found ) addError( right_val + " is not a label of " + left_val );
    else command->potentiel = e;
    
    return found;
    
  } catch ( gum::Exception & err ) { addError(err.getContent()); 
  } catch ( string & err ) { addError(err); }
  return false;
}

bool SkoorInterpreter::checkUnobserve( UnobserveCommand * command )
{
  try {
    string name = command->value;
    
    // Contruct the pair (instance,attribut)
    const gum::prm::System& sys = getSystem( name );
    const gum::prm::Instance& instance = sys.get( findInstanceName(name, sys) );
    const gum::prm::Attribute& attr = instance.get( findAttributeName(name, instance) );
    gum::prm::PRMInference::Chain chain = std::make_pair(&instance, &attr);
    
    command->system = &sys;
    command->chain = std::make_pair ( &instance, &attr );

    return true;
    
  } catch ( gum::Exception & err ) { addError(err.getContent()); 
  } catch ( string & err ) { addError(err); }
  return false;
}

bool SkoorInterpreter::checkQuery( QueryCommand * command )
{
  try {
    string name = command->value;
    
    // Contruct the pair (instance,attribut)
    const gum::prm::System& sys = getSystem( name );
    const gum::prm::Instance& instance = sys.get( findInstanceName(name, sys) );
    const gum::prm::Attribute& attr = instance.get( findAttributeName(name, instance) );
    gum::prm::PRMInference::Chain chain = std::make_pair(&instance, &attr);
    
    command->system = &sys;
    command->chain = std::make_pair ( &instance, &attr );

    return true;
    
  } catch ( gum::Exception & err ) { addError(err.getContent()); 
  } catch ( string & err ) { addError(err); }
  return false;
}

// Import the system skool file
// Return false if any error.
bool SkoorInterpreter::import ( SkoorContext * context, string import_name ) try {
  
  if ( m_verbose ) m_log << "# Loading system '" << import_name << "' => '" << flush;

  std::replace ( import_name.begin(), import_name.end(), '.', '/' );
  import_name += ".skool";
  
  if ( m_verbose ) m_log << import_name << "' ... " << endl << flush;
  
  std::ifstream file_test;
  bool found = false;
  string import_abs_filename;
  
  // Search in skoor file dir.
  string skoorFilename = context->getFilename();
  if ( ! skoorFilename.empty() ) {
		size_t index = skoorFilename.find_last_of('/');
		if ( index != string::npos ) {
			string dir = skoorFilename.substr( 0, index+1 );
			import_abs_filename = dir + import_name;
			if ( m_verbose ) m_log << "# Search from filedir '" << import_abs_filename << "' ... " << flush;
			file_test.open(import_abs_filename.c_str());
			if (file_test.is_open()) {
				if ( m_verbose ) m_log << "found !" << endl << flush;
				file_test.close();
				found = true;      
			} else if ( m_verbose )
				m_log << "not found." << endl << flush;
		}
  }
  
  // Deduce root path from package name.
  string package = context->getPackage();
  if ( ! found && ! package.empty() ) {
    string root;
    
    // if filename is not empty, start from it.
    string filename = context->getFilename();
    if ( ! filename.empty() ) {
      size_t size = filename.find_last_of('/');
      if ( size != string::npos )
        root += filename.substr(0, size + 1); // take with the '/'
    }
    
    //
    root += "../";
    int count = (int) std::count( package.begin(), package.end(), '.' );
    for ( int i = 0 ; i < count ; i++ )
      root += "../";
    
    import_abs_filename = root + import_name;
		if ( m_verbose ) m_log << "# Search from package '" << package << "' => '" << import_abs_filename << "' ... " << flush;
		file_test.open(import_abs_filename.c_str());
		if (file_test.is_open()) {
			if ( m_verbose ) m_log << "found !" << endl << flush;
			file_test.close();
			found = true;      
		} else if ( m_verbose )
			m_log << "not found." << endl << flush;
  }
  
  // Search import in all paths.  
  for (vector<string>::iterator i = m_paths.begin(); ! found && i != m_paths.end(); i++) {
    import_abs_filename = (*i) + import_name;
    if ( m_verbose ) m_log << "# Search from classpath '" << import_abs_filename << "' ... " << flush;
    file_test.open(import_abs_filename.c_str());
    if (file_test.is_open()) {
      if ( m_verbose ) m_log << " found !" << endl << flush;
      file_test.close();
      found = true;
    } else if ( m_verbose ) 
      m_log << " not found." << endl << flush;
  }
  
  if (not found) {
    if (m_verbose) m_log << "Finished with errors." << endl;
    addError("import not found.");
    return false;
  }
  
  // May throw std::IOError if file does't exist
  int previousSkoolError = m_reader->errors();
  int previousSkoorError = errors();
  try {
    m_reader->readFile(import_abs_filename);
    
    // Show errors and warning
    if ( m_verbose && (m_reader->errors() > (unsigned int) previousSkoolError || errors() > previousSkoorError) )
      m_log << "Finished with errors." << endl;
    else if ( m_verbose )
      m_log << "Finished." << endl;
      
  } catch ( const gum::IOError & err ) {
    if (m_verbose) m_log << "Finished with errors." << endl;    
    addError( err.getContent() );
  }
    
  // Add skool errors and warnings to skoor errors
  for ( ; previousSkoolError < m_reader->getErrorsContainer().count() ; previousSkoolError++ )
    m_errors.add( m_reader->getErrorsContainer().getError( previousSkoolError ) );
  
  return errors() == previousSkoorError;
  
} catch ( const gum::Exception & err ) {
  if ( m_verbose ) m_log << "Finished with exceptions." << endl;
  addError( err.getContent() );
  return false;
}


std::string SkoorInterpreter::findSystemName ( std::string & s ) {
  size_t dot = s.find_first_of ( '.' );
  std::string name = s.substr ( 0, dot );
  
  // We look first for real system, next for alias.
  if ( m_reader->prm()->isSystem ( name ) ) {
    s = s.substr( dot+1 );
    return name;
  }
  if ( ! m_context->aliasToImport( name ).empty() ) {
    s = s.substr( dot+1 );
    return m_context->aliasToImport( name );
  }

  while ( dot != std::string::npos ) {
    if ( m_reader->prm()->isSystem ( name ) ) {
      s = s.substr( dot+1 );
      return name;
    }

    dot = s.find ( '.', dot + 1 );
    name = s.substr ( 0, dot );
  }

  throw "could not find any system in '" + s + "'.";
}

std::string SkoorInterpreter::findInstanceName ( std::string& s, const gum::prm::System& sys ) {
  // We have found system before, so 's' has been stripped.
  size_t dot = s.find_first_of ( '.' );
  std::string name = s.substr ( 0, dot );
  if ( ! sys.exists(name) )
    throw "'" + name + "' is not an instance of system '" + sys.name() +"'.";
  s = s.substr( dot+1 );
  return name;
}

std::string SkoorInterpreter::findAttributeName ( const std::string & s, const gum::prm::Instance& instance ) {
  if ( ! instance.exists ( s ) ) 
    throw "'" + s + "' is not an attribute of instance '" + instance.name() +"'.";
  return s;

}

// After this method, ident doesn't contains the system name anymore.
const System & SkoorInterpreter::getSystem( string & ident ) {
  try {
    return m_reader->prm()->getSystem ( findSystemName (ident) );
  } catch ( const string & ) {}
  if ( m_context->getMainImport() != 0 && m_reader->prm()->isSystem(m_context->getMainImport()->value) )
    return m_reader->prm()->getSystem ( m_context->getMainImport()->value );
  
  throw "could not find any system or alias in '" + ident + "' and no default alias has been set.";
}

///
bool SkoorInterpreter::observe ( const ObserveCommand * command ) try {
  
  const PRMInference::Chain & chain = command->chain;
  
  // Generate the inference engine if it doesn't exist.
  if ( ! m_inf )
    generateInfEngine( *(command->system) );

  // Prevent from something
  if ( m_inf->hasEvidence ( chain ) )
    addWarning ( command->leftValue + " is already observed" );

  m_inf->addEvidence ( chain, command->potentiel );
  
  if ( m_verbose ) m_log << "# Added evidence " << command->rightValue << " over attribute " << command->leftValue << endl;
  return true;
    
} catch ( gum::OperationNotAllowed & ex ) {
  addError( "someting went wrong when adding evidence " + command->rightValue +
        " over " + command->leftValue + " : " + ex.getContent() );
  return false;
  
} catch (const string & msg ) {
  addError( msg );
  return false;
}


///
bool SkoorInterpreter::unobserve( const UnobserveCommand * command ) try
{
  string name = command->value;
  gum::prm::PRMInference::Chain chain = command->chain;
    
  // Prevent from something
  if ( ! m_inf->hasEvidence(chain) ) {
    addWarning(name + " was not observed");
  } else {
    m_inf->removeEvidence(chain);
    if (m_verbose) m_log << "# Removed evidence over attribute " << name << std::endl;
  }
  
  return true;

} catch ( const string & msg ) {
  addError( msg );
  return false;
}


///
void SkoorInterpreter::query ( const QueryCommand * command ) try 
{
	const string & query = command->value;
  const Attribute & attr = *(command->chain.second);
  
  gum::Potential<gum::prm::prm_float> m;
  
	// Create inference engine if it has not been already created.
  if ( ! m_inf )
    generateInfEngine ( *(command->system) );
  
  // Inference
  if (m_verbose) m_log << "# Starting inference over query: " << query << "... " << flush;
  gum::Timer timer;
  timer.reset();
  
  m_inf->marginal ( command->chain, m );
  
  // Compute spent time
  double t = timer.step();
  if (m_verbose) m_log << "Finished." << endl;
  if (m_verbose) m_log << "# Time in seconds (accuracy ~0.001): " << t << endl;
  
  // Show results
  gum::Instantiation j ( m );
  if (m_verbose) m_log << endl;
  QueryResult result;
  for ( j.setFirst (); not j.end (); j.inc () ) {
    string label = attr.type().variable().label ( j.val ( attr.type().variable() ) );
    float value = m.get ( j );
    result.push_back( make_pair(label, value) );
    if (m_verbose) m_log << label << " : " << value << endl;
  }
  m_results.push_back( make_pair(query,result) );
  if (m_verbose) m_log << endl;
  
} catch ( gum::Exception& e ) {
  throw "something went wrong while infering: " + e.getContent();
  
} catch ( const string & msg ) {
  addError ( msg );
}


///
void SkoorInterpreter::setEngine( const SetEngineCommand * command )
{
	m_engine = command->value;
}


///
void SkoorInterpreter::setGndEngine( const SetGndEngineCommand * command )
{
	m_bn_engine = command->value;
}


///
void SkoorInterpreter::generateInfEngine(const gum::prm::System& sys) 
{  
  if ( m_verbose ) m_log << "# Building the inference engine... " << std::flush;
  
  //
  if ( m_inf ) {
    addWarning("an inference engine has already been defined.");
    delete m_inf;
    m_inf = 0;
  }
  
  //
  if ( m_engine == "SVED" ) {
    m_inf = new gum::prm::SVED( *(m_reader->prm()), sys);
    
  //
  } else if (m_engine == "GRD") {
    
    typedef gum::prm::prm_float p_f;
    gum::BayesNetInference<p_f>* bn_inf = 0;
    gum::BayesNet<p_f>* bn = new gum::BayesNet<p_f>();
    gum::BayesNetFactory<p_f> bn_factory(bn);
    
    if (m_verbose) m_log << "(Grounding the network... " << flush;
    sys.groundedBN(bn_factory);    
    if (m_verbose) m_log << "Finished)" << flush;
    
    if (m_bn_engine == "VE") {
      bn_inf = new gum::VariableElimination<p_f>(*bn);
    } else if (m_bn_engine == "VEBB") {
      bn_inf = new gum::VEWithBB<p_f>(*bn);
    } else if (m_bn_engine == "lazy") {
      bn_inf = new gum::LazyPropagation<p_f>(*bn);
    } else {
      bn_inf = new gum::ShaferShenoyInference<p_f>(*bn);
    }
    
    gum::prm::GroundedInference* grd_inf = new gum::prm::GroundedInference(*(m_reader->prm()), sys);
    grd_inf->setBNInference(bn_inf);
    m_inf = grd_inf;
    
  //
  } else {
    if (m_engine != "SVE")
      addWarning("unkown engine '" + m_engine + "', use SVE insteed.");
    m_inf = new gum::prm::SVE(*(m_reader->prm()), sys);
  }
  
  if (m_verbose) m_log << "Finished." << std::endl;
}


/* ************************************************************************** */

/// # of errors + warnings
int SkoorInterpreter::count() const
{
  return m_errors.count();
}
  
///
int SkoorInterpreter::errors() const {
  return m_errors.error_count;
}

///
int SkoorInterpreter::warnings() const {
  return m_errors.warning_count;
}

/// 
ParseError SkoorInterpreter::getError( int i ) const
{
  if ( i >= count() )
    throw "Index out of bound.";
  return m_errors.getError( i );
}

/// Return container with all errors.
ErrorsContainer SkoorInterpreter::getErrorsContainer() const
{
    return m_errors;
}
  
///
void SkoorInterpreter::showElegantErrors() const {
  m_errors.showElegantErrors();
}

///
void SkoorInterpreter::showElegantErrorsAndWarnings() const {
  m_errors.showElegantErrorsAndWarnings();
}

///
void SkoorInterpreter::showErrorCounts() const {
  cerr << "Errors : "   << m_errors.error_count   << endl;
  cerr << "Warnings : " << m_errors.warning_count << endl;
}


/* ************************************************************************** */

///
void SkoorInterpreter::addError( string msg )
{
  m_errors.addError( msg, m_context->getFilename(), m_current_line, 0 );
  if (m_verbose) m_log << m_errors.last().toString() << endl;
}

///
void SkoorInterpreter::addWarning( string msg )
{
  m_errors.addWarning( msg, m_context->getFilename(), m_current_line, 0 );
  if (m_verbose) m_log << m_errors.last().toString() << endl;
}

    } // namespace skoor
  } // namespace prm
} // namespace gum
