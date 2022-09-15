"""
The purpose of this module is to provide tools for mapping Influcence Diagram (and inference) in dot language in order to
be displayed/saved as image.
"""

# (c) Copyright by Pierre-Henri Wuillemin, 2022
# (pierre-henri.wuillemin@lip6.fr)

# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.

# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!

import time
import math
import hashlib
from tempfile import mkdtemp

import pydot as dot

import pyAgrum as gum
import pyAgrum.lib._colors as gumcols
from pyAgrum.lib.proba_histogram import saveFigProba

def ID2dot(diag, size=None):
  """
  create a pydot representation of the influence diagram

  Parameters
  ----------
  diag: pyAgrum.InfluenceDiagram
    the model
  size: int|str
    the size of the visualization

  Returns
  -------
  pydot.Dot
    the dot representation of the influence diagram
  """
  res = "digraph  { \n graph [bgcolor=transparent];\n"

  # chance node
  res += f'''
    node [fillcolor="{gum.config["influenceDiagram", "default_chance_bgcolor"]}",
          fontcolor="{gum.config["influenceDiagram", "default_chance_fgcolor"]}",
          style=filled,shape={gum.config["influenceDiagram", "chance_shape"]}, 
          height=0,margin=0.1];
'''
  for node in diag.nodes():
    if diag.isChanceNode(node):
      res += '   "' + diag.variable(node).name() + '";' + "\n"

  # decision node
  res += f'''
    node [fillcolor="{gum.config["influenceDiagram", "default_decision_bgcolor"]}",
          fontcolor="{gum.config["influenceDiagram", "default_decision_fgcolor"]}",
          style=filled,shape={gum.config["influenceDiagram", "decision_shape"]}, 
          height=0,margin=0.1];
'''
  for node in diag.nodes():
    if diag.isDecisionNode(node):
      res += '   "' + diag.variable(node).name() + '";' + "\n"

  # utility node
  res += f'''
    node [fillcolor="{gum.config["influenceDiagram", "default_utility_bgcolor"]}",
          fontcolor="{gum.config["influenceDiagram", "default_utility_fgcolor"]}",
          style=filled,shape={gum.config["influenceDiagram", "utility_shape"]}, height=0,margin=0.1];
'''
  for node in diag.nodes():
    if diag.isUtilityNode(node):
      res += '   "' + diag.variable(node).name() + '";' + "\n"

  # arcs
  res += "\n"
  for node in diag.nodes():
    for chi in diag.children(node):
      res += '  "' + diag.variable(node).name() + '"->"' + \
             diag.variable(chi).name() + '"'
      if diag.isDecisionNode(chi):
        res += f' [style="{gum.config["influenceDiagram", "decision_arc_style"]}"]'
      elif diag.isUtilityNode(chi):
        res += f' [style="{gum.config["influenceDiagram", "utility_arc_style"]}"]'
      res += ";\n"
  res += "}"

  g = dot.graph_from_dot_data(res)[0]

  # workaround for some badly parsed graph (pyparsing>=3.03)
  g.del_node('"\\n"')

  if size is None:
    size = gum.config["influenceDiagram", "default_id_size"]
  g.set_size(size)
  return g


def LIMIDinference2dot(diag, size, engine, evs, targets):
  """
  create a pydot representation of an inference in a influence diagram

  Parameters
  ----------
  diag: pyAgrum.InfluenceDiagram
    the model
  size: float|str
    the size of the rendered graph
  engine: pyAgrum.InfluenceDiagramInference
    the inference algorithm used. If None, ShaferShenoyLIMIDInference will be used
  evs: Dict[str,str|int|List[float]]
    the evidence
  targets: Set[str]
    set of targetted variable. If targets={} then each node is a target

  Returns
  -------
  pydot.Dot
    the representation of the inference
  """
  startTime = time.time()
  if engine is None:
    ie = gum.ShaferShenoyLIMIDInference(diag)
  else:
    ie = engine
  ie.setEvidence(evs)
  ie.makeInference()
  stopTime = time.time()
  meu = ie.MEU()

  temp_dir = mkdtemp("", "tmp", None)  # with TemporaryDirectory() as temp_dir:

  dotstr = "digraph structs {\n  fontcolor=\"" + \
           gumcols.getBlackInTheme() + "\";bgcolor=\"transparent\";"

  fmt = '.' + gum.config["influenceDiagram", "utility_visible_digits"] + 'f'
  if gum.config.asBool["influenceDiagram", "utility_show_loss"]:
    titut = f'mEL {-meu["mean"]:{fmt}}'
  else:
    titut = f'MEU {meu["mean"]:{fmt}}'
  if gum.config.asBool["influenceDiagram", "utility_show_stdev"]:
    titut += f' (stdev={math.sqrt(meu["variance"]):{fmt}})'

  slabel = f'label="{titut}'

  if gum.config.asBool["notebook", "show_inference_time"]:
    slabel += f"\nInference in {1000 * (stopTime - startTime):6.2f}ms"
  dotstr += slabel + "\";\n"

  for nid in diag.nodes():
    name = diag.variable(nid).name()

    # defaults
    if diag.isChanceNode(nid):
      bgcolor = gum.config["influenceDiagram", "default_chance_bgcolor"]
      fgcolor = gum.config["influenceDiagram", "default_chance_fgcolor"]
      shape = gum.config["influenceDiagram", "chance_shape"]
    elif diag.isDecisionNode(nid):
      bgcolor = gum.config["influenceDiagram", "default_decision_bgcolor"]
      fgcolor = gum.config["influenceDiagram", "default_decision_fgcolor"]
      shape = gum.config["influenceDiagram", "decision_shape"]
    else:  # diag.isUtilityNode(nid):
      bgcolor = gum.config["influenceDiagram", "default_utility_bgcolor"]
      fgcolor = gum.config["influenceDiagram", "default_utility_fgcolor"]
      shape = gum.config["influenceDiagram", "utility_shape"]

    # 'hard' colour for evidence (?)
    if name in evs or nid in evs:
      bgcolor = gum.config["notebook", "evidence_bgcolor"]
      fgcolor = gum.config["notebook", "evidence_fgcolor"]

    styleattribute = 'style=filled, height=0,margin=0.1'
    colorattribute = f'fillcolor="{bgcolor}", fontcolor="{fgcolor}", color="#000000"'

    if not diag.isUtilityNode(nid):
      if len(targets) == 0 or name in targets or nid in targets:
        # shape="rectangle"
        filename = temp_dir + \
                   hashlib.md5(name.encode()).hexdigest() + "." + \
                   gum.config["notebook", "graph_format"]
        saveFigProba(ie.posterior(name), filename, bgcolor=bgcolor, util=ie.posteriorUtility(nid), txtcolor=fgcolor)
        dotstr += f' "{name}" [shape=rectangle,image="{filename}",label="", {colorattribute}];\n'
      else:
        dotstr += f' "{name}" [{colorattribute},shape={shape},{styleattribute}]'
    else:  # utility node
      mv = ie.meanVar(name)

      if gum.config.asBool["influenceDiagram", "utility_show_loss"]:
        coef = -1
      else:
        coef = 1

      fmt = f'.{gum.config.asInt["influenceDiagram", "utility_visible_digits"]}f'
      labut = f'{name} : {coef * mv["mean"]:{fmt}}'
      if gum.config.asBool["influenceDiagram", "utility_show_stdev"]:
        labut += f' ({math.sqrt(mv["variance"]):{fmt}})'

      dotstr += f' "{name}" [label="{labut}",{colorattribute},{styleattribute},shape={shape}]'

  # arcs
  dotstr += "\n"
  for node in diag.nodes():
    for chi in diag.children(node):
      dotstr += '  "' + diag.variable(node).name() + '"->"' + \
                diag.variable(chi).name() + '"'
      if diag.isDecisionNode(chi):
        dotstr += f' [style="{gum.config["influenceDiagram", "decision_arc_style"]}"]'
      elif diag.isUtilityNode(chi):
        dotstr += f' [style="{gum.config["influenceDiagram", "utility_arc_style"]}"]'
      dotstr += ";\n"
  dotstr += "}"

  g = dot.graph_from_dot_data(dotstr)[0]

  # workaround for some badly parsed graph (pyparsing>=3.03)
  g.del_node('"\\n"')

  if size is None:
    size = gum.config["influenceDiagram", "default_id_inference_size"]
  g.set_size(size)
  g.temp_dir = temp_dir

  return g
