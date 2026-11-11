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

"""One page per member of the documented classes and per documented function, and tables linking to them.

``.. pyagrum-class:: pyagrum.X`` replaces ``.. autoclass:: pyagrum.X`` (same options). It renders the docstring of
the class and tables of its members, each linked to its own page in ``api/``. A member shared by several
documented classes (inherited without being redefined) has a single page, named after the most general documented
class that owns it, with the list and the inheritance diagram of all the classes using it.

``.. pyagrum-functions::`` (a module as argument, or the function names as content) replaces ``.. autofunction::``:
a table of the functions, each one with its own page.

Each generated page shows the example of its member, if any: the file ``doc/examples/<page name>.py``. A class
shows its own example, if any, after its docstring: the file ``doc/examples/<class name>.class.py``.

Citations (``:cite:``) of the docstrings become footnote citations in the generated pages, each page ending with its
own ``footbibliography``; the hand-written page holding their toctree ends with the list of all these references.

The pages are generated at ``builder-inited`` from a scan of the ``.rst`` sources; references to a member through a
subclass (``pyagrum.BayesNet.cpt``) are resolved to its page (``pyagrum.IBayesNet.cpt`` would not be documented).
"""

import inspect
import re
from pathlib import Path
from types import SimpleNamespace

from docutils import nodes
from docutils.parsers.rst import directives
from docutils.statemachine import StringList
from pyagrum_apiinventory import API_DIR, EXAMPLES_DIR, Inventory, moduleFunctions, resolve, scan
from sphinx import addnodes
from sphinx.ext.autodoc.mock import mock
from sphinx.ext.autosummary import extract_summary
from sphinx.ext.inheritance_diagram import inheritance_diagram
from sphinx.util import logging
from sphinx.util.docutils import SphinxDirective
from sphinx.util.matching import Matcher

logger = logging.getLogger(__name__)

_PASSED_OPTIONS = ("noindex", "no-index", "show-inheritance")
_ROLES = {"method": "meth", "property": "attr", "attribute": "attr"}
_DIRECTIVES = {"method": "automethod", "property": "autoproperty", "attribute": "autoattribute"}
_TABLES = {"method": "Methods", "property": "Attributes", "attribute": "Attributes"}
_CITE = re.compile(r"(?<![\w:]):cite:")
_UNDERLINE = re.compile(r"^\s*([-=~^])\1+\s*$")
# extract_summary takes the document before Sphinx 9 (the CI builds the doc with Sphinx 8.1), the settings since
_SUMMARY_TAKES_DOCUMENT = "document" in inspect.signature(extract_summary).parameters


def _summary(obj, settings) -> str:
  """First sentence of the docstring (empty when it starts with a section such as Returns)."""
  lines = (inspect.getdoc(obj) or "").strip().splitlines()
  if not lines or (len(lines) > 1 and _UNDERLINE.match(lines[1])):
    return ""
  context = SimpleNamespace(settings=settings) if _SUMMARY_TAKES_DOCUMENT else settings
  return " ".join(extract_summary(lines, context).split())


def _example(path: Path) -> list[str]:
  """The example of a page (the file of doc/examples), as rst lines; none if the file does not exist."""
  if not path.exists():
    return []
  code = [line for line in path.read_text(encoding="utf-8").splitlines() if not line.startswith("# requires:")]
  while code and not code[0].strip():
    code.pop(0)
  return [".. rubric:: Example", "", ".. code-block:: python", ""] + [f"   {line}".rstrip() for line in code] + [""]


def _table(rows: list[tuple[str, str]]) -> list[str]:
  """A summary table (same look as autosummary): one row per (reference, summary)."""
  empty = "\\ "  # an escaped space: an empty but valid cell
  lines = [".. list-table::", "   :class: autosummary longtable", ""]
  for reference, summary in rows:
    lines += [f"   * - {reference}", f"     - {summary or empty}"]
  return lines + [""]


def functionSource(name: str, example: list[str]) -> str:
  title = name.removeprefix("pyagrum.")
  lines = [title, "=" * len(title), "", f".. autofunction:: {name}", ""] + example + [".. footbibliography::", ""]
  return "\n".join(lines)


def functionsSource(inventory: Inventory, names: list[str], docname: str, settings) -> list[str]:
  lines = _table([(f":func:`~{name}`", _summary(resolve(name), settings)) for name in names])
  owned = [name for name in names if inventory.functions.get(name) == docname]
  if owned:
    lines += [".. toctree::", "   :hidden:", ""] + [f"   /{API_DIR}/{name}" for name in owned] + [""]
  return lines


def pageSource(inventory: Inventory, owner: str, member: str, example: list[str]) -> str:
  page = inventory.pages[(owner, member)]
  kind = page["kind"]
  role = _ROLES[kind]
  title = f"{owner.rsplit('.', 1)[-1]}.{member}"
  lines = [title, "=" * len(title), "", f".. {_DIRECTIVES[kind]}:: {owner}.{member}", ""] + example
  users = page["users"]
  if len(users) == 1:
    lines += [".. rubric:: Class", "", f":class:`~{owner}`", ""]
  else:
    users = [owner] + sorted((u for u in users if u != owner), key=lambda u: u.rsplit(".", 1)[-1])
    lines += [".. rubric:: Used by", "", ", ".join(f":class:`~{u}`" for u in users), ""]
    lines += [f".. inheritance-diagram:: {' '.join(users)}", "   :parts: 1", f"   :top-classes: {owner}", ""]
  if page["overriddenIn"]:
    lines += [".. rubric:: Overridden in", ""]
    short = {o: f"{o.rsplit('.', 1)[-1]}.{member}" for o in page["overriddenIn"]}
    lines += [", ".join(f":{role}:`{short[o]} <{o}.{member}>`" for o in sorted(short, key=short.get)), ""]
  lines += [".. footbibliography::", ""]
  return "\n".join(lines)


def classSource(
  inventory: Inventory, name: str, options: dict[str, str], docname: str, settings, example: list[str]
) -> list[str]:
  lines = [f".. autoclass:: {name}", "   :no-members:"]
  lines += [f"   :{key}:" for key in _PASSED_OPTIONS if key in options]
  lines.append("")
  lines += example
  groups: dict[str, dict[str, list[str]]] = {}  # owner -> table -> members
  for member in inventory.members.get(name, []):
    owner = inventory.owner[(name, member)]
    table = _TABLES[inventory.pages[(owner, member)]["kind"]]
    groups.setdefault(owner, {}).setdefault(table, []).append(member)
  cls = inventory.classes[name]
  order = [name] + [inventory.names[k] for k in cls.__mro__[1:] if inventory.names.get(k) in groups]
  order += [o for o in groups if o not in order]  # owners outside the MRO: none, kept for safety
  for owner in order:
    for table, members in sorted(groups.get(owner, {}).items(), key=lambda t: t[0] != "Methods"):
      header = table if owner == name else f"{table} inherited from :class:`~{owner}`"
      role = "meth" if table == "Methods" else "attr"
      ownerCls = inventory.classes[owner]
      lines += [f".. rubric:: {header}", ""]
      lines += _table([(f":{role}:`~{owner}.{m}`", _summary(getattr(ownerCls, m), settings)) for m in members])
  owned = [m for m in inventory.members.get(name, []) if inventory.owner[(name, m)] == name]
  if owned and inventory.primary.get(name) == docname:
    lines += [".. toctree::", "   :hidden:", ""]
    lines += [f"   /{inventory.pageName(name, m)}" for m in owned]
    lines.append("")
  return lines


class PyagrumClass(SphinxDirective):
  """``.. pyagrum-class:: pyagrum.X``: docstring of the class and tables of its members."""

  required_arguments = 1
  option_spec = {
    key: directives.unchanged
    for key in (
      "members",
      "undoc-members",
      "inherited-members",
      "no-inherited-members",
      "exclude-members",
      "no-members",
      "noindex",
      "no-index",
      "show-inheritance",
      "special-members",
    )
  }

  def run(self) -> list[nodes.Node]:
    inventory: Inventory = self.env.app._pyagrumInventory
    name = self.arguments[0]
    if name not in inventory.classes:
      return []  # import failure already reported by the scan
    options = {key: value or "" for key, value in self.options.items()}
    container = nodes.container()
    example = _example(Path(self.env.srcdir) / EXAMPLES_DIR / f"{Inventory.classExampleName(name)}.py")
    lines = classSource(inventory, name, options, self.env.docname, self.state.document.settings, example)
    self.state.nested_parse(
      StringList(lines, source=self.env.doc2path(self.env.docname)), self.content_offset, container
    )
    return container.children


class PyagrumFunctions(SphinxDirective):
  """``.. pyagrum-functions:: module`` or with the function names as content: table of the functions."""

  optional_arguments = 1
  has_content = True

  def run(self) -> list[nodes.Node]:
    inventory: Inventory = self.env.app._pyagrumInventory
    names = moduleFunctions(self.arguments[0]) if self.arguments else [n for line in self.content for n in line.split()]
    container = nodes.container()
    lines = functionsSource(inventory, names, self.env.docname, self.state.document.settings)
    self.state.nested_parse(
      StringList(lines, source=self.env.doc2path(self.env.docname)), self.content_offset, container
    )
    return container.children


def _generate(app) -> None:
  srcdir = Path(app.srcdir)
  with mock(app.config.autodoc_mock_imports):  # same imports as autodoc (qiskit for pyagrum.qBNSampling)
    inventory = Inventory(scan(srcdir, Matcher(app.config.exclude_patterns)))
  app._pyagrumInventory = inventory
  for docname, message in inventory.problems:
    logger.warning(message, location=docname)

  # inheritance diagrams: name the classes by their documented name (pyagrum.DAG, not pyagrum.pyagrumcpp.DAG)
  for cls, name in inventory.names.items():
    app.config.inheritance_alias.setdefault(f"{cls.__module__}.{cls.__qualname__}", name)

  apidir = srcdir / API_DIR
  apidir.mkdir(parents=True, exist_ok=True)
  examples = srcdir / EXAMPLES_DIR
  sources = {
    f"{owner}.{member}.rst": pageSource(inventory, owner, member, _example(examples / f"{owner}.{member}.py"))
    for owner, member in inventory.pages
  }
  sources |= {f"{name}.rst": functionSource(name, _example(examples / f"{name}.py")) for name in inventory.functions}
  lowered: dict[str, str] = {}
  for page, source in sources.items():
    if page.lower() in lowered:  # case-insensitive file systems (macOS, Windows)
      logger.warning(f"pyagrum-class: {page} and {lowered[page.lower()]} differ only by case")
    lowered[page.lower()] = page
    path = apidir / page
    if not path.exists() or path.read_text(encoding="utf-8") != source:
      path.write_text(source, encoding="utf-8")  # unchanged pages keep their date: no rebuild
  for path in apidir.glob("*.rst"):
    if path.name not in sources:
      path.unlink()
  pages = set(inventory.pageNames())
  names = inventory.exampleNames()
  for path in sorted(examples.glob("*.py")):
    if path.stem not in names:  # renamed or removed member or class, or typo in the file name
      logger.warning(f"pyagrum-class: example {path.name} matches no documented member, function or class")
  missing = len(pages - {path.stem for path in examples.glob("*.py")})
  logger.info(
    f"pyagrum-class: {len(inventory.classes)} classes, {len(inventory.pages)} member pages, "
    f"{len(inventory.functions)} function pages, {missing} pages without example"
  )


def _missingReference(app, env, node, contnode):
  """Resolve a member reached through a subclass (pyagrum.BayesNet.cpt) to the page that owns it."""
  if node.get("refdomain") != "py" or not hasattr(app, "_pyagrumInventory"):
    return None
  inventory: Inventory = app._pyagrumInventory
  target = node["reftarget"]
  module, cls = node.get("py:module"), node.get("py:class")
  candidates = [target, f"pyagrum.{target}"]
  if module:
    candidates += [f"{module}.{target}"] + ([f"{module}.{cls}.{target}"] if cls else [])
  for full in candidates:
    name, _, member = full.rpartition(".")
    owner = inventory.owner.get((name, member))
    if owner is not None and owner != name:
      return env.get_domain("py").resolve_xref(
        env, node["refdoc"], app.builder, node["reftype"], f"{owner}.{member}", node, contnode
      )
  return None


def _footcite(app, what, name, obj, options, lines: list[str]) -> None:
  """In the generated pages, citations are footnotes listed by the footbibliography of the page."""
  if app.env.docname.startswith(f"{API_DIR}/"):
    lines[:] = [_CITE.sub(":footcite:", line) for line in lines]


def _membersReferences(app, docname: str, source: list[str]) -> None:
  """End a page with the references cited by the generated pages of its toctree (a list: no citation target)."""
  inventory: Inventory | None = getattr(app, "_pyagrumInventory", None)
  keys = inventory.references.get(docname) if inventory else None
  if keys:
    selected = ", ".join(f'"{key.lower()}"' for key in sorted(keys))  # bibtex compares lowercased keys
    source[0] += "\n\n.. rubric:: References of the members\n\n.. bibliography::\n   :list: bullet\n"
    source[0] += f"   :filter: key in {{{selected}}}\n"


def _skipNode(translator, node) -> None:
  raise nodes.SkipNode


def _markdownToctree(translator, node) -> None:
  if node.get("hidden"):  # the hidden toctrees of the class pages: nothing to render
    raise nodes.SkipNode
  translator.unknown_visit(node)


def setup(app):
  app.setup_extension("sphinx.ext.inheritance_diagram")
  # markdown (website): no image map; the classes are listed in text just above the diagram
  app.add_node(inheritance_diagram, override=True, markdown=(_skipNode, None))
  app.add_node(addnodes.toctree, override=True, markdown=(_markdownToctree, None))
  app.add_directive("pyagrum-class", PyagrumClass)
  app.add_directive("pyagrum-functions", PyagrumFunctions)
  app.connect("builder-inited", _generate)
  app.connect("missing-reference", _missingReference)
  app.connect("autodoc-process-docstring", _footcite)
  app.connect("source-read", _membersReferences)
  return {"parallel_read_safe": True, "parallel_write_safe": True}
