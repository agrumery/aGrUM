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

import os
import re
import sys
from collections.abc import Iterable
import types
import inspect
from datetime import datetime
from subprocess import call, run as subprocess_run, PIPE

from .configuration import cfg
from .utils import *

from .check_dependencies import check_gum_dependencies
from .ActBuilder import ActBuilder

import pyagrum as gum

gumPath = os.path.join(os.path.dirname(sys.argv[0]), f"build/pyAgrum/{cfg.buildPath['Release']}/wrappers")


def _prefix(name: str) -> str:
  return " " * (4 * (name.count(".") - 1)) + "- "


class PyAgrumDocCoverage:
  def __init__(self, verbose: bool):
    self._verbose = verbose
    self.nb_class = 0
    self.nb_meth = 0
    self.nb_func = 0

    self.undoc_class = []
    self.undoc_func = []
    self.undoc_meth = []

    self.partial_doc_class = []
    self.partial_doc_func = []
    self.partial_doc_meth = []

    # preparecp the log file for missing documentations
    self.nodocfile = "nodoc.log"
    self.nodocfilebefore = "nodoc.before.log"
    if os.path.exists(self.nodocfile):
      if os.path.exists(self.nodocfilebefore):
        os.remove(self.nodocfilebefore)
      os.rename(self.nodocfile, self.nodocfilebefore)
    with open(self.nodocfile, "w") as f:
      # add the date to logfile
      f.write(datetime.now().strftime("%Y-%m-%d %H:%M:%S") + "\n")

  def lognodoc(self, msg: str):
    with open(self.nodocfile, "a") as f:
      f.write(msg + "\n")

  def notif_and_log(self, msg: str = "") -> None:
    notif(msg)
    self.lognodoc(msg)

  @staticmethod
  def _is_not_valid(msg: str) -> bool:
    # deprecated does not follow the rules of validity for documentation
    if msg.strip().startswith("Deprecated"):
      return False

    # msg les than 3 lines are not valid
    if msg.count("\n") < 3:
      return True

    if "'PyObject *'" in msg:
      return True

    return False

  def _check_function_oc(self, name: str, func: str):
    res = "check"
    is_meth = name.count(".") > 1

    skipped = {
      "gum.Potential",
    }
    if name in skipped:
      if self._verbose:
        notif(_prefix(name) + name + " : skipped")
      return

    if is_meth:
      self.nb_meth += 1
    else:
      self.nb_func += 1

    if not hasattr(func, "__doc__") or func.__doc__ is None:
      if is_meth:
        self.undoc_meth.append(name)
      else:
        self.undoc_func.append(name)
      res = "no doc"
    else:
      if self._is_not_valid(func.__doc__):
        if is_meth:
          self.partial_doc_meth.append(name)
        else:
          self.partial_doc_func.append(name)

        res = "partial"

    if res != "check":
      self.lognodoc(name + " : " + res)
    if self._verbose:
      notif(_prefix(name) + name + " : " + res)

  def _check_class_doc(self, name: str, clas: str):
    res = "check"
    self.nb_class += 1

    if not hasattr(clas, "__doc__") or clas.__doc__ is None:
      self.undoc_class.append(name)
      res = "no doc"
    else:
      if self._is_not_valid(clas.__doc__):
        self.partial_doc_class.append(name)
        res = "partial"

    if res != "check":
      self.lognodoc(name + " : " + res)
    if self._verbose:
      notif(_prefix(name) + name + " : " + res)

  @staticmethod
  def _ignored_class(clas: str) -> bool:
    if gumPath not in sys.path:
      sys.path.insert(0, gumPath)

    return clas in {cls.__name__ for cls in gum.GumException.__subclasses__()}

  def _traversal(self, entities: Iterable[str], container: str):
    if gumPath not in sys.path:
      sys.path.insert(0, gumPath)

    for entity in entities:
      if entity[0] != "_":
        complete_entity_name = container + "." + entity
        instance_entity = eval(complete_entity_name)
        if type(instance_entity) is types.FunctionType:
          self._check_function_oc(complete_entity_name, instance_entity)
        elif inspect.isclass(instance_entity):
          if not self._ignored_class(instance_entity.__name__):
            self._check_class_doc(complete_entity_name, instance_entity.__name__)
            self._traversal(dir(instance_entity), complete_entity_name)

  def check_missing_doc(self):
    if gumPath not in sys.path:
      sys.path.insert(0, gumPath)

    DELIM: str = "\n    + "

    self.nb_class = 0
    self.nb_meth = 0
    self.nb_func = 0

    self.undoc_class = []
    self.undoc_func = []
    self.undoc_meth = []

    self.partial_doc_class = []
    self.partial_doc_func = []
    self.partial_doc_meth = []

    self._traversal(dir(gum), "gum")

    pc = 1.0 - (len(self.undoc_class) + len(self.partial_doc_class)) / (1.0 * self.nb_class)
    pm = 1.0 - (len(self.undoc_meth) + len(self.partial_doc_meth)) / (1.0 * self.nb_meth)
    pf = 1.0 - (len(self.undoc_func) + len(self.partial_doc_func)) / (1.0 * self.nb_func)

    tc = self.nb_class - (len(self.undoc_class) + len(self.partial_doc_class))
    tm = self.nb_meth - (len(self.undoc_meth) + len(self.partial_doc_meth))
    tf = self.nb_func - (len(self.undoc_func) + len(self.partial_doc_func))

    self.lognodoc("\n\n" + "=" * 50)
    self.notif_and_log(f"Documentation in [[pyAgrum {gum.__version__}]]")

    self.notif_and_log(f"  Classes   : coverage={(pc * 100.0):6.2f}% [[({tc}/{self.nb_class})]]")
    if self._verbose:
      self.notif_and_log("---------")
      self.notif_and_log("  - nbr of classes : " + str(self.nb_class))
      self.notif_and_log("  - nbr of partially documented classes : " + str(len(self.partial_doc_class)))
      self.notif_and_log(DELIM.join([""] + self.partial_doc_class))
      self.notif_and_log()
      self.notif_and_log("  - nbr of undocumented classes : " + str(len(self.undoc_class)))
      self.notif_and_log(DELIM.join([""] + self.undoc_class))

    self.notif_and_log(f"  Methods   : coverage={(pm * 100.0):6.2f}% [[({tm}/{self.nb_meth})]]")
    if self._verbose:
      self.notif_and_log("---------")
      self.notif_and_log("  - nbr of methods: " + str(self.nb_meth))
      self.notif_and_log("  - nbr of partially documented methods : " + str(len(self.partial_doc_meth)))
      self.notif_and_log(DELIM.join([""] + self.partial_doc_meth))
      self.notif_and_log()
      self.notif_and_log("  - nbr of undocumented methods : " + str(len(self.undoc_meth)))
      self.notif_and_log(DELIM.join([""] + self.undoc_meth))

    self.notif_and_log(f"  Functions : coverage={(pf * 100.0):6.2f}% [[({tf}/{self.nb_func})]]")
    if self._verbose:
      self.notif_and_log("-----------")
      self.notif_and_log("  - nbr of functions: " + str(self.nb_func))
      self.notif_and_log("  - nbr of partially documented functions : " + str(len(self.partial_doc_func)))
      self.notif_and_log(DELIM.join([""] + self.partial_doc_func))
      self.notif_and_log()
      self.notif_and_log("  - nbr of undocumented functions : " + str(len(self.undoc_func)))
      self.notif_and_log(DELIM.join([""] + self.undoc_func))
    self.lognodoc("=" * 50)

    return (
      len(self.undoc_class)
      + len(self.partial_doc_class)
      + len(self.undoc_meth)
      + len(self.partial_doc_meth)
      + len(self.undoc_func)
      + len(self.partial_doc_func)
    )


def missing_docs(show_funct: bool = False):
  return PyAgrumDocCoverage(verbose=show_funct).check_missing_doc()


class ActBuilderGuideline(ActBuilder):
  def __init__(self, current: dict[str, str | bool]):
    super().__init__(current)

  def check_consistency(self):
    return True

  def build(self):
    self.run_start()

    guideline(
      self.current,
      self.current["verbose"],
      self.current["correction"],
      self.current["dry_run"],
      self.current["guideline_check"],
    )

    self.run_done()
    return True


_GUIDELINE_ALL_CHECKS: frozenset[str] = frozenset({"cpp", "python", "header", "coverage", "deps", "mypy"})


def _parse_checks(spec: str) -> frozenset[str]:
  """Parse a check specification like 'all-cpp' or 'cpp+mypy' into a set of check names."""
  known = _GUIDELINE_ALL_CHECKS | {"all"}
  tokens = re.split(r"([+\-])", spec.strip())
  result: set[str] = set()
  op = "+"
  for tok in tokens:
    if tok in ("+", "-"):
      op = tok
      continue
    if not tok:
      continue
    if tok not in known:
      raise ValueError(f"Unknown check '{tok}'. Valid: {', '.join(sorted(known))}")
    expanded = _GUIDELINE_ALL_CHECKS if tok == "all" else {tok}
    if op == "+":
      result |= expanded
    else:
      result -= expanded
  return frozenset(result)


def guideline(
  current: dict[str, str | bool],
  details: bool,
  correction: bool,
  dry_run: bool = False,
  checks: str | None = None,
) -> int:
  def _aff_errors(nb: int, typ: str) -> int:
    if nb > 0:
      # spaces to remove others possible characters
      error(f"{nb} {typ} error{'s' if nb > 1 else ''}{' ' * 40}")
    return nb

  active = _parse_checks(checks if checks is not None else "all")
  run_cpp = "cpp" in active
  run_python = "python" in active
  run_header = "header" in active
  run_coverage = "coverage" in active
  run_deps = "deps" in active
  run_mypy = "mypy" in active

  effective_correction = correction and not dry_run
  active_checks_label = checks if checks is not None else "all"
  notif(
    f"[[aGrUM {'detailed ' if details else ''}guideline "
    f"[{active_checks_label}]"
    f"{' (with correction)' if effective_correction else ''}"
    f"{' (dry-run)' if dry_run else ''}]]"
  )

  nbrError = 0

  if run_cpp:
    notif("  [[(1) ]]*.cpp[[ file for every ]]*.h[[ file]]")
    nbrError += _aff_errors(_check_cpp_file_exists(details, effective_correction), "missing cpp file")

  if run_header:
    notif("  [[(2) check for ]]LGPL+MIT[[ license]]")
    nbrError += _aff_errors(
      _check_LGPL_MIT_license_CPP(details, effective_correction),
      "missing LGPL+MIT cpp licence",
    )
    nbrError += _aff_errors(
      _check_LGPL_MIT_license_py(details, effective_correction),
      "missing LGPL+MIT python licence",
    )

  if run_coverage:
    notif("  [[(3) check for missing documentation in pyAgrum]]")
    nbrError += _aff_errors(_check_missing_docs(details), "missing documentation")

  if run_deps:
    notif("  [[(4) check for deps]]")
    nbrError += _aff_errors(
      check_gum_dependencies(
        graph=current["build_graph"],
        details=details,
        correction=effective_correction,
      ),
      "redundant dependency",
    )

  if run_cpp:
    notif("  [[(5) check for cpp format]]")
    nbrError += _aff_errors(_check_clang_format(details, effective_correction, dry_run), "format")

  if run_python:
    notif("  [[(6) check for py format]]")
    nbrError += _aff_errors(_check_ruff_format(details, effective_correction, dry_run), "format")

  if run_mypy:
    notif("  [[(7) check mypy type annotations in pyLibs]]")
    nbrError += _aff_errors(
      _check_mypy(details, effective_correction, dry_run),
      "mypy type (unrecorded in coverage)",
    )

  return nbrError


def _check_code_format(
  sources,
  tool_path,
  tool_name,
  check_cmd_fn,
  fix_cmd_fn,
  exceptions,
  details,
  correction,
  dry_run=False,
  fix_stderr_visible=False,
) -> int:
  nbrError = 0
  if tool_path is None:
    warn(f"No correct [[{tool_name}]] tool has been found.")
    return 0
  if dry_run:
    for src in sources:
      if any(s in src for s in exceptions):
        continue
      notif(f"  {check_cmd_fn(tool_path, src)}")
    return 0
  with open(os.devnull, "w") as blackhole:
    for src in sources:
      if any(s in src for s in exceptions):
        continue
      if (
        call(
          check_cmd_fn(tool_path, src),
          shell=True,
          stderr=blackhole,
          stdout=blackhole,
        )
        == 1
      ):
        nbrError += 1
        if correction:
          if fix_stderr_visible:
            call(fix_cmd_fn(tool_path, src), shell=True)
          else:
            call(
              fix_cmd_fn(tool_path, src),
              shell=True,
              stderr=blackhole,
              stdout=blackhole,
            )
          notif(f"[[{src.split('/')[-1]}]] : [[(✓)]]")
        else:
          notif(f"err [[{src}]]")
      else:
        notif_oneline(f"[[{src.split('/')[-1]}]] OK")
  return nbrError


def _check_ruff_format(details: bool, correction: bool, dry_run: bool = False) -> int:
  return _check_code_format(
    sources=srcPyIpynbAgrum(),
    tool_path=cfg.ruff,
    tool_name="ruff",
    check_cmd_fn=lambda t, s: f"{t} format --check {s}",
    fix_cmd_fn=lambda t, s: f"{t} format {s}",
    exceptions={
      "/apps/",
      "/notebooks-archives/",
      "/generated-files/",
      "Untitled*.ipynb",
      "wrappers/pyagrum/cmake",
    },
    details=details,
    correction=correction,
    dry_run=dry_run,
  )


def _check_clang_format(details: bool, correction: bool, dry_run: bool = False) -> int:
  return _check_code_format(
    sources=srcAgrum(),
    tool_path=cfg.clangformat,
    tool_name="clang-format",
    check_cmd_fn=lambda t, s: f"{t} {s} | cmp {s} -",
    fix_cmd_fn=lambda t, s: f"{t} -i {s}",
    exceptions={f"{os.sep}external{os.sep}", "Parser", "Scanner", "doctest"},
    details=details,
    correction=correction,
    dry_run=dry_run,
    fix_stderr_visible=True,
  )


def _LGPL_MIT_atTop_CPP(filename: str, details: bool, correction: bool) -> int:
  before = licence = code = ""

  state = "before"  # before->inComment->after
  in_error = False

  with open(filename, encoding="UTF8") as origin:
    while line := origin.readline():
      if line.strip() == "":
        if state == "before":
          if before != "":
            before += line
        elif state == "inComment":
          licence += line
        else:
          code += line
        continue

      if state == "before":
        if line.startswith("%feature") or line.startswith("/*!"):
          state = "after"
          code += line
        elif not line.startswith("/**"):
          if not in_error:
            in_error = True
            if details:
              notif(f"[[{filename}]] lines before the CPP license.")
          before += line
        else:
          licence += line
          state = "inComment"
      elif state == "inComment":
        licence += line
        if line.strip().endswith("*/"):
          state = "after"
      else:  # state == "after"
        code += line

    err = 0
    if licence.strip() != _template_cpp_license.strip():
      err = 1
      res = f"[[{filename:.<80}]] missing up-to-date LGPL+MIT license (1)"
      if correction:
        with open(filename, "w", encoding="UTF8") as dest:
          dest.write(before)
          dest.write(_template_cpp_license)
          dest.write(code)
        res = f"{res} [[(✓)]]"
      if details or correction:
        notif(res)

  return err


def _LGPL_MIT_atTop_py(filename: str, details: bool, correction: bool) -> int:
  before = licence = code = ""

  state = "before"  # before->inComment->after
  in_error = False
  with open(filename) as origin:
    while line := origin.readline():
      if line.strip() == "":
        if state == "before":
          if before != "":
            before += line
        elif state == "inComment":
          licence += line
        else:
          code += line
        continue

      if state == "before":
        if line.startswith("import"):
          state = "after"
          code += line
        elif line[0] != "#" or line.startswith("#!"):
          if not in_error:
            in_error = True
            if details:
              notif(f"[[{filename}]] lines before the license.")
          before += line
        else:
          licence += line
          state = "inComment"
      elif state == "inComment":
        if line[0] != "#":
          state = "after"
          code += line
        else:
          licence += line
      else:  # state == "after"
        code += line

  err = 0
  if not (licence.startswith(_template_py_license) and licence[len(_template_py_license) :].strip("\n") == ""):
    err = 1
    res = f"[[{filename:.<80}]] missing up-to-date LGPL+MIT license (2)"
    if correction:
      with open(filename, "w") as dest:
        dest.write(before)
        dest.write(_template_py_license)
        dest.write(code)
      res = f"{res} [[(✓)]]"
    notif(res)

  return err


def _LGPL_MIT_atTop_cmake(filename: str, details: bool, correction: bool) -> int:
  licence = code = ""

  state = "before"  # before->inComment->after
  with open(filename) as origin:
    while line := origin.readline():
      if line.strip() == "":
        if state == "before":
          pass
        elif state == "inComment":
          licence += line
        else:
          code += line
        continue

      if state == "before" and line[0] == "#":
        licence += line
        state = "inComment"
      elif state == "inComment":
        if line[0] != "#":
          state = "after"
          code += line
        else:
          licence += line
      else:  # state == "after"
        code += line

  err = 0
  if not (licence.startswith(_template_py_license) and licence[len(_template_py_license) :].strip("\n") == ""):
    err = 1
    res = f"[[{filename:.<80}]] missing up-to-date LGPL+MIT license (3"
    if correction:
      with open(filename, "w") as dest:
        dest.write(_template_py_license)
        dest.write(code)
      res = f"{res} [[(✓)]]"
    notif(res)

  return err


def _check_LGPL_MIT_license_CPP(details: bool, correction: bool) -> int:
  nbrError = 0

  exceptions = [
    f"{os.sep}mvsc{os.sep}",
    f"{os.sep}external{os.sep}",
    "Parser",
    "Scanner",
    "doctest",
  ]
  for gum_file in srcAgrum():
    if any(subs in gum_file for subs in exceptions):
      if details:
        notif(f"skip header test for [[{gum_file}]]")
      continue
    nbrError += _LGPL_MIT_atTop_CPP(gum_file, details, correction)
  for gum_file in srcGeneratorAgrum():
    if any(subs in gum_file for subs in exceptions):
      if details:
        notif(f"skip header test for [[{gum_file}]]")
      continue
    nbrError += _LGPL_MIT_atTop_CPP(gum_file, details, correction)

  return nbrError


def _check_LGPL_MIT_license_py(details: bool, correction: bool) -> int:
  nbrError = 0

  exceptions = []
  for pygum_file in srcPyAgrum():
    if details:
      notif_oneline(f"[[{pygum_file.split('/')[-1]}]]")
    if any(subs in pygum_file for subs in exceptions):
      continue
    nbrError += _LGPL_MIT_atTop_py(pygum_file, details, correction)

  for cmake_file in srcCmakeAgrum():
    if details:
      notif_oneline(f"[[{cmake_file.split('/')[-1]}]]")
    nbrError += _LGPL_MIT_atTop_cmake(cmake_file, details, correction)

  return nbrError


def _check_cpp_file_exists(details: bool, correction: bool) -> int:
  nbrError = 0

  exceptions = [
    f"{os.sep}mvsc{os.sep}",
    f"{os.sep}signal{os.sep}",
    f"{os.sep}external{os.sep}",
    f"multidim{os.sep}patterns{os.sep}",
    "agrum.h",
    "inline.h",
    "base.h",
    "bn.h",
    "cn.h",
    "id.h",
    "mrf.h",
    f"MN{os.sep}MarkovNet.h",
    f"MN{os.sep}inference{os.sep}ShaferShenoyMNInference.h",
  ]

  for header in recglob(f"src{os.sep}agrum", "*.h"):
    if any(subs in header for subs in exceptions):
      if details:
        notif(f"skip {header}")
      continue

    subs = header[:-1]
    if subs.endswith("_tpl."):
      continue
    if subs.endswith("_inl."):
      continue
    cppfile = subs + "cpp"
    if not os.path.isfile(cppfile):
      nbrError += 1
      if correction:
        with open(cppfile, "w") as dest:
          dest.write(_template_cpp_license)
          dest.write(f'\n#include "{header[4:]}"\n')
        error("No cpp file for [[" + header + "h]] : [[added]]")
      else:
        error("No cpp file for [[" + header + "h]]")

  return nbrError


def _check_mypy(details: bool, correction: bool, dry_run: bool = False) -> int:
  mypy_cmd = f"{cfg.python} -m mypy"

  if dry_run:
    notif(f"  {mypy_cmd} {cfg.pymodulesPath}")
    return 0

  probe = subprocess_run(
    f"{mypy_cmd} --version",
    shell=True,
    stdout=PIPE,
    stderr=PIPE,
  )
  if probe.returncode != 0:
    warn("[[mypy]] not installed in current Python environment. Skipping.")
    return 0

  result = subprocess_run(
    f"{mypy_cmd} {cfg.pymodulesPath}",
    shell=True,
    stdout=PIPE,
    stderr=PIPE,
    text=True,
  )

  lines = result.stdout.splitlines()
  nbrError = 0

  # Summary is on last line: "Found N errors in M files (...)"
  if lines and " error" in lines[-1]:
    try:
      nbrError = int(lines[-1].split()[1])
    except (ValueError, IndexError):
      nbrError = 0

  if details and nbrError > 0:
    for line in lines:
      notif(line)

  return nbrError


def _check_missing_docs(details: bool) -> int:
  return missing_docs(details)


def get_template_license() -> tuple[str, str]:
  """
  Get the template license for python and c++ files
  """
  current_year = datetime.now().year

  template_license = f"""
  This file is part of the aGrUM/pyAgrum library.

  Copyright (c) 2005-{current_year} by
      - Pierre-Henri WUILLEMIN(_at_LIP6)
      - Christophe GONZALES(_at_AMU)

  The aGrUM/pyAgrum library is free software; you can redistribute it
  and/or modify it under the terms of either :

   - the GNU Lesser General Public License as published by
     the Free Software Foundation, either version 3 of the License,
     or (at your option) any later version,
   - the MIT license (MIT),
   - or both in dual license, as here.

  (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)

  This aGrUM/pyAgrum library is distributed in the hope that it will be
  useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.

  See LICENCES for more details.

  SPDX-FileCopyrightText: Copyright 2005-{current_year}
      - Pierre-Henri WUILLEMIN(_at_LIP6)
      - Christophe GONZALES(_at_AMU)
  SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
  
  Contact  : info_at_agrum_dot_org
  homepage : http://agrum.gitlab.io
  gitlab   : https://gitlab.com/agrumery/agrum
      """
  width = max(len(line.rstrip()) for line in template_license.splitlines())

  template_py_license = "#" * (width + 4) + "\n"
  template_cpp_license = "/" + ("*" * (width + 4)) + "\n"

  for n, l in enumerate(template_license.splitlines()):
    if n == 0 and l.strip() == "":
      continue
    template_py_license += f"# {l.rstrip():{width}} #\n"
    template_cpp_license += f" * {l.rstrip():{width}} *\n"

  template_py_license += "#" * (width + 4) + "\n\n"
  template_cpp_license += " " + "*" * (width + 4) + "/\n\n"

  return template_py_license, template_cpp_license


_template_py_license, _template_cpp_license = get_template_license()
