# TODO_CLAUDE.md

Actions à réaliser lors d'une prochaine session.

**Règle** : ne jamais traiter un item de cette liste sans l'avoir signalé à l'utilisateur et obtenu son accord
explicite.

**Scores** : Importance et Difficulté sur 5.

---

## Dépendances

```
A1 (VariableContext)   A2 (Concepts graphes) ──→ A3 (Algos génériques)
        │                                                  │
        └──────────────────────────────────────────────→ B2 (SWIG)
B1 (Typage Python) ──────────────────────────────────→ B2 (SWIG, stubs)
C  (Namespaces anonymes) — indépendant
```

---

## A. Architecture C++

### A1. VariableContext : supprimer les dangling pointers dans MultiDim/Tensor | I:5 D:5

#### Problème

`MultiDimImplementation<T>` stocke les variables comme pointeurs bruts :
```cpp
Sequence<const DiscreteVariable*> _vars_;
```
Le contrat de durée de vie (la variable doit outlive tous les Tensors qui la référencent) est
implicite, non vérifiable par le compilateur, et violable dès qu'un Tensor est créé hors du
modèle graphique qui possède ses variables. Dans le MRF, plusieurs facteurs partagent la même
variable via ce pointeur — c'est le cas le plus exposé.

#### Architecture cible

Un `VariableContext` est le propriétaire exclusif des variables. Modèles graphiques et Tensors
partagent le même contexte via `shared_ptr<VariableContext>`. Les Tensors conservent des
**pointeurs bruts** dans ce contexte — zéro overhead, mais garantis valides tant qu'un
`shared_ptr` existe.

```cpp
// src/agrum/base/variables/variableContext.h
namespace gum {

  class VariableContext {
  public:
    /// Insère une variable (par copie) et retourne un pointeur stable.
    const DiscreteVariable* intern(const DiscreteVariable& var);

    /// Vérifie qu'une variable appartient bien à ce contexte.
    bool owns(const DiscreteVariable* ptr) const;

    /// Contexte par défaut pour les Tensors créés sans contexte explicite
    /// (singleton statique — variables à durée de vie indéfinie, comportement
    /// identique à l'actuel pour les usages standalone et les tests).
    static std::shared_ptr<VariableContext> defaultContext();

  private:
    std::vector<std::unique_ptr<DiscreteVariable>> vars_;
  };

}
```

```cpp
// MultiDimImplementation
class MultiDimImplementation<T> {
  std::shared_ptr<VariableContext> context_;   // tient le contexte en vie
  Sequence<const DiscreteVariable*> _vars_;    // pointeurs bruts dans le contexte

public:
  explicit MultiDimImplementation(std::shared_ptr<VariableContext> ctx
                                  = VariableContext::defaultContext());
  void add(const DiscreteVariable& v);   // assertion : context_->owns(&v)
};
```

```cpp
// BayesNet
class BayesNet<GUM_SCALAR> {
  std::shared_ptr<VariableContext> context_;   // remplace _varMap_ comme owner
public:
  explicit BayesNet(std::shared_ptr<VariableContext> ctx
                    = std::make_shared<VariableContext>());
  std::shared_ptr<VariableContext> context() const;
};
```

#### Ce qui change / ce qui ne change pas

| | Aujourd'hui | Avec VariableContext |
|---|---|---|
| Stockage dans Tensor | `const DiscreteVariable*` (brut) | `const DiscreteVariable*` (brut dans contexte) |
| Overhead d'accès aux variables | nul | **nul** |
| Ownership | implicite / modèle graphique | `VariableContext` |
| Durée de vie variable | manuelle | garantie par `shared_ptr` |
| Tensor sans contexte | créé librement | → contexte par défaut |
| Opérations inter-tensors | par identité de pointeur | **identique** |
| Copie de modèle | copie variables en interne | crée un nouveau contexte avec copies |

#### Spécificités par modèle graphique

Tous les modèles utilisent `VariableNodeMap` pour la possession des variables — la migration
de l'ownership vers `VariableContext` est structurellement uniforme. Les structures de facteurs
diffèrent mais n'affectent pas ce schéma.

**BayesNet** : `_varMap_` → `VariableContext`. Un Tensor d'inférence reçoit `bn.context()` ;
la destruction du BN ne détruit pas les variables tant que ce Tensor vit.

**InfluenceDiagram** : `_variableMap_` → `VariableContext`. Deux maps de Tensors
(`_tensorMap_` chance/décision, `_utilityMap_` utilité) reçoivent toutes deux le même
`shared_ptr<VariableContext>` à la construction.

**MarkovRandomField** : `_varMap_` → `VariableContext`. Les facteurs sont indexés par
`NodeSet` (entiers) et non par pointeurs de variables — l'indexation n'est pas affectée.
La procédure de suppression de nœud (recherche multi-facteurs par NodeId) fonctionne sans
modification.

**CN, CM** : héritent de BayesNet — même patron, à vérifier lors de la migration.

#### Ordre d'implémentation

1. **`variableContext.h/.cpp`** — `intern()`, `owns()`, `defaultContext()`.
2. **`multiDimImplementation`** — ajouter `context_`, assertion dans `add()`.
3. **`BayesNet`** — remplacer `_varMap_` comme owner ; exposer `context()`.
4. **`InfluenceDiagram`** — remplacer `_variableMap_` ; propager aux deux maps de Tensors.
5. **`MarkovRandomField`** — remplacer `_varMap_` ; vérifier la suppression de nœud.
6. **CN, CM** — même migration.
7. **Inférence** — les algorithmes créant des Tensors intermédiaires passent `model.context()`.
8. **Tests** — (a) Tensor survit à la destruction du modèle ; (b) suppression nœud MRF
   sans pointeurs invalides.
9. **pyAgrum / SWIG** — exposer `VariableContext` (voir B2).

#### Hors scope

- Opérations entre Tensors de contextes **distincts** : restent une erreur
  (`context_ == other.context_` dans les opérateurs). Comportement actuel préservé.
- `StructuredBayesBall` (PRM) : migration séparée, même principe.

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

| Fonction générique | Méthode(s) source | Concept |
|--------------------|-------------------|---------|
| `directedPath` | `ArcGraphPart::directedPath` | `GUM_DiGraphable` |
| `directedUnorientedPath` | `ArcGraphPart::directedUnorientedPath` | `GUM_DiGraphable` |
| `hasDirectedPath` | `DiGraph::hasDirectedPath` | `GUM_DiGraphable` |
| `ancestors`, `descendants` | `ArcGraphPart::ancestors/descendants` | `GUM_DiGraphable` |
| `family` (NodeId et NodeSet) | `ArcGraphPart::family` | `GUM_DiGraphable` |
| `undirectedPath` | `EdgeGraphPart::undirectedPath` | `GUM_UndiGraphable` |
| `hasUndirectedPath` (3 surcharges) | `EdgeGraphPart::hasUndirectedPath` | `GUM_UndiGraphable` |
| `hasUndirectedCycle` | `UndiGraph::hasUndirectedCycle` | `GUM_UndiGraphable` |
| `nodes2ConnectedComponent` | `UndiGraph::nodes2ConnectedComponent` | `GUM_UndiGraphable` |
| `topologicalOrder` | `DiGraph::topologicalOrder` | `GUM_DiGraphable` |
| `chainComponent`, `boundary` | `MixedGraph::chainComponent/boundary` | `GUM_MixedGraphable` |
| `mixedOrientedPath`, `mixedUnorientedPath` | `MixedGraph::*` | `GUM_MixedGraphable` |
| `hasMixedOrientedPath` | `MixedGraph::hasMixedOrientedPath` | `GUM_MixedGraphable` |
| `moralGraph` | `DAG::moralGraph`, `PDAG::moralGraph` | `GUM_DiGraphable` / `GUM_MixedGraphable` |
| `moralizedAncestralGraph` | `DAG::`, `PDAG::` | `GUM_DiGraphable` / `GUM_MixedGraphable` |
| `dSeparation`, `minimalCondSet` | `DAG::` | `GUM_DiGraphable` |
| `cSeparation` | `PDAG::` | `GUM_MixedGraphable` |
| `bayesBallRequisiteNodes` | `BayesBall::requisiteNodes` | `GUM_DiGraphable` |

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

- **`wrappers/pyagrum/swigsrc/*.i` + `pyagrum.i` — Simplifier la structure des includes**
  | I:3 D:3

  Analyser l'ordre des `%include` dans `pyagrum.i` et `aGrUM_wrap_*.i` / `swigsrc/*.i`.
  Vérifier l'absence de redondances et que la convention "swigsrc avant aGrUM_wrap" est
  appliquée partout.

- **`wrappers/pyagrum/swigsrc/*.i` — Audit des typemaps manquants** | I:3 D:3

  Identifier les blocs `%extend` / code inline qui appellent manuellement des helpers de
  conversion (`populateHashTableStrStrFromPyDict`, `populateStringSetFromPySequence`, etc.)
  alors qu'un typemap existe ou pourrait exister. Remplacer par le typemap correspondant.

- **`wrappers/pyagrum/swigsrc/*.i` — Audit des `%ignore` redondants** | I:2 D:2

  Vérifier pour chaque `%ignore` si le retrait produit un warning SWIG ou une API non
  désirée. Candidats prioritaires : `%ignore gum::MultiDimWithOffset/MultiDimImplementation/
  MultiDimInterface/MultiDimDecorator/MultiDimArray` dans `tensor.i`.

- **`VariableContext` — Exposition SWIG** | I:3 D:3 *(dépend de A1)*

  Exposer `VariableContext` via SWIG. Adapter les typemaps si nécessaire pour que les
  modèles Python puissent partager un contexte explicite.

---

## C. Maintenance

**`src/agrum/` — Namespaces anonymes** | I:3 D:2

Utiliser `namespace { ... }` pour toute fonction/variable interne à un `.cpp` (au lieu de
`static` ou d'une exposition accidentelle dans le namespace global).

**`src/agrum/` — Méthodes `= default` et redondances** | I:2 D:2

Passer en revue les constructeurs/destructeurs/opérateurs de copie/déplacement triviaux et
les remplacer par `= default` quand applicable. Identifier et supprimer les opérateurs
redondants (`!=` exprimable via `==`, comparaisons dérivables via `<=>`, etc.).
