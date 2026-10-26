# aGrUM Changelog

## Changelog for next version

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
