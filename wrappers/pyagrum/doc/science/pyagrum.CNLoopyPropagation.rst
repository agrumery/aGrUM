CNLoopyPropagation implements L2U, the loopy version :cite:`ide2008approximate` of the 2U algorithm, which propagates
probability intervals exactly in polytree-shaped credal networks with binary variables
:cite:`fagiuoli19982u`. It works on binary credal networks; a network with non-binary variables is first binarized
(``CredalNet.approximatedBinarization``) :cite:`antonucci2010generalized`, which gives outer (larger) bounds.
