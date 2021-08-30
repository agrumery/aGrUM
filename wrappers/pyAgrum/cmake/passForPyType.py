"""
Swig creates very nice code but cannot induce python
types for wrapped function. This utility change many
different c++ oddities in generated pyAgrum.py
"""

# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2017
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
import os
import sys
import re


def notif(s):
  print(f"** pyAgrum : {s}")


debugmode = False
if len(sys.argv) == 2:  # normal mode
  target = sys.argv[1]
  resultat = target + "new.py"
  backup = target + "old.py"
else:
  resultat = sys.argv[1]
  target = resultat + "old.py"
  backup = "NO MOVE"
  debugmode = True

list_rules = [  # ("T","N",stop) : replace ["] [const] T [*|&] ["] by N (N is a type in python)
# not wrapped
  (
    "gum::NodeProperty< std::vector< std::vector< std::vector< double,std::allocator< double > >,std::allocator< std::vector< double,std::allocator< double > > > >,std::allocator< std::vector< std::vector< double,std::allocator< double > >,std::allocator< std::vector< double,std::allocator< double > > > > > > >",
    '"pyAgrum.YetUnWrapped"'),
  (
    "std::vector< std::vector< std::vector< double,std::allocator< double > >,std::allocator< std::vector< double,std::allocator< double > > > >,std::allocator< std::vector< std::vector< double,std::allocator< double > >,std::allocator< std::vector< double,std::allocator< double > > > > > >",
    '"pyAgrum.YetUnWrapped"'),
  (
  "std::vector< std::vector< double,std::allocator< double > >,std::allocator< std::vector< double,std::allocator< double > > > >",
  '"pyAgrum.YetUnWrapped"'),
  (
  "std::vector< std::vector< gum::Idx,std::allocator< gum::Idx > >,std::allocator< std::vector< gum::Idx,std::allocator< gum::Idx > > > >",
  '"pyAgrum.YetUnWrapped"'),
  ("gum::IApproximationSchemeConfiguration::ApproximationSchemeSTATE", "int"),
  ("gum::IApproximationSchemeConfiguration", '"pyAgrum.YetUnWrapped"'),
  ("gum::learning::DatabaseTable< >",'"pyAgrum.YetUnWrapped"'),
  ("std::vector< gum::NodeId,std::allocator< gum::NodeId > >", '"pyAgrum.YetUnWrapped"'),
  ("std::vector< gum::Idx,std::allocator< gum::Idx > >", '"pyAgrum.YetUnWrapped"'),
  ("std::vector< gum::NodeSet,std::allocator< gum::NodeSet > >", '"pyAgrum.YetUnWrapped"'),

# complicated std types
  ("std::vector< double >::size_type", "int"),
  ("std::vector< unsigned int >::size_type", "int"),
  ("std::vector< double >::difference_type", "int"),
  ("std::vector< unsigned int >::difference_type", "int"),
  ("std::vector< double >::value_type", "float"),
  ("std::vector< unsigned int >::value_type", "int"),
  ("std::vector< std::string >::value_type", "str"),

# containers
  ("std::vector< double,std::allocator< double > >", "List[float]"),
  ("std::vector< int,std::allocator< int > >", "List[int]"),
  ("std::vector< unsigned int,std::allocator< unsigned int > >", "List[int]"),
  ("std::vector< std::string,std::allocator< std::string > >", "List[str]"),
  ("std::vector< PythonLoadListener,std::allocator< PythonLoadListener > >", 'List["pyAgrum.PythonLoadListener"]'),
  ("std::vector< gum::Arc,std::allocator< gum::Arc > >", 'List[Tuple[int,int]]'),
  ("gum::NodeProperty< gum::NodeId >", "Dict[int,int]"),
  ("gum::NodeSet", "List[int]"),
  ("gum::Set< gum::Instantiation >", "List[Dict[str,int]]"),
  ("gum::Sequence< gum::DiscreteVariable const \\* >", "List[object]"),
  ("gum::Sequence< int >", "List[int]"),
  ("gum::Sequence< gum::NodeId >", "List[int]"),
  ("gum::NodeGraphPart", "Set[int]"),
  ("gum::EdgeSet", "Set[Tuple[int,int]]"),
  ("gum::ArcSet", "Set[Tuple[int,int]]"),

# removing templates and correct namespace for pyAgrum's classes
  ("gum::Potential< double >", '"pyAgrum.Potential"'),
  ("gum::BayesNet< double >", '"pyAgrum.BayesNet"'),
  ("gum::MarkovNet< double >", '"pyAgrum.MarkovNet"'),
  ("gum::InfluenceDiagram< double >", '"pyAgrum.InfluenceDiagram"'),
  ("gum::DiscreteVariable", '"pyAgrum.DiscreteVariable"'),
  ("gum::Instantiation", '"pyAgrum.Instantiation"'),
  ("gum::JunctionTree", '"pyAgrum.JunctionTree"'),
  ("gum::RangeVariable", '"pyAgrum.RangeVariable"'),
  ("gum::IntegerVariable", '"pyAgrum.IntegerVariable"'),
  ("gum::LabelizedVariable", '"pyAgrum.LabelizedVariable"'),
  ("gum::DiscretizedVariable< double >", '"pyAgrum.DiscretizedVariable"'),
  ("gum::credal::CredalNet< double >::NodeType", 'int'),
  ("gum::credal::CredalNet< double >", '"pyAgrum.CredalNet"'),
  ("gum::credal::CNLoopyPropagation< double >::InferenceType", 'int'),
  ("gum::credal::CNLoopyPropagation< double >", '"pyAgrum.CNLoopyPropagation"'),
  ("gum::DAG", '"pyAgrum.DAG"'),
  ("gum::UndiGraph", '"pyAgrum.UndiGraph"'),
  ("gum::MixedGraph", '"pyAgrum.MixedGraph"'),
  ("gum::JoinTree", '"pyAgrum.CliqueGraph"'),
  ("gum::IBayesNet< double >", '"pyAgrum.IBayesNet"'),
  ("gum::IMarkovNet< double >", '"pyAgrum.IMarkovNet"'),
  ('gum::VariableNodeMap', '"pyAgrum.VariableNodeMap"'),
  ('gum::FactorTable< double >', 'List[Set[int]]'),
  ('gum::MultiDimContainer< double >', '"pyAgrum.Potential"'),
  ('gum::MultiDimAdressable', '"pyAgrum.Potential"'),
  ('gum::MultiDimImplementation< double >', '"pyAgrum.Potential"'),
  ('gum::Variable','"pyAgrum.Variable"'),
  ('gum::IDiscretizedVariable','"pyAgrum.DiscretizedVariable"'),
  ('gum::DAG','"pyAgrum.DAG"'),

  # enum
  ('gum::RelevantPotentialsFinderType',"int"),
  ('gum::FindBarrenNodesType',"int"),

# weird shortcuts from time to time
  ('"Arc"', '"pyAgrum.Arc"'),
  ('"Edge"', '"pyAgrum.Edge"'),
  ('"UndiGraph"', '"pyAgrum.UndiGraph"'),
  ('"DiscreteVariable"', '"pyAgrum.DiscreteVariable"'),
  ('"DiscretizedVariable"', '"pyAgrum.DiscretizedVariable"'),
  ('"Variable"', '"pyAgrum.Variable"'),
  ('"BayesNet"', '"pyAgrum.BayesNet"'),
  ('"VariableNodeMap"', '"pyAgrum.VariableNodeMap"'),
  ('"UGmodel"', '"pyAgrum.UGmodel"'),
  ('"MultiDimContainer"', '"pyAgrum.Potential"'),
  ('"Triangulation"', '"pyAgrum.Triangulation"'),
  ('"DAGmodel"', '"pyAgrum.DAGmodel"'),

# type simplifications
  ("gum::VarType", "int"),
  ("gum::NodeId", "int"),
  ("gum::Idx", "int"),
  ("gum::Size", "int"),
  ("PyObject", "object"),
  ("void const", "object"),
  ("void", "None"),
  ("std::string", "str"),
  ("std::size_t", "int"),

# remove type decorators (const, &, etc.) ... to be treated as complete word : \b
  ("\\bbool\\b", "bool"),
  ("\\bsize_t\\b", "int"),
  ("\\bint\\b", "int"),
  ("\\bunsigned int\\b", "int"),
  ("\\blong\\b", "int"),
  ("\\bdouble\\b", "float"),

# keep correct comment with template
  ("< float >", "< double >"),
  ("gum::BayesNet","pyAgrum.BayesNet"),
  ("gum::Potential","pyAgrum.Potential"),
  ("gum::MarkovNet","pyAgrum.MarkovNet"),
]

rules = {f"R{i + 1}": cpl for i, cpl in enumerate(list_rules)}

compiled = {k: re.compile(fr'("?)(?:const\s*)?{rules[k][0]}(?:\s*const)?(?:\s*[\*|&])?(?:\s*const)?\1') for k in rules}
triggered = {k: 0 for k in rules}

notif(f"Pythonizing types for {target} via {resultat}, backup in {backup}")
if debugmode:
  notif("  (debug mode)")

if not os.path.exists(target):
  raise IOError(f"File '{target}' not found.")

typing_added = False
with open(target, "r") as src:
  with open(resultat, "w") as dst:
    for noline, line in enumerate(src.read().splitlines()):
      originalline = line
      num = 1
      rule = f"R{num}"
      while rule in rules:
        if compiled[rule].search(line) is not None:
          line = compiled[rule].sub(rules[rule][1], line)
          triggered[rule] += 1
        num += 1
        rule = f"R{num}"
      if debugmode:
        if line != originalline:
          pass  # print("## "+originalline.strip(),file=dst)
      if not typing_added:
        if line.strip() == '# Import the low-level C/C++ module':  # we add annotation module
          print("## added by passForType (pyAgrum)", file=dst)
          print("from typing import List,Set,Dict,Tuple", file=dst)
          print("## end of added by passForType (pyAgrum)\n", file=dst)
          typing_added = True
      print(line, file=dst)

sum = 0
for k in triggered:
  sum += triggered[k]
  rule = rules[k][0]
  if len(rule) > 45:
    rule = rule[0:42] + "..."
  res = rules[k][1]
  if len(res) > 25:
    res = "..." + res[-22:]
  notif(f"{k:5}|{rule:>45} -> {res:25}|{triggered[k]}")
notif("-" * 85)
notif(f"     | total : {sum}")

if debugmode:
  notif("No move here (debug mode).")
else:
  notif(f"backup in {backup}.")
  if os.path.exists(backup):
    os.remove(backup) # file exits, delete it
  os.rename(target, backup)
  notif(f"{target} updated.")
  os.rename(resultat, target)
