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

"""Git `clean` filter hiding insignificant notebook re-executions.

A re-executed notebook differs from its committed version by timestamps, memory addresses,
graphviz ids, ... even when no cell source changed. This filter makes git see such a notebook
as unchanged: if the notebook has the same cells (type, source, tags) and the same notebook
metadata as the version in the index, the indexed version is emitted instead of the working
tree one. The working tree file is never modified.

Installation on a clone
-----------------------
The `.gitattributes` file at the repository root (versioned) attaches the filter `nbsmart` to the
notebooks, but the definition of the filter lives in the local `.git/config` and is NOT versioned.
Each clone must declare it once, from the repository root:

    git config filter.nbsmart.clean "python3 wrappers/pyagrum/doc/sphinx/notebooks/nbsmart_filter.py %f"

Requirements: `python3` (standard library only, no virtualenv needed) in the PATH of the shell
running git. Without this declaration, git silently ignores the filter and notebooks behave
as usual. To uninstall: `git config --unset filter.nbsmart.clean`.

Usage
-----
`git diff` already ignores re-executed notebooks whose cells did not change, but `git status`
still lists them as modified until they are added. After re-executing notebooks, run

    git add wrappers/pyagrum/doc/sphinx/notebooks/*.ipynb

Insignificant notebooks then stage nothing and disappear from `git status`; the others are
staged with their new outputs. Review the notebooks that really changed before this `git add`.

Bypass, when outputs really changed while sources did not:

    NBSMART_DISABLE=1 git add <notebook>

If this script changes, force git to re-evaluate the notebooks:

    git update-index --really-refresh

Git runs this filter from the repository root, with the working tree file on stdin and the
repository-relative path as the only argument.
"""

import json
import os
import subprocess
import sys


def _significant(nb: dict) -> tuple:
  """Return the part of a notebook that matters: cell types, sources, tags and notebook metadata."""
  cells = [
    (c.get("cell_type"), "".join(c.get("source", [])), tuple(c.get("metadata", {}).get("tags", [])))
    for c in nb.get("cells", [])
  ]
  return cells, nb.get("metadata")


def _indexed_version(path: str) -> bytes | None:
  """Return the content of `path` in the index, or None if there is none."""
  res = subprocess.run(["git", "cat-file", "blob", f":{path}"], capture_output=True)
  return res.stdout if res.returncode == 0 else None


def main() -> None:
  working = sys.stdin.buffer.read()
  out = working  # on any doubt, behave as if the filter did not exist

  if len(sys.argv) > 1 and not os.environ.get("NBSMART_DISABLE"):
    indexed = _indexed_version(sys.argv[1])
    if indexed is not None:
      try:
        if _significant(json.loads(working)) == _significant(json.loads(indexed)):
          out = indexed
      except (ValueError, AttributeError, TypeError):
        pass  # not a valid notebook: leave it untouched

  sys.stdout.buffer.write(out)


if __name__ == "__main__":
  main()
