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
  m_current_line(-1),
  m_prm_taken(false)
{
}

/// This constructor use \a c as context.
SkoorInterpreter::SkoorInterpreter( SkoorContext * c ) :
  m_context(c),
  m_reader(0),
  m_inf(0),
  m_syntax_flag(false),
  m_verbose(false),
  m_log(cout),
  m_current_line(-1),
  m_prm_taken(false)
{
}

/// This constructor parse \a filename and use it as context.
SkoorInterpreter::SkoorInterpreter( const string & filename ) :
  m_context(new SkoorContext(filename)),
  m_reader(0),
  m_inf(0),
  m_syntax_flag(false),
  m_verbose(false),
  m_log(cout),
  m_current_line(-1),
  m_prm_taken(false)
{   
  // Add file directory to path
  size_t size = filename.find_last_of('/');
  if ( size != string::npos )
    m_paths.push_back( filename.substr(0, size + 1) ); // take with the '/'
  
  // Test if filename exist
  std::ifstream file_test;
  file_test.open(filename.c_str());
  if (file_test.is_open()) {
    file_test.close();
  
    skoor::Scanner s( filename.c_str() );
    
    Parser p( &s );
    p.setSkoorContext( m_context );
    p.Parse();
    
    m_errors += p.errors();
  } else
    addError("file not found.");    
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

/// Clear the current context (and errors).
void SkoorInterpreter::clearContext()
{
  m_context->clear();
  m_errors = ErrorsContainer();
  if ( m_reader ) {
	delete m_inf;
	m_inf = 0;
	delete m_reader->prm();
	delete m_reader;
	m_reader = 0;
  }
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
 * Crée le prm correspondant au contexte courant.
 * Renvoie true en cas de succès, ou false en cas échéant d'échec
 * de l'interprétation du contexte (import introuvable ou non défini,
 * etc).
 * */
bool SkoorInterpreter::interpret()
{
  // Don't parse if any syntax errors.
  if ( errors() > 0 )
    return false;
  
  // Create the reader for this time
  if ( m_reader ) {
	delete m_inf;
	m_inf = 0;
    delete m_reader->prm();
    delete m_reader;
  }
  m_prm_taken = false;
  m_reader = new skool::SkoolReader();  
  
  // Set paths to search from.
  for ( size_t i = 0 ; i < m_paths.size() ; i++ )
    m_reader->addClassPath( m_paths[i] );
  
  // On importe tous les systèmes.
  vector<ImportCommand *> imports = m_context->getImports();
  if (m_verbose) m_log << "## Load imports" << endl;
  for ( vector<ImportCommand *>::const_iterator i = imports.begin() ; i < imports.end() ; i++ ) {
    m_current_line = (*i)->line;
    // if import doen't succed stop here unless syntax mode is activated.
    if ( ! import( (*i)->value ) && ! isInSyntaxMode() )
      return false;
  }
  if (m_verbose) m_log << endl << endl;
  
  // For each session
  vector<SkoorSession *> sessions = m_context->getSessions();
  for ( vector<SkoorSession *>::const_iterator i = sessions.begin() ; i < sessions.end() ; i++ ) {
    
    if (m_verbose) m_log << "## Start session '" << (*i)->getName() << "'..." << endl << endl;
    
    // For each command
    vector<SkoorCommand *> commands = (*i)->getCommands();
    for ( vector<SkoorCommand *>::const_iterator j = commands.begin() ; j < commands.end() ; j++ ) {
      
      if ( m_verbose ) m_log << "# * Going to process command : " << (*j)->toString() << endl;
      
      // Update the current line (for warnings and errors)
      m_current_line = (*j)->line;
      
      // We process it.
      bool result = true;
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
        default :
          addError( "Error : Unknow command : " + 
            (*j)->toString() + "\n -> Command not processed.");
      }
      
      // If there was a problem, skip the rest of this session, 
      // unless syntax mode is activated.
      if ( ! result && ! isInSyntaxMode() ) {
        if (m_verbose) m_log << "Errors : skip the rest of this session." << endl;
        break;
      }
    }
    
    if (m_verbose) m_log << endl << "## Session '" << (*i)->getName() << "' finished." << endl << endl << endl;
  }
  
  return errors() == 0;
}


/**
* Parse the command line, in the previous context.
* Must be a package, import or request-block declaration.
* */
// Update le contexte en appelant le Parser () ou pas (donc directement) ?
void SkoorInterpreter::processCommandLine( const string & commandLine )
{
    
  skoor::Scanner s( ( unsigned char* ) commandLine.c_str(), ( int ) commandLine.length() );
  Parser p( &s );
  p.setSkoorContext( m_context );
  p.Parse();
  
  m_errors += p.errors();

  //if (m_verbose) m_log << "# * Going to process command : " << commandLine << endl << flush;
  
	//m_current_line = -1;
	//unsigned int size = commandLine.find(";");
	//if (size == string::npos)
		//addWarning("';' not found. Try to parse anyway, but there may be strange behaviour.");
	
	//unsigned int pos = string::npos;
	//if ( (pos = commandLine.find("=")) != string::npos ) {
		//string leftValue  = commandLine.substr(0, pos);
		//string rightValue = commandLine.substr(pos+1, size);
		
	//} else if ( commandLine.find("unobserve ") != string::npos ) {
		//string value = commandLine.substr(10, size);
		
	//} else if ( commandLine.find("engine ") != string::npos ) {
		//string value = commandLine.substr(7, size);
		
	//} else if ( commandLine.find("gnd_engine ") != string::npos ) {
		//string value = commandLine.substr(11, size);
		
	//} else if ( commandLine.find("? ") != string::npos ) {
		//string value = commandLine.substr(2, size);
		
	//} else {
		//addError("Fail to parse command. Did you miss the ';' on previous line ?");
	//}
}


/* ************************************************************************** */


// Import the system skool file
// Return false if any error.
bool SkoorInterpreter::import ( string import_name ) try {
  
  if ( m_verbose ) m_log << "# Loading system '" << import_name << "' => '" << flush;

  std::replace ( import_name.begin(), import_name.end(), '.', '/' );
  import_name += ".skool";
  
  if ( m_verbose ) m_log << import_name << "' ... " << endl << flush;
  
  std::ifstream file_test;
  bool found = false;
  string import_abs_filename;
  
  // Search in skoor file dir.
  string skoorFilename = m_context->getFilename();
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
  string package = m_context->getPackage();
  if ( ! package.empty() ) {
    string root;
    
    // if filename is not empty, start from it.
    string filename = m_context->getFilename();
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


std::string SkoorInterpreter::findSystemName ( const std::string& s ) {
  std::string name;
  size_t dot = s.find_first_of ( '.' );

  while ( dot != std::string::npos ) {
    name = s.substr ( 0, dot );

    if ( m_reader->prm()->isSystem ( name ) ) return name;

    dot = s.find ( '.', dot + 1 );
  }

  throw "could not find any system in " + s;
}

std::string SkoorInterpreter::findInstanceName ( const std::string& s, const std::string& sys ) {
  std::string name = s.substr ( sys.length() + 1 );
  size_t dot = name.find ( '.' );
  name = name.substr ( 0, dot );

  if ( ! m_reader->prm()->getSystem ( sys ).exists ( name ) )
    throw "could not find any instance in " + name;
  
  return name;
}

std::string SkoorInterpreter::findAttributeName ( const std::string & s, const gum::prm::Instance& instance ) {
  size_t dot = s.find_last_of ( '.' );

  if ( dot == std::string::npos )
    throw "could not find any attribute in " + s;
    
  std::string name = s.substr ( dot + 1, std::string::npos );
  
  if ( ! instance.exists ( name ) ) 
    throw "could not find any attribute in " + s;
    
  return name;

}


///
bool SkoorInterpreter::observe ( const ObserveCommand * command ) try {
  
  const std::string left_val  = command->leftValue;
  const std::string right_val = command->rightValue;
  
  // Contruct the pair (instance,attribut)
  const gum::prm::System& sys = m_reader->prm()->getSystem ( findSystemName ( left_val ) );
  const gum::prm::Instance& instance = sys.get ( findInstanceName ( left_val, sys.name() ) );
  const gum::prm::Attribute& attr = instance.get ( findAttributeName ( left_val, instance ) );
  gum::prm::PRMInference::Chain chain = std::make_pair ( &instance, &attr );

  if (isInSyntaxMode()) 
    return true;

  // Generate the inference engine if it doesn't exist.
  if ( ! m_inf )
    generateInfEngine( sys );

  // Prevent from something
  if ( m_inf->hasEvidence ( chain ) )
    addWarning ( left_val + " is already observed" );

  gum::Potential<gum::prm::prm_float> e;

  e.add ( chain.second->type().variable() );
  gum::Instantiation i ( e );
  bool found = false;

  // Set observation
  for ( i.setFirst(); not i.end(); i.inc() ) {
    if ( chain.second->type().variable().label ( i.val ( chain.second->type().variable() ) ) == right_val ) {
      e.set ( i, ( gum::prm::prm_float ) 1.0 );
      found = true;
    } else {
      e.set ( i, ( gum::prm::prm_float ) 0.0 );
    }
  }

  if (! found)
    throw right_val + " is not a label of " + left_val;

  // 
  m_inf->addEvidence ( chain, e );
  //affichage en demande
  
  if ( m_verbose ) m_log << "# Added evidence " << right_val << " over attribute " << left_val << endl;
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


///
bool SkoorInterpreter::unobserve( const UnobserveCommand * command ) try
{
  string name = command->value;
  
  // Contruct the pair (instance,attribut)
  const gum::prm::System& sys = m_reader->prm()->getSystem( findSystemName(name) );
  const gum::prm::Instance& instance = sys.get( findInstanceName(name, sys.name()) );
  const gum::prm::Attribute& attr = instance.get( findAttributeName(name, instance) );
     
  gum::prm::PRMInference::Chain chain = std::make_pair(&instance, &attr);
  
  if (isInSyntaxMode())
    return true;
    
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
	string query = command->value;
	  
	// Contruct the pair (instance,attribut)
  const gum::prm::System& sys = m_reader->prm()->getSystem ( findSystemName ( query ) );
  const gum::prm::Instance& instance = sys.get ( findInstanceName ( query, sys.name() ) );
  const gum::prm::Attribute& attr = instance.get ( findAttributeName ( query, instance ) );
  gum::prm::PRMInference::Chain chain = std::make_pair ( &instance, &attr );
  gum::Potential<gum::prm::prm_float> m;

	if ( isInSyntaxMode() )
		return;

	// Create inference engine if it has not been already created.
  if ( ! m_inf )
    generateInfEngine ( sys );
  
  
  // Inference
  if (m_verbose) m_log << "# Starting inference over query: " << query << "... " << flush;
  gum::Timer timer;
  timer.reset();
  m_inf->marginal ( chain, m );
  
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
