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

import glob
import json
import os
import re
import shlex
import shutil
import subprocess
import sys
import zipfile
from pathlib import Path


def notif(message: str) -> None:
  """Prints a notification message to the console."""
  print(f"� {message}")  # Green text for notifications


def run(cmd: list[str], cwd: str | None = None) -> None:
  """Runs a command and raises if it fails."""
  notif(f"Running {shlex.join(cmd)}")
  subprocess.run(cmd, cwd=cwd, check=True)


def copy_tree(src: str, dest: str) -> None:
  """Copies the folder src into dest (merged with the existing content of dest)."""
  notif(f"Copying {src} to {dest}")
  shutil.copytree(src, dest, dirs_exist_ok=True)


def parse_notebooks_rst(filepath: str) -> dict[str, list[str]]:
  sections: dict[str, list[str]] = {}
  current_section = None
  notebook_patterns: list[str] = []
  numsec = 1

  potential_section = ""
  with open(filepath) as f:
    for n, line in enumerate(f):
      # Detect section titles (lines with '=' or '-' underlines)
      match = re.match(r"^=+$", line)
      if match:
        if potential_section == "":
          notif(f"Warning: Empty section title at line {n + 1}")
        else:
          if current_section is not None:
            if len(notebook_patterns) == 0:
              notif(f"Warning: No notebooks found for section '{current_section}' at line {n + 1}")
            sections[current_section] = sorted(notebook_patterns)
          current_section = f"{numsec}-{potential_section}"
          notebook_patterns: list[str] = []
          numsec += 1

      potential_section = line.strip()
      match = re.match(r"^.*(notebooks.*)$", line)
      if match:
        notebook_pattern = match.group(1).strip()
        # only the notebooks themselves (not the leftovers such as *.ipynb.orig)
        for globbed in glob.glob(notebook_pattern):
          if globbed.endswith(".ipynb"):
            notebook_patterns.append(globbed.removesuffix(".ipynb") + ".md")

  # Catch last section if file does not end with newline
  if current_section and notebook_patterns:
    sections[current_section] = sorted(notebook_patterns)

  return sections


def parse_thumbnails_html(filepath: str) -> dict[str, str]:
  """Parse the HTML file to extract notebook names and their corresponding thumbnail paths."""
  start = False
  key = None
  value = None
  res = dict()

  with open(filepath) as file:
    for line in file:
      if '<div class="nbsphinx-gallery">' in line:
        start = True
        continue
      if "<footer>" in line:
        break
      if not start:
        continue
      if key is None:
        if "reference internal" in line and "notebooks/" in line:
          key = line.split("notebooks/")[1].split(".html")[0]
        continue
      else:
        if "noLogo.png" in line:
          value = "_static/noLogo.png"
        elif "nbsphinx-broken-thumbnail" in line:
          value = "_static/nbsphinx-broken-thumbnail.svg"
        else:
          value = "_images/" + line.split("_images/")[1].split('"></div')[0]
        res[key] = value
        key = None
        value = None

  return res


def retrieveNotebooksSections(rst_path: str, json_path: str):
  notif(f"Parsing {rst_path} for notebook sections...")
  sections_dict = parse_notebooks_rst(rst_path)
  notif(f"Found {len(sections_dict)} sections.")

  with open(json_path, "w") as fp:
    json.dump(sections_dict, fp, indent=2)
  notif(f"Sections saved to {json_path}")


def retrieveCiteTitles(rst_path: str, bib_path: str, json_path: str):
  """Save the short title (before the first colon) of every work cited by :cite:t: in the section titles of rst_path.

  The website needs it since :cite:t: roles are not resolved in notebooks_sections.json.
  """
  keys = set()
  with open(rst_path) as f:
    for line in f:
      keys.update(re.findall(r":cite:t:`([^`]+)`", line))

  with open(bib_path) as f:
    bib = f.read()

  titles: dict[str, str] = {}
  for key in sorted(keys):
    entry = re.search(r"@\w+\{" + re.escape(key) + r",(.*?)\n\}", bib, re.DOTALL)
    title = re.search(r"title\s*=\s*\{(.*)\}\s*,", entry.group(1)) if entry else None
    if title is None:
      notif(f"Warning: no title found in {bib_path} for citation '{key}'")
      continue
    titles[key] = title.group(1).replace("{", "").replace("}", "").split(":")[0].strip()

  with open(json_path, "w") as fp:
    json.dump(titles, fp, indent=2)
  notif(f"Found {len(titles)} cited titles, saved to {json_path}")


def retrieveThumbnails(RTD_path: str, json_path: str):
  res = parse_thumbnails_html(RTD_path)
  notif(f"Found {len(res)} thumbnails.")

  with open(json_path, "w") as fp:
    json.dump(res, fp, indent=2)
  notif(f"Thumbnails saved to {json_path}")


def parse_changelog() -> dict[str, str]:
  """Parse CHANGELOG.md and extract version descriptions"""

  changelog_path = "../../../../CHANGELOG.md"
  changelog = Path(changelog_path).read_text(encoding="utf-8")
  tabs = changelog.split("## Changelog for ")[1:]
  return {lines[0].strip(): "\n".join(lines[1:]).strip() for lines in (t.split("\n") for t in tabs)}


def getTagVersion(datadir: str):
  notif("Retrieving tag versions and descriptions from CHANGELOG.md...")
  descriptions = parse_changelog()

  # Get all tags with their dates and messages
  cmd = [
    "git",
    "for-each-ref",
    "--sort=-creatordate",
    "--format=%(refname:short)|%(creatordate:short)|%(contents:subject)",
    "refs/tags",
  ]
  result = subprocess.run(cmd, capture_output=True, text=True, check=True)

  tags_info = []
  for line in result.stdout.splitlines():
    if not line.strip():
      continue

    parts = line.split("|", 2)
    if len(parts) >= 3:
      tag, date_iso, _ = parts
      if tag not in descriptions:  # we just add tag if documented in CHANGELOG.md
        continue

      tags_info.append({"tag": tag, "date": date_iso, "description": descriptions[tag]})
  # Save to JSON file
  changelog = os.path.join(datadir, "CHANGELOG.json")
  with open(changelog, "w") as f:
    json.dump(tags_info, f, indent=2)
  notif(f"Tag versions and descriptions saved to {changelog}.")


def prepareSphinx(site_dir: str):
  # copy every file and folder in the current directory to the site_dir (generated/ and api/ are rebuilt by Sphinx)
  for item in os.listdir("."):
    if item in {
      site_dir,
      "webbuild.py",
      "Makefile",
      "notebooks",
      "notebooks.rst",
      "generated",
      "api",
      "_build",
      "__pycache__",
    }:
      continue
    if item.endswith(".zip"):  # do not copy zip files
      continue
    if os.path.isdir(item):
      copy_tree(item, os.path.join(site_dir, item))
    else:
      shutil.copy2(item, os.path.join(site_dir, item))


def prepareNotebooks(site_dir: str):
  notebooks = sorted(glob.glob("notebooks/*.ipynb"))
  notif(f"Converting {len(notebooks)} notebooks to markdown...")
  run(["jupyter", "nbconvert", "--to", "markdown", f"--output-dir={os.path.join(site_dir, 'notebooks')}", *notebooks])


def chgIndexForWeb(site_dir: str):
  index_path = os.path.join(site_dir, "index.rst")
  if not os.path.exists(index_path):
    notif(f"Error: {index_path} does not exist.")
    return

  buffer = ""
  with open(index_path) as f:
    skip = False
    for line in f:
      if line.startswith(".. SKIP WEBSITE"):
        skip = True
        continue
      if line.startswith(".. END SKIP WEBSITE"):
        skip = False
        continue
      if not skip:
        buffer += line

  with open(index_path, "w") as f:
    f.write(buffer)


def goRTD(site_dir: str):
  notif("Running Sphinx to build the site (RTD version)...")
  # the sources are the documentation itself (site_dir is excluded in conf.py)
  run(["sphinx-build", "-M", "html", "..", "RTD", "-j", "auto"], cwd=site_dir)
  # copy the folders html/_static and html/_images in the folder markdown
  for folder in ("_static", "_images"):
    copy_tree(os.path.join(site_dir, "RTD", "html", folder), os.path.join(site_dir, "markdown", folder))


# doc/, resolved before any chdir (__file__ may be relative)
_DOC = Path(__file__).resolve().parent.parent

# the only folders of site_dir read by the website (pyagrum.gitlab.io, scripts/importDocs.py)
_WEB_FOLDERS = ("data", "markdown", "notebooks")


def goSphinx(site_dir: str):
  notif("Running Sphinx to build the site (markdown)...")
  # the sources are a copy in site_dir: the examples and the science fragments are given by their absolute path
  dirs = [
    arg
    for name, folder in (("pyagrum_examples_dir", "examples"), ("pyagrum_science_dir", "science"))
    for arg in ("-D", f"{name}={_DOC / folder}")
  ]
  # no html build: its _static and _images are already given by goRTD
  run(["sphinx-build", "-M", "markdown", ".", ".", "-j", "auto", *dirs], cwd=site_dir)
  # written by pyagrum_apipages.py: the navigation between the member pages of a class
  shutil.move(os.path.join(site_dir, "markdown", "api_navigation.json"), os.path.join(site_dir, "data"))
  # written by pyagrum_markdown.py: the sub-pages of each page, grouped by section
  shutil.move(os.path.join(site_dir, "markdown", "doc_structure.json"), os.path.join(site_dir, "data"))


def updateMarkdownFiles(site_dir: str):
  # in every *.md file of the markdown folder, "```pycon" is replaced by "```python"
  markdown_dir = os.path.join(site_dir, "markdown")
  for md_file in glob.glob(os.path.join(markdown_dir, "**", "*.md"), recursive=True):
    with open(md_file) as f:
      content = f.read()
    if "```pycon" not in content:
      continue
    with open(md_file, "w") as f:
      f.write(content.replace("```pycon", "```python"))


def goZipAndFinish(site_dir: str):
  # only the folders read by the website are zipped (the rest of site_dir is ~95% of its size)
  zip_file = f"{site_dir}.zip"
  with zipfile.ZipFile(zip_file, "w", zipfile.ZIP_DEFLATED) as zf:
    for folder in _WEB_FOLDERS:
      for path in sorted(Path(site_dir, folder).rglob("*")):
        zf.write(path)
  shutil.rmtree(site_dir)  # raises if the folder cannot be removed
  notif(f"Sphinx build completed and zipped in {zip_file}, {site_dir} removed.")


if __name__ == "__main__":
  if len(sys.argv) != 1:
    notif("Usage: python webbuild.py")
    sys.exit(1)
  # the name is fixed: conf.py excludes "site" from the sources of the documentation
  site_dir = "site"

  # every path is relative to the folder of this script
  os.chdir(_DOC / "sphinx")

  notif(f"Building site in {site_dir}...")

  if os.path.exists(site_dir):
    notif(f"Removing the previous {site_dir}...")
    shutil.rmtree(site_dir)
  os.makedirs(os.path.join(site_dir, "data"))
  os.makedirs(os.path.join(site_dir, "notebooks"))

  goRTD(site_dir)
  retrieveNotebooksSections(rst_path="notebooks.rst", json_path=f"{site_dir}/data/notebooks_sections.json")
  retrieveCiteTitles(rst_path="notebooks.rst", bib_path="refs.bib", json_path=f"{site_dir}/data/cite_titles.json")
  retrieveThumbnails(
    RTD_path=os.path.join(site_dir, "RTD/html/notebooks.html"),
    json_path=f"{site_dir}/data/thumbnails.json",
  )
  getTagVersion(os.path.join(site_dir, "data"))

  notif("Cleaning RTD directory...")
  shutil.rmtree(os.path.join(site_dir, "RTD"))

  prepareSphinx(site_dir)
  chgIndexForWeb(site_dir)
  goSphinx(site_dir)
  updateMarkdownFiles(site_dir)
  prepareNotebooks(site_dir)
  goZipAndFinish(site_dir)
