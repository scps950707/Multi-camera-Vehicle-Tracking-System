#include "hungarianAlg.hpp"
#include <iomanip>

int main()
{

    /* for example row size:total worker, column size: total jobs */
    int rows = 4;
    int cols = 4;
    vector<float> cost(
    {
        80, 40, 50, 46,
        40, 70, 20, 25,
        30, 10, 20, 30,
        35, 20, 25, 30
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
