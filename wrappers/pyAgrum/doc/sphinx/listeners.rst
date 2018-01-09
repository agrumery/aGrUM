Listeners
=========

aGrUM includes a mechanism for listening to actions (close to QT signal/slot). Some of them have been ported to pyAgrum :

LoadListener
------------
Listeners could be added in order to monitor the progress when loading gum.BayesNet 

>>> import pyAgrum as gum
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

Listeners could also be added when structural modification are made in a gum.BayesNet:

>>> import pyAgrum as gum
>>>
>>> ## creating a BayesNet
>>> bn=gum.BayesNet()
>>>
>>> ## adding structural listeners
>>> bn.addStructureListener(whenNodeAdded=lambda n,s:print('adding {}:{}'.format(n,s)),
>>>                         whenArcAdded=lambda i,j: print('adding {}->{}'.format(i,j)),
>>>                         whenNodeDeleted=lambda n:print('deleting {}'.format(n)),
>>>                         whenArcDeleted=lambda i,j: print('deleting {}->{}'.format(i,j)))
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

DatabaseGenerationListener
--------------------------