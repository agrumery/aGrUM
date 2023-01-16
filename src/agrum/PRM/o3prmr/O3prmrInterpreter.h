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

#ifndef SKOORINTERPRETER_H
#define SKOORINTERPRETER_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  include <agrum/PRM/o3prm/O3prmReader.h>
#  include <agrum/PRM/o3prmr/O3prmrContext.h>

#endif   // DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace prm {

    namespace o3prmr {

      /**
       *
       * */
      class SingleResult {
        public:
        std::string label;
        float       p;
      };

      /**
       *
       * */
      class QueryResult {
        public:
        double                      time;
        std::string                 command;
        std::vector< SingleResult > values;
      };

      /**
       * @brief Represents a O3PRMR context.
       * O3prmr use the @<double@> version of the O3PRM templatized ecosystem
       */
      class O3prmrInterpreter {
        public:
        /// This constructor create an empty context.
        O3prmrInterpreter();
        /// Destructor. Delete current context.
        ~O3prmrInterpreter();

        /**
         * Interpret the file or the command line.
         * If syntax mode is activated, check only syntax and semantic.
         * If errors occured, return false. Errors messages can be retrieve be
         * errorsContainer() methods.
         * If any errors occured, return true.
         * Requests results can be retrieve be results() methods.
         * */
        bool interpretFile(const std::string& filename);
        bool interpretLine(const std::string& line);

        /// Retrieve prm object.
        const gum::prm::PRM< double >* prm() const;
        /// Retrieve inference motor object.
        const gum::prm::PRMInference< double >* inference() const;
        /// Return a vector of QueryResults.
        /// Each QueryResults is a struct with query command, time and values,
        /// a vector of struct SingleResult, with pair label/value.
        const std::vector< QueryResult >& results() const;
        /// Return container with all errors.
        ErrorsContainer errorsContainer() const;

        /// Getter and setter for the context.
        O3prmrContext< double >* getContext() const;
        void                     setContext(O3prmrContext< double >* context);

        /// Root paths to search from there packages.
        /// Default are working dir, request file dir if any
        /// and one is calculate from package if any.
        std::vector< std::string > getPaths() const;
        void                       addPath(std::string path);
        void                       clearPaths();

        /// syntax mode don't process anything, just check syntax. Default is
        /// false.
        bool isInSyntaxMode() const;
        void setSyntaxMode(bool f);

        /// verbose mode show more details on the program execution. Default is
        /// false.
        bool isVerboseMode() const;
        void setVerboseMode(bool f);

        /**
         * En cas d'échec, l'API de gestion d'erreurs est présente.
         * */
        /// # of errors + warnings
        Size count() const;
        /// # of errors
        Size errors() const;
        /// # of warnings
        Size warnings() const;
        /// throw a string error if i >= count
        ParseError error(Idx i) const;
        /// send on std::cerr the list of errors
        void showElegantErrors(std::ostream& o = std::cerr) const;
        /// send on std::cerr the list of errors or warnings
        void showElegantErrorsAndWarnings(std::ostream& o = std::cerr) const;
        /// send on std::cerr the number of errors and the number of warnings
        void showErrorCounts(std::ostream& o = std::cerr) const;

        private:
        bool checkSemantic(O3prmrContext< double >* context);
        bool checkSetEngine(SetEngineCommand* command);
        bool checkSetGndEngine(SetGndEngineCommand* command);
        bool checkObserve(ObserveCommand< double >* command);
        bool checkUnobserve(UnobserveCommand< double >* command);
        bool checkQuery(QueryCommand< double >* command);

        bool interpret(O3prmrContext< double >* c);
        bool import(O3prmrContext< double >* context, std::string import);
        bool observe(const ObserveCommand< double >* command);
        bool unobserve(const UnobserveCommand< double >* command);
        void query(const QueryCommand< double >* command);
        void setEngine(const SetEngineCommand* command);
        void setGndEngine(const SetGndEngineCommand* command);

        std::string findSystemName(std::string& s);
        std::string findInstanceName(std::string& s, const gum::prm::PRMSystem< double >& sys);
        std::string findAttributeName(const std::string&                     s,
                                      const gum::prm::PRMInstance< double >& instance);
        const PRMSystem< double >& system(std::string& ident);
        void                       generateInfEngine(const gum::prm::PRMSystem< double >& sys);

        void addError(std::string msg);
        void addWarning(std::string msg);

        std::string _readFile_(const std::string& file);

        O3prmrContext< double >*                m_context;
        gum::prm::o3prm::O3prmReader< double >* m_reader;
        std::vector< std::string >              m_paths;
        gum::BayesNet< double >*                m_bn;
        // Don't delete this, the m_inf_map will do it
        gum::prm::PRMInference< double >*                                m_inf;
        HashTable< const PRMSystem< double >*, PRMInference< double >* > m_inf_map;
        std::string                                                      m_engine;
        std::string                                                      m_bn_engine;
        std::vector< QueryResult >                                       m_results;
        gum::ErrorsContainer                                             m_errors;
        bool                                                             m_syntax_flag;
        bool                                                             m_verbose;
        std::ostream&                                                    m_log;
        int                                                              m_current_line;

        //// We need this when using grounded inference
        // gum::BayesNetInference<gum::prm::prm_float>* bn;

      };   // class O3prmrInterpreter

    }   // namespace o3prmr
  }     // namespace prm
}   // namespace gum

#endif   // GUM_SKOORINTERPRETER_H
