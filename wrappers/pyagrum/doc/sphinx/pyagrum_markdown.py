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

"""Markdown (website) rendering of the nodes that sphinx-markdown-builder does not know, and structure of the documentation.

sphinx-markdown-builder skips an unknown node with all its content (warning ``unknown node type``). The nodes used by
the documentation are rendered here; the other builders (html...) are not concerned.

The markdown builder also writes ``doc_structure.json``: for each page holding toctrees, its sub-pages grouped by the
section of the page holding their toctree (the website builds its tree of pages from it).
"""

import json
from pathlib import Path

from docutils import nodes
from sphinx import addnodes


def _pass(translator, node) -> None:
  pass


def _visitCitation(translator, node) -> None:
  # an entry of a bibliography (sphinxcontrib-bibtex): rendered as a footnote (anchor, label and text)
  translator.visit_footnote(node)


def _departCitation(translator, node) -> None:
  translator.depart_footnote(node)


def _visitCaption(translator, node) -> None:
  # caption of a figure or of a code block: a paragraph
  translator.ensure_eol(2)


def _departCaption(translator, node) -> None:
  translator.ensure_eol(2)


def _visitClassifier(translator, node) -> None:
  # type of a term in a definition list (``x : int``)
  translator.add(" : ")


def docStructure(env) -> dict[str, list[dict]]:
  """{docname: [{"title": section of the page holding the toctree (None: the page itself), "pages": [docnames]}]},
  one group per visible toctree (the hidden ones list the generated member pages), in the order of the page."""
  structure = {}
  for docname in sorted(env.toctree_includes):
    groups = []
    for toctree in env.get_doctree(docname).findall(addnodes.toctree):
      if toctree.get("hidden"):
        continue
      section = toctree.parent
      while section is not None and not isinstance(section, nodes.section):
        section = section.parent
      # the top section of a page (its title) is the page itself
      top = section is None or not isinstance(section.parent, nodes.section)
      title = None if top else section.next_node(nodes.title).astext()
      pages = [ref for _, ref in toctree["entries"] if ref in env.found_docs]  # no url, no "self"
      if pages:
        groups.append({"title": title, "pages": pages})
    if groups:
      structure[docname] = groups
  return structure


def _writeStructure(app, exception) -> None:
  if exception is None and app.builder.name == "markdown":
    path = Path(app.outdir) / "doc_structure.json"
    path.write_text(json.dumps(docStructure(app.env), indent=1), encoding="utf-8")


def setup(app):
  # abbreviation: the separator * (or /) of the keyword-only (positional-only) parameters in a signature
  app.add_node(nodes.abbreviation, override=True, markdown=(_pass, _pass))
  app.add_node(nodes.citation, override=True, markdown=(_visitCitation, _departCitation))
  app.add_node(nodes.caption, override=True, markdown=(_visitCaption, _departCaption))
  app.add_node(nodes.classifier, override=True, markdown=(_visitClassifier, _pass))
  app.connect("build-finished", _writeStructure)
  return {"parallel_read_safe": True, "parallel_write_safe": True}
