# -*- coding: utf-8 -*-
# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2019
# (pierre-henri.wuillemin@lip6.fr)
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.
# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!

"""
configuration tool for pyAgrum
"""
# for python2's compatibility
# for metaclass both in python2 and 3
from __future__ import print_function
from six import with_metaclass
try:
    FileNotFoundError
except NameError:
    FileNotFoundError = IOError
## end for python2's compatibility

from configparser import ConfigParser
import os


class Singleton(type):
  _instances = {}

  def __call__(cls, *args, **kwargs):
    if cls not in cls._instances:
      cls._instances[cls] = super(Singleton, cls).__call__(*args, **kwargs)
    return cls._instances[cls]


class PyAgrumConfiguration(with_metaclass(Singleton)):
  """ PyAgrumConfiguration is a the pyAgrum configuration singleton
  """

  def __init__(self):
    self.__parser = ConfigParser(allow_no_value=False)

    defaultsfn = os.path.dirname(__file__) + "/defaults.ini"
    self.__parser.read(defaultsfn)
    self.__defaults = self.__str__()

  def set(self, section, option, value):
    """set a property in a section

    Arguments:
        section {str} -- the section name (has to exist in defaults)
        option {str} -- the option/property name (has to exist in defaults)
        value {str} -- the value (will be store as string)

    Raises:
        SyntaxError: if the secion name or the property name does not exist
    """
    if section in self.__parser.sections():
      if option in self.__parser[section]:
        self.__parser.set(section, option, str(value))
        return
      else:
        raise SyntaxError("You can not add option '"+section +
                          "," + option + "' in pyAgrum configuration")
    else:
      raise SyntaxError("You can not add section '" +
                        section + "' in pyAgrum configuration")

  def get(self, section, option):
    """ Give the value associated to section.option

    Arguments:
        section {str} -- the section
        option {str} -- the property

    Returns:
        str -- the value (as string)
    """
    return self.__parser.get(section, option)

  def __diff(self):
    mine = self.__parser
    c = ConfigParser()
    c.read_string(self.__defaults)
    def aff_sec(section): return "["+section+"]\n"+"\n".join(
        ["  {} = {}".format(key, mine[section][key]) for key in mine[section].keys() if mine.get(section, key) != c.get(section, key)])
    return "\n".join([sec for sec in [aff_sec(section) for section in mine.sections()] if "=" in sec])

  def save(self):
    """Save the diff with the defaults in 'pyagrum.ini' in the current directory
    """
    with open("pyagrum.ini", "w") as configfile:
      print(self.__diff(), file=configfile)

  def reset(self):
    """ back to defaults
    """
    self.__parser.read_string(self.__defaults)

  def load(self):
    """load pyagrum.ini in the current directory, and change the properties if needed

    Raises:
        FileNotFoundError: if there is no pyagrum.ini in the current directory
    """
    if os.path.isfile("pyagrum.ini"):
      # to force the use of the protected set() method
      c = ConfigParser()
      c.read("pyagrum.ini")
      for section in c.sections():
        if section not in self.__parser.sections():
          print("[pyagrum.ini] Section '{}' does not exist.".format(section))
        for option in c[section]:
          try:
            self.set(section, option, c[section][option])
          except SyntaxError:
            print("[pyagrum.ini] Option '{}.{}' does not exist.".format(section,option))
    else:
      raise FileNotFoundError("No file 'pyagrum.ini' in current directory.")

  def grep(self, search):
    """ grep in the configuration any section or properties matching the argument. If a section match the argume, all the section is displayed.

    Arguments:
        search {str} -- the string to find    
    """
    mine = self.__parser
    lowsearch = search.lower()
    def aff_sec(section, all): return "["+section+"]\n"+"\n".join(
        ["  {} = {}".format(key, mine[section][key]) for key in mine[section].keys() if all or lowsearch in key])
    print("\n".join([sec for sec in [aff_sec(section, lowsearch in section)
                     for section in mine.sections()] if "=" in sec]))

  def diff(self):
    """ print the diff between actual configuration and the defaults. This is what is saved in the file 'pyagrum.ini' by the method `PyAgrumConfiguration.save()`
    """
    print(self.__diff())

  def __str__(self):
    mine = self.__parser
    def aff_sec(section): return "["+section+"]\n"+"\n".join(
        ["  {} = {}".format(key, mine[section][key]) for key in mine[section].keys()])
    return "\n".join([aff_sec(section) for section in mine.sections()])

  def __repr__(self):
    res = self.__diff()
    if "=" in res:
      return res
    else:
      return "# no customized property\n" + self.__str__()

  def __getitem__(self, key):
    return self.get(key[0], key[1])

  def __setitem__(self, key, value):
    return self.set(key[0], key[1], value)

  def __delitem__(self, key):
    raise SyntaxError("No deletion of item in configuration")
