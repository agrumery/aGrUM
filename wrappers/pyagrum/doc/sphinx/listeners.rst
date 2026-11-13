Listeners
=========

aGrUM includes a mechanism for listening to actions (close to QT signal/slot). Some of them have been ported to pyAgrum :

LoadListener
------------
Listeners could be added in order to monitor the progress when loading a `pyagrum.BayesNet`

.. code-block:: python

   import pyagrum as gum

   # creating a new listener
   def foo(progress):
       if progress==200:
           print(' BN loaded ')
           return
       elif progress==100:
           car='%'
       elif progress%10==0:
           car='#'
       else:
           car='.'
       print(car,end='',flush=True)

   def bar(progress):
       if progress==50:
           print('50%')


   gum.loadBN('./bn.bif',listeners=[foo,bar])
   # .........#.........#.........#.........#..50%
   # .......#.........#.........#.........#.........#.........% | bn loaded


StructuralListener
------------------

Listeners could also be added when structural modification are made in a `pyagrum.BayesNet`:

.. code-block:: python

   import pyagrum as gum

   ## creating a BayesNet
   bn=gum.BayesNet()

   ## adding structural listeners
   bn.addStructureListener(whenNodeAdded=lambda n,s:print(f'adding {n}:{s}'),
                           whenArcAdded=lambda i,j: print(f'adding {i}->{j}'),
                           whenNodeDeleted=lambda n:print(f'deleting {n}'),
                           whenArcDeleted=lambda i,j: print(f'deleting {i}->{j}'))

   ## adding another listener for when a node is deleted
   bn.addStructureListener(whenNodeDeleted=lambda n: print('yes, really deleting '+str(n)))

   ## adding nodes to the BN
   l=[bn.add(item,3) for item in 'ABCDE']
   # adding 0:A
   # adding 1:B
   # adding 2:C
   # adding 3:D
   # adding 4:E

   ## adding arc to the BN
   bn.addArc(1,3)
   # adding 1->3

   ## removing a node from the BN
   bn.erase('C')
   # deleting 2
   # yes, really deleting 2

The listeners created by ``addStructureListener`` (of `pyagrum.BayesNet`, `pyagrum.InfluenceDiagram` and
`pyagrum.MarkovRandomField`) are instances of the following classes.

.. pyagrum-class:: pyagrum.PythonBNListener
   :exclude-members: whenArcAdded, whenArcDeleted, whenNodeAdded, whenNodeDeleted

.. pyagrum-class:: pyagrum.PythonMRFListener
   :exclude-members: whenEdgeAdded, whenEdgeDeleted, whenNodeAdded, whenNodeDeleted

ApproximationSchemeListener
---------------------------

A listener can be attached to any approximation-based inference engine (loopy propagation, sampling, etc.)
to monitor its progress step by step.

.. code-block:: python

   import pyagrum as gum

   ie = gum.GibbsSampling(gum.fastBN("A->B->C;A->C"))
   ie.setMaxIter(2000)
   ie.setPeriodSize(500)

   listen = gum.PythonApproximationListener(ie)

   def on_progress(step, error, duration):
       print(f"step {step:4d} | error={error:.2e} | {duration:.3f}s")

   def on_stop(message):
       print(f"Stopped: {message}")

   listen.setWhenProgress(on_progress)
   listen.setWhenStop(on_stop)

   ie.makeInference()
   # step  300 | error=... | 0.001s
   # step  800 | error=... | 0.002s
   # ...
   # Stopped: stopped with ...

.. pyagrum-class:: pyagrum.PythonApproximationListener
   :exclude-members: whenProgress, whenStop

DatabaseGenerationListener
--------------------------

A listener can be attached to a :class:`pyagrum.BNDatabaseGenerator` to monitor the progress of
database generation.

.. code-block:: python

   import pyagrum as gum

   bn = gum.fastBN("A->B->C;A->C")
   gen = gum.BNDatabaseGenerator(bn)

   listen = gum.PythonDatabaseGeneratorListener(gen)

   def on_progress(step, duration):  # step: the percentage of the database already generated
       if step % 25 == 0:
           print(f"{step}% generated in {duration:.3f}s")

   def on_stop(message):
       print(f"Done: {message}")

   listen.setWhenProgress(on_progress)
   listen.setWhenStop(on_stop)

   gen.drawSamples(500)
   # 0% generated in 0.000s
   # 25% generated in 0.000s
   # 50% generated in 0.000s
   # 75% generated in 0.000s
   # Done: Database of size 500(500) generated in ... seconds. Log2likelihood : ...

.. pyagrum-class:: pyagrum.PythonDatabaseGeneratorListener
   :exclude-members: whenProgress, whenStop
