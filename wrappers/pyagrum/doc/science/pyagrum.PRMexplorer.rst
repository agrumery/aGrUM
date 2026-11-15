PRMexplorer reads a probabilistic relational model written in the O3PRM language and lists its elements; it does
not perform inference. A probabilistic relational model :cite:`koller1998probabilistic` describes a Bayesian network
by classes: a class has attributes (random variables with their conditional probability tables) and reference slots
towards other classes, and an attribute may depend, through slot chains, on the attributes of related objects (when a
slot reaches several objects, an aggregate summarizes their values); a system instantiates the classes and links the
objects, which defines a Bayesian network. The object-oriented features of the language are those of
:cite:`torti2010reinforcing`: attribute types with type inheritance (a type refines the values of its super type),
class inheritance, and interfaces implemented by classes. PRMexplorer exposes the types, interfaces, classes and
systems of the model, with their attributes, references, slot chains, aggregates and inheritance relations.
