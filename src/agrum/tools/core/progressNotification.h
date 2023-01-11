/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief priority queues (in which an element cannot appear more than once)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */

#ifndef AGRUM_PROGRESSNOTIFICATION_H
#define AGRUM_PROGRESSNOTIFICATION_H

#include <agrum/tools/core/signal/signalers.h>
#include <agrum/tools/core/timer.h>

namespace gum {

  /**
   * @class ProgressNotifier
   * @brief Notification for progress using listener
   *
   * An interface for progress notification
   */
  class ProgressNotifier {
    public:
    /// Progression (percent) and time.
    Signaler2< Size, double > onProgress;

    /// with a possible explanation for stopping
    Signaler1< const std::string& > onStop;
  };

  /**
   * @class ProgressListener
   * @brief The ProgressListener class.
   *
   */
  class ProgressListener: public Listener {
    public:
    /**
     * @brief Class constructor.
     * @param the progress notifier.
     */
    explicit ProgressListener(ProgressNotifier& notif);

    /**
     * @brief Class destructor.
     */
    virtual ~ProgressListener();

    /**
     * @brief When progress was made.
     * @param src The signal source.
     * @param step The advancement in percent.
     * @param time The time.
     */
    virtual void whenProgress(const void* src, const Size step, const double time) = 0;

    /**
     * @brief When it stops.
     * @param src The signal source.
     * @param message A message.
     */
    virtual void whenStop(const void* src, const std::string& message) = 0;

    protected:
    /**
     * Copy constructor.
     */
    ProgressListener(const ProgressListener& other);

    /**
     * Copy operator.
     */
    ProgressListener& operator=(const ProgressListener& other);

    private:
    /// The scheme configuration.
    ProgressNotifier& _notif_;
  };
}   // namespace gum
#endif   // AGRUM_PROGRESSNOTIFICATION_H
