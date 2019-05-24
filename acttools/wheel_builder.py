#!/usr/bin/python
# -*- coding: utf-8 -*-# ***************************************************************************
# *   Copyright (C) 2015 by Pierre-Henri WUILLEMIN                          *
# *   {prenom.nom}_at_lip6.fr                                               *
# *                                                                         *
# *   "act" is free software; you can redistribute it and/or modify         *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
# **************************************************************************
import sys
import fileinput
import warnings
import re
import hashlib
import zipfile
import platform
import os

from shutil import move, rmtree

from tempfile import mkdtemp, mkstemp

from datetime import datetime

from os.path import isfile, isdir, join, relpath
from os import fdopen, remove, rename, listdir, walk

from subprocess import check_call, CalledProcessError, PIPE, Popen, STDOUT

from .utils import notif, warn, critic

from .configuration import cfg

FOUND_WHEEL=True

try:
  import wheel.pep425tags as pep425
except ImportError:
  FOUND_WHEEL=False

def wheel(current):
  """If the current Python version used differs from the one asked, fork into
  the proper Python interpreter."""
  if FOUND_WHEEL:
    this = sys.version_info[0]
    target = current["python"]
    if str(this) != str(target):
      warn("Cannot build pyAgrum's wheel for Python{0} when invoking act with Python{1}.".format(target, this))
      critic("Please call act with Python{0} to build pyAgrum under Python{0}.".format(target))

    go(current)
  else:
    critic("Please install package wheel to build wheels using act (pip install wheel).")

def nightly_wheel(current):
  """If the current Python version used differs from the one asked, fork into
  the proper Python interpreter."""
  if FOUND_WHEEL:
    this = sys.version_info[0]
    target = current["python"]
    if str(this) != str(target):
      warn("Cannot build pyAgrum's wheel for Python{0} when invoking act with Python{1}.".format(target, this))
      critic("Please call act with Python{0} to build pyAgrum under Python{0}.".format(target))

    go(current,True)
  else:
    critic("Please install package wheel to build wheels using act (pip install wheel).")

def go(current, nightly=False):
  """Get a temporary directory to build the wheel and cal sequentially all steps
  to build the wheel."""
  tmp = mkdtemp(prefix='act')
  notif('Building wheel in {0}'.format(tmp))
  try:
    prepare(current, tmp,nightly)
    notif("Finished building pyAgrum.")
    install_dir, version = build_wheel(tmp,nightly)
    notif("Finished building wheel directory.")
    zip_file = zip_wheel(tmp, install_dir, version,nightly)
    notif("Finished zipping wheel.")
    move(join(tmp, zip_file), join(current['destination'], zip_file))
    notif("Wheel moved to: {0}.".format(join(current['destination'], zip_file)))
  except CalledProcessError as err:
    critic("Failed building pyAgrum", rc=err.returncode)
  finally:
    rmtree(tmp, True)

def prepare(current, tmp, nightly=False):
  """Prepare step for building the wheel: builds and install pyAgrum in the temporary
  directory and check that this script was called with the same version of Python used
  to build pyAgrum."""
  version = sys.version_info
  this_version = "{0}.{1}.{2}".format(version[0], version[1], version[2])
  gum_version = install_pyAgrum(current, tmp)
  if gum_version.count('.') == 1:
    this_version = "{0}.{1}".format(version[0], version[1])
  if this_version != gum_version:
    warn("You MUST build wheel with the same Python version used to build pyAgrum.")
    warn("Python version used to build the wheel: {0}".format(this_version))
    critic("Python version used to build pyAgrum:   {0}".format(gum_version))

def safe_windows_path(path):
  return path.replace('\\', '/')

def install_pyAgrum(current, tmp, nightly=False):
  """Instals pyAgrum in tmp and return the Python version used to build it."""
  targets = 'install release pyAgrum'
  version = sys.version_info[0]
  options = '--no-fun --withoutSQL -m all -d "{0}" --python={1}'.format(safe_windows_path(tmp), version)
  if platform.system() == "Windows":
    cmd = "python"
    if current['mvsc']:
      options = "{0} --mvsc".format(options)
    elif current['mvsc32']:
      options = "{0} --mvsc32".format(options)
    elif current['mvsc17']:
      options = "{0} --mvsc17".format(options)
    elif current['mvsc17_32']:
      options = "{0} --mvsc17_32".format(options)
    elif current['mingw64']:
      options = "{0} --mingw64".format(options)
  else:
    cmd = sys.executable
  cmd = '{0} act {1} {2}'.format(cmd, targets, options)
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
      m = re.match('^-- python version : ([23]\.[0-9]+(\.[0-9]+)*).*$', line.decode(encoding))
    except UnicodeDecodeError:
      # Windows may use latin-1 without saying it
      m = re.match('^-- python version : ([23]\.[0-9]+(\.[0-9]+)*).*$', line.decode('latin-1'))
    if m:
      version = m.group(1)
  if version == None:
    major = sys.version_info[0]
    minor = sys.version_info[1]
    micro = sys.version_info[2]
    version = "{0}.{1}.{2}".format(major, minor, micro)
    notif("Could not find Python version, opting for current Python version: {0})".format(version))
  return version

def build_wheel(tmp,nightly=False):
  """Update the WHEEL file with the proper Python version, remove unnecessary
  files and generated the RECORD file. Returns the root of the wheel's
  directory."""
  install_dir = get_base_dir(tmp)
  version = get_pyAgrum_version(install_dir)
  dist_info_dir = "pyAgrum-{0}.dist-info".format(version)
  commit_id = os.popen('git rev-parse --short HEAD').split("\n")[0]

  if(nightly):
    dist_info_dir = "pyAgrum_nightly-{0}.dev{1}_{2}.dist-info".format(version,datetime.today().strftime('%Y%m%d'),commit_id)

  dist_info = join(install_dir, dist_info_dir)

  if(nightly):
    rename(join(install_dir,"pyAgrum-{0}.dist-info".format(version)),join(install_dir,"pyAgrum_nightly-{0}.dev{1}_{2}.dist-info".format(version,datetime.today().strftime('%Y%m%d'),commit_id)))

  update_wheel_file(dist_info)
  clean_up(install_dir)
  write_record_file(install_dir, version, nightly)

  if(nightly):
    update_metadata(join(install_dir,dist_info),version)

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
    return join(tmp, "lib", "python{0}.{1}".format(major,minor), "site-packages")

def get_pyAgrum_version(path):
  """Look up the egg-file in the directory path generated by act when
  installing pyAgrum to get pyAgrum's version."""
  pattern = '^pyAgrum-([.0-9]+).*$'
  try:
    files = [f for f in listdir(path) if isfile(join(path,f))]
    for f in files:
      m = re.match(pattern, f)
      if m != None:
        return m.group(1)
  except :
    warn("Error while accessing to path {0}".format(path))
  warn("Could not retrieve pyAgrum version.")
  return ""

def update_wheel_file(dist_info):
  """Adds proper tags using wheel's package implementation of PEP427."""
  path = join(dist_info, "WHEEL")
  tags = get_tags()
  act_version = cfg.__version
  lines = []
  try :
    with open(path) as f:
      lines = [ l.replace("#PYAGRUM_WHEEL_TAGS#", tags).replace("#ACT_VERSION#", act_version) for l in f.readlines() ]
    with open(path, "wt") as f:
      for line in lines:
        f.write(line)
  except:
    critic("Could not update WHEEL file: {0}".format(path))

def get_tags():
  """Get proper tags using wheel's package implementation of PEP427."""
  impl = pep425.get_abbr_impl() + pep425.get_impl_ver()
  abi = pep425.get_abi_tag()
  arch = pep425.get_platform()
  if arch == "linux_x86_64":
    arch = 'manylinux1_x86_64'
  elif arch == "linux_i686":
    arch = 'manylinux1_i686'
  tags = '{0}-{1}-{2}'.format(impl, abi, arch)
  return tags

def clean_up(install_dir):
  """Remone unescessary files in isntall_dir (for now, only th egg-info
  file)."""
  filelist = [ f for f in listdir(install_dir) if f.endswith("egg-info") ]
  for f in filelist:
    try:
      remove(join(install_dir, f))
    except:
      warn("Could not remove dir: {0}".format(join(install_dir, f)))

def write_record_file(install_dir, version, nightly=False):
  """Writes the record file."""
  files_hash = []
  for root, dirs, files in walk(install_dir):
    for f in files:
      try:
        path = join(root, f)
        sha = sha256_checksum(path)
        path = path[len(install_dir):]
        files_hash.append("{0},sha256={1}\n".format(path, sha))
      except:
        critic("Could not compute sha256 for file: {0}".format(join(root, f)))
  try:
    if(nightly):
      commit_id = os.popen('git rev-parse --short HEAD').split("\n")[0]
      dist_info_dir = "pyAgrum_nightly-{0}.dev{1}_{2}.dist-info".format(version,datetime.today().strftime('%Y%m%d'),commit_id)
    else:
      dist_info_dir = "pyAgrum-{0}.dist-info".format(version)

    with open(join(install_dir, dist_info_dir, "RECORD"), 'w') as f:
      for l in files_hash:
        f.write(l)
  except:
    critic("Could not write RECORD file.")

def sha256_checksum(filename, block_size=65536):
  """Returns the sha256 checksum of filename."""
  sha256 = hashlib.sha256()
  with open(filename, 'rb') as f:
    for block in iter(lambda: f.read(block_size), b''):
      sha256.update(block)
  return sha256.hexdigest()

def update_metadata(dist_info_dir,version):
  replace(join(dist_info_dir,'METADATA'),'Name: pyagrum','Name: pyagrum-nightly')
  commit_id = os.popen('git rev-parse --short HEAD').split("\n")[0]
  replace(join(dist_info_dir,'METADATA'),'Version: {0}'.format(version),'Version: {0}.dev{1}_{2}'.format(version,datetime.today().strftime('%Y%m%d'),commit_id))

def replace(file_path, pattern, subst):
  fh, abs_path = mkstemp()
  with fdopen(fh,'w') as new_file:
    with open(file_path) as old_file:
      for line in old_file:
        new_file.write(line.replace(pattern,subst))
  remove(file_path)
  move(abs_path,file_path)

def zip_wheel(tmp, install_dir, version, nightly=False):
  """Zip all files in install_dir."""
  if(nightly):
    commit_id = os.popen('git rev-parse --short HEAD').split("\n")[0]
    zip_name = "pyAgrum_nightly-{0}.dev{1}_{2}-{3}.whl".format(version,datetime.today().strftime('%Y%m%d'),commit_id,get_tags())
  else:
    zip_name = "pyAgrum-{0}-{1}.whl".format(version, get_tags())
  zipf = zipfile.ZipFile(join(tmp, zip_name), 'w', zipfile.ZIP_DEFLATED)
  for root, dirs, files in walk(install_dir):
    for f in files:
      try:
        zipf.write(join(install_dir, root, f), relpath(join(root, f), install_dir))
      except:
        critic("Could not archive file: {0}".format(join(install_dir, root, f)))
  return zip_name
