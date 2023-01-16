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
 * @brief Headers of O3prmInterpreter
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6), Ni NI, Lionel TORTI & Vincent RENAUDINEAU
 */

#ifndef SKOORSYNTAXTREE_H
#define SKOORSYNTAXTREE_H

#include <map>
#include <string>
#include <vector>


#include <agrum/PRM/inference/PRMInference.h>

namespace gum {
  namespace prm {
    namespace o3prmr {

      /**
       * This is an abstract class.
       * It represent a o3prmr command.
       * There five types of command, such sub-classes.
       * */
      class O3prmrCommand {
        public:
        int line;
        enum class RequestType : char {
          Observe,
          Unobserve,
          Query,
          SetEngine,
          SetGndEngine
        };

        explicit O3prmrCommand(int line) : line(line) {}
        O3prmrCommand(const O3prmrCommand& c) : line(c.line) {}
        virtual ~O3prmrCommand() {}

        virtual RequestType type() const     = 0;
        virtual std::string toString() const = 0;
      };

      ///
      class ImportCommand {
        public:
        ImportCommand(int line, const std::string& value, const std::string& alias) :
            line(line), value(value), alias(alias) {}
        ImportCommand(const ImportCommand& c) : line(c.line), value(c.value), alias(c.alias) {}

        int         line;
        std::string value;
        std::string alias;

        std::string toString() const {
          return "import " + value + (alias.empty() ? "" : "as " + alias) + ";";
        }
      };

      ///
      class SetEngineCommand: public O3prmrCommand {
        public:
        SetEngineCommand(int line, const std::string& value) : O3prmrCommand(line), value(value) {}
        SetEngineCommand(const SetEngineCommand& c) : O3prmrCommand(c), value(c.value) {}

        std::string value;

        RequestType type() const { return RequestType::SetEngine; }
        std::string toString() const { return "engine " + value + ";"; }
      };

      ///
      class SetGndEngineCommand: public O3prmrCommand {
        public:
        SetGndEngineCommand(int line, const std::string& value) :
            O3prmrCommand(line), value(value) {}
        SetGndEngineCommand(const SetGndEngineCommand& c) : O3prmrCommand(c), value(c.value) {}

        std::string value;

        RequestType type() const { return RequestType::SetGndEngine; }
        std::string toString() const { return "grd_engine " + value + ";"; }
      };

      ///
      template < typename GUM_SCALAR >
      class ObserveCommand: public O3prmrCommand {
        public:
        ObserveCommand(int line, const std::string& leftValue, const std::string& rightValue) :
            O3prmrCommand(line), leftValue(leftValue), rightValue(rightValue), system(0) {}
        ObserveCommand(const ObserveCommand& c) :
            O3prmrCommand(c), leftValue(c.leftValue), rightValue(c.rightValue), system(c.system),
            chain(c.chain) {}

        std::string                                leftValue;
        std::string                                rightValue;
        const PRMSystem< GUM_SCALAR >*             system;
        typename PRMInference< GUM_SCALAR >::Chain chain;
        Potential< GUM_SCALAR >                    potentiel;

        RequestType type() const { return RequestType::Observe; }
        std::string toString() const { return leftValue + " = " + rightValue + ";"; }
      };

      ///
      template < typename GUM_SCALAR >
      class UnobserveCommand: public O3prmrCommand {
        public:
        std::string                                value;
        const PRMSystem< GUM_SCALAR >*             system;
        typename PRMInference< GUM_SCALAR >::Chain chain;

        UnobserveCommand(int line, const std::string& value) :
            O3prmrCommand(line), value(value), system(0) {}
        UnobserveCommand(const UnobserveCommand& c) :
            O3prmrCommand(c), value(c.value), system(c.system), chain(c.chain) {}

        RequestType type() const { return RequestType::Unobserve; }
        std::string toString() const { return "unobserve " + value + ";"; }
      };

      ///
      template < typename GUM_SCALAR >
      class QueryCommand: public O3prmrCommand {
        public:
        QueryCommand(int line, const std::string& val) :
            O3prmrCommand(line), value(val), system(nullptr) {}

        std::string                                value;
        const PRMSystem< GUM_SCALAR >*             system;
        typename PRMInference< GUM_SCALAR >::Chain chain;

        RequestType type() const { return RequestType::Query; }
        std::string toString() const { return "? " + value + ";"; }
      };

      /**
       * This class contains a o3prmr session.
       * It have a name and a sequence of commands.
       * */
      template < typename GUM_SCALAR >
      class O3prmrSession {
        /// The session name;
        std::string m_name;
        /// A sequence of commands.
        std::vector< O3prmrCommand* >                                           m_commands;
        std::map< const PRMSystem< GUM_SCALAR >*, PRMInference< GUM_SCALAR >* > m_infEngineMap;

        public:
        explicit O3prmrSession(const std::string& name = std::string());
        O3prmrSession(const O3prmrSession& s);
        virtual ~O3prmrSession();

        std::string name() const;

        std::vector< O3prmrCommand* > commands() const;
        void addObserve(int line, const std::string& leftValue, const std::string& rightValue);
        void addUnobserve(int line, const std::string& value);
        void addQuery(int line, const std::string& value);
        void addSetEngine(int line, const std::string& value);
        void addSetGndEngine(int line, const std::string& value);
        void addCommand(const O3prmrCommand* command);

        virtual std::string toString() const;
        O3prmrSession&      operator+=(const O3prmrSession& c);

        private:
        void addCommand(O3prmrCommand* command);
      };

      /**
       * Represent a o3prmr context, with an import, and some sequencials
       * commands.
       */
      template < typename GUM_SCALAR >
      class O3prmrContext {
        std::string                                 m_filename;
        std::string                                 m_package;
        std::vector< O3prmrSession< GUM_SCALAR >* > m_sessions;
        std::vector< ImportCommand* >               m_imports;
        ImportCommand*                              m_mainImport;

        public:
        explicit O3prmrContext(const std::string& filename = std::string());
        O3prmrContext(const O3prmrContext& s);
        virtual ~O3prmrContext();

        const ImportCommand* mainImport() const { return m_mainImport; }

        std::string filename() const;

        std::string package() const;
        void        setPackage(const std::string& package);

        std::string                   aliasToImport(const std::string& alias);
        std::vector< ImportCommand* > imports() const;
        void addImport(int line, const std::string& import, const std::string& alias);
        void addImport(int line, const std::string& import, bool ismain);
        void addImport(const ImportCommand& i) {
          m_imports.push_back(new ImportCommand(i.line, i.value, i.alias));

          if (i.alias == "default") m_mainImport = m_imports.back();
        }

        std::vector< O3prmrSession< GUM_SCALAR >* > sessions() const;
        void addSession(const O3prmrSession< GUM_SCALAR >& session);

        virtual std::string toString() const;
        O3prmrContext&      operator+=(const O3prmrContext& c);
      };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class ObserveCommand< double >;
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class UnobserveCommand< double >;
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class QueryCommand< double >;
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class O3prmrSession< double >;
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class O3prmrContext< double >;
#        endif
#      endif
#    endif
#  endif
#endif


    }   // namespace o3prmr
  }     // namespace prm
}   // namespace gum

#include <agrum/PRM/o3prmr/O3prmrContext_tpl.h>

#endif   // SKOORSYNTAXTREE_H
