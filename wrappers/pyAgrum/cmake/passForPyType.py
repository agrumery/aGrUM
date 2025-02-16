############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version.                              #
#    - the MIT license (MIT)                                               #
#    - or both in dual license, as here                                    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
Swig creates very nice code but cannot induce python
types for wrapped function. This utility change many
different c++ oddities in generated pyAgrum.py
"""

import os
import shutil
import sys
import re


def notif(s):
  print(f"** pyAgrum : {s}")


def process_filters(src_filename: str, target_filename: str, is_python: bool, debug_mode: bool):
  list_rules_python = [  # ("T","N",stop) : replace ["] [const] T [*|&] ["] by N (N is a type in python)
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
    # (
    #  "std::vector< std::vector< gum::Idx,std::allocator< gum::Idx > >,std::allocator< std::vector< gum::Idx,std::allocator< gum::Idx > > > >",
    #  '"pyAgrum.YetUnWrapped"'),
    ("gum::IApproximationSchemeConfiguration::ApproximationSchemeSTATE", "int"),
    ("gum::IApproximationSchemeConfiguration", '"pyAgrum.YetUnWrapped"'),
    # ("gum::learning::DatabaseTable", '"pyAgrum.YetUnWrapped"'),
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
    # ("std::vector< gum::Idx,std::allocator< gum::Idx > >", 'List[int]'),
    ("std::vector< gum::NodeId,std::allocator< gum::NodeId > >", 'List[int]'),
    ("std::vector< double,std::allocator< double > >", "List[float]"),
    ("std::vector< int,std::allocator< int > >", "List[int]"),
    ("std::vector< unsigned int,std::allocator< unsigned int > >", "List[int]"),
    ("std::vector< std::string,std::allocator< std::string > >", "List[str]"),
    ("\"Vector_string\"", "List[str]"),
    ("std::vector< PythonLoadListener,std::allocator< PythonLoadListener > >", 'List["pyAgrum.PythonLoadListener"]'),
    # ("std::vector< gum::Arc,std::allocator< gum::Arc > >", 'List[Tuple[int,int]]'),
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
    # ("gum::MarkovNet< double >", '"pyAgrum.MarkovRandomField"'),
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
    # ("gum::DAG", '"pyAgrum.DAG"'),
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
    ('gum::VariableSet', '"pyAgrum.VariableSet"'),
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
    # ('"DiscreteVariable"', '"pyAgrum.DiscreteVariable"'),
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
    # ("< float >", "< double >"),
    ("gum::BayesNet", 'pyAgrum.BayesNet'),
    ("gum::Potential", 'pyAgrum.Potential'),
    # ("gum::MarkovNet", 'pyAgrum.MarkovRandomField'),
    ("gum::DiGraph", '"pyAgrum.DiGraph"'),
    ("gum::", "pyAgrum."),
    ("gum\\.", "pyAgrum."),
  ]

  list_rules_cpp = [  # ("T","N",stop) : replace ["] [const] T [*|&] ["] by N (N is a type in python)
    ("<double>@", '@'),
  ]

  if is_python:
    notif('Python mode')
    list_rules = list_rules_python
  else:
    notif('C++ mode')
    list_rules = list_rules_cpp

  rules = {f"R{i + 1}": cpl for i, cpl in enumerate(list_rules)}

  compiled = {k: re.compile(fr'("?)(?:const\s*)?{rules[k][0]}(?:\s*const)?(?:\s*[*|&])?(?:\s*const)?\1')
              for k in rules}
  triggered = {k: 0 for k in rules}
  if is_python:
    commentstr = "# ##"
  else:
    commentstr = "// ## "

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
        if debug_mode and line != originalline:
          print(commentstr + originalline.strip(), file=dst)
        if is_python and not typing_added:
          if line.strip() == '## added by passForType (pyAgrum)':  # we already added annotation module
            typing_added = True
          elif line.strip() == '# Import the low-level C/C++ module':  # we add annotation module
            print(commentstr + " added by passForType (pyAgrum)", file=dst)
            print("from typing import List,Set,Dict,Tuple", file=dst)
            print(commentstr + " recursive import for typehints annotation", file=dst)
            print("import pyAgrum", file=dst)
            print(commentstr + " end of added by passForType (pyAgrum)\n", file=dst)
            typing_added = True
        print(line, file=dst)

  total = 0
  notif("-" * 85)
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


def do_the_job(src_filename: str, target_filename: str, backup_filename: str, is_python: bool, debug_mode: bool):
  if target_filename == "inplace":
    notif(f"Pythonizing types into {src_filename} (inplace)")
  else:
    notif(f"Pythonizing types from {src_filename} to {target_filename}")

  if debug_mode:
    notif("  - debug mode")

  if not os.path.exists(src_filename):
    raise IOError(f"File '{src_filename}' not found.")

  if target_filename == "inplace":
    t_filename = src_filename
    s_filename = target_filename + ".copy"
    shutil.copy(t_filename, s_filename)
  else:
    t_filename = target_filename
    s_filename = src_filename

  if not debug_mode:
    notif(f"  - backup in {backup_filename}.")
    if os.path.exists(t_filename):
      if os.path.exists(backup_filename):
        os.remove(backup_filename)  # file exits, delete it
      shutil.copy(t_filename, backup_filename)

  process_filters(s_filename, t_filename, is_python, debug_mode)

  if target_filename == "inplace":
    os.remove(s_filename)

  notif(f"  - {t_filename} updated.")


def main(*arg):
  arg = ['passForPyType'] + list(arg)
  if len(arg) <= 1:
    notif(f"{arg[0]} src [dest]")
  else:
    name, suf = os.path.splitext(arg[1])
    is_python = suf == '.py'

    if len(arg) == 3:  # normal mode
      do_the_job(src_filename=arg[1],
                 target_filename=arg[2],
                 backup_filename=f"{name}.old{suf}",
                 is_python=is_python,
                 debug_mode=False)
    elif len(arg) == 2:  # debug mode
      do_the_job(src_filename=arg[1],
                 target_filename=f"{name}_debug{suf}",
                 backup_filename="NO MOVE",
                 is_python=is_python,
                 debug_mode=True)
    else:
      notif(f"{arg[0]} src [dest]")


if __name__ == "__main__":
  if len(sys.argv) == 1:
    PATH = os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + "/generated-files"
    main(f"{PATH}/rawPyAgrum.py", f"{PATH}/pyAgrum.py")
    main(f"{PATH}/pyAgrumPYTHON_wrap.cxx", "inplace")
  else:
    main(*sys.argv[1:])
