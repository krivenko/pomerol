//
// This file is a part of pomerol - a scientific ED code for obtaining 
// properties of a Hubbard model on a finite-size lattice 
//
// Copyright (C) 2010-2012 Andrey Antipov <Andrey.E.Antipov@gmail.com>
// Copyright (C) 2010-2012 Igor Krivenko <Igor.S.Krivenko@gmail.com>
//
// pomerol is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pomerol is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pomerol.  If not, see <http://www.gnu.org/licenses/>.


/** \file src/Thermal.h
** \brief Thermal object (an object which has sense only for a finite temperature).
**
** \author Igor Krivenko (Igor.S.Krivenko@gmail.com)
*/
#ifndef __INCLUDE_THERMAL_H
#define __INCLUDE_THERMAL_H

#include "Misc.h"

namespace Pomerol{

struct Thermal {
    const RealType beta;
    const ComplexType MatsubaraSpacing;

    Thermal(RealType beta);
};

} // end of namespace Pomerol
#endif // endif :: #ifndef __INCLUDE_THERMAL_H

