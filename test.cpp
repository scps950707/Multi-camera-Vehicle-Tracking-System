/********************************************************************
 ********************************************************************
 ** C++ class implementation of the Hungarian algorithm by David Schwarz, 2012
 **
 **
 ** O(n^3) implementation derived from libhungarian by Cyrill Stachniss, 2004
 **
 **
 ** Solving the Minimum Assignment Problem using the
 ** Hungarian Method.
 **
 ** ** This file may be freely copied and distributed! **
 **
 **
 ** This file is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied
 ** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 ** PURPOSE.
 **
 ********************************************************************
 ********************************************************************/

#include "hungarianAlg.hpp"

int main()
{

    /* an example cost matrix */
    vector<float> r(
    {
        10, 10, 13, 12, 14,
        19, 18, 16, 19, 17,
        8, 7, 9, 8, 10,
        15, 17, 14, 18, 19
    }
    );
    vector<int> assignment;

    assignmentProblemSolver aps( 4, 5 );
    aps.Solve( r, assignment );
    cout << "sizie:" << assignment.size() << endl;

    for ( uint i = 0; i < assignment.size(); i++ )
    {
        cout << assignment[i] << " ";
        cout << r[i * 5 + assignment[i]] << endl;
    }


    return 0;
}

