import sys
import os
import re
import glob
import pickle


def parse_notebooks_rst(filepath: str) -> dict[str, list[str]]:
  sections: dict[str, list[str]] = {}
  current_section = None
  notebook_patterns: list[str] = []
  numsec = 1

  potential_section = ""
  with open(filepath, "r") as f:
    for n, line in enumerate(f):
      # Detect section titles (lines with '=' or '-' underlines)
      match = re.match(r"^[=]+$", line)
      if match:
        if potential_section == "":
          print(f"Warning: Empty section title at line {n + 1}")
        else:
          if current_section is not None:
            if len(notebook_patterns) == 0:
              print(f"Warning: No notebooks found for section '{current_section}' at line {n + 1}")
            sections[current_section] = sorted(notebook_patterns)
          current_section = f"{numsec}-{potential_section}"
          notebook_patterns: list[str] = []
          numsec += 1

      potential_section = line.strip()
      match = re.match(r"^.*(notebooks.*)$", line)
      if match:
        notebook_pattern = match.group(1).strip()
        for globbed in glob.glob(notebook_pattern):
          notebook_patterns.append(globbed.replace("ipynb", "md"))  # type: ignore

  # Catch last section if file does not end with newline
  if current_section and notebook_patterns:
    sections[current_section] = notebook_patterns

  return sections


def goForNotebooksSections(rst_path: str, pickle_path: str):
  print(f"Parsing {rst_path} for notebook sections...")
  sections_dict = parse_notebooks_rst(rst_path)
  print(f"Found {len(sections_dict)} sections.")
  with open(pickle_path, "wb") as f:
    pickle.dump(sections_dict, f)
  print(f"Sections saved to {pickle_path}")
  print(sections_dict)


def prepareSphinx(site_dir: str):
  # copy every file and folder in the current directory to the site_dir
  for item in os.listdir("."):
    if item not in {site_dir, "webbuild.py", "notebooks", "Makefile"}:
      src_path = os.path.join(".", item)
      dest_path = os.path.join(site_dir, item)
      if os.path.isdir(src_path):
        os.makedirs(dest_path, exist_ok=True)
        for sub_item in os.listdir(src_path):
          sub_src = os.path.join(src_path, sub_item)
          sub_dest = os.path.join(dest_path, sub_item)
          if os.path.isfile(sub_src):
            with open(sub_src, "rb") as fsrc:
              with open(sub_dest, "wb") as fdest:
                fdest.write(fsrc.read())
      else:
        with open(src_path, "rb") as fsrc:
          with open(dest_path, "wb") as fdest:
            fdest.write(fsrc.read())


def prepareNotebooks(site_dir: str):
  os.chdir("notebooks")
  # for all ipynb files, convert them to md files
  for notebook in glob.glob("*.ipynb"):
    md_file = notebook.replace(".ipynb", ".md")
    print(f"Converting {notebook} to {md_file}...")
    os.system(f"jupyter nbconvert --to markdown --output-dir=../{site_dir}/notebooks --output={md_file} {notebook}")
  os.chdir("..")


def chgIndexForWeb(site_dir: str):
  index_path = os.path.join(site_dir, "index.rst")
  if not os.path.exists(index_path):
    print(f"Error: {index_path} does not exist.")
    return

  buffer = ""
  with open(index_path, "r") as f:
    pred_line = ""
    skip = False
    for line in f:
      if line.startswith("<!-- SKIP"):
        skip = True
        continue
      if line.startswith("<!-- END SKIP"):
        skip = False
        continue
      if not skip:
        buffer += line

  with open(index_path, "w") as f:
    f.write(buffer)


def goSphinx(site_dir: str):
  os.chdir(site_dir)
  # Run Sphinx to build the site
  os.system("sphinx-build -M html . .")
  os.system("sphinx-build -M markdown . .")
  # move the folders html/_static and html/_images in the folder markdown
  os.system("mv html/_static/* markdown/_static/.")
  os.system("mv html/_images markdown/.")
  os.system("rm -rf html")
  os.chdir("..")


def goZipAndFinish(site_dir: str):
  # compress the site_dir into a zip file
  zip_file = f"{site_dir}.zip"
  os.system(f"zip -r {zip_file} {site_dir}")
  os.system(f"rm -rf {site_dir}")
  print(f"Sphinx build completed and zipped in {zip_file}.")


if __name__ == "__main__":
  if len(sys.argv) != 2:
    print("Usage: python webbuild.py <SITEDIR>")
    sys.exit(1)
  site_dir = sys.argv[1]
  print(f"Building site in {site_dir}...")
  # Ensure the site directory exists
  if not os.path.exists(site_dir):
    os.makedirs(site_dir)
    os.makedirs(os.path.join(site_dir, "data"), exist_ok=True)
    os.makedirs(os.path.join(site_dir, "notebooks"), exist_ok=True)
    os.makedirs(os.path.join(site_dir, "html"), exist_ok=True)

  goForNotebooksSections(rst_path="notebooks.rst", pickle_path=f"{site_dir}/data/notebooks_sections.pkl")
  prepareSphinx(site_dir)
  chgIndexForWeb(site_dir)
  goSphinx(site_dir)
  prepareNotebooks(site_dir)
  goZipAndFinish(site_dir)
