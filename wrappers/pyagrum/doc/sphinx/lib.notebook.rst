Jupyter notebook with Graphical Models
======================================

``pyagrum.lib.notebook`` aims to facilitate the use of pyAgrum with jupyter notebook (or lab).

.. image:: _static/pyAgrum_all.png

Visualization of graphical models
---------------------------------

.. important::

    For many graphical representations functions, the parameter `size` is directly transferred to `graphviz`. Hence, Its format is a string containing an int. However if `size` ends in an exclamation point "!" (such as `size="4!"`), then `size` is taken to be the desired minimum size. In this case, if both dimensions of the drawing are less than size, the drawing is scaled up uniformly until at least one dimension equals its dimension in size.

    .. image:: _static/effect_of_Graphviz_size.png

.. autofunction:: pyagrum.lib.notebook.showBN
.. autofunction:: pyagrum.lib.notebook.getBN
.. autofunction:: pyagrum.lib.notebook.showInfluenceDiagram
.. autofunction:: pyagrum.lib.notebook.getInfluenceDiagram
.. autofunction:: pyagrum.lib.notebook.showMRF
.. autofunction:: pyagrum.lib.notebook.getMRF
.. autofunction:: pyagrum.lib.notebook.showCN
.. autofunction:: pyagrum.lib.notebook.getCN
.. autofunction:: pyagrum.lib.notebook.showInference
.. autofunction:: pyagrum.lib.notebook.getInference
.. autofunction:: pyagrum.lib.notebook.showJunctionTree
.. autofunction:: pyagrum.lib.notebook.getJunctionTree

Visualization of Tensors
---------------------------

.. autofunction:: pyagrum.lib.notebook.showProba
.. autofunction:: pyagrum.lib.notebook.getPosterior
.. autofunction:: pyagrum.lib.notebook.showPosterior
.. autofunction:: pyagrum.lib.notebook.getTensor
.. autofunction:: pyagrum.lib.notebook.showTensor

Visualization of graphs
-----------------------

.. autofunction:: pyagrum.lib.notebook.getDot
.. autofunction:: pyagrum.lib.notebook.showDot

.. autofunction:: pyagrum.lib.notebook.getGraph
.. autofunction:: pyagrum.lib.notebook.showGraph


Visualization of approximation algorithm
----------------------------------------

.. autofunction:: pyagrum.lib.notebook.animApproximationScheme


Helpers
-------

.. autofunction:: pyagrum.lib.notebook.configuration
.. autofunction:: pyagrum.lib.notebook.sideBySide
