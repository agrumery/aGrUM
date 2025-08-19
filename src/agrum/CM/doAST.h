/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#ifndef GUM_DO_AST_H
#define GUM_DO_AST_H


#include <string>
#include <memory>
#include <set>
#include <map>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/BN/BayesNet.h>


namespace gum {

template <typename GUM_SCALAR> class BayesNet;
template <typename GUM_SCALAR> class Tensor;

// ================================================================
// ASTtree
// ================================================================
template <typename GUM_SCALAR>
class ASTtree {
public:
  explicit ASTtree(const std::string& type);
  virtual ~ASTtree() = default;

  ASTtree(const ASTtree&) = delete;
  ASTtree& operator=(const ASTtree&) = delete;
  ASTtree(ASTtree&&) noexcept = default;
  ASTtree& operator=(ASTtree&&) noexcept = default;

  [[nodiscard]] const std::string& type() const noexcept { return _type; }

  [[nodiscard]] virtual std::string toString(const std::string& prefix = "") const = 0;
  virtual std::string protectToLatex(HashTable<std::string,int>& nameOccur) const = 0;
  virtual std::string fastToLatex(HashTable<std::string,int>& nameOccur) const = 0;

  [[nodiscard]] std::string toLatex(HashTable<std::string,int> nameOccur = HashTable<std::string,int>()) const;

  [[nodiscard]] virtual std::unique_ptr<ASTtree<GUM_SCALAR>> copy() const = 0;
  virtual Tensor<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& contextual_bn) const = 0;

protected:
  static constexpr const char* CONTINUE_PREFIX = "| ";
  std::string _type;

  static std::string _latexCorrect(const std::string& srcName,
                                   HashTable<std::string,int>& nameOccur);
  static List<std::string> _latexCorrect(const Set<std::string>& srcNames,
                                                HashTable<std::string,int>& nameOccur);
};

// ================================================================
// ASTBinaryOp
// ================================================================
template <typename GUM_SCALAR>
class ASTBinaryOp : public ASTtree<GUM_SCALAR> {
public:
  ASTBinaryOp(const std::string& typ,
              std::unique_ptr<ASTtree<GUM_SCALAR>> op1,
              std::unique_ptr<ASTtree<GUM_SCALAR>> op2);

  ASTBinaryOp(const ASTBinaryOp&) = delete;
  ASTBinaryOp& operator=(const ASTBinaryOp&) = delete;
  ASTBinaryOp(ASTBinaryOp&&) noexcept = default;
  ASTBinaryOp& operator=(ASTBinaryOp&&) noexcept = default;

  [[nodiscard]] const ASTtree<GUM_SCALAR>& op1() const { return *_op1; }
  [[nodiscard]] const ASTtree<GUM_SCALAR>& op2() const { return *_op2; }

  [[nodiscard]] std::string toString(const std::string& prefix = "") const override;

protected:
  std::unique_ptr<ASTtree<GUM_SCALAR>> _op1, _op2;
};


// ================================================================
// ASTplus (ASTBinaryOp)
// ================================================================
template <typename GUM_SCALAR>
class ASTplus : public ASTBinaryOp<GUM_SCALAR> {
public:
  ASTplus(std::unique_ptr<ASTtree<GUM_SCALAR>> op1,
          std::unique_ptr<ASTtree<GUM_SCALAR>> op2);

  std::string protectToLatex(HashTable<std::string,int>& nameOccur) const override;
  std::string fastToLatex(HashTable<std::string,int>& nameOccur) const override;
  std::unique_ptr<ASTtree<GUM_SCALAR>> copy() const override;
  Tensor<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& contextual_bn) const override;
};


// ================================================================
// ASTminus (ASTBinaryOp)
// ================================================================
template <typename GUM_SCALAR>
class ASTminus : public ASTBinaryOp<GUM_SCALAR> {
public:
  ASTminus(std::unique_ptr<ASTtree<GUM_SCALAR>> op1,
           std::unique_ptr<ASTtree<GUM_SCALAR>> op2);

  std::string protectToLatex(HashTable<std::string,int>& nameOccur) const override;
  std::string fastToLatex(HashTable<std::string,int>& nameOccur) const override;
  std::unique_ptr<ASTtree<GUM_SCALAR>> copy() const override;
  Tensor<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& contextual_bn) const override;
};


// ================================================================
// ASTmult (ASTBinaryOp)
// ================================================================
template <typename GUM_SCALAR>
class ASTmult : public ASTBinaryOp<GUM_SCALAR> {
public:
  ASTmult(std::unique_ptr<ASTtree<GUM_SCALAR>> op1,
          std::unique_ptr<ASTtree<GUM_SCALAR>> op2);

  std::string protectToLatex(HashTable<std::string,int>& nameOccur) const override;
  std::string fastToLatex(HashTable<std::string,int>& nameOccur) const override;
  std::unique_ptr<ASTtree<GUM_SCALAR>> copy() const override;
  Tensor<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& contextual_bn) const override;
};


// ================================================================
// ASTdiv (ASTBinaryOp)
// ================================================================
template <typename GUM_SCALAR>
class ASTdiv : public ASTBinaryOp<GUM_SCALAR> {
public:
  ASTdiv(std::unique_ptr<ASTtree<GUM_SCALAR>> op1,
         std::unique_ptr<ASTtree<GUM_SCALAR>> op2);

  std::string protectToLatex(HashTable<std::string,int>& nameOccur) const override;
  std::string fastToLatex(HashTable<std::string,int>& nameOccur) const override;
  std::unique_ptr<ASTtree<GUM_SCALAR>> copy() const override;
  Tensor<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& contextual_bn) const override;
};



// ================================================================
// ASTposteriorProba   :  P_bn(vars | knw_min)
// ================================================================
template <typename GUM_SCALAR>
class ASTposteriorProba : public ASTtree<GUM_SCALAR> {
public:
  // Constructor for P_bn(vars | knw), knw minimized using bn
  explicit ASTposteriorProba(const BayesNet<GUM_SCALAR>& bn,
                             const Set<std::string>& vars,
                             const Set<std::string>& knw);

  // Constructor for P_bn(vars | knw), knw already minimalized
  explicit ASTposteriorProba(const Set<std::string>& vars,
                             const Set<std::string>& knw);


  // Accessors
  const Set<std::string>& vars() const noexcept { return _vars; }
  const Set<std::string>& knw()  const noexcept { return _knw; }

  // AST interface
  std::string toString(const std::string& prefix = "") const override;
  std::string protectToLatex(HashTable<std::string,int>& nameOccur) const override;
  std::string fastToLatex(HashTable<std::string,int>& nameOccur) const override;

  std::unique_ptr<ASTtree<GUM_SCALAR>> copy() const override;
  Tensor<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& contextual_bn) const override;

private:
  Set<std::string> _vars;  // names of conditioned variables
  Set<std::string> _knw;   // names of conditioning variables (already minimalized)
};


// ================================================================
// ASTjointProba :  P(vars) in observational BN
// ================================================================
template <typename GUM_SCALAR>
class ASTjointProba : public ASTtree<GUM_SCALAR> {
public:
  explicit ASTjointProba(const Set<std::string>& varNames);

  [[nodiscard]] const Set<std::string>& varNames() const noexcept { return _varNames; }

  [[nodiscard]] std::string toString(const std::string& prefix = "") const override;
  std::string protectToLatex(HashTable<std::string,int>& nameOccur) const override;
  std::string fastToLatex(HashTable<std::string,int>& nameOccur) const override;

  [[nodiscard]] std::unique_ptr<ASTtree<GUM_SCALAR>> copy() const override;
  Tensor<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& contextual_bn) const override;

private:
  Set<std::string> _varNames;
};

// ================================================================
// ASTsum  :  sum out over variable of sub-term
// ================================================================
template <typename GUM_SCALAR>
class ASTsum : public ASTtree<GUM_SCALAR> {
public:
  ASTsum(const std::string& var, std::unique_ptr<ASTtree<GUM_SCALAR>> term);

  // multi-variable overload: recursively builds Σ over vars[0], vars[1], ...
  ASTsum(const List<std::string>& vars,
         std::unique_ptr<ASTtree<GUM_SCALAR>> term);

  [[nodiscard]] const ASTtree<GUM_SCALAR>& term() const { return *_term; }

  [[nodiscard]] std::string toString(const std::string& prefix = "") const override;
  std::string protectToLatex(HashTable<std::string,int>& nameOccur) const override;
  std::string fastToLatex(HashTable<std::string,int>& nameOccur) const override;

  [[nodiscard]] std::unique_ptr<ASTtree<GUM_SCALAR>> copy() const override;
  Tensor<GUM_SCALAR> eval(const BayesNet<GUM_SCALAR>& contextual_bn) const override;

private:
  std::string _var;
  std::unique_ptr<ASTtree<GUM_SCALAR>> _term;
};


// ================================================================
// productOfTrees  (utility function)
// ================================================================
template <typename GUM_SCALAR>
[[nodiscard]] std::unique_ptr<ASTtree<GUM_SCALAR>>
productOfTrees(List<std::unique_ptr<ASTtree<GUM_SCALAR>>>&& lterms);


// ================================================================
// Extern template declarations
// ================================================================
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class ASTtree<double>;
  extern template class ASTplus<double>;
  extern template class ASTminus<double>;
  extern template class ASTmult<double>;
  extern template class ASTdiv<double>;
  extern template class ASTposteriorProba<double>;
  extern template class ASTjointProba<double>;
  extern template class ASTsum<double>;
  extern template std::unique_ptr<ASTtree<double>>
  productOfTrees<double>(List<std::unique_ptr<ASTtree<double>>>&&);
#endif

} // namespace gum

#include <agrum/CM/doAST_tpl.h>


#endif // GUM_DO_AST_H