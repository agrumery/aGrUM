# TODO_CLAUDE.md

Actions à réaliser lors d'une prochaine session.

**Règle** : ne jamais traiter un item de cette liste sans l'avoir signalé à l'utilisateur et obtenu son accord
explicite.

**Scores** : Importance et Difficulté sur 5.

---

## Dépendances

```
A1 (VariableContext) ──→ A1b (DiscreteGraphicalModel)
A1 (VariableContext)    A2 (Concepts graphes) ──→ A3 (Algos génériques)
        │                                                  │
        └──────────────────────────────────────────────→ B2 (SWIG)
B1 (Typage Python) ──────────────────────────────────→ B2 (SWIG, stubs)
```

---

## RAJOUTS MANUELS

### Problème acttools

quand on fait "act test agrum" , même si aucun fichier n'a changé , ni les tests à effectuer, il semble qu'il y a ait
recompilation de gumTestIncludes.cpp.o

Quand il y a une erreur de compilation (make en erreur), la suite de l'exécution se continue. Il faudrait plutôt s'arrêter en erreur


### GUM_CONSTRUCTOR, GUM_DESTRUCTOR, GUM_COPY, GUM_MOVE

Rajouter les macros GUM_CONSTRUCTOR, GUM_DESTRUCTOR, GUM_COPY, GUM_MOVE dans les classes de CM. Plus
généralement, vérifier dans toutes les classes.

### Tensor<->numpy.array

la classe `pyagrum.Tensor` possède un nouvel accesseur direct sans copie via numpy : `pyagrum.tensor.as_nparray`.

- Indiquer, dans la documentation et les requirements, quelle est version de numpy minimale nécessaire pour que cette
  fonctionnalité soit disponible.
- Vérifier si le code de tensor.i, et en particulier les opérateurs __getitem__/__setitem__ peuvent profiter de
  cette nouvelle fonctionnalité pour éviter des copies intermédiaires. En particulier, les opérations de slicing
  pourraient être optimisées.

## A. Architecture C++

### A1. VariableContext : ownership et identité sémantique des variables | I:5 D:5

#### Problème

`MultiDimImplementation<T>` stocke les variables comme pointeurs bruts (`Sequence<const DiscreteVariable*> _vars_`).
Deux problèmes distincts :

**1. Durée de vie implicite.** La variable doit outlive tous les Tensors qui la référencent —
contrat non vérifiable par le compilateur, violable dès qu'un Tensor est créé hors du modèle
graphique propriétaire. Dans le MRF, plusieurs facteurs partagent la même variable via ce
pointeur — cas le plus exposé.

**2. Absence d'identité sémantique.** Deux variables de même nom et type peuvent coexister
comme objets distincts (copies dans deux BNs séparés, variable locale passée à `add()`, etc.).
Les opérations entre Tensors reposant sur l'identité de pointeur, des variables sémantiquement
identiques mais physiquement distinctes bloquent les opérations directes entre Tensors.

#### Sémantique

Un `VariableContext` est à la fois **propriétaire** et **namespace** des variables : `intern()`
déduplique par `(nom, type concret)` — même nom + même type → même pointeur ; même nom + type
différent → `GUM_ERROR`. Deux Tensors dans le même contexte opèrent directement par identité de
pointeur, même issus de BNs distincts.

#### Architecture cible

```cpp
// src/agrum/base/variables/variableContext.h
class VariableContext {
public:
  /// Déduplication par (nom, type concret). Thread-safe (shared_mutex interne).
  const DiscreteVariable* intern(const DiscreteVariable& var);

  // Table globale nommée (partagée entre tous les threads)
  static std::shared_ptr<VariableContext> create(const std::string& name); // GUM_ERROR si existe
  static std::shared_ptr<VariableContext> get(const std::string& name);    // GUM_ERROR si absent
  static void select(const std::string& name);   // switch explicite sans RAII
  static std::shared_ptr<VariableContext> current();  // "default" si aucune sélection

  // Accès bas-coût pour la construction des Tensors (pas de refcount)
  static VariableContext*               currentRaw();   // lecture TLS sans atomic
  static std::weak_ptr<VariableContext> currentWeak();  // pour shadow debug uniquement

  // RAII — restaure le contexte précédent du thread à la sortie (composable)
  class Scope {
  public:
    explicit Scope(const std::string& name);
    explicit Scope(std::shared_ptr<VariableContext>);
    ~Scope();
  private:
    std::shared_ptr<VariableContext> previous_;
  };

private:
  std::vector<std::unique_ptr<DiscreteVariable>> vars_;
  mutable std::shared_mutex vars_mutex_;

  static std::unordered_map<std::string, std::shared_ptr<VariableContext>> registry_; // global
  static std::shared_mutex registry_mutex_;
  static thread_local std::shared_ptr<VariableContext> current_;  // par thread
};
// Le contexte "default" est créé à l'initialisation et toujours présent.
```

```cpp
// MultiDimDecorator<T> — classe parente de Tensor<T>
// context_ est un raw pointer : zéro atomic à la construction/destruction.
// Validité garantie par le Scope (ou GraphicalModel) qui détient le shared_ptr.
// MultiDimImplementation est inchangé.
class MultiDimDecorator<T> {
  VariableContext* context_;                         // raw — 8 octets, zéro atomic
#ifndef NDEBUG
  std::weak_ptr<VariableContext> context_dbg_;       // shadow : détecte les accès post-destruction
#endif
public:
  MultiDimDecorator() : context_(VariableContext::currentRaw()) {
#ifndef NDEBUG
    context_dbg_ = VariableContext::currentWeak();
#endif
  }
  void add(const DiscreteVariable& v) override {
    GUM_ASSERT_DBG(!context_dbg_.expired(), "Tensor outlives its VariableContext");
    content_->add(context_->intern(v));   // canonicalisation ici, stockage brut inchangé
  }
};
```

```cpp
// GraphicalModel — racine commune à BN, MRF, ID, CM, CN
// Porte context_ : tout algorithme prenant un GraphicalModel& accède à context() sans cast.
class GraphicalModel {
protected:
  std::shared_ptr<VariableContext> context_ = VariableContext::current();
public:
  std::shared_ptr<VariableContext> context() const { return context_; }
  // addVariable() dans chaque sous-classe : context_->intern() puis _varMap_.insert(ptr)
};
```

```cpp
// Algorithme d'inférence — adopte le contexte du modèle (Scope : composable)
class LazyPropagation {
  std::shared_ptr<VariableContext> ctx_;
public:
  explicit LazyPropagation(const IBayesNet<double>& bn) : ctx_(bn.context()) {}
  void makeInference() {
    VariableContext::Scope scope(ctx_);  // tous les Tensors intermédiaires → même contexte
  }
};
```

#### Modes d'usage

```cpp
// Mode 1 — code existant, inchangé
BayesNet<double> bn;
Tensor<double> t;
t.add(someVar);   // contexte "default"

// Mode 2 — namespace nommé, switch explicite
VariableContext::create("exp1");
VariableContext::select("exp1");
BayesNet<double> bn1, bn2;    // même contexte → opérations croisées directes
VariableContext::select("default");

// Mode 3 — Scope RAII (restaure le contexte précédent à la sortie)
{
  VariableContext::Scope ctx("exp1");
  BayesNet<double> bn1, bn2;
}

// Mode 4 — thread worker adoptant le contexte d'un BN existant
std::thread([ctx = bn.context()]() {
  VariableContext::Scope scope(ctx);
  // Tensors créés ici → même contexte que bn
}).join();
```

#### Overhead

**Tensors** — `MultiDimDecorator` gagne un raw pointer `VariableContext*` (8 octets), capturé
à la construction par lecture du `thread_local` sans refcount (zéro atomic). `MultiDimImplementation`
et `MultiDimArray` sont **inchangés**. `add()` coûte un `shared_lock` non contesté + hash lookup
— marginal, limité à la phase de construction. Les opérations de calcul (marginalisation, produit,
projection) comparent des pointeurs bruts inchangés : **overhead nul**.

En debug (`#ifndef NDEBUG`), un `weak_ptr` shadow est capturé à la construction et vérifié
avant chaque `add()` — absent en release, sans impact sur le chemin de calcul.

**Modèles graphiques** — `addVariable()` appelle `intern()` une fois par variable à la
construction du modèle (hors chemin critique). Toutes les lectures (`variable()`, itération sur
les nœuds) accèdent directement aux pointeurs bruts : **overhead nul**.

#### Ce qui change / ce qui ne change pas

|                                            | Aujourd'hui                    | Avec VariableContext                                       |
|--------------------------------------------|--------------------------------|------------------------------------------------------------|
| Stockage dans Tensor                       | `const DiscreteVariable*` brut | idem (dans contexte)                                       |
| `MultiDimImplementation` / `MultiDimArray` | —                              | **inchangés**                                              |
| Overhead calcul Tensor                     | nul                            | **nul**                                                    |
| Overhead construction/destruction Tensor   | nul                            | **nul** (raw pointer, zéro atomic)                         |
| Overhead `add()` / `addVariable()`         | nul                            | hash lookup + shared_lock (construction)                   |
| Constructeurs MultiDim / BN                | —                              | **inchangés** (pas de paramètre ctx)                       |
| Ownership variables                        | implicite / modèle             | `VariableContext`                                          |
| Durée de vie variable                      | manuelle                       | garantie par `shared_ptr` (dans modèle/Scope)              |
| Tensor outlive son contexte                | non détectable                 | **GUM_ERROR en debug** (shadow `weak_ptr`)                 |
| Opérations Tensors même BN                 | identité de pointeur           | **identique**                                              |
| Opérations Tensors inter-BNs (même ctx)    | échouent                       | **fonctionnent directement**                               |
| Opérations Tensors inter-contextes         | non détectées                  | exception explicite                                        |
| Thread safety                              | non garantie                   | `shared_mutex` (table + vars), `thread_local` pour current |

#### Rôle réduit de `_varMap_`

`VariableNodeMap` fait aujourd'hui deux choses : ownership et mapping `NodeId ↔ Variable* / name ↔ NodeId`.
Avec `VariableContext`, l'ownership passe au contexte. `_varMap_` devient un mapping pur, sans
responsabilité de durée de vie.

> **Note** : remonter `_varMap_` dans `GraphicalModel` est traité en A1b.

#### Spécificités par modèle

Tous les modèles suivent le même patron : `context_` hérité de `GraphicalModel`, `_varMap_`
(ou `_variableMap_` pour ID) réduit au mapping `NodeId ↔ ptr`.

- **BayesNet** : les Tensors d'inférence sont valides tant que le modèle vit. Pour un Tensor
  destiné à outlive le BN, capturer explicitement `bn.context()` dans un `shared_ptr<VariableContext>`
  externe afin de prolonger la durée de vie du contexte.
- **InfluenceDiagram** : `_tensorMap_` et `_utilityMap_` partagent le même `context_`.
- **MarkovRandomField** : facteurs indexés par `NodeSet` (entiers) — non affectés. Suppression de nœud : inchangée.
- **CN, CM** : héritent de BayesNet — même patron, à vérifier.
- **Algorithmes d'inférence** : `Scope(model.context())` en entrée de chaque méthode créant des Tensors.

#### Ordre d'implémentation

1. **`variableContext.h/.cpp`** — `intern()`, table nommée (`create/get/select`), `Scope`,
   `current()`, `currentRaw()`, `currentWeak()`, `shared_mutex` (table + vars).
2. **`multiDimDecorator`** — `context_` raw pointer + shadow `context_dbg_` (`#ifndef NDEBUG`) ;
   `assertContextAlive_()` appelée dans `add()` et les opérations binaires.
3. **`GraphicalModel`** — `context_`, `context()`, `addVariable()` via `intern()` + `_varMap_`.
4. **`BayesNet`, `InfluenceDiagram`, `MarkovRandomField`** — adapter `addVariable()` ;
   vérifier suppression de nœud MRF.
5. **CN, CM** — vérifier.
6. **Inférence** — `Scope(model.context())` dans les algorithmes.
7. **Tests** — (a) Tensor survit à la destruction du modèle ; (b) opérations inter-BNs
   dans le même contexte ; (c) conflit de type → exception ; (d) thread worker adopte le contexte.
8. **pyAgrum / SWIG** — exposer `VariableContext` et `Scope` (voir B2).

#### Hors scope

- Opérations entre Tensors de contextes **distincts** : erreur explicite (`context_ == other.context_`).
- `StructuredBayesBall` (PRM) : migration séparée, même principe.

---

~~### A1b. `DiscreteGraphicalModel` : factoriser `_varMap_` | I:3 D:2~~

~~**TERMINÉ** — branche `feature/moveVarMap`, fusionnable dans `feature/wrapperCausalCPP`.~~

~~`DiscreteGraphicalModel` introduite entre `GraphicalModel` et `DAGmodel`/`UGmodel` :
`varMap_` (protected) + 5 accesseurs (`variableNodeMap`, `variable`, `nodeId`, `idFromName`,
`variableFromName`) implémentés une seule fois. `_varMap_`/`_variableMap_` supprimés de BN,
MRF, ID. Bindings SWIG mis à jour (`%include discreteGraphicalModel.h`).
Note : `addVariable()` reste à faire dans le cadre de A1 (dépend de `VariableContext`).~~

---

### A2. Concepts C++20 pour les interfaces graphes | I:4 D:3

Définir des concepts C++20 capturant les interfaces structurelles des graphes aGrUM, sur le
modèle de `GUM_Numeric` dans `src/agrum/base/core/concepts.h`.

#### Fichier cible

`src/agrum/base/graphs/graphConcepts.h` — séparé de `concepts.h` car il dépend de
`NodeId`/`NodeSet` depuis `graphElements.h`.

#### Concepts à définir

```cpp
namespace gum {

  template < typename T >
  concept GUM_NodeGraphable = requires(const T g, NodeId id) {
    { g.existsNode(id) } -> std::convertible_to< bool >;
    { g.size()         } -> std::convertible_to< Size >;
    { g.empty()        } -> std::convertible_to< bool >;
  };

  template < typename T >
  concept GUM_DiGraphable = GUM_NodeGraphable< T >
    && requires(const T g, NodeId u, NodeId v) {
      { g.existsArc(u, v) } -> std::convertible_to< bool >;
      { g.parents(u)      } -> std::convertible_to< const NodeSet& >;
      { g.children(u)     } -> std::convertible_to< const NodeSet& >;
    };

  template < typename T >
  concept GUM_UndiGraphable = GUM_NodeGraphable< T >
    && requires(const T g, NodeId u, NodeId v) {
      { g.existsEdge(u, v) } -> std::convertible_to< bool >;
      { g.neighbours(u)    } -> std::convertible_to< const NodeSet& >;
    };

  template < typename T >
  concept GUM_MixedGraphable = GUM_DiGraphable< T > && GUM_UndiGraphable< T >;

}
```

#### Ordre d'implémentation

1. Créer `graphConcepts.h` avec les 4 concepts.
2. Vérifier via `static_assert` que `DAG`, `DiGraph`, `UndiGraph`, `MixedGraph`, `PDAG`
   satisfont chacun le concept attendu.
3. Appliquer aux paramètres template des algorithmes (voir A3).

---

### A3. Algorithmes graphes génériques | I:4 D:4

**Prérequis : A2 terminé.**

#### Objectif

Extraire les algorithmes de traversée/analyse implémentés dans les classes graphes en
**fonctions template libres** contraintes par les concepts de A2, dans un nouveau répertoire
`src/agrum/base/graphs/algorithms/generic/`. Les méthodes des classes deviennent de simples
délégations.

#### Patron général

```cpp
// algorithms/generic/pathFinding.h
namespace gum::graph {
  template < GUM_DiGraphable G >
  bool hasDirectedPath(const G& g, NodeId from, NodeId to);
}

// Dans DiGraph / arcGraphPart :
bool DiGraph::hasDirectedPath(NodeId from, NodeId to) {
  return graph::hasDirectedPath(*this, from, to);
}
```

#### Structure cible

```
src/agrum/base/graphs/algorithms/generic/
├── pathFinding.h      // directedPath, undirectedPath, directedUnorientedPath,
│                      //   hasDirectedPath, hasUndirectedPath (toutes variantes)
├── reachability.h     // ancestors, descendants, family, chainComponent,
│                      //   nodes2ConnectedComponent, boundary
├── cycleDetection.h   // topologicalOrder (DiGraphable), hasUndirectedCycle (UndiGraphable)
├── moralization.h     // moralGraph, moralizedAncestralGraph (Di- et MixedGraphable)
└── separation.h       // dSeparation / minimalCondSet (DiGraphable),
                       //   cSeparation (MixedGraphable), bayesBallRequisiteNodes
```

#### Inventaire des méthodes à migrer

| Fonction générique                         | Méthode(s) source                      | Concept                                  |
|--------------------------------------------|----------------------------------------|------------------------------------------|
| `directedPath`                             | `ArcGraphPart::directedPath`           | `GUM_DiGraphable`                        |
| `directedUnorientedPath`                   | `ArcGraphPart::directedUnorientedPath` | `GUM_DiGraphable`                        |
| `hasDirectedPath`                          | `DiGraph::hasDirectedPath`             | `GUM_DiGraphable`                        |
| `ancestors`, `descendants`                 | `ArcGraphPart::ancestors/descendants`  | `GUM_DiGraphable`                        |
| `family` (NodeId et NodeSet)               | `ArcGraphPart::family`                 | `GUM_DiGraphable`                        |
| `undirectedPath`                           | `EdgeGraphPart::undirectedPath`        | `GUM_UndiGraphable`                      |
| `hasUndirectedPath` (3 surcharges)         | `EdgeGraphPart::hasUndirectedPath`     | `GUM_UndiGraphable`                      |
| `hasUndirectedCycle`                       | `UndiGraph::hasUndirectedCycle`        | `GUM_UndiGraphable`                      |
| `nodes2ConnectedComponent`                 | `UndiGraph::nodes2ConnectedComponent`  | `GUM_UndiGraphable`                      |
| `topologicalOrder`                         | `DiGraph::topologicalOrder`            | `GUM_DiGraphable`                        |
| `chainComponent`, `boundary`               | `MixedGraph::chainComponent/boundary`  | `GUM_MixedGraphable`                     |
| `mixedOrientedPath`, `mixedUnorientedPath` | `MixedGraph::*`                        | `GUM_MixedGraphable`                     |
| `hasMixedOrientedPath`                     | `MixedGraph::hasMixedOrientedPath`     | `GUM_MixedGraphable`                     |
| `moralGraph`                               | `DAG::moralGraph`, `PDAG::moralGraph`  | `GUM_DiGraphable` / `GUM_MixedGraphable` |
| `moralizedAncestralGraph`                  | `DAG::`, `PDAG::`                      | `GUM_DiGraphable` / `GUM_MixedGraphable` |
| `dSeparation`, `minimalCondSet`            | `DAG::`                                | `GUM_DiGraphable`                        |
| `cSeparation`                              | `PDAG::`                               | `GUM_MixedGraphable`                     |
| `bayesBallRequisiteNodes`                  | `BayesBall::requisiteNodes`            | `GUM_DiGraphable`                        |

**Hors scope** : `DAGCycleDetector` (stateful, incrémental), `MeekRules` (logique
PDAG-spécifique), `SimplicialSet` (triangulation), `SpanningForest` (déjà abstrait).

#### Cas particulier : BayesBall

`BayesBall::requisiteNodes(const DAG& dag, ...)` n'utilise que `dag.parents()` et
`dag.children()` — pure `GUM_DiGraphable`. Son cœur migre dans `separation.h`.
`BayesBall::relevantTensors` garde sa dépendance `IBayesNet` (mapping variable→NodeId)
mais délègue la traversée graphe à la fonction générique. La classe `BayesBall` reste
dans `BN/algorithms/`.

#### Ordre d'implémentation

1. `pathFinding.h` — bon banc d'essai du patron.
2. `reachability.h` — s'appuie sur pathFinding.
3. `cycleDetection.h`
4. `moralization.h` — dépend de reachability.
5. `separation.h` — dépend de moralization et reachability ; inclut `bayesBallRequisiteNodes`.
6. Mise à jour de `BayesBall` pour déléguer à `separation.h`.

À chaque étape : `act test release aGrUM -m BASE+BN`.

---

## B. Couche Python / SWIG

### B1. Typage Python | I:4 D:3

À traiter dans l'ordre (chaque étape débloque la suivante).

1. **`wrappers/pyagrum/pyLibs/` — Déplacer `_types.py`** | I:4 D:1

   `pyLibs/causal/_types.py` contient `NodeId`, `NodeSet`, `NameSet`, etc., utiles à
   l'ensemble de `pyLibs/`. Le déplacer en `pyLibs/_types.py` et mettre à jour tous les
   imports. Vérifier l'absence de redéfinitions ailleurs.

2. **`wrappers/pyagrum/pyLibs/**/*.py` — Compléter les annotations de type** | I:3 D:2

   Parcourir tous les fichiers et ajouter les annotations manquantes (paramètres et retours).
   S'appuyer sur `pyLibs/_types.py`. Ne pas modifier les docstrings NumPy existantes.

3. **`wrappers/pyagrum/` — Stubs `.pyi` pour les classes SWIG** | I:4 D:4 (multi-étapes)

    1. **Génération** : lancer `stubgen -p pyagrum` à chaque `act install pyAgrum`, sortie
       dans `pyagrum-stubs/`.
    2. **Post-traitement** : script remplaçant les `Any` génériques par les vrais types, en
       lisant `gum_typemaps.i` et les annotations de `pyLibs/`.
    3. **Marqueur** : ajouter `py.typed` dans le package une fois les stubs opérationnels.
    4. **Test CI** : vérifier via `inspect` qu'aucune méthode publique n'a disparu des stubs
       entre deux builds.

---

### B2. SWIG | I:3 D:3

- **Simplifier la structure des includes** | I:3 D:3

  Analyser l'ordre des `%include` dans `pyagrum.i` et `aGrUM_wrap_*.i` / `swigsrc/*.i`.
  Vérifier l'absence de redondances et que la convention "swigsrc avant aGrUM_wrap" est
  appliquée partout.

- **Audit des typemaps manquants** | I:3 D:3

  Identifier les blocs `%extend` / code inline qui appellent manuellement des helpers de
  conversion (`populateHashTableStrStrFromPyDict`, `populateStringSetFromPySequence`, etc.)
  alors qu'un typemap existe ou pourrait exister. Remplacer par le typemap correspondant.

- **Audit des `%ignore` redondants** | I:2 D:2

  Vérifier pour chaque `%ignore` si le retrait produit un warning SWIG ou une API non
  désirée. Candidats prioritaires : `%ignore gum::MultiDimWithOffset/MultiDimImplementation/
  MultiDimInterface/MultiDimDecorator/MultiDimArray` dans `tensor.i`.

- **Exposition de `VariableContext`** | I:3 D:3 *(dépend de A1)*

  Exposer `VariableContext` et `Scope` via SWIG. Adapter les typemaps si nécessaire pour
  que les modèles Python puissent partager un contexte explicite.

---

## C. Maintenance ✓ (fait)

~~**`src/agrum/` — Namespaces anonymes** | I:3 D:2~~

~~**`src/agrum/` — Méthodes `= default` et redondances** | I:2 D:2~~
