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
 * @brief Headers of IPRMFactory.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_IPRM_FACTORY_H
#define GUM_IPRM_FACTORY_H

#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/agrum.h>


#include <agrum/PRM/PRM.h>

namespace gum {

  namespace prm {
    /**
     * @class IPRMFactory
     * @brief non-template interface-like parent for every PRM Factory
     *
     * @ingroup prm_group
     */
    struct IPRMFactory {
      virtual ~IPRMFactory(){};

      // virtual PRM<GUM_SCALAR>* prm() const=0;
      virtual PRMObject::prm_type currentType() const    = 0;
      virtual PRMObject*          getCurrent()           = 0;
      virtual const PRMObject*    getCurrent() const     = 0;
      virtual PRMObject*          closeCurrent()         = 0;
      virtual std::string         currentPackage() const = 0;
      // Class<GUM_SCALAR>& retrieveClass ( const std::string& name )=0;
      // PRMType& retrieveType ( const std::string& name )=0;
      // PRMType& retrieveCommonType ( const
      // std::vector<PRMClassElement<GUM_SCALAR>*>& elts )=0;
      virtual void        pushPackage(const std::string& name)                               = 0;
      virtual std::string popPackage()                                                       = 0;
      virtual void        addImport(const std::string& name)                                 = 0;
      virtual void        startDiscreteType(const std::string& name, std::string super = "") = 0;
      virtual void        addLabel(const std::string& l, std::string extends = "")           = 0;
      virtual void        endDiscreteType()                                                  = 0;
      virtual void        startDiscretizedType(const std::string& name)                      = 0;
      virtual void        addTick(double tick)                                               = 0;
      virtual void        endDiscretizedType()                                               = 0;
      virtual void        addRangeType(const std::string& name, long minVal, long maxVal)    = 0;
      virtual void        startClass(const std::string&        c,
                                     const std::string&        extends          = "",
                                     const Set< std::string >* implements       = nullptr,
                                     bool                      delayInheritance = false)
         = 0;
      virtual void continueClass(const std::string& c)        = 0;
      virtual void endClass(bool checkImplementations = true) = 0;
      virtual void startInterface(const std::string& i,
                                  const std::string& extends          = "",
                                  bool               delayInheritance = false)
         = 0;
      virtual void continueInterface(const std::string& name)                     = 0;
      virtual void addAttribute(const std::string& type, const std::string& name) = 0;
      virtual void endInterface()                                                 = 0;
      // virtual void addAttribute ( PRMAttribute<GUM_SCALAR>* attr ) =0;
      virtual void
         startAttribute(const std::string& type, const std::string& name, bool scalar_attr = false)
         = 0;
      virtual void continueAttribute(const std::string& name)                 = 0;
      virtual void addParent(const std::string& name)                         = 0;
      virtual void setRawCPFByFloatLines(const std::vector< float >& array)   = 0;
      virtual void setRawCPFByFloatColumns(const std::vector< float >& array) = 0;
      virtual void setCPFByFloatRule(const std::vector< std::string >& labels,
                                     const std::vector< float >&       values)
         = 0;
      virtual void setCPFByRule(const std::vector< std::string >& labels,
                                const std::vector< std::string >& values)
         = 0;
      virtual void setRawCPFByColumns(const std::vector< std::string >& array)                  = 0;
      virtual void setRawCPFByLines(const std::vector< std::string >& array)                    = 0;
      virtual void endAttribute()                                                               = 0;
      virtual void addParameter(const std::string& type, const std::string& name, double value) = 0;
      virtual void addAggregator(const std::string&                name,
                                 const std::string&                agg_type,
                                 const std::vector< std::string >& chains,
                                 const std::vector< std::string >& params,
                                 std::string                       type = "")
         = 0;
      virtual void addNoisyOrCompound(const std::string&                name,
                                      const std::vector< std::string >& chains,
                                      const std::vector< float >&       numbers,
                                      float                             leak,
                                      const std::vector< std::string >& label)
         = 0;
      virtual void addReferenceSlot(const std::string& type, const std::string& name, bool isArray)
         = 0;
      virtual void startSystem(const std::string& name)                          = 0;
      virtual void endSystem()                                                   = 0;
      virtual void addInstance(const std::string& type, const std::string& name) = 0;
      virtual void addInstance(const std::string&                      type,
                               const std::string&                      name,
                               const HashTable< std::string, double >& params)
         = 0;
      virtual void addArray(const std::string& type, const std::string& name, Size size) = 0;
      virtual void incArray(const std::string& l_i, const std::string& r_i)              = 0;
      virtual void setReferenceSlot(const std::string& left_instance,
                                    const std::string& left_reference,
                                    const std::string& right_instance)
         = 0;
      virtual void setReferenceSlot(const std::string& l_i, const std::string& r_i) = 0;

      virtual bool isClassOrInterface(const std::string& type) const     = 0;
      virtual bool isArrayInCurrentSystem(const std::string& name) const = 0;
    };
  } /* namespace prm */
} /* namespace gum */

#endif /* GUM_IPRM_FACTORY_H */
