#include "hungarianAlg.hpp"
#include <iomanip>

int main()
{

    /* for example row size:total worker, column size: total jobs */
    int rows = 4;
    int cols = 5;
    vector<float> cost(
    {
        10, 10, 13, 12, 14,
        19, 18, 16, 19, 17,
        8, 7, 9, 8, 10,
        15, 17, 14, 18, 19
    }
    );
    vector<int> assignment;

    assignmentProblemSolver aps( rows, cols );
    aps.Solve( cost, assignment );

    cout << "original matrix" << endl;
    for ( int i = 0; i < rows; i++ )
    {
        for ( int j = 0; j < cols; j++ )
        {
            cout << setw( 3 ) << cost[i * cols + j];
        }
        cout << endl;
    }

    cout << "expect" << endl;
    cout << "10  x  x  x  x" << endl;
    cout << " x  x  x  x 17" << endl;
    cout << " x  x  9  x  x" << endl;
    cout << " x 17  x  x  x" << endl;

    cout << "assigned" << endl;

    for ( unsigned int i = 0; i < assignment.size(); i++ )
    {
        for ( int j = 0; j < cols; j++ )
        {
            if ( assignment[i] == j )
            {
                cout << setw( 3 ) << cost[i * cols + assignment[i]];
            }
            else
            {
                cout << setw( 3 ) << "x";
            }
        }
        cout << endl;
    }
    return 0;
}
