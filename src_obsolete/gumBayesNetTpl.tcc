/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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

/** @file
 * @brief Template implementation of gumBayesNetTpl.h classes.
 *
 * @author Lionel Torti
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// Default constructor
template<typename T_DATA> INLINE
gumBayesNetTpl<T_DATA>::gumBayesNetTpl()
{
  GUM_CONSTRUCTOR(gumBayesNetTpl);
}

// Copy constructor
template<typename T_DATA> INLINE
gumBayesNetTpl<T_DATA>::gumBayesNetTpl(const gumBayesNetTpl<T_DATA>& source):
  gumTopologyTpl(source)
{
  GUM_CONSTRUCTOR(gumBayesNetTpl);

  gumPotential<T_DATA> *sourcePtr = NULL;
  gumPotential<T_DATA> *copyPtr = NULL;
  for (gumHashTableIterator<gumId, gumPotential<T_DATA>*> cptIter = source.__probaMap.begin();
       cptIter != source.__probaMap.end();
       ++cptIter)
  {
    // First we build the node's CPT
    copyPtr = new gumPotential<T_DATA>();
    (*copyPtr) << gumTopologyTpl::variable(cptIter.key());
    // Add it's parents
    const gumList< gumRefPtr< gumArc > > &parentArcs = gumTopologyTpl::_dag.getParentArcs(cptIter.key());
    for (gumListIterator< gumRefPtr< gumArc > > arcIter = parentArcs.begin();
         arcIter != parentArcs.end();
         ++arcIter)
    {
      (*copyPtr) << gumTopologyTpl::variable((*arcIter)->tail());
    }

    // Second, we fill the CPT
    sourcePtr = *cptIter;
    gumInstantiation srcInst(*sourcePtr);
    gumInstantiation cpInst(*copyPtr);

    // Slowest but safest
    for (cpInst.setFirst(); !cpInst.end(); cpInst.inc()) {
      for (gumIdx i = 0; i < cpInst.nbrDim(); i++) {
        gumId id = gumTopologyTpl::getIdFromVariable(cpInst.variable(i));
        srcInst.chgVal(source.variable(id), cpInst.val(i));
      }
      (*copyPtr)[cpInst] = (*sourcePtr)[srcInst];
    }
    // We add the CPT to the CPT's hashmap
    __probaMap.insert(cptIter.key(), copyPtr);
  }
}

// Destructor
template<typename T_DATA> INLINE
gumBayesNetTpl<T_DATA>::~gumBayesNetTpl()
{
  GUM_DESTRUCTOR(gumBayesNetTpl);
  for (gumHashTableIterator<gumId, gumPotential<T_DATA>*> iter = __probaMap.begin();
       iter != __probaMap.end();
       ++iter)
  {
    delete *iter;
  }
}

/*// Add a variable, it's associate node and it's CPT
template<typename T_DATA> INLINE
gumId
gumBayesNetTpl<T_DATA>::addVariable(const gumDiscreteVariable * const variable, gumId varId)
{
  gumId id = gumTopologyTpl::addVariable(variable, varId);

  gumPotential<T_DATA> *cpt = new gumPotential<T_DATA>();
  (*cpt) << *(_variableMap[id]);
  __probaMap.insert(id, cpt);

  return id;
}*/

// Add a variable, it's associate node and it's CPT
template<typename T_DATA> INLINE
gumId
gumBayesNetTpl<T_DATA>::addVariable(const gumDiscreteVariable * const variable)
{
  gumId id = gumTopologyTpl::addVariable(variable);

  gumPotential<T_DATA> *cpt = new gumPotential<T_DATA>();
  (*cpt) << *(_variableMap[id]);
  __probaMap.insert(id, cpt);

  return id;
}

// Erase a variable and update dependent CPTs.
template<typename T_DATA> INLINE
void
gumBayesNetTpl<T_DATA>::eraseVariable(gumId varId)
{
  if (_variableMap.exists(varId))
  {
    // Reduce the variable child's CPT
    const gumList< gumRefPtr< gumArc > >& childs = _dag.getChildrenArcs(varId);
    for (gumListIterator< gumRefPtr< gumArc > > iter = childs.begin();
         iter != childs.end();
         ++iter)
    {
      gumId childId = (*iter)->head();
      __probaMap[childId]->eraseDim(*_variableMap[varId]);
    }
    _dag.eraseNode(varId);
    delete _variableMap[varId];
    _variableMap.erase(varId);
    delete __probaMap[varId];
    __probaMap.erase(varId);
  }
}

// Returns the CPT of a variable
template<typename T_DATA> INLINE
const gumPotential<T_DATA>&
gumBayesNetTpl<T_DATA>::cpt(gumId varId) const
{
  return *(__probaMap[varId]);
}

// Add an arc in the BN, and update arc.head's CPT
template<typename T_DATA> INLINE
const gumArc&
gumBayesNetTpl<T_DATA>::insertArc(gumId tail, gumId head)
{
  const gumArc& realArc = gumTopologyTpl::insertArc(tail, head);
  // Find concerned variables
  gumDiscreteVariable *parent;
  gumPotential<T_DATA> *childCPT;
  parent = _variableMap[tail];
  childCPT = __probaMap[head];

  // Add parent in the child's CPT
  (*childCPT) << *parent;

  return realArc;
}

// Removes an arc in the BN, and update head's CTP
template<typename T_DATA> INLINE
void
gumBayesNetTpl<T_DATA>::eraseArc(gumId tail, gumId head)
{
  if (_variableMap.exists(tail) && _variableMap.exists(head))
  {
    gumTopologyTpl::eraseArc(tail, head);

    // Find concerned variables
    gumDiscreteVariable *parent;
    gumPotential<T_DATA> *childCPT;
    parent = _variableMap[tail];
    childCPT = __probaMap[head];

    // Remove parent froms child's CPT
    (*childCPT) >> *parent;
  }
}

// Clear every variables, nodes, arcs, and CPTs of this BN
template<typename T_DATA> INLINE
void
gumBayesNetTpl<T_DATA>::clear()
{
  gumTopologyTpl::clear();

  for (gumHashTableIterator<gumId, gumPotential<T_DATA>*> iter = __probaMap.begin();
       iter != __probaMap.end();
       ++iter)
  {
    delete *iter;
  }

  __probaMap.clear();
}

// When domain changes, need to modify the CPT
template<typename T_DATA> INLINE
void
gumBayesNetTpl<T_DATA>::variableDomainChanged(gumDiscreteVariableEvent& e)
{
  gumDiscreteVariable* varPtr = e.getSource();
  gumId varId = _variableMap.keyByVal(varPtr);

  delete __probaMap[varId];
  __probaMap[varId] = new gumPotential<T_DATA>();

  *(__probaMap[varId]) << *varPtr;

  // Re-adding the parents in the CPT
  for (gumArcListIterator<gumArc> iter = getDAG().beginParentArcs(varId);
       iter != getDAG().endArcs();
       ++iter)
  {
    *(__probaMap[varId]) << *(_variableMap[iter->tail()]);
  }

  // Updating it's chidren CPTs
  for (gumArcListIterator<gumArc> i = getDAG().beginChildrenArcs(varId);
       i != getDAG().endArcs();
       ++i)
  {
    delete __probaMap[i->head()];
    __probaMap[i->head()] = new gumPotential<T_DATA>();
    *(__probaMap[i->head()]) << *(_variableMap[i->head()]);

    for (gumArcListIterator<gumArc> j = getDAG().beginParentArcs(i->head());
         j != getDAG().endArcs();
         ++j)
    {
      *(__probaMap[i->head()]) << *(_variableMap[j->tail()]);
    }
  }
}

// Copy Operator
template<typename T_DATA> INLINE
gumBayesNetTpl<T_DATA>&
gumBayesNetTpl<T_DATA>::operator=(const gumBayesNetTpl<T_DATA>& source)
{
  gumPotential<T_DATA> *sourcePtr, *copyPtr;
  for (gumHashTable<gumId, gumPotential<T_DATA>*> iter = source.__probaMap.begin();
       iter != source.__probaMap.end();
       ++iter)
  {
    sourcePtr = *iter;
    copyPtr = new gumPotential<T_DATA>(*sourcePtr);
    __probaMap.insert(iter.key(), copyPtr);
  }
  return *this;
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

