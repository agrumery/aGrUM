pyAgrum.lib.notebook
====================

``pyAgrum.lib.notebook`` aims to facilitate the use of pyAgrum with jupyter notebook (or lab).

.. image:: _static/pyAgrum_all.png

Visualization of graphical models
---------------------------------

.. important::

    For many graphical representations functions, the parameter `size` is directly transferred to `graphviz`. Hence, Its format is a string containing an int. However if `size` ends in an exclamation point "!" (such as `size="4!"`), then `size` is taken to be the desired minimum size. In this case, if both dimensions of the drawing are less than size, the drawing is scaled up uniformly until at least one dimension equals its dimension in size.

    .. image:: _static/effect_of_Graphviz_size.png

.. autofunction:: pyAgrum.lib.notebook.showBN
.. autofunction:: pyAgrum.lib.notebook.getBN
.. autofunction:: pyAgrum.lib.notebook.showInfluenceDiagram
.. autofunction:: pyAgrum.lib.notebook.getInfluenceDiagram
.. autofunction:: pyAgrum.lib.notebook.showMN
.. autofunction:: pyAgrum.lib.notebook.getMN
.. autofunction:: pyAgrum.lib.notebook.showCN
.. autofunction:: pyAgrum.lib.notebook.getCN
.. autofunction:: pyAgrum.lib.notebook.showInference
.. autofunction:: pyAgrum.lib.notebook.getInference
.. autofunction:: pyAgrum.lib.notebook.showJunctionTree
.. autofunction:: pyAgrum.lib.notebook.getJunctionTree
.. autofunction:: pyAgrum.lib.notebook.showInformation
.. autofunction:: pyAgrum.lib.notebook.getInformation

Visualization of Potentials
---------------------------

.. autofunction:: pyAgrum.lib.notebook.showProba
.. autofunction:: pyAgrum.lib.notebook.getPosterior
.. autofunction:: pyAgrum.lib.notebook.showPosterior
.. autofunction:: pyAgrum.lib.notebook.getPotential
.. autofunction:: pyAgrum.lib.notebook.showPotential

Exporting visualisations
-------------------------

.. autofunction:: pyAgrum.lib.notebook.export
.. autofunction:: pyAgrum.lib.notebook.exportInference

Visualization of graphs
-----------------------

.. autofunction:: pyAgrum.lib.notebook.getDot
.. autofunction:: pyAgrum.lib.notebook.showDot

.. autofunction:: pyAgrum.lib.notebook.getGraph
.. autofunction:: pyAgrum.lib.notebook.showGraph


Visualization of approximation algorithm
----------------------------------------

.. autofunction:: pyAgrum.lib.notebook.animApproximationScheme


Helpers
-------

.. autofunction:: pyAgrum.lib.notebook.configuration
.. autofunction:: pyAgrum.lib.notebook.sideBySide
