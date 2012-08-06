//
// This file is a part of pomerol - a scientific ED code for obtaining 
// properties of a Hubbard model on a finite-size lattice 
//
// Copyright (C) 2010-2012 Andrey Antipov <antipov@ct-qmc.org>
// Copyright (C) 2010-2012 Igor Krivenko <igor@shg.ru>
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


/** \file src/DensityMatrix.h
** \brief A storage of the matrix elements of the hamiltonian in Fock basis, provides eigenvalues and eigenfunctions
** 
** \author Andrey Antipov(antipov@ct-qmc.org)
** \author Igor Krivenko (igor@shg.ru)
*/

#ifndef __INCLUDE_HAMILTONIAN_H
#define __INCLUDE_HAMILTONIAN_H
#include "Misc.h"
#include "Logger.h"
#include "HDF5Storage.h"
#include "IndexClassification.h"
#include "IndexHamiltonian.h"
#include "StatesClassification.h"
#include "HamiltonianPart.h"

namespace Pomerol{

/** This class represents a Hamiltonian, written as a matrix of matrix elements in a Fock basis.
 * It is a container for several hamiltonian parts, each for single defined QuantumNumbers and a corresponding BlockNumber. 
 * It provides eigenvalues and eigenfunctions of any of its parts once they are obtained within its parts. 
 * The diagonalization and entering routines are done inside of HamiltonianPart instances.
 */
class Hamiltonian : public HDF5Storable, public ComputableObject
{
    /** Statuses of the object */
    enum {Constructed, Prepared, Diagonalized};
    /** Array of pointers to the Hamiltonian Parts */
    std::vector<boost::shared_ptr<HamiltonianPart> > parts;
    /** A reference to the IndexClassification object. */
    const IndexClassification &IndexInfo;
    /** A reference to the IndexHamiltonian object. */
    const IndexHamiltonian &F;
    /** A reference to the StatesClassification object. */
    const StatesClassification& S;
    /** A value of the ground energy - needed for further renormalization */
    RealType GroundEnergy;
public:

    /** Constructor. */
    Hamiltonian(const IndexClassification &IndexInfo, const IndexHamiltonian& F, const StatesClassification &S);
    /** Destructor. */
    ~Hamiltonian();

    void prepare();
    void diagonalize(void);
    void reduce(const RealType Cutoff);

    const HamiltonianPart& getPart(const QuantumNumbers &in) const;
    const HamiltonianPart& getPart(BlockNumber in) const;
    RealType getEigenValue(unsigned long state) const;
    RealType getGroundEnergy() const;

    void save(H5::CommonFG* RootGroup) const;
    void load(const H5::CommonFG* RootGroup);

private:
    void computeGroundEnergy();
};

} // end of namespace Pomerol
#endif // endif :: #ifndef __INCLUDE_HAMILTONIAN_H

