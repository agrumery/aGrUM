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

import json
import os
import pathlib
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
import textwrap

from IPython.core.magic import Magics, cell_magic, magics_class
from IPython.display import HTML, display

from .notebook import show_dots

_ABI_PROBE_SRC = """\
#include <agrum/base/variables/labelizedVariable.h>
int main() {
    gum::LabelizedVariable v("v", "v", 2);
    return v.domainSize() == 2 ? 0 : 1;
}
"""

_CMAKE_PROBE = """\
cmake_minimum_required(VERSION 3.16)
project(agrum_probe LANGUAGES CXX)
find_package(aGrUM CONFIG REQUIRED)
message(STATUS "AGRUM_INCLUDE_DIRS=${AGRUM_INCLUDE_DIRS}")
message(STATUS "AGRUM_CXX_FLAGS=${AGRUM_CXX_FLAGS}")
message(STATUS "AGRUM_LIBRARY_DIRS=${AGRUM_LIBRARY_DIRS}")
message(STATUS "AGRUM_LIBRARIES=${AGRUM_LIBRARIES}")
get_target_property(AGRUM_LINK_LIBS agrum INTERFACE_LINK_LIBRARIES)
message(STATUS "AGRUM_LINK_LIBS=${AGRUM_LINK_LIBS}")
"""

_INSERT_ADD_DOT = """\
#include <iostream>
#include <fstream>

void cppnb_add_dot(std::string_view strdot) {
  static int dot_counter = 0;
  std::string filename = "__tmp_" + std::to_string(dot_counter++) + "__.dot";
  std::ofstream ofs(filename);
  ofs << strdot;
  ofs.close();
}
"""

_NB_DOT_MAX = 5


def find_working_compiler(cxxflags: str, ldflags: str) -> str:
  """Finds a C++ compiler able to actually compile and link against the installed aGrUM
    (rules out ABI mismatches, e.g. libc++ vs libstdc++, that a plain PATH lookup would miss)."""
  candidates = ['g++', 'clang++', 'c++', 'cl']
  with tempfile.TemporaryDirectory() as tmp:
    src = os.path.join(tmp, "abi_probe.cpp")
    with open(src, "w") as f:
      f.write(_ABI_PROBE_SRC)
    binary = os.path.join(tmp, "abi_probe")

    tried = []
    for compiler in candidates:
      path = shutil.which(compiler)
      if not path:
        continue
      cmd = [path, "-std=c++20", src, "-o", binary] + shlex.split(cxxflags) + shlex.split(ldflags)
      result = subprocess.run(cmd, capture_output=True, text=True)
      if result.returncode != 0:
        tried.append((path, result.stderr.strip().splitlines()[-1] if result.stderr else ""))
        continue
      run = subprocess.run([binary], capture_output=True, text=True)
      if run.returncode == 0:
        return path
      tried.append((path, run.stderr.strip().splitlines()[-1] if run.stderr else f"exit {run.returncode}"))

    details = "\n".join(f"  {p}: {err}" for p, err in tried)
    raise EnvironmentError(f"No compiler found that links against this aGrUM build:\n{details}")


def find_agrum_flags(agrum_prefix: str | None = None) -> dict[str, str]:
  """Queries aGrUMConfig.cmake and returns the raw AGRUM_* variables.
    Defaults to the current Python environment's prefix (venv/conda), where
    `act install aGrUM` also installs the CMake package config.

    Tries several C++ compilers for the CMake *configure* step itself:
    aGrUM's CMake package pulls in dependencies (e.g. OpenMP) that CMake's
    ambient default compiler may not satisfy even though a working compiler
    is available (e.g. Xcode's AppleClang has no OpenMP support on macOS,
    while Homebrew's clang++ or g++ do)."""
  agrum_prefix = agrum_prefix or sys.prefix
  candidates = [c for c in dict.fromkeys(shutil.which(c) for c in ('g++', 'clang++', 'c++')) if c] or [None]

  with tempfile.TemporaryDirectory() as tmp:
    with open(os.path.join(tmp, "CMakeLists.txt"), "w") as f:
      f.write(_CMAKE_PROBE)

    errors = []
    for i, compiler in enumerate(candidates):
      build = os.path.join(tmp, f"build_{i}")
      cmd = ["cmake", "-S", tmp, "-B", build]
      if agrum_prefix:
        cmd.append(f"-DCMAKE_PREFIX_PATH={agrum_prefix}")
      if compiler:
        cmd.append(f"-DCMAKE_CXX_COMPILER={compiler}")
      result = subprocess.run(cmd, capture_output=True, text=True)
      if result.returncode == 0:
        break
      errors.append((compiler or "<cmake default>", result.stderr.strip().splitlines()[-1] if result.stderr else ""))
    else:
      details = "\n".join(f"  {c}: {err}" for c, err in errors)
      raise EnvironmentError(f"aGrUM not found via CMake with any candidate compiler (set agrum_prefix?):\n{details}")

    agrum_vars = {}
    for line in result.stdout.splitlines():
      m = re.search(r"-- (AGRUM_\w+)=(.*)", line)
      if m:
        agrum_vars[m.group(1)] = m.group(2)
    return agrum_vars


def agrum_build_flags(agrum_prefix: str | None = None) -> tuple[str, str]:
  """Returns (CXXFLAGS, LDFLAGS) needed to compile/link against an installed aGrUM."""
  agrum_vars = find_agrum_flags(agrum_prefix)
  cxxflags = f"-I{agrum_vars['AGRUM_INCLUDE_DIRS']} {agrum_vars['AGRUM_CXX_FLAGS']}".strip()
  lib_dir = agrum_vars['AGRUM_LIBRARY_DIRS']
  ldflags = f"-L{lib_dir} -Wl,-rpath,{lib_dir} " + " ".join(
    f"-l{lib}" for lib in reversed(agrum_vars['AGRUM_LINK_LIBS'].split(';')) if lib
  )
  return cxxflags, ldflags


def find_dot() -> str:
  """Finds a working Graphviz dot executable."""
  candidates = ['dot', 'dot.exe']
  for dot in candidates:
    path = shutil.which(dot)
    if path:
      return path
  raise EnvironmentError("Graphviz dot not found in PATH")


def _cache_path() -> pathlib.Path:
  """Location of the cached compiler/aGrUM configuration (user-wide, not the notebook's cwd)."""
  try:
    import platformdirs
    cache_dir = pathlib.Path(platformdirs.user_cache_dir("gum_cppnb"))
  except ImportError:
    cache_dir = pathlib.Path.home() / ".cache" / "gum_cppnb"
  cache_dir.mkdir(parents=True, exist_ok=True)
  return cache_dir / "config.json"


def check_agrum() -> dict[str, str]:
  """Probes compiler/dot/aGrUM CMake flags and refreshes the on-disk cache."""
  config: dict[str, str] = {}
  config['cxxflags'], config['ldflags'] = agrum_build_flags()
  config['ccexe'] = find_working_compiler(config['cxxflags'], config['ldflags'])
  config['dotexe'] = find_dot()

  with open(_cache_path(), "w") as f:
    json.dump(config, f)
  return config


def _load_config() -> dict[str, str]:
  cache = _cache_path()
  if cache.exists():
    try:
      with open(cache) as f:
        return json.load(f)
    except json.JSONDecodeError:
      pass
  return check_agrum()


def cppnb_message(msg: str, color: str = "lightyellow") -> None:
  """Prints a message in a highlighted box with color."""
  display(HTML(f'<blockquote style="background-color: {color}; border-left: 5px solid #FFA500; padding: 10px;"><pre>{msg}</pre></blockquote>'))


def cppnb_warning(msg: str) -> None:
  """Prints a warning message in a highlighted box."""
  cppnb_message(msg, "lightblue")


def cppnb_error(msg: str) -> None:
  """Prints an error message in a highlighted box."""
  cppnb_message(msg, "lightcoral")


def cppnb_welcome() -> None:
  """Prints a welcome message with the detected configuration."""
  cppnb_message("""gum_cppnb
    (C) 2026 Seth Aguila, Anis Khacef and Pierre-Henri Wuillemin

    Welcome to the aGrUM C++ notebook environment!

    Configuration detected (use %reload_ext gum_cppnb to re-detect).

    You can now use the %%cpp magic to compile and run C++ code with aGrUM.""")


@magics_class
class CppNbMagics(Magics):
  """Registers the %%cpp cell magic, holding the probed config and a private
    scratch directory for compile artifacts (own temp dir per extension load,
    no shared /tmp path). The compiled program itself still runs with the
    notebook's own working directory as cwd, so relative paths in user code
    (e.g. writing to an `out/` folder) behave as in a normal notebook."""

  def __init__(self, shell, config: dict[str, str]):
    super().__init__(shell)
    # NB: "config" is a reserved traitlets attribute on Magics/HasTraits
    # (IPython's own configuration system) -- must not shadow it.
    self.agrum_config = config
    self.workdir = tempfile.mkdtemp(prefix="gum_cppnb_")

  def _tmp_dot_path(self, i: int) -> str:
    return f"__tmp_{i}__.dot"

  def _clear_dots(self) -> None:
    for i in range(_NB_DOT_MAX):
      dotname = self._tmp_dot_path(i)
      if os.path.exists(dotname):
        os.remove(dotname)

  @cell_magic
  def cpp(self, line, cell):
    '''Compile+run a C++ snippet; print its stdout.'''
    self._clear_dots()

    cxxflags = shlex.split(self.agrum_config['cxxflags'])
    ldflags = shlex.split(self.agrum_config['ldflags'])

    src = os.path.join(self.workdir, "_nb.cpp")
    binary = os.path.join(self.workdir, "_nb")
    with open(src, "w") as f:
      f.write(_INSERT_ADD_DOT + textwrap.dedent(cell))

    r = subprocess.run([self.agrum_config['ccexe']] + cxxflags +
                        [src, '-o', binary] + ldflags,
                        capture_output=True, text=True)
    if r.returncode:
      # Compilation error
      cppnb_warning(r.stderr)
      return

    run = (['stdbuf', '-o0'] if shutil.which('stdbuf') else []) + [binary]
    r = subprocess.run(run, capture_output=True, text=True)
    print(r.stdout, end='')

    if r.returncode:
      # Runtime error
      cppnb_error(r.stderr)
      return

    dots = [self._tmp_dot_path(i) for i in range(_NB_DOT_MAX)
            if os.path.exists(self._tmp_dot_path(i))]

    if dots:
      show_dots(*[(p, f"[{i}]") for i, p in enumerate(dots)], config=self.agrum_config)
      self._clear_dots()


def load_ipython_extension(ipython):
  config = _load_config()
  cppnb_welcome()
  ipython.register_magics(CppNbMagics(ipython, config))


def unload_ipython_extension(ipython):
  pass
