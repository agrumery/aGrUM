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
 * @brief This file contains getters and setters defintion for ApproximationSchem
 *settings
  * @see ApproximationScheme
 *
 * @author Pierre-Henri WUILLEMIN
 */
#ifndef GUM_APPROXIMATION_SCHEME_CONFIGURATION_H
#define GUM_APPROXIMATION_SCHEME_CONFIGURATION_H

#include <math.h>

#include <agrum/config.h>
#include <agrum/core/signal/signaler.h>
#include <agrum/core/timer.h>

namespace gum {

  /// Approximation Scheme
  /**
   * An interface for configuration of approximation scheme
   @endcode
   */
  class IApproximationSchemeConfiguration {
    public:
    Signaler3<Size, double, double> onProgress; // progression,error,time
    Signaler1<std::string> onStop; // criteria messageApproximationScheme

    enum class ApproximationSchemeSTATE : char {
      Undefined,
      Continue,
      Epsilon,
      Rate,
      Limit,
      TimeLimit,
      Stopped
    };

    /// Constructors and Destructors
    /// @{
    IApproximationSchemeConfiguration() {
      GUM_CONSTRUCTOR(IApproximationSchemeConfiguration);
    };

    ~IApproximationSchemeConfiguration() {
      GUM_DESTRUCTOR(IApproximationSchemeConfiguration);
    };
    /// @}

    std::string messageApproximationScheme() const {
      std::stringstream s;

      switch (stateApproximationScheme()) {
        case ApproximationSchemeSTATE::Continue:
          s << "in progress";
          break;

        case ApproximationSchemeSTATE::Epsilon:
          s << "stopped with epsilon=" << epsilon();
          break;

        case ApproximationSchemeSTATE::Rate:
          s << "stopped with rate=" << minEpsilonRate();
          break;

        case ApproximationSchemeSTATE::Limit:
          s << "stopped with max iteration=" << maxIter();
          break;

        case ApproximationSchemeSTATE::TimeLimit:
          s << "stopped with timeout=" << maxTime();
          break;

        case ApproximationSchemeSTATE::Stopped:
          s << "stopped on request";
          break;

        case ApproximationSchemeSTATE::Undefined:
          s << "undefined state";
          break;
      };

      return s.str();
    }

    /// Given that we approximate f(t), stopping criterion on |f(t+1)-f(t)|
    /// If the criterion was disabled it will be enabled
    /// @{
    /// @throw OutOfLowerBound if eps<0
    virtual void setEpsilon(double eps) = 0;

    /// Get the value of epsilon
    virtual double epsilon(void) const = 0;

    /// Disable stopping criterion on epsilon
    virtual void disableEpsilon() = 0;
    /// Enable stopping criterion on epsilon
    virtual void enableEpsilon() = 0;
    /// @return true if stopping criterion on epsilon is enabled, false otherwise
    virtual bool isEnabledEpsilon() const = 0;
    /// @}

    /// Given that we approximate f(t), stopping criterion on d/dt(|f(t+1)-f(t)|)
    /// If the criterion was disabled it will be enabled
    /// @{
    /// @throw OutOfLowerBound if rate<0
    virtual void setMinEpsilonRate(double rate) = 0;

    /// Get the value of the minimal epsilon rate
    virtual double minEpsilonRate(void) const = 0;

    /// Disable stopping criterion on epsilon rate
    virtual void disableMinEpsilonRate() = 0;
    /// Enable stopping criterion on epsilon rate
    virtual void enableMinEpsilonRate() = 0;
    /// @return true if stopping criterion on epsilon rate is enabled, false
    /// otherwise
    virtual bool isEnabledMinEpsilonRate() const = 0;
    /// @}

    /// stopping criterion on number of iterations
    /// @{
    /// If the criterion was disabled it will be enabled
    /// @param max The maximum number of iterations
    /// @throw OutOfLowerBound if max<=1
    virtual void setMaxIter(Size max) = 0;

    /// @return the criterion on number of iterations
    virtual Size maxIter(void) const = 0;

    /// Disable stopping criterion on max iterations
    virtual void disableMaxIter() = 0;
    /// Enable stopping criterion on max iterations
    virtual void enableMaxIter() = 0;
    /// @return true if stopping criterion on max iterations is enabled, false
    /// otherwise
    virtual bool isEnabledMaxIter() const = 0;
    /// @}

    /// stopping criterion on timeout
    /// If the criterion was disabled it will be enabled
    /// @{
    /// @throw OutOfLowerBound if timeout<=0.0
    /** timeout is time in second (double).
    */
    virtual void setMaxTime(double timeout) = 0;

    /// returns the timeout (in seconds)
    virtual double maxTime(void) const = 0;

    /// get the current running time in second (double)
    virtual double currentTime(void) const = 0;

    /// Disable stopping criterion on timeout
    virtual void disableMaxTime() = 0;
    /// Enable stopping criterion on timeout
    virtual void enableMaxTime() = 0;
    /// @return true if stopping criterion on timeout is enabled, false otherwise
    virtual bool isEnabledMaxTime() const = 0;
    /// @}

    /// how many samples between 2 stopping isEnableds
    /// @{
    /// @throw OutOfLowerBound if p<1
    virtual void setPeriodSize(Size p) = 0;

    virtual Size periodSize(void) const = 0;
    /// @}

    /// size of burn in on number of iteration
    /// @{

    /// @throw OutOfLowerBound if b<1
    virtual void setBurnIn(Size b) = 0;

    virtual Size burnIn(void) const = 0;
    /// @}

    /// verbosity
    /// @{
    virtual void setVerbosity(bool v) = 0;

    virtual bool verbosity(void) const = 0;
    /// @}

    /// history
    /// @{

    virtual ApproximationSchemeSTATE stateApproximationScheme() const = 0;

    /// @throw OperationNotAllowed if scheme not performed
    virtual Size nbrIterations() const = 0;

    /// @throw OperationNotAllowed if scheme not performed or verbosity=false
    virtual const std::vector<double> &history() const = 0;
    /// @}

    // @name transmission of configuration
    void copyConfiguration(const IApproximationSchemeConfiguration &cfg) {
      GUM_TRACE("COPYING CONFIGURATION");
    }
  };
} // namespace gum
#endif // GUM_APPROXIMATION_SCHEME_CONFIGURATION_H
