#/bin/python3

import sys
import re
from os.path import join, isfile
from os import listdir
from subprocess import call

def is_wheel_file(wheel_dir, f):
  return isfile(join(wheel_dir, f)) and f.endswith('.whl')

def getWheel(wheel_dir):
  onlyfiles = [f for f in listdir(wheel_dir) if is_wheel_file(wheel_dir, f)]
  if len(onlyfiles) != 1:
    print('Did not found a single wheel in {0}'.format(wheel_dir))
    sys.exit(2)
  return join(wheel_dir, onlyfiles[0])

if __name__ == '__main__':
  if len(sys.argv) != 2:
    print("Please indicate the dir where to look for a wheel file.")
    sys.exit(1)
  wheel = getWheel(sys.argv[1])
  call(["pip", "uninstall", "-y", "pyagrum"])
  call(["pip", "uninstall", "-y", "pyagrum-nightly"])
  call(["pip", "install", wheel])
