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
import platform
import time

from .configuration import cfg
from .utils import *

from .ActBuilder import ActBuilder


class ActBuilderPyAgrum(ActBuilder):
  def __init__(self, current: dict[str, str | bool]):
    super().__init__(current)

  def check_consistency(self) -> bool:
    cur_modules = self.current["modules"]
    if self.current["action"] == "test":
      match cur_modules.lower():
        case "all":
          self.modules = set([""] + list(cfg.modules.keys()))
        case "quick":
          self.modules = {""}
        case _:
          if cur_modules.lower().startswith("quick+"):
            module = cur_modules.lower()[len("quick+") :]

            if not module in cfg.pymodules:
              error(f"Module [{module}] not supported. Please select among [all,quick,quick+[{set(cfg.pymodules)}].")
              return False
            else:
              self.modules = {"", module}
          else:
            error(f"Module [{cur_modules}] not supported. '-m quick' is used by default.")
            self.modules = {""}

    if self.current["tests"] == "list":
      error("List of [pyAgrum]'s tests is not available for now. Sorry.")
      return False

    if not self.check_compiler_and_maker():
      return False

    if not self.current["static_lib"]:
      error("Static library forced for [pyAgrum] target.")
      self.current["static_lib"] = True

    return True

  def build_cmake(self):
    line = cfg.cmake + " ../../.."  # we are in build/[Release|Debug]/target

    line += " -DCMAKE_EXPORT_COMPILE_COMMANDS=ON "  # for clang-tidy

    if self.current["mode"] == "Release":
      line += " -DCMAKE_BUILD_TYPE=RELEASE"
    else:
      line += " -DCMAKE_BUILD_TYPE=DEBUG"

    if self.current["withSQL"]:
      line += " -DUSE_NANODBC=ON"
    else:
      line += " -DUSE_NANODBC=OFF"

    line += " -DCMAKE_INSTALL_PREFIX=" + '"' + self.current["destination"] + '"'

    # defaults to lib64 on manylinux images
    if "Linux" in platform.system():
      line += " -DCMAKE_INSTALL_LIBDIR=lib"

    if self.current["verbose"]:
      line += " -DCMAKE_VERBOSE_MAKEFILE=ON"
    else:
      line += " -DCMAKE_VERBOSE_MAKEFILE=OFF"

    if self.current["static_lib"]:
      line += " -DBUILD_SHARED_LIBS=OFF"
    else:
      line += " -DBUILD_SHARED_LIBS=ON"

    if self.current["coverage"]:
      line += " -DGUM_COVERAGE=ON"
    else:
      line += " -DGUM_COVERAGE=OFF"

    line += " -DBUILD_ALL=ON"

    if self.current["fixed_seed"]:
      line += " -DGUM_RANDOMSEED=" + cfg.fixedSeedValue
    else:
      line += " -DGUM_RANDOMSEED=0"

    line += " -DBUILD_PYTHON=ON"

    if self.current["stable_abi_off"]:
      line += " -DAGRUM_PYTHON_SABI=OFF"
    else:
      line += " -DAGRUM_PYTHON_SABI=ON"

    if self.current["no_gil"]:
      line += " -DAGRUM_PYTHON_NOGIL=ON"
    else:
      line += " -DAGRUM_PYTHON_NOGIL=OFF"

    line += f' -DPython_EXECUTABLE="{self.current["python3target"]}"'

    match self.current["compiler"]:
      case "mvsc22":
        line += ' -G "Visual Studio 17 2022" -A x64'
      case "mvsc22_32":
        line += ' -G "Visual Studio 17 2022" -A Win32'
      case "mvsc19":
        line += ' -G "Visual Studio 16 2019" -A x64'
      case "mvsc19_32":
        line += ' -G "Visual Studio 16 2019" -A Win32'
      case "mvsc17":
        line += ' -G "Visual Studio 15 2017 Win64"'
      case "mvsc17_32":
        line += ' -G "Visual Studio 15 2017"'
      case "mvsc15":
        line += ' -G "Visual Studio 14 2015 Win64"'
      case "mvsc15_32":
        line += ' -G "Visual Studio 14 2015"'
      case "mingw64":
        line += ' -G "MinGW Makefiles"'
      case "clang":
        if self.current["clangpath"] != "":
          clangp = os.path.join(self.current["clangpath"], "clang")
        else:
          clangp = "clang"
        line += f" -DCMAKE_C_COMPILER={clangp} -DCMAKE_CXX_COMPILER={clangp}++"
      case _:  # gcc
        if self.current["gccpath"] != "":
          gccp = os.path.join(self.current["gccpath"], "g")
        else:
          gccp = "g"
        line += f" -DCMAKE_C_COMPILER={gccp}cc -DCMAKE_CXX_COMPILER={gccp}++"

    if self.current["threads"] == "omp":
      line += " -DCMAKE_GUM_THREADS=omp"
    else:
      line += " -DCMAKE_GUM_THREADS=stl"

    if self.current["profiling"]:
      line += " -DUSE_PROFILE=ON"
    else:
      line += " -DUSE_PROFILE=OFF"

    return line.replace("\\", "/")  # for windows compatibility

  def build_makeForMsBuildSystem(self) -> str:
    line = cfg.msbuild
    match self.current["action"]:
      case "test":
        line += f' agrum.sln /t:_pyagrum /p:Configuration="{self.current["mode"]}"'
      case "install":
        line += f' INSTALL.vcxproj /p:Configuration="{self.current["mode"]}"'
      case "lib":
        line += f' INSTALL.vcxproj /p:Configuration="{self.current["mode"]}"'
      case _:
        critic(
          f"Action [{self.current['action']}] not treated for target [pyAgrum] for now in this compiler weird world."
        )

    line += f" /p:BuildInParallel=true /maxcpucount:{self.current['jobs']}"
    return line

  def build_makeForMakeSystem(self) -> str:
    line = cfg.make

    match self.current["action"]:
      case "test":
        pass
      case "install":
        line += " install"
      case "uninstall":
        line += " uninstall"
      case "lib":
        pass  # nothing to do
      case "doc":
        line += " doc"
      case _:
        critic(f"Action [{self.current['action']}] not treated for now")

    line += f" -j {self.current['jobs']}"

    line += " -C wrappers/pyagrum"

    return line

  def build_make(self):
    if platform.system() == "Windows" and self.current["compiler"] != "mingw64":
      return self.build_makeForMsBuildSystem()
    else:
      return self.build_makeForMakeSystem()

  def build_post(self) -> str:
    line = ""
    if self.current["action"] == "test":
      gumTest = "gumTest.py " + self.current["mode"]

      if self.current["tests"] not in {"", "all"}:
        gumTest += " -m quick -t " + self.current["tests"]
      else:
        # quick_specifictest
        if self.current["modules"].startswith("quick"):
          gumTest += " -m " + self.current["modules"].lower() + " -t all"
        else:  # "all"
          gumTest += " -m all  -t all"

      line = f"{cfg.python} ../../../wrappers/pyagrum/testunits/{gumTest}"

    return line

  def build(self):
    prefix = "build ⮕ " + self.current["mode"] + " ⮕ "
    self.run_start(prefix)

    safe_cd(self.current, "build")
    safe_cd(self.current, "pyAgrum")
    safe_cd(self.current, cfg.buildPath[self.current["mode"]])

    err = False
    gc = gm = gb = 0
    t0 = time.time()
    if self.current["build"] == "all":
      self.run_start(prefix + "cmake")
      cmake_cde = self.build_cmake()
      err = err or 0 < self.execFromLine(cmake_cde, checkRC=False)
    t1 = time.time()
    if self.current["build"] != "no-make":
      self.run_start(prefix + "make")
      make_cde = self.build_make()
      err = err or 0 < self.execFromLine(make_cde, checkRC=False)
    t2 = time.time()
    self.run_start(prefix + "post")
    post_cde = self.build_post()
    err = err or 0 < self.execFromLine(post_cde, checkRC=False, bufferized=False)
    t3 = time.time()

    gc = t1 - t0
    gm = t2 - t1
    gb = t3 - t2

    safe_cd(self.current, "..")
    safe_cd(self.current, "..")
    safe_cd(self.current, "..")

    self.conclude(gc, gm, gb)
    if not err:
      self.run_done(prefix)
    else:
      self.run_failed(prefix)

    return not err
