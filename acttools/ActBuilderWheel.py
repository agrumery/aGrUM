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

import sys
import re
import hashlib
import zipfile
import platform
import sysconfig
import os

from base64 import urlsafe_b64encode
from shutil import move, rmtree
from tempfile import mkdtemp, mkstemp
from datetime import datetime
from os.path import join, relpath
from os import fdopen, remove, rename, listdir, walk
from subprocess import CalledProcessError
from .utils import warn, critic
from .configuration import cfg

from .ActBuilderPyAgrum import ActBuilderPyAgrum

from .utils import *

from .ActBuilder import ActBuilder


def safe_compiler_path(path):
  return path.replace("\\", "/")


def install_pyAgrum(current: dict[str, str | bool], tmp):
  """Instals pyAgrum in tmp and return the Python version used to build it."""
  c = current.copy()
  c["destination"] = tmp
  c["action"] = "install"

  instbuilder = ActBuilderPyAgrum(c)
  if instbuilder.check_consistency():
    if not instbuilder.build():
      error("pyAgrum's installation failed 😭                        ")
      sys.exit(1)
  else:
    error("pyAgrum's installation failed due to consistency checks 😭")
    sys.exit(1)


def build_wheel(tmp, stable_abi_off, minimal_python_api, nightly=False):
  """Update the WHEEL file with the proper Python version, remove unnecessary
  files and generated the RECORD file. Returns the root of the wheel's
  directory."""
  install_dir = get_base_dir(tmp)
  version = get_pyAgrum_version(install_dir)
  dist_info_dir = f"pyagrum-{version}.dist-info"
  commit_time = os.popen('git log -1 --format="%at"').read().split("\n")[0]

  if nightly:
    dist_info_dir = f"pyagrum_nightly-{version}.dev{datetime.today().strftime('%Y%m%d')}{commit_time}.dist-info"

  dist_info = join(install_dir, dist_info_dir)

  if nightly:
    rename(
      join(install_dir, f"pyagrum-{version}.dist-info"),
      join(install_dir, f"pyagrum_nightly-{version}.dev{datetime.today().strftime('%Y%m%d')}{commit_time}.dist-info"),
    )

  update_wheel_file(dist_info, stable_abi_off, minimal_python_api)
  clean_up(install_dir)

  if nightly:
    update_metadata(join(install_dir, dist_info), version)

  write_record_file(install_dir, version, nightly)

  return install_dir, version


def get_base_dir(tmp):
  """Find the proper directory where pyAgrum is installed (normally
  tmp/lib/pythonX.Y/sites-packages where X.Y is the Python version used to
  build pyAgrum)."""
  if platform.system() == "Windows":
    return join(tmp, "lib", "site-packages")
  else:
    major = sys.version_info[0]
    minor = sys.version_info[1]
    return join(tmp, "lib", f"python{major}.{minor}", "site-packages")


def get_pyAgrum_version(path):
  """Look up dist-info in the directory path generated by act when
  installing pyAgrum to get pyAgrum's version."""
  pattern = "^pyagrum-([.0-9]+).dist-info$"
  try:
    files = [f for f in listdir(path)]
    for f in files:
      m = re.match(pattern, f)
      if m != None:
        return m.group(1)
  except:
    warn(f"Error while accessing to path {path}")
  warn("Could not retrieve pyAgrum version.")
  return ""


def update_wheel_file(dist_info, stable_abi_off, minimal_python_api):
  """Adds proper tags using wheel's package implementation of PEP427."""
  path = join(dist_info, "WHEEL")
  tags = get_tags(stable_abi_off, minimal_python_api)
  act_version = cfg.act_version
  lines = []
  try:
    with open(path) as f:
      lines = [l.replace("#PYAGRUM_WHEEL_TAGS#", tags).replace("#ACT_VERSION#", act_version) for l in f.readlines()]
    with open(path, "wt") as f:
      for line in lines:
        f.write(line)
  except:
    critic(f"Could not update WHEEL file: {path}")


def get_tags(stable_abi_off, minimal_python_api):
  """Get proper tags for PEP427 wheel filenames using only the standard library."""
  arch = sysconfig.get_platform().replace("-", "_").replace(".", "_")
  if arch == "linux_x86_64":
    arch = "manylinux2014_x86_64"
  elif arch == "linux_i686":
    arch = "manylinux2014_i686"
  elif arch == "linux_aarch64":
    arch = "manylinux2014_aarch64"

  if stable_abi_off:
    impl = f"cp{sys.version_info.major}{sys.version_info.minor}"
    abi = impl
    tags = f"{impl}-{abi}-{arch}"
  else:
    tags = f"{minimal_python_api}-abi3-{arch}"
  return tags


def clean_up(install_dir):
  """Remove unnecessary files in install_dir (for now, only th egg-info
  file)."""
  filelist = [f for f in listdir(install_dir) if f.endswith("egg-info")]
  for f in filelist:
    try:
      remove(join(install_dir, f))
    except:
      warn(f"Could not remove dir: {join(install_dir, f)}")


def write_record_file(install_dir, version, nightly=False):
  """Writes the record file."""
  files_hash = []
  for root, dirs, files in walk(install_dir):
    for f in files:
      path = join(root, f)
      sha, size = sha256_checksum(path)
      path = path[len(install_dir) + 1 :]
      files_hash.append(f"{path},{sha},{size}\n")
  try:
    if nightly:
      commit_time = os.popen('git log -1 --format="%at"').read().split("\n")[0]
      dist_info_dir = f"pyagrum_nightly-{version}.dev{datetime.today().strftime('%Y%m%d')}{commit_time}.dist-info"

    else:
      dist_info_dir = f"pyagrum-{version}.dist-info"

    with open(join(install_dir, dist_info_dir, "RECORD"), "w") as f:
      for l in files_hash:
        f.write(l)
      f.write(f"{join(dist_info_dir, 'RECORD')},,")
  except:
    critic("Could not write RECORD file.")


def sha256_checksum(file_path, block_size=65536):
  """Returns the sha256 checksum of file."""
  try:
    h = hashlib.sha256()
    length = 0
    with open(file_path, "rb") as f:
      for block in iter(lambda: f.read(block_size), b""):
        h.update(block)
        length += len(block)

    digest = "sha256=" + urlsafe_b64encode(h.digest()).decode("latin1").rstrip("=")
    return (digest, str(length))
  except:
    critic(f"Could not compute sha256 for file: {file_path}")


def update_metadata(dist_info_dir, version):
  replace(join(dist_info_dir, "METADATA"), "Name: pyAgrum", "Name: pyAgrum-nightly")
  commit_time = os.popen('git log -1 --format="%at"').read().split("\n")[0]
  replace(
    join(dist_info_dir, "METADATA"),
    f"Version: {version}",
    f"Version: {version}.dev{datetime.today().strftime('%Y%m%d')}{commit_time}",
  )


def replace(file_path, pattern, subst):
  fh, abs_path = mkstemp()
  with fdopen(fh, "w") as new_file:
    with open(file_path) as old_file:
      for line in old_file:
        new_file.write(line.replace(pattern, subst))
  remove(file_path)
  move(abs_path, file_path)


def zip_wheel(tmp, install_dir, version, stable_abi_off, minimal_python_api, nightly=False):
  """Zip all files in install_dir."""
  tags = get_tags(stable_abi_off, minimal_python_api)
  if nightly:
    commit_time = os.popen('git log -1 --format="%at"').read().split("\n")[0]
    zip_name = f"pyagrum_nightly-{version}.dev{datetime.today().strftime('%Y%m%d')}{commit_time}-{tags}.whl"
  else:
    zip_name = f"pyagrum-{version}-{tags}.whl"

  zipf = zipfile.ZipFile(join(tmp, zip_name), "w", zipfile.ZIP_DEFLATED)
  for root, dirs, files in walk(install_dir):
    for f in files:
      try:
        zipf.write(join(install_dir, root, f), relpath(join(root, f), install_dir))
      except Exception as e:
        if isinstance(e, zipfile.BadZipFile):
          critic(f"Bad zip file: {join(install_dir, root, f)}")
        elif "Permission denied" in str(e):
          critic(f"Permission denied for file: {join(install_dir, root, f)}")
        else:
          critic(f"Could not archive file: {join(install_dir, root, f)} : {e}")

  return zip_name


class ActBuilderWheel(ActBuilder):
  def __init__(self, current: dict[str, str | bool]):
    super().__init__(current)
    self.wheel_path = None

  def check_consistency(self):
    version = sys.version_info
    gum_py_version = cfg.python_version
    if gum_py_version.count(".") == 1:
      this_py_version = f"{version[0]}.{version[1]}"
    else:
      this_py_version = f"{version[0]}.{version[1]}.{version[2]}"
    if this_py_version != gum_py_version:
      warn(f"Python version used to build the wheel: {this_py_version}")
      warn(f"Python version used to build pyAgrum  : {gum_py_version}")
      error("You MUST build wheel with the same Python version used to build pyAgrum.")
      return False
    return True

  def build(self) -> bool:
    zippath = None
    self.run_start()
    nightly = self.current.get("action") == "nightly_wheel"

    tmp = mkdtemp(prefix="act")
    self.run_start(f"Building wheel in {tmp}")

    try:
      self.run_start("Building pyAgrum")
      install_pyAgrum(self.current, tmp)
      self.run_done("Finished building pyAgrum.")

      install_dir, version = build_wheel(tmp, self.current["stable_abi_off"], cfg.minimal_python_api, nightly)
      self.run_done("Finished building wheel directory.")
      zip_file = zip_wheel(tmp, install_dir, version, self.current["stable_abi_off"], cfg.minimal_python_api, nightly)
      self.run_done("Finished zipping wheel.")
      zippath = join(self.current["destination"], zip_file)
      move(join(tmp, zip_file), zippath)
      self.run_done(f"Wheel moved to: {zippath}.")
    except CalledProcessError as err:
      critic("Failed building pyAgrum", rc=err.returncode)
    finally:
      rmtree(tmp, True)

    self.run_done()
    if zippath is not None:
      self.wheel_path = zippath

    return True
