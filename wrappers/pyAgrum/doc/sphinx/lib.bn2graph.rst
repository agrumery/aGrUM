Module bn2graph
===============

A module to graphically display Bayesian networks using `pydotplus <https://pypi.org/project/pydotplus/>`_ (and then `graphviz <https://graphviz.org/>`_).

.. figure:: _static/bn2graph_test.png
    :align: right
    :alt: bn2graph_test.png
    :width: 170

.. code-block:: python
  :linenos:

  import pyAgrum as gum
  from pyAgrum.lib.bn2graph import BN2dot

  bn = gum.fastBN("a->b->d;a->c->d[3]->e;f->b")
  g = BN2dot(bn,
             nodeColor={'a': 1,
                        'b': 0.3,
                        'c': 0.4,
                        'd': 0.1,
                        'e': 0.2,
                        'f': 0.5},
             arcColor={(0, 1): 0.2,
                       (1, 2): 0.5},
             arcWidth={(0, 3): 0.4,
                       (3, 2): 0.5,
                       (2,4) :0.6})

  g.write("bn2graph_test.png", format='png')


Visualization of Potentials
---------------------------

.. autofunction:: pyAgrum.lib.bn2graph.proba2histo

Visualization of Bayesian networks
----------------------------------

.. autofunction:: pyAgrum.lib.bn2graph.BN2dot
.. autofunction:: pyAgrum.lib.bn2graph.BNinference2dot

Hi-level functions
------------------
.. autofunction:: pyAgrum.lib.bn2graph.dotize
.. autofunction:: pyAgrum.lib.bn2graph.pngize
.. autofunction:: pyAgrum.lib.bn2graph.pdfize
