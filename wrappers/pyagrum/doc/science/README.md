# Science fragments of the pyAgrum documentation

One optional file `<class name>.rst` per documented class (`pyagrum.CPDAG.rst`, `pyagrum.ktbn.KTBNInference.rst`):
the references of the algorithms or models implemented by the class. It is shown in the page of the class, after its
docstring and its example, under the rubric "Scientific notes" (Sphinx extension `doc/sphinx/pyagrum_apipages.py`), its
citations as footnotes of the page.

- The docstrings hold no list of references: they are in these files (a docstring may still cite a reference with
  `:cite:` where the text needs it).
- `ScienceTestSuite` (`act test release pyAgrum`) checks that every file matches a documented class, that every cited
  key is in `doc/sphinx/refs.bib`, and that every entry of `refs.bib` has a `url` (or a `doi`).

## Format

- A short reStructuredText fragment, no title: what the class implements, and from which work, each reference cited
  by `:cite:` with its key in `refs.bib` (`:cite:`chickering1995transformational``).
- One paragraph or a short bullet list; no code (code goes to `doc/examples`).

## References (`doc/sphinx/refs.bib`)

- Only references checked to exist, with their exact authors, title, venue, year, volume, pages (from the publisher, a
  DOI resolver, dblp or the proceedings: never from memory).
- Every entry has a `url` to an online version: the open version when there is one (arXiv, proceedings site of
  PMLR/UAI/NeurIPS/JMLR, author page of a published version), otherwise the publisher page; and a `doi` when there
  is one.
- Key: `<first author lowercase><year><first significant word of the title>` (`meek1995causal`).
