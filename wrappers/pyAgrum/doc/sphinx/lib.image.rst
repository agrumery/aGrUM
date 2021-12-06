pyAgrum.lib.image
=================

`pyAgrum.lib.image` aims to graphically export models and inference using `pydot <https://pypi.org/project/pydot/>`_ (and then `graphviz <https://graphviz.org/>`_).

For more details, <https://lip6.fr/Pierre-Henri.Wuillemin/aGrUM/docs/last/notebooks/colouringAndExportingBNs.ipynb.html>

.. figure:: _static/bn2graph_test.png
    :align: right
    :alt: bn2graph_test.png
    :width: 170

.. code-block:: python
  :linenos:

  import pyAgrum as gum
  from pyAgrum.lib.image as gumimage

  bn = gum.fastBN("a->b->d;a->c->d[3]->e;f->b")
  gumimage.export(bn,"out/test_export.png",
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


Visualization of models and inference
-------------------------------------

.. autofunction:: pyAgrum.lib.image.export
.. autofunction:: pyAgrum.lib.image.exportInference
