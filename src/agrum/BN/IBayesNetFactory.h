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
 * @brief Header of the IBayesNetFactory class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_INTERFACE_BAYESNET_FACTORY_H
#define GUM_INTERFACE_BAYESNET_FACTORY_H

#include <string>
#include <vector>

#include <agrum/tools/graphicalModels/variableNodeMap.h>
#include <agrum/tools/multidim/implementations/multiDimAdressable.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum {
  /**
   * @class IBayesNetFactory
   * @headerfile IBayesNetFactory.h <agrum/BN/IBayesNetFactory.h>
   * @ingroup bn_group
   *
   * IBayesNetFactory is the non-template interface for @ref BayesNetFactory :
   * many ways to build a BN do not depend on the specification of the
   * GUM_SCALAR template argument (for instance for BN readers).
   */
  class IBayesNetFactory {
    public:
    /**
     * @brief The enumeration of states in which the factory can be in.
     *
     * Every documentation section's name indicates from which state you can
     * call it's methods, and in which state it places the factory.
     *
     * There is an exception for the delegated CPT definition methods which do
     * not change the state of the factory.
     */
    enum class factory_state : char {
      NONE,
      NETWORK,
      VARIABLE,
      PARENTS,
      RAW_CPT,
      FACT_CPT,
      FACT_ENTRY
    };

    IBayesNetFactory() : _verbose_(false){};
    // just to make some compilers happy
    virtual ~IBayesNetFactory(){};

    /**
     * @name verbosity control
     * @{
     */
    void setVerbose() { _verbose_ = true; };

    void resetVerbose() { _verbose_ = false; };

    bool isVerbose() { return _verbose_; };

    /**
     * @}
     */

    /// @{
    /// @brief Please @see BayesNetFactory<GUM_SCALAR> for documentation.
    virtual const DiscreteVariable& varInBN(NodeId id)                        = 0;
    virtual factory_state           state() const                             = 0;
    virtual NodeId                  variableId(const std::string& name) const = 0;
    virtual Size                    cptDomainSize(NodeId n) const             = 0;

    virtual void startNetworkDeclaration()                                                     = 0;
    virtual void addNetworkProperty(const std::string& propName, const std::string& propValue) = 0;
    virtual void endNetworkDeclaration()                                                       = 0;
    virtual void startVariableDeclaration()                                                    = 0;
    virtual void variableName(const std::string& name)                                         = 0;
    virtual void variableDescription(const std::string& desc)                                  = 0;
    virtual void variableType(const VarType& type)                                             = 0;
    virtual void addModality(const std::string& name)                                          = 0;
    virtual void addMin(const long& min)                                                       = 0;
    virtual void addMax(const long& max)                                                       = 0;
    virtual void setVariableCPTImplementation(MultiDimAdressable* impl)                        = 0;
    virtual NodeId endVariableDeclaration()                                                    = 0;
    virtual void   startParentsDeclaration(const std::string& var)                             = 0;
    virtual void   addParent(const std::string& var)                                           = 0;
    virtual void   endParentsDeclaration()                                                     = 0;
    virtual void   startRawProbabilityDeclaration(const std::string& var)                      = 0;
    virtual void   rawConditionalTable(const std::vector< std::string >& variables,
                                       const std::vector< float >&       rawTable)
       = 0;
    virtual void rawConditionalTable(const std::vector< float >& rawTable)                 = 0;
    virtual void endRawProbabilityDeclaration()                                            = 0;
    virtual void startFactorizedProbabilityDeclaration(const std::string& var)             = 0;
    virtual void startFactorizedEntry()                                                    = 0;
    virtual void endFactorizedEntry()                                                      = 0;
    virtual void setParentModality(const std::string& parent, const std::string& modality) = 0;
    virtual void setVariableValuesUnchecked(const std::vector< float >& values)            = 0;
    virtual void setVariableValues(const std::vector< float >& values)                     = 0;
    virtual void endFactorizedProbabilityDeclaration()                                     = 0;
    virtual void setVariable(const DiscreteVariable& var)                                  = 0;
    virtual void
       setVariableCPT(const std::string& varName, MultiDimAdressable* table, bool redefineParents)
       = 0;
    /// @}

    private:
    bool _verbose_;
  };
} /* namespace gum */

#endif   // GUM_ASBTRACT_BAYESNET_FACTORY_H
