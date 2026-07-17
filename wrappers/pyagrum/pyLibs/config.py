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

"""
configuration tool for pyAgrum
"""

import os
import re
import warnings
from configparser import ConfigParser
from typing import Any, NamedTuple


class GumSingleton(type):
  _instances: dict[type, Any] = {}

  def __call__(cls, *args, **kwargs):
    if cls not in cls._instances:
      cls._instances[cls] = super().__call__(*args, **kwargs)
    return cls._instances[cls]


class ConfigMetadata(NamedTuple):
  """Metadata attached to a configuration key by a '###' line in ``defaults.ini``."""

  kind: str  # "int" | "float" | "bool" | "str" | "enum" | "deprecated"
  values: tuple[str, ...]  # allowed values when kind == "enum"
  doc: str  # one-line documentation
  target: tuple[str, str] | None  # replacement (section, option) when kind == "deprecated"


# a metadata line is exactly three '#' (not four or more) followed by 'type :: documentation'
_RE_META = re.compile(r"^\s*###(?!#)\s*(?P<payload>.*)$")
_RE_SECTION = re.compile(r"^\s*\[(?P<name>[^\]]+)\]\s*$")
_RE_KEY = re.compile(r"^\s*(?P<key>[A-Za-z_][A-Za-z0-9_]*)\s*[=:]")
_RE_DEPRECATED = re.compile(r"^deprecated\s*->\s*(?P<section>[^.\s]+)\.(?P<option>\S+)$")
_BASIC_KINDS = ("int", "float", "bool", "str")

_TRUE_VALUES = frozenset({"TRUE", "1", "ON", "YES"})
_FALSE_VALUES = frozenset({"FALSE", "0", "OFF", "NO"})
_BOOL_VALUES = _TRUE_VALUES | _FALSE_VALUES


def _parseMetaSpec(spec: str, doc: str) -> ConfigMetadata:
  match = _RE_DEPRECATED.match(spec)
  if match is not None:
    return ConfigMetadata("deprecated", (), doc, (match["section"], match["option"].lower()))
  if spec in _BASIC_KINDS:
    return ConfigMetadata(spec, (), doc, None)
  if "|" in spec:
    values = tuple(v.strip() for v in spec.split("|"))
    if all(values):
      return ConfigMetadata("enum", values, doc, None)
  raise SyntaxError(f"'{spec}' is not a valid type (int, float, bool, str, v1|v2|..., deprecated -> section.key)")


def _parseSchema(text: str) -> dict[tuple[str, str], ConfigMetadata]:
  """Parse the '###' metadata lines of a defaults.ini content.

  A metadata line must immediately precede the ``key = value`` line it documents.
  Any malformed metadata line raises a SyntaxError (fail-fast: defaults.ini is a
  developer-maintained file).
  """
  schema: dict[tuple[str, str], ConfigMetadata] = {}
  section = None
  pending = None
  lines = text.splitlines()

  def _err(numline, msg):
    raise SyntaxError(f"[defaults.ini:{numline}] {msg}")

  for numline, line in enumerate(lines, start=1):
    key_match = _RE_KEY.match(line)
    if pending is not None and key_match is None:
      _err(numline, "metadata line must be immediately followed by a 'key = value' line")

    match = _RE_META.match(line)
    if match is not None:
      payload = match["payload"]
      if "::" not in payload:
        _err(numline, "metadata line must follow the syntax '### type :: documentation'")
      spec, doc = (part.strip() for part in payload.split("::", 1))
      if doc == "":
        _err(numline, "missing documentation in metadata line")
      try:
        pending = _parseMetaSpec(spec, doc)
      except SyntaxError as e:
        _err(numline, str(e))
      continue

    match = _RE_SECTION.match(line)
    if match is not None:
      section = match["name"]
      continue

    if key_match is not None and pending is not None:
      if section is None:
        _err(numline, "metadata outside of any section")
      key = (section, key_match["key"].lower())
      if key in schema:
        _err(numline, f"duplicate metadata for '{section}.{key_match['key']}'")
      schema[key] = pending
      pending = None

  if pending is not None:
    _err(len(lines), "metadata line at end of file")

  return schema


class PyAgrumConfiguration(metaclass=GumSingleton):
  """PyAgrumConfiguration is a the pyAgrum configuration singleton.  The configuration is build
  as a classical ConfigParser with read-only structure. Then a value is adressable using a double key: ``[section,key]``.

  Each key is described in ``defaults.ini`` by a metadata line ``### type :: documentation`` where type is
  ``int``, ``float``, ``bool``, ``str``, an enumeration ``v1|v2|...`` or a deprecation ``deprecated -> section.key``.
  Setting a value that does not respect the declared type raises a ``ValueError``. Accessing a deprecated key
  emits a ``DeprecationWarning`` and transparently uses the replacement key. While values are always stored as
  strings, ``config.typed[section, key]`` gives the value converted according to the declared type. Boolean and
  enumerated values are canonicalized when set: any accepted boolean spelling (``yes``/``on``/``1``/...) is
  stored as ``True`` or ``False``, an enumerated value is stored with the casing declared in ``defaults.ini``.

  See `this notebook <https://lip6.fr/Pierre-Henri.Wuillemin/aGrUM/docs/last/notebooks/configForPyAgrum.ipynb.html>`_.

  Examples
  --------
    >>> import pyagrum
    >>> pyagrum.config["dynamicBN", "default_graph_size"] = 10
    >>> pyagrum.config["dynamicBN", "default_graph_size"]
    "10"
  """

  @staticmethod
  def _check_int(s):
    try:
      int(s)
      return True
    except ValueError:
      return False

  @staticmethod
  def _check_float(s):
    try:
      float(s)
      return True
    except ValueError:
      return False

  @staticmethod
  def _check_bool(s):
    return s.upper() in _BOOL_VALUES

  @staticmethod
  def _check_bool_true(s):
    return s.upper() in _TRUE_VALUES

  class _CastAsTyped:
    def __init__(self, container):
      self.container = container

    def __getitem__(self, x):
      return self.container.getTyped(x[0], x[1])

    def __setitem__(self, x, v):
      self.container[x] = v

  class _DeprecatedCast(_CastAsTyped):
    """Historical accessors ``asInt``, ``asFloat``, ``asBool``: deprecated since 3.1.0, they delegate to ``typed``."""

    def __init__(self, container, name):
      super().__init__(container)
      self.name = name

    def _warn(self):
      warnings.warn(
        f"[pyAgrum] pyagrum.config.{self.name} is deprecated since 3.1.0, use pyagrum.config.typed instead.",
        DeprecationWarning,
        stacklevel=3,
      )

    def __getitem__(self, x):
      self._warn()
      return super().__getitem__(x)

    def __setitem__(self, x, v):
      self._warn()
      super().__setitem__(x, v)

  def __init__(self, defaultsfn=None):
    self.__parser = ConfigParser(allow_no_value=False)

    if defaultsfn is None:
      defaultsfn = os.path.join(os.path.dirname(__file__), "defaults.ini")
    with open(defaultsfn, encoding="utf-8") as f:
      text = f.read()
    self.__parser.read_string(text)
    self.__schema = _parseSchema(text)
    self.__checkSchema()

    self.__defaults = self.__snapshot()
    self.__hooks = []
    self.__stacks = []

    self.asInt = self._DeprecatedCast(self, "asInt")
    self.asFloat = self._DeprecatedCast(self, "asFloat")
    self.asBool = self._DeprecatedCast(self, "asBool")
    self.typed = self._CastAsTyped(self)

  def __checkSchema(self):
    """Fail-fast consistency checks between the schema and the default values."""
    for (section, option), meta in self.__schema.items():
      if meta.kind == "deprecated":
        tsec, topt = meta.target
        if not self.__parser.has_section(tsec) or not self.__parser.has_option(tsec, topt):
          raise SyntaxError(
            f"[defaults.ini] deprecated key '{section}.{option}' redirects to unknown key '{tsec}.{topt}'."
          )
        tmeta = self.__schema.get((tsec, topt))
        if tmeta is not None and tmeta.kind == "deprecated":
          raise SyntaxError(
            f"[defaults.ini] deprecated key '{section}.{option}' redirects to deprecated key '{tsec}.{topt}'."
          )
      else:
        self.__parser.set(section, option, self.__validated(section, option, self.__parser.get(section, option)))

  def __validated(self, section, option, value):
    """Check `value` against the declared type of `section.option` and return it as a
    (possibly canonicalized) string. Raise ValueError if the value does not respect the type."""
    s = str(value)
    meta = self.__schema.get((section, option))
    if meta is None:
      return s
    if meta.kind == "int" and not self._check_int(s):
      raise ValueError(f"'{s}' must be an int for '{section}.{option}'.")
    if meta.kind == "float" and not self._check_float(s):
      raise ValueError(f"'{s}' must be a float for '{section}.{option}'.")
    if meta.kind == "bool":
      return self.__validatedBool(s, section, option)
    if meta.kind == "enum":
      return self.__validatedEnum(s, meta, section, option)
    return s

  def __validatedBool(self, s, section, option):
    """Canonicalize an accepted boolean spelling (yes/on/1/...) to 'True' or 'False'."""
    if not self._check_bool(s):
      raise ValueError(f"'{s}' must be a boolean (False/True, 0/1, Off/On, No/Yes) for '{section}.{option}'.")
    return "True" if self._check_bool_true(s) else "False"

  def __validatedEnum(self, s, meta, section, option):
    """Canonicalize a value to the declared casing of its enumeration."""
    for v in meta.values:
      if v.lower() == s.lower():
        return v
    raise ValueError(f"'{s}' must be one of {'|'.join(meta.values)} for '{section}.{option}'.")

  def __deprecationMessage(self, section, option, meta):
    return (
      f"[pyAgrum] '{section}.{option}' is deprecated ({meta.doc}), use '{meta.target[0]}.{meta.target[1]}' instead."
    )

  def __isVisible(self, section, option):
    """Deprecated keys are hidden from every display/diff/save."""
    meta = self.__schema.get((section, option))
    return meta is None or meta.kind != "deprecated"

  def __snapshot(self):
    """Copy of the current values as ``{section: {option: value}}``."""
    return {section: dict(self.__parser.items(section)) for section in self.__parser.sections()}

  def __restore(self, snapshot):
    for section, options in snapshot.items():
      for option, value in options.items():
        self.__parser.set(section, option, value)

  def __render(self, keep):
    """Ini-like rendering of the visible keys satisfying the ``keep(section, option)`` predicate.
    Sections without any matching key are omitted."""
    parts = []
    for section in self.__parser.sections():
      lines = [
        f"  {option} = {value}"
        for option, value in self.__parser.items(section)
        if self.__isVisible(section, option) and keep(section, option)
      ]
      if lines:
        parts.append(f"[{section}]\n" + "\n".join(lines))
    return "\n".join(parts)

  def add_hook(self, fn):
    self.__hooks.append(fn)

  def run_hooks(self):
    for fn in self.__hooks:
      fn()

  def set(self, section, option, value, no_hook=False):
    """set a property in a section. Preferably use ``__getitem__`` and ``__setitem__``.

    Examples
    --------
      >>> pyagrum.config["dynamicBN", "default_graph_size"] = 10
      >>> pyagrum.config["dynamicBN", "default_graph_size"]
      "10"

    Parameters
    ----------
    section : str
        The section name (has to exist in defaults).
    option : str
        The option/property name (has to exist in defaults).
    value : str
        The value (will be stored as string).
    no_hook : bool, optional
        Should this call trigger the hooks?

    Raises
    ------
    SyntaxError
        If the section name or the property name does not exist.
    ValueError
        If the value does not respect the type declared for this key in defaults.
    """
    if not self.__parser.has_section(section):
      raise SyntaxError(f"Section '{section}' unknown in pyAgrum configuration.")
    opt = option.lower()
    if not self.__parser.has_option(section, opt):
      raise SyntaxError(f"Key '{section},{option}' unknown in pyAgrum configuration.")
    meta = self.__schema.get((section, opt))
    if meta is not None and meta.kind == "deprecated":
      warnings.warn(self.__deprecationMessage(section, opt, meta), DeprecationWarning, stacklevel=2)
      self.set(meta.target[0], meta.target[1], value, no_hook)
      return
    self.__parser.set(section, opt, self.__validated(section, opt, value))
    if not no_hook:
      self.run_hooks()

  def get(self, section, option):
    """Give the value associated to section.option. Preferably use ``__getitem__`` and ``__setitem__``.

    Examples
    --------
      >>> pyagrum.config["dynamicBN", "default_graph_size"] = 10
      >>> pyagrum.config["dynamicBN", "default_graph_size"]
      "10"

    Parameters
    ----------
    section : str
        The section.
    option : str
        The property.

    Returns
    -------
    str
        The value (as string).
    """
    opt = option.lower()
    meta = self.__schema.get((section, opt))
    if meta is not None and meta.kind == "deprecated":
      warnings.warn(self.__deprecationMessage(section, opt, meta), DeprecationWarning, stacklevel=2)
      return self.get(*meta.target)
    return self.__parser.get(section, option)

  def doc(self, section, option):
    """Give the type and the one-line documentation declared for section.option in defaults.

    Examples
    --------
      >>> pyagrum.config.doc("notebook", "tensor_visible_digits")
      "int :: number of visible digits when displaying a tensor"

    Parameters
    ----------
    section : str
        The section.
    option : str
        The property.

    Returns
    -------
    str
        The metadata as ``type :: documentation`` (``deprecated -> section.key :: documentation``
        for a deprecated key).

    Raises
    ------
    SyntaxError
        If the section name or the property name does not exist.
    """
    if not self.__parser.has_section(section):
      raise SyntaxError(f"Section '{section}' unknown in pyAgrum configuration.")
    opt = option.lower()
    if not self.__parser.has_option(section, opt):
      raise SyntaxError(f"Key '{section},{option}' unknown in pyAgrum configuration.")
    meta = self.__schema.get((section, opt))
    if meta is None:
      return "str :: (undocumented)"
    if meta.kind == "deprecated":
      return f"deprecated -> {meta.target[0]}.{meta.target[1]} :: {meta.doc}"
    if meta.kind == "enum":
      return f"{'|'.join(meta.values)} :: {meta.doc}"
    return f"{meta.kind} :: {meta.doc}"

  def getTyped(self, section, option):
    """Give the value associated to section.option, converted according to the type declared
    in defaults: ``int`` and ``float`` are converted, ``bool`` gives a Python bool, enumerations
    and ``str`` stay strings. Preferably use the ``typed`` accessor: ``config.typed[section, option]``.

    Examples
    --------
      >>> pyagrum.config.typed["notebook", "tensor_visible_digits"]
      4
      >>> pyagrum.config.typed["notebook", "tensor_with_colors"]
      True

    Parameters
    ----------
    section : str
        The section.
    option : str
        The property.

    Returns
    -------
    int | float | bool | str
        The value, converted according to the declared type.
    """
    value = self.get(section, option)
    meta = self.__schema.get((section, option.lower()))
    if meta is not None and meta.kind == "deprecated":
      meta = self.__schema.get(meta.target)
    if meta is None:
      return value
    if meta.kind == "int":
      return int(value)
    if meta.kind == "float":
      return float(value)
    if meta.kind == "bool":
      return self._check_bool_true(value)
    return value

  def schema(self):
    """Iterate over the whole configuration schema (in the order of the defaults).

    Examples
    --------
      >>> for section, option, meta, default in pyagrum.config.schema():
      ...   print(f"{section}.{option} ({meta.kind}) = {default} : {meta.doc}")

    Yields
    ------
    Tuple[str, str, ConfigMetadata, Optional[str]]
        section, option, the metadata of the key (type, allowed values, documentation,
        deprecation target) and its default value (None for a deprecated key).
    """
    for section in self.__parser.sections():
      for option in self.__parser[section]:
        meta = self.__schema.get((section, option))
        if meta is None:
          meta = ConfigMetadata("str", (), "(undocumented)", None)
        default = None if meta.kind == "deprecated" else self.__defaults[section][option]
        yield section, option, meta, default

  def __diff(self):
    return self.__render(lambda section, option: self.__parser.get(section, option) != self.__defaults[section][option])

  def save(self):
    """Save the diff with the defaults in ``pyagrum.ini`` in the current directory"""
    with open("pyagrum.ini", "w", encoding="utf-8") as configfile:
      print(self.__diff(), file=configfile)

  def reset(self):
    """back to defaults"""
    self.__restore(self.__defaults)
    self.run_hooks()

  def load(self):
    """load pyagrum.ini in the current directory, and change the properties if needed.
    If pyagrum.ini contains deprecated keys or invalid values, a warning is emitted and
    pyagrum.ini is rewritten (deprecated keys migrated to their replacement, invalid
    values removed).

    Raises
    ------
    FileNotFoundError
        If there is no pyagrum.ini in the current directory.
    """
    if not os.path.isfile("pyagrum.ini"):
      raise FileNotFoundError("No file 'pyagrum.ini' in current directory.")

    # to force to use the protected set() method
    c = ConfigParser()
    c.read("pyagrum.ini", encoding="utf-8")
    rewrite_needed = False
    for section in c.sections():
      if not self.__parser.has_section(section):
        rewrite_needed = True
        warnings.warn(f"[pyagrum.ini] Section '{section}' does not exist.", stacklevel=2)
        continue
      for option in c[section]:
        meta = self.__schema.get((section, option.lower()))
        if meta is not None and meta.kind == "deprecated":
          # set() emits the DeprecationWarning; the rewrite below migrates pyagrum.ini
          rewrite_needed = True
        try:
          self.set(section, option, c[section][option], no_hook=True)
        except SyntaxError:
          rewrite_needed = True
          warnings.warn(f"[pyagrum.ini] Option '{section}.{option}' does not exist.", stacklevel=2)
        except ValueError as err:
          rewrite_needed = True
          warnings.warn(f"[pyagrum.ini] Invalid value for '{section}.{option}': {err}", stacklevel=2)
    self.run_hooks()
    if rewrite_needed:
      self.save()

  def grep(self, search):
    """grep in the configuration any section or property matching the argument. If a section matches
    the argument, the whole section is displayed.

    Parameters
    ----------
    search : str
        The string to find.
    """
    lowsearch = search.lower()
    print(self.__render(lambda section, option: lowsearch in section.lower() or lowsearch in option))

  def diff(self):
    """print the diff between actual configuration and the defaults. This is what is saved in the file ``pyagrum.ini`` by the method `PyAgrumConfiguration.save()`"""
    print(self.__diff())

  def __str__(self):
    return self.__render(lambda section, option: True)

  def __repr__(self):
    res = self.__diff()
    if res:
      return res
    return "# no customized property\n" + str(self)

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
    if not self.__stacks:
      raise IndexError("[pyAgrum] No configuration to pop")
    self.__restore(self.__stacks.pop())
    self.run_hooks()

  def push(self):
    """
    Push the current configuration in the stack
    """
    self.__stacks.append(self.__snapshot())
