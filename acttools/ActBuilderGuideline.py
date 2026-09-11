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
from subprocess import call, run as subprocess_run, Popen, PIPE, STDOUT

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


def _count_loc(root: Path, suffixes: set[str]) -> int:
  total = 0
  for path in root.rglob("*"):
    if path.suffix in suffixes and path.is_file():
      try:
        total += sum(1 for _ in path.open(errors="replace"))
      except OSError:
        pass
  return total


def _html_stats(html_dir: Path) -> tuple[int, int]:
  if not html_dir.is_dir():
    return 0, 0
  pages = list(html_dir.rglob("*.html"))
  size = sum(p.stat().st_size for p in pages)
  return len(pages), size


def _fmt_size(nbytes: int) -> str:
  for unit in ("B", "KB", "MB", "GB"):
    if nbytes < 1024:
      return f"{nbytes:.0f} {unit}"
    nbytes //= 1024
  return f"{nbytes:.0f} TB"


def compute_project_stats():
  notif("[[Project statistics]]")

  cpp_suffixes = {".h", ".cpp"}
  swig_suffixes = {".i"}
  py_suffixes = {".py"}

  loc_src = _count_loc(Path("src/agrum"), cpp_suffixes)
  notif(f"  C++ source      (src/agrum/)              : [[{loc_src:>10,}]] lines")

  loc_tests = _count_loc(Path("src/testunits"), cpp_suffixes)
  notif(f"  C++ tests       (src/testunits/)           : [[{loc_tests:>10,}]] lines")

  loc_pyagrum = (
    _count_loc(Path("wrappers/pyagrum/swigsrc"), swig_suffixes)
    + _count_loc(Path("wrappers/swig"), swig_suffixes)
    + _count_loc(Path("wrappers/pyagrum/pyLibs"), py_suffixes)
    + _count_loc(Path("wrappers/pyagrum/extensions"), {".h"})
  )
  notif(f"  pyAgrum sources (swigsrc/+swig/+pyLibs/+extensions/) : [[{loc_pyagrum:>10,}]] lines")

  loc_pytests = _count_loc(Path("wrappers/pyagrum/testunits"), py_suffixes)
  notif(f"  pyAgrum tests   (pyagrum/testunits/)       : [[{loc_pytests:>10,}]] lines")

  act_script = sys.argv[0]
  doxy_dir = Path("build/aGrUM/release/docs/html")
  sphinx_dir = Path("build/pyAgrum/release/wrappers/pyagrum/docs")

  to_build = []
  if not any(doxy_dir.rglob("*.html")):
    to_build.append(("aGrUM", doxy_dir))
  if not any(sphinx_dir.rglob("*.html")):
    to_build.append(("pyAgrum", sphinx_dir))

  if to_build:
    targets = " and ".join(f"[[{t}]]" for t, _ in to_build)
    notif(f"  Building {targets} documentation in parallel...")
    procs = [(t, Popen([act_script, "doc", t, "release"], stdout=PIPE, stderr=STDOUT)) for t, _ in to_build]
    for target, proc in procs:
      out, _ = proc.communicate()
      notif(f"  [[{target} doc]] output:")
      printutf8(out.decode(errors="replace"))

  doxy_pages, doxy_size = _html_stats(doxy_dir)
  if doxy_pages:
    notif(f"  Doxygen doc     (build/.../docs/html/)    : [[{doxy_pages:>6}]] pages  ({_fmt_size(doxy_size)})")
  else:
    notif("  Doxygen doc     : build failed or no output")

  sphinx_pages, sphinx_size = _html_stats(sphinx_dir)
  if sphinx_pages:
    notif(f"  Sphinx doc      (build/.../docs/)          : [[{sphinx_pages:>6}]] pages  ({_fmt_size(sphinx_size)})")
  else:
    notif("  Sphinx doc      : build failed or no output")

  nb_dir = Path("wrappers/pyagrum/doc/sphinx/notebooks")
  notebooks = list(nb_dir.glob("*.ipynb")) if nb_dir.is_dir() else []
  notif(f"  Tutorials       (sphinx/notebooks/)        : [[{len(notebooks):>6}]] notebooks")


class ActBuilderGuideline(ActBuilder):
  def __init__(self, current: dict[str, str | bool]):
    super().__init__(current)

  def check_consistency(self):
    return True

  def build(self):
    self.run_start()

    if self.current["stats"]:
      compute_project_stats()
    else:
      guideline(
        self.current,
        self.current["verbose"],
        self.current["correction"],
        self.current["dry_run"],
        self.current["guideline_check"],
        self.current["source"],
      )

    self.run_done()
    return True


_GUIDELINE_ALL_CHECKS: frozenset[str] = frozenset(
  {"cpp", "python", "header", "coverage", "deps", "tidy", "pyrefly", "pureheader", "inline", "pygum"}
)
# tidy/inline need build/aGrUM/<mode>/compile_commands.json (act lib release aGrUM first);
# pygum needs wrappers/pyagrum/generated-files/*.cxx (act install pyAgrum first) -- all three
# skip themselves with a warning if their prerequisite build artifact is missing, but are left
# out of the default set since a plain `act guideline` shouldn't silently require a prior build.
_GUIDELINE_DEFAULT_CHECKS: frozenset[str] = _GUIDELINE_ALL_CHECKS - {"tidy", "inline", "pygum"}


def _parse_checks(spec: str) -> frozenset[str]:
  """Parse a check specification like 'all-cpp' or 'cpp+pyrefly' into a set of check names."""
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
      critic(f"Unknown check '{tok}'. Valid checks : {', '.join(sorted(known))}")
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
  source: str | None = None,
) -> int:
  def _aff_errors(nb: int, typ: str) -> int:
    if nb > 0:
      # spaces to remove others possible characters
      error(f"{nb} {typ} error{'s' if nb > 1 else ''}{' ' * 40}")
    return nb

  active = _parse_checks(checks) if checks is not None else _GUIDELINE_DEFAULT_CHECKS
  run_cpp = "cpp" in active
  run_python = "python" in active
  run_header = "header" in active
  run_coverage = "coverage" in active
  run_deps = "deps" in active
  run_tidy = "tidy" in active
  run_pyrefly = "pyrefly" in active
  run_pureheader = "pureheader" in active
  run_inline = "inline" in active
  run_pygum = "pygum" in active

  effective_correction = correction and not dry_run
  active_checks_label = checks if checks is not None else "default"
  notif(
    f"[[aGrUM {'detailed ' if details else ''}guideline "
    f"[{active_checks_label}]"
    f"{' (with correction)' if effective_correction else ''}"
    f"{' (dry-run)' if dry_run else ''}]]"
  )
  if source:
    notif(f"  [[--source]] filter: '{source}'")
    reset_source_filter_match_count()

  nbrError = 0

  if run_cpp:
    notif("  [[(1-cpp) ]]*.cpp[[ file for every ]]*.h[[ file]]")
    nbrError += _aff_errors(_check_cpp_file_exists(details, effective_correction, source), "missing cpp file")
  else:
    notif("  (1-cpp) pass")

  if run_header:
    notif("  [[(2-header) check for ]]LGPL+MIT[[ license]]")
    nbrError += _aff_errors(
      _check_LGPL_MIT_license_CPP(details, effective_correction, source),
      "missing LGPL+MIT cpp licence",
    )
    nbrError += _aff_errors(
      _check_LGPL_MIT_license_py(details, effective_correction, source),
      "missing LGPL+MIT python licence",
    )
  else:
    notif("  (2-header) pass")

  if run_coverage:
    notif("  [[(3-coverage) check for missing documentation in pyAgrum]]")
    nbrError += _aff_errors(_check_missing_docs(details), "missing documentation")
  else:
    notif("  (3-coverage) pass")

  if run_deps:
    notif("  [[(4-deps) check for deps]]")
    nbrError += _aff_errors(
      check_gum_dependencies(
        graph=current["build_graph"],
        details=details,
        correction=effective_correction,
      ),
      "redundant dependency",
    )
  else:
    notif("  (4-deps) pass")

  if run_tidy:
    notif("  [[(5-tidy) check clang-tidy fixes]]")
    nbrError += _aff_errors(_check_clang_tidy(details, effective_correction, dry_run), "clang-tidy")
  else:
    notif("  (5-tidy) pass")

  if run_cpp:
    notif("  [[(6-cpp) check for cpp format]]")
    nbrError += _aff_errors(_check_clang_format(details, effective_correction, dry_run, source), "format")
  else:
    notif("  (6-cpp) pass")

  if run_python:
    notif("  [[(7-python) check for py format]]")
    nbrError += _aff_errors(_check_ruff_format(details, effective_correction, dry_run, source), "format")
  else:
    notif("  (7-python) pass")

  if run_pyrefly:
    notif("  [[(8-pyrefly) check pyrefly type annotations in pyLibs]]")
    nbrError += _aff_errors(
      _check_pyrefly(details, effective_correction, dry_run),
      "pyrefly type (unrecorded in coverage)",
    )
  else:
    notif("  (8-pyrefly) pass")

  if run_pureheader:
    notif("  [[(9-pureheader) check for function bodies in ]]*.h[[ declaration headers]]")
    nbrError += _aff_errors(_check_pure_headers(details, source), "body in declaration header")
  else:
    notif("  (9-pureheader) pass")

  if run_inline:
    notif("  [[(10-inline) check for oversized functions in ]]*.h_inl[[ files (readability-function-size)]]")
    nbrError += _aff_errors(_check_inline_size(details, dry_run), "oversized inline function")
  else:
    notif("  (10-inline) pass")

  if source and source_filter_match_count() == 0:
    warn(f"--source pattern '{source}' matched no file: no check actually inspected anything.")

  if run_pygum:
    notif("  [[(11-pygum) check PYGUM_SHARED_PUBLIC tagging for BASE/BN symbols used by leaf pyAgrum modules]]")
    nbrError += _aff_errors(_check_pygum_export(details, effective_correction), "missing PYGUM_SHARED_PUBLIC")
  else:
    notif("  (11-pygum) pass")

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
  source=None,
  dry_run=False,
  fix_stderr_visible=False,
) -> int:
  sources = filterSources(sources, source)
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


def _check_ruff_format(details: bool, correction: bool, dry_run: bool = False, source: str | None = None) -> int:
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
    source=source,
  )


_TIDY_PROGRESS_RE = re.compile(r"^\[(\d+)/(\d+)\].*? (\S+\.cpp)$")


def _run_tidy_cmd(cmd: str, details: bool) -> tuple[int, dict[str, int]]:
  nbrError = 0
  warnings_by_file: dict[str, int] = {}
  proc = Popen(cmd, shell=True, stdout=PIPE, stderr=STDOUT, text=True)
  for line in proc.stdout:
    line = line.rstrip()
    m_prog = _TIDY_PROGRESS_RE.match(line)
    m_warn = _TIDY_WARNING_RE.match(line)
    if m_prog:
      notif_oneline(f"[[{os.path.basename(m_prog.group(3))}]]")
    elif m_warn:
      nbrError += 1
      if details:
        fname = os.path.basename(m_warn.group(1))
        warnings_by_file[fname] = warnings_by_file.get(fname, 0) + 1
        notif(line)
  proc.wait()
  return nbrError, warnings_by_file


_TIDY_WARNING_RE = re.compile(r"^(.+?):\d+:\d+: warning:")


def _check_clang_tidy(details: bool, correction: bool, dry_run: bool = False) -> int:
  tool = cfg.run_clang_tidy
  if tool is None:
    warn("No [[run-clang-tidy]] tool found. Skipping tidy check.")
    return 0

  build_dir = os.path.join("build", "aGrUM", cfg.buildPath["Release"])
  if not os.path.isfile(os.path.join(build_dir, "compile_commands.json")):
    warn(f"No [[compile_commands.json]] in {build_dir}. Run 'act lib release aGrUM' first.")
    return 0

  fix_flag = " -fix -format" if correction else ""
  cmd = f"{tool}{fix_flag} -config-file=.clang-tidy-fix -p {build_dir} 'src/agrum/(?!base/external)'"

  if dry_run:
    notif(f"  {cmd}")
    return 0

  nbrError, warnings_by_file = _run_tidy_cmd(cmd, details)

  if details:
    for fname, count in sorted(warnings_by_file.items()):
      notif(f"  err [[{fname}]] ({count} warning{'s' if count > 1 else ''})")
  elif nbrError > 0:
    notif(f"  {nbrError} clang-tidy warning{'s' if nbrError > 1 else ''}")

  if nbrError == 0:
    notif("    clang-tidy: [[(✓)]]")

  return nbrError


def _check_inline_size(details: bool, dry_run: bool = False) -> int:
  tool = cfg.run_clang_tidy
  if tool is None:
    warn("No [[run-clang-tidy]] tool found. Skipping inline check.")
    return 0

  build_dir = os.path.join("build", "aGrUM", cfg.buildPath["Release"])
  if not os.path.isfile(os.path.join(build_dir, "compile_commands.json")):
    warn(f"No [[compile_commands.json]] in {build_dir}. Run 'act lib release aGrUM' first.")
    return 0

  cmd = f"{tool} -config-file=.clang-tidy-inline -p {build_dir} 'src/agrum/(?!base/external)'"

  if dry_run:
    notif(f"  {cmd}")
    return 0

  nbrError, warnings_by_file = _run_tidy_cmd(cmd, details)

  if details:
    for fname, count in sorted(warnings_by_file.items()):
      notif(f"  err [[{fname}]] ({count} warning{'s' if count > 1 else ''})")
  elif nbrError > 0:
    notif(f"  {nbrError} oversized inline function{'s' if nbrError > 1 else ''}")

  if nbrError == 0:
    notif("    inline size: [[(✓)]]")

  return nbrError


def _check_clang_format(details: bool, correction: bool, dry_run: bool = False, source: str | None = None) -> int:
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
    source=source,
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


def _check_LGPL_MIT_license_CPP(details: bool, correction: bool, source: str | None = None) -> int:
  nbrError = 0

  exceptions = [
    f"{os.sep}mvsc{os.sep}",
    f"{os.sep}external{os.sep}",
    "Parser",
    "Scanner",
    "doctest",
  ]
  for gum_file in filterSources(srcAgrum(), source):
    if any(subs in gum_file for subs in exceptions):
      if details:
        notif(f"skip header test for [[{gum_file}]]")
      continue
    nbrError += _LGPL_MIT_atTop_CPP(gum_file, details, correction)
  for gum_file in filterSources(srcGeneratorAgrum(), source):
    if any(subs in gum_file for subs in exceptions):
      if details:
        notif(f"skip header test for [[{gum_file}]]")
      continue
    nbrError += _LGPL_MIT_atTop_CPP(gum_file, details, correction)

  return nbrError


def _check_LGPL_MIT_license_py(details: bool, correction: bool, source: str | None = None) -> int:
  nbrError = 0

  exceptions = []
  for pygum_file in filterSources(srcPyAgrum(), source):
    if details:
      notif_oneline(f"[[{pygum_file.split('/')[-1]}]]")
    if any(subs in pygum_file for subs in exceptions):
      continue
    nbrError += _LGPL_MIT_atTop_py(pygum_file, details, correction)

  for cmake_file in filterSources(srcCmakeAgrum(), source):
    if details:
      notif_oneline(f"[[{cmake_file.split('/')[-1]}]]")
    nbrError += _LGPL_MIT_atTop_cmake(cmake_file, details, correction)

  return nbrError


def _check_cpp_file_exists(details: bool, correction: bool, source: str | None = None) -> int:
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

  for header in filterSources(recglob(f"src{os.sep}agrum", "*.h"), source):
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


def _check_pyrefly(details: bool, correction: bool, dry_run: bool = False) -> int:
  pyrefly_cmd = "pyrefly check"

  if dry_run:
    notif(f"  {pyrefly_cmd} {cfg.pymodulesPath}")
    return 0

  probe = subprocess_run(
    "pyrefly --version",
    shell=True,
    stdout=PIPE,
    stderr=PIPE,
  )
  if probe.returncode != 0:
    warn("[[pyrefly]] not installed in current Python environment. Skipping.")
    return 0

  result = subprocess_run(
    f"{pyrefly_cmd} {cfg.pymodulesPath}",
    shell=True,
    stdout=PIPE,
    stderr=PIPE,
    text=True,
  )

  nbrError = 0

  # Summary goes to stderr: " INFO N errors (K suppressed)" followed by config notice lines
  match = re.search(r"INFO (\d+) error", result.stderr)
  if match:
    nbrError = int(match.group(1))

  suppressed_match = re.search(r"\((\d+) suppressed\)", result.stderr)
  suppressed = int(suppressed_match.group(1)) if suppressed_match else 0

  if nbrError == 0:
    suffix = f" ({suppressed} suppressed)" if suppressed else ""
    notif(f"    pyrefly: [[(✓)]]{suffix}")
  elif details:
    for line in result.stdout.splitlines():
      notif(line)

  return nbrError


# --- pureheader check: module-level compiled patterns ---

# Explicit INLINE-keyword body in a declaration header.
_PH_INLINE_BODY_RE = re.compile(r"\bINLINE\b.*\{")

# Method/function body: closing paren followed by optional C++ qualifiers
# (const, volatile, noexcept, noexcept(expr), override, final, -> trailing-return),
# then the opening brace.
_PH_METHOD_BODY_RE = re.compile(
  r"\)"  # closing paren of params
  r"\s*(?:const\s+)?"  # optional const
  r"(?:volatile\s+)?"  # optional volatile
  r"(?:noexcept\s*(?:\([^)]*\))?\s*)?"  # optional noexcept or noexcept(expr)
  r"(?:override\s+)?"  # optional override
  r"(?:final\s+)?"  # optional final
  r"(?:const\s+)?"  # optional trailing const (rare)
  r"(?:->[^{]*)?"  # optional trailing return type  -> T
  r"\{"
)

# Control-flow keywords that open a brace but are not function bodies.
_PH_CONTROL_FLOW_RE = re.compile(r"\b(?:if|for|while|switch|catch|else)\s*\(")

# GUM debug macros: constructor/destructor helpers that open a brace on their line.
_PH_GUM_MACRO_RE = re.compile(r"\b(?:GUM_CONSTRUCTOR|GUM_DESTRUCTOR|GUM_CONS_MOV|GUM_CONS_CPY)\b")

# consteval (or constexpr) function with an inline empty body — legitimate in headers.
_PH_CONSTEVAL_RE = re.compile(r"\b(?:consteval|constexpr)\b.*\)\s*[^{]*\{\s*\}")

# requires-expression (C++20 concepts).
_PH_REQUIRES_RE = re.compile(r"\brequires\b")

# friend declaration or definition — inline friends in template classes are legitimate.
_PH_FRIEND_BODY_RE = re.compile(r"\bfriend\b.*\{")

# Empty-body pattern for multi-line consteval/constexpr continuation.
_PH_EMPTY_BODY_RE = re.compile(r"\{\s*\}$")

# Class/struct/namespace/enum opening — not a method body.
_PH_AGGREGATE_RE = re.compile(r"^(?:class|struct|namespace|enum)\b")

# Lambda: [ captures ]( params ) { — not a method body.
_PH_LAMBDA_RE = re.compile(r"\]\s*\(")

# Virtual base specifier — not a method body.
_PH_VIRTUAL_BASE_RE = re.compile(r"\b(?:public|protected|private)\s+virtual\b")

_PH_SKIP_DIRS = frozenset(
  [
    f"{os.sep}external{os.sep}",
    f"{os.sep}mvsc{os.sep}",
    f"{os.sep}cocoR{os.sep}",
    f"{os.sep}patterns{os.sep}",  # code-pattern fragments included via macros
  ]
)


def _check_pure_headers(details: bool, source: str | None = None) -> int:
  """Check that *.h declaration headers contain no function/method bodies.

  Skips *_tpl.h, *_inl.h, *TestSuite.h, *Inline.h, and directories
  external/, mvsc/, cocoR/, patterns/.

  Flags:
    - INLINE ... {              explicit INLINE-keyword definitions
    - ) [qualifiers] [-> T] {   any function/method body (incl. trailing return type)

  Does NOT flag:
    - friend ... {              inline friends in template classes (legitimate)
    - consteval/constexpr ... {} single-line empty bodies (required in header)
    - requires(...) {           C++20 concept constraints
    - GUM_CONSTRUCTOR/DESTRUCTOR/CONS_CPY/CONS_MOV lines
    - class/struct/namespace/enum openings
    - control-flow blocks (if/for/while/switch/catch)
    - lambdas ([captures](...) {)
  """
  nbrError = 0
  for header in filterSources(recglob(f"src{os.sep}agrum", "*.h"), source):
    basename = header.split(os.sep)[-1]
    if basename.endswith(("_tpl.h", "_inl.h", "Inline.h")):
      continue
    if "TestSuite" in basename:
      continue
    if any(exc in header for exc in _PH_SKIP_DIRS):
      continue

    violations: list[tuple[int, str]] = []
    with open(header, encoding="utf-8", errors="replace") as f:
      in_macro = False
      in_code_block = False
      prev_code_line = ""
      for lineno, line in enumerate(f, start=1):
        stripped = line.strip()

        # skip multi-line macro continuations
        if in_macro:
          if not stripped.endswith("\\"):
            in_macro = False
          continue

        # track @code/@endcode Doxygen blocks (may appear in non-comment context)
        if "@code" in stripped:
          in_code_block = True
        if "@endcode" in stripped:
          in_code_block = False
          continue
        if in_code_block:
          continue

        # skip preprocessor directives and comment lines
        if stripped.startswith(("#", "*", "//", "/*")):
          if stripped.startswith("#") and stripped.endswith("\\"):
            in_macro = True
          continue

        # skip C++20 requires-expressions
        if _PH_REQUIRES_RE.search(stripped):
          prev_code_line = stripped
          continue

        # skip consteval/constexpr single-line empty body (required in headers)
        if _PH_CONSTEVAL_RE.search(stripped):
          prev_code_line = stripped
          continue

        # skip multi-line consteval/constexpr continuation (empty {} on next line)
        if _PH_EMPTY_BODY_RE.search(stripped) and re.search(r"\b(?:consteval|constexpr)\b", prev_code_line):
          prev_code_line = stripped
          continue

        # skip inline friend definitions (legitimate C++ pattern in template classes)
        if _PH_FRIEND_BODY_RE.search(stripped):
          prev_code_line = stripped
          continue

        # --- violation detection ---
        if _PH_INLINE_BODY_RE.search(stripped):
          violations.append((lineno, stripped))

        elif _PH_METHOD_BODY_RE.search(stripped):
          # skip class/struct/namespace/enum openings
          if _PH_AGGREGATE_RE.match(stripped):
            prev_code_line = stripped
            continue
          # skip virtual base class specifiers (class Foo : public virtual Bar {)
          if _PH_VIRTUAL_BASE_RE.search(stripped):
            prev_code_line = stripped
            continue
          # skip control-flow blocks
          if _PH_CONTROL_FLOW_RE.search(stripped):
            prev_code_line = stripped
            continue
          # skip lambdas: [captures](...) {
          if stripped.startswith("[") or _PH_LAMBDA_RE.search(stripped):
            prev_code_line = stripped
            continue
          # skip GUM constructor/destructor macro lines
          if _PH_GUM_MACRO_RE.search(stripped):
            prev_code_line = stripped
            continue
          violations.append((lineno, stripped))

        prev_code_line = stripped

    if violations:
      nbrError += len(violations)
      if details:
        for lineno, text in violations:
          notif(f"  err [[{header}:{lineno}]] {text[:80]}")
      else:
        notif(f"  err [[{header}]] ({len(violations)} violation{'s' if len(violations) > 1 else ''})")

  if nbrError == 0:
    notif("    pureheader: [[(✓)]]")
  return nbrError


# --- pygum check: PYGUM_SHARED_PUBLIC completeness for BASE/BN symbols crossing
# the core <-> leaf-module DLL boundary (see src/cmake/config.h.in and
# wrappers/pyagrum/CMakeLists.txt for the whole-archive/dllexport design).

_PYGUM_SKIP_DIRS = _PH_SKIP_DIRS  # external/, mvsc/, cocoR/, patterns/: vendored or
# generated code, not meant to be auto-tagged by this check.

_PYGUM_LEAF_MODULES = ("mrf", "id", "cn", "cm", "prm")

# Class/struct definition opening its body on the same line, with an optional
# PYGUM_SHARED_PUBLIC tag right after the keyword -- e.g. "class Foo {" or
# "struct PYGUM_SHARED_PUBLIC Foo : public Bar {". Forward declarations
# ("class Foo;", no "{" on the line) never match, which is intentional: only
# the real definition is a tagging candidate.
_PYGUM_CLASS_DECL_RE = re.compile(
  r"^\s*(?:class|struct)\s+(?:(PYGUM_SHARED_PUBLIC)\s+)?([A-Za-z_]\w*)\b[^;{]*\{"
)

# GUM_MAKE_ERROR(TYPE, ...) invocation sites: the macro (exceptions.h) always
# expands to "class PYGUM_SHARED_PUBLIC TYPE : public SUPERCLASS { ... }", so every
# exception class it generates is always tagged -- no need to inspect the expansion.
_PYGUM_MAKE_ERROR_RE = re.compile(r"\bGUM_MAKE_ERROR\(\s*([A-Za-z_]\w*)")

# Bounded explicit instantiation, either the "extern template class ... <T>;" forward
# declaration (header) or the "template class ... <T>;" definition (cpp) -- e.g.
# "extern template class PYGUM_SHARED_PUBLIC BayesNet< double >;" or
# "template class PYGUM_SHARED_PUBLIC gum::Set< int >;". This is the ONLY tagging a
# class template ever gets: its generic "template <...> class Foo {" declaration is
# deliberately left untagged (MSVC C4910: dllexport/PYGUM_SHARED_PUBLIC is incompatible
# with "extern template class" on a still-generic declaration, see
# GUM_NO_EXTERN_TEMPLATE_CLASS in CMakeLists.txt) -- so this pattern must count as
# tagging evidence on its own, independently of (and overriding) the plain class
# declaration scan below.
_PYGUM_EXPLICIT_INST_RE = re.compile(
  r"^\s*(?:extern\s+)?template\s+class\s+PYGUM_SHARED_PUBLIC\s+(?:gum::)?([A-Za-z_]\w*)\s*<"
)

# A gum::Name reference in generated SWIG C++ code. Only the first identifier
# after "gum::" is captured -- good enough to check class-level tagging, but
# nested namespaces (gum::learning::Foo captures "learning") are not resolved:
# a documented false-negative, not a false-positive risk.
_PYGUM_REF_RE = re.compile(r"\bgum::([A-Za-z_]\w*)")


def _pygum_declared_symbols() -> dict[str, tuple[bool, str, int]]:
  """Maps class/struct name -> (is PYGUM_SHARED_PUBLIC tagged, file, line) for every
  class declared directly in src/agrum/base/ or src/agrum/BN/ (BASE/BN are the only
  modules embedded into core _pyagrum -- see Modules.agrum.cmake -- so they're the
  only ones whose symbols need the producer/consumer split at all; CM/CN/ID/MRF/PRM's
  own classes are self-contained PYGUM_PUBLIC, never checked here). Free functions are
  not tracked: historically resolved by hand (a handful of cases), not worth a
  fragile declaration parser.

  Scans both *.h (generic declarations, GUM_MAKE_ERROR sites, extern template class
  instantiations) and *.cpp (the "template class PYGUM_SHARED_PUBLIC ..." explicit
  instantiation definitions, which live in the .cpp, not the header) -- a name is
  recorded as tagged the moment ANY sighting says so, regardless of scan order, so a
  class template's untagged generic declaration never overrides a tagged explicit
  instantiation found in a different file.
  """
  declared: dict[str, tuple[bool, str, int]] = {}

  def register(name: str, tagged: bool, path: str, lineno: int) -> None:
    if name in declared and declared[name][0]:
      return  # already known tagged elsewhere -- an untagged sighting can't downgrade it
    if tagged or name not in declared:
      declared[name] = (tagged, path, lineno)

  for base_dir in (f"src{os.sep}agrum{os.sep}base", f"src{os.sep}agrum{os.sep}BN"):
    for mask in ("*.h", "*.cpp"):
      for path in recglob(base_dir, mask):
        if any(exc in path for exc in _PYGUM_SKIP_DIRS):
          continue
        with open(path, encoding="utf-8", errors="replace") as f:
          for lineno, line in enumerate(f, start=1):
            m = _PYGUM_CLASS_DECL_RE.match(line)
            if m:
              register(m.group(2), m.group(1) is not None, path, lineno)
              continue
            m = _PYGUM_MAKE_ERROR_RE.search(line)
            if m:
              register(m.group(1), True, path, lineno)
              continue
            m = _PYGUM_EXPLICIT_INST_RE.match(line)
            if m:
              register(m.group(1), True, path, lineno)
  return declared


def _pygum_leaf_references() -> dict[str, set[str]] | None:
  """Maps a referenced gum::Name -> set of leaf modules (mrf/id/cn/cm/prm) whose
  generated SWIG wrap .cxx references it. Returns None if none of the generated
  wrap files exist yet (no prior pyAgrum build/SWIG generation to inspect).
  """
  gen_dir = os.path.join("wrappers", "pyagrum", "generated-files")
  refs: dict[str, set[str]] = {}
  found_any = False
  for mod in _PYGUM_LEAF_MODULES:
    wrap_file = os.path.join(gen_dir, f"{mod}PYTHON_wrap.cxx")
    if not os.path.isfile(wrap_file):
      continue
    found_any = True
    with open(wrap_file, encoding="utf-8", errors="replace") as f:
      content = f.read()
    for m in _PYGUM_REF_RE.finditer(content):
      refs.setdefault(m.group(1), set()).add(mod)
  return refs if found_any else None


def _pygum_is_template_decl(lines: list[str], lineno: int) -> bool:
  """True if the declaration at 1-based `lineno` is preceded (skipping blank/comment
  lines) by a `template <...>` line. Such a class must never be tagged at its generic
  declaration -- see _PYGUM_EXPLICIT_INST_RE -- only a bounded explicit instantiation
  can carry PYGUM_SHARED_PUBLIC for it.
  """
  i = lineno - 2  # 0-based index of the line just above the declaration
  while i >= 0:
    stripped = lines[i].strip()
    if stripped == "" or stripped.startswith(("//", "/*", "*")):
      i -= 1
      continue
    return stripped.startswith("template") and "<" in stripped
  return False


def _check_pygum_export(details: bool, correction: bool) -> int:
  """Check that every BASE/BN class referenced (via gum::Name) from a leaf pyAgrum
  module's generated SWIG wrap file is tagged PYGUM_SHARED_PUBLIC. An untagged
  symbol still resolves to dllimport for that leaf module on Windows
  (config.h.in: GUM_FOR_SWIG defined, PYGUM_SHARED_EXPORTING not) for a symbol
  that core's binary never actually exports -- MinGW's --export-all-symbols
  (wrappers/pyagrum/CMakeLists.txt) papers over the gap by exporting everything
  regardless of tags, but MSVC (no such fallback: CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS
  is off) and -fvisibility=hidden macOS/Linux builds (no fallback either) do not
  -- so a gap here is a latent MSVC/macOS/Linux regression that only MinGW CI
  would stay silent about.

  Heuristic, not a full C++ parser (see _pygum_declared_symbols/_PYGUM_REF_RE):
  only class/struct names are checked, single-line declarations only, nested
  namespaces are not resolved. Requires a prior 'act install pyAgrum' (or at
  least SWIG generation) so the generated wrap .cxx files exist -- skipped with
  a warning otherwise, same as the tidy/inline checks skip without
  compile_commands.json.

  --correction only ever edits a NON-template class's own declaration line. A
  class template's generic declaration is never auto-tagged -- see
  _pygum_is_template_decl -- doing so would reintroduce the exact MSVC C4910
  (dllexport/extern template class conflict) this codebase already hit once;
  those are reported with a distinct message asking for a bounded explicit
  instantiation to be added by hand instead.
  """
  leaf_refs = _pygum_leaf_references()
  if leaf_refs is None:
    warn("No generated SWIG wrap files in wrappers/pyagrum/generated-files/. Run 'act install pyAgrum' first.")
    return 0

  declared = _pygum_declared_symbols()

  violations: list[tuple[str, str, int, list[str]]] = []
  for name, modules in leaf_refs.items():
    if name not in declared:
      continue
    tagged, header, lineno = declared[name]
    if not tagged:
      violations.append((name, header, lineno, sorted(modules)))
  violations.sort(key=lambda v: (v[1], v[2]))

  for name, header, lineno, modules in violations:
    with open(header, encoding="utf-8") as f:
      lines = f.readlines()
    is_template = _pygum_is_template_decl(lines, lineno)

    res = f"  err [[{header}:{lineno}]] gum::{name} used by {'+'.join(modules)} but not PYGUM_SHARED_PUBLIC"
    if is_template:
      res = (
        f"{res} [[(template -- add a bounded 'extern template class "
        f"PYGUM_SHARED_PUBLIC {name}<T>;' instead, not auto-fixed)]]"
      )
    elif correction:
      lines[lineno - 1] = re.sub(
        r"^(\s*)(class|struct)\s+", r"\1\2 PYGUM_SHARED_PUBLIC ", lines[lineno - 1], count=1
      )
      with open(header, "w", encoding="utf-8") as f:
        f.writelines(lines)
      res = f"{res} [[(✓)]]"
    if details or correction or is_template:
      notif(res)

  if not violations:
    notif("    pygum: [[(✓)]]")
  return len(violations)


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
