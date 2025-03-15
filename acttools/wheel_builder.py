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

import sys
import fileinput
import warnings
import re
import hashlib
import zipfile
import platform
import os
import time

from base64 import urlsafe_b64encode

from shutil import move, rmtree

from tempfile import mkdtemp, mkstemp

from datetime import datetime

from os.path import isfile, isdir, join, relpath
from os import fdopen, remove, rename, listdir, walk

from subprocess import check_call, CalledProcessError, PIPE, Popen, STDOUT

from .utils import notif, warn, critic

from .configuration import cfg

FOUND_WHEEL = True

try:
  import wheel.bdist_wheel as pep
  from wheel.vendored.packaging import tags as wheel_tags
except ImportError:
  FOUND_WHEEL = False


def wheel(current:dict[str,str]):
  """If the current Python version used differs from the one asked, fork into
  the proper Python interpreter."""
  if FOUND_WHEEL:
    _go_wheel(current)
  else:
    critic("Please install package wheel to build wheels using act (pip install wheel).")


def nightly_wheel(current:dict[str,str]):
  """If the current Python version used differs from the one asked, fork into
  the proper Python interpreter."""
  if FOUND_WHEEL:
    _go_wheel(current, True)
  else:
    critic("Please install package wheel to build wheels using act (pip install wheel).")


def _go_wheel(current:dict[str,str], nightly=False):
  """Get a temporary directory to build the wheel and cal sequentially all steps
  to build the wheel."""
  print(cfg)
  tmp = mkdtemp(prefix='act')
  notif(f'Building wheel in {tmp}')
  try:
    _prepare_wheel(current, tmp, nightly)
    notif("Finished building pyAgrum.")
    install_dir, version = build_wheel(tmp, current['stable_abi_off'], cfg.minimal_python_api, nightly)
    notif("Finished building wheel directory.")
    zip_file = zip_wheel(tmp, install_dir, version, current['stable_abi_off'], cfg.minimal_python_api, nightly)
    notif("Finished zipping wheel.")
    move(join(tmp, zip_file), join(current['destination'], zip_file))
    notif(f"Wheel moved to: {join(current['destination'], zip_file)}.")
  except CalledProcessError as err:
    critic("Failed building pyAgrum", rc=err.returncode)
  finally:
    rmtree(tmp, True)


def _prepare_wheel(current:dict[str,str], tmp, nightly=False):
  """Prepare step for building the wheel: builds and install pyAgrum in the temporary
  directory and check that this script was called with the same version of Python used
  to build pyAgrum."""
  version = sys.version_info
  this_version = f"{version[0]}.{version[1]}.{version[2]}"
  gum_version = install_pyAgrum(current, tmp)
  if gum_version.count('.') == 1:
    this_version = f"{version[0]}.{version[1]}"
  if this_version != gum_version:
    warn("You MUST build wheel with the same Python version used to build pyAgrum.")
    warn(f"Python version used to build the wheel: {this_version}")
    critic(f"Python version used to build pyAgrum:   {gum_version}")


def safe_compiler_path(path):
  return path.replace('\\', '/')


def install_pyAgrum(current:dict[str,str], tmp, nightly=False):
  """Instals pyAgrum in tmp and return the Python version used to build it."""
  targets = 'install release pyAgrum'
  version = sys.version_info[0]
  options = f'--no-fun --withoutSQL -m all -d "{safe_compiler_path(tmp)}"'
  if platform.system() == "Windows":
    cmd = "python"
    options = f"{options} --compiler={current['compiler']}"
  else:
    cmd = sys.executable
  cmd = f'{cmd} act {targets} {options}'
  proc = Popen(cmd, shell=True, stdout=PIPE, stderr=STDOUT)
  out = proc.stdout.readlines()
  return get_python_version(out)

def get_python_version(out):
  """Retrieves the Python version from act's output when building pyAgrum."""
  version = None
  for line in out:
    m = ""
    encoding = sys.stdout.encoding if sys.stdout.encoding else 'utf-8'
    try:
      m = re.match(
          r'^-- python version : ([23]\.[0-9]+(\.[0-9]+)*).*$', line.decode(encoding))
    except UnicodeDecodeError:
      # Windows may use latin-1 without saying it
      m = re.match(
          r'^-- python version : ([23]\.[0-9]+(\.[0-9]+)*).*$', line.decode('latin-1'))
    if m:
      version = m.group(1)
  if version == None:
    major = sys.version_info[0]
    minor = sys.version_info[1]
    micro = sys.version_info[2]
    version = "{0}.{1}.{2}".format(major, minor, micro)
    notif("Could not find Python version, opting for current Python version: {0})".format(
        version))
  return version

def build_wheel(tmp, stable_abi_off, minimal_python_api, nightly=False):
  """Update the WHEEL file with the proper Python version, remove unnecessary
  files and generated the RECORD file. Returns the root of the wheel's
  directory."""
  install_dir = get_base_dir(tmp)
  version = get_pyAgrum_version(install_dir)
  dist_info_dir = f"pyagrum-{version}.dist-info"
  commit_time = os.popen('git log -1 --format="%at"').read().split('\n')[0]

  if (nightly):
    dist_info_dir = f"pyagrum_nightly-{version}.dev{datetime.today().strftime('%Y%m%d')}{commit_time}.dist-info"

  dist_info = join(install_dir, dist_info_dir)

  if (nightly):
    rename(join(install_dir, f"pyagrum-{version}.dist-info"),
           join(install_dir,
                f"pyagrum_nightly-{version}.dev{datetime.today().strftime('%Y%m%d')}{commit_time}.dist-info"))

  update_wheel_file(dist_info, stable_abi_off, minimal_python_api)
  clean_up(install_dir)

  if (nightly):
    update_metadata(join(install_dir, dist_info), version)

  write_record_file(install_dir, version, nightly)

  return install_dir, version


def get_base_dir(tmp):
  """Find the proper directory where pyAgrum is installed (normaly
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
  pattern = '^pyagrum-([.0-9]+).dist-info$'
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
      lines = [l.replace("#PYAGRUM_WHEEL_TAGS#", tags).replace(
        "#ACT_VERSION#", act_version) for l in f.readlines()]
    with open(path, "wt") as f:
      for line in lines:
        f.write(line)
  except:
    critic(f"Could not update WHEEL file: {path}")


def get_tags(stable_abi_off, minimal_python_api):
  """Get proper tags using wheel's package implementation of PEP427."""
  try:
    arch = pep.safer_name(pep.get_platform(None))
  except:
    arch = pep.safer_name(pep.get_platform())
  if arch == "linux_x86_64":
    arch = 'manylinux2014_x86_64'
  elif arch == "linux_i686":
    arch = 'manylinux2014_i686'
  elif arch == "linux_aarch64":
    arch = 'manylinux2014_aarch64'
  if 'macosx' in arch:
    arch = arch.replace('.', '_')

  if stable_abi_off:
    impl = wheel_tags.interpreter_name() + wheel_tags.interpreter_version()
    abi = pep.get_abi_tag()
    tags = f'{impl}-{abi}-{arch}'
  else:
    tags = f'{minimal_python_api}-abi3-{arch}'
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
      path = path[len(install_dir)+1:]
      files_hash.append(f"{path},{sha},{size}\n")
  try:
    if (nightly):
      commit_time = os.popen('git log -1 --format="%at"').read().split('\n')[0]
      dist_info_dir = f"pyagrum_nightly-{version}.dev{datetime.today().strftime('%Y%m%d')}{commit_time}.dist-info"

    else:
      dist_info_dir = f"pyagrum-{version}.dist-info"

    with open(join(install_dir, dist_info_dir, "RECORD"), 'w') as f:
      for l in files_hash:
        f.write(l)
      f.write(f"{join(dist_info_dir,'RECORD')},,")
  except:
    critic("Could not write RECORD file.")

def sha256_checksum(file_path, block_size=65536):
  """Returns the sha256 checksum of file."""
  try:
    h = hashlib.sha256()
    length = 0
    with open(file_path, 'rb') as f:
      for block in iter(lambda: f.read(block_size), b''):
        h.update(block)
        length += len(block)

    digest = 'sha256=' + urlsafe_b64encode(
        h.digest()
    ).decode('latin1').rstrip('=')
    return (digest, str(length))
  except:
    critic(f"Could not compute sha256 for file: {file_path}")

def update_metadata(dist_info_dir, version):
  replace(join(dist_info_dir, 'METADATA'),
          'Name: pyAgrum', 'Name: pyAgrum-nightly')
  commit_time = os.popen('git log -1 --format="%at"').read().split('\n')[0]
  replace(join(dist_info_dir, 'METADATA'), f'Version: {version}',
          f"Version: {version}.dev{datetime.today().strftime('%Y%m%d')}{commit_time}")


def replace(file_path, pattern, subst):
  fh, abs_path = mkstemp()
  with fdopen(fh, 'w') as new_file:
    with open(file_path) as old_file:
      for line in old_file:
        new_file.write(line.replace(pattern, subst))
  remove(file_path)
  move(abs_path, file_path)


def zip_wheel(tmp, install_dir, version, stable_abi_off, minimal_python_api, nightly=False):
  """Zip all files in install_dir."""
  tags = get_tags(stable_abi_off, minimal_python_api)
  if (nightly):
    commit_time = os.popen('git log -1 --format="%at"').read().split('\n')[0]
    zip_name = f"pyagrum_nightly-{version}.dev{datetime.today().strftime('%Y%m%d')}{commit_time}-{tags}.whl"
  else:
    zip_name = f"pyagrum-{version}-{tags}.whl"

  zip_path = join(tmp, zip_name)
  with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
    for root, dirs, files in os.walk(install_dir):
      for f in files:
        try:
          full_path = join(root, f)
          relative_path = relpath(full_path, install_dir)
          
          # Rename pyAgrum to pyagrum in the ZIP structure
          relative_path = relative_path.replace("pyAgrum", "pyagrum")
          
          zipf.write(full_path, relative_path)
        except Exception as e:
          critic(f"Could not archive file: {full_path}. Error: {e}")

    return zip_name