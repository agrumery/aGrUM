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
#include <string_view>
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
      virtual ~IPRMFactory() {};

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
      virtual void        pushPackage(std::string_view name)                                        = 0;
      virtual std::string popPackage()                                                            = 0;
      virtual void        addImport(std::string_view name)                                        = 0;
      virtual void        startDiscreteType(std::string_view name, std::string_view super = "")   = 0;
      virtual void        addLabel(std::string_view l, std::string_view extends = "")             = 0;
      virtual void        endDiscreteType()                                                       = 0;
      virtual void        startDiscretizedType(std::string_view name)                             = 0;
      virtual void        addTick(double tick)                                                    = 0;
      virtual void        endDiscretizedType()                                                    = 0;
      virtual void        addRangeType(std::string_view name, long minVal, long maxVal)           = 0;
      virtual void        startClass(std::string_view           c,
                                     std::string_view           extends          = "",
                                     const Set< std::string >*  implements       = nullptr,
                                     bool                       delayInheritance = false)         = 0;
      virtual void        continueClass(std::string_view c)                                       = 0;
      virtual void        endClass(bool checkImplementations = true)                              = 0;
      virtual void        startInterface(std::string_view i,
                                         std::string_view extends          = "",
                                         bool             delayInheritance = false)               = 0;
      virtual void        continueInterface(std::string_view name)                                = 0;
      virtual void        addAttribute(std::string_view type, std::string_view name)              = 0;
      virtual void        endInterface()                                                          = 0;
      // virtual void addAttribute ( PRMAttribute<GUM_SCALAR>* attr ) =0;
      virtual void startAttribute(std::string_view type,
                                  std::string_view name,
                                  bool             scalar_attr = false)                              = 0;
      virtual void continueAttribute(std::string_view name)                                          = 0;
      virtual void addParent(std::string_view name)                                                  = 0;
      virtual void setRawCPFByFloatLines(const std::vector< float >& array)                          = 0;
      virtual void setRawCPFByFloatColumns(const std::vector< float >& array)                        = 0;
      virtual void setCPFByFloatRule(const std::vector< std::string >& labels,
                                     const std::vector< float >&       values)                       = 0;
      virtual void setCPFByRule(const std::vector< std::string >& labels,
                                const std::vector< std::string >& values)                            = 0;
      virtual void setRawCPFByColumns(const std::vector< std::string >& array)                       = 0;
      virtual void setRawCPFByLines(const std::vector< std::string >& array)                         = 0;
      virtual void endAttribute()                                                                    = 0;
      virtual void addParameter(std::string_view type, std::string_view name, double value)          = 0;
      virtual void addAggregator(std::string_view                  name,
                                 std::string_view                  agg_type,
                                 const std::vector< std::string >& chains,
                                 const std::vector< std::string >& params,
                                 std::string_view                  type = "")                        = 0;
      virtual void addNoisyOrCompound(std::string_view                  name,
                                      const std::vector< std::string >& chains,
                                      const std::vector< float >&       numbers,
                                      float                             leak,
                                      const std::vector< std::string >& label)                       = 0;
      virtual void addReferenceSlot(std::string_view type, std::string_view name, bool isArray)
          = 0;
      virtual void startSystem(std::string_view name)                                       = 0;
      virtual void endSystem()                                                              = 0;
      virtual void addInstance(std::string_view type, std::string_view name)                = 0;
      virtual void addInstance(std::string_view                        type,
                               std::string_view                        name,
                               const HashTable< std::string, double >& params)             = 0;
      virtual void addArray(std::string_view type, std::string_view name, Size size)        = 0;
      virtual void incArray(std::string_view l_i, std::string_view r_i)                    = 0;
      virtual void setReferenceSlot(std::string_view left_instance,
                                    std::string_view left_reference,
                                    std::string_view right_instance)                        = 0;
      virtual void setReferenceSlot(std::string_view l_i, std::string_view r_i)            = 0;

      virtual bool isClassOrInterface(std::string_view type) const     = 0;
      virtual bool isArrayInCurrentSystem(std::string_view name) const = 0;
    };
  } /* namespace prm */
} /* namespace gum */

#endif /* GUM_IPRM_FACTORY_H */
