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
 * @brief Headers of IPRMFactory.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_IPRM_FACTORY_H
#define GUM_IPRM_FACTORY_H

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <limits>

#include <agrum/config.h>

#include <agrum/variables/discreteVariable.h>
#include <agrum/multidim/multiDimSparse.h>

#include <agrum/PRM/utils_prm.h>
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
      virtual PRMObject::PRMType currentType() const = 0;
      virtual PRMObject *getCurrent() = 0;
      virtual const PRMObject *getCurrent() const = 0;
      virtual PRMObject *closeCurrent() = 0;
      virtual std::string currentPackage() const = 0;
      // Class<GUM_SCALAR>& retrieveClass ( const std::string& name )=0;
      // Type<GUM_SCALAR>& retrieveType ( const std::string& name )=0;
      // Type<GUM_SCALAR>& retrieveCommonType ( const
      // std::vector<ClassElement<GUM_SCALAR>*>& elts )=0;
      virtual void pushPackage(const std::string &name) = 0;
      virtual std::string popPackage() = 0;
      virtual void startDiscreteType(const std::string &name,
                                     std::string super = "") = 0;
      virtual void addLabel(const std::string &l, std::string extends = "") = 0;
      virtual void endDiscreteType() = 0;
      virtual void startClass(const std::string &c, const std::string &extends = "",
                              const Set<std::string> *implements = nullptr) = 0;
      virtual void endClass() = 0;
      virtual void startInterface(const std::string &i,
                                  const std::string &extends = "") = 0;
      virtual void addAttribute(const std::string &type,
                                const std::string &name) = 0;
      virtual void endInterface() = 0;
      // virtual void addAttribute ( Attribute<GUM_SCALAR>* attr ) =0;
      virtual void startAttribute(const std::string &type,
                                  const std::string &name) = 0;
      virtual void addParent(const std::string &name) = 0;
      virtual void setRawCPFByFloatLines(const std::vector<float> &array) = 0;
      virtual void setRawCPFByFloatColumns(const std::vector<float> &array) = 0;
      virtual void setCPFByFloatRule(const std::vector<std::string> &labels,
                                     const std::vector<float> &values) = 0;
      virtual void endAttribute() = 0;
      virtual void addParameter(const std::string &type, const std::string &name,
                                std::string value = "") = 0;
      virtual void addAggregator(const std::string &name,
                                 const std::string &agg_type,
                                 const std::vector<std::string> &chains,
                                 const std::vector<std::string> &params) = 0;
      virtual void addNoisyOrCompound(const std::string &name,
                                      const std::vector<std::string> &chains,
                                      const std::vector<float> &numbers, float leak,
                                      const std::vector<std::string> &label) = 0;
      virtual void addReferenceSlot(const std::string &type, const std::string &name,
                                    bool isArray) = 0;
      virtual void startSystem(const std::string &name) = 0;
      virtual void endSystem() = 0;
      virtual void addInstance(const std::string &type, const std::string &name) = 0;
      virtual void addArray(const std::string &type, const std::string &name,
                            Size size) = 0;
      virtual void incArray(const std::string &l_i, const std::string &r_i) = 0;
      virtual void setReferenceSlot(const std::string &left_instance,
                                    const std::string &left_reference,
                                    const std::string &right_instance) = 0;
      virtual void setReferenceSlot(const std::string &l_i,
                                    const std::string &r_i) = 0;
      virtual void setParameter(const std::string &i, const std::string &p,
                                const std::string &v) = 0;

      virtual bool isClassOrInterface(const std::string &type) const = 0;
      virtual bool isArrayInCurrentSystem(const std::string &name) const = 0;
    };
  } /* namespace prm */
} /* namespace gum */

#endif /* GUM_IPRM_FACTORY_H */
