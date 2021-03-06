/*--------------------------------------------------------------------------------------
|     __  ______  ____  ______ |                                                       |
|    / / / / __ \/ __ \/ ____/ | HopeFOAM: High Order Parallel Extensible CFD Software |
|   / /_/ / / / / /_/ / __/    |                                                       |
|  / __  / /_/ / ____/ /___    |                                                       |
| /_/ /_/\____/_/   /_____/    | Copyright(c) 2017-2017 The Exercise Group, AMS, China.|
|                              |                                                       |
----------------------------------------------------------------------------------------

License
    This file is part of HopeFOAM, which is developed by Exercise Group, Innovation 
    Institute for Defence Science and Technology, the Academy of Military Science (AMS), China.

    HopeFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    HopeFOAM is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with HopeFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::domainDecomposition

Description
    Automatic domain decomposition class for finite-volume meshes

SourceFiles
    domainDecomposition.C
    domainDecompositionMesh.C
    domainDecompositionDistribute.C

\*---------------------------------------------------------------------------*/

#ifndef domainDecomposition_H
#define domainDecomposition_H

#include "fvMesh.H"
#include "labelList.H"
#include "SLList.H"
#include "PtrList.H"
#include "point.H"
#include "Time.H"
#include "volFields.H"


namespace Foam
{



/*---------------------------------------------------------------------------*\
                     Class domainDecomposition Declaration
\*---------------------------------------------------------------------------*/

class domainDecomposition
:
    public fvMesh
{
	//friend class processorPolyPatch;
//	friend class processorCyclicPolyPatch;
    // Private data

        //- Optional: points at the facesInstance
        autoPtr<pointIOField> facesInstancePointsPtr_;

        //- Mesh decomposition control dictionary
        IOdictionary decompositionDict_;

        //- Number of processors in decomposition
        label nProcs_;

        //- Is the decomposition data to be distributed for each processor
        bool distributed_;

        //- Processor label for each cell
        labelList cellToProc_;

        //- Labels of points for each processor
        labelListList procPointAddressing_;

        //- Labels of faces for each processor
        // Note: Face turning index is stored as the sign on addressing
        // Only the processor boundary faces are affected: if the sign of the
        // index is negative, the processor face is the reverse of the
        // original face. In order to do this properly, all face
        // indices will be incremented by 1 and the decremented as
        // necessary to avoid the problem of face number zero having no
        // sign.
        List<DynamicList<label> > procFaceAddressing_;

        //- Labels of cells for each processor
        labelListList procCellAddressing_;

        //- Sizes for processor mesh patches
        // Excludes inter-processor boundaries
        labelListList procPatchSize_;

        //- Start indices for processor patches
        // Excludes inter-processor boundaries
        labelListList procPatchStartIndex_;


        // Per inter-processor patch information

        //- Neighbour processor ID for inter-processor boundaries
        labelListList procNeighbourProcessors_;

        //- Sizes for inter-processor patches
        labelListList procProcessorPatchSize_;

        //- Start indices (in procFaceAddressing_) for inter-processor patches
        labelListList procProcessorPatchStartIndex_;

        //- Sub patch IDs for inter-processor patches
        List<labelListList> procProcessorPatchSubPatchIDs_;

        //- Sub patch sizes for inter-processor patches
        List<labelListList> procProcessorPatchSubPatchStarts_;

    // Private Member Functions

        void distributeCells();

        //- Mark all elements with value or -2 if occur twice
        static void mark
        (
            const labelList& zoneElems,
            const label zoneI,
            labelList& elementToZone
        );

        //- Append single element to list
        static void append(labelList&, const label);

        //- Add face to interProcessor patch.
        void addInterProcFace
        (
            const label facei,
            const label ownerProc,
            const label nbrProc,

            List<Map<label> >&,
            List<DynamicList<DynamicList<label> > >&
        ) const;


public:

    // Constructors

        //- Construct from IOobject
        domainDecomposition(const IOobject& io);


    //- Destructor
    ~domainDecomposition();


    // Member Functions

        //- Number of processor in decomposition
        label nProcs() const
        {
            return nProcs_;
        }

        //- Is the decomposition data to be distributed for each processor
        bool distributed() const
        {
            return distributed_;
        }

        //- Decompose mesh.
        void decomposeMesh();

        //- Write decomposition
        bool writeDecomposition(const bool decomposeSets);

        //- Cell-processor decomposition labels
        const labelList& cellToProc() const
        {
            return cellToProc_;
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
