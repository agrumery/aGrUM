For Windows
===========

  - install conda
  - conda install sphinx swig make(?)
  - git clone [aGrUM@gitlab]
  - git checkout documentation/swigDoc

How to document
===============
  http://www.sphinx-doc.org/en/stable/ext/example_numpy.html#example-numpy
  https://github.com/numpy/numpy/blob/master/doc/HOWTO_DOCUMENT.rst.txt

References to pyagrum objects in the docstrings (numpydoc convention, checked by ``DocstringReferencesTestSuite``):

  - in the prose, a pyagrum object is written with single backticks and its full name: ```pyagrum.BayesNet```,
    ```pyagrum.graphFromGUMString``` (no ``gum.`` alias). Sphinx links it (``default_role = "py:obj"``);
  - double backticks are for literal code only: ````pyagrum.config['learning', 'alpha']````, ````True````;
  - the type lines of the numpydoc sections stay plain (``bn : pyagrum.BayesNet``): Sphinx links them
    (``napoleon_preprocess_types``).
