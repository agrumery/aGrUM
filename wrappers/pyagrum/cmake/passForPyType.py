############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
Swig creates very nice code but cannot induce python
types for wrapped function. This utility change many
different c++ oddities in generated pyagrum.py
"""

import os
import shutil
import sys
import re
from dataclasses import dataclass, field


def notif(s):
    print(f"** pyAgrum : {s}")


# Sentinel value used to request in-place transformation (overwrite source file).
_INPLACE = "inplace"


@dataclass
class _Rule:
    pattern_str: str  # original pattern string, used for reporting
    replacement: str
    type_only: (
        bool  # if True, only applied on lines that contain type annotation markers
    )
    regex: re.Pattern = field(repr=False)
    triggered: int = field(default=0, repr=False)


def _make_rule(pattern: str, replacement: str, type_only: bool = False) -> _Rule:
    regex = re.compile(
        rf'("?)(?:const\s*)?{pattern}(?:\s*const)?(?:\s*[*&])?(?:\s*const)?\1'
    )
    return _Rule(
        pattern_str=pattern, replacement=replacement, type_only=type_only, regex=regex
    )


# --- std::vector<T> member-type rules (generated to avoid repetition) ---

_VEC_ELEMENT_TYPES = ["double", "unsigned int", "std::string"]
_VEC_INT_SUFFIXES = ["size_type", "difference_type", "iterator", "reverse_iterator"]
_VEC_VALUE_TYPES = {"double": "float", "unsigned int": "int", "std::string": "str"}

_VEC_RULES: list[tuple] = (
    [
        (f"std::vector< {t} >::{s}", "int")
        for t in _VEC_ELEMENT_TYPES
        for s in _VEC_INT_SUFFIXES
    ]
    + [(f"std::vector< {t} >::allocator", "object") for t in _VEC_ELEMENT_TYPES]
    + [
        (f"std::vector< {t} >::value_type", _VEC_VALUE_TYPES[t])
        for t in _VEC_ELEMENT_TYPES
    ]
)

# --- Python rules: replace C++ types with Python equivalents ---
# Each entry is (cpp_pattern, python_type) or (cpp_pattern, python_type, type_only).
# type_only=True restricts the rule to lines containing type annotation markers (-> or def)
# to avoid replacing English words that happen to match a C++ type name.

_RULES_PYTHON_RAW: list[tuple] = [
    # not wrapped
    (
        "gum::NodeProperty< std::vector< std::vector< std::vector< double,std::allocator< double > >,std::allocator< std::vector< double,std::allocator< double > > > >,std::allocator< std::vector< std::vector< double,std::allocator< double > >,std::allocator< std::vector< double,std::allocator< double > > > > > > >",
        "pyagrum.YetUnWrapped",
    ),
    (
        "std::vector< std::vector< std::vector< double,std::allocator< double > >,std::allocator< std::vector< double,std::allocator< double > > > >,std::allocator< std::vector< std::vector< double,std::allocator< double > >,std::allocator< std::vector< double,std::allocator< double > > > > > >",
        "pyagrum.YetUnWrapped",
    ),
    (
        "std::vector< std::vector< double,std::allocator< double > >,std::allocator< std::vector< double,std::allocator< double > > > >",
        "pyagrum.YetUnWrapped",
    ),
    # (
    #  "std::vector< std::vector< gum::Idx,std::allocator< gum::Idx > >,std::allocator< std::vector< gum::Idx,std::allocator< gum::Idx > > > >",
    #  "pyagrum.YetUnWrapped"),
    ("gum::IApproximationSchemeConfiguration::ApproximationSchemeSTATE", "int"),
    ("gum::IApproximationSchemeConfiguration", "pyagrum.YetUnWrapped"),
    # ("gum::learning::DatabaseTable", "pyagrum.YetUnWrapped"),
    (
        "std::vector< gum::NodeSet,std::allocator< gum::NodeSet > >",
        "pyagrum.YetUnWrapped",
    ),
    # complicated std types (generated from _VEC_RULES)
    *_VEC_RULES,
    # containers
    (
        "std::optional< std::vector< gum::NodeId,std::allocator< gum::NodeId > > >",
        "list[int] | None",
    ),
    ("std::pair< gum::Instantiation,double >", 'tuple["pyagrum.Instantiation",float]'),
    ("std::unique_ptr< gum::DiscreteVariable >", "pyagrum.DiscreteVariable"),
    # ("std::vector< gum::Idx,std::allocator< gum::Idx > >", 'list[int]'),
    ("std::vector< gum::NodeId,std::allocator< gum::NodeId > >", "list[int]"),
    (
        "std::vector< std::pair< gum::Idx,gum::Idx >,std::allocator< std::pair< gum::Idx,gum::Idx > > >",
        "tuple[tuple[int, int], ...]",
    ),
    ("std::vector< double,std::allocator< double > >", "tuple[float, ...]"),
    ("std::vector< int,std::allocator< int > >", "tuple[int, ...]"),
    ("std::vector< unsigned int,std::allocator< unsigned int > >", "tuple[int, ...]"),
    ("std::vector< std::string,std::allocator< std::string > >", "tuple[str, ...]"),
    ("std::vector< int >::size_type", "int"),
    ("std::vector< int >::difference_type", "int"),
    ("std::vector< int >::value_type const &", "int"),
    ("std::vector< int >::value_type", "int"),
    (
        "std::vector< PythonLoadlistener,std::allocator< PythonLoadlistener > >",
        'list["pyagrum.PythonLoadlistener"]',
    ),
    ("HashTable< gum::NodeId,gum::NodeSet >", "dict[int, list[int]]"),
    ("HashTable< std::string,std::string >", "dict[str, str]"),
    (
        "HashTable< gum::Counterfactual< double >::VarName,gum::Counterfactual< double >::ValName > const &",
        "dict[str,str]",
    ),
    ("gum::Set< std::string >", "set[str]"),
    ("std::vector< gum::Arc,std::allocator< gum::Arc > >", "list[tuple[int,int]]"),
    (
        "std::tuple< gum::CausalImpact< double >,gum::Tensor< double >,std::string >",
        'tuple["pyagrum.CausalImpact","pyagrum.Tensor",str]',
    ),
    ("gum::NodeProperty< gum::NodeId >", "dict[int,int]"),
    ("std::optional< gum::NodeSet >", "list[int] | None"),
    ("gum::NodeSet", "list[int]"),
    ("gum::Set< gum::Instantiation >", "list[dict[str,int]]"),
    ("gum::Sequence< gum::DiscreteVariable const \\* >", "list[object]"),
    ("gum::Sequence< int >", "list[int]"),
    ("gum::Sequence< gum::NodeId >", "list[int]"),
    ("gum::NodeGraphPart", "set[int]"),
    ("gum::EdgeSet", "set[tuple[int,int]]"),
    ("gum::ArcSet", "set[tuple[int,int]]"),
    # removing templates and correct namespace for pyAgrum's classes
    ("gum::Tensor< double >", "pyagrum.Tensor"),
    ("gum::BayesNet< double >", "pyagrum.BayesNet"),
    # ("gum::MarkovNet< double >", "pyagrum.MarkovRandomField"),
    ("gum::CausalModel< double >", "pyagrum.CausalModel"),
    ("gum::CausalFormula< double >", "pyagrum.CausalModel"),
    ("gum::MarkovRandomField< double >", "pyagrum.MarkovRandomField"),
    ("gum::InfluenceDiagram< double >", "pyagrum.InfluenceDiagram"),
    ("gum::DiscreteVariable", "pyagrum.DiscreteVariable"),
    ("gum::Instantiation", "pyagrum.Instantiation"),
    ("gum::JunctionTree", "pyagrum.JunctionTree"),
    ("gum::RangeVariable", "pyagrum.RangeVariable"),
    ("gum::IntegerVariable", "pyagrum.IntegerVariable"),
    ("gum::NumericalDiscreteVariable", "pyagrum.NumericalDiscreteVariable"),
    ("gum::LabelizedVariable", "pyagrum.LabelizedVariable"),
    ("gum::DiscretizedVariable< double >", "pyagrum.DiscretizedVariable"),
    ("gum::credal::CredalNet< double >::NodeType", "int"),
    ("gum::credal::CredalNet< double >", "pyagrum.CredalNet"),
    ("gum::credal::CNLoopyPropagation< double >::InferenceType", "int"),
    ("gum::credal::CNLoopyPropagation< double >", "pyagrum.CNLoopyPropagation"),
    ("gum::UndiGraph", "pyagrum.UndiGraph"),
    ("gum::MixedGraph", "pyagrum.MixedGraph"),
    ("gum::PDAG", "pyagrum.PDAG"),
    ("gum::JoinTree", "pyagrum.CliqueGraph"),
    ("gum::IBayesNet< double >", "pyagrum.IBayesNet"),
    ("gum::IMarkovRandomField< double >", "pyagrum.IMarkovRandomField"),
    ("gum::VariableNodeMap", "pyagrum.VariableNodeMap"),
    ("gum::FactorTable< double >", "list[set[int]]"),
    ("gum::MultiDimContainer< double >", "pyagrum.Tensor"),
    ("gum::MultiDimAdressable", "pyagrum.Tensor"),
    ("gum::MultiDimImplementation< double >", "pyagrum.Tensor"),
    ("gum::VariableSet", "pyagrum.VariableSet"),
    ("gum::Variable", "pyagrum.Variable"),
    ("gum::IDiscretizedVariable", "pyagrum.DiscretizedVariable"),
    ("gum::DAG", "pyagrum.DAG"),
    ("gum::learning::BNLearner< double >", "pyagrum.BNLearner"),
    # enum
    ("gum::RelevantTensorsFinderType", "int"),
    ("gum::FindBarrenNodesType", "int"),
    # weird shortcuts from time to time
    ('"Arc"', "pyagrum.Arc"),
    ('"Edge"', "pyagrum.Edge"),
    ('"UndiGraph"', "pyagrum.UndiGraph"),
    ('"DiscreteVariable"', "pyagrum.DiscreteVariable"),
    ('"DAG"', "pyagrum.DAG"),
    ('"Instantiation"', "pyagrum.Instantiation"),
    ('"DiscretizedVariable"', "pyagrum.DiscretizedVariable"),
    ('"Variable"', "pyagrum.Variable"),
    ('"BayesNet"', "pyagrum.BayesNet"),
    ('"VariableNodeMap"', "pyagrum.VariableNodeMap"),
    ('"UGmodel"', "pyagrum.UGmodel"),
    ('"MultiDimContainer"', "pyagrum.Tensor"),
    ('"Triangulation"', "pyagrum.Triangulation"),
    ('"DAGmodel"', "pyagrum.DAGmodel"),
    # type simplifications
    ("gum::VarType", "int"),
    ("gum::NodeId", "int"),
    ("gum::Idx", "int"),
    ("gum::Size", "int"),
    ("PyObject", "object"),
    ("void const", "object"),
    ("void", "None"),
    ("std::string_view", "str"),
    ("std::string", "str"),
    ("char const *", "str"),
    ("std::size_t", "int"),
    ("object_type", "object"),
    # remove type decorators (const, &, etc.) ... to be treated as complete word : \b
    # type_only=True: avoid consuming RST emphasis closing * in docstring text (e.g. (*bool*))
    ("\\bbool\\b", "bool", True),
    ("\\bsize_t\\b", "int", True),
    ("\\bint\\b", "int", True),
    ("\\bunsigned int\\b", "int", True),
    # type_only=True: only apply on lines with type annotation markers (-> or def)
    # to avoid replacing English words ("as long as", "double the size", etc.)
    ("\\blong\\b", "int", True),
    ("\\bdouble\\b", "float", True),
    # keep correct comment with template
    # ("< float >", "< double >"),
    ("gum::BayesNet", "pyagrum.BayesNet"),
    ("gum::Tensor", "pyagrum.Tensor"),
    # ("gum::MarkovNet", 'pyagrum.MarkovRandomField'),
    ("gum::DiGraph", "pyagrum.DiGraph"),
    ("gum::", "pyagrum."),
    ("gum\\.", "pyagrum."),
]

# --- C++ rules: fix up the raw SWIG C++ wrapper file ---

_RULES_CPP: list[tuple] = [
    ("<double>@", "@"),
    (" HashTable", " gum::HashTable"),
    (r"\(HashTable", r"(gum::HashTable"),
    (r"\(CausalFormula", r"(gum::CausalFormula"),
    (r" CausalFormula", r" gum::CausalFormula"),
    (r" Bijection", r" gum::Bijection"),
]


def _build_rules(raw_rules: list[tuple], is_python: bool) -> list[_Rule]:
    """Compile raw rule tuples into _Rule objects, applying Python-mode quoting if needed."""
    rules = []
    for raw in raw_rules:
        pattern, replacement = raw[0], raw[1]
        type_only = raw[2] if len(raw) > 2 else False
        if (
            is_python
            and replacement.startswith("pyagrum.")
            and replacement != "pyagrum."
        ):
            replacement = f'"{replacement}"'
        rules.append(_make_rule(pattern, replacement, type_only))
    return rules


def process_filters(
    src_filename: str, is_python: bool, debug_mode: bool
) -> tuple[str, list[_Rule]]:
    """Process src_filename through type-mapping rules.

    Returns the transformed content as a string and the list of rules (with
    triggered counts), so the caller can decide whether to print the stats.
    """
    rules = _build_rules(_RULES_PYTHON_RAW if is_python else _RULES_CPP, is_python)

    commentstr = "# ##" if is_python else "// ## "

    # Lines without type annotation markers skip type_only rules, preventing replacement
    # of English words that match a C++ primitive type name (e.g. "as long as").
    _type_annotation_line = re.compile(r"(?:->|\bdef\s+\w)")

    typing_added = False
    output_lines = []
    with open(src_filename, "r") as src:
        for line in src.read().splitlines():
            originalline = line
            has_annotation = bool(_type_annotation_line.search(line))
            for rule in rules:
                if rule.type_only and not has_annotation:
                    continue
                if rule.regex.search(line):
                    line = rule.regex.sub(rule.replacement, line)
                    rule.triggered += 1
            if debug_mode and line != originalline:
                output_lines.append(commentstr + originalline.strip())
            if is_python and not typing_added:
                if (
                    line.strip() == "## added by passForType (pyAgrum)"
                ):  # already added annotation module
                    typing_added = True
                elif (
                    line.strip() == "# Import the low-level C/C++ module"
                ):  # add annotation module
                    output_lines.append(
                        commentstr + " recursive import for typehints annotation"
                    )
                    output_lines.append("import pyagrum")
                    output_lines.append(
                        commentstr + " end of added by passForType (pyAgrum)\n"
                    )
                    typing_added = True
            output_lines.append(line)

    return "\n".join(output_lines) + "\n", rules


def _print_stats(rules: list[_Rule], is_python: bool) -> None:
    notif("Python mode" if is_python else "C++ mode")
    total = 0
    notif("-" * 85)
    for i, rule in enumerate(rules, 1):
        total += rule.triggered
        pat = (
            rule.pattern_str[:42] + "..."
            if len(rule.pattern_str) > 45
            else rule.pattern_str
        )
        res = (
            "..." + rule.replacement[-22:]
            if len(rule.replacement) > 25
            else rule.replacement
        )
        notif(f"R{i:<4}|{pat:>45} -> {res:25}|{rule.triggered}")
    notif("-" * 85)
    notif(f"     | total : {total}")


def do_the_job(
    src_filename: str,
    target_filename: str,
    backup_filename: str,
    is_python: bool,
    debug_mode: bool,
):
    if target_filename == _INPLACE:
        notif(f"Pythonizing types into {src_filename} (inplace)")
    else:
        notif(f"Pythonizing types from {src_filename} to {target_filename}")

    if debug_mode:
        notif("  - debug mode")

    if not os.path.exists(src_filename):
        raise IOError(f"File '{src_filename}' not found.")

    t_filename = src_filename if target_filename == _INPLACE else target_filename

    if not debug_mode:
        notif(f"  - backup {src_filename} in {backup_filename}.")
        if os.path.exists(src_filename):
            shutil.copy(src_filename, backup_filename)

    new_content, rules = process_filters(src_filename, is_python, debug_mode)

    if os.path.exists(t_filename):
        with open(t_filename, "r") as f:
            if f.read() == new_content:
                notif(f"  - {t_filename} unchanged (skipped).")
                return

    _print_stats(rules, is_python)

    with open(t_filename, "w") as f:
        f.write(new_content)

    notif(f"  - {t_filename} updated.")


def main(*args):
    if not args:
        notif("passForPyType src [dest]")
        return
    name, suf = os.path.splitext(args[0])
    is_python = suf == ".py"

    if len(args) == 2:  # normal mode
        namedest, sufdest = os.path.splitext(args[1])
        do_the_job(
            src_filename=args[0],
            target_filename=args[1],
            backup_filename=f"{namedest}{sufdest}.backup",
            is_python=is_python,
            debug_mode=False,
        )
    elif len(args) == 1:  # debug mode
        do_the_job(
            src_filename=args[0],
            target_filename=f"{name}_debug{suf}",
            backup_filename="NO MOVE",
            is_python=is_python,
            debug_mode=True,
        )
    else:
        notif("passForPyType src [dest]")


if __name__ == "__main__":
    if len(sys.argv) == 1:
        PATH = (
            os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
            + "/generated-files"
        )
        main(f"{PATH}/rawPyAgrum.py", f"{PATH}/pyagrum.py")
        main(f"{PATH}/pyAgrumPYTHON_wrap.cxx", _INPLACE)
    else:
        main(*sys.argv[1:])
