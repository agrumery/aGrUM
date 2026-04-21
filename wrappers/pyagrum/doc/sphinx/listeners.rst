Listeners
=========

aGrUM includes a mechanism for listening to actions (close to QT signal/slot). Some of them have been ported to pyAgrum :

LoadListener
------------
Listeners could be added in order to monitor the progress when loading a pyagrum.BayesNet

>>> import pyagrum as gum
>>>
>>> # creating a new liseners
>>> def foo(progress):
>>>    if progress==200:
>>>        print(' BN loaded ')
>>>        return
>>>    elif progress==100:
>>>        car='%'
>>>    elif progress%10==0:
>>>        car='#'
>>>    else:
>>>        car='.'
>>>    print(car,end='',flush=True)
>>>
>>> def bar(progress):
>>>    if progress==50:
>>>        print('50%')
>>>
>>>
>>> gum.loadBN('./bn.bif',listeners=[foo,bar])
>>> # .........#.........#.........#.........#..50%
>>> # .......#.........#.........#.........#.........#.........% | bn loaded


StructuralListener
------------------

Listeners could also be added when structural modification are made in a pyagrum.BayesNet:

>>> import pyagrum as gum
>>>
>>> ## creating a BayesNet
>>> bn=gum.BayesNet()
>>>
>>> ## adding structural listeners
>>> bn.addStructureListener(whenNodeAdded=lambda n,s:print(f'adding {n}:{s}'),
>>>                         whenArcAdded=lambda i,j: print(f'adding {i}->{j}'),
>>>                         whenNodeDeleted=lambda n:print(f'deleting {n}'),
>>>                         whenArcDeleted=lambda i,j: print(f'deleting {i}->{j}'))
>>>
>>> ## adding another listener for when a node is deleted
>>> bn.addStructureListener(whenNodeDeleted=lambda n: print('yes, really deleting '+str(n)))
>>>
>>> ## adding nodes to the BN
>>> l=[bn.add(item,3) for item in 'ABCDE']
>>> # adding 0:A
>>> # adding 1:B
>>> # adding 2:C
>>> # adding 3:D
>>> # adding 4:E
>>>
>>> ## adding arc to the BN
>>> bn.addArc(1,3)
>>> # adding 1->3
>>>
>>> ## removing a node from the BN
>>> bn.erase('C')
>>> # deleting 2
>>> # yes, really deleting 2

ApproximationSchemeListener
---------------------------

A listener can be attached to any approximation-based inference engine (loopy propagation, sampling, etc.)
to monitor its progress step by step.

>>> import pyagrum as gum
>>>
>>> bn = gum.fastBN("A->B->C;A->C")
>>> ie = gum.LoopyBeliefPropagation(bn)
>>>
>>> listen = gum.PythonApproximationListener(ie)
>>>
>>> def on_progress(step, error, duration):
>>>     print(f"step {step:4d} | error={error:.2e} | {duration:.3f}s")
>>>
>>> def on_stop(message):
>>>     print(f"Stopped: {message}")
>>>
>>> listen.setWhenProgress(on_progress)
>>> listen.setWhenStop(on_stop)
>>>
>>> ie.makeInference()
>>> # step    1 | error=3.14e-01 | 0.001s
>>> # step    2 | error=1.02e-02 | 0.002s
>>> # ...
>>> # Stopped: stopped with epsilon=1e-06

DatabaseGenerationListener
--------------------------

A listener can be attached to a :class:`pyagrum.BNDatabaseGenerator` to monitor the progress of
database generation.

>>> import pyagrum as gum
>>>
>>> bn = gum.fastBN("A->B->C;A->C")
>>> gen = gum.BNDatabaseGenerator(bn)
>>>
>>> listen = gum.PythonDatabaseGeneratorListener(gen)
>>>
>>> def on_progress(step, duration):
>>>     if step % 100 == 0:
>>>         print(f"Generated {step} samples in {duration:.3f}s")
>>>
>>> def on_stop(message):
>>>     print(f"Done: {message}")
>>>
>>> listen.setWhenProgress(on_progress)
>>> listen.setWhenStop(on_stop)
>>>
>>> gen.drawSamples(500)
>>> # Generated 100 samples in 0.012s
>>> # Generated 200 samples in 0.023s
>>> # ...
>>> # Done: generation completed
