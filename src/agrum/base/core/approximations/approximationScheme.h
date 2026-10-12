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
 * @brief This file contains general scheme for iteratively convergent
 * algorithms.
 *
 * ApproximationSettings provides as well 2 signals :
 *   - onProgress(int pourcent,double error)
 *   - onStop(const std::string& message)
 * @see gum::ApproximationListener for dedicated listener.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_APPROXIMATION_SCHEME_H
#define GUM_APPROXIMATION_SCHEME_H

#include <agrum/agrum.h>

#include <agrum/base/core/approximations/IApproximationSchemeConfiguration.h>

namespace gum {

  /**
   * @class ApproximationScheme
   * @brief Approximation Scheme.
   * @ingroup approximationscheme_group
   *
   * @warning Doxygen does not like spanning command on multiple line, so we
   * could not configure it with the correct include directive. Use the
   * following code snippet to include this file.
   * @code
   * #include <agrum/base/core/approximations/approximationScheme.h>
   * @endcode
   *
   * The approximation scheme is assumed to be used like this:
   * @code
   * initApproximationScheme();
   *
   * // this loop can stop with ApproximationSchemeSTATE::Epsilon,
   * // ApproximationSchemeSTATE::Rate and ApproximationSchemeSTATE::Limit
   * do {
   * // compute new values and a GUM_SCALAR error representing the progress
   * // in this step.
   *   updateApproximationScheme();
   *   // compute state of the approximation
   *   if (startOfPeriod()) {
   *      // w.r.t to the state of approximation compute epsilon
   *   }
   * } while ( continueApproximationScheme( epsilon ));
   * // end of loop
   *
   * if ( verbosity() ) {
   *   switch ( stateApproximationScheme() ) {
   *     case ApproximationSchemeSTATE::Continue:  // should not be possible
   *       break;
   *     case ApproximationSchemeSTATE::Epsilon: {
   *         GUM_TRACE( "stop with
   *             epsilon="<<epsilon() );
   *         break;
   *     }
   *     case ApproximationSchemeSTATE::Rate: {
   *         GUM_TRACE( "stop with
   *             rate="<<minEpsilonRate() );
   *         break;
   *     }
   *     case ApproximationSchemeSTATE::Limit: {
   *         GUM_TRACE( "stop with max
   *             iteration="<<maxIter() );
   *         break;
   *     }
   *     case ApproximationSchemeSTATE::TimeLimit: {
   *         GUM_TRACE( "stop with
   *             timemout="<<currentTime() );
   *         break;
   *     }
   *   }
   * }
   * // equivalent to
   * if (verbosity()) {
   *   GUM_TRACE(messageApproximationScheme());
   * }
   * @endcode
   */
  class GUM_SHARED_PUBLIC ApproximationScheme: public IApproximationSchemeConfiguration {
    public:
    // ========================================================================
    /// @name Constructors and Destructors
    // ========================================================================
    /// @{

    /**
     * @brief Build a scheme whose parameters are read from the configuration.
     *
     * The section `approximation` of gum::Configuration provides the epsilon, the minimal epsilon
     * rate, the maximum number of iterations, the maximum time (disabled by default), the burn-in,
     * the period size and the verbosity.
     */
    ApproximationScheme();

    /**
     * @brief Build a scheme whose parameters are read from the configuration, except the verbosity.
     * @param verbosity If true, the history of the errors is recorded.
     */
    explicit ApproximationScheme(bool verbosity);

    ~ApproximationScheme() override;

    /// @}
    // ========================================================================
    /// @name Getters and setters
    // ========================================================================
    /// @{

    /**
     * @brief Given that we approximate f(t), stopping criterion on
     * |f(t+1)-f(t)|.
     *
     * If the criterion was disabled it will be enabled.
     *
     * @param eps The new epsilon value.
     * @throw OutOfBounds Raised if eps < 0.
     */
    void setEpsilon(double eps) override;

    /**
     * @brief Returns the value of epsilon.
     * @return Returns the value of epsilon.
     */
    double epsilon() const override;

    /**
     * @brief Disable stopping criterion on epsilon.
     */
    void disableEpsilon() override;

    /**
     * @brief Enable stopping criterion on epsilon.
     */
    void enableEpsilon() override;

    /**
     * @brief Returns true if stopping criterion on epsilon is enabled, false
     * otherwise.
     * @return Returns true if stopping criterion on epsilon is enabled, false
     * otherwise.
     */
    bool isEnabledEpsilon() const override;

    /**
     * @brief Given that we approximate f(t), stopping criterion on
     * d/dt(|f(t+1)-f(t)|).
     *
     * If the criterion was disabled it will be enabled
     *
     * @param rate The minimal epsilon rate.
     * @throw OutOfBounds if rate<0
     */
    void setMinEpsilonRate(double rate) override;

    /**
     * @brief Returns the value of the minimal epsilon rate.
     * @return Returns the value of the minimal epsilon rate.
     */
    double minEpsilonRate() const override;

    /**
     * @brief Disable stopping criterion on epsilon rate.
     */
    void disableMinEpsilonRate() override;

    /**
     * @brief Enable stopping criterion on epsilon rate.
     */
    void enableMinEpsilonRate() override;

    /**
     * @brief Returns true if stopping criterion on epsilon rate is enabled,
     * false otherwise.
     * @return Returns true if stopping criterion on epsilon rate is enabled,
     * false otherwise.
     */
    bool isEnabledMinEpsilonRate() const override;

    /**
     * @brief Stopping criterion on number of iterations.
     *
     * If the criterion was disabled it will be enabled.
     *
     * @param max The maximum number of iterations.
     * @throw OutOfBounds Raised if max <= 1.
     */
    void setMaxIter(Size max) override;

    /**
     * @brief Returns the criterion on number of iterations.
     * @return Returns the criterion on number of iterations.
     */
    Size maxIter() const override;

    /**
     * @brief Disable stopping criterion on max iterations.
     */
    void disableMaxIter() override;

    /**
     * @brief Enable stopping criterion on max iterations.
     */
    void enableMaxIter() override;

    /**
     * @brief Returns true if stopping criterion on max iterations is enabled,
     * false otherwise.
     * @return Returns true if stopping criterion on max iterations is enabled,
     * false otherwise.
     */
    bool isEnabledMaxIter() const override;

    /**
     * @brief Stopping criterion on timeout.
     *
     * If the criterion was disabled it will be enabled.
     *
     * @param timeout The timeout value in seconds.
     * @throw OutOfBounds Raised if timeout <= 0.0.
     */
    void setMaxTime(double timeout) override;

    /**
     * @brief Returns the timeout (in seconds).
     * @return Returns the timeout (in seconds).
     */
    double maxTime() const override;

    /**
     * @brief Returns the current running time in second.
     * @return Returns the current running time in second.
     */
    double currentTime() const override;

    /**
     * @brief Disable stopping criterion on timeout.
     * @return Disable stopping criterion on timeout.
     */
    void disableMaxTime() override;

    /**
     * @brief Enable stopping criterion on timeout.
     */
    void enableMaxTime() override;

    /**
     * @brief Returns true if stopping criterion on timeout is enabled, false
     * otherwise.
     * @return Returns true if stopping criterion on timeout is enabled, false
     * otherwise.
     */
    bool isEnabledMaxTime() const override;

    /**
     * @brief How many samples between two stopping is enable.
     * @param p The new period value.
     * @throw OutOfBounds Raised if p < 1.
     */
    void setPeriodSize(Size p) override;

    /**
     * @brief Returns the period size.
     * @return Returns the period size.
     */
    Size periodSize() const override;

    /**
     * @brief Set the verbosity on (true) or off (false).
     * @param v If true, then verbosity is turned on.
     */
    void setVerbosity(bool v) override;

    /**
     * @brief Returns true if verbosity is enabled.
     * @return Returns true if verbosity is enabled.
     */
    bool verbosity() const override;

    /**
     * @brief Returns the approximation scheme state.
     * @return Returns the approximation scheme state.
     */
    ApproximationSchemeSTATE stateApproximationScheme() const override;

    /**
     * @brief Returns the number of iterations.
     * @return Returns the number of iterations.
     * @throw OperationNotAllowed Raised if the scheme did not perform.
     */
    Size nbrIterations() const override;

    /**
     * @brief Returns the scheme history.
     * @return Returns the scheme history.
     * @throw OperationNotAllowed Raised if the scheme did not performed or
     * if verbosity is set to false.
     */
    const std::vector< double >& history() const override;

    /**
     * @brief Initialise the scheme.
     */
    void initApproximationScheme();

    /**
     * @brief Returns true if we are at the beginning of a period (compute
     * error is mandatory).
     * @return Returns true if we are at the beginning of a period (compute
     * error is mandatory).
     */
    bool startOfPeriod() const;

    /**
     * @brief Update the scheme w.r.t the new error and increment steps.
     * @param incr The new increment steps.
     */
    void updateApproximationScheme(unsigned int incr = 1);

    /**
     * @brief Returns the remaining burn in.
     * @return Returns the remaining burn in.
     */
    Size remainingBurnIn() const;

    /**
     * @brief Stop the approximation scheme.
     */
    void stopApproximationScheme();

    /**
     * @brief Update the scheme w.r.t the new error.
     *
     * Test the stopping criterion that are enabled.
     *
     * @param error The new error value.
     * @return false if state become != ApproximationSchemeSTATE::Continue
     * @throw OperationNotAllowed Raised if state !=
     * ApproximationSchemeSTATE::Continue.
     */
    bool continueApproximationScheme(double error);


    ///  @}

    private:
    /**
     * @brief Stop the scheme given a new state.
     * @param new_state The scheme new state.
     */
    void stopScheme_(ApproximationSchemeSTATE new_state);

    protected:
    /**
     * @brief Set the parameters of the scheme from a section of gum::Configuration.
     *
     * The keys `epsilon`, `minEpsilonRate`, `maxIter` and `periodSize` of the section are read
     * (and `maxTime` if withMaxTime is true, `verbosity` if withVerbosity is true, `burnIn` if the
     * section has this key) and applied
     * through the corresponding setters, which enable the matching stopping criteria and check
     * the values.
     *
     * @param section The name of the section of the configuration.
     * @param withMaxTime If true, the key `maxTime` is read and the timeout is enabled.
     * @param withVerbosity If true, the key `verbosity` is read ; otherwise the verbosity is left
     * unchanged.
     * @throw OutOfBounds Raised if a value of the section is not valid for its setter.
     */
    void setFromConfiguration_(const std::string& section,
                               bool               withMaxTime,
                               bool               withVerbosity = true);

    /**
     * @brief Read a non negative integer key of gum::Configuration.
     * @param section The name of the section of the configuration.
     * @param key The name of the key of the section.
     * @return The value of the key.
     * @throw OutOfBounds Raised if the value is negative.
     */
    static Size sizeFromConfiguration_(const std::string& section, const std::string& key);

    /// Current epsilon.
    double current_epsilon_{-1.};

    /// Last epsilon value.
    double last_epsilon_{-1.};

    /// Current rate.
    double current_rate_{-1.};

    /// The current step.
    Size current_step_{0};

    /// The timer.
    Timer timer_;

    /// The current state.
    ApproximationSchemeSTATE current_state_;

    /// The scheme history, used only if verbosity == true.
    std::vector< double > history_;

    /// Threshold for convergence.
    double eps_;

    /// If true, the threshold convergence is enabled.
    bool enabled_eps_;

    /// Threshold for the epsilon rate.
    double min_rate_eps_;

    /// If true, the minimal threshold for epsilon rate is enabled.
    bool enabled_min_rate_eps_;

    /// The timeout.
    double max_time_;

    /// If true, the timeout is enabled.
    bool enabled_max_time_;

    /// The maximum iterations.
    Size max_iter_;

    /// If true, the maximum iterations stopping criterion is enabled.
    bool enabled_max_iter_;

    /// Number of iterations before checking stopping criteria.
    Size burn_in_;

    /// Checking criteria frequency.
    Size period_size_;

    /// If true, verbosity is enabled.
    bool verbosity_;
  };
}   // namespace gum

#ifndef GUM_NO_INLINE
#  include <agrum/base/core/approximations/approximationScheme_inl.h>
#endif

#endif   // GUM_APPROXIMATION_SCHEME_H
