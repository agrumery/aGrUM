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
 * @brief Implementation of O3prmReader.
 *
 * @author Pierre-Henri WUILLEMIN, Ni NI, Lionel TORTI & Vincent RENAUDINEAU
 */


#include "O3prmrContext.h"

namespace gum {
  namespace prm {
    namespace o3prmr {

      /* ******************************************************************* */

      O3prmrContext::O3prmrContext( const std::string& filename ) {
        m_filename = filename;
        m_mainImport = 0;
      }

      O3prmrContext::O3prmrContext( const O3prmrContext& s ) {
        m_filename = s.m_filename;
        m_mainImport = s.m_mainImport;
        *this += s;
      }

      O3prmrContext::~O3prmrContext() {
        for ( int i = m_imports.size() - 1 ; i >= 0 ; i-- )
          delete m_imports[i];

        for ( int i = m_sessions.size() - 1 ; i >= 0 ; i-- )
          delete m_sessions[i];
      }

      std::string O3prmrContext::filename() const {
        return m_filename;
      }

      std::string O3prmrContext::package() const {
        return m_package;
      }

      void O3prmrContext::setPackage( const std::string& package ) {
        m_package = package;
      }

      std::string O3prmrContext::aliasToImport( const std::string& alias ) {
        for ( int i = m_imports.size() - 1 ; i >= 0 ; i-- )
          if ( m_imports[i]->alias == alias )
            return m_imports[i]->value;

        return std::string();
      }

      std::vector<ImportCommand*> O3prmrContext::imports() const {
        return m_imports;
      }

      void O3prmrContext::addImport( int line, const std::string& import, const std::string& alias ) {
        m_imports.push_back( new ImportCommand( line, import, alias ) );

        if ( alias == "default" )
          m_mainImport = m_imports.back();
      }

      void O3prmrContext::addImport( int line, const std::string& import, bool ismain ) {
        m_imports.push_back( new ImportCommand( line, import, import ) );

        if ( ismain )
          m_mainImport = m_imports.back();
      }

      std::vector<O3prmrSession*> O3prmrContext::sessions() const {
        return m_sessions;
      }

      void O3prmrContext::addSession( O3prmrSession* session ) {
        m_sessions.push_back( session );
      }

      void O3prmrContext::addSession( const O3prmrSession& session ) {
        addSession( new O3prmrSession( session ) );
      }

      std::string O3prmrContext::toString() const {
        std::string output;

        if ( ! m_package.empty() ) {
          output += "package " + m_package + ";\n";
          output += "\n";
        }

        for ( std::vector<ImportCommand*>::const_iterator i = m_imports.begin() ; i < m_imports.end() ; i++ )
          output += ( *i )->toString() + "\n";

        output += "\n";

        for ( std::vector<O3prmrSession*>::const_iterator i = m_sessions.begin() ; i < m_sessions.end() ; i++ )
          output += ( *i )->toString() + "\n";

        return output;
      }

      O3prmrContext& O3prmrContext::operator+= ( const O3prmrContext& c ) {
        const std::vector<ImportCommand*>& imports = c.imports();

        for ( std::vector<ImportCommand*>::const_iterator i = imports.begin() ; i != imports.end() ; i++ )
          addImport( **i ) ;

        const std::vector<O3prmrSession*>& sessions = c.sessions();

        if ( sessions.size() == 1 && sessions.back()->name() == "default" ) {
          * ( this->m_sessions.back() ) += * ( sessions.back() );
        } else
          for ( std::vector<O3prmrSession*>::const_iterator i = sessions.begin() ; i != sessions.end() ; i++ )
            addSession( **i ) ;

        return *this;
      }

      /* ******************************************************************* */

      O3prmrSession::O3prmrSession( const std::string& name ) {
        m_name = name;
      }

      O3prmrSession::O3prmrSession( const O3prmrSession& s ) {
        m_name = s.m_name;
        *this += s;
      }

      O3prmrSession::~O3prmrSession() {
        for ( int i = m_commands.size() - 1 ; i >= 0 ; i-- )
          delete m_commands[i];

        m_commands.clear();
      }

      std::string O3prmrSession::name() const {
        return m_name;
      }

      std::vector<O3prmrCommand*> O3prmrSession::commands() const {
        return m_commands;
      }

      void O3prmrSession::addCommand( O3prmrCommand* command ) {
        m_commands.push_back( command );
      }

      void O3prmrSession::addObserve( int line, const std::string& leftValue, const std::string& rightValue ) {
        addCommand( new ObserveCommand( line,leftValue, rightValue ) );
      }

      void O3prmrSession::addUnobserve( int line, const std::string& value ) {
        addCommand( new UnobserveCommand( line,value ) );
      }

      void O3prmrSession::addQuery( int line, const std::string& value ) {
        addCommand( new QueryCommand( line,value ) );
      }

      void O3prmrSession::addSetEngine( int line, const std::string& value ) {
        addCommand( new SetEngineCommand( line,value ) );
      }

      void O3prmrSession::addSetGndEngine( int line, const std::string& value ) {
        addCommand( new SetGndEngineCommand( line,value ) );
      }

      void O3prmrSession::addCommand( const O3prmrCommand* command ) {
        switch ( command->type() ) {
          case O3prmrCommand::RequestType::SetEngine :
            m_commands.push_back( new SetEngineCommand( * ( SetEngineCommand* ) command ) );
            break;

          case O3prmrCommand::RequestType::SetGndEngine :
            m_commands.push_back( new SetGndEngineCommand( * ( SetGndEngineCommand* ) command ) );
            break;

          case O3prmrCommand::RequestType::Observe :
            m_commands.push_back( new ObserveCommand( * ( ObserveCommand* ) command ) );
            break;

          case O3prmrCommand::RequestType::Unobserve :
            m_commands.push_back( new UnobserveCommand( * ( UnobserveCommand* ) command ) );
            break;

          case O3prmrCommand::RequestType::Query :
            m_commands.push_back( new QueryCommand( * ( QueryCommand* ) command ) );
            break;
        }
      }

      std::string O3prmrSession::toString() const {
        std::string output;

        output += "request " + m_name + " {\n";

        for ( std::vector<O3prmrCommand*>::const_iterator i = m_commands.begin() ; i < m_commands.end() ; i++ )
          output += "\t" + ( *i )->toString() + "\n";

        output += "}\n";

        return output;
      }

      O3prmrSession& O3prmrSession::operator+= ( const O3prmrSession& c ) {
        for ( std::vector<O3prmrCommand*>::const_iterator i = c.m_commands.begin() ; i < c.m_commands.end() ; i++ )
          addCommand( *i );

        return *this;
      }

      /* ******************************************************************* */

    } // namespace o3prmr
  } // namespace prm
} // namespace gum
