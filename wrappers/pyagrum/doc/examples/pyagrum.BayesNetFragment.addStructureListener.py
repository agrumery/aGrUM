import pyagrum as gum

bn = gum.fastBN("A->B->C")
fragment = gum.BayesNetFragment(bn)
events = []
fragment.addStructureListener(
  whenNodeAdded=lambda i, name: events.append(f"node {name} installed"),
  whenArcAdded=lambda i, j: events.append(f"arc {i}->{j} installed"),
)
fragment.installNode("A")
fragment.installNode("B")  # the arc A->B of bn is installed with B
events
# ['node A installed', 'node B installed', 'arc 0->1 installed']
