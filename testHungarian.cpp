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

