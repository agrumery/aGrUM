/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Headers of O3prmInterpreter
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6), Ni NI, Lionel TORTI & Vincent RENAUDINEAU
 */

#ifndef GUM_O3PRMR_CONTEXT_H
#define GUM_O3PRMR_CONTEXT_H

#include <map>
#include <string>
#include <vector>

#include <agrum/PRM/inference/PRMInference.h>

#include <string_view>

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
        enum class RequestType : char { Observe, Unobserve, Query, SetEngine, SetGndEngine };

        explicit O3prmrCommand(int line);

        O3prmrCommand(const O3prmrCommand& c);

        virtual ~O3prmrCommand() = default;

        virtual RequestType type() const     = 0;
        virtual std::string toString() const = 0;
      };

      ///
      class ImportCommand {
        public:
        ImportCommand(int line, std::string_view value, std::string_view alias);

        ImportCommand(const ImportCommand& c);

        int         line;
        std::string value;
        std::string alias;

        std::string toString() const;
      };

      ///
      class SetEngineCommand: public O3prmrCommand {
        public:
        SetEngineCommand(int line, std::string_view value);

        SetEngineCommand(const SetEngineCommand& c);

        std::string value;

        RequestType type() const;

        std::string toString() const;
      };

      ///
      class SetGndEngineCommand: public O3prmrCommand {
        public:
        SetGndEngineCommand(int line, std::string_view value);

        SetGndEngineCommand(const SetGndEngineCommand& c);

        std::string value;

        RequestType type() const;

        std::string toString() const;
      };

      ///
      template < GUM_Numeric GUM_SCALAR >
      class ObserveCommand: public O3prmrCommand {
        public:
        ObserveCommand(int line, std::string_view leftValue, std::string_view rightValue);

        ObserveCommand(const ObserveCommand& c);

        std::string                                leftValue;
        std::string                                rightValue;
        const PRMSystem< GUM_SCALAR >*             system;
        typename PRMInference< GUM_SCALAR >::Chain chain;
        Tensor< GUM_SCALAR >                       potentiel;

        RequestType type() const;

        std::string toString() const;
      };

      ///
      template < GUM_Numeric GUM_SCALAR >
      class UnobserveCommand: public O3prmrCommand {
        public:
        std::string                                value;
        const PRMSystem< GUM_SCALAR >*             system;
        typename PRMInference< GUM_SCALAR >::Chain chain;

        UnobserveCommand(int line, std::string_view value);

        UnobserveCommand(const UnobserveCommand& c);

        RequestType type() const;

        std::string toString() const;
      };

      ///
      template < GUM_Numeric GUM_SCALAR >
      class QueryCommand: public O3prmrCommand {
        public:
        QueryCommand(int line, std::string_view val);

        std::string                                value;
        const PRMSystem< GUM_SCALAR >*             system;
        typename PRMInference< GUM_SCALAR >::Chain chain;

        RequestType type() const;

        std::string toString() const;
      };

      /**
       * This class contains a o3prmr session.
       * It have a name and a sequence of commands.
       * */
      template < GUM_Numeric GUM_SCALAR >
      class O3prmrSession {
        /// The session name;
        std::string m_name;
        /// A sequence of commands.
        std::vector< O3prmrCommand* >                                           m_commands;
        std::map< const PRMSystem< GUM_SCALAR >*, PRMInference< GUM_SCALAR >* > m_infEngineMap;

        public:
        explicit O3prmrSession(std::string_view name = "");
        O3prmrSession(const O3prmrSession& s);
        virtual ~O3prmrSession();

        std::string name() const;

        std::vector< O3prmrCommand* > commands() const;
        void addObserve(int line, std::string_view leftValue, std::string_view rightValue);
        void addUnobserve(int line, std::string_view value);
        void addQuery(int line, std::string_view value);
        void addSetEngine(int line, std::string_view value);
        void addSetGndEngine(int line, std::string_view value);
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
      template < GUM_Numeric GUM_SCALAR >
      class O3prmrContext {
        std::string                                 m_filename;
        std::string                                 m_package;
        std::vector< O3prmrSession< GUM_SCALAR >* > m_sessions;
        std::vector< ImportCommand* >               m_imports;
        ImportCommand*                              m_mainImport;

        public:
        explicit O3prmrContext(std::string_view filename = "");
        O3prmrContext(const O3prmrContext& s);
        virtual ~O3prmrContext();

        const ImportCommand* mainImport() const;

        std::string filename() const;

        std::string package() const;
        void        setPackage(std::string_view package);

        std::string                   aliasToImport(std::string_view alias);
        std::vector< ImportCommand* > imports() const;
        void addImport(int line, std::string_view import, std::string_view alias);
        void addImport(int line, std::string_view import, bool ismain);

        void addImport(const ImportCommand& i);

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
  }   // namespace prm
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/PRM/o3prmr/O3prmrContext_inl.h>
#endif   // GUM_NO_INLINE

#include <agrum/PRM/o3prmr/O3prmrContext_tpl.h>

#endif   // GUM_O3PRMR_CONTEXT_H
