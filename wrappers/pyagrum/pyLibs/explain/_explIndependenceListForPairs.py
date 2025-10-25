# Imports
import pyagrum as gum
import pylab
import matplotlib as mpl
import itertools

def _independenceListForPairs(bn, target=None):
  """
  returns a list of triples `(i,j,k)` for each non arc `(i,j)` such that `i` is independent of `j` given `k`.

  Parameters
  ----------
  bn: gum.BayesNet
    the Bayesian Network

  target: (optional) str or int
    the name or id of the target variable. If a target is given, only the independence given a subset of the markov blanket of the target are tested.

  Returns
  -------
  List[(str,str,List[str])]
    A list of independence found in the structure of BN.
  """

  def powerset(iterable):
    xs = list(iterable)
    # note we return an iterator rather than a list
    return itertools.chain.from_iterable(itertools.combinations(xs, n) for n in range(len(xs) + 1))

  # testing every d-separation
  l = []
  nams = sorted(bn.names())
  if target is None:
    firstnams = nams.copy()
    indepnodes = bn.names()
  else:
    indepnodes = {bn.variable(i).name() for i in gum.MarkovBlanket(bn, target).nodes()}
    if isinstance(target, str):
      firstnams = [target]
    else:
      firstnams = [bn.variable(target).name()]

  for i in firstnams:
    nams.remove(i)
    for j in nams:
      if not (bn.existsArc(i, j) or bn.existsArc(j, i)):
        for k in powerset(sorted(indepnodes - {i, j})):
          if bn.isIndependent(i, j, k):
            l.append((i, j, tuple(k)))
            break
  return l


def independenceListForPairs(bn, filename, target=None, plot=True, alphabetic=False):
  """
  get the p-values of the chi2 test of a (as simple as possible) independence proposition for every non arc.

  Parameters
  ----------
  bn : gum.BayesNet
    the Bayesian network

  filename : str
    the name of the csv database

  alphabetic : bool
    if True, the list is alphabetically sorted else it is sorted by the p-value

  target: (optional) str or int
    the name or id of the target variable

  plot : bool
    if True, plot the result

  Returns
  -------
    the list
  """

  learner = gum.BNLearner(filename, bn)
  vals = {}
  for indep in _independenceListForPairs(bn, target):
    vals[indep] = learner.chi2(*indep)[1]

  if plot:
    plotvals = dict()
    for indep in vals:
      key = "$" + indep[0] + " \\perp " + indep[1]
      if len(indep[2]) > 0:
        key += " \\mid " + ",".join(indep[2])
      key += "$"
      plotvals[key] = vals[indep]

    if not alphabetic:
      sortedkeys = sorted(plotvals, key=plotvals.__getitem__, reverse=False)
    else:
      sortedkeys = list(plotvals.keys())

    fig = pylab.figure(figsize=(10, 1 + 0.25 * len(plotvals)))
    ax = fig.add_subplot(1, 1, 1)
    ax.plot([plotvals[k] for k in sortedkeys], sortedkeys, "o")
    ax.grid(True)
    ax.vlines(x=0.05, ymin=-0.5, ymax=len(vals) - 0.5, colors="purple")
    ax.add_patch(mpl.patches.Rectangle((0, -0.5), 0.05, len(vals), color="yellow"))

  return vals