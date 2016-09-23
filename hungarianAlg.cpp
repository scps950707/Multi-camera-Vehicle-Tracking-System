#include "hungarianAlg.hpp"

/* assignmentProblemSolver::assignmentProblemSolver( int nOfRows, int nOfColumns ) {{{*/
assignmentProblemSolver::assignmentProblemSolver( int nOfRows, int nOfColumns )
{
    this->nOfRows = nOfRows;
    this->nOfColumns = nOfColumns;
    // Total elements number
    this->nOfElements = nOfRows * nOfColumns;
    // Memory allocation use () opterator in C++ to init array to 0
    this->coveredColumns = new bool[nOfColumns]();
    this->coveredRows = new bool[nOfRows]();
    this->starMatrix = new bool[nOfElements]();
    this->primeMatrix = new bool[nOfElements]();
    this->newStarMatrix = new bool[nOfElements](); /* used in step4 */
    this->costMatrix = new float[nOfElements];
    this->minDim = nOfRows <= nOfColumns ? nOfRows : nOfColumns;
}
/* }}} */

/* assignmentProblemSolver::~assignmentProblemSolver() {{{*/
assignmentProblemSolver::~assignmentProblemSolver()
{
    delete []costMatrix;
    delete []coveredColumns;
    delete []coveredRows;
    delete []starMatrix;
    delete []primeMatrix;
    delete []newStarMatrix;
}
/* }}} */

/* float assignmentProblemSolver::Solve; {{{*/
void assignmentProblemSolver::Solve( vector<float> &costMatrixIn )
{
    assignment.resize( nOfRows, -1 );
    for ( int i = 0; i < nOfElements; i++ )
    {
        costMatrix[i] = costMatrixIn[i];
    }
    assignmentOptimal();
}
/* }}} */

/* vector<int> assignmentProblemSolver::getAssignment() {{{*/
vector<int> assignmentProblemSolver::getAssignment()
{
    return this->assignment;
}
/* }}} */

/* void assignmentProblemSolver::assignmentOptimal {{{*/
void assignmentProblemSolver::assignmentOptimal()
{
    /* preliminary steps */
    if ( nOfRows <= nOfColumns )
    {
        for ( int row = 0; row < nOfRows; row++ )
        {
            /* find the smallest element in the row */
            float minVal = FLT_MAX;
            for ( int col = 0; col < nOfColumns; col++ )
            {
                minVal = min( minVal, costMatrix[row * nOfColumns + col] );
            }
            /* subtract the smallest element from each element of the row */
            for ( int col = 0; col < nOfColumns; col++ )
            {
                costMatrix[row * nOfColumns + col] -= minVal;
            }
        }
        /* Steps 1 and 2a */
        for ( int row = 0; row < nOfRows; row++ )
        {
            for ( int col = 0; col < nOfColumns; col++ )
            {
                if ( costMatrix[row + nOfRows * col] == 0 )
                {
                    if ( !coveredColumns[col] )
                    {
                        starMatrix[row + nOfRows * col] = true;
                        coveredColumns[col] = true;
                        break;
                    }
                }
            }
        }
    }
    else /* if(nOfRows > nOfColumns) */
    {
        for ( int col = 0; col < nOfColumns; col++ )
        {
            /* find the smallest element in the column */
            float minVal = FLT_MAX;
            for ( int row = 0; row < nOfRows; row++ )
            {
                minVal = min( minVal, costMatrix[col * nOfRows + row] );
            }
            /* subtract the smallest element from each element of the column */
            for ( int row = 0; row < nOfRows; row++ )
            {
                costMatrix[col * nOfRows + row] -= minVal;
            }
        }
        /* Steps 1 and 2a */
        for ( int col = 0; col < nOfColumns; col++ )
        {
            for ( int row = 0; row < nOfRows; row++ )
            {
                if ( costMatrix[row + nOfRows * col] == 0 )
                {
                    if ( !coveredRows[row] )
                    {
                        starMatrix[row + nOfRows * col] = true;
                        coveredColumns[col] = true;
                        coveredRows[row] = true;
                        break;
                    }
                }
            }
        }

        for ( int row = 0; row < nOfRows; row++ )
        {
            coveredRows[row] = false;
        }
    }
    /* move to step 2b */
    step2b();
}
/* }}} */

/* void assignmentProblemSolver::buildAssignmentVector {{{*/
void assignmentProblemSolver::buildAssignmentVector()
{
    for ( int row = 0; row < nOfRows; row++ )
    {
        for ( int col = 0; col < nOfColumns; col++ )
        {
            if ( starMatrix[row + nOfRows * col] )
            {
                assignment[row] = col;
                break;
            }
        }
    }
}
/* }}} */

/* void assignmentProblemSolver::step2a {{{*/
void assignmentProblemSolver::step2a()
{
    for ( int col = 0; col < nOfColumns; col++ )
    {
        for ( int row = 0; row < nOfRows; row++ )
        {
            if ( starMatrix[nOfRows * col + row] )
            {
                coveredColumns[col] = true;
                break;
            }
        }
    }
    step2b();
}
/* }}} */

/* void assignmentProblemSolver::step2b {{{*/
void assignmentProblemSolver::step2b()
{
    /* count covered columns */
    int nOfCoveredColumns = 0;
    for ( int col = 0; col < nOfColumns; col++ )
    {
        if ( coveredColumns[col] )
        {
            nOfCoveredColumns++;
        }
    }
    if ( nOfCoveredColumns == minDim )
    {
        /* algorithm finished */
        buildAssignmentVector();
    }
    else
    {
        /* move to step 3 */
        step3();
    }
}
/* }}} */

/* void assignmentProblemSolver::step3 {{{*/
void assignmentProblemSolver::step3()
{
    bool zerosFound = true;
    while ( zerosFound )
    {
        zerosFound = false;
        for ( int col = 0; col < nOfColumns; col++ )
        {
            if ( !coveredColumns[col] )
            {
                for ( int row = 0; row < nOfRows; row++ )
                {
                    if ( ( !coveredRows[row] ) && ( costMatrix[row + nOfRows * col] == 0 ) )
                    {
                        /* prime zero */
                        primeMatrix[row + nOfRows * col] = true;
                        /* find starred zero in current row */
                        int starCol = 0;
                        for ( ; starCol < nOfColumns; starCol++ )
                        {
                            if ( starMatrix[row + nOfRows * starCol] )
                            {
                                break;
                            }
                        }
                        if ( starCol == nOfColumns ) /* no starred zero found */
                        {
                            /* move to step 4 */
                            step4( row, col );
                            return;
                        }
                        else
                        {
                            coveredRows[row] = true;
                            coveredColumns[starCol] = false;
                            zerosFound = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    /* move to step 5 */
    step5();
}
/* }}} */

/* void assignmentProblemSolver::step4 {{{*/
void assignmentProblemSolver::step4( int row, int col )
{
    /* generate temporary copy of starMatrix */
    for ( int n = 0; n < nOfElements; n++ )
    {
        newStarMatrix[n] = starMatrix[n];
    }
    /* star current zero */
    newStarMatrix[row + nOfRows * col] = true;
    /* find starred zero in current column */
    int starCol = col;
    int starRow = 0;
    for ( ; starRow < nOfRows; starRow++ )
    {
        if ( starMatrix[starRow + nOfRows * starCol] )
        {
            break;
        }
    }
    while ( starRow < nOfRows )
    {
        /* unstar the starred zero */
        newStarMatrix[starRow + nOfRows * starCol] = false;
        /* find primed zero in current row */
        int primeRow = starRow;
        int primeCol = 0;
        for ( ; primeCol < nOfColumns; primeCol++ )
        {
            if ( primeMatrix[primeRow + nOfRows * primeCol] )
            {
                break;
            }
        }
        /* star the primed zero */
        newStarMatrix[primeRow + nOfRows * primeCol] = true;
        /* find starred zero in current column */
        starCol = primeCol;
        for ( starRow = 0; starRow < nOfRows; starRow++ )
        {
            if ( starMatrix[starRow + nOfRows * starCol] )
            {
                break;
            }
        }
    }
    /* use temporary copy as new starMatrix */
    /* delete all primes, uncover all rows */
    for ( int n = 0; n < nOfElements; n++ )
    {
        primeMatrix[n] = false;
        starMatrix[n] = newStarMatrix[n];
    }
    for ( int n = 0; n < nOfRows; n++ )
    {
        coveredRows[n] = false;
    }
    /* move to step 2a */
    step2a();
}
/* }}} */

/* void assignmentProblemSolver::step5 {{{*/
void assignmentProblemSolver::step5()
{
    /* find smallest uncovered element h */
    float h = FLT_MAX;
    for ( int row = 0; row < nOfRows; row++ )
    {
        if ( !coveredRows[row] )
        {
            for ( int col = 0; col < nOfColumns; col++ )
            {
                if ( !coveredColumns[col] )
                {
                    h = min( h, costMatrix[row + nOfRows * col] );
                }
            }
        }
    }
    /* add h to each covered row */
    for ( int row = 0; row < nOfRows; row++ )
    {
        if ( coveredRows[row] )
        {
            for ( int col = 0; col < nOfColumns; col++ )
            {
                costMatrix[row + nOfRows * col] += h;
            }
        }
    }
    /* subtract h from each uncovered column */
    for ( int col = 0; col < nOfColumns; col++ )
    {
        if ( !coveredColumns[col] )
        {
            for ( int row = 0; row < nOfRows; row++ )
            {
                costMatrix[row + nOfRows * col] -= h;
            }
        }
    }
    /* move to step 3 */
    step3();
}
/* }}} */
