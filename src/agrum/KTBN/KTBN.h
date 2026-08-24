/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Class representing k-order dynamic Bayesian networks (k-DBN).
 *
 * A gum::KTBN stores a compact <i>template</i> of @f$k@f$ consecutive time
 * slices that fully characterizes a (time-homogeneous) k-order dynamic
 * Bayesian network. It can be unrolled into a standard gum::BayesNet for any
 * number of time slices.
 *
 * @author Seth AGUILA & Anis KHACEF
 */
#ifndef GUM_KTBN_H
#define GUM_KTBN_H

#include <concepts>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/BN/BayesNet.h>

namespace gum {

  /**
   * @class KTBNModality
   * @headerfile KTBN.h <agrum/KTBN/KTBN.h>
   * @brief A parent's value in gum::KTBN::fillCPT(): a modality index @b or a
   * modality label.
   *
   * Implicitly built from either spelling, so a single fillCPT() signature
   * serves both and one parent map may mix them:
   * @code
   *   m.fillCPT("X", 1, {{{"X", 0}, 1}, {{"C", KTBN::ATEMPORAL}, "high"}}, {0.6, 0.4});
   * @endcode
   *
   * @warning An unquoted integer is always an @b index, a quoted string always a
   * @b label. The two differ for a variable whose modalities are themselves
   * numeric: on a @c RangeVariable over 2..5, @c 2 is the third modality (the
   * one labelled @c "4") while @c "2" is the first. Quote the value whenever you
   * mean the label.
   *
   * @note Two overloads differing only in the value type (@c Idx vs
   * @c std::string) would be ambiguous for every braced argument, a braced list
   * having no type to resolve on — hence this wrapper.
   */
  struct KTBNModality {
    /// @brief From a modality index.
    template < std::integral T >
    KTBNModality(T modality);

    /// @brief From a modality label.
    KTBNModality(const char* modality);

    /// @brief From a modality label.
    KTBNModality(std::string modality);

    /**
     * @brief Resolves this value to a modality index of @p var, looking the
     * label up when one was given.
     * @throw NotFound / OutOfBounds if the index is out of @p var's domain or
     * the label is not one of its modalities (which of the two is raised for an
     * unknown label depends on the variable type).
     */
    Idx toIndex(const DiscreteVariable& var) const;

    /// @brief Whether the value was spelled as a label rather than an index.
    bool isLabel;
    /// @brief The index, when isLabel is false.
    Idx index{};
    /// @brief The label, when isLabel is true.
    std::string label;
  };

  /**
   * @class KTBN
   * @headerfile KTBN.h <agrum/KTBN/KTBN.h>
   * @brief Class representing a k-order dynamic Bayesian network (k-DBN).
   * @ingroup bn_group
   *
   * A <b>k-order dynamic Bayesian network</b> (k-DBN, sometimes called a
   * k-Time-slice Bayesian Network or k-TBN) generalizes the classical 2-TBN:
   * the conditional distribution of a variable at time @f$t@f$ may depend on
   * the @f$k@f$ most recent time slices @f$t, t-1, \ldots, t-k+1@f$ instead of
   * a single step backward.
   *
   * Rather than storing an (infinite) unrolled network, a KTBN only stores a
   * @b template made of exactly @f$k@f$ time slices. This template captures
   * both:
   *  - the @b initial @b distribution, encoded by slices @f$0, \ldots, k-2@f$
   *    (the initial slices, where the full @f$k@f$-history is not yet available), and
   *  - the @b transition @b kernel, encoded by slice @f$k-1@f$ as a function of
   *    slices @f$0, \ldots, k-1@f$. Because the process is time-homogeneous,
   *    this kernel is reused for every @f$t \geq k-1@f$ when unrolling.
   *
   * Two kinds of random variables are distinguished:
   *  - @b temporal @b variables (also called @e processes here), which evolve
   *    through time and are therefore represented by @f$k@f$ instances (one per
   *    slice) in the template;
   *  - @b atemporal @b variables, which are constant through time (e.g. a static
   *    context parameter) and are represented by a single instance.
   *
   * @par Node identity: the bracket notation
   * The @b identity of a node in the underlying engine is its name. Temporal
   * nodes are stored using bracket notation: the @f$t@f$-th instance of a
   * process @c base is named <tt>base[t]</tt> (e.g. <tt>"X[0]"</tt>,
   * <tt>"X[1]"</tt>, ...). Atemporal variables keep their bare name. The class
   * holds @b no gum::NodeId index table: a (process, slice) couple is resolved
   * to its node by encoding it and asking the engine, and a node's slice is
   * recovered by decoding its name. The public API is always expressed in terms
   * of (process, slice) couples; the bracket encoding is an internal detail.
   *
   * @par Internal storage
   * Structure and parameters are stored in an underlying gum::BayesNet (the
   * template) used purely as a storage engine, so that gum::Tensor CPTs, cycle
   * detection, topological ordering and the existing I/O readers/writers are
   * reused as-is. The gum::BayesNet is the @b single source of truth; the class
   * only caches two name sets (the temporal processes and the atemporal
   * variables) for fast O(1) membership queries. These sets are
   * std::unordered_set, so their iteration order is unspecified; methods that
   * need a deterministic order must sort explicitly.
   *
   * @par Name collision rule
   * All base names must be @b globally @b unique: a name cannot be used for
   * both a temporal process and an atemporal variable simultaneously.  In
   * addition, an atemporal variable named @c "X[0]" is forbidden when a
   * temporal process @c "X" exists (and vice-versa), because both would map
   * to the same BN node @c "X[0]".  The reservation covers @b every
   * bracket-suffixed name over an existing process, not only the slices that
   * process actually holds: @c "X[999]" is refused for @f$k=3@f$ as well, since
   * the slice index is parsed without an upper bound and such a name would
   * otherwise shadow the process.  add() and changeVariableName() enforce these
   * rules and throw @c DuplicateLabel or @c InvalidArgument on violation.
   *
   * @par Arc semantics
   * Arcs must respect temporal causality. For an arc @f$u \rightarrow v@f$:
   *  - if @e v is atemporal, then @e u must be atemporal too (a static variable
   *    cannot depend on a time-varying one);
   *  - if @e v is temporal at slice @f$t_v@f$ and @e u is temporal at slice
   *    @f$t_u@f$, then @f$t_u \leq t_v@f$ (no arc from the future to the past).
   *    The @e lag of the arc is @f$t_v - t_u \in [0, k-1]@f$;
   *  - an atemporal @e u may point to a temporal @e v at any slice.
   *
   * @warning Unlike the Python prototype, unroll() correctly handles arcs of
   * @e arbitrary lag (e.g. an arc <tt>X[0] -> X[2]</tt> for @f$k=3@f$), not
   * only arcs between consecutive slices.
   */
  template < GUM_Numeric GUM_SCALAR >
  class KTBN {
    public:
    /// Conventional time-slice value denoting an atemporal (static) variable.
    static constexpr int ATEMPORAL = -1;

    // ===========================================================================
    /// @name Constructors and Destructor
    // ===========================================================================
    /// @{

    /**
     * @brief Default constructor.
     * @param k The order of the k-DBN, i.e. the number of time slices stored in
     * the template. Must be @f$\geq 1@f$.
     * @throw InvalidArgument if @c k is 0.
     */
    explicit KTBN(Size k = 2);

    /// @brief Destructor.
    virtual ~KTBN();

    /// @brief Copy constructor.
    KTBN(const KTBN< GUM_SCALAR >& source);

    /// @brief Move constructor.
    KTBN(KTBN< GUM_SCALAR >&& source) noexcept;

    /// @brief Copy assignment operator.
    KTBN< GUM_SCALAR >& operator=(const KTBN< GUM_SCALAR >& source);

    /// @brief Move assignment operator.
    KTBN< GUM_SCALAR >& operator=(KTBN< GUM_SCALAR >&& source) noexcept;

    /// @}
    // ===========================================================================
    /// @name Accessors
    // ===========================================================================
    /// @{

    /// @return The order @f$k@f$ of the k-DBN.
    Size k() const;

    /// @return The number of nodes in the template (all slices).
    Size size() const;

    /// @return The number of arcs in the template.
    Size sizeArcs() const;

    /// @return @c true if the template contains no variable.
    bool empty() const;

    /**
     * @brief Removes all variables and arcs, keeping the order @f$k@f$.
     */
    void clear();

    /// @}
    // ===========================================================================
    /// @name Variable management
    // ===========================================================================
    /// @{

    /**
     * @brief Adds a variable to the k-DBN.
     *
     * A temporal variable (process) is cloned into @f$k@f$ instances (one per
     * slice); an atemporal variable is added once.
     *
     * @param var The variable to add (added by copy).
     * @param temporal Whether the variable is temporal.
     * @throw DuplicateLabel if a variable with that base name already exists
     * (any kind — base names are globally unique).
     * @throw InvalidArgument if a temporal @c base[t] would collide with an
     * existing atemporal node of that encoded name, or vice-versa.
     */
    void add(const DiscreteVariable& var, bool temporal = true);

    /// @brief Adds a variable using the "fast" textual description syntax.
    /// @see gum::BayesNet::add(std::string_view,unsigned int)
    void add(std::string_view fast_description,
             bool             temporal       = true,
             unsigned int     default_nbrmod = 2);

    /// @brief Convenience shortcut for <tt>add(var, true)</tt>.
    void addTemporal(const DiscreteVariable& var);
    /// @brief Convenience shortcut for <tt>add(var, false)</tt>.
    void addAtemporal(const DiscreteVariable& var);

    /// @brief Convenience shortcut for <tt>add(fast_description, true, default_nbrmod)</tt>.
    void addTemporal(std::string_view fast_description, unsigned int default_nbrmod = 2);
    /// @brief Convenience shortcut for <tt>add(fast_description, false, default_nbrmod)</tt>.
    void addAtemporal(std::string_view fast_description, unsigned int default_nbrmod = 2);

    /**
     * @brief Removes a variable and all its incident arcs.
     *
     * The kind (temporal or atemporal) is determined automatically from the
     * registered name sets. For a temporal process all @f$k@f$ slice nodes are
     * removed.
     * @throw NotFound if no variable with that name exists.
     */
    void erase(std::string_view base);

    /**
     * @brief Renames a variable (temporal process or atemporal variable).
     *
     * The kind is determined automatically from the registered name sets.
     * For a temporal process all @f$k@f$ slice names are re-encoded with the
     * new base.
     * @throw NotFound if no variable named @c oldBase exists.
     * @throw DuplicateLabel if a variable named @c newBase already exists (any kind).
     * @throw InvalidArgument if @c newBase is empty or would create a BN node-name collision.
     */
    void changeVariableName(std::string_view oldBase, std::string_view newBase);

    /// @}
    // ===========================================================================
    /// @name Variable queries
    // ===========================================================================
    /// @{

    /// @return @c true if a variable with this base name exists.
    bool exists(std::string_view base) const;

    /// @return The set of temporal process base names.
    const std::unordered_set< std::string >& temporalVarNames() const;

    /// @return The set of atemporal variable base names.
    const std::unordered_set< std::string >& atemporalVarNames() const;

    /// @return The number of temporal processes.
    Size nbTemporalVars() const;

    /// @return The number of atemporal variables.
    Size nbAtemporalVars() const;

    /// @return All nodes as (base, slice) pairs (atemporal nodes use KTBN::ATEMPORAL).
    std::vector< std::pair< std::string, int > > nodes() const;

    /// @brief Parents of a node as (base, slice) pairs (@c ATEMPORAL if atemporal).
    std::vector< std::pair< std::string, int > > parents(std::string_view base, int slice) const;
    /// @brief Returns the parents using an engine name (@c "X[1]", @c "C", …).
    std::vector< std::pair< std::string, int > > parents(std::string_view node_name) const;

    /// @brief Children of a node as (base, slice) pairs (@c ATEMPORAL if atemporal).
    std::vector< std::pair< std::string, int > > children(std::string_view base, int slice) const;
    /// @brief Returns the children using an engine name (@c "X[1]", @c "C", …).
    std::vector< std::pair< std::string, int > > children(std::string_view node_name) const;

    /**
     * @brief Returns the gum::DiscreteVariable of a (process, slice) couple.
     *
     * Use @c KTBN::ATEMPORAL as slice for atemporal variables.
     * @throw NotFound if no such variable exists.
     * @throw OutOfBounds if @c slice is out of @f$[0,k-1]@f$ for a temporal variable.
     * @throw OperationNotAllowed if the temporal/atemporal kind does not match @c slice.
     */
    const DiscreteVariable& variable(std::string_view base, int slice) const;

    /// @brief Same, using an engine name: resolved via @c _determineNode_, so
    ///        @c "X[1]" and bare @c "C" are both accepted.
    const DiscreteVariable& variable(std::string_view node_name) const;

    /// @brief The time slice of @p var, or @c ATEMPORAL if it is atemporal.
    /// @throw NotFound if @p var is not a node of this k-DBN.
    int timeSlice(const DiscreteVariable& var) const;

    /// @brief Returns the base name (without bracket encoding) of @p var.
    /// @throw NotFound if @p var is not a node of this k-DBN.
    std::string baseName(const DiscreteVariable& var) const;

    /// @}
    // ===========================================================================
    /// @name Arc management
    // ===========================================================================
    /// @{

    /**
     * @brief Adds an arc between two (process, slice) endpoints.
     *
     * @param tailBase Base name of the tail variable.
     * @param tailSlice Slice of the tail (KTBN::ATEMPORAL for an atemporal tail).
     * @param headBase Base name of the head variable.
     * @param headSlice Slice of the head (KTBN::ATEMPORAL for an atemporal head).
     * @throw NotFound if an endpoint does not exist.
     * @throw OutOfBounds if a slice argument is out of @f$[0,k-1]@f$ for a
     * temporal endpoint.
     * @throw OperationNotAllowed if the arc violates temporal causality (a
     * temporal variable pointing to an atemporal one, or an arc from a future
     * slice to a past slice).
     * @throw DuplicateElement if the arc already exists.
     * @throw InvalidDirectedCycle if the arc would create a cycle.
     */
    void addArc(std::string_view tailBase, int tailSlice, std::string_view headBase, int headSlice);

    /// Adds an arc using internal engine names (bracket notation: @c "X[0]", @c "C", …).
    /// @see fillCPT(node_name, …) for the same naming convention.
    /// @throw NotFound if an endpoint name is not in the template.
    /// @throw OutOfBounds / OperationNotAllowed / DuplicateElement / InvalidDirectedCycle —
    ///        same conditions as the (base, slice) overload.
    void addArc(std::string_view tail, std::string_view head);

    /**
     * @brief Removes an arc between two (process, slice) endpoints.
     *
     * The arc must exist: erasing an absent arc throws (via the underlying
     * CPT update), even when both endpoints are valid variables.
     * @throw NotFound / OperationNotAllowed / OutOfBounds if an endpoint
     *        variable does not exist (same rules as addArc()), or if the arc
     *        itself does not exist.
     */
    void eraseArc(std::string_view tailBase,
                  int              tailSlice,
                  std::string_view headBase,
                  int              headSlice);

    /// Removes an arc using internal engine names (bracket notation).
    /// @throw NotFound if an endpoint name is unknown or the arc does not exist.
    void eraseArc(std::string_view tail, std::string_view head);

    /// @return @c true if the arc exists in the template.
    bool existsArc(std::string_view tailBase,
                   int               tailSlice,
                   std::string_view  headBase,
                   int               headSlice) const;

    /// @return @c true if the arc exists; uses internal engine names (bracket notation).
    /// @throw NotFound if an endpoint name is unknown.
    bool existsArc(std::string_view tail, std::string_view head) const;

    /// @return All arcs as (tail, head) pairs of (base, slice).
    std::vector< std::pair< std::pair< std::string, int >, std::pair< std::string, int > > >
        arcs() const;

    /// @}
    // ===========================================================================
    /// @name Conditional probability tables
    // ===========================================================================
    /// @{

    /**
     * @brief Returns the CPT of a (process, slice) couple.
     *
     * The returned reference is const but its content is mutable — use any
     * standard gum::Tensor method to fill it:
     * @code
     *   m.cpt("C").fillWith({0.4, 0.6});
     *   m.cpt("X", 2).fillWith({0.8,0.2, 0.3,0.7, 0.6,0.4, 0.1,0.9});
     *   m.cpt("Y", 1).fillWith(GUM_SCALAR(0.5));  // uniform
     * @endcode
     * Values for the vector overload are in the CPT's natural iteration order:
     * the node's own variable varies fastest, parents follow in the order they
     * appear in cpt().variable(1..n).  Use cpt().variable(i).name() to inspect
     * the ordering before filling.
     */
    const Tensor< GUM_SCALAR >& cpt(std::string_view base, int slice) const;
    /// @brief Returns the CPT using an engine name (@c "X[1]", @c "C", …).
    const Tensor< GUM_SCALAR >& cpt(std::string_view node_name) const;

    /**
     * @brief Fills one conditional distribution P(node | parent configuration).
     *
     * The order-safe, bracket-free way to fill a CPT: the node and its parents
     * are addressed by their (base, slice) identity, so the result does not
     * depend on the positional order of cpt().fillWith({...}).
     *
     * Each parent is a dictionary entry keyed by its (base, slice) identity
     * (use ATEMPORAL as @c slice for a static parent). ALL parents of the node
     * must be listed, in any order. @p distribution holds the probabilities
     * over the node's own modalities for that parent configuration.
     *
     * A parent's value is either a modality index or a modality label, and the
     * two may be mixed — see gum::KTBNModality, whose @b warning on numeric
     * modalities applies here.
     *
     * @code
     *   m.fillCPT("C", KTBN::ATEMPORAL, {}, {0.4, 0.6});      // P(C), no parents
     *   m.fillCPT("X", 0, {}, {0.7, 0.3});                    // P(X[0]), no parents
     *   m.fillCPT("X", 1, {{{"X", 0}, 1}, {{"C", KTBN::ATEMPORAL}, 0}},
     *             {0.6, 0.4});                                // P(X[1] | X[0]=1, C=0)
     *   m.fillCPT("X", 1, {{{"X", 0}, "1"}, {{"C", KTBN::ATEMPORAL}, 0}},
     *             {0.6, 0.4});                                // same, X[0] by label
     * @endcode
     *
     * @param base         Base name of the target node.
     * @param slice        Slice of the target node (ATEMPORAL for static).
     * @param parents      One (base, slice) -> value entry per parent, any order.
     * @param distribution Probabilities over the node's modalities; its size
     *                     must equal the node's domain size.
     * @throw NotFound / OutOfBounds if the node or a parent does not exist, if a
     *        parent index is out of range, or if a parent label is not one of
     *        that parent's modalities (which of the two is raised for an unknown
     *        label depends on the variable type).
     * @throw SizeError if @p distribution size differs from the node domain
     *        size, or if not every parent is specified.
     * @throw InvalidArgument if an entry's node is not a parent of the target.
     */
    void fillCPT(std::string_view                                               base,
                 int                                                            slice,
                 const std::map< std::pair< std::string, int >, KTBNModality >& parents,
                 const std::vector< GUM_SCALAR >&                               distribution) const;

    /**
     * @brief Fills one conditional distribution using internal bracket-notation
     * engine name for the target node — the bracket notation visible in
     * toDot() and toString(): <tt>"X[t]"</tt> for a temporal variable at
     * slice @c t, and the bare variable name for an atemporal variable
     * (e.g. <tt>"C"</tt>). Each parent is keyed either by its engine name
     * (@c "X[0]") or by its (base, slice) identity — both spellings may be
     * mixed in the same dictionary.
     *
     * @warning An engine-name key can be written as a bare string literal, but
     * a (base, slice) key needs an explicit @c std::pair{...}: @c std::variant's
     * converting constructor takes one value convertible to an alternative, not
     * a nested brace list, so a bare @c {"X", 0} cannot implicitly construct the
     * pair alternative the way it would a @c std::pair or @c std::tuple element.
     *
     * As in the (base, slice) overload, a parent's value is either a modality
     * index or a modality label — see gum::KTBNModality.
     *
     * @code
     *   m.fillCPT("X[1]", {{"X[0]", 1}, {"C", 0}}, {0.6, 0.4});
     *   m.fillCPT("X[1]", {{std::pair{"X", 0}, 1}, {std::pair{"C", KTBN::ATEMPORAL}, 0}},
     *             {0.6, 0.4});
     *   m.fillCPT("X[1]", {{"X[0]", 1}, {std::pair{"C", KTBN::ATEMPORAL}, 0}},
     *             {0.6, 0.4});                                // mixed key styles
     *   m.fillCPT("X[1]", {{"X[0]", "1"}, {"C", 0}}, {0.6, 0.4});   // mixed value styles
     * @endcode
     *
     * @param node_name    Internal engine name of the target node.
     * @param parents      One engine_name-or-(base,slice) -> value entry per
     *                     parent, any order.
     * @param distribution Probabilities over the node's modalities; its size
     *                     must equal the node's domain size.
     * @throw NotFound     if @p node_name or a parent does not exist.
     * @throw SizeError / InvalidArgument / OutOfBounds — same conditions as the
     *        @c (base, slice) overload.
     */
    void fillCPT(std::string_view node_name,
                 const std::map< std::variant< std::string, std::pair< std::string, int > >,
                                 KTBNModality >&  parents,
                 const std::vector< GUM_SCALAR >& distribution) const;

    /// @brief Randomly generates every CPT of the template.
    void generateCPTs() const;

    /// @brief Randomly generates the CPT of a single node.
    void generateCPT(std::string_view base, int slice) const;
    /// @brief Same, using an engine name (@c "X[1]", @c "C", …).
    void generateCPT(std::string_view node_name) const;

    /// @}
    // ===========================================================================
    /// @name Transformations
    // ===========================================================================
    /// @{

    /// @return A deep copy of the underlying template as a gum::BayesNet.
    BayesNet< GUM_SCALAR > toBN() const;

    /**
     * @brief Unrolls the k-DBN into a standard gum::BayesNet.
     *
     * The result has exactly @c nbTimeSlices time slices. Slices @f$0, \ldots,
     * k-1@f$ are copied verbatim from the template; every additional slice
     * @f$t \geq k@f$ replicates the incoming pattern of slice @f$k-1@f$ (the
     * transition kernel), shifting each temporal parent's slice accordingly so
     * that lags are preserved. Variables are named with the same
     * <tt>base[slice]</tt> bracket-notation convention (atemporal variables
     * keep their name).
     *
     * @param nbTimeSlices Total number of time slices of the unrolled network.
     * @throw OperationNotAllowed if @c nbTimeSlices is smaller than @f$k@f$.
     */
    BayesNet< GUM_SCALAR > unroll(Size nbTimeSlices) const;

    /// @}
    // ===========================================================================
    /// @name Persistence and conversion
    // ===========================================================================
    /// @{

    /**
     * @brief Saves the template in the GUM format (text <tt>.jgum</tt> or binary
     * <tt>.bgum</tt>).
     *
     * The extension selects the format: <tt>.jgum</tt> writes the text (JSON)
     * variant, anything else the binary (msgpack) one, gaining a <tt>.bgum</tt>
     * extension if missing. The temporal/atemporal classification and the order
     * @f$k@f$ are stored as properties, so load() reconstructs the k-DBN exactly
     * — including k=1 processes and bracket-named atemporal variables, which
     * fromBN() cannot disambiguate from node names alone.
     * @param filename The destination file.
     */
    void save(std::string_view filename) const;

    /**
     * @brief Loads a k-DBN from a GUM file produced by save().
     *
     * Text iff the name ends with <tt>.jgum</tt>, otherwise binary (gaining a
     * <tt>.bgum</tt> extension if missing). If the file carries the KTBN
     * classification properties it is restored exactly; otherwise it is
     * re-derived from node names via fromBN().
     * @param filename The GUM file.
     * @throw IOError if the file cannot be read or is not valid.
     * @throw OperationNotAllowed if the classification properties are absent
     * and the fallback fromBN() finds an inconsistent temporal structure.
     */
    static KTBN< GUM_SCALAR > load(std::string_view filename);

    /**
     * @brief Builds a k-DBN from an existing gum::BayesNet whose node names
     * follow the bracket notation (<tt>base[t]</tt> for temporal, bare name for
     * atemporal).
     *
     * The order @f$k@f$ is inferred as one plus the largest slice index found.
     * Temporal node names are assumed to be @b canonical, i.e. <tt>base[t]</tt>
     * with a plain decimal slice and no leading zeros (the form produced by
     * add() / unroll()).
     * A group of bracket-named nodes that does @b not cover every slice
     * @f$0..k-1@f$ is @b not an error: each of its nodes becomes an atemporal
     * variable, bracket name kept, and a message is appended to @p warnings.
     * When no temporal process survives, @f$k@f$ falls back to 1.
     *
     * @param bn The source Bayesian network (copied).
     * @param atemporalNodes Node names -- exactly as they appear in @p bn,
     *        bracket-suffixed or not -- to classify atemporal outright. Only
     *        needed to lift an ambiguity: a bare name is atemporal anyway, so
     *        listing it changes nothing, and listing a bracket name says
     *        "I meant this" where the reclassification above would only guess
     *        (and warn).
     * @param warnings If non-null, receives one message per reclassified group.
     * @throw NotFound if a name in @p atemporalNodes is not a node of @p bn.
     * @throw OperationNotAllowed if the temporal structure is inconsistent:
     * two variables mapping to the same (process, slice), a base name used
     * both as a bare node and as bracket-named slices, an arc from a temporal
     * node into an atemporal one, or an arc from the future to the past.
     *
     * @warning A lone @c "X[0]" with no other bracket node in @p bn yields
     * @f$k=1@f$, where a single slice is a @e complete process: it is kept
     * temporal, silently. List it in @p atemporalNodes to say otherwise.
     */
    static KTBN< GUM_SCALAR > fromBN(const BayesNet< GUM_SCALAR >&            bn,
                                     const std::unordered_set< std::string >& atemporalNodes = {},
                                     std::vector< std::string >* warnings = nullptr);

    /// @}
    // ===========================================================================
    /// @name Various
    // ===========================================================================
    /// @{

    /// @return A human-readable description of the k-DBN.
    std::string toString() const;

    /// @brief Returns a Graphviz DOT string with one cluster per time slice.
    std::string toDot() const;

    /**
     * @brief Returns a Graphviz DOT string of the k-DBN unrolled over @p T time slices.
     * @param T Total number of time slices to display. Must be @f$\geq k@f$.
     * @param highlightReplicated If true, shade slices @f$\geq k@f$ (lightcyan)
     *        differently from the template slices @f$0, \ldots, k-1@f$ (gray).
     * @throw OperationNotAllowed if @c T < k.
     */
    std::string toUnrolledDot(Size T, bool highlightReplicated = false) const;

    /**
     * @brief Returns the Graphviz DOT string of the underlying storage BayesNet.
     *
     * Nodes are labelled with their internal engine names (bracket notation:
     * <tt>base[t]</tt> for temporal nodes, bare name for atemporal nodes).
     * No time-slice clustering is applied.
     */
    std::string bnToDot() const;

    /// @}

    private:
    /// The order (number of time slices in the template).
    Size _k_;

    /// The underlying Bayesian network used as a storage engine for the template.
    BayesNet< GUM_SCALAR > _bn_;

    /// Base names of the registered temporal processes.
    std::unordered_set< std::string > _temporal_;

    /// Base names of the registered atemporal variables.
    std::unordered_set< std::string > _atemporal_;

    /// @brief Encodes (base, slice) → engine name: <tt>base[t]</tt>, or
    /// <tt>base</tt> if atemporal.
    std::string _encode_(std::string_view base, int slice) const;

    /// @brief Purely syntactic parse of an engine name → (base, slice). Slice is
    /// ATEMPORAL when there is no <tt>[digits]</tt> suffix. Does not consult the
    /// cached name sets, so a bracket-named atemporal node decodes as temporal here.
    std::pair< std::string, int > _decodeName_(std::string_view name) const;

    /// @brief Cache-aware classification of a node name → (base, slice): nodes
    /// registered in @c _atemporal_ (atemporal and orphan-bracket) map to ATEMPORAL,
    /// every other name is parsed by _decodeName_.
    std::pair< std::string, int > _determineNode_(const std::string& name) const;

    /// @brief Maps a set of node ids to (base, slice) pairs (via _determineNode_).
    std::vector< std::pair< std::string, int > > _determineNodeSet_(const NodeSet& ids) const;

    /// @brief Resolves and validates a (base, slice) endpoint into its NodeId.
    NodeId _validateVariable_(std::string_view base, int slice) const;

    /// @brief Checks that a variable named @p base can be added.
    /// @param temporal true for a temporal process, false for an atemporal variable.
    /// @throw DuplicateLabel / InvalidArgument on a name collision (see add()).
    void _validateAdd_(const std::string& base, bool temporal) const;

    /// @brief Rebuilds the cached name sets from the storage engine content
    /// (used by fromBN()/load(); decodes names once).
    void _determineNodesFromBN_(const std::unordered_set< std::string >& atemporalNodes,
                                std::vector< std::string >*              warnings);

    /// @brief Resolves a user filename to <tt>(filepath, binary)</tt>: ensures a
    /// <tt>.jgum</tt>/<tt>.bgum</tt> extension (<tt>.bgum</tt> appended by default)
    /// and reports whether the format is binary. Single source of truth for the
    /// GUM format convention shared by save() and load().
    static std::pair< std::string, bool > _resolveGumFormat_(std::string_view filename);

    /// @brief Renders @p bn as time-slice-clustered DOT. Shared engine behind
    /// toDot() (on @c _bn_) and toUnrolledDot() (on unroll(T)).
    std::string _timeSlicesToDot_(const BayesNet< GUM_SCALAR >& bn, bool highlightReplicated) const;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class KTBN< double >;
#endif

  /// @brief Prints the k-DBN using its toString() description.
  template < GUM_Numeric GUM_SCALAR >
  std::ostream& operator<<(std::ostream& output, const KTBN< GUM_SCALAR >& kdbn);

}   // namespace gum

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/KTBN/KTBN_inl.h>
#endif /* GUM_NO_INLINE */

#include <agrum/KTBN/KTBN_tpl.h>

#endif /* GUM_KTBN_H */
