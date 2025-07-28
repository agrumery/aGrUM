/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief Implementation of O3prmReader<double>.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6), Ni NI, Lionel TORTI & Vincent RENAUDINEAU
 */
#include <filesystem>

#include <agrum/agrum.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/tools/BayesNetInference.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/PRM/inference/groundedInference.h>
#include <agrum/PRM/inference/SVE.h>
#include <agrum/PRM/inference/SVED.h>
#include <agrum/PRM/o3prmr/cocoR/Parser.h>
#include <agrum/PRM/o3prmr/O3prmrInterpreter.h>

namespace gum {

  namespace prm {

    namespace o3prmr {

      /* **************************************************************************
       */

      /// This constructor create an empty context.
      O3prmrInterpreter::O3prmrInterpreter() :
          m_context(new O3prmrContext< double >()), m_reader(new o3prm::O3prmReader< double >()),
          m_bn(0), m_inf(0), m_syntax_flag(false), m_verbose(false), m_log(std::cout),
          m_current_line(-1) {}

      /// Destructor. Delete current context.
      O3prmrInterpreter::~O3prmrInterpreter() {
        delete m_context;
        if (m_bn) { delete m_bn; }
        for (auto p: m_inf_map) {
          delete p.second;
        }
        delete m_reader->prm();
        delete m_reader;
      }

      /* **************************************************************************
       */

      /// Getter for the context.
      O3prmrContext< double >* O3prmrInterpreter::getContext() const { return m_context; }

      /// Setter for the context.
      void O3prmrInterpreter::setContext(O3prmrContext< double >* context) {
        delete m_context;

        if (context == 0) m_context = new O3prmrContext< double >();
        else m_context = context;
      }

      /// Root paths to search from there packages.
      /// Default are './' and one is calculate from request package if any.
      std::vector< std::string > O3prmrInterpreter::getPaths() const { return m_paths; }

      /// Root paths to search from there packages.
      /// Default are './' and one is calculate from request package if any.
      void O3prmrInterpreter::addPath(std::string path) {
        if (path.length() && path.back() != '/') { path = path + '/'; }

        std::filesystem::directory_entry dir(path);
        if (dir.exists()) {
          m_paths.push_back(path);
        } else {
          GUM_ERROR(NotFound, "not a directory")
        }
      }

      /// Root paths to search from there packages.
      /// Default are './' and one is calculate from request package if any.
      void O3prmrInterpreter::clearPaths() { m_paths.clear(); }

      /// syntax mode don't process anything, just check syntax.
      bool O3prmrInterpreter::isInSyntaxMode() const { return m_syntax_flag; }

      /// syntax mode don't process anything, just check syntax.
      void O3prmrInterpreter::setSyntaxMode(bool f) { m_syntax_flag = f; }

      /// verbose mode show more details on the program execution.
      bool O3prmrInterpreter::isVerboseMode() const { return m_verbose; }

      /// verbose mode show more details on the program execution.
      void O3prmrInterpreter::setVerboseMode(bool f) { m_verbose = f; }

      /// Retrieve prm object.
      const PRM< double >* O3prmrInterpreter::prm() const { return m_reader->prm(); }

      /// Retrieve inference motor object.
      const PRMInference< double >* O3prmrInterpreter::inference() const { return m_inf; }

      /// Return a std::vector of QueryResults.
      /// Each QueryResults is a struct with query command, time and values,
      /// a std::vector of struct SingleResult, with pair label/value.
      const std::vector< QueryResult >& O3prmrInterpreter::results() const { return m_results; }

      /**
       * Parse the file or the command line.
       * If errors occured, return false. Errors messages can be retrieve be
       * getErrorsContainer() methods.
       * If any errors occured, return true.
       * Requests results can be retrieve be results() methods.
       * */
      bool O3prmrInterpreter::interpretFile(const std::string& filename) {
        m_results.clear();

        try {
          std::string file_content = _readFile_(filename);

          delete m_context;
          m_context = new O3prmrContext< double >(filename);
          O3prmrContext< double > c(filename);

          // On vérifie la syntaxe
          unsigned char* buffer = new unsigned char[file_content.length() + 1];
          strcpy((char*)buffer, file_content.c_str());
          Scanner s(buffer, int(file_content.length() + 1));
          Parser  p(&s);
          p.setO3prmrContext(&c);
          p.Parse();

          m_errors = p.errors();

          if (errors() > 0) { return false; }

          // Set paths to search from.
          delete m_reader->prm();
          delete m_reader;
          m_reader = new o3prm::O3prmReader< double >();

          for (size_t i = 0; i < m_paths.size(); i++) {
            m_reader->addClassPath(m_paths[i]);
          }

          // On vérifie la sémantique.
          if (!checkSemantic(&c)) { return false; }

          if (isInSyntaxMode()) {
            return true;
          } else {
            return interpret(&c);
          }
        } catch (gum::Exception&) { return false; }
      }

      std::string O3prmrInterpreter::_readFile_(const std::string& file) {
        // read entire file into string
        std::ifstream istream(file, std::ifstream::binary);
        if (istream) {
          // get length of file:
          istream.seekg(0, istream.end);
          int length = int(istream.tellg());
          istream.seekg(0, istream.beg);

          std::string str;
          str.resize(length, ' ');   // reserve space
          char* begin = &*str.begin();

          istream.read(begin, length);
          istream.close();

          return str;
        }
        GUM_ERROR(OperationNotAllowed, "Could not open file")
      }

      bool O3prmrInterpreter::interpretLine(const std::string& line) {
        m_results.clear();

        // On vérifie la syntaxe
        O3prmrContext< double > c;
        Scanner                 s((unsigned char*)line.c_str(), (int)line.length());
        Parser                  p(&s);
        p.setO3prmrContext(&c);
        p.Parse();
        m_errors = p.errors();

        if (errors() > 0) return false;

        // On vérifie la sémantique.
        if (!checkSemantic(&c)) return false;

        if (isInSyntaxMode()) return true;
        else return interpret(&c);
      }

      /**
       * Crée le prm correspondant au contexte courant.
       * Renvoie true en cas de succès, ou false en cas échéant d'échec
       * de l'interprétation du contexte (import introuvable ou non défini,
       * etc).
       * */
      bool O3prmrInterpreter::interpret(O3prmrContext< double >* c) {
        if (isVerboseMode()) m_log << "## Start interpretation." << std::endl << std::flush;

        // Don't parse if any syntax errors.
        if (errors() > 0) return false;

        // For each session
        std::vector< O3prmrSession< double >* > sessions = c->sessions();

        for (const auto session: sessions)
          for (auto command: session->commands()) {
            // We process it.
            bool result = true;

            try {
              switch (command->type()) {
                case O3prmrCommand::RequestType::Observe :
                  result = observe((ObserveCommand< double >*)command);
                  break;

                case O3prmrCommand::RequestType::Unobserve :
                  result = unobserve((UnobserveCommand< double >*)command);
                  break;

                case O3prmrCommand::RequestType::SetEngine :
                  setEngine((SetEngineCommand*)command);
                  break;

                case O3prmrCommand::RequestType::SetGndEngine :
                  setGndEngine((SetGndEngineCommand*)command);
                  break;

                case O3prmrCommand::RequestType::Query :
                  query((QueryCommand< double >*)command);
                  break;
              }
            } catch (Exception& err) {
              result = false;
              addError(err.errorContent());
            } catch (std::string& err) {
              result = false;
              addError(err);
            }

            // If there was a problem, skip the rest of this session,
            // unless syntax mode is activated.
            if (!result) {
              if (m_verbose) m_log << "Errors : skip the rest of this session." << std::endl;

              break;
            }
          }

        if (isVerboseMode()) m_log << "## End interpretation." << std::endl << std::flush;

        return errors() == 0;
      }

      /* **************************************************************************
       */

      /**
       * Check semantic validity of context.
       * Import first all import, and check that systems, instances, attributes
       *and
       *labels exists.
       * While checking, prepare data structures for interpretation.
       * Return true if all is right, false otherwise.
       *
       * Note : Stop checking at first error unless syntax mode is activated.
       * */
      bool O3prmrInterpreter::checkSemantic(O3prmrContext< double >* context) {
        // Don't parse if any syntax errors.
        if (errors() > 0) return false;

        // On importe tous les systèmes.
        for (const auto command: context->imports()) {
          m_current_line = command->line;
          // if import doen't succed stop here unless syntax mode is activated.
          bool succeed = import(context, command->value);

          if (!succeed && !isInSyntaxMode()) return false;

          // En cas de succès, on met à jour le contexte global
          if (succeed) m_context->addImport(*command);
        }

        if (m_verbose)
          m_log << "## Check semantic for " << context->sessions().size() << " sessions"
                << std::endl;

        // On vérifie chaque session
        for (const auto session: context->sessions()) {
          std::string              sessionName = session->name();
          O3prmrSession< double >* new_session = new O3prmrSession< double >(sessionName);

          if (m_verbose)
            m_log << "## Start session '" << sessionName << "'..." << std::endl << std::endl;

          for (const auto command: session->commands()) {
            if (m_verbose)
              m_log << "# * Going to check command : " << command->toString() << std::endl;

            // Update the current line (for warnings and errors)
            m_current_line = command->line;

            // We check it.
            bool result = true;

            try {
              switch (command->type()) {
                case O3prmrCommand::RequestType::SetEngine :
                  result = checkSetEngine((SetEngineCommand*)command);
                  break;

                case O3prmrCommand::RequestType::SetGndEngine :
                  result = checkSetGndEngine((SetGndEngineCommand*)command);
                  break;

                case O3prmrCommand::RequestType::Observe :
                  result = checkObserve((ObserveCommand< double >*)command);
                  break;

                case O3prmrCommand::RequestType::Unobserve :
                  result = checkUnobserve((UnobserveCommand< double >*)command);
                  break;

                case O3prmrCommand::RequestType::Query :
                  result = checkQuery((QueryCommand< double >*)command);
                  break;

                default :
                  addError("Error : Unknow command : " + command->toString()
                           + "\n -> Command not processed.");
                  result = false;
              }
            } catch (Exception& err) {
              result = false;
              addError(err.errorContent());
            } catch (std::string& err) {
              result = false;
              addError(err);
            }

            // If there was a problem, skip the rest of this session,
            // unless syntax mode is activated.
            if (!result && !isInSyntaxMode()) {
              if (m_verbose) m_log << "Errors : skip the rest of this session." << std::endl;

              break;
            }

            // On l'ajoute au contexte globale
            if (result) new_session->addCommand((const O3prmrCommand*)command);
          }

          // Ajoute la session au contexte global,
          // ou à la dernière session.
          if (sessionName == "default" && m_context->sessions().size() > 0)
            *(m_context->sessions().back()) += *new_session;
          else m_context->addSession(*new_session);

          if (m_verbose)
            m_log << std::endl
                  << "## Session '" << sessionName << "' finished." << std::endl
                  << std::endl
                  << std::endl;

          // todo : check memory leak
          // delete new_session; ??
        }

        if (isVerboseMode() && errors() != 0) m_errors.elegantErrorsAndWarnings(m_log);

        return errors() == 0;
      }

      bool O3prmrInterpreter::checkSetEngine(SetEngineCommand* command) {
        m_engine = command->value;
        return m_engine == "SVED" || m_engine == "GRD" || m_engine == "SVE";
      }

      bool O3prmrInterpreter::checkSetGndEngine(SetGndEngineCommand* command) {
        m_bn_engine = command->value;
        return m_bn_engine == "VE" || m_bn_engine == "VEBB" || m_bn_engine == "lazy";
      }

      bool O3prmrInterpreter::checkObserve(ObserveCommand< double >* command) {
        try {
          std::string       left_val  = command->leftValue;
          const std::string right_val = command->rightValue;

          // Contruct the pair (instance,attribut)
          const PRMSystem< double >&    sys      = system(left_val);
          const PRMInstance< double >&  instance = sys.get(findInstanceName(left_val, sys));
          const PRMAttribute< double >& attr = instance.get(findAttributeName(left_val, instance));
          typename PRMInference< double >::Chain chain = std::make_pair(&instance, &attr);

          command->system = &sys;
          command->chain  = std::make_pair(&instance, &attr);

          // Check label exists for this type.
          // Tensor<double> e;
          command->potentiel.add(chain.second->type().variable());
          Instantiation i(command->potentiel);
          bool          found = false;

          for (i.setFirst(); !i.end(); i.inc()) {
            if (chain.second->type().variable().label(i.val(chain.second->type().variable()))
                == right_val) {
              command->potentiel.set(i, (double)1.0);
              found = true;
            } else {
              command->potentiel.set(i, (double)0.0);
            }
          }

          if (!found) addError(right_val + " is not a label of " + left_val);

          // else command->potentiel = e;

          return found;

        } catch (Exception& err) { addError(err.errorContent()); } catch (std::string& err) {
          addError(err);
        }

        return false;
      }

      bool O3prmrInterpreter::checkUnobserve(UnobserveCommand< double >* command) {
        try {
          std::string name = command->value;

          // Contruct the pair (instance,attribut)
          const PRMSystem< double >&    sys      = system(name);
          const PRMInstance< double >&  instance = sys.get(findInstanceName(name, sys));
          const PRMAttribute< double >& attr     = instance.get(findAttributeName(name, instance));
          // PRMInference<double>::Chain chain = std::make_pair(&instance,
          // &attr);

          command->system = &sys;
          command->chain  = std::make_pair(&instance, &attr);

          return true;

        } catch (Exception& err) { addError(err.errorContent()); } catch (std::string& err) {
          addError(err);
        }

        return false;
      }

      bool O3prmrInterpreter::checkQuery(QueryCommand< double >* command) {
        try {
          std::string name = command->value;

          // Contruct the pair (instance,attribut)
          const PRMSystem< double >&    sys      = system(name);
          const PRMInstance< double >&  instance = sys.get(findInstanceName(name, sys));
          const PRMAttribute< double >& attr     = instance.get(findAttributeName(name, instance));
          // PRMInference<double>::Chain chain = std::make_pair(&instance,
          // &attr);

          command->system = &sys;
          command->chain  = std::make_pair(&instance, &attr);

          return true;

        } catch (Exception& err) { addError(err.errorContent()); } catch (std::string& err) {
          addError(err);
        }

        return false;
      }

      // Import the system o3prm file
      // Return false if any error.

      bool O3prmrInterpreter::import(O3prmrContext< double >* context, std::string import_name) {
        try {
          if (m_verbose) { m_log << "# Loading system '" << import_name << "' => '" << std::flush; }

          std::string import_package = import_name;

          std::replace(import_name.begin(), import_name.end(), '.', '/');
          import_name += ".o3prm";

          if (m_verbose) { m_log << import_name << "' ... " << std::endl << std::flush; }

          std::ifstream file_test;
          bool          found = false;
          std::string   import_abs_filename;

          // Search in o3prmr file dir.
          std::string o3prmrFilename = context->filename();

          if (!o3prmrFilename.empty()) {
            size_t index = o3prmrFilename.find_last_of('/');

            if (index != std::string::npos) {
              std::string dir     = o3prmrFilename.substr(0, index + 1);
              import_abs_filename = dir + import_name;

              if (m_verbose) {
                m_log << "# Search from filedir '" << import_abs_filename << "' ... " << std::flush;
              }

              file_test.open(import_abs_filename.c_str());

              if (file_test.is_open()) {
                if (m_verbose) { m_log << "found !" << std::endl << std::flush; }

                file_test.close();
                found = true;
              } else if (m_verbose) {
                m_log << "not found." << std::endl << std::flush;
              }
            }
          }

          // Deduce root path from package name.
          std::string package = context->package();

          if (!found && !package.empty()) {
            std::string root;

            // if filename is not empty, start from it.
            std::string filename = context->filename();

            if (!filename.empty()) {
              size_t size = filename.find_last_of('/');

              if (size != std::string::npos) {
                root += filename.substr(0, size + 1);   // take with the '/'
              }
            }

            //
            root += "../";
            int count = (int)std::count(package.begin(), package.end(), '.');

            for (int i = 0; i < count; i++)
              root += "../";

            import_abs_filename = std::filesystem::absolute(std::filesystem::path(root)
                                                            / std::filesystem::path(import_name))
                                      .string();

            if (m_verbose) {
              m_log << "# Search from package '" << package << "' => '" << import_abs_filename
                    << "' ... " << std::flush;
            }

            file_test.open(import_abs_filename.c_str());

            if (file_test.is_open()) {
              if (m_verbose) { m_log << "found !" << std::endl << std::flush; }

              file_test.close();
              found = true;
            } else if (m_verbose) {
              m_log << "not found." << std::endl << std::flush;
            }
          }

          // Search import in all paths.
          for (const auto& path: m_paths) {
            import_abs_filename = path + import_name;

            if (m_verbose) {
              m_log << "# Search from classpath '" << import_abs_filename << "' ... " << std::flush;
            }

            file_test.open(import_abs_filename.c_str());

            if (file_test.is_open()) {
              if (m_verbose) { m_log << " found !" << std::endl << std::flush; }

              file_test.close();
              found = true;
              break;
            } else if (m_verbose) {
              m_log << " not found." << std::endl << std::flush;
            }
          }

          if (!found) {
            if (m_verbose) { m_log << "Finished with errors." << std::endl; }

            addError("import not found.");
            return false;
          }

          // May throw std::IOError if file does't exist
          Size previousO3prmError  = m_reader->errors();
          Size previousO3prmrError = errors();

          try {
            m_reader->readFile(import_abs_filename, import_package);

            // Show errors and warning
            if (m_verbose
                && (m_reader->errors() > (unsigned int)previousO3prmError
                    || errors() > previousO3prmrError)) {
              m_log << "Finished with errors." << std::endl;
            } else if (m_verbose) {
              m_log << "Finished." << std::endl;
            }

          } catch (const IOError& err) {
            if (m_verbose) { m_log << "Finished with errors." << std::endl; }

            addError(err.errorContent());
          }

          // Add o3prm errors and warnings to o3prmr errors
          for (; previousO3prmError < m_reader->errorsContainer().count(); previousO3prmError++) {
            m_errors.add(m_reader->errorsContainer().error(previousO3prmError));
          }

          return errors() == previousO3prmrError;

        } catch (const Exception& err) {
          if (m_verbose) { m_log << "Finished with exceptions." << std::endl; }

          addError(err.errorContent());
          return false;
        }
      }

      std::string O3prmrInterpreter::findSystemName(std::string& s) {
        size_t      dot  = s.find_first_of('.');
        std::string name = s.substr(0, dot);

        // We look first for real system, next for alias.
        if (prm()->isSystem(name)) {
          s = s.substr(dot + 1);
          return name;
        }

        if (!m_context->aliasToImport(name).empty()) {
          s = s.substr(dot + 1);
          return m_context->aliasToImport(name);
        }

        while (dot != std::string::npos) {
          if (prm()->isSystem(name)) {
            s = s.substr(dot + 1);
            return name;
          }

          dot  = s.find('.', dot + 1);
          name = s.substr(0, dot);
        }

        throw "could not find any system in '" + s + "'.";
      }

      std::string O3prmrInterpreter::findInstanceName(std::string&               s,
                                                      const PRMSystem< double >& sys) {
        // We have found system before, so 's' has been stripped.
        size_t      dot  = s.find_first_of('.');
        std::string name = s.substr(0, dot);

        if (!sys.exists(name))
          throw "'" + name + "' is not an instance of system '" + sys.name() + "'.";

        s = s.substr(dot + 1);
        return name;
      }

      std::string O3prmrInterpreter::findAttributeName(const std::string&           s,
                                                       const PRMInstance< double >& instance) {
        if (!instance.exists(s))
          throw "'" + s + "' is not an attribute of instance '" + instance.name() + "'.";

        return s;
      }

      // After this method, ident doesn't contains the system name anymore.
      const PRMSystem< double >& O3prmrInterpreter::system(std::string& ident) {
        try {
          return prm()->getSystem(findSystemName(ident));
        } catch (const std::string&) {}

        if ((m_context->mainImport() != 0) && prm()->isSystem(m_context->mainImport()->value))
          return prm()->getSystem(m_context->mainImport()->value);

        throw "could not find any system or alias in '" + ident
            + "' and no default alias has been set.";
      }

      ///

      bool O3prmrInterpreter::observe(const ObserveCommand< double >* command) try {
        const typename PRMInference< double >::Chain& chain = command->chain;

        // Generate the inference engine if it doesn't exist.
        if (!m_inf) { generateInfEngine(*(command->system)); }

        // Prevent from something
        if (m_inf->hasEvidence(chain)) addWarning(command->leftValue + " is already observed");

        m_inf->addEvidence(chain, command->potentiel);

        if (m_verbose)
          m_log << "# Added evidence " << command->rightValue << " over attribute "
                << command->leftValue << std::endl;

        return true;

      } catch (OperationNotAllowed& ex) {
        addError("something went wrong when adding evidence " + command->rightValue + " over "
                 + command->leftValue + " : " + ex.errorContent());
        return false;

      } catch (const std::string& msg) {
        addError(msg);
        return false;
      }

      ///

      bool O3prmrInterpreter::unobserve(const UnobserveCommand< double >* command) try {
        std::string                            name  = command->value;
        typename PRMInference< double >::Chain chain = command->chain;

        // Prevent from something
        if (!m_inf || !m_inf->hasEvidence(chain)) {
          addWarning(name + " was not observed");
        } else {
          m_inf->removeEvidence(chain);

          if (m_verbose) m_log << "# Removed evidence over attribute " << name << std::endl;
        }

        return true;

      } catch (const std::string& msg) {
        addError(msg);
        return false;
      }

      ///
      void O3prmrInterpreter::query(const QueryCommand< double >* command) try {
        const std::string& query = command->value;

        if (m_inf_map.exists(command->system)) {
          m_inf = m_inf_map[command->system];
        } else {
          m_inf = nullptr;
        }

        // Create inference engine if it has not been already created.
        if (!m_inf) { generateInfEngine(*(command->system)); }

        // Inference
        if (m_verbose) {
          m_log << "# Starting inference over query: " << query << "... " << std::endl;
        }

        Timer timer;
        timer.reset();

        Tensor< double > m;
        m_inf->posterior(command->chain, m);

        // Compute spent time
        double t = timer.step();

        if (m_verbose) { m_log << "Finished." << std::endl; }

        if (m_verbose) { m_log << "# Time in seconds (accuracy ~0.001): " << t << std::endl; }

        // Show results

        if (m_verbose) { m_log << std::endl; }

        QueryResult result;
        result.command = query;
        result.time    = t;

        Instantiation                 j(m);
        const PRMAttribute< double >& attr = *(command->chain.second);

        for (j.setFirst(); !j.end(); j.inc()) {
          // auto label_value = j.val ( attr.type().variable() );
          auto        label_value = j.val(0);
          std::string label       = attr.type().variable().label(label_value);
          float       value       = float(m.get(j));

          SingleResult singleResult;
          singleResult.label = label;
          singleResult.p     = value;

          result.values.push_back(singleResult);

          if (m_verbose) { m_log << label << " : " << value << std::endl; }
        }

        m_results.push_back(result);

        if (m_verbose) { m_log << std::endl; }

      } catch (Exception& e) {
        GUM_SHOWERROR(e);
        throw "something went wrong while infering: " + e.errorContent();

      } catch (const std::string& msg) { addError(msg); }

      ///
      void O3prmrInterpreter::setEngine(const SetEngineCommand* command) {
        m_engine = command->value;
      }

      ///
      void O3prmrInterpreter::setGndEngine(const SetGndEngineCommand* command) {
        m_bn_engine = command->value;
      }

      ///
      void O3prmrInterpreter::generateInfEngine(const PRMSystem< double >& sys) {
        if (m_verbose) m_log << "# Building the inference engine... " << std::flush;

        //
        if (m_engine == "SVED") {
          m_inf = new SVED< double >(*(prm()), sys);

          //
        } else if (m_engine == "SVE") {
          m_inf = new SVE< double >(*(prm()), sys);

        } else {
          if (m_engine != "GRD") {
            addWarning("unkown engine '" + m_engine + "', use GRD insteed.");
          }

          MarginalTargetedInference< double >* bn_inf = nullptr;
          if (m_bn) { delete m_bn; }
          m_bn = new BayesNet< double >();
          BayesNetFactory< double > bn_factory(m_bn);

          if (m_verbose) m_log << "(Grounding the network... " << std::flush;

          sys.groundedBN(bn_factory);

          if (m_verbose) m_log << "Finished)" << std::flush;

          // bn_inf = new LazyPropagation<double>( *m_bn );
          bn_inf = new VariableElimination< double >(m_bn);

          auto grd_inf = new GroundedInference< double >(*(prm()), sys);
          grd_inf->setBNInference(bn_inf);
          m_inf = grd_inf;
        }

        m_inf_map.insert(&sys, m_inf);
        if (m_verbose) m_log << "Finished." << std::endl;
      }

      /* **************************************************************************
       */

      /// # of errors + warnings
      Size O3prmrInterpreter::count() const { return m_errors.count(); }

      ///
      Size O3prmrInterpreter::errors() const { return m_errors.error_count; }

      ///
      Size O3prmrInterpreter::warnings() const { return m_errors.warning_count; }

      ///
      ParseError O3prmrInterpreter::error(Idx i) const {
        if (i >= count()) throw "Index out of bound.";

        return m_errors.error(i);
      }

      /// Return container with all errors.
      ErrorsContainer O3prmrInterpreter::errorsContainer() const { return m_errors; }

      ///
      void O3prmrInterpreter::showElegantErrors(std::ostream& o) const {
        m_errors.elegantErrors(o);
      }

      ///
      void O3prmrInterpreter::showElegantErrorsAndWarnings(std::ostream& o) const {
        m_errors.elegantErrorsAndWarnings(o);
      }

      ///
      void O3prmrInterpreter::showErrorCounts(std::ostream& o) const {
        m_errors.syntheticResults(o);
      }

      /* **************************************************************************
       */

      ///
      void O3prmrInterpreter::addError(std::string msg) {
        m_errors.addError(msg, m_context->filename(), m_current_line, 0);

        if (m_verbose) m_log << m_errors.last().toString() << std::endl;
      }

      ///
      void O3prmrInterpreter::addWarning(std::string msg) {
        m_errors.addWarning(msg, m_context->filename(), m_current_line, 0);

        if (m_verbose) m_log << m_errors.last().toString() << std::endl;
      }

    }   // namespace o3prmr
  }   // namespace prm
}   // namespace gum
