# Breaking API Changes since 2.3.2

## Graph — path-finding functions

**C++ (`gum::graph::` free functions and class methods)**

| Old signature | New signature |
|---|---|
| `std::vector<NodeId> directedPath(g, n1, n2)` | `std::optional<std::vector<NodeId>> directedPath(g, n1, n2)` |
| `std::vector<NodeId> directedUnorientedPath(g, n1, n2)` | `std::optional<std::vector<NodeId>> directedUnorientedPath(g, n1, n2)` |
| `std::vector<NodeId> undirectedPath(g, n1, n2)` | `std::optional<std::vector<NodeId>> undirectedPath(g, n1, n2)` |
| `std::vector<NodeId> mixedOrientedPath(g, n1, n2)` — empty = not found | `std::optional<std::vector<NodeId>> mixedOrientedPath(g, n1, n2)` — nullopt = not found |
| `std::vector<NodeId> mixedUnorientedPath(g, n1, n2)` — empty = not found | `std::optional<std::vector<NodeId>> mixedUnorientedPath(g, n1, n2)` — nullopt = not found |

Applies to `DiGraph`, `UndiGraph`, `MixedGraph`, `PDAG`, `CliqueGraph` (inherited), and all `GUM_DiGraphable`/`GUM_UndiGraphable`/`GUM_MixedGraphable` graphs.

**Python**

`undirectedPath`, `directedPath`, `directedUnorientedPath`, `mixedOrientedPath`, `mixedUnorientedPath` now return `None` instead of raising `NotFound` (or returning `[]`) when no path exists.

---

## CausalModel — renames and signature changes

### `CausalModel`

- `observedBN()` renamed → `observationalBN()`
- `addLatentVariable(..., keepArcs=...)` parameter `keepArcs` renamed → `assumeNonSpurious`
- `backDoor()`, `frontDoor()` — return type `NodeSet` → `std::optional<NodeSet>` to distinguish "no valid set exists" (`nullopt`) from "the empty set is valid" (`{}`). **Python**: return `None` instead of raising when impossible.

### `DoorCriteria`

- `EnumerationOptions` struct removed. Options (`excluded_nodes`, `max_cardinality`, `only_minimal`, `stopAtFirst`) are now direct parameters.
- All methods are now `static`.
- `nodesOnDirectedPaths(dag, X, Y)` — return type `NodeSet` → `std::optional<NodeSet>` (nullopt = no directed path).

### `CausalImpact`

- Direct access to `result` field removed. Use `impact()` (replaces the intermediate `getResult()`).

### `Counterfactual`

- Template parameter renamed: `GUM_SCALAR` → `GUM_ELEMENT` (i.e. `Counterfactual<GUM_ELEMENT>`).
- `getResult()` → `impact()`.

### `Separation` (formerly `DSeparation`)

- Class `DSeparation` (in `CM/tools/`) renamed → `Separation`.

---

## Core containers — `tryGet` / `tryFirst` / `trySecond` / `tryPos`

`HashTable<Key,Val>::tryGet(key)` and related methods:

| Old return type | New return type |
|---|---|
| `Val*` (nullptr if absent) | `optional_ref<Val>` |
| `const Val*` (nullptr if absent) | `optional_ref<const Val>` |

`gum::optional_ref<T>` behaves like `std::optional` for references (C++26 feature backported). Update callers:

```cpp
// before
if (auto* p = table.tryGet(key)) { use(*p); }
// after
if (auto ref = table.tryGet(key)) { use(*ref); }
```

Same applies to `tryFirst`, `trySecond`, `tryPos`.

---

## `Signaler` — variadic template

`Signaler1<A>`, `Signaler2<A,B>`, `Signaler3<A,B,C>`, … removed. Use variadic `Signaler<A>`, `Signaler<A,B>`, `Signaler<A,B,C>`, … instead.

---

## `string_view` migration (widespread)

`const std::string&` parameters replaced by `std::string_view` across the public API (BayesNet, variables, I/O readers, learning, …). Passing `std::string` or string literals remains compatible. Code that stored or compared `const std::string&` bindings directly may need adjustment.

---

## Python — type aliases removed (`ctbn`, `clg`)

`NodeId` and `NameOrId` type aliases in `pyAgrum.ctbn` and `pyAgrum.clg` have been removed. Use `int` where `NodeId` was expected, and `int | str` where `NameOrId` was expected.

---

## `DiscreteGraphicalModel` — new base class

`IBayesNet` and `IMarkovRandomField` now inherit from `DiscreteGraphicalModel`, which factors five variable-map accessors previously duplicated in each. Existing code using these classes as base types is unaffected, but direct subclasses of `IBayesNet` or `IMarkovRandomField` should check that the five accessors (`variable`, `variableNodeMap`, `nodeId`, `idFromName`, `existsInModel`) are no longer defined in those interfaces and are now resolved via `DiscreteGraphicalModel`.

---

## `DiscreteVariable::closestLabel` — now virtual

`closestLabel(double)` is now `virtual`. ABI change: recompilation required for any code that links against aGrUM as a shared library.
