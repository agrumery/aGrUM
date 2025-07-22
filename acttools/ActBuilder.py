############################################################################
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import multiprocessing
import platform
from subprocess import PIPE, Popen, STDOUT

from .configuration import cfg
from .utils import *


class ActBuilder:
  def __init__(self, current: dict[str, str | bool]):
    self.current = current

  def warn(self, msg: str) -> None:
    """
    notif a message like : "{BuilderName}:{STARTED|DONE|FAILED|...}" where msg is one of these keywords.
    Parameters
    ----------
    msg : str
      containt {STARTED|DONE|FAILED|...}
    """
    builder_name = self.__class__.__name__[10:]
    if builder_name.startswith("ActBuilder"):  # as it should
      builder_name = builder_name[10:]
    notif(f"{builder_name} ⌛ [{msg}]", cfg.prefix_line)

  def run_start(self, subaction: str = ""):
    self.warn(f"{subaction} 🚀                    ")

  def run_done(self, subaction: str = ""):
    self.warn(f"{subaction} ✅                         ")

  def run_failed(self, subaction: str = ""):
    self.warn(f"{subaction} ❌                       ")

  def check_consistency(self):
    raise NotImplementedError()

  def run(self):
    raise NotImplementedError()

  def execFromLine(self, cmd: str, silent: bool = False, checkRC: bool = True) -> int:
    if self.current["dry_run"]:
      notif(cmd)
      return 0

    process = Popen(cmd, shell=True, stdout=PIPE, stderr=STDOUT)

    if not silent:
      while True:
        line = process.stdout.readline()
        if line == b"" or process.poll() is not None:
          break
        if line:
          try:
            response = line.decode().strip()
          except UnicodeDecodeError:
            response = line.decode("utf-8", errors="replace").strip()

          notif(response, cfg.prefix_line)

    process.stdout.close()
    return_code = process.wait()

    if checkRC and return_code > 0:
      error(f"Received error {return_code} with command [{cmd}]")
    return return_code

  def check_compiler_and_maker(self) -> bool:
    if platform.system() == "Windows":
      if self.current["compiler"] in ["clang", "gcc"]:
        error(f"{self.current['compiler']} forbidden : clang or gcc only configured for non-windows system.")
        return False
      else:
        if self.current["compiler"] != "mingw64":  # mvsc
          if not self.current["static_lib"]:
            notif("Static library forced with MSVC compiler.")
            self.current["static_lib"] = True
    else:
      if self.current["compiler"] not in ["clang", "gcc"]:
        error(f"{self.current['compiler']} forbidden : only clang or gcc for non-windows system.")
        return False

    if self.current["compiler"] in {"clang", "gcc", "mingw64"}:
      if cfg.make is None:
        error("Make not found")
        return False
    else:
      if cfg.msbuild is None:
        error("MsBuild not found")
        return False

    if self.current["mode"].lower() not in {"release", "debug"}:
      warn(f"Mode [{self.current['mode']}] not supported. Using 'release' as default.")
      self.current["mode"] = "Release"
    elif self.current["mode"].lower() == "debug":
      self.current["mode"] = "Debug"
    else:
      self.current["mode"] = "Release"

    self.current["jobs"] = self.check_nbr_of_jobs()
    notif(f"Using [{self.current['jobs']}] jobs for building.")

    return True

  def check_nbr_of_jobs(self) -> str:
    """
    Check the number of jobs to run based on the job parameter.
    If error, return a default value based on the number of available processors.
    """
    jobrequest = self.current["jobs"]
    # number of jobs
    nbrProc = multiprocessing.cpu_count()
    if nbrProc == 1:
      return "1"
    else:
      if jobrequest == "except1":
        return str(nbrProc - 1)
      elif jobrequest == "half":
        return str(int(nbrProc / 2))  # >=1
      elif jobrequest == "halfexcept1":
        if nbrProc <= 3:
          return "1"
        else:
          return str(int(nbrProc / 2) - 1)
      elif jobrequest == "all":
        return str(nbrProc)
      else:
        try:
          nbrJob = int(jobrequest)
          if nbrJob < 1:
            nbrJob = 1
          if nbrJob > nbrProc:
            return str(nbrProc)
          else:
            return str(nbrJob)
        except ValueError:
          notif(f"Option '-j {jobrequest}' is invalid. Using '-j halfexcept1'.")
          self.current["jobs"] = "halfexcept1"
          return self.check_nbr_of_jobs()

  def conclude(self, tc: int = 0, tm: int = 0, tp: int = 0):
    MINTIME = 0.01
    if tc < MINTIME and tm < MINTIME and tp < MINTIME:
      warn("No build time to report.")
    else:
      notif("┏─────────┳──────────────┓")
      if tc > MINTIME:
        notif(f"┃ cmake   ┃ ⏱️ {tc:9.2f}s ┃")
      if tm > MINTIME:
        notif(f"┃ make    ┃ ⏱️ {tm:9.2f}s ┃")
      if tp > MINTIME:
        notif(f"┃ post    ┃ ⏱️ {tp:9.2f}s ┃")
      notif("┗─────────┻──────────────┛")
