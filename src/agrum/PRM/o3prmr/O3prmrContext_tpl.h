/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Implementation of O3prmReader.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6), Ni NI, Lionel TORTI & Vincent RENAUDINEAU
 */

#include "O3prmrContext.h"

namespace gum {
  namespace prm {
    namespace o3prmr {

      /* ******************************************************************* */

      template < typename GUM_SCALAR >
      O3prmrContext< GUM_SCALAR >::O3prmrContext(const std::string& filename) {
        m_filename   = filename;
        m_mainImport = 0;
      }

      template < typename GUM_SCALAR >
      O3prmrContext< GUM_SCALAR >::O3prmrContext(const O3prmrContext& s) {
        m_filename   = s.m_filename;
        m_mainImport = s.m_mainImport;
        *this += s;
      }

      template < typename GUM_SCALAR >
      O3prmrContext< GUM_SCALAR >::~O3prmrContext() {
        for (Idx i = Size(m_imports.size()); i > 0; i--)
          delete m_imports[i - 1];

        for (Size i = Size(m_sessions.size()); i > 0; i--)
          delete m_sessions[i - 1];
      }

      template < typename GUM_SCALAR >
      std::string O3prmrContext< GUM_SCALAR >::filename() const {
        return m_filename;
      }

      template < typename GUM_SCALAR >
      std::string O3prmrContext< GUM_SCALAR >::package() const {
        return m_package;
      }

      template < typename GUM_SCALAR >
      void O3prmrContext< GUM_SCALAR >::setPackage(const std::string& package) {
        m_package = package;
      }

      template < typename GUM_SCALAR >
      std::string O3prmrContext< GUM_SCALAR >::aliasToImport(const std::string& alias) {
        for (Idx i = Size(m_imports.size()); i > 0; i--)
          if (m_imports[i - 1]->alias == alias) return m_imports[i - 1]->value;

        return std::string();
      }

      template < typename GUM_SCALAR >
      std::vector< ImportCommand* > O3prmrContext< GUM_SCALAR >::imports() const {
        return m_imports;
      }

      template < typename GUM_SCALAR >
      void O3prmrContext< GUM_SCALAR >::addImport(int                line,
                                                  const std::string& import,
                                                  const std::string& alias) {
        m_imports.push_back(new ImportCommand(line, import, alias));

        if (alias == "default") m_mainImport = m_imports.back();
      }

      template < typename GUM_SCALAR >
      void
         O3prmrContext< GUM_SCALAR >::addImport(int line, const std::string& import, bool ismain) {
        m_imports.push_back(new ImportCommand(line, import, import));

        if (ismain) m_mainImport = m_imports.back();
      }

      template < typename GUM_SCALAR >
      std::vector< O3prmrSession< GUM_SCALAR >* > O3prmrContext< GUM_SCALAR >::sessions() const {
        return m_sessions;
      }

      template < typename GUM_SCALAR >
      void O3prmrContext< GUM_SCALAR >::addSession(const O3prmrSession< GUM_SCALAR >& session) {
        m_sessions.push_back(new O3prmrSession< GUM_SCALAR >(session));
      }

      template < typename GUM_SCALAR >
      std::string O3prmrContext< GUM_SCALAR >::toString() const {
        std::string output;

        if (!m_package.empty()) {
          output += "package " + m_package + ";\n";
          output += "\n";
        }

        for (auto i = m_imports.begin(); i < m_imports.end(); i++)
          output += (*i)->toString() + "\n";

        output += "\n";

        for (auto i = m_sessions.begin(); i < m_sessions.end(); i++)
          output += (*i)->toString() + "\n";

        return output;
      }

      template < typename GUM_SCALAR >
      O3prmrContext< GUM_SCALAR >& O3prmrContext< GUM_SCALAR >::operator+=(const O3prmrContext& c) {
        const std::vector< ImportCommand* >& imports = c.imports();

        for (std::vector< ImportCommand* >::const_iterator i = imports.begin(); i != imports.end();
             i++)
          addImport(**i);

        const std::vector< O3prmrSession< GUM_SCALAR >* >& sessions = c.sessions();

        if (sessions.size() == 1 && sessions.back()->name() == "default") {
          *(this->m_sessions.back()) += *(sessions.back());
        } else
          for (auto i = sessions.begin(); i != sessions.end(); i++)
            addSession(**i);

        return *this;
      }

      /* ******************************************************************* */

      template < typename GUM_SCALAR >
      O3prmrSession< GUM_SCALAR >::O3prmrSession(const std::string& name) {
        m_name = name;
      }

      template < typename GUM_SCALAR >
      O3prmrSession< GUM_SCALAR >::O3prmrSession(const O3prmrSession< GUM_SCALAR >& s) {
        m_name = s.m_name;
        *this += s;
      }

      template < typename GUM_SCALAR >
      O3prmrSession< GUM_SCALAR >::~O3prmrSession() {
        for (Idx i = Size(m_commands.size()); i >= 1; i--)
          delete m_commands[i - 1];

        m_commands.clear();
      }

      template < typename GUM_SCALAR >
      std::string O3prmrSession< GUM_SCALAR >::name() const {
        return m_name;
      }

      template < typename GUM_SCALAR >
      std::vector< O3prmrCommand* > O3prmrSession< GUM_SCALAR >::commands() const {
        return m_commands;
      }

      template < typename GUM_SCALAR >
      void O3prmrSession< GUM_SCALAR >::addCommand(O3prmrCommand* command) {
        m_commands.push_back(command);
      }

      template < typename GUM_SCALAR >
      void O3prmrSession< GUM_SCALAR >::addObserve(int                line,
                                                   const std::string& leftValue,
                                                   const std::string& rightValue) {
        addCommand(new ObserveCommand< GUM_SCALAR >(line, leftValue, rightValue));
      }

      template < typename GUM_SCALAR >
      void O3prmrSession< GUM_SCALAR >::addUnobserve(int line, const std::string& value) {
        addCommand(new UnobserveCommand< GUM_SCALAR >(line, value));
      }

      template < typename GUM_SCALAR >
      void O3prmrSession< GUM_SCALAR >::addQuery(int line, const std::string& value) {
        addCommand(new QueryCommand< GUM_SCALAR >(line, value));
      }

      template < typename GUM_SCALAR >
      void O3prmrSession< GUM_SCALAR >::addSetEngine(int line, const std::string& value) {
        addCommand(new SetEngineCommand(line, value));
      }

      template < typename GUM_SCALAR >
      void O3prmrSession< GUM_SCALAR >::addSetGndEngine(int line, const std::string& value) {
        addCommand(new SetGndEngineCommand(line, value));
      }

      template < typename GUM_SCALAR >
      void O3prmrSession< GUM_SCALAR >::addCommand(const O3prmrCommand* command) {
        switch (command->type()) {
          case O3prmrCommand::RequestType::SetEngine:
            m_commands.push_back(new SetEngineCommand(*(SetEngineCommand*)command));
            break;

          case O3prmrCommand::RequestType::SetGndEngine:
            m_commands.push_back(new SetGndEngineCommand(*(SetGndEngineCommand*)command));
            break;

          case O3prmrCommand::RequestType::Observe:
            m_commands.push_back(
               new ObserveCommand< GUM_SCALAR >(*(ObserveCommand< GUM_SCALAR >*)command));
            break;

          case O3prmrCommand::RequestType::Unobserve:
            m_commands.push_back(
               new UnobserveCommand< GUM_SCALAR >(*(UnobserveCommand< GUM_SCALAR >*)command));
            break;

          case O3prmrCommand::RequestType::Query:
            m_commands.push_back(
               new QueryCommand< GUM_SCALAR >(*(QueryCommand< GUM_SCALAR >*)command));
            break;
        }
      }

      template < typename GUM_SCALAR >
      std::string O3prmrSession< GUM_SCALAR >::toString() const {
        std::string output;

        output += "request " + m_name + " {\n";

        for (std::vector< O3prmrCommand* >::const_iterator i = m_commands.begin();
             i < m_commands.end();
             i++) {
          output += "\t";
          output += (*i)->toString() + "\n";
        }

        output += "}\n";

        return output;
      }

      template < typename GUM_SCALAR >
      O3prmrSession< GUM_SCALAR >&
         O3prmrSession< GUM_SCALAR >::operator+=(const O3prmrSession< GUM_SCALAR >& c) {
        for (std::vector< O3prmrCommand* >::const_iterator i = c.m_commands.begin();
             i < c.m_commands.end();
             i++)
          addCommand(*i);

        return *this;
      }

      /* ******************************************************************* */

    }   // namespace o3prmr
  }     // namespace prm
}   // namespace gum
