/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

/*
 * Python-specific part of the gum::Configuration binding (the generic part is in
 * wrappers/swig/configuration.i). The logic stays in gum::Configuration: the Python code below only
 * converts between C++ and Python objects (values, exceptions, notices as warnings, hooks).
 *
 * This file has to be included BEFORE aGrUM_wrap_BASE.i: the %rename below must be known when the
 * class is declared.
 */

%{
#include <type_traits>
#include <variant>
%}

// the methods completed by the Python code below are hidden behind a leading underscore
%rename("_%s") gum::Configuration::get;
%rename("_%s") gum::Configuration::set;
%rename("_%s") gum::Configuration::doc;
%rename("_%s") gum::Configuration::getValue;
%rename("_%s") gum::Configuration::load;
%rename("_%s") gum::Configuration::save;
%rename("_%s") gum::Configuration::reset;
%rename("_%s") gum::Configuration::push;
%rename("_%s") gum::Configuration::pop;
%rename("_%s") gum::Configuration::grep;
%rename("_%s") gum::Configuration::diff;
%rename("_%s") gum::Configuration::runHooks;
%rename("_%s") gum::Configuration::addPythonHook;
%rename("_%s") gum::Configuration::schemaRows;
%rename("_%s") gum::Configuration::takeNoticeRows;

%feature("docstring") gum::Configuration
"
The configuration of pyAgrum (``pyagrum.config`` is the configuration singleton of the aGrUM C++ library,
``gum::Configuration``). The configuration is a read-only structure of sections and keys: a value is
adressable using a double key: ``[section,key]``. Section names are case-sensitive, key names are not.

The sections and keys are created by the defaults, in this order: ``defaults.ini`` of aGrUM, then
``defaults.ini`` of pyAgrum (both embedded in the library when pyAgrum is compiled). The file
``pyagrum.ini`` of the current directory, if any, is loaded when pyAgrum is imported: it changes the
value of some of these keys, but never creates a section or a key.

Each key is described in ``defaults.ini`` by a metadata line ``### type :: documentation`` where type is
``int``, ``float``, ``bool``, ``str``, an enumeration ``v1|v2|...`` or a deprecation
``deprecated -> section.key``. Setting a value that does not respect the declared type raises a
``ValueError``. Accessing a deprecated key emits a ``DeprecationWarning`` and transparently uses the
replacement key. While values are always stored as strings, ``config.typed[section, key]`` gives the
value converted according to the declared type. Boolean and enumerated values are canonicalized when set:
any accepted boolean spelling (``yes``/``on``/``1``/...) is stored as ``True`` or ``False``, an
enumerated value is stored with the casing declared in ``defaults.ini``.

See `this notebook <https://lip6.fr/Pierre-Henri.Wuillemin/aGrUM/docs/last/notebooks/configForPyAgrum.ipynb.html>`_.

Examples
--------
  >>> import pyagrum
  >>> pyagrum.config[\"dynamicBN\", \"graphSize\"] = 10
  >>> pyagrum.config[\"dynamicBN\", \"graphSize\"]
  \"10\"
"

%extend gum::Configuration {
  // the value of section.key converted by the C++ configuration according to the declared type of
  // the key: a bool, an int, a float or a str
  PyObject* getValue(std::string_view section, std::string_view key) const {
    return std::visit(
        [](const auto& value) -> PyObject* {
          using T = std::decay_t< decltype(value) >;
          if constexpr (std::is_same_v< T, bool >) return PyBool_FromLong(value ? 1 : 0);
          else if constexpr (std::is_same_v< T, std::int64_t >) return PyLong_FromLongLong(value);
          else if constexpr (std::is_same_v< T, double >) return PyFloat_FromDouble(value);
          else return PyUnicode_FromString(value.c_str());
        },
        self->getValue(section, key));
  }

  // registers a Python callable (without argument) as a hook of the configuration. The hook is
  // called with the GIL; an exception raised by the callable is propagated to whoever changed the
  // configuration. The callable is kept until the end of the process (the configuration is a
  // singleton, no Python object may be released from a static destructor).
  void addPythonHook(PyObject* callable) {
    if (!PyCallable_Check(callable)) {
      PyErr_SetString(PyExc_TypeError, "Need a callable object!");
      throw PythonErrorAlreadySet();
    }
    Py_INCREF(callable);
    self->addHook([callable](const gum::Configuration&) {
      const PyGILState_STATE gil = PyGILState_Ensure();
      // Configuration::set() runs the hooks again to roll back a failed change: the Python error of
      // the first failure is still pending and must neither be seen by this call nor be replaced
      PyObject *pendingType, *pendingValue, *pendingTrace;
      PyErr_Fetch(&pendingType, &pendingValue, &pendingTrace);
      PyObject*  result = PyObject_CallNoArgs(callable);
      const bool failed = (result == nullptr);
      Py_XDECREF(result);
      if (pendingType != nullptr) {
        if (failed) PyErr_Clear();
        PyErr_Restore(pendingType, pendingValue, pendingTrace);
      }
      PyGILState_Release(gil);
      if (failed) throw PythonErrorAlreadySet();
    });
  }

  std::string __repr__() const { return self->describe(); }
  std::string __str__() const { return self->toString(); }
}

%pythoncode %{
import warnings
from typing import NamedTuple


class ConfigMetadata(NamedTuple):
  """Metadata attached to a configuration key by a '###' line in ``defaults.ini``."""

  kind: str  # "int" | "float" | "bool" | "str" | "enum" | "deprecated"
  values: tuple[str, ...]  # allowed values when kind == "enum"
  doc: str  # one-line documentation
  target: tuple[str, str] | None  # replacement (section, option) when kind == "deprecated"
%}

%extend gum::Configuration {
%pythoncode %{
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

@property
def typed(self):
  """Accessor giving the values converted according to their declared type: ``config.typed[section, option]``."""
  return self._CastAsTyped(self)

@property
def asInt(self):
  return self._DeprecatedCast(self, "asInt")

@property
def asFloat(self):
  return self._DeprecatedCast(self, "asFloat")

@property
def asBool(self):
  return self._DeprecatedCast(self, "asBool")

def _emitNotices(self):
  """Turn the notices queued by the C++ configuration (deprecations, problems found in a user file) into warnings."""
  rows = self._takeNoticeRows()
  for i in range(0, len(rows), 2):
    kind, message = rows[i], rows[i + 1]
    if kind == "deprecation":
      warnings.warn(f"[pyAgrum] {message}", DeprecationWarning, stacklevel=4)
    else:
      warnings.warn(message, stacklevel=4)

def _run(self, name, *args):
  """Call a method of the C++ configuration, with the pyAgrum conventions for errors and warnings."""
  try:
    return getattr(self, name)(*args)
  except InvalidArgument as e:
    raise ValueError(str(e)) from None
  except OperationNotAllowed:
    raise IndexError("[pyAgrum] No configuration to pop") from None
  finally:
    self._emitNotices()

def add_hook(self, fn):
  """Register a function (without argument) called after each change of the configuration (see ``set``,
  ``reset``, ``pop`` and ``load``), after the hooks of the aGrUM library itself."""
  self._run("_addPythonHook", fn)

def run_hooks(self):
  """Run the hooks."""
  self._run("_runHooks")

def set(self, section, option, value, no_hook=False):
  """set a property in a section. Preferably use ``__getitem__`` and ``__setitem__``.

  Examples
  --------
    >>> pyagrum.config["dynamicBN", "graphSize"] = 10
    >>> pyagrum.config["dynamicBN", "graphSize"]
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
  self._run("_set", section, option, str(value), no_hook)

def get(self, section, option):
  """Give the value associated to section.option. Preferably use ``__getitem__`` and ``__setitem__``.

  Examples
  --------
    >>> pyagrum.config["dynamicBN", "graphSize"] = 10
    >>> pyagrum.config["dynamicBN", "graphSize"]
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
  return self._run("_get", section, option)

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
  return self._run("_doc", section, option)

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
  return self._run("_getValue", section, option)

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
  rows = self._schemaRows()
  for i in range(0, len(rows), 8):
    section, option, kind, values, doc, tsection, toption, default = rows[i : i + 8]
    meta = ConfigMetadata(
      kind,
      tuple(values.split("|")) if kind == "enum" else (),
      doc,
      (tsection, toption) if kind == "deprecated" else None,
    )
    yield section, option, meta, None if kind == "deprecated" else default

def save(self):
  """Save the diff with the defaults in ``pyagrum.ini`` in the current directory"""
  self._run("_save", self.userFileName())

def reset(self):
  """back to defaults"""
  self._run("_reset")

def load(self):
  """load pyagrum.ini in the current directory, and change the properties if needed.
  If pyagrum.ini contains deprecated keys or invalid values, a warning is emitted.
  pyagrum.ini is never modified by ``load()``: a deprecated key is applied to its
  replacement, and ``save()`` writes pyagrum.ini with the new key names.

  Raises
  ------
  FileNotFoundError
      If there is no readable pyagrum.ini in the current directory.
  SyntaxError
      If pyagrum.ini contains a malformed line (in this case, nothing is changed).
  """
  try:
    self._run("_load", self.userFileName())
  except GumIOError as e:
    raise FileNotFoundError(str(e)) from None

def grep(self, search):
  """grep in the configuration any section or property matching the argument. If a section matches
  the argument, the whole section is displayed.

  Parameters
  ----------
  search : str
      The string to find.
  """
  print(self._run("_grep", search))

def diff(self):
  """print the diff between actual configuration and the defaults. This is what is saved in the file ``pyagrum.ini`` by the method `save()`"""
  print(self._run("_diff"))

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
  self._run("_pop")

def push(self):
  """
  Push the current configuration in the stack
  """
  self._run("_push")
%}
}
