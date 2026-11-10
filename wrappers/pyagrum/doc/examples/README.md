# Examples of the pyAgrum documentation

One file `<page name>.py` per page of member or function of the Sphinx documentation (`doc/sphinx/api/`):
`pyagrum.DiGraph.addNode.py` for the method `addNode` documented by the class `pyagrum.DiGraph`, `pyagrum.fastBN.py`
for the function `pyagrum.fastBN`. A member inherited by several documented classes has one page, named after the most
general documented class: its example uses that class.

- The example is shown in the page of its member (Sphinx extension `doc/sphinx/pyagrum_apipages.py`).
- `ExamplesTestSuite` (`act test release pyAgrum`) checks that every page has an example, that every example matches a
  page, and runs every example (`testunits/tests/examplesRunner.py`).

## Format

- A plain, self-contained Python script (its own imports), without license header: it is shown as is.
- A comment block right after an expression shows its result: `# <repr of the value>` (`#` alone for an empty line),
  or the printed text for a `print(...)` call. The tests check these results exactly. Any other comment must not
  directly follow an expression: put it on the line before, or at the end of the code line.
- A first line `# requires: pandas, sklearn` names the optional modules needed by the example (not shown in the page;
  the example is skipped by the tests when one of them is missing).
- The example runs in an empty temporary directory: files are written and read there, with relative names.

Checking examples while writing them:

    python ../../testunits/tests/examplesRunner.py pyagrum.DiGraph.addNode.py ...
