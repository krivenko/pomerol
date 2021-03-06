//
// This file is a part of pomerol - a scientific ED code for obtaining 
// properties of a Hubbard model on a finite-size lattice 
//
// Copyright (C) 2010-2011 Andrey Antipov <Andrey.E.Antipov@gmail.com>
// Copyright (C) 2010-2011 Igor Krivenko <Igor.S.Krivenko@gmail.com>
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


/** \file Symmetrizer.cpp
**  \brief Implementation of the Symmetrizer class - a class to store and get the information about the symmetries of the system.
** 
**  \author    Andrey Antipov (Andrey.E.Antipov@gmail.com)
*/

#include "Symmetrizer.h"
#include "OperatorPresets.h"

namespace Pomerol { 

//
//Symmetrizer::IndexPermutation
//

Symmetrizer::IndexPermutation::IndexPermutation(const DynamicIndexCombination &in):N(in.getNumberOfIndices())
{
    if ( checkConsistency(in) && checkIrreducibility(in) ) { 
        Combinations.push_back(new DynamicIndexCombination(in));
        CycleLength=0;
        calculateCycleLength();    
    }
    else throw ( DynamicIndexCombination::exWrongIndices()) ;
}

bool Symmetrizer::IndexPermutation::checkConsistency(const DynamicIndexCombination &in)
{

    for (ParticleIndex i=0; i<N; ++i) {
        if (in.getIndex(i)>=N) { 
            ERROR("Indices in IndexPermutation should belong to the interval 0..N-1"); 
            return false;
            };
        for (ParticleIndex j=i+1; j<N; ++j)
            if (in.getIndex(i)==in.getIndex(j)) { 
            ERROR("Found equal indices in given combination");
            return false;
            }
        };
    return true;
}

bool Symmetrizer::IndexPermutation::checkIrreducibility(const DynamicIndexCombination &in)
{
    std::map<ParticleIndex, unsigned int> nontrivial_indices; // Here collected indices, which are changed
    std::vector<ParticleIndex> trivial_indices;
    ParticleIndex current_index=0;

    while ((nontrivial_indices.size() + trivial_indices.size())!=N) {
        //DEBUG("Current index : " << current_index << "-->" << in.getIndex(current_index));
        if (current_index == in.getIndex(current_index)) { // Index is a trivial index - no loop for it is done
            trivial_indices.push_back(current_index);
            current_index++;
            }
        else if (nontrivial_indices.find(current_index)!=nontrivial_indices.end()) // Check that this index was found during previous iterations.
            current_index++;
            else if ( nontrivial_indices.size()==0 ) { // This is a first nontrivial index found - start a loop then.
                nontrivial_indices[current_index]=1;
                ParticleIndex result=in.getIndex(current_index);
                //DEBUG("Begin with" << current_index);
                //DEBUG("-->" << result);
                while ( result!=current_index) { // make a small loop in indices to determine the length of found cycle
                    nontrivial_indices[result]=1;
                    result=in.getIndex(result);
                    //DEBUG("-->" << result);
                    };
                current_index++;
            }
        else { 
            ERROR("Permutation " << in << " is reducible"); 
            return false; // Once an index which is not a part of previously checked loop is found means a permutation is reducible.
            };
    };
    if ( trivial_indices.size() == N ) { ERROR("Identity permutation " << in << " is rejected."); return false; } // reject trivial identity permutation.
    return true; 
}

void Symmetrizer::IndexPermutation::calculateCycleLength()
{
    DynamicIndexCombination initial(**(Combinations.begin()));
    DynamicIndexCombination current(**(Combinations.begin()));
    DynamicIndexCombination trivial (Symmetrizer::generateTrivialCombination(N)); // #warning think of better static implementation
    DynamicIndexCombination next(N);
    for (ParticleIndex i=0; i<N; ++i) trivial[i] = i;
    bool exit_loop=false;
    while (!exit_loop) { 
        for (ParticleIndex i=0; i<N; ++i) next[i]=current[current[i]];
        CycleLength++;
        exit_loop = ( next == initial || next == trivial );
        current = next;
        }
}

const DynamicIndexCombination& Symmetrizer::IndexPermutation::getIndices( unsigned int cycle_number ) const
{
    return *Combinations[cycle_number];
}

const unsigned int Symmetrizer::IndexPermutation::getCycleLength() const
{
    return CycleLength;
}

const char* Symmetrizer::IndexPermutation::exEqualIndices::what() const throw(){
    return "Cannot have equal indices in the Symmetrizer index combination";
};

// 
// Symmetrizer::QuantumNumbers
//

Symmetrizer::QuantumNumbers::QuantumNumbers(int amount):amount(amount),numbers( std::vector<MelemType>(amount) ),NumbersHash(numbers_hash_generator(numbers)) 
{
};

bool Symmetrizer::QuantumNumbers::set ( int pos, MelemType val )
{ 
    if (pos<amount) { 
        numbers[pos] = val; 
        NumbersHash = numbers_hash_generator(numbers); 
        }
    else { 
        ERROR("Tried to insert element " << val << " to wrong position " << pos << " in " << __PRETTY_FUNCTION__ );
        return false; 
        };
    return true; 
}

bool Symmetrizer::QuantumNumbers::operator< (const Symmetrizer::QuantumNumbers& rhs) const 
{
    return (NumbersHash<rhs.NumbersHash);
}

bool Symmetrizer::QuantumNumbers::operator== (const Symmetrizer::QuantumNumbers& rhs) const 
{
    return (NumbersHash==rhs.NumbersHash);
}

bool Symmetrizer::QuantumNumbers::operator!= (const Symmetrizer::QuantumNumbers& rhs) const 
{
    return (NumbersHash!=rhs.NumbersHash);
}

//
// Symmetrizer
//

Symmetrizer::Symmetrizer(const IndexClassification &IndexInfo, const IndexHamiltonian &Storage):
    ComputableObject(), 
    IndexInfo(IndexInfo), 
    Storage(Storage), 
    NSymmetries(0)
{
}

const DynamicIndexCombination& Symmetrizer::generateTrivialCombination(ParticleIndex N)
{
    static DynamicIndexCombination trivial(N);
    for (ParticleIndex i=0; i<N; ++i) trivial[i] = i;
    return trivial;
}

const std::vector<boost::shared_ptr<Operator> >& Symmetrizer::getOperations() const
{
    return Operations;
}

bool Symmetrizer::checkSymmetry(const Operator &in)
{
    boost::shared_ptr<Operator> OP1 ( new Operator(in));
    if (Storage.commutes(*OP1)) { 
        Operations.push_back(OP1);
        NSymmetries++;
        return true;
    }
    else return false;
}

void Symmetrizer::compute(bool ignore_symmetries)
{
    IndexSize = IndexInfo.getIndexSize(); 
    if (Status>=Computed) return;
    if (!ignore_symmetries) {
        // Check particle number conservation
        Operator op_n = Pomerol::OperatorPresets::N(IndexSize);
        if (this->checkSymmetry(op_n)) INFO("[ H ,"<< op_n << " ]=0");

        // Check Sz conservation
        bool valid_sz = true;
        for (ParticleIndex i=0; i<IndexSize && valid_sz; ++i) 
            valid_sz = valid_sz && (IndexInfo.getInfo(i).Spin == up || IndexInfo.getInfo(i).Spin == down);
        if (valid_sz) {
            std::vector<ParticleIndex> SpinUpIndices;
            for (ParticleIndex i=0; i<IndexSize; ++i) { 
                unsigned short Spin = IndexInfo.getInfo(i).Spin;
                if ( Spin == up ) SpinUpIndices.push_back(i);
                } 
            Operator op_sz = Pomerol::OperatorPresets::Sz(IndexSize, SpinUpIndices);
            if (this->checkSymmetry(op_sz)) INFO("[ H ,"<< op_sz << " ]=0");
            };
    };

    Status = Computed;
}

Symmetrizer::QuantumNumbers Symmetrizer::getQuantumNumbers() const
{
    return Symmetrizer::QuantumNumbers(NSymmetries);
}

std::ostream& operator<<(std::ostream& output, const Symmetrizer::QuantumNumbers& out)
{
    output << "[";
    for (int i=0 ;i<out.amount-1; ++i) output << out.numbers[i] << ",";
    if ( out.amount ) output << out.numbers[out.amount-1];
    output << "]";
    return output;
}

} // end of namespace Pomerol 

