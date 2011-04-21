
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
 * @brief Headers of SkoolInterpreter
 *
 * @author Pierre-Henri WUILLEMIN, Ni NI, Lionel TORTI & Vincent RENAUDINEAU
 */

#ifndef SKOORSYNTAXTREE_H
#define SKOORSYNTAXTREE_H

#include <string>
#include <vector>
#include <map>

#include <agrum/prm/system.h>
#include <agrum/prm/PRMInference.h>

namespace gum {
  namespace prm {    
    namespace skoor {

/**
 * This is an abstract class.
 * It represent a skoor command.
 * There five types of command, such sub-classes.
 * */
class SkoorCommand {
public :
  int line;
  enum Type {Observe, Unobserve, Query, SetEngine, SetGndEngine};
  
  SkoorCommand( int line ) : line(line) {}
  SkoorCommand( const SkoorCommand & c ) : line(c.line) {}
  virtual ~SkoorCommand() {}
  
  virtual Type type() const = 0;
  virtual std::string toString() const = 0;
};

///
class ImportCommand {
public :
  ImportCommand( int line, const std::string & value, const std::string & alias )
   : line(line), value(value), alias(alias) {}
  ImportCommand( const ImportCommand & c )
   : line(c.line), value(c.value), alias(c.alias) {}

  int line;
  std::string value;
  std::string alias;
  
  std::string toString() const {
    return "import " + value + (alias.empty()?"":"as "+alias) + ";";
  }
};

///
class SetEngineCommand : public SkoorCommand {
public :
  SetEngineCommand( int line, const std::string & value ) : SkoorCommand(line), value(value) {}
  SetEngineCommand( const SetEngineCommand & c ) : SkoorCommand(c), value(c.value) {}
  
  std::string value;
  
  Type type() const { return SetEngine; }
  std::string toString() const {
    return "engine " + value + ";";
  }
};

///
class SetGndEngineCommand : public SkoorCommand {
public :
  SetGndEngineCommand( int line, const std::string & value ) : SkoorCommand(line), value(value) {}
  SetGndEngineCommand( const SetGndEngineCommand & c ) : SkoorCommand(c), value(c.value) {}
  
  std::string value;
  
  Type type() const { return SetGndEngine; }
  std::string toString() const {
    return "grd_engine " + value + ";";
  }
};

///
class ObserveCommand : public SkoorCommand {
public :
  ObserveCommand( int line, const std::string & leftValue, const std::string & rightValue )
   : SkoorCommand(line), leftValue(leftValue), rightValue(rightValue), system(0) {}
  ObserveCommand( const ObserveCommand & c )
   : SkoorCommand(c), leftValue(c.leftValue), rightValue(c.rightValue), system(c.system), chain(c.chain) {}
  
  std::string leftValue;
  std::string rightValue;
  const System * system;
  PRMInference::Chain chain;
  Potential<prm_float> potentiel;
  
  Type type() const { return Observe; }
  std::string toString() const {
    return leftValue + " = " + rightValue + ";";
  }
};

///
class UnobserveCommand : public SkoorCommand {
public :
  std::string value;
  const System * system;
  PRMInference::Chain chain;
  
  UnobserveCommand( int line, const std::string & value )
   : SkoorCommand(line), value(value), system(0) {}
  UnobserveCommand( const UnobserveCommand & c )
   : SkoorCommand(c), value(c.value), system(c.system), chain(c.chain) {}
  
  Type type() const { return Unobserve; }
  std::string toString() const {
    return "unobserve " + value + ";";
  }
};

///
class QueryCommand : public SkoorCommand {
public :
  QueryCommand( int line, const std::string & value ) : SkoorCommand(line) { this->value = value; }
  
  std::string value;
  const System * system;
  PRMInference::Chain chain;
  
  Type type() const { return Query; }
  std::string toString() const {
    return "? " + value + ";";
  }
};

/**
 * This class contains a skoor session.
 * It have a name and a sequence of commands.
 * */
class SkoorSession {
  /// The session name;
  std::string m_name;
  /// A sequence of commands.
  std::vector<SkoorCommand *> m_commands;
  std::map<const System *,PRMInference *> m_infEngineMap;
  
public:
  SkoorSession(const std::string & name = std::string());
  SkoorSession( const SkoorSession & s );
  virtual ~SkoorSession();
  
  std::string getName() const;
  
  std::vector<SkoorCommand *> getCommands() const;
  void addObserve( int line, const std::string & leftValue, const std::string & rightValue );
  void addUnobserve( int line, const std::string & value );
  void addQuery( int line, const std::string & value );
  void addSetEngine( int line, const std::string & value );
  void addSetGndEngine( int line, const std::string & value );
  void addCommand( const SkoorCommand * command );
  
  virtual std::string toString() const;
  SkoorSession & operator+=( const SkoorSession & c );

private:
  void addCommand( SkoorCommand * command );
};

/**
  Represent a skoor context, with an import, and some sequencials commands.
 */
class SkoorContext {
  std::string m_filename;
  std::string m_package;
  std::vector<SkoorSession *> m_sessions;
  std::vector<ImportCommand *> m_imports;
  ImportCommand * m_mainImport;
  
public :
  SkoorContext(const std::string & filename = std::string());
  SkoorContext( const SkoorContext & s );
  virtual ~SkoorContext();
  
  const ImportCommand * getMainImport() const { return m_mainImport; }

  std::string getFilename() const;
  
  std::string getPackage() const;
  void setPackage( const std::string & package );

  std::string aliasToImport( const std::string & alias );
  std::vector<ImportCommand *> getImports() const;
  void addImport( int line, const std::string & import, const std::string & alias );
  void addImport( int line, const std::string & import, bool ismain );
  void addImport( const ImportCommand & i ) { 
    m_imports.push_back( new ImportCommand(i.line,i.value,i.alias) );
    if ( i.alias == "default" ) m_mainImport = m_imports.back(); }
  
  std::vector<SkoorSession *> getSessions() const;
  void addSession( SkoorSession * session );
  void addSession( const SkoorSession & session );
  
  void clear();
  
  virtual std::string toString() const;
  SkoorContext & operator+=( const SkoorContext & c );
};


    } // namespace skoor
  } // namespace prm
} // namespace gum

#endif //SKOORSYNTAXTREE_H

