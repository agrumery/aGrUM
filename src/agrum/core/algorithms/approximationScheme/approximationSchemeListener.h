/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Pierre-Henri WUILLEMIN <pierre-henri.wuillemin@lip6.fr>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef APPROXIMATIONSCHEMELISTENER_H
#define APPROXIMATIONSCHEMELISTENER_H

#include <agrum/config.h>
#include <agrum/core/signal/listener.h>

#include <agrum/core/algorithms/approximationScheme/IApproximationSchemeConfiguration.h>

namespace gum {
  class ApproximationSchemeListener : public Listener {

    public:
    ApproximationSchemeListener(IApproximationSchemeConfiguration &sch);
    virtual ~ApproximationSchemeListener();

    virtual void whenProgress(const void *src, Size pourcent, double error,
                              double time) = 0;
    virtual void whenStop(const void *src, std::string message) = 0;

    protected:
    ApproximationSchemeListener(const ApproximationSchemeListener &other);
    ApproximationSchemeListener &operator=(const ApproximationSchemeListener &other);

    private:
    IApproximationSchemeConfiguration &__sch;
  };
}

#endif // APPROXIMATIONSCHEMELISTENER_H
