pyAgrum
=======

``pyAgrum`` is a scientific C++ and Python library dedicated to Bayesian 
networks and other Probabilistic Graphical Models. Based on the C++ Agrum 
library, it provides  a high-level interface to the C++ part of aGrUM allowing 
to create, manage and perform efficient computations with Bayesian networks and others probabilistic graphical models (Markov networks, influence diagrams and LIMIDs, dynamic BN, probabilistic relational models).

Sample code:

.. code:: python

    import pyAgrum as gum
    
    bn=gum.BayesNet('WaterSprinkler')
    print(bn)

Example
=======

.. code:: python

    import pyAgrum as gum

    # Creating BayesNet with 4 variables
    bn=gum.BayesNet('WaterSprinkler')
    print(bn)
    # Adding nodes the long way
    c=bn.add(gum.LabelizedVariable('c','cloudy ?',2))
    print(c)
    # Adding nodes the short way
    s, r, w = [ bn.add(name, 2) for name in "srw" ] 
    print (s,r,w)
    print (bn)
    # Addings arcs c -> s, c -> r, s -> w, r -> w
    bn.addArc(c,s)
    for link in [(c,r),(s,w),(r,w)]:
      bn.addArc(*link)
    print(bn)
    # Filling CPTs
    bn.cpt(c).fillWith([0.5,0.5])
    bn.cpt(s)[0,:]=0.5 # equivalent to [0.5,0.5]
    bn.cpt(s)[1,:]=[0.9,0.1]
    bn.cpt(w)[0,0,:] = [1, 0] # r=0,s=0
    bn.cpt(w)[0,1,:] = [0.1, 0.9] # r=0,s=1
    bn.cpt(w)[1,0,:] = [0.1, 0.9] # r=1,s=0
    bn.cpt(w)[1,1,:] = [0.01, 0.99] # r=1,s=1
    bn.cpt(r)[{'c':0}]=[0.8,0.2]
    bn.cpt(r)[{'c':1}]=[0.2,0.8]
    # Saving BN as a BIF file
    gum.saveBN(bn,"WaterSprinkler.bif")
    # Loading BN from a BIF file
    bn2=gum.loadBN("WaterSprinkler.bif")
    # Inference
    ie=gum.LazyPropagation(bn)
    ie.makeInference()
    print (ie.posterior(w))
    # Adding evidence
    ie.setEvidence({'s': 1, 'c': 0})
    ie.makeInference()
    print(ie.posterior(w))
    ie.setEvidence({'s': [0, 1], 'c': [1, 0]})
    ie.makeInference()
    print(ie.posterior(w))


LICENSE
=======

Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES
{prenom.nom}_at_lip6.fr                                               
                                                                      
This program is free software; you can redistribute it and/or modify  it under
the terms of the GNU General Public License as published by  the Free Software
Foundation; either version 2 of the License, or     (at your option) any later
version.                                   
                                                                      
This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.
                                                                      
You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA  02111-1307, USA.


Authors
=======

-  Pierre-Henri Wuillemin
-  Christophe Gonzales

Maintainers
===========

- Lionel Torti
- Gaspard Ducamp
