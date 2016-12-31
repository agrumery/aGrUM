/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
 * @file
 * @brief This file contains general scheme for iteratively convergent
 * algorithms.
 *
 * ApproximationSettings provides as well 2 signals :
 *   - onProgress(int pourcent,double error)
 *   - onStop(std::string message)
 * @see gum::ApproximationListener for dedicated listener.
 *
 * @author Pierre-Henri WUILLEMIN
 */
#ifndef GUM_APPROXIMATION_SCHEME_H
#define GUM_APPROXIMATION_SCHEME_H

#include <cmath>

#include <agrum/config.h>
#include <agrum/core/approximations/IApproximationSchemeConfiguration.h>
#include <agrum/core/signal/signaler.h>
#include <agrum/core/timer.h>

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
   * #include <agrum/core/approximations/approximationScheme.h>
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
  class ApproximationScheme : public IApproximationSchemeConfiguration {
    public:
    // ========================================================================
    /// @name Constructors and Destructors
    // ========================================================================
    /// @{

    ApproximationScheme( bool v = false );

    virtual ~ApproximationScheme();

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
     * @throw OutOfLowerBound Raised if eps < 0.
     */
    void setEpsilon( double eps );

    /**
     * @brief Returns the value of epsilon.
     * @return Returns the value of epsilon.
     */
    double epsilon( void ) const;

    /**
     * @brief Disable stopping criterion on epsilon.
     */
    void disableEpsilon();

    /**
     * @brief Enable stopping criterion on epsilon.
     */
    void enableEpsilon();

    /**
     * @brief Returns true if stopping criterion on epsilon is enabled, false
     * otherwise.
     * @return Returns true if stopping criterion on epsilon is enabled, false
     * otherwise.
     */
    bool isEnabledEpsilon() const;

    /**
     * @brief Given that we approximate f(t), stopping criterion on
     * d/dt(|f(t+1)-f(t)|).
     *
     * If the criterion was disabled it will be enabled
     *
     * @param rate The minimal epsilon rate.
     * @throw OutOfLowerBound if rate<0
     */
    void setMinEpsilonRate( double rate );

    /**
     * @brief Returns the value of the minimal epsilon rate.
     * @return Returns the value of the minimal epsilon rate.
     */
    double minEpsilonRate( void ) const;

    /**
     * @brief Disable stopping criterion on epsilon rate.
     */
    void disableMinEpsilonRate();

    /**
     * @brief Enable stopping criterion on epsilon rate.
     */
    void enableMinEpsilonRate();

    /**
     * @brief Returns true if stopping criterion on epsilon rate is enabled,
     * false otherwise.
     * @return Returns true if stopping criterion on epsilon rate is enabled,
     * false otherwise.
     */
    bool isEnabledMinEpsilonRate() const;

    /**
     * @brief Stopping criterion on number of iterations.
     *
     * If the criterion was disabled it will be enabled.
     *
     * @param max The maximum number of iterations.
     * @throw OutOfLowerBound Raised if max <= 1.
     */
    void setMaxIter( Size max );

    /**
     * @brief Returns the criterion on number of iterations.
     * @return Returns the criterion on number of iterations.
     */
    Size maxIter( void ) const;

    /**
     * @brief Disable stopping criterion on max iterations.
     */
    void disableMaxIter();

    /**
     * @brief Enable stopping criterion on max iterations.
     */
    void enableMaxIter();

    /**
     * @brief Returns true if stopping criterion on max iterations is enabled,
     * false otherwise.
     * @return Returns true if stopping criterion on max iterations is enabled,
     * false otherwise.
     */
    bool isEnabledMaxIter() const;

    /**
     * @brief Stopping criterion on timeout.
     *
     * If the criterion was disabled it will be enabled.
     *
     * @param timeout The timeout value in seconds.
     * @throw OutOfLowerBound Raised if timeout <= 0.0.
     */
    void setMaxTime( double timeout );

    /**
     * @brief Returns the timeout (in seconds).
     * @return Returns the timeout (in seconds).
     */
    double maxTime( void ) const;

    /**
     * @brief Returns the current running time in second.
     * @return Returns the current running time in second.
     */
    double currentTime( void ) const;

    /**
     * @brief Disable stopping criterion on timeout.
     * @return Disable stopping criterion on timeout.
     */
    void disableMaxTime();

    /**
     * @brief Enable stopping criterion on timeout.
     */
    void enableMaxTime();

    /**
    * @brief Returns true if stopping criterion on timeout is enabled, false
    * otherwise.
    * @return Returns true if stopping criterion on timeout is enabled, false
    * otherwise.
    */
    bool isEnabledMaxTime() const;

    /**
     * @brief How many samples between two stopping is enable.
     * @param p The new period value.
     * @throw OutOfLowerBound Raised if p < 1.
     */
    void setPeriodSize( Size p );

    /**
     * @brief Returns the period size.
     * @return Returns the period size.
     */
    Size periodSize( void ) const;

    /**
     * @brief Number of burn in for one iteration.
     * @param b The number of burn in.
     * @throw OutOfLowerBound Raised if b < 1.
     */
    void setBurnIn( Size b );

    /**
     * @brief Returns the number of burn in.
     * @return Returns the number of burn in.
     */
    Size burnIn( void ) const;

    /**
     * @brief Set the verbosity on (true) or off (false).
     * @param v If true, then verbosity is turned on.
     */
    void setVerbosity( bool v );

    /**
     * @brief Returns true if verbosity is enabled.
     * @return Returns true if verbosity is enabled.
     */
    bool verbosity( void ) const;

    /**
     * @brief Returns the approximation scheme state.
     * @return Returns the approximation scheme state.
     */
    ApproximationSchemeSTATE stateApproximationScheme() const;

    /**
     * @brief Returns the number of iterations.
     * @return Returns the number of iterations.
     * @throw OperationNotAllowed Raised if the scheme did not perform.
     */
    Size nbrIterations() const;

    /**
     * @brief Returns the scheme history.
     * @return Returns the scheme history.
     * @throw OperationNotAllowed Raised if the scheme did not performed or
     * if verbosity is set to false.
     */
    const std::vector<double>& history() const;

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
    bool startOfPeriod();

    /**
     * @brief Update the scheme w.r.t the new error and increment steps.
     * @param incr The new increment steps.
     */
    void updateApproximationScheme( unsigned int incr = 1 );

    /**
     * @brief Returns the remaining burn in.
     * @return Returns the remaining burn in.
     */
    Size remainingBurnIn();

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
    bool continueApproximationScheme( double error );

    ///  @}

    private:
    /**
     * @brief Stop the scheme given a new state.
     * @param new_state The scheme new state.
     */
    void _stopScheme( ApproximationSchemeSTATE new_state );

    protected:
    /// Current epsilon.
    double _current_epsilon;

    /// Last epsilon value.
    double _last_epsilon;

    /// Current rate.
    double _current_rate;

    /// The current step.
    Size _current_step;

    /// The timer.
    Timer _timer;

    /// The current state.
    ApproximationSchemeSTATE _current_state;

    /// The scheme history, used only if verbosity == true.
    std::vector<double> _history;

    /// Threshold for convergence.
    double _eps;

    /// If true, the threshold convergence is enabled.
    bool _enabled_eps;

    /// Threshold for the epsilon rate.
    double _min_rate_eps;

    /// If true, the minimal threshold for epsilon rate is enabled.
    bool _enabled_min_rate_eps;

    /// The timeout.
    double _max_time;

    /// If true, the timeout is enabled.
    bool _enabled_max_time;

    /// The maximum iterations.
    Size _max_iter;

    /// If true, the maximum iterations stopping criterion is enabled.
    bool _enabled_max_iter;

    /// Number of iterations before checking stopping criteria.
    Size _burn_in;

    /// Checking criteria frequency.
    Size _period_size;

    /// If true, verbosity is enabled.
    bool _verbosity;
  };
}  // namespace gum

#ifndef GUM_NO_INLINE
#include <agrum/core/approximations/approximationScheme_inl.h>
#endif

#endif  // GUM_APPROXIMATION_SCHEME_H
