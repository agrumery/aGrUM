import pyagrum as gum

with open("data.csv", "w") as f:  # C is mostly "A or B"
  f.write("A,B,C\n" + "0,0,0\n0,1,1\n1,0,1\n1,1,1\n" * 40 + "0,0,1\n0,1,0\n1,0,0\n1,1,0\n" * 5)
learner = gum.BNLearner("data.csv")
learner.useGreedyHillClimbing().useScoreBIC()
state = learner.state()
state["Algorithm"][0], state["Score"][0], state["Size"][0]
# ('Greedy Hill Climbing', 'BIC', '(180,3)')
