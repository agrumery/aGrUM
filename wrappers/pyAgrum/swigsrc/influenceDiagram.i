/**
 *
 *  Copyright 2005-2024 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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

%ignore gum::InfluenceDiagram::variableNodeMap;
%ignore gum::InfluenceDiagram::insertArc;
%ignore gum::InfluenceDiagram::getPartialTemporalOrder;
%ignore gum::InfluenceDiagram:: addUtilityNode ( const gum::DiscreteVariable& variable,gum::MultiDimImplementation<GUM_SCALAR>* aContent, NodeId id = 0 );

%include "extensions/IDGenerator.h"

%{
#include "extensions/IDGenerator.h"
%}


IMPROVE_DIRECTED_GRAPHICAL_MODEL_API(gum::InfluenceDiagram);

%extend gum::InfluenceDiagram {
   bool loadBIFXML(std::string name, PyObject *l=(PyObject*)0) {
    try {
        gum::BIFXMLIDReader<GUM_SCALAR> reader(self,name);
        reader.proceed();
        return true;
    } catch (gum::IOError& e) {
        throw(e);
    }

    return false;
  };

  void saveBIFXML(std::string name) {
      gum::BIFXMLIDWriter<GUM_SCALAR> writer;
      writer.write( name, *self );
  };

  %pythoncode {
def addVariables(self,listFastVariables,default_nbr_mod=2):
   """
   Add a list of variable in the form of 'fast' syntax.

   Parameters
   ----------
   listFastVariables: List[str]
     the list of variables following :ref:`fast syntax<Quick specification of (randomly parameterized) graphical models>` extended for :func:`pyAgrum.fastID`.
   default_nbr_mod: int
     the number of modalities for the variable if not specified in the fast description. Note that default_nbr_mod=1 is
     mandatory to create variables with only one modality (for utility for instance).

   Returns
   -------
   List[int]
     the list of created ids.
   """
   return [self.add(descr,default_nbr_mod) for descr in listFastVariables]

def addArcs(self,listArcs):
  """
  add a list of arcs in te model.

  Parameters
  ----------
  listArcs : List[Tuple[int,int]]
    the list of arcs
  """
  for arc in listArcs:
    self.addArc(*arc)


def addStructureListener(self,whenNodeAdded=None,whenNodeDeleted=None,whenArcAdded=None,whenArcDeleted=None):
  """
  Add the listeners in parameters to the list of existing ones.

  Parameters
  ----------
  whenNodeAdded : lambda expression
    a function for when a node is added
  whenNodeDeleted : lambda expression
    a function for when a node is removed
  whenArcAdded : lambda expression
    a function for when an arc is added
  whenArcDeleted : lambda expression
    a function for when an arc is removed
  """
  if [whenNodeAdded,whenNodeDeleted,whenArcAdded,whenArcDeleted]==[None,None,None,None]:
    return

  if not hasattr(self,"_listeners"):
    self._listeners=[]

  nl = PythonBNListener(self, self.variableNodeMap())
  if whenNodeAdded is not None:
    nl.setWhenNodeAdded(whenNodeAdded)
  if whenNodeDeleted is not None:
    nl.setWhenNodeDeleted(whenNodeDeleted)
  if whenArcAdded is not None:
    nl.setWhenArcAdded(whenArcAdded)
  if whenArcDeleted is not None:
    nl.setWhenArcDeleted(whenArcDeleted)

  self._listeners.append(nl)


def __getstate__(self):
    pyAgrum.base._gum_add_properties_while_getstate_(self)
    state={
          "chance":[self.variable(i).toFast() for i in self.nodes() if self.isChanceNode(i)],
          "utility":[self.variable(i).toFast() for i in self.nodes() if self.isUtilityNode(i)],
          "decision":[self.variable(i).toFast() for i in self.nodes() if self.isDecisionNode(i)],
          "parents":{**{self.variable(i).name():list(self.cpt(i).names)[1:] for i in self.nodes()  if self.isChanceNode(i)},
                 **{self.variable(i).name():list(self.utility(i).names)[1:] for i in self.nodes()  if self.isUtilityNode(i)},
                 **{self.variable(i).name():[self.variable(j).name() for j in self.parents(i)] for i in self.nodes() if self.isDecisionNode(i)}},
          "cpt":{self.variable(i).name():self.cpt(i)[:].flatten().tolist() for i in self.nodes() if self.isChanceNode(i)},
          "reward":{self.variable(i).name():self.utility(i)[:].flatten().tolist() for i in self.nodes() if self.isUtilityNode(i)},
          "properties":{k:self.property(k) for k in self.properties()}
    }
    return state

def __setstate__(self,state):
    self.__init__()
    for fastvar in state['chance']:
        self.addChanceNode(fastvar)
    for fastvar in state['utility']:
        self.addUtilityNode(fastvar)
    for fastvar in state['decision']:
        self.addDecisionNode(fastvar)
    self.beginTopologyTransformation()
    for son in state['parents']:
        for father in state['parents'][son]:
            self.addArc(father,son)
    self.endTopologyTransformation()
    for node in state['cpt']:
        self.cpt(node).fillWith(state['cpt'][node])
    for node in state['reward']:
        self.utility(node).fillWith(state['reward'][node])
    for prop in state['properties']:
        self.setProperty(prop,state['properties'][prop])
    return self


def toFast(self, filename: str = None) -> str:
  """
  Export the influence Diagram as *fast* syntax (in a string or in a python file)

  Parameters
  ----------
  filename : Optional[str]
    the name of the file (including the prefix), if None , use sys.stdout
  """
  def _toFastVar(model,i):
    res=""
    if model.isUtilityNode(i):
      res="$"
    elif model.isDecisionNode(i):
      res="*"
    return res+model.variable(i).toFast()

  def _toFastBN(model,pythoncode=False):
    res = []
    sovars = set()
    for x, y in model.arcs():
      if x in sovars:
        src = model.variable(x).name()
      else:
        src = _toFastVar(model,x)
        sovars.add(x)
      if y in sovars:
        dst = model.variable(y).name()
      else:
         dst = _toFastVar(model,y)
         sovars.add(y)
      res.append(f"{src}->{dst}")

    for x in model.nodes():
      if x not in sovars:
         res .append(_toFastVar(model,x))

    if pythoncode:
      return 'model=gum.fastID("""'+';\n     '.join(res)+'""")'
    else:
      return ';'.join(res)

  if filename is None:
    return _toFastBN(self)
  else:
    with open(filename, "w") as pyfile:
      print(_toFastBN(self,pythoncode=True), file=pyfile)
  }
}
