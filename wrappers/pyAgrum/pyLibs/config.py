# (c) Copyright 2022-2024 by Pierre-Henri Wuillemin(@LIP6)
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
from configparser import ConfigParser
import os


class GumSingleton(type):
  _instances = {}

  def __call__(cls, *args, **kwargs):
    if cls not in cls._instances:
      cls._instances[cls] = super(GumSingleton, cls).__call__(*args, **kwargs)
    return cls._instances[cls]


class PyAgrumConfiguration(metaclass=GumSingleton):
  """ PyAgrumConfiguration is a the pyAgrum configuration singleton.  The configuration is build
  as a classical ConfigParser with read-only structure. Then a value is adressable using a double key: ``[section,key]``.

  See `this notebook <https://lip6.fr/Pierre-Henri.Wuillemin/aGrUM/docs/last/notebooks/configForPyAgrum.ipynb.html>`_.

  Examples
  --------
    >>> gum.config['dynamicBN','default_graph_size']=10
    >>> gum.config['dynamicBN','default_graph_size']
    "10"
  """

  def _check_int(self, s):
    if s[0] in ('-', '+'):
      return s[1:].isdigit()
    return s.isdigit()

  def _check_float(self, s):
    t = s.split(".")
    if len(t) == 1:
      return self._check_int(t[0])
    elif len(t) == 2:
      return self._check_int(t[0]) and t[1].isdigit()
    else:
      return False

  def _check_bool(self, s):
    return self._check_bool_true(s) or self._check_bool_false(s)

  def _check_bool_true(self, s):
    return s.upper() in ["TRUE", "1", "ON", "YES"]

  def _check_bool_false(self, s):
    return s.upper() in ["FALSE", "0", "OFF", "NO"]

  class _Casterization:
    def __init__(self, container):
      self.container = container

  class _CastAsInt(_Casterization):
    def __getitem__(self, x):
      return int(self.container[x])

    def __setitem__(self, x, v):
      s = str(v)
      if self.container._check_int(s):
        self.container[x] = s
      else:
        raise ValueError(f"'{s}' must contain an int.")

  class _CastAsFloat(_Casterization):
    def __getitem__(self, x):
      return float(self.container[x])

    def __setitem__(self, x, v):
      s = str(v)
      if self.container._check_float(s):
        self.container[x] = s
      else:
        raise ValueError(f"'{s}' must contain a float.")

  class _CastAsBool(_Casterization):
    def __getitem__(self, x):
      return self.container._check_bool_true(self.container[x])

    def __setitem__(self, x, v):
      s = str(v)
      if self.container._check_bool(s):
        self.container[x] = s
      else:
        raise ValueError(f"'{s}' must contain a boolean (False/True, 0/1, Off/On).")

  def __init__(self):
    self.__parser = ConfigParser(allow_no_value=False)

    defaultsfn = os.path.dirname(__file__) + "/defaults.ini"
    self.__parser.read(defaultsfn)
    self.__defaults = self.__str__()
    self.__hooks = []
    self.__stacks = []

    self.asInt = self._CastAsInt(self)
    self.asFloat = self._CastAsFloat(self)
    self.asBool = self._CastAsBool(self)

  def add_hook(self, fn):
    self.__hooks.append(fn)

  def run_hooks(self):
    for fn in self.__hooks:
      fn()

  def set(self, section, option, value, no_hook=False):
    """set a property in a section. Preferably use ``__getitem__`` and ``__setitem__``.

    Examples
    --------
      >>> gum.config['dynamicBN','default_graph_size']=10
      >>> gum.config['dynamicBN','default_graph_size']
      "10"

    Arguments:
        section {str} -- the section name (has to exist in defaults)
        option {str} -- the option/property name (has to exist in defaults)
        value {str} -- the value (will be store as string)
        no_hook {bool} -- (optional) should this call trigger the hooks ?

    Raises:
        SyntaxError: if the secion name or the property name does not exist
    """
    if section in self.__parser.sections():
      if option in self.__parser[section]:
        self.__parser.set(section, option, str(value))
        if not no_hook:
          self.run_hooks()
      else:
        raise SyntaxError(f"Key '{section},{option}' unknown in pyAgrum configuration.")
    else:
      raise SyntaxError(f"Section '{section}' unknown in pyAgrum configuration.")

  def get(self, section, option):
    """ Give the value associated to section.option. Preferably use ``__getitem__`` and ``__setitem__``.

    Examples
    --------
      >>> gum.config['dynamicBN','default_graph_size']=10
      >>> gum.config['dynamicBN','default_graph_size']
      "10"

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

    def aff_sec(section): return "[" + section + "]\n" + "\n".join(
      [f"  {key} = {mine[section][key]}" for key in mine[section].keys() if
       mine.get(section, key) != c.get(section, key)])

    return "\n".join([sec for sec in [aff_sec(section) for section in mine.sections()] if "=" in sec])

  def save(self):
    """Save the diff with the defaults in ``pyagrum.ini`` in the current directory
    """
    with open("pyagrum.ini", "w") as configfile:
      print(self.__diff(), file=configfile)

  def reset(self):
    """ back to defaults
    """
    self.__parser.read_string(self.__defaults)
    self.run_hooks()

  def load(self):
    """load pyagrum.ini in the current directory, and change the properties if needed

    Raises:
        FileNotFoundError: if there is no pyagrum.ini in the current directory
    """
    if os.path.isfile("pyagrum.ini"):
      # to force to use the protected set() method
      c = ConfigParser()
      c.read("pyagrum.ini")
      error_found = False
      for section in c.sections():
        if section not in self.__parser.sections():
          error_found = True
          print(f"[pyagrum.ini] Section '{section}' does not exist.")
        for option in c[section]:
          try:
            self.set(section, option, c[section][option], no_hook=True)
          except SyntaxError:
            error_found = True
            print(f"[pyagrum.ini] Option '{section}.{option}' does not exist.")
        self.run_hooks()
      if error_found:
        self.save()
    else:
      raise FileNotFoundError("No file 'pyagrum.ini' in current directory.")

  def grep(self, search):
    """ grep in the configuration any section or properties matching the argument. If a section match the argume, all the section is displayed.

    Arguments:
        search {str} -- the string to find
    """
    mine = self.__parser
    lowsearch = search.lower()

    def aff_sec(section, all): return "[" + section + "]\n" + "\n".join(
      [f"  {key} = {mine[section][key]}" for key in mine[section].keys() if all or lowsearch in key])

    print("\n".join([sec for sec in [aff_sec(section, lowsearch in section)
                                     for section in mine.sections()] if "=" in sec]))

  def diff(self):
    """ print the diff between actual configuration and the defaults. This is what is saved in the file ``pyagrum.ini`` by the method `PyAgrumConfiguration.save()`
    """
    print(self.__diff())

  def __str__(self):
    mine = self.__parser

    def aff_sec(section): return "[" + section + "]\n" + "\n".join(
      [f"  {key} = {mine[section][key]}" for key in mine[section].keys()])

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

  def pop(self):
    """
    Pop the last config from the stack and set it as the current configuration
    """
    if len(self.__stacks) > 0:
      self.__parser.read_string(self.__stacks.pop())
      self.run_hooks()
    else:
      raise IndexError("[pyAgrum] No configuration to pop")

  def push(self):
    """
    Push the current configuration in the stack
    """
    self.__stacks.append(str(self))
