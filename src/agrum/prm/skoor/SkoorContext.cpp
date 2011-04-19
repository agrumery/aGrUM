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
// ============================================================================

#include "SkoorContext.h"

namespace gum {
  namespace prm {
    namespace skoor {
    
using namespace std;

/* ******************************************************************* */

SkoorContext::SkoorContext(const std::string & filename)
{
  m_filename = filename;
  m_mainImport = 0;
}

SkoorContext::SkoorContext( const SkoorContext & s )
{
  m_filename = s.m_filename;
  m_mainImport = s.m_mainImport;
  *this += s;
}
  
SkoorContext::~SkoorContext()
{
  clear();
}
  
string SkoorContext::getFilename() const
{
  return m_filename;
}
  
string SkoorContext::getPackage() const
{
  return m_package;
}

void SkoorContext::setPackage( const string & package )
{
  m_package = package;
}

string SkoorContext::aliasToImport( const string & alias )
{
  for ( int i = m_imports.size() - 1 ; i >= 0 ; i-- )
    if ( m_imports[i]->alias == alias )
      return m_imports[i]->value;
  return string();
}

vector<ImportCommand *> SkoorContext::getImports() const
{
  return m_imports;
}

void SkoorContext::addImport( int line, const std::string & import, const std::string & alias )
{
  m_imports.push_back( new ImportCommand(line, import, alias) );
  if ( alias == "default" )
    m_mainImport = m_imports.back();
}

void SkoorContext::addImport( int line, const std::string & import, bool ismain )
{
  m_imports.push_back( new ImportCommand(line, import, import) );
  if ( ismain )
    m_mainImport = m_imports.back();
}
  
std::vector<SkoorSession *> SkoorContext::getSessions() const
{
  return m_sessions;
}

void SkoorContext::addSession( SkoorSession * session )
{
  m_sessions.push_back( session );
}

void SkoorContext::addSession( const SkoorSession & session )
{
  addSession( new SkoorSession(session) );
}
  
void SkoorContext::clear()
{
  m_package = "";
  for ( int i = m_imports.size() - 1 ; i >= 0 ; i-- )
    delete m_imports[i];
  m_imports.clear();
  for ( int i = m_sessions.size() - 1 ; i >= 0 ; i-- )
    delete m_sessions[i];
  m_sessions.clear();
}
  
string SkoorContext::toString() const
{
  string output;
  
  if ( ! m_package.empty() ) {
    output += "package " + m_package + ";\n";
    output += "\n";
  }
  
  for ( vector<ImportCommand *>::const_iterator i = m_imports.begin() ; i < m_imports.end() ; i++ )
    output += (*i)->toString() + "\n";
  output += "\n";
  
  for ( vector<SkoorSession *>::const_iterator i = m_sessions.begin() ; i < m_sessions.end() ; i++ )
    output += (*i)->toString() + "\n";
    
  return output;
}

SkoorContext & SkoorContext::operator+=( const SkoorContext & c )
{
  const std::vector<ImportCommand *> & imports = c.getImports();
  for ( std::vector<ImportCommand *>::const_iterator i = imports.begin() ; i != imports.end() ; i++ )
    addImport( **i ) ;
    
  const std::vector<SkoorSession *> & sessions = c.getSessions();
  if ( sessions.size() == 1 && sessions.back()->getName() == "default" ) {
    *(this->m_sessions.back()) += *(sessions.back());
  } else 
    for ( std::vector<SkoorSession *>::const_iterator i = sessions.begin() ; i != sessions.end() ; i++ )
      addSession( **i ) ;
  return *this;
}
  
/* ******************************************************************* */

SkoorSession::SkoorSession( const string & name )
{
  m_name = name;
}

SkoorSession::SkoorSession( const SkoorSession & s )
{
  m_name = s.m_name;
  *this += s;
}

SkoorSession::~SkoorSession()
{
  for ( int i = m_commands.size() - 1 ; i >= 0 ; i-- )
    delete m_commands[i];
  m_commands.clear();
}

string SkoorSession::getName() const
{
  return m_name;
}

vector<SkoorCommand *> SkoorSession::getCommands() const
{
  return m_commands;
}

void SkoorSession::addCommand( SkoorCommand * command )
{
  m_commands.push_back( command );
}

void SkoorSession::addObserve( int line, const string & leftValue, const string & rightValue )
{
  addCommand( new ObserveCommand(line,leftValue, rightValue) );
}

void SkoorSession::addUnobserve( int line, const string & value )
{
  addCommand( new UnobserveCommand(line,value) );
}

void SkoorSession::addQuery( int line, const string & value )
{
  addCommand( new QueryCommand(line,value) );
}

void SkoorSession::addSetEngine( int line, const string & value )
{
  addCommand( new SetEngineCommand(line,value) );
}

void SkoorSession::addSetGndEngine( int line, const string & value )
{
  addCommand( new SetGndEngineCommand(line,value) );
}
  
void SkoorSession::addCommand( const SkoorCommand * command )
{
  switch ( command->type() ) {
    case SkoorCommand::SetEngine :
      m_commands.push_back( new SetEngineCommand(*(SetEngineCommand*) command) );
      break;
    case SkoorCommand::SetGndEngine :
      m_commands.push_back( new SetGndEngineCommand(*(SetGndEngineCommand*) command) );
      break;
    case SkoorCommand::Observe :
      m_commands.push_back( new ObserveCommand(*(ObserveCommand*) command) );
      break;
    case SkoorCommand::Unobserve :
      m_commands.push_back( new UnobserveCommand(*(UnobserveCommand*) command) );
      break;
    case SkoorCommand::Query :
      m_commands.push_back( new QueryCommand(*(QueryCommand*) command) );
      break;
  }
}
  
string SkoorSession::toString() const
{
  string output;
  
  output += "request " + m_name + " {\n";
  for ( vector<SkoorCommand *>::const_iterator i = m_commands.begin() ; i < m_commands.end() ; i++ )
    output += "\t" + (*i)->toString() + "\n";
  output += "}\n";
  
  return output;
}

SkoorSession & SkoorSession::operator+=( const SkoorSession & c )
{
  for ( vector<SkoorCommand *>::const_iterator i = c.m_commands.begin() ; i < c.m_commands.end() ; i++ )
    addCommand( *i );
  return *this;
}

/* ******************************************************************* */

    } // namespace skoor
  } // namespace prm
} // namespace gum
