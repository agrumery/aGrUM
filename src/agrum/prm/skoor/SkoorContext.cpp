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

      /* ******************************************************************* */

      SkoorContext::SkoorContext ( const std::string& filename ) {
        m_filename = filename;
        m_mainImport = 0;
      }

      SkoorContext::SkoorContext ( const SkoorContext& s ) {
        m_filename = s.m_filename;
        m_mainImport = s.m_mainImport;
        *this += s;
      }

      SkoorContext::~SkoorContext() {
        for ( int i = m_imports.size() - 1 ; i >= 0 ; i-- )
          delete m_imports[i];

        for ( int i = m_sessions.size() - 1 ; i >= 0 ; i-- )
          delete m_sessions[i];
      }

      std::string SkoorContext::filename() const {
        return m_filename;
      }

      std::string SkoorContext::package() const {
        return m_package;
      }

      void SkoorContext::setPackage ( const std::string& package ) {
        m_package = package;
      }

      std::string SkoorContext::aliasToImport ( const std::string& alias ) {
        for ( int i = m_imports.size() - 1 ; i >= 0 ; i-- )
          if ( m_imports[i]->alias == alias )
            return m_imports[i]->value;

        return std::string();
      }

      std::vector<ImportCommand*> SkoorContext::imports() const {
        return m_imports;
      }

      void SkoorContext::addImport ( int line, const std::string& import, const std::string& alias ) {
        m_imports.push_back ( new ImportCommand ( line, import, alias ) );

        if ( alias == "default" )
          m_mainImport = m_imports.back();
      }

      void SkoorContext::addImport ( int line, const std::string& import, bool ismain ) {
        m_imports.push_back ( new ImportCommand ( line, import, import ) );

        if ( ismain )
          m_mainImport = m_imports.back();
      }

      std::vector<SkoorSession*> SkoorContext::sessions() const {
        return m_sessions;
      }

      void SkoorContext::addSession ( SkoorSession* session ) {
        m_sessions.push_back ( session );
      }

      void SkoorContext::addSession ( const SkoorSession& session ) {
        addSession ( new SkoorSession ( session ) );
      }

      std::string SkoorContext::toString() const {
        std::string output;

        if ( ! m_package.empty() ) {
          output += "package " + m_package + ";\n";
          output += "\n";
        }

        for ( std::vector<ImportCommand*>::const_iterator i = m_imports.begin() ; i < m_imports.end() ; i++ )
          output += ( *i )->toString() + "\n";

        output += "\n";

        for ( std::vector<SkoorSession*>::const_iterator i = m_sessions.begin() ; i < m_sessions.end() ; i++ )
          output += ( *i )->toString() + "\n";

        return output;
      }

      SkoorContext& SkoorContext::operator+= ( const SkoorContext& c ) {
        const std::vector<ImportCommand*> & imports = c.imports();

        for ( std::vector<ImportCommand*>::const_iterator i = imports.begin() ; i != imports.end() ; i++ )
          addImport ( **i ) ;

        const std::vector<SkoorSession*> & sessions = c.sessions();

        if ( sessions.size() == 1 && sessions.back()->name() == "default" ) {
          * ( this->m_sessions.back() ) += * ( sessions.back() );
        } else
          for ( std::vector<SkoorSession*>::const_iterator i = sessions.begin() ; i != sessions.end() ; i++ )
            addSession ( **i ) ;

        return *this;
      }

      /* ******************************************************************* */

      SkoorSession::SkoorSession ( const std::string& name ) {
        m_name = name;
      }

      SkoorSession::SkoorSession ( const SkoorSession& s ) {
        m_name = s.m_name;
        *this += s;
      }

      SkoorSession::~SkoorSession() {
        for ( int i = m_commands.size() - 1 ; i >= 0 ; i-- )
          delete m_commands[i];

        m_commands.clear();
      }

      std::string SkoorSession::name() const {
        return m_name;
      }

      std::vector<SkoorCommand*> SkoorSession::commands() const {
        return m_commands;
      }

      void SkoorSession::addCommand ( SkoorCommand* command ) {
        m_commands.push_back ( command );
      }

      void SkoorSession::addObserve ( int line, const std::string& leftValue, const std::string& rightValue ) {
        addCommand ( new ObserveCommand ( line,leftValue, rightValue ) );
      }

      void SkoorSession::addUnobserve ( int line, const std::string& value ) {
        addCommand ( new UnobserveCommand ( line,value ) );
      }

      void SkoorSession::addQuery ( int line, const std::string& value ) {
        addCommand ( new QueryCommand ( line,value ) );
      }

      void SkoorSession::addSetEngine ( int line, const std::string& value ) {
        addCommand ( new SetEngineCommand ( line,value ) );
      }

      void SkoorSession::addSetGndEngine ( int line, const std::string& value ) {
        addCommand ( new SetGndEngineCommand ( line,value ) );
      }

      void SkoorSession::addCommand ( const SkoorCommand* command ) {
        switch ( command->type() ) {
          case SkoorCommand::SetEngine :
            m_commands.push_back ( new SetEngineCommand ( * ( SetEngineCommand* ) command ) );
            break;
          case SkoorCommand::SetGndEngine :
            m_commands.push_back ( new SetGndEngineCommand ( * ( SetGndEngineCommand* ) command ) );
            break;
          case SkoorCommand::Observe :
            m_commands.push_back ( new ObserveCommand ( * ( ObserveCommand* ) command ) );
            break;
          case SkoorCommand::Unobserve :
            m_commands.push_back ( new UnobserveCommand ( * ( UnobserveCommand* ) command ) );
            break;
          case SkoorCommand::Query :
            m_commands.push_back ( new QueryCommand ( * ( QueryCommand* ) command ) );
            break;
        }
      }

      std::string SkoorSession::toString() const {
        std::string output;

        output += "request " + m_name + " {\n";

        for ( std::vector<SkoorCommand*>::const_iterator i = m_commands.begin() ; i < m_commands.end() ; i++ )
          output += "\t" + ( *i )->toString() + "\n";

        output += "}\n";

        return output;
      }

      SkoorSession& SkoorSession::operator+= ( const SkoorSession& c ) {
        for ( std::vector<SkoorCommand*>::const_iterator i = c.m_commands.begin() ; i < c.m_commands.end() ; i++ )
          addCommand ( *i );

        return *this;
      }

      /* ******************************************************************* */

    } // namespace skoor
  } // namespace prm
} // namespace gum
