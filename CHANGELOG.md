# aGrUM Changelog

## Changelog for next version

- pyAgrum

    - New `randomShuffle` and `randomGamma` (the draws of aGrUM, the same on every platform for a given seed).
    - `initRandom` now reaches the draws of all the modules (`KTBNGenerator`, ...: each module had its own generator,
      with a fixed seed). A new `pyagrum.ini` key `core.randomSeed` gives the seed at start (see aGrUM below) and
      `randomGeneratorSeed()` returns the seed in use. `import pyagrum` no longer calls `initRandom(0)`.
    - New `randomNumpyGenerator()`: a `numpy.random.Generator` whose seed is drawn from the generator of aGrUM.
      `pyagrum.clg` (`randomCLG`, `ForwardSampling`, `CLGLearner`), `pyagrum.bnmixture` (bootstrap weights) and the
      beeswarm plot of `pyagrum.explain` now draw with it, so that `initRandom` makes them reproducible (they used
      Python `random` or the global generator of numpy). The argument `seed` of `ForwardSampling.makeSample` is
      deprecated after 3.3.2 (use `initRandom`).
    - Configuration of pyAgrum reorganized in lowerCamelCase, with sections by subject; the former names are
      deprecated after 3.3.2 and still work: `notebook.tensor_*` → `tensor.*` (`tensor_visible_digits` →
      `tensor.visibleDigits`...), `notebook.histogram_*` → `histogram.*`, `notebook.graphdiff_*` → `graphdiff.*`, the
      other keys of `notebook`, `factorgraph`, `influenceDiagram`, `credalnet`, `causal`, `ROC`, `ctbn` and `bnmixture`
      in lowerCamelCase (`notebook.graph_format` → `notebook.graphFormat`, `ctbn.graph_size` → `ctbn.graphSize`...).
      `bnmixture.line_size` and `ctbn.show_latent_names`, `ctbn.latent_bgcolor`, `ctbn.latent_fgcolor`, no longer
      used, are deprecated after 3.3.2. The arcs of the drawings of `bnmixture` use `bnmixture.arcCmap` (it was read
      from `notebook.arcCmap`). The color of the upper bounds in the histograms of a credal network is the new key
      `credalnet.histoMaxColor` (it was fixed; `credalnet.histo_max_color`, never read, now redirects to it).
    - `showROC_PR(bgcolor=...)` now colors the background of the figure (it was ignored).
    - The keys of the configuration are displayed as they are declared (`maxNumberOfThreads`, it was `maxnumberofthreads`),
      in `print(pyagrum.config)`, `grep`, `diff`, `schema()` and in the messages; they are still found whatever their case.
    - The graphs have `saveGUM`/`saveGUMstring` and are pickled through the GUM format: the isolated nodes, the names
      and the marks were lost, and `CPDAG`, `MPDAG`, `MAG`, `EssentialGraph` could not be pickled. New
      `graphFromGUMString`, `loadGraph` and `saveGraph` (format given by the extension, as `loadBN`/`saveBN`: `.jgum`,
      `.bgum` or `.pkl`, see the new `availableGraphExts`). Reading a graph pickled by pyAgrum 3.3.2 or before is
      deprecated after 3.3.2.
    - `MarkedGraph`, `fastMarkedGraph` and `fastPAG`; the marks of a `PAG` are `pyagrum.EdgeMark` values (an `IntEnum`:
      the constants `EdgeMark_Circle`, `EdgeMark_Tail` and `EdgeMark_Arrowhead` still work).
    - `saveGUM`/`loadGUM` of `BayesNet`, `InfluenceDiagram`, `MarkovRandomField` and of the graphs: by default
      (`binary=None`), the format is given by the extension, `.jgum` for the text format, `.bgum` or any other
      extension for the binary one; an explicit `binary` contradicting `.jgum`/`.bgum` raises `InvalidArgument`.
    - New module `pyagrum.graph` (`import pyagrum.graph as gg`): the algorithms on graphs, one function per algorithm
      whatever the kind of graph, each accepting a family of graphs given by what it reads (the arcs, the edges, both,
      or the marked edges): `topologicalOrder`, `hasDirectedCycle`, `ancestors`, `descendants`, `directedPath`,
      `hasDirectedPath`, `hasUndirectedCycle`, `isChordal`, `lexBFS`, `maximumCardinalitySearch`,
      `isPerfectEliminationOrdering`, `undirectedPath`, `hasPartiallyDirectedCycle`, `mixedOrientedPath`,
      `degeneracyOrdering`, `anteriors`, `isAncestral`, `isMaximal`, `cpdag`, `consistentExtension`, `meekClosure`,
      `orientAsDAG`, and `separation` (d-, c- or m-separation according to the class of the graph). The immutable
      graphs (`CPDAG`, `MPDAG`, `EssentialGraph`, `MAG`) are accepted as the other graphs. Nodes by id or by name. New
      notebook `03-Tutorial_graphs`.
    - `BayesNet.changeTensor` is removed: use `bn.cpt(node).fillWith(tensor)`.
    - Names between single quotes in every *fast* syntax (`fastBN`, `fastID`, `fastMRF`, `fastVariable`, `fastGraph`
      and the typed `fast*Graph`, `fastMarkedGraph`, `fastPAG`, `add`): `fastBN("'Visit Asia'{yes|no}->'X-ray'")`; a
      doubled quote stands for a quote (`'l''eau'`). A quote opens a quoted name only at the beginning of a name
      (`O'Brien` and the labels `{l'eau|x}` are unchanged). A circle may touch a quoted name (`fastPAG("'A'o->'B'")`).
      New `quoteFastName` and `fastNameNeedsQuotes`.
    - New `checkFastSyntax` (and `FastSyntaxCheck`, `FastSyntax`): the syntaxes for which a fast description is valid
      (`"A->B"` is a directed graph, a Bayesian network and an influence diagram), with the error for the others; every
      syntax is checked when the object is built (it is never modified, hence it can be read by several threads). Valid
      means accepted by the fast function: an arc given twice (`fastBN`, `fastID`, `DuplicateElement`), an empty factor
      or a factor given twice (`fastMRF`, `InvalidArgument`) are refused by the analysis of the description.
      `fastGraph` no longer parses the description in Python: it uses `checkFastSyntax`, and returns a `MarkedGraph`
      for a description with marks (`"A o-> B"`, it raised an error).
    - Behaviour change: a malformed fast description (an operator without a node, an unclosed quote, a misplaced `<` or
      `>`, a number which cannot be read, a variable without a name) raises `InvalidArgument` in every fast syntax
      (`fastBN("A->")` raised `InvalidArc`, `fastVariable("A[x]")` a Python `ValueError`, `fastBN("A<->B")` created a
      variable `A<`, `fastBN("A->[3]")` a variable without a name). `InvalidArc` and `InvalidEdge` are kept for an arc
      or an edge that the target does not support.
    - `toFast()` of `BayesNet`, `InfluenceDiagram` and `MarkovRandomField` is computed in C++, in the order of the node
      ids; `toFast(filename)` writes a Python file which can be run (it lacked `import pyagrum`).
    - Behaviour change: `toFast()` (of the variables, `BayesNet`, `InfluenceDiagram`, `MarkovRandomField`) quotes a
      name which would not be read back as is otherwise (`'X-ray'[3]`), so that `fastBN(bn.toFast())` gives back the
      names. **The files saved by pyAgrum (`.jgum`, `.bgum`, `.bifxml`, pickle...) write such names quoted, and
      pyAgrum 3.3.2 or older reads them with the quotes in the name** (this includes the variables `X[0]` of a KTBN).
      The simple names are written as before.
    - A parameter of an approximation changed after a computation (`setMaxIter`, `setEpsilon`, `setBurnIn`...) is taken
      into account by the next one: approximate inferences (`GibbsSampling`, `MonteCarloSampling`, `ImportanceSampling`,
      `WeightedSampling`, `LoopyBeliefPropagation`, `Loopy*Sampling`), `CNLoopyPropagation`, `GibbsBNdistance` and
      `MCBNDistance` returned the results computed with the former parameters.
    - `compute(lazy=False)` of `ExactBNdistance`, `GibbsBNdistance` and `MCBNDistance` forces a new computation (the
      Bayesian networks are only referenced: a change made in place is not detected).
    - `enableEpsilon`/`disableEpsilon`/`isEnabledEpsilon` (and the same for `MinEpsilonRate`, `MaxIter` and
      `MaxTime`) for the approximate inferences, the credal inferences and `BNLearner`, as for the distances.
    - `pyagrum.graph`: a node given by a `bool` or a `float` raises `TypeError` (it was silently converted), the domain
      sizes of `orientAsDAG` may be given by name, and every function names the accepted classes in its `TypeError`.
    - `pyagrum.lib.notebook` warns (`warnings.warn`) instead of printing when Graphviz is missing or when the obsolete
      `title` of `FlowLayout` is used; an invalid dot string raises `ValueError` in `pyagrum.lib.dotstyle`.
    - `addStructureListener` (`BayesNet`, `InfluenceDiagram`, `MarkovRandomField`) returns the new listener
      (`PythonBNListener` or `PythonMRFListener`, None without function), on which other functions can be set.
      `PythonApproximationListener` takes the approximation algorithm itself (an inference, ...). The four listener
      classes are documented in the reference, with an example for each of their setters.
    - New `RandomGeneratorGuard`: saves the state of the pseudo-random generator and restores it (`restore()` or at the
      end of a `with` block, even on an exception), so that a block can call `initRandom` and draw without changing
      the draws of the rest of the program.
    - `pyagrum.qBNSampling` no longer depends on `qiskit-ibm-runtime`: `qBNMC.aerSimulation` uses the sampler of
      `qiskit-aer`.
    - `counterfactual`: the abduction replaced each parentless variable by its own posterior, losing the dependence
      created by the profile between them (wrong result for instance when Y = U1 xor U2 xor T). The counterfactual is
      now a posterior in the twin network of Balke and Pearl (`counterfactualModel(cm, whatif)` builds it, with copies
      `V*` of the whatif variables and of their descendants), which is exact. A causal model with latent variables is
      refused (their distributions are unknown; they were ignored). Deprecated after 3.3.2: the argument `profile` of
      `counterfactualModel` and `Counterfactual.counterFactualModel` (ignored, the twin network does not depend on
      it) and `Counterfactual.impact`.
    - `Tensor.randomCPT(varId=0)` takes the position of the conditioned variable. The random CPTs (`fastBN`,
      `randomBN`, `BNGenerator`, `BayesNet.generateCPTs`) are drawn from a Dirichlet of parameter 1 (see aGrUM).
    - `skbn.BNClassifier` with `learningMethod="TAN"` learns a selective TAN (Blanco et al., 2005): the features and
      the edges of the tree are selected by G-tests at the level of the new parameter `TANSignificance` (by default,
      the new key `skbn.tanSignificance` of `pyagrum.config`, 0.01); `TANSignificance=1` gives the TAN of Friedman et
      al. (all the features, a spanning tree). Behaviour change: the features independent of the class are no longer
      used, and the edges are no longer pruned below half of the mean conditional mutual information.

- aGrUM

    - New `gum::RandomGeneratorGuard`: saves the seed and the state of the generator of aGrUM, restored by `restore()`
      or by its destructor.
    - Behaviour change: `ExactBNdistance`, `MCBNDistance` and `GibbsBNdistance` return an infinite `klPQ` (resp.
      `klQP`) when a configuration has a positive probability in P and a null one in Q (resp. the reverse), instead of
      the sum over the other configurations, which could even be negative; `errorPQ` (resp. `errorQP`) still counts
      these configurations.
    - Behaviour change: `ShaferShenoyLIMIDInference` makes the no-forgetting assumption when the decisions are totally
      ordered (a classical influence diagram), as with `addNoForgettingAssumption` in this order: the strategy is the
      optimal one of the influence diagram (the policies may have more parents). For a LIMID, the solubility check
      compared only the decisions of a same level: some non soluble LIMIDs were accepted, and their strategy was not
      guaranteed to be optimal; the extremality is now checked against all the remaining decisions.
    - Behaviour change: the defaults of `GibbsSampling` and `LoopyGibbsSampling` are now those of the other samplers,
      `gibbs.epsilon` = 0.01 and `gibbs.minEpsilonRate` = 1e-5 (they were exp(-1.6) = 0.2 and exp(-5) = 0.0067, a
      legacy of a former error: the sampling stopped at the end of the burn-in, with posteriors at about 0.1). The
      inference is more precise and longer (about 38000 samples instead of 300).
    - Behaviour change: the random generator is now initialized at start from the new key `core.randomSeed` (default
      0, the clock): the draws of a program change at each run, unless it calls `initRandom` or sets this key in
      `agrum.ini`. The key is read once; changing it later only queues a notice saying to use `initRandom()`. There
      is now one generator for all the shared modules, and `randomGeneratorSeed()` returns the seed in use. The CMake
      variable `GUM_RANDOMSEED` and the option `act --fixed_seed`, which had no effect on the draws, are removed. The
      test runs display their seed.
    - Names between single quotes in every fast syntax (`fastGraph`, `fastMarkedGraph`, `fastVariable`,
      `fastPrototype` of `BayesNet`, `InfluenceDiagram`, `MarkovRandomField`), shared lexical tools in
      `base/core/fastSyntax.h` (`quoteFastName`, `fastNameNeedsQuotes`). The three `fastPrototype` share one analysis
      of the description (`base/graphicalModels/fastModel.h`), used by the new `checkFastSyntax`/`FastSyntaxCheck`
      (`base/graphicalModels/fastSyntaxCheck.h`). `DiscreteVariable::toFast()` quotes the names which need it (see
      the warning in the pyAgrum section). New `toFast()` of `BayesNet`, `InfluenceDiagram` and `MarkovRandomField`.
      Behaviour change: a malformed description raises `InvalidArgument` in every fast syntax (see pyAgrum), and a
      `;` inside `[...]` or `{...}` no longer separates two chains.
    - Behaviour change: a seed gives the same draws on every platform. `randomValue`, `randomProba` and the shuffles
      of the database generators used the distributions of `<random>`, whose algorithms depend on the standard
      library (the same seed gave other values with libc++, libstdc++ or MSVC); the draws are now computed from the
      outputs of `std::mt19937` only, so the values drawn for a given seed changed once. New `randomShuffle` and
      `randomGamma` (used by `Dirichlet`); `randomValue` is unbiased up to `max` above 2^32; `randomProba` is in [0, 1).
      The PRM generators no longer use `std::rand` (they ignored `initRandom`).
    - New `invalidateTensors()` for the inferences of Bayesian networks, Markov random fields and influence
      diagrams (also in pyAgrum): it tells the inference that the values of tensors of its model changed in place
      (`cpt(...).fillWith(...)`), which it is not notified of. `LazyPropagation`, `ShaferShenoyInference` and
      `ShaferShenoyMRFInference` kept messages computed from the former values, `ImportanceSampling` its proposal.
    - New `MarkedGraph` (edges with a mark at each end: tail, arrowhead or circle; adding an existing edge updates its
      marks; directed view `parents`/`children`/`spouses`/`undirectedNeighbours`; `skeleton()`) and
      `fastMarkedGraph` (an edge described twice with different marks raises `InvalidEdge`); `PAG` derives from it
      instead of `UndiGraph` (a `PAG` is no longer an `UndiGraph`). New keys `dot.bidirectedEdgeColor` and
      `dot.circleMarkEdgeColor` for their DOT representation.
    - New immutable class `MAG` (maximal ancestral graph): `fromDAG` with latent and selected nodes (Richardson and
      Spirtes 2002), `fromPAG` (representative MAG, Zhang 2008), checked `fromMarkedGraph`, `mSeparation`,
      `neighbours` (undirected edges, as for the other graphs) and `adjacents` (any edge); new
      `graph::ancestors`, `graph::anteriors`, `graph::isAncestral`, `graph::isMaximal`, `graph::circleComponent`,
      `graph::representativeGraph` (Zhang 2008, Theorem 2) and `graph::mSeparation` for
      marked graphs. Also in pyAgrum.
    - New constructor `DAG(const DiGraph&)`: the nodes, names and arcs of a directed graph (only the arcs of a mixed
      graph), the acyclicity being checked once (`InvalidDirectedCycle`). Also in pyAgrum (`pyagrum.DAG(digraph)`).
    - `graph::cpdag` returns a `CPDAG` (same as `CPDAG::fromDAG`). New `graph::separation` (d-separation for `DAG`,
      `CPDAG`, `MPDAG`, c-separation for `PDAG`, `ChainGraph`, m-separation for `MAG`), `graph::ancestors` and
      `graph::descendants` of a set of nodes, `graph::anteriors` for mixed graphs, `graph::descendants` for marked
      graphs, overloads of `graph::consistentExtension` and `graph::degeneracyOrdering` for `CPDAG`/`MPDAG` and of
      `graph::ancestors`, `graph::descendants`, `graph::anteriors`, `graph::isAncestral`, `graph::isMaximal` for `MAG`.
      The ancestors (descendants, anteriors) of nodes are reached by at least one arc (edge): a node of the set belongs
      to the result only on a cycle or as an ancestor of another one (anteriors: going back and forth through the same
      edge is not a cycle). New concept `GUM_MarkedGraphable`: the marked graphs (`MarkedGraph`, `PAG`, `MAG`) no longer
      satisfy `GUM_UndiGraphable`, the undirected algorithms (`isChordal`...) are not defined on them.
    - FCI: `learnPDAG`/`learnDAG` go through the representative MAG of the learned PAG (Zhang 2008, Theorem 2)
      instead of applying the Meek rules to the PAG seen as a mixed graph; `PAG::toMixedGraph` is deprecated after
      3.3.2 (use `MAG::fromPAG`). If the learned PAG is not valid, the former approximation is used, or
      `InvalidArgument` is raised with the new key `learning.fciErrorOnInvalidPAG` (or `setFCIErrorOnInvalidPAG`).
    - Configuration reorganized (lowerCamelCase keys, sections by subject); the former names are deprecated after
      3.3.2 and still work: the look of the graphs moves from `notebook` to `dot` (`notebook.arcColor` →
      `dot.arcColor`...), the junction trees to `junctionTree` (`notebook.junctionTreeGraphSize` →
      `junctionTree.graphSize`...), `notebook.markovRandomFieldView` → `markovRandomField.view`. In the ini files,
      `### deprecated <type>` declares a deprecated key without replacement (still read, no longer used).
    - The graphs (but `CliqueGraph`) in the native GUM format (jgum/bgum): `graphToGUMString`, `saveGraphGUM`,
      `graphFromGUMString<G>`, `loadGraphGUM<G>`, `graphGUMType` (class, nodes with names, arcs, edges, marks).
      The format of a file is given by its extension unless `binary` is given (binary for an extension other than
      `.jgum`). On reading, the number of absent ids lower than the largest one is at most max(number of nodes, 65536)
      (a graph stores them: `IOError` otherwise); `NodeGraphPart::addNodeWithId` rejects the largest `NodeId`.
    - Bug fix: `topologicalOrder` (`DiGraph`, `MixedGraph` and their subclasses) raises `InvalidDirectedCycle` for a
      cycle separated from the nodes without parent (in release mode, it returned an incomplete order).
    - `PDAG` follows the literature: only directed cycles and double connections (arc and edge on a same pair) are
      forbidden. New `ChainGraph` (PDAG without partially directed cycle) with `moralGraph`,
      `moralizedAncestralGraph` and `cSeparation` (LWF); these methods of `PDAG` are deprecated after 3.3.2.
      So is `PDAG::hasMixedReallyOrientedPath` (with a partially directed cycle, it finds a walk, not always a
      path): use `ChainGraph::hasMixedReallyOrientedPath`, exact on a chain graph.
      New `graph::hasDirectedCycle` and `graph::hasPartiallyDirectedCycle`.
    - New graph algorithms: `graph::lexBFS`, `graph::maximumCardinalitySearch`, `graph::isPerfectEliminationOrdering`,
      `graph::isChordal`, `graph::degeneracyOrdering`, `graph::consistentExtension` (extension of a PDAG,
      Wienöbst et al. 2021) and `graph::cpdag` (CPDAG of a DAG, Chickering 1995).
    - New immutable graph classes `CPDAG` (Markov equivalence class of DAGs) and `MPDAG` (CPDAG with background
      knowledge), built from a DAG, by Meek closure or by checked conversion, with `dSeparation` (d-separation of the
      represented DAGs, computed on a consistent extension kept from the construction), `contains` and
      `consistentExtension`; also in pyAgrum. `CPDAG`, `MPDAG` and `MAG` share a read-only base (`ImmutableGraph`,
      `ImmutableMixedGraph`) with, beyond the accessors of their graph, `hasName`, `empty`, `connectedComponents`
      and `skeleton` (and, in pyAgrum, `connectedComponentsList`, `connectedComponentsCount`, `adjacencyMatrix`).
    - New `graph::meekClosure` (closure of an extendable PDAG under the Meek rules, Wienöbst et al. 2021, in O(dm)),
      used by `MeekRules` whenever its input is an extendable PDAG; the former propagation remains for the other inputs.
    - New `graph::orientAsDAG`: orientation of a PDAG into a DAG looking for small CPTs (a consistent extension if the
      PDAG is extendable), with a minimal total size of the CPTs for a CPDAG (Clique-Picking recursion of Wienöbst et
      al. 2021) for its undirected components of at most `graph.orientAsDAGExactMaxSize` nodes (new configuration key,
      100 by default), a greedy heuristic otherwise. `MeekRules::propagateToDAG` and the constraint-based learners
      (MIIC, PC, FCI) use it with the domain sizes of the variables: the orientation of the reversible arcs of a learned
      BN may change.
    - `EssentialGraph` stores the names of the nodes and no longer depends on the model it was built from (a copy of
      a learned essential graph kept the model's one instead of the learned PDAG); it is built by `graph::cpdag` in
      linear time, can be built from a named `PDAG`, and gives its `cpdag()`. `BNLearner::learnEssentialGraph` is
      written in C++ (in pyAgrum, the errors of the learning are no longer hidden). `EssentialGraph::idFromName`
      returns `std::optional` (`None` in pyAgrum) instead of raising `NotFound`, as for the other graphs.
    - `EssentialGraph` is now always a CPDAG, and a subclass of `CPDAG` (all its methods, drawn with the clusters of
      its chain components): built from a `PDAG` (or read in the GUM format), the PDAG is checked to be a CPDAG
      (`InvalidArgument`, `IOError`); it can also be built from a `CPDAG`. `BNLearner::learnEssentialGraph` gives the
      CPDAG of the learned DAG whatever the algorithm (with MIIC, PC or FCI, it gave the learned PDAG, which may not
      be a CPDAG: this one is still given by `learnPDAG`).
    - Same look for `PDAG`, `ChainGraph`, `CPDAG`, `MPDAG` and `EssentialGraph` in DOT: clusters for the chain
      components (of a `ChainGraph` and a `CPDAG`), compelled arcs colored; new keys `dot.chainComponentBgColor` and
      `dot.compelledArcColor` (formerly hard-coded); `CPDAG.toDot` and `MPDAG.toDot` accept `DotOptions`.
    - `CredalNet::intervalToCredalWithFiles` (deprecated since 2013, it wrote lrs files in the current directory and
      could crash) is removed: use `intervalToCredal`, which gives the same credal sets.
    - `BayesNet::changeTensor` is removed (it deleted the CPT, leaving dangling the references to it): use
      `cpt(node).fillWith(tensor)`, which changes the parameters and keeps the CPT.
    - The default threshold `epsilon` of `gibbsKL` and `mcKL` (`GibbsBNdistance`, `MCBNDistance`) is now 1e-5 (it was
      1e-10, never reached: the computations always ran up to `maxIter`, 10^7 draws, about 20 s even for 3 nodes). The
      stop now comes after about 2.5e5 draws (relative error about 0.2% for Monte Carlo, 0.6% for Gibbs, measured on a
      small network).
    - `dSeparation`/`cSeparation` of two nodes (`DAG`, `PDAG`, `ChainGraph`, `graph::dSeparated`,
      `graph::cSeparated`) raise `InvalidArgument` when X and Y are the same node, as the versions with sets of
      nodes do for sets which are not disjoint (pyAgrum already went through the sets).
    - `prm::ClusteredLayerGenerator::setClusterRatio` is effective (it was ignored: a cluster was always generated):
      the probability for a generated class to be replaced by a cluster, 1 by default (the former behaviour); a ratio
      outside [0, 1] raises `OutOfBounds`. The PRMs generated for a given seed change (one more draw by class).
    - The setters of `ApproximationScheme` call the new protected hook `onApproximationSchemeChanged_()`: the
      approximate inferences (through `invalidateTensors()`), `CNLoopyPropagation` and the approximate distances
      outdate their results, so that the next computation takes the new parameters into account.
    - `BNdistance::compute(bool lazy = true)` (it replaces the protected `process_()`): with `lazy == false`, the
      computation is always done.
    - `act guideline`: new check `examples` (in the default checks), an example for every documented member and
      function of pyAgrum (it was a test of pyAgrum).
    - **The atemporal names of `KTBN::fromBN`, `KTBNLearner` and `KTBNAdaptiveLearner` are a
      `std::set<std::string>` (they were a `std::unordered_set<std::string>`)**: the names are checked and reported in
      the same order on every platform. C++ code naming `std::unordered_set` must name `std::set` (no change in
      pyAgrum, which still takes a Python `set`).
    - Behaviour change: the random CPTs (`SimpleCPTGenerator`, hence `BayesNet::generateCPTs`, `fastBN`, the
      generators of Bayesian networks) draw each conditional distribution uniformly on the probability simplex
      (Dirichlet of parameter 1, as Ide and Cozman), instead of normalizing uniform draws in [0, 1]. The CPTs generated
      for a given seed change. `Tensor::randomCPT(varId = 0)` takes the position of the conditioned variable.
    - The generators of Bayesian networks give the bounds of the number of arcs (n - 1 to n(n - 1)/2) when they refuse
      it.

- Bug fixes (aGrUM and pyAgrum), thanks to @ldesreumaux for issues #206 to #210

    - `MeekRules::propagateToDAG` (used by PC to learn a DAG) could create an unshielded collider absent from the
      CPDAG, hence a DAG outside the learned Markov equivalence class; it is now a consistent extension (#207).
    - `MeekRules::choices()`, for an input which is not an extendable PDAG: a choice could be the orientation erased by
      the resolution of the double-headed arcs (hence not an arc of the result), and a pair of nodes could appear
      several times, in both orientations. Each choice is now an arc of the result of `propagateToCPDAG` or
      `propagateToDAG`, at most once per pair of nodes, in order of selection. In C++, `choices()` returns a
      `std::vector<Arc>` by value (it was a const reference).
    - `graph::dSeparated` (hence `dSeparation` of `DAG`, `CPDAG`, `MPDAG` and of the models) and
      `graph::mSeparation`: a node of Y which belongs to Z is separated from any node, as a node of X in Z already
      was (it was reported as not separated, so that exchanging X and Y could change the answer).
    - `graph::dSeparated`, `graph::cSeparated` and `graph::mSeparation` (hence `dSeparation`, `cSeparation`,
      `mSeparation` of the graphs and of the models, and `pyagrum.graph.separation`) raise `InvalidNode` for a node of
      X, Y or Z which is not a node of the graph (such a node was reported as separated from any node).
    - The constraint-based algorithms (PC, FCI, MIIC) ignored the maximum indegree outside the orientation of the
      v-structures: `learnDAG`/`learnBN` now raise `OperationNotAllowed` when the learned DAG exceeds it (their
      independence tests never remove an edge to satisfy it), or return the DAG (with a warning in pyAgrum) with the
      new key `learning.errorOnExceededMaxIndegree` set to `False` (or `setErrorOnExceededMaxIndegree`);
      `maxIndegreeExceeded()` tells whether the last learned DAG exceeds it (#208).
    - `BNLearner::setPCAlpha`, `BNLearner::setFCIAlpha` and `CIBasedLearning::setAlpha` raise `OutOfBounds` for a
      significance threshold outside ]0,1[ (negative, 0, 1, greater than 1, infinite or NaN), which was silently
      accepted and changed the learned graph (#209).
    - A structural constraint compatible with an unshielded collider could hide it: PC, FCI and MIIC now consider an
      arc into the middle node (e.g. a mandatory arc) as an arm of the collider, and PC and FCI test the pairs
      forbidden in both directions during the skeleton phase (then remove them), so that they get a separating set
      (#206).
    - The chi2 and G2 independence tests (PC, FCI, `BNLearner::chi2` and `BNLearner::G2`) computed their degrees of
      freedom from the number of skipped cells: they are now computed from the nonempty rows and columns of each
      nonempty slice, the empty rows, columns and slices having no degree of freedom (#210).
    - MIIC raised `DuplicateElement` when a mandatory arc had its reverse forbidden (by the user, the slice order or
      a node without parent).
    - FCI could remove a mandatory arc during its Possible-D-Sep phase: `learnPAG`, `learnPDAG` and `learnDAG` returned
      a graph without it, and without any error.
    - FCI never tested the separating sets of size 1 of its Possible-D-Sep phase (only the neighbours had been tested
      before): an edge could be kept while a single node of Possible-D-Sep separates its ends.
    - FCI (rule R5) could orient as undirected the edges of a covered circle path (the triples at both ends of the path
      were not checked).
    - `core.randomSeed` is now refused outside [0,4294967295] (it was truncated without a message: 4294967296 gave 0,
      hence a seed taken from the clock). New type `uint32` for the keys of the configuration (`int[0,4294967295]`).
    - `fastGraph`, `fastMarkedGraph` and the `fastPrototype` of `BayesNet`, `InfluenceDiagram` and `MarkovRandomField`
      (`fastBN`, `fastID`, `fastMRF`...): a line break separates two chains, as `;` (it glued the names around it:
      `"A->B\nC->D"` created a node `BC`); a blank chain (`"; ;"`) is ignored (`fastBN`/`fastID` raised `InvalidArc`,
      `fastMRF` created a variable with an empty name).
    - `fastGraph`, `fastMarkedGraph`: a node is given by its id only if its token is written with digits only and fits in
      an `int`, else all the tokens are names (`"99999999999->1"` created the arc 0->1; `"+5"` and `"5"` were the same
      node). An unquoted node name contains no blank, `<` nor `>` outside its brackets (`"A<->B"` created a node `A<`,
      `"A o -B"` a node `A o`): such a token raises `InvalidArgument`. In id mode, an id leaving more
      than max(number of nodes, 65536) absent ids below it raises `InvalidArgument` (`"0->2000000000"` reserved two
      billion ids).
      `isInteger`/`isIntegerWithResult` refuse a sign alone and a value out of the range of an `int` (which was
      truncated).
    - The graphs of inferences in pyAgrum (`BNinference2dot`, `CNinference2dot`, `MRFinference2UGdot`,
      `MRFinference2FactorGraphdot`, `LIMIDinference2dot`, the inferences of CTBN and BNMixture): `nodeColor` keyed by
      the ids of the nodes raised `KeyError`; the images of the nodes were written beside their temporary directory
      and never removed (they now live in it and are removed with the graph); the colors of the factors of a factor
      graph were not quoted (invalid DOT) and its edges were directed.
    - `CNinference2dot`: the observed nodes (`evs`) have the color of the evidence, as in the other inference graphs.
    - Binary GUM files (`.bgum`): the announced size is little-endian on any host, as documented; a file whose size does
      not match it (data after the payload included) is refused; the limit of 256 MB is removed (a larger file could be
      saved but not read).
    - `EMHistory()` could have one value less than `EMnbrIterations()` when EM stopped on consecutive decreases of the
      log-likelihood or on the time limit: plotting one against the other could fail at random.
    - `CNLoopyPropagation` with modalities (`insertModals`, `insertModalsFile`): `makeInference` could raise `NotFound`
      (the option of storing the vertices of the credal sets was not initialized) and `dynamicExpMin`/`dynamicExpMax`
      raised `OperationNotAllowed` (the dynamic expectations were never computed).
    - Sampling inferences (`MonteCarloSampling`, `WeightedSampling`, `ImportanceSampling`, `GibbsSampling` and their
      loopy versions): with explicit targets, `makeInference` raised `NotFound`; after a change of the evidence, the
      sampling still used the former evidence (`ImportanceSampling` never stopped, hence `evidenceImpact` too).
    - `ctbn.ForwardSamplingInference.makeParallelInference` raised `PicklingError` (and the results of the child
      processes were lost): it now samples in parallel processes, returns the number of runs of each sampling and gives
      the same result as `averageInference`. The forward sampling of CTBN and `ctbn.randomCTBN` are reproducible
      after `initRandom` (their draws did not use the generator of pyAgrum); `randomCTBN` with one variable (the
      default) raised `ValueError`.
    - `CNMonteCarloSampling` computed wrong bounds: the CPTs of the sampled networks are changed in place and the
      inference of each thread kept the posteriors of its first sample (now `invalidateTensors()`); its result also
      depended on the number of threads (each sample now has its own seed) and the repetitive independence drew
      concurrently with the shared generator.
    - `CNMonteCarloSampling` ignored the stopping criteria of the user: each `makeInference` set again epsilon,
      disabled `maxIter` and the rate. These defaults are now set by the constructor and the user may change them;
      an epsilon below 1000 times the machine epsilon raises `OutOfBounds` (below, the result would depend on the
      number of threads).
    - `MCBNDistance` and `GibbsBNdistance` enabled again the rate criterion during the computation (it could not be
      disabled).
    - `MCBNDistance` and `GibbsBNdistance` underestimated the Jensen-Shannon divergence when the supports of the two BNs
      differ: a sample where Q is 0 added nothing (instead of 1), and the mass of Q where P is 0, never drawn, was
      missing (it is now estimated by 1 - E_P[Q/P] when P has a null parameter).
    - `GibbsSampling`, `LoopyGibbsSampling` and `GibbsBNdistance` never changed the value of a variable with a
      deterministic CPT (a function of its parents, e.g. `either` in asia): it kept the value of the first sample, and
      the posteriors were wrong whatever the number of samples. Such a variable now follows its parents.
    - `MCBNDistance` and `GibbsBNdistance` underestimated the Hellinger distance when the supports of the two BNs
      differ (the mass of Q where P is 0, never drawn, was missing): it is now computed as sqrt(2 - 2 rho) from the
      estimate of the Bhattacharyya coefficient rho.
    - `InformationTheory` erased the joint targets of the inference given as argument (only X, Y and Z remained): they
      are now restored.
    - `InformationTheory.variationOfInformationXY` returned H(X)+H(Y) instead of H(X|Y)+H(Y|X) = H(X)+H(Y)-2I(X;Y)
      (the test checked the same wrong formula).
    - FCI, Possible-D-Sep phase: a node adjacent to X or Y through a mandatory arc only was never tested as a
      singleton separating set (the pair may have been removed by the skeleton phase before the arc was added back).
    - The sampling inferences (`GibbsSampling`, `MonteCarloSampling`, ...) could stop on epsilon after a few
      identical samples (e.g. a Gibbs chain with near-deterministic CPTs and a small `burnIn`): their error, the
      length of a Wald confidence interval, was 0. It is now the length of the Wilson score interval, never 0.
    - `UndiGraphListener` and `MixedGraphListener` were not exported from the shared libraries (link error of the
      structure listeners of `MarkovRandomField` with the shared build on Windows).
    - `MixedGraphListener` takes a `const MixedGraph*`, as `DiGraphListener` and `UndiGraphListener` (it could not
      listen to the graph of an immutable graph class).
    - `MarkedGraph::addEdge` sets the marks before emitting `onEdgeAdded`: a listener can read the marks of the new
      edge (`markAt` raised `NotFound`).
    - `fastMRF("A---B")` raises `InvalidArgument` (it gave the variables `A` and `-B`): an unquoted name of a fast
      description of a model neither begins nor ends with `-`. `fastVariable("+[0,1,2]")` raises `InvalidArgument` (it
      gave a variable with an empty name).
    - A deprecated key of the configuration must be documented by `deprecated after <last tag with the key>`
      (`SyntaxError` otherwise): the deprecation message, built from this documentation, is always a sentence.
    - `bnmixture.BNMBootstrapLearner.updateState` raised `NotImplementedError`: it now behaves as for the other
      mixture learners (without argument, the learner copies the state set by the `use...` methods).
    - `lib.notebook.animApproximationScheme` failed outside a notebook (`AttributeError`): the curve is now drawn
      in the current figure, without live update.
    - Python listeners (`addStructureListener`, `PythonApproximationListener`, listeners of the database
      generators and of the loaders): an exception raised by a callback was left set, so that the next calls to
      Python failed with `SystemError`; it is now reported by `sys.unraisablehook`. A non-callable callback raises
      `InvalidArgument` (it was stored anyway), the former callback is released when it is replaced, the node ids
      and the steps are given as 64-bit integers on Windows too, and the results of the callbacks are released.
      The listeners of `loadBN`/`loadMRF`: a non-callable one raises `InvalidArgument` (all of them were silently
      ignored) and each listener of the list leaked one reference.
    - A list of non-strings given where a list of names is expected was read as empty names: `variables([0, 2])`
      raised `NotFound` with the key `<>`; it now raises `TypeError` (`variables` takes a set of ids).
    - `ctbn.CTBN.equals` no longer prints why the two CTBNs differ.
    - `BNDatabaseGenerator.drawSamples` with evidence returned a wrong log2-likelihood: the rejected samples were
      partly counted (and, at the timeout, the last kept sample was counted but dropped from the database).
    - Evidence with a null probability given to `MonteCarloSampling`, `WeightedSampling`, `ImportanceSampling` (and
      their loopy versions) or to `BNDatabaseGenerator.drawSamples`: the rejection of the draws looped forever (with
      `timeout=0` for the generator; `setMaxTime` was not applied during the rejections). They now raise
      `IncompatibleEvidence`, before any draw when the removal of impossible values (as for `GibbsSampling`) proves the
      incompatibility, else when the number of consecutive rejected draws reaches the new key
      `sampling.maxConsecutiveRejections` (1000000) or when the time limit is reached before any compatible draw (the
      generator returned an empty database and a log2-likelihood of 0).
    - `BNDatabaseGenerator`, `ExactBNdistance`, `GibbsBNdistance` and `MCBNDistance` did not keep their Bayesian
      networks alive in Python: built on a temporary BN (`BNDatabaseGenerator(fastBN(...))`), `drawSamples` or `compute`
      crashed. They now keep a reference to them, as the inferences do.
    - The `listeners` argument of `loadBN` and `loadMRF`: a sequence whose length cannot be read raises
      `InvalidArgument` (a Python error was left pending and the vector of listeners was resized to a huge size).
    - `loadGraph` and `saveGraph` read the extension of the file name only (a dot in a directory name gave an
      extension such as `v2/graph` in the error message).
    - The conversions of C++ results into Python objects check every step: a string which is not valid UTF-8 (e.g. a
      label read from a latin-1 CSV file) raises `UnicodeDecodeError` (instead of a `SystemError`, or a crash), and no
      partially built object is returned.
    - `Tensor.expectedValue` raises the exception of the function (the function is no longer called after it).
    - Invalid integers given to `BNLearner.setSliceOrder`, `BNLearner.useK2`, the KTBN (base, slice) keys and modality
      indices, and a malformed `latents` argument of `CausalModel` raise an exception instead of leaving a Python
      error pending.
    - `pyagrum.lib.image.export` and `pyagrum.lib.image.exportInference` without file name: the temporary file is
      closed (`ResourceWarning`) and always removed (it was left on Windows).
    - `PRMexplorer.load` accepts a file with warnings only (it raised `FatalError`), and `PRMexplorer.getSuperType`
      returns the name of the super type (it returned the name of an internal variable).
    - `CredalNet.setCPT` can be called from Python (its vertices, a list of lists of probabilities, could not be
      converted).
    - `skbn.BNClassifier`: an unfitted classifier can be pickled, copied and cloned (it failed with scikit-learn 1.8).
    - `skbn.BNClassifier` with `learningMethod="TAN"` and a single feature: `fit` failed (the network was lost).
    - `clg.CLG.asDiscreteBN` and the `NoDiscretization` parameters of `DiscreteTypeProcessor` quote the names of the
      variables (a name such as `X-ray` was misread).
    - `help(pyagrum.config.typed)` (and `asInt`, `asFloat`, `asBool`) shows the documentation of the accessor.
    - `MarkovRandomField.toFast(filename)` raised `NameError`.
    - `variable`, `variableFromName`, `cpt` (`BayesNet`, `BayesNetFragment`, `InfluenceDiagram`, `MarkovRandomField`),
      `utility` (`InfluenceDiagram`) and `factor` (`MarkovRandomField`) keep their model alive: on a temporary model
      (`fastBN("A->B").variable("A").domainSize()`), the result referenced a destroyed object (crash).
    - `GibbsBNdistance` and `MCBNDistance`: the Bhattacharya distance started from an uninitialized value, and the copy
      of a distance lost its Hellinger, Bhattacharya and Jensen-Shannon values.
    - `CNMonteCarloSampling` created while aGrUM used 1 or 2 threads kept most of its marginals at their initial bounds
      (min 1, max 0): the merge of the samples stopped after the first nodes.
    - `addArc` of `DAG`, `PDAG`, `ChainGraph` and `addEdge` of `PDAG`, `ChainGraph`: a mono-cycle on a missing node raises
      `InvalidNode`, as for the other graphs (it raised `InvalidDirectedCycle`).
    - The binary GUM format of the graphs (`.bgum`) accepts a node id stored as a signed integer (msgpack `int8` to
      `int64`, written by other encoders than aGrUM's): a non-negative id was refused.
    - The GUM readers (`.jgum`, `.bgum` and the GUM strings, of the graphs, `BayesNet`, `InfluenceDiagram` and
      `MarkovRandomField`) refuse an object with the same key twice: the last value was kept silently. The node ids of
      `"names"` in a GUM graph have no leading zero (`"0"` and `"00"` named the same node).
    - `CNMonteCarloSampling` stopped when a period of samples changed nothing at all in the bounds: a rounding
      difference of one ulp between the inference engines of the threads gave other periods, hence other bounds
      (up to 0.014), with 1 or 4 threads for the same seed. A variation below `1000 * epsilon` of the scalar type is
      now taken as no change: the result no longer depends on the number of threads.
    - The messages of `Configuration::load` (`pyagrum.config.load`) give the key as written in the file (it was
      lowercased, so that a message could show two spellings of the same key); the notice "`core.randomSeed` ... has no
      effect" is no longer given when the seed comes back to the one in use (`reset()`, `pop()`, the end of a
      `ConfigurationGuard` or of a `with pyagrum.config:` block).
    - `WeightedSampling` and `ImportanceSampling` (and the loopy versions): the width of the Wilson intervals of the
      stopping criterion counts the effective size (Kish) of the weighted sample, (sum w)^2 / sum w^2, instead of the
      number of samples: a sample dominated by a few large weights no longer looks converged too early (the
      unweighted samplers are unchanged).
    - `CNLoopyPropagation`: `epsilon` (and the stopping criterion) is the largest change of the marginals of an
      iteration (it was the largest value of a per-thread maximum initialized to the largest double, so that the
      iterations never stopped on `epsilon`); with evidence, the interval Λ of a node could collapse to its upper or
      lower bound (exact bounds are now found on binary polytrees); `CredalNet::approximatedBinarization` decoded the
      configurations of the parents in the wrong order (the binarized network did not have the original CPTs).
    - `kNML` (MIIC's correction) with a prior: the pseudo-counts of the prior were added with the ids of the joint
      counts instead of those of the marginal counts (wrong weights, out-of-bounds access).
    - `BNLearner`: `learnParameters` (and the initialization of EM) after a structure learning used the score of that
      learning, not the current one (`useScoreBDeu`... called in between was ignored); MIIC converted its mutual
      informations (in bits) with `exp` as if they were in nats, which biased the orientation of the triples.
    - `DiscreteTypeProcessor`: MDLP no longer forces a cut point when none is informative (a `ValueError` says that the
      variable cannot be discretized by MDLP); the elbow method crashed (1D input to `KBinsDiscretizer`), assigned a
      point to the wrong center after an empty interval and divided by 0 on a flat curve.
    - `CausalEffectEstimation`: the X-learner combined its two effects with swapped weights (Künzel et al.:
      e(x) tau0(x) + (1 - e(x)) tau1(x)); TSLS returned the mean of all the coefficients of the second stage instead of
      the coefficient of the treatment, and misaligned the rows of a conditional estimate; the individual effects of
      the propensity stratification were not in the order of the rows (the average effect was right).
    - `CausalShapValues` and `CausalShallValues`: each configuration of the background was weighted by its frequency
      times its probability in the mutilated network (counted twice; the baseline of `CausalShapValues` was not P(Y)),
      and a coalition whose values were not in the background got the value 0. The weight is now the probability only,
      and the coalition is set in every row of the background.
    - `causalImpact` with `knowing`: the third rule of the do-calculus removed the arcs into all of X instead of
      X(Z), the variables of X which are not ancestors of Z, and could answer P(Y | Z) for an effect that exists.
    - `CausalModel.observationalBN`, `CausalModel.variable`, `CausalImpact.cm`, `Counterfactual.originalModel`,
      `Counterfactual.twinModel` and `Counterfactual.value` keep their object alive, and `CausalImpact`,
      `Counterfactual` and `causalImpact` keep their causal model alive: on a temporary model
      (`CausalModel(bn).observationalBN().names()`), the result referenced a destroyed object (wrong names or crash).

- Optimizations (aGrUM and pyAgrum)

    - `MPDAG.contains` compares the v-structures instead of building two CPDAGs: 50 to 75 times faster.
    - The copy of a `MarkovRandomField` updates its graph once instead of once per factor (quadratic time, 27 times
      faster for 1000 factors); its listeners no longer see the edges erased then added again.
    - `CNMonteCarloSampling` inserts the evidence once per thread instead of once per sample: 20 to 30 % more samples
      per second with evidence, same results.
    - The closures of the graph algorithms (`ancestors`, `descendants`, `anteriors`, hence `isAncestral` and `isMaximal`
      of the marked graphs) visit the neighbours in place instead of building a set for each node, with a stack as
      frontier: 1.1 to 1.8 times faster, same results.
    - `GibbsSampling` with hard evidence prunes the domains by the evidence once per inference instead of twice (the
      check of the evidence is reused for the first sample of the chain): 30 % faster for a short inference (40 nodes,
      CPTs with many zeros, 20 iterations), unchanged without evidence, same results.

## Changelog for 3.3.2

- pyAgrum

    - `parents`, `children`, `family`, `descendants`, `ancestors` (BN, ID) and `neighbours` (MRF) are the C++ methods:
      `parents`/`children` also accept a set of ids or a list of names, and a wrong argument (e.g. a float or a
      negative id) raises `TypeError` instead of `pyagrum.InvalidArgument` (or of being silently accepted).
    - `PAG.addNodes` returns a set of ids, as for the other graphs (it returned a list).
    - `generateSample` (overloaded on `name_out`), `mutilateBN`, `MEU` and `meanVar` are annotated.
    - Bug fixes: memory corruption in `BNDatabaseGenerator.varOrder()` for node ids above 256, reference counting in
      `PRMexplorer`, memory leaks in `Tensor.argmin/argmax`, `MEU`/`meanVar` and when parsing Python sequences.
      `BNLearner.useK2`/`setSliceOrder` with a wrong argument raise `TypeError` (instead of `SystemError`).

- aGrUM

    - Leaf modules (CM, CN, ID, PRM): explicit template instantiations tagged `GUM_PUBLIC_<MODULE>` for shared builds.
    - `DAGmodel` (BN, ID, CausalModel): `parents`, `children`, `family`, `ancestors` and `descendants` of a missing
      node id raise `NotFound` (they returned an empty set, or `{id}` for `family`).
    - Removed `gum::learning::SimpleMiic` (legacy, unused since `Miic` replaced it).

## Changelog for 3.3.1

- pyAgrum

    - **Static typing**: pyAgrum is marked as typed (`py.typed`), the entry points written in Python are annotated and
      the remaining C++ types in the annotations are translated (sets of node ids are now `set[int]`).
    - `CredalNet` CPT accessors and `ShaferShenoyLIMIDInference.reversePartialOrder()` return Python lists instead of
      unusable SWIG objects.
    - Fixed `NameError` in `loadMRF`/`saveMRF`/`loadID`/`saveID` for an unknown extension, and missing imports in two
      tutorials.

- aGrUM

    - `act guideline`: clearer output and new checks `lint` (ruff), `pyannot` and `pytyping` (mypy).
    - `ruff.toml` selects its rules explicitly; the whole repository is clean for them.

## Changelog for 3.3.0

- pyAgrum

    - **Configuration in C++**: `pyagrum.config` is now the C++ `gum::Configuration` (`PyAgrumConfiguration` and
      `pyagrum/config.py` removed), with the same API and the same `defaults.ini` / `pyagrum.ini` files.
    - **Renamed configuration keys**: the `default_` prefix is dropped and the keys of the look of the graphs
      (`notebook`, `factorgraph`, `dynamicBN`, `influenceDiagram`, `credalnet`, `causal`) are now lowerCamelCase keys
      defined in aGrUM. The old names are deprecated (still accepted, with a `DeprecationWarning`).
    - **Graphs drawn by aGrUM**: the raw `toDot()` of graphs and models now gives the look displayed in notebooks, and
      accepts a new `pyagrum.DotOptions` (size, layout, labels, colors of nodes and arcs). Font colors can be `adaptive`
      (black or white according to the background, the new default). Graphs given by the user are shown as they are.
      Code parsing the text of a `toDot()` may have to be adapted.
    - `pyagrum.lib._colors` renamed `pyagrum.lib.dotstyle` (deprecated relay kept); `hex2rgb` deprecated (use
      `color2rgb`); `CausalModel.toDot()` arguments `SHOW_LATENT_NAMES`, `NODE_BG`, `NODE_FG`, `EDGE_COL` deprecated.
    - `skbn.BNClassifier`: `priorWeight` now defaults to the configuration.
    - Fixed `proba2hex` (invalid colors) and `CLGInference2dot` (without evidence). pyAgrum requires `pydot>=2`.
    - **Exceptions**: all the aGrUM exceptions are now exported by `pyagrum` (`FactoryError`, `NotImplementedYet`,
      `IncompatibleEvidence`, `LearningError`, ...). `TypeError` and `IOError` are renamed `GumTypeError` and
      `GumIOError` so that `from pyagrum import *` no longer shadows the Python builtins. `pyagrum.IOError` is deprecated
      (use `GumIOError`), as `pyagrum.SyntaxError`: syntax errors are raised as the builtin `SyntaxError` (the aGrUM
      class is no longer wrapped). A few checks of Python arguments (e.g. `setEvidence` or `setTargets` with a wrong
      type) and `del pyagrum.config[...]` now raise the builtin `TypeError` or `SyntaxError` instead of the aGrUM ones.

- aGrUM

    - **New: `gum::Configuration`**: thread-safe, typed and documented configuration singleton (defaults embedded in
      the library from several sources, `agrum.ini` read at start-up, domains of validity, hooks, `push`/`pop`,
      `save`/`load`).
    - **Defaults from the configuration**: approximation schemes, sampling/Gibbs/LBP inferences, KL distances, learners
      (new key `learning.BNLearnerPreferredAlgo`), jgum writers and the text display of tensors read their defaults from
      the configuration. The corresponding macros (`DEFAULT_EPSILON`, `GIBBSKL_DEFAULT_*`, `GUM_CHI_EPSILON`, ...) are
      removed.
    - **Numerical tolerances**: `core.epsilonCoarse`, `epsilonStandard` and `epsilonFine` replace hard-coded literals
      (some tolerances change slightly, e.g. in `IBayesNet::check()` and `AlmostDifferent`).
    - **New `gum::dot`** (`DotStyle`, `DotWriter`, `DotOptions`, colors): all the `toDot()` share the same writer, read
      the configuration and accept `DotOptions`; new `CredalNet::toDot()`; several malformed dot outputs fixed.
    - `KTBNLearner` and `KTBNAdaptiveLearner` gain `usePC()`, `useFCI()` and `useGreedyThickThinning()`.
    - Fixed the copy/move of `IBNLearner` and `BNLearner::copyState()`, which lost some options.
    - `GibbsBNdistance` now draws the variables at random by default (`gibbsKL.drawnAtRandom`).
    - **Much faster reading of models with clang and MSVC** (BIF, DSL, NET, UAI, O3PRM...): e.g. a large BIF file from
      8 s to 0.1 s with clang.
    - **Aggregators `min`, `max`, `median`, `amplitude`, `sum` on values**: they worked on the indices of the
      parents, which was wrong for ranges not starting at 0 and for parents with different domains. They now work on
      the values of the parents and give the closest value of the aggregator (saturated at both bounds). `sum` and
      `amplitude` need numerical variables (use `count` to count the true parents), `min`, `max` and `median` accept
      labelized variables (order of the labels) or numerical ones, but not both: an incompatible variable is rejected
      with `TypeError` (`GumTypeError` in pyAgrum). The decomposition of aggregators now gives the right domains to the
      intermediate nodes (#198, #201). The aggregators derive from `MultiDimFoldAggregator` (`count`, `exists`,
      `forall`, `and`, `or`: `neutralElt_()` and `fold_()` on the indices of the parents) or from
      `MultiDimValueAggregator` (`min`, `max`, `median`, `amplitude`, `sum`: `buildValue_()` on their values).
    - O3PRM: the error of a system that cannot be instantiated gives the reason (e.g. an unassigned reference slot or a
      parent refused by an aggregator) instead of an unknown error.

- Bug fixes (aGrUM and pyAgrum), thanks to @jschueller for issues #181 to #205

    - `ImportanceSampling`: the weight of a sample kept only the ratio of the last node (#181).
    - Hybrid samplers (`LoopySamplingInference`, `Loopy*Sampling` in pyAgrum): they stopped too early, while the LBP
      prior still outweighed the samples, and the prior was weighted in the wrong units with weighted samplers (#181).
    - `LoopyBeliefPropagation`: the error of a sweep was never reset, so LBP stopped after two sweeps (#182).
    - `ShaferShenoyLIMIDInference`: the optimal policy of a decision now keeps its decision parents (#183), and the
      variance of `MEU()` includes the covariances between utilities (#188, thanks to Mathias Alerini, Equinor).
    - `StructuredPlaner`: action-dependent rewards are taken into account (#184); `FMDPSimulator` and the planners no
      longer crash on a variable without transition for an action, which now keeps its value (#187).
    - `ShaferShenoyMRFInference::jointPosterior(wanted, declared)` returned the joint of the declared target, not of
      the wanted one (#185).
    - `MultiDimFunctionGraphManager`: `setSon` and `eraseNode` left inconsistent parent links (#186).
    - `MultiDimNoisyORCompound`: a causal weight lower than the leak (invalid model, giving probabilities outside
      [0,1]) is now rejected with `OutOfBounds` (#190).
    - `evidenceProbability()` (`LazyPropagation`, `ShaferShenoyInference`, `ShaferShenoyMRFInference`) ignored the
      connected components without target; the MRF one returned Z(e) instead of P(e) (#191, #193).
    - `evidenceImpact`, `evidenceJointImpact` and `jointMutualInformation` (BN and MRF) no longer replace the targets
      and the evidence of the user (#192).
    - `GibbsSampling`: the chain could freeze on a starting state of null probability; it now starts from a state of
      positive probability (or raises `IncompatibleEvidence`/`OperationNotAllowed`) (#189); exactly `burnIn()`
      transitions are done and `burnIn(0)` is allowed (#194).
    - `MeekRules`: rule R4 was not Meek's R4, so some compelled edges stayed undirected (#195, thanks to
      @ldesreumaux).
    - `MultiDimNoisyORCompound`, `MultiDimNoisyORNet`, `MultiDimNoisyAND`: weights outside [0,1] or NaN (and a null
      leak for `NoisyAND`, also through its setter) are rejected (#205). `BayesNet::addArc` and `addWeightedArc` no
      longer leave an arc in the graph when the CPT refuses it.
    - PRM: the CPT of a cast descendant (`setAsCastDescendant`) ignored the label map (#199).
    - `qBNSampling`: `qBNRejection` reused stale operators after `setEvidence`/`useFragmentBN` (#197); the quantum
      registers are created in the order of the qubit map (#200); the binarized parent states follow the CPT order
      (#204); a variable with a single state is rejected with `OperationNotAllowed` instead of crashing (#203).

## Changelog for 3.2.1

- pyAgrum

    - Fixed broken notebook thumbnails (notebooks 22, 67, 80) and added an I/O summary table image to notebook 91.
    - Expected scikit-learn warnings are now silenced in the pyAgrum tests.

- aGrUM

    - Fixed missing exported symbols (`GUM_PUBLIC` tagging) in several `BASE`, `BN` and `PRM` classes.
    - `GUM_NO_EXTERN_TEMPLATE_CLASS` is now a public configuration macro.
    - Removed nested duplicate visibility macros.

## Changelog for 3.2.0

- pyAgrum

    - **New: KTBN (k-order dynamic Bayesian networks)** (thanks to Seth Aguila and Anis Khacef): learn, generate
      and run inference on dynamic Bayesian networks with more than one time slice of memory. Inference (filtering
      and smoothing) works over any time window with interventions AND observations, and the memory order K itself
      can be learned from data instead of being fixed by hand.
      Replaces `pyagrum.lib.dynamicBN` (now deprecated), with new notebooks.
    - **Lighter, faster import**: pyAgrum's Python modules (`markov_random_field`, `influence_diagram`,
      `credal_net`, `causal_model`, `prm`, `ktbn`) are now loaded only when needed instead of all at once.
    - Cleaner error messages (internal technical details no longer shown).
    - Minor notebook fixes.
    - **CausalModel from a DAG**: `CausalModel` can now be built from a plain `DAG`, without a `BayesNet`
      (`hasObservationalBN()`); structural queries work either way, CPT-dependent ones raise
      `OperationNotAllowed`. `connectedComponents()` return type aligned with other graphical models.
  
- aGrUM

    - **New: KTBN (k-order dynamic Bayesian networks)** (thanks to Seth Aguila and Anis Khacef): C++ engine behind
      the pyAgrum feature above -- construction, inference over an arbitrary time window (filtering and smoothing),
      automatic learning of the memory order K, and random generation.
    - Windows build reliability improvements (several linking/build fixes).
    - New CI job testing pyAgrum wheels against a conda-forge-like environment on Windows.
    - **CausalModel decoupled from BayesNet**: new DAG-only constructor; `observationalBN()` now optional
      (`hasObservationalBN()` guard); fixes `inducedCausalSubModel` silently building a BN with
      uninitialized CPTs. `connectedComponents()` now returns `NodeProperty<NodeId>`, like `DAGmodel`/`DiGraph`.
    - **BUILD_SHARED_LIBS=ON support**: per-module shared libraries for pyAgrum on conda-forge, with
      `GUM_PUBLIC_<MODULE>` visibility tagging across all modules and numerous accompanying Windows
      link/build fixes.
    - Documentation improvements and typo fixes.

## Changelog for 3.1.0

- pyAgrum

    - **fastGraph**:
        - Added `fastDiGraph()`, `fastUndiGraph()`, `fastMixedGraph()`, `fastDAG()`, and `fastPDAG()`. `fastGraph(desc)` is preserved as a single entry point that guesses the returned graph type from `desc`.
        - The dot-like syntax now accepts non-numeric tokens as node names (not only unsigned ints); malformed or
          type-incompatible descriptions raise `pyagrum.InvalidArc` / `pyagrum.InvalidEdge`
          instead of a generic error.
    - **Configuration**:
        - `defaults.ini` now carries typed metadata (`### type :: doc` comments) with validation; added `config.typed`
          accessor and key-deprecation support.
        - Boolean values are canonicalized to `True`/`False` at set time.
        - Deprecated `asBool`/`asInt`/`asFloat` accessors.
        - The configuration reference documentation is now generated from `defaults.ini`.
    - **Native GUM Format (jgum / bgum)**:
        - `saveGUM()` for `BayesNet`, `InfluenceDiagram`, and `MarkovRandomField` gains an `indent` parameter,
          defaulting to `core.default_jgumIndent`.
        - Aggregator and ICI model nodes are now serialized compactly instead of as dense CPTs.
    - **MCBNDistance**:
        - Wrapped the new `MCBNDistance` C++ class.
        - Fixed `animApproximationScheme` flicker by consolidating it into `utils.py`.
    - **Documentation**: several improvements.
    - **Misc**:
        - Refreshed `pydot>=2` requirement.

- aGrUM

    - **MCBNDistance**:
        - Added `MCBNDistance`, an independent Monte Carlo estimator of the KL divergence between two Bayesian networks. For a large BN, MCBNDistance is often faster and more accurate than the `GibbsBNDistance` estimator.
    - **Native GUM Format (jgum / bgum)**:
        - Aggregator and ICI model nodes are now serialized compactly instead of as dense CPTs.
        - Hardened aggregator/ICI serialization: `gum::Exception` on malformed input, O(N) writer,
          `std::optional` aggregator `value()`, private `addAggregator`/`addICIModel`, deduplicated aggregator dispatch.
    - **fastGraph**:
        - Replaced the ad hoc Python `fastGraph` parser with a generic C++ template,
          `gum::fastGraph<GRAPH_TYPE>(std::string_view)`, building a `DiGraph`, `UndiGraph`,
          `MixedGraph`, `DAG`, or `PDAG` from a dot-like textual description; node tokens can now be names (not only
          unsigned-int ids) and are preserved when the built graph feeds `moralGraph()`. Malformed or type-incompatible
          descriptions throw `InvalidArc`/`InvalidEdge` instead of being silently misread.
    - **StructuralMetrics**:
        - Guarded `precision`/`recall`/`f_score` against 0/0 division.
    - **PRM fixes**:
        - Fixed PRM test suites broken since the `internalGraph()` rename and the CxxTest-to-doctest migration.
        - Fixed `PRMInstance::_addReferingInstance_` storing the target attribute's safe name instead of the slot
          chain's own name, which broke SVED/StructuredBayesBall name resolution.
        - Promoted PRM to an independent CMake module (`PRM_DEPS=BN`) instead of folding it into
          `BN_DIRS`.
    - **Testing infrastructure**:
        - optimization, bug fixes to the new doctest infrastructure.
    - **Build & Tooling**:
        - `act guideline` gains a `--source` glob filter to restrict checks to matching files.
        - Added `gum_cppnb`, a standalone C++ notebook tool auto-installed by `act install aGrUM`, with BN/MRF/ID/CM
          tutorials.
        - Finalized the BN+BNLEARNING merge: dropped the redundant BN/learning glob entry
        - Required `swig>=4.3` (thanks to Julien Schueller).
        - Simplified CMake threading logic: `GUM_THREADS` cache variable (`stl`/`omp`) replaces the previous
          flag-assembly logic (thanks to Julien Schueller).

## Changelog for 3.0.0

This major release brings three headline changes: the causal module is promoted from pure Python to a first-class C++
module (`CM`), structure learning is extended with PC, FCI (with PAG output), GreedyThickThinning, and triangle
deletions in GreedyHillClimbing, and a new native GUM serialization format (`jgum` / `bgum`) is introduced for all
graphical models. It also consolidates widespread API modernisation (C++20 `std::format`, `string_view`, `optional_ref`,
`std::optional`, `concepts`) and several breaking renames detailed below.

- pyAgrum

    - **Breaking API**:
        - Graph path-finding functions `undirectedPath`, `directedPath`, `directedUnorientedPath`,
          `mixedOrientedPath`, `mixedUnorientedPath` now return `None` instead of raising `NotFound` (or returning `[]`)
          when no path exists.
        - Causal module renames: `CausalModel.observedBN()` → `observationalBN()`;
          `CausalModel.addLatentVariable(..., keepArcs=...)` parameter `keepArcs` renamed to
          `assumeNonSpurious`; `CausalModel.backDoor()`/`frontDoor()` return `None` instead of raising when no valid set
          exists (`set()` when the empty set is valid); `DoorCriteria`'s `EnumerationOptions`
          removed (options `excluded_nodes`, `max_cardinality`, `only_minimal`, `stopAtFirst` are now direct keyword
          parameters, all methods now static); `CausalImpact` direct access to `result` removed (use
          `impact()`); `Counterfactual.getResult()` renamed `impact()`; `DSeparation` renamed `Separation`.
        - `NodeId` and `NameOrId` type aliases removed from `pyagrum.ctbn` and `pyagrum.clg` (use `int` and
          `int | str`).
        - `_gum_add_properties_while_getstate_` removed; pickle metadata now handled by `updateMetaData()`
          (pickled objects from earlier versions may not unpickle correctly).
        - `BNClassifier` now requires a `DiscreteTypeProcessor` constructor parameter; `createBNClassifier`
          signature changed accordingly; binary prediction dispatch and `predict_proba` updated; `model()`
          method added (returns a copy of the underlying BN).
    - **Causal Module**:
        - Added SWIG Python bindings for the C++ causal module (`CausalModel`, `CausalFormula`,
          `DoorCriteria`, `Counterfactual`, ...).
        - Refactored `causalEffectEstimation` as a top-level subpackage.
        - Fixed type references in docstrings (`pyagrum.causal.CausalModel` → `pyagrum.CausalModel`) and restructured
          the causality section in the Sphinx documentation.
        - Updated causality notebooks; added `causal2graph` utility for causal graph visualization; added AST printing
          for `CausalImpact` results.
    - **Structure Learning (FCI / PAG)**:
        - Added SWIG bindings for `PAG` (Partial Ancestral Graph) and the FCI (Fast Causal Inference)
          algorithm.
        - Exposed `BNLearner.setAlgorithmFCI()`, `fciExhaustiveSepSet()` / `setFCIExhaustiveSepSet()`.
        - Added documentation and test suite for PAG/FCI.
    - **Native GUM Format (jgum / bgum)**:
        - Added `saveGUM()`, `loadGUM()`, `saveGUMstring()`, `loadGUMstring()` to `BayesNet`,
          `InfluenceDiagram`, and `MarkovRandomField`.
        - Added SWIG type mapping for `optional<vector<NodeId>>`.
        - Added `GumFormatTestSuite` covering jgum/bgum round-trips and string serialization.
        - Added Sphinx page `jgum-bgum-format.rst` and notebook *91-Tools-LoadAndSaveGraphicalModels*
          illustrating all I/O formats.
    - **qBNSampling** (experimental, thanks to Tibor Dubois, Thierry Rioual, Mehmet Gunes):
        - New `pyagrum.qBNSampling` module: quantum circuit encoding of Bayesian Networks and rejection-sampling
          inference.
    - **CLG (Conditional Linear Gaussian)**:
        - Added `CLG.asDiscreteBN()` to convert a CLG model to a discretized BN.
        - Added `CLG.__eq__`, `__getstate__`, `__setstate__` (pickle support).
        - New `CLGModelTestSuite` and extended `CLGInference`/`SEM`/`Sampling`/`Random` test suites.
        - `randomCLG` gains `max_parents` and `ratio_arc` parameters; `GaussianVariable` sigma guard added (thanks to
          Ima Bernada).
    - **BNClassifier & skbn** (thanks to Lou Toubiana):
        - Added `model()` method returning a copy of the underlying BN; full coverage
          `BNClassifierTestSuite`.
        - Refactored `skbn` for full sklearn API compliance: `ClassifierMixin` placed before
          `BaseEstimator` in MRO, fitted attributes renamed to the trailing-underscore convention (`bn_`,
          `threshold_`, `target_`, ...), `validate_data()` used for input validation in `fit`, `predict`, and
          `predict_proba`.
        - `fit(X, y)` signature is now positional and strictly sklearn-compliant; the old
          `fit(X=None, y=None, data=None, targetName=None)` form is removed.
        - Fixed pandas 2.x compatibility (`X` cast to `object` dtype before processing in `fit`) and suppressed a
          spurious sklearn `'X does not have valid feature names'` warning in `predict` /
          `predict_proba`.
    - **StructuralMetrics**:
        - Exposed `StructuralMetrics` (formerly `StructuralComparator`) with SHD, tp/fp/fn/tn accessors and SID
          (Structural Intervention Distance) for BN comparison.
        - Updated `GraphicalBNComparator` to delegate to `StructuralMetrics`; added `structuralFScore`.
    - **Tensor & Numpy**:
        - `Tensor` methods `random()`, `randomDistribution()`, `randomCPT()`, `noising()` now return `self`
          for chaining.
        - Added Python bindings for `mean()`, `variance()`, `stdDev()`, `isNumerical()`.
        - Numpy interop: new `as_nparray()`, `toarray()`, and `fillWith(ndarray)` methods; optimized
          `Tensor` numpy access with zero-copy `__getitem__` and `memcpy`-based `__setitem__` (`numpy >=
          1.7` required).
    - **Graph API**:
        - Exposed `nameFromId()`, `idFromName()`, `setName()`, `hasName()` on all graph types (`DiGraph`,
          `DAG`, `MixedGraph`, ...).
        - Added `connectedComponents()` (returns `dict[int, int]`), `connectedComponentsList()`, and
          `connectedComponentsCount()`.
        - Fixed missing `descendants()` / `ancestors()` on `DiGraph`; `NodeSet`, `ArcSet`, `EdgeSet` now use dedicated
          typemaps.
    - **Documentation**:
        - Added sentinel typedefs to replace `-> object` with precise Python return type annotations in generated
          bindings.
        - Added `%feature(docstring)` entries for all 2174 methods; docstrings follow NumPy format (100% docstring
          coverage).
    - **Performance & Infrastructure**:
        - Introduced `PYTHONIZED_MARGINALS` macro and global numpy import for generated inference code.
        - Lazy-import of `pandas` via `TYPE_CHECKING` (~240 ms saved on import time).
        - Added `-fvisibility=hidden`: `_pyagrum.so` reduced from 16.8 MB to 12.9 MB (−23 %).
        - Added full type annotations to all pyLibs (`lib/`, `clg/`, `skbn/`, `bnmixture/`, `ctbn/`,
          `causalEffectEstimation/`); added inference type aliases `BNInference`, `MRFInference`,
          `CNInference`; replaced `mypy` with **pyrefly** in `act guideline`, fixing all type errors across pyLibs;
          added `MatrixLike`/`ArrayLike` type aliases to `__init__.in.py`.
        - `BNMixture`: manifest-based `saveBNM`/`loadBNM`, `loadRetroCompatibleBNM` for backward compatibility; fixed
          `saveBNM`/`loadBNM` on Windows (colons in BN names).
        - `explain` (ShAP / SHALL): API improvements, causal SHAP fix, binary BN test resources added; fixed
          uninitialized array in `_labelToPos` (`np.empty` → `np.zeros`).
        - Rewritten `ipython.py`, new `jt2graph.py`, fixed `prepareDot` fontcolor; fixed `html2image`
          issues in notebook contexts and improved export cropping.
        - `act install`: support no-make mode via `cmake --install`; `--only` flag (alias for `--build
          no-make`). `act test pyAgrum`: persistent `--test_build_path` option. Fixed `fastPrototype`
          docstring separator; removed useless `newFactory` methods.

- aGrUM

    - **Breaking API**:
        - Graph path-finding functions `directedPath`, `directedUnorientedPath`, `undirectedPath`,
          `mixedOrientedPath`, `mixedUnorientedPath` now return
          `std::optional<std::vector<NodeId>>` (`nullopt` when no path exists), for `DiGraph`, `UndiGraph`,
          `MixedGraph`, `PDAG`, `CliqueGraph`, and all `GUM_DiGraphable`/`GUM_UndiGraphable`/
          `GUM_MixedGraphable` graphs.
        - Causal module renames: `CausalModel::observedBN()` → `observationalBN()`;
          `CausalModel::addLatentVariable(..., keepArcs=...)` parameter `keepArcs` renamed to
          `assumeNonSpurious`; `CausalModel::backDoor()`/`frontDoor()` return type `NodeSet` →
          `std::optional<NodeSet>` (`nullopt` = no valid set, `{}` = empty set is valid); `DoorCriteria`'s
          `EnumerationOptions` struct removed (options are now direct parameters, all methods now
          `static`), `DoorCriteria::nodesOnDirectedPaths(dag, X, Y)` return type `NodeSet` →
          `std::optional<NodeSet>`; `CausalImpact` direct access to `result` removed (use `impact()`);
          `Counterfactual<GUM_ELEMENT>` template parameter renamed from `GUM_SCALAR`, `getResult()` renamed
          `impact()`; `DSeparation` (in `CM/tools/`) renamed `Separation`.
        - `HashTable<Key,Val>::tryGet(key)` and related methods (`tryFirst`, `trySecond`, `tryPos`) now return
          `optional_ref<Val>` / `optional_ref<const Val>` instead of `Val*` / `const Val*`
          (`nullptr` if absent). `gum::optional_ref<T>` behaves like `std::optional` for references (C++26 feature
          backported); callers using `if (auto* p = table.tryGet(key))` must switch to
          `if (auto ref = table.tryGet(key))`.
        - `Signaler1<A>`, `Signaler2<A,B>`, `Signaler3<A,B,C>`, ... removed; use variadic `Signaler<A>`,
          `Signaler<A,B>`, `Signaler<A,B,C>`, ... instead.
        - Widespread `string_view` migration: `const std::string&` parameters replaced by
          `std::string_view` across the public API (BayesNet, variables, I/O readers, learning, ...); passing
          `std::string` or string literals remains compatible.
        - I/O writers' `write()` no longer `const`: `BNWriter<GUM_SCALAR>::write()` (and all subclass writers, including
          `GumBNWriter`) now takes the BN by non-`const` reference, to allow writers to call `bn.updateMetaData()`
          before serialization; `CredalNet::saveBNsMinMax()` is similarly affected.
        - New base class `DiscreteGraphicalModel`: `IBayesNet` and `IMarkovRandomField` now inherit from it, which
          factors five variable-map accessors (`variable`, `variableNodeMap`, `nodeId`,
          `idFromName`, `existsInModel`) previously duplicated in each interface; direct subclasses must no longer
          define those accessors themselves.
        - `DiscreteVariable::closestLabel` is now virtual (ABI change: recompilation required for any code linking
          against aGrUM as a shared library).
        - `DAGmodel::dag()` and `UGmodel::graph()` now return value copies (with node names propagated), not `const`
          references; use `internalDag()` / `internalGraph()` for the O (1) stable `const`
          reference when graph mutation through the model API is not needed.
        - `StructuralComparator` renamed `StructuralMetrics` (update all include paths and type names accordingly).
    - **Structure Learning**:
        - Added the `FCI` (Fast Causal Inference) algorithm producing a `PAG` (Partial Ancestral Graph)
          from data; integrated into `IBNLearner`/`BNLearner` (`setAlgorithmFCI`); fixed `possibleDSep`
          criterion (Zhang 2008); added exhaustive sepset mode (`setFCIExhaustiveSepSet`); enforced background knowledge
          in orientation rules (R1/R2/R9/R10).
        - Added the `PC` constraint-based structure learning algorithm, integrated into
          `IBNLearner`/`BNLearner` alongside Miic.
        - Added the `GreedyThickThinning` score-based structure learning algorithm, integrated into
          `IBNLearner`/`BNLearner`.
        - Extended `GreedyHillClimbing` with arc-triangle deletion operations:
          `GraphChangesSelector4DiGraph` now supports `applyArcDeletion`, `applyArcReversal`,
          `applyTriangleDeletion`; `LocalSearchWithTabuList` updated; `totalOrder` constraint added.
        - Independence tests refactoring: extracted `CachedContingencyCounter` base class from Chi2 and G2
          implementations; `IndependenceTest::statistics()` made pure virtual (overridden in
          `IndepTestChi2` and `IndepTestG2`); added silent-cell df correction in Chi2/G2 tests; fixed G2 df for sampling
          zeros; new `Chi2TestSuite`.
        - `ConstraintBasedLearning` refactoring: extracted `ConstraintBasedLearning` base class from Miic; extracted
          `CIBasedLearning` base class adding scorer-agnostic API (`learnPDAG`/`learnDAG`/`learnBN`); CMI types and
          comparators moved to Miic;
          `setMutualInformation()` injector added; `applyStructuralConstraints_` factored into
          `ConstraintBasedLearning`; renamed `scores_and_tests/` directory to `scores/`.
    - **Causal Module (CM) Development** (thanks to SCALNYX):
        - Promoted the causal module from pure Python to a first-class C++ module.
        - Introduced `CausalModel` and `CausalFormula`.
        - Developed an Abstract Syntax Tree (AST) for do-calculus, including LaTeX export and evaluation.
        - Added the `DoorCriteria` class for backdoor and frontdoor set enumeration.
        - Implemented ID/IDC algorithms and formula introspection.
        - Added `counterfactual` and `counterfactualModel` functions with associated tests.
    - **Native GUM Format (jgum / bgum)**:
        - Added `GumBNReader`/`GumBNWriter`, `GumIDReader`/`GumIDWriter`, `GumMRFReader`/`GumMRFWriter`
          supporting both JSON (`.jgum`) and binary (`.bgum`) serialization.
        - All GUM readers support `proceedFromString()` and a no-filename constructor for in-memory round-trips.
        - Extracted `_readVector_`/`_writeVector_` helpers to `GumBinaryIO.h`.
        - Fixed empty-BN jgum serialization; fixed binary writers to open files with `ios::binary`; fixed
          `IDReader::proceed()` return type (`void` → `Size`).
    - **Modeling & Core API**:
        - Added optional node name support to `NodeGraphPart`: `nameFromId()`, `idFromName()`,
          `setName()`, `hasName()`; names propagated to `toDot()` output (format: `id:name`);
          `checkConsistency()` made public, `friend` declarations for test suites removed;
          `GraphicalModel::_nameNodes_()` propagates node names to returned graphs (`moralGraph()`,
          `moralizedAncestralGraph()`, `EssentialGraph::pdag()` / `skeleton()`, `MarkovBlanket::dag()`).
        - Introduced `DiscreteGraphicalModel` to factorize variable management across `IBayesNet` and
          `IMarkovRandomField`.
        - Added `connectedComponents()` to `DAGmodel` and `UGmodel`, with C++ and Python tests for BN, ID, MRF; added
          `descendants()` and `ancestors()` to `MarkovBlanket` and `EssentialGraph`.
        - Replaced nullable pointers with `optional_ref<T>` and implemented `std::optional` in various interfaces; added
          a `data()` method to `MultiDimArray` for contiguous buffer access.
        - Made `DiscreteVariable::closestLabel(double)` virtual; added `DiscreteVariable::isNumerical()`
          (returns `true` iff `varType != LABELIZED`).
        - Added `Tensor::mean()`, `variance()`, `stdDev()` (fixes variance computation: was `E[X²]`, now
          `E[(X−μ)²]`); added `isCloseToZero()` / `isCloseToOne()` helpers used in these methods;
          `Tensor::toString` now uses Unicode box-drawing characters (│ ║ ─) for table borders.
        - `StructuralMetrics` (formerly `StructuralComparator`): added SHD metrics with tp/fp/fn/tn accessors; added SID
          (Structural Intervention Distance) for DAG-vs-DAG and `BayesNet` overload; name-based alignment for BN
          compare/SID.
        - `BIFXMLBNReader` improvements (thanks to Omi Johnson): added `std::istream` constructor for in-memory parsing;
          now reads the network `NAME` property from BIF/XML files.
        - C++20 graph concepts: added `GUM_DiGraphable`, `GUM_UndiGraphable`, `GUM_MixedGraphable` concepts for
          graph-agnostic programming; added generic path/reachability/cycle algorithms operating on any
          concept-satisfying graph type; added generic moralization and separation algorithms (`DAG`/`PDAG`
          methods now delegate); added generic `BayesBall` algorithm (`dSeparated` and
          `BayesBall::requisiteNodes` delegate); promoted `minimalCondSet`, `markovBlanket`, `areConnected`
          to the generic graph layer.
        - `CN` module: fixed critical bugs (invalid `delete[]` on `strtok` pointer, `setCPT` const-ref signature,
          `insertEvidenceFile` override placement, operator precedence in `LrsWrapper` guards); fixed naming convention
          violations (protected attributes renamed to trailing-underscore convention).
    - **Code Quality & Static Analysis**:
        - Replaced `std::stringstream` with `std::format` across all modules; added `override` specifier to all virtual
          method overrides; added `[[nodiscard]]` to `clone()` and factory methods; normalized include guards to the
          `GUM_SOMETHING_H` convention; added parent `#include` in `_inl.h`/`_tpl.h`
          files for IDE LSP support.
        - Integrated `clang-tidy` into `act guideline` (`check tidy` / `--correction` applies fixes); a DeepSeek
          static-analysis audit fixed all CRIT/HIGH/MED/LOW issues across the codebase;
          `clang-format` applied to C++ test suites and BN learning sources; fixed `noexcept` on
          `IndepTestChi2`/`G2` moves and braced-init returns.
        - Portability: fixed `int2Pow` to use `uint64_t` for portable 64-bit shift on Windows; fixed GCC 16 warnings
          (`-Warray-bounds` pragmas, `gum::Size` casts for signed/unsigned comparisons); fixed GCC
          `optimize` pragma guard against Clang in `BNLearner`; fixed `-Wextra-semi` and `extern template`
          SWIG warnings; fixed Windows compilation issues (binary writers, CI uninstall).
        - Learning: `MeekRules` improvements; fixed `propagateToCPDAG` (restored edges-before-arcs insertion order in
          PDAG); fixed `ArcDeletion` bug in `GraphChangesSelector4DiGraph`; `BayesBall`
          (`_bayesBall_`) replaced the `exists+insert+[]` pattern with `getWithDefault`, plus 9 deterministic tests.
        - Migrated the test framework from CxxTest to **doctest** (updated from 2.4.12 to 2.5.2, suppressed
          `-Wc2y-extensions`); replaced `GUM_CHECK_*` macros with `CHECK_*` across all test suites; replaced deprecated
          `tmpnam` with a `getTempFilePath` helper across all test suites; added `LpInterface`/`LrsWrapper` tests and
          marginal sanity checks; removed improper `try/catch`
          logic, replacing it with explicit existence checks; optimized Coco/R parser performance; fixed various MSVC
          compilation issues and name lookup errors (notably regarding `gum::Arc`).
        - Added move constructors and move assignment operators across the class hierarchy; fixed GUM debug macros and
          the atexit table.
        - CMake: removed uninstall target and obsolete policies; `AVLTree` move `operator=` no longer
          `noexcept` (contains `GUM_ERROR`).
    - **Build & Tooling**:
        - `act`: `--stats` flag for project stats; `--consolidate` split; `cm.h` and `base/io` added to the dependency
          map; `act guideline` gains `--dry-run`, `--check` with `+/-` syntax, non-persistent
          `--verbose`.

# Older changelogs

- [Changelogs for `2.*.*`](changelogs/changelogs.2.md)
- [Changelogs for `1.*.*`](changelogs/changelogs.1.md)
- [Changelogs for `0.*.*`](changelogs/changelogs.0.md)
