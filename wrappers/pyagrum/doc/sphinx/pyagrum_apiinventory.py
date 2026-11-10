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

"""Inventory of the pyAgrum API documented by the Sphinx documentation (no dependency on Sphinx).

The ``.rst`` sources declare the documented classes (``.. pyagrum-class::``) and functions
(``.. pyagrum-functions::``). The inventory lists their members and the page owning each of them: a member shared by
several documented classes has a single page, named after the most general documented class that owns it.

Used by the Sphinx extension ``pyagrum_apipages`` (page generation) and by the pyAgrum tests (one example per page in
``doc/examples``).
"""

import importlib
import inspect
import re
from collections.abc import Callable
from pathlib import Path

API_DIR = "api"
EXAMPLES_DIR = "../examples"  # relative to the Sphinx sources: one file <page name>.py per generated page
_DIRECTIVE = re.compile(r"^[ \t]*\.\. pyagrum-(?P<kind>class|functions)::[ \t]*(?P<name>\S*)[ \t]*$")
_OPTION = re.compile(r"^[ \t]+:(?P<key>[a-z-]+):[ \t]*(?P<value>.*)$")
_SKIPPED = {"thisown", "clone"}  # SWIG ownership flag, and members hidden by conf.skip
_CITED = re.compile(r"(?<![\w:]):(?:foot)?cite:(?:[a-z]+:)?`([^`]+)`")


def resolve(name: str):
  """The object named by a dotted path: an attribute first (pyagrum.clg.CLG is the class, not the submodule)."""
  parts = name.split(".")
  obj = importlib.import_module(parts[0])
  for i, part in enumerate(parts[1:], start=2):
    try:
      obj = getattr(obj, part)
    except AttributeError:
      obj = importlib.import_module(".".join(parts[:i]))
  return obj


def _definer(cls: type, member: str) -> type | None:
  """The class of the MRO of cls where member is defined."""
  return next((k for k in cls.__mro__ if member in k.__dict__), None)


def _kind(cls: type, member: str) -> str:
  raw = _definer(cls, member).__dict__[member]
  if isinstance(raw, (staticmethod, classmethod)) or inspect.isroutine(raw):
    return "method"
  if inspect.isdatadescriptor(raw):
    return "property"
  return "attribute"


def citedKeys(obj) -> set[str]:
  """Bibtex keys cited in the docstring of obj."""
  return {key.strip() for keys in _CITED.findall(inspect.getdoc(obj) or "") for key in keys.split(",") if key.strip()}


def _splitList(value: str) -> list[str]:
  return [v.strip() for v in value.split(",") if v.strip()]


def moduleFunctions(module: str) -> list[str]:
  """Public functions defined in a module, in source order."""
  mod = importlib.import_module(module)
  found = [
    (obj.__code__.co_firstlineno, name)
    for name, obj in vars(mod).items()
    if not name.startswith("_") and inspect.isfunction(obj) and obj.__module__ == module
  ]
  return [f"{module}.{name}" for _, name in sorted(found)]


def scan(srcdir: Path, excluded: Callable[[str], bool]) -> list[tuple[str, str, dict[str, str], str]]:
  """(kind, name, options, docname) of every pyagrum-class and of every function of the pyagrum-functions.

  excluded tells whether a source path (relative to srcdir, as for Sphinx exclude_patterns) is excluded.
  """
  found = []
  for path in sorted(srcdir.rglob("*.rst")):
    rel = path.relative_to(srcdir).as_posix()
    parents = [p.as_posix() for p in path.relative_to(srcdir).parents][:-1]  # excluded directories (site...)
    if rel.startswith(API_DIR) or excluded(rel) or any(excluded(parent) for parent in parents):
      continue
    lines = path.read_text(encoding="utf-8").splitlines()
    for i, line in enumerate(lines):
      match = _DIRECTIVE.match(line)
      if match is None:
        continue
      options: dict[str, str] = {}
      key = None
      body = lines[i + 1 :]
      j = 0
      for j, option in enumerate(body):
        if not option.strip():
          break
        opt = _OPTION.match(option)
        if opt:
          key = opt["key"]
          options[key] = opt["value"].strip()
        elif key is not None and option[:1] in " \t":  # continuation of a long option value
          options[key] += " " + option.strip()
        else:
          break
      docname = rel[: -len(".rst")]
      if match["kind"] == "class":
        found.append(("class", match["name"], options, docname))
        continue
      names = moduleFunctions(match["name"]) if match["name"] else []
      for line in body[j + 1 :] if not match["name"] else []:  # content: one function per line
        if line.strip() and line[:1] not in " \t":
          break
        names += line.split()
      found += [("function", name, {}, docname) for name in names]
  return found


class Inventory:
  """Documented classes, their members, and the page owning each member."""

  def __init__(self, directives_found: list[tuple[str, str, dict[str, str], str]]):
    self.classes: dict[str, type] = {}  # documented name -> class
    self.names: dict[type, str] = {}  # class -> first documented name
    self.primary: dict[str, str] = {}  # documented name -> docname holding the toctree of its pages
    self.members: dict[str, list[str]] = {}  # documented name -> listed members
    self.owner: dict[tuple[str, str], str] = {}  # (documented name, member) -> owner of its page
    self.pages: dict[tuple[str, str], dict] = {}  # (owner, member) -> kind, users, overriddenIn
    self.functions: dict[str, str] = {}  # function name -> docname holding the toctree of its page
    self.references: dict[str, set[str]] = {}  # docname -> keys cited in the generated pages of its toctree
    self.problems: list[tuple[str, str]] = []  # (docname, message): objects that cannot be documented
    self.unavailable: set[str] = set()  # names that cannot be imported (missing optional dependency...)

    # everything is imported before listing the members: a module may add methods to a class (clg.notebook: CLG.toDot)
    options_of: dict[str, dict[str, str]] = {}
    functionObjects = {}
    for kind, name, options, docname in directives_found:
      try:
        obj = resolve(name)
      except (ImportError, AttributeError):
        self.problems.append((docname, f"pyagrum-{kind}: cannot import {name}"))
        self.unavailable.add(name)
        continue
      if kind == "function":
        self.functions.setdefault(name, docname)
        functionObjects[name] = obj
        continue
      cls = obj
      if not inspect.isclass(cls):
        self.problems.append((docname, f"pyagrum-class: {name} is not a class"))
        continue
      self.classes.setdefault(name, cls)
      self.names.setdefault(cls, name)
      self.primary.setdefault(name, docname)
      options_of.setdefault(name, options)

    for name, cls in self.classes.items():
      self.members[name] = self._listed(cls, options_of[name])
      for member in self.members[name]:
        owner = self._owner(cls, member)
        self.owner[(name, member)] = owner
        page = self.pages.setdefault((owner, member), {"kind": _kind(cls, member), "users": [], "overriddenIn": []})
        page["users"].append(name)

    for owner, member in self.pages:
      keys = citedKeys(getattr(self.classes[owner], member))
      if keys:
        self.references.setdefault(self.primary[owner], set()).update(keys)
    for name, docname in self.functions.items():
      keys = citedKeys(functionObjects[name])
      if keys:
        self.references.setdefault(docname, set()).update(keys)

    for (owner, member), page in self.pages.items():
      ownerCls = self.classes[owner]
      for name, cls in self.classes.items():
        if cls is not ownerCls and issubclass(cls, ownerCls) and (name, member) in self.owner:
          other = self.owner[(name, member)]
          if other != owner and other not in page["overriddenIn"]:
            page["overriddenIn"].append(other)

  def _listed(self, cls: type, options: dict[str, str]) -> list[str]:
    if "no-members" in options:
      return []
    special = _splitList(options.get("special-members", ""))
    names = [n for n in dir(cls) if (not n.startswith("_") or n in special) and n not in _SKIPPED]
    if options.get("members"):
      wanted = set(_splitList(options["members"]))
      names = [n for n in names if n in wanted]
    excluded = set(_splitList(options.get("exclude-members", "")))
    # members inherited from outside pyagrum (int, MutableMapping, Exception...) are not documented here
    names = [n for n in names if n not in excluded and _definer(cls, n).__module__.startswith("pyagrum")]
    if "no-inherited-members" in options:
      names = [n for n in names if n in cls.__dict__]
    return names

  def _owner(self, cls: type, member: str) -> str:
    """The most general documented class of the MRO of cls using the same definition of member."""
    definer = _definer(cls, member)
    for k in reversed(cls.__mro__):
      if k in self.names and _definer(k, member) is definer:
        return self.names[k]
    return self.names[cls]

  def pageName(self, owner: str, member: str) -> str:
    return f"{API_DIR}/{owner}.{member}"

  def pageNames(self) -> list[str]:
    """Names of all the generated pages (without directory): ``owner.member`` and ``module.function``."""
    return sorted([f"{owner}.{member}" for owner, member in self.pages] + list(self.functions))
