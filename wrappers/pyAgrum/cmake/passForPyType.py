"""
Swig creates very nice code but cannot induce python
types for wrapped function. This utility change many
different c++ oddities in generated pyAgrum.py
"""

# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
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
import shutil
import sys
import re


def notif(s):
  print(f"** pyAgrum : {s}")


def process_filters(src_filename: str, target_filename: str, debugmode: bool):
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
    #(
    #  "std::vector< std::vector< gum::Idx,std::allocator< gum::Idx > >,std::allocator< std::vector< gum::Idx,std::allocator< gum::Idx > > > >",
    #  '"pyAgrum.YetUnWrapped"'),
    ("gum::IApproximationSchemeConfiguration::ApproximationSchemeSTATE", "int"),
    ("gum::IApproximationSchemeConfiguration", '"pyAgrum.YetUnWrapped"'),
    #("gum::learning::DatabaseTable", '"pyAgrum.YetUnWrapped"'),
    ("std::vector< gum::NodeSet,std::allocator< gum::NodeSet > >", '"pyAgrum.YetUnWrapped"'),

    # complicated std types
    ("std::vector< double >::size_type", "int"),
    ("std::vector< unsigned int >::size_type", "int"),
    ("std::vector< std::string >::size_type", "int"),

    ("std::vector< double >::difference_type", "int"),
    ("std::vector< unsigned int >::difference_type", "int"),
    ("std::vector< std::string >::difference_type", "int"),

    ("std::vector< double >::iterator", "int"),
    ("std::vector< unsigned int >::iterator", "int"),
    ("std::vector< std::string >::iterator", "int"),

    ("std::vector< double >::reverse_iterator", "int"),
    ("std::vector< unsigned int >::reverse_iterator", "int"),
    ("std::vector< std::string >::reverse_iterator", "int"),

    ("std::vector< double >::allocator", "object"),
    ("std::vector< unsigned int >::allocator", "object"),
    ("std::vector< std::string >::allocator", "object"),

    ("std::vector< double >::value_type", "float"),
    ("std::vector< unsigned int >::value_type", "int"),
    ("std::vector< std::string >::value_type", "str"),

    # containers
    ('std::pair< gum::Instantiation,double >', 'Tuple["pyAgrum.Instantiation",float]'),
    ('std::unique_ptr< gum::DiscreteVariable >', '"pyAgrum.DiscreteVariable"'),
    #("std::vector< gum::Idx,std::allocator< gum::Idx > >", 'List[int]'),
    ("std::vector< gum::NodeId,std::allocator< gum::NodeId > >", 'List[int]'),
    ("std::vector< double,std::allocator< double > >", "List[float]"),
    ("std::vector< int,std::allocator< int > >", "List[int]"),
    ("std::vector< unsigned int,std::allocator< unsigned int > >", "List[int]"),
    ("std::vector< std::string,std::allocator< std::string > >", "List[str]"),
    ("\"Vector_string\"", "List[str]"),
    ("std::vector< PythonLoadListener,std::allocator< PythonLoadListener > >", 'List["pyAgrum.PythonLoadListener"]'),
    #("std::vector< gum::Arc,std::allocator< gum::Arc > >", 'List[Tuple[int,int]]'),
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
    #("gum::MarkovNet< double >", '"pyAgrum.MarkovRandomField"'),
    ("gum::MarkovRandomField< double >", '"pyAgrum.MarkovRandomField"'),
    ("gum::InfluenceDiagram< double >", '"pyAgrum.InfluenceDiagram"'),
    ("gum::DiscreteVariable", '"pyAgrum.DiscreteVariable"'),
    ("gum::Instantiation", '"pyAgrum.Instantiation"'),
    ("gum::JunctionTree", '"pyAgrum.JunctionTree"'),
    ("gum::RangeVariable", '"pyAgrum.RangeVariable"'),
    ("gum::IntegerVariable", '"pyAgrum.IntegerVariable"'),
    ("gum::NumericalDiscreteVariable", '"pyAgrum.NumericalDiscreteVariable"'),
    ("gum::LabelizedVariable", '"pyAgrum.LabelizedVariable"'),
    ("gum::DiscretizedVariable< double >", '"pyAgrum.DiscretizedVariable"'),
    ("gum::credal::CredalNet< double >::NodeType", 'int'),
    ("gum::credal::CredalNet< double >", '"pyAgrum.CredalNet"'),
    ("gum::credal::CNLoopyPropagation< double >::InferenceType", 'int'),
    ("gum::credal::CNLoopyPropagation< double >", '"pyAgrum.CNLoopyPropagation"'),
    #("gum::DAG", '"pyAgrum.DAG"'),
    ("gum::UndiGraph", '"pyAgrum.UndiGraph"'),
    ("gum::MixedGraph", '"pyAgrum.MixedGraph"'),
    ("gum::PDAG", '"pyAgrum.PDAG"'),
    ("gum::JoinTree", '"pyAgrum.CliqueGraph"'),
    ("gum::IBayesNet< double >", '"pyAgrum.IBayesNet"'),
    ("gum::IMarkovRandomField< double >", '"pyAgrum.IMarkovRandomField"'),
    ('gum::VariableNodeMap', '"pyAgrum.VariableNodeMap"'),
    ('gum::FactorTable< double >', 'List[Set[int]]'),
    ('gum::MultiDimContainer< double >', '"pyAgrum.Potential"'),
    ('gum::MultiDimAdressable', '"pyAgrum.Potential"'),
    ('gum::MultiDimImplementation< double >', '"pyAgrum.Potential"'),
    ('gum::Variable', '"pyAgrum.Variable"'),
    ('gum::IDiscretizedVariable', '"pyAgrum.DiscretizedVariable"'),
    ('gum::DAG', '"pyAgrum.DAG"'),
    ('gum::learning::BNLearner< double >', '"pyAgrum.BNLearner"'),

    # enum
    ('gum::RelevantPotentialsFinderType', "int"),
    ('gum::FindBarrenNodesType', "int"),

    # weird shortcuts from time to time
    ('"Arc"', '"pyAgrum.Arc"'),
    ('"Edge"', '"pyAgrum.Edge"'),
    ('"UndiGraph"', '"pyAgrum.UndiGraph"'),
    #('"DiscreteVariable"', '"pyAgrum.DiscreteVariable"'),
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
    ("char const *", "str"),
    ("std::size_t", "int"),
    ("object_type", "object"),

    # remove type decorators (const, &, etc.) ... to be treated as complete word : \b
    ("\\bbool\\b", "bool"),
    ("\\bsize_t\\b", "int"),
    ("\\bint\\b", "int"),
    ("\\bunsigned int\\b", "int"),
    ("\\blong\\b", "int"),
    ("\\bdouble\\b", "float"),

    # keep correct comment with template
    #("< float >", "< double >"),
    ("gum::BayesNet", 'pyAgrum.BayesNet'),
    ("gum::Potential", 'pyAgrum.Potential'),
    #("gum::MarkovNet", 'pyAgrum.MarkovRandomField'),
    ( "gum::DiGraph",'"pyAgrum.DiGraph"'),
    ( "gum::","pyAgrum."),
    ( "gum\\.","pyAgrum."),
  ]

  rules = {f"R{i + 1}": cpl for i, cpl in enumerate(list_rules)}

  compiled = {k: re.compile(fr'("?)(?:const\s*)?{rules[k][0]}(?:\s*const)?(?:\s*[*|&])?(?:\s*const)?\1') for k in
              rules}
  triggered = {k: 0 for k in rules}

  typing_added = False
  with open(src_filename, "r") as src:
    with open(target_filename, "w") as dst:
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
          if line.strip() == '## added by passForType (pyAgrum)':  # we already added annotation module
            typing_added = True
          elif line.strip() == '# Import the low-level C/C++ module':  # we add annotation module
            print("## added by passForType (pyAgrum)", file=dst)
            print("from typing import List,Set,Dict,Tuple", file=dst)
            print("# recursive import for typehints annotation", file=dst)
            print("import pyAgrum", file=dst)
            print("## end of added by passForType (pyAgrum)\n", file=dst)
            typing_added = True
        print(line, file=dst)

  total = 0
  for k in triggered:
    total += triggered[k]
    rule = rules[k][0]
    if len(rule) > 45:
      rule = rule[0:42] + "..."
    res = rules[k][1]
    if len(res) > 25:
      res = "..." + res[-22:]
    notif(f"{k:5}|{rule:>45} -> {res:25}|{triggered[k]}")
  notif("-" * 85)
  notif(f"     | total : {total}")


def do_the_job(src_filename: str, target_filename: str, backup_filename: str, debugmode: bool):
  notif(f"Pythonizing types from {src_filename} to {target_filename}")
  if debugmode:
    notif("  - debug mode")

  if not os.path.exists(src_filename):
    raise IOError(f"File '{src_filename}' not found.")

  notif(f"  - backup in {backup_filename}.")
  if os.path.exists(target_filename):
    if os.path.exists(backup_filename):
      os.remove(backup_filename)  # file exits, delete it
    shutil.copy(target_filename, backup_filename)

  process_filters(src_filename, target_filename, debugmode)

  notif(f"  - {target_filename} updated.")


if len(sys.argv) == 3:  # normal mode
  do_the_job(src_filename=sys.argv[1],
             target_filename=sys.argv[2],
             backup_filename=sys.argv[2] + "old.py",
             debugmode=False)
elif len(sys.argv) == 2:  # normal mode
  do_the_job(src_filename=sys.argv[1],
             target_filename='debug_' + sys.argv[1],
             backup_filename="NO MOVE",
             debugmode=True)
else:
  notif(f"{sys.argv[0]} src [dest]")
