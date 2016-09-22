#include "hungarianAlg.hpp"

AssignmentProblemSolver::AssignmentProblemSolver()
{
}

/* float AssignmentProblemSolver::Solve; {{{*/
float AssignmentProblemSolver::Solve(
    const vector<float> &distMatrixIn,
    size_t nOfRows,
    size_t nOfColumns,
    std::vector<int> &assignment
)
{
    assignment.resize( nOfRows, -1 );
    float cost = 0;
    assignmentoptimal( assignment, cost, distMatrixIn, nOfRows, nOfColumns );
    return cost;
}
/* }}} */

/* void AssignmentProblemSolver::assignmentoptimal {{{*/
// --------------------------------------------------------------------------
// Computes the optimal assignment (minimum overall costs) using Munkres algorithm.
// --------------------------------------------------------------------------
void AssignmentProblemSolver::assignmentoptimal( vector<int> &assignment, float &cost, const vector<float> &distMatrixIn, size_t nOfRows, size_t nOfColumns )
{
    // Generate distance cv::Matrix
    // and check cv::Matrix elements positiveness :)

    // Total elements number
    size_t nOfElements = nOfRows * nOfColumns;
    // Memory allocation
    float *distMatrix = new float[nOfElements];
    // Pointer to last element
    float *distMatrixEnd = distMatrix + nOfElements;

    for ( size_t row = 0; row < nOfElements; row++ )
    {
        float value = distMatrixIn[row];
        distMatrix[row] = value;
    }

    // Memory allocation use () opterator in C++ to init array to 0
    bool *coveredColumns = new bool[nOfColumns]();
    bool *coveredRows = new bool[nOfRows]();
    bool *starMatrix = new bool[nOfElements]();
    bool *primeMatrix = new bool[nOfElements]();
    bool *newStarMatrix = new bool[nOfElements](); /* used in step4 */

    /* preliminary steps */
    if ( nOfRows <= nOfColumns )
    {
        for ( size_t row = 0; row < nOfRows; row++ )
        {
            /* find the smallest element in the row */
            float *distMatrixTemp = distMatrix + row;
            float  minValue = *distMatrixTemp;
            distMatrixTemp += nOfRows;
            while ( distMatrixTemp < distMatrixEnd )
            {
                float value = *distMatrixTemp;
                if ( value < minValue )
                {
                    minValue = value;
                }
                distMatrixTemp += nOfRows;
            }
            /* subtract the smallest element from each element of the row */
            distMatrixTemp = distMatrix + row;
            while ( distMatrixTemp < distMatrixEnd )
            {
                *distMatrixTemp -= minValue;
                distMatrixTemp += nOfRows;
            }
        }
        /* Steps 1 and 2a */
        for ( size_t row = 0; row < nOfRows; row++ )
        {
            for ( size_t col = 0; col < nOfColumns; col++ )
            {
                if ( distMatrix[row + nOfRows * col] == 0 )
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
        for ( size_t col = 0; col < nOfColumns; col++ )
        {
            /* find the smallest element in the column */
            float *distMatrixTemp = distMatrix + nOfRows * col;
            float *columnEnd = distMatrixTemp + nOfRows;
            float  minValue = *distMatrixTemp++;
            while ( distMatrixTemp < columnEnd )
            {
                float value = *distMatrixTemp++;
                if ( value < minValue )
                {
                    minValue = value;
                }
            }
            /* subtract the smallest element from each element of the column */
            distMatrixTemp = distMatrix + nOfRows * col;
            while ( distMatrixTemp < columnEnd )
            {
                *distMatrixTemp++ -= minValue;
            }
        }
        /* Steps 1 and 2a */
        for ( size_t col = 0; col < nOfColumns; col++ )
        {
            for ( size_t row = 0; row < nOfRows; row++ )
            {
                if ( distMatrix[row + nOfRows * col] == 0 )
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

        for ( size_t row = 0; row < nOfRows; row++ )
        {
            coveredRows[row] = false;
        }
    }
    /* move to step 2b */
    step2b( assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, ( nOfRows <= nOfColumns ) ? nOfRows : nOfColumns );
    /* compute cost and remove invalid assignments */
    computeassignmentcost( assignment, cost, distMatrixIn, nOfRows );
    /* free allocated memory */
    delete []distMatrix;
    delete []coveredColumns;
    delete []coveredRows;
    delete []starMatrix;
    delete []primeMatrix;
    delete []newStarMatrix;
}
/* }}} */

/* void AssignmentProblemSolver::buildassignmentvector {{{*/
void AssignmentProblemSolver::buildassignmentvector( vector<int> &assignment, bool *starMatrix, size_t nOfRows, size_t nOfColumns )
{
    for ( size_t row = 0; row < nOfRows; row++ )
    {
        for ( size_t col = 0; col < nOfColumns; col++ )
        {
            if ( starMatrix[row + nOfRows * col] )
            {
                assignment[row] = static_cast<int>( col );
                break;
            }
        }
    }
}
/* }}} */

/* void AssignmentProblemSolver::computeassignmentcost {{{*/
void AssignmentProblemSolver::computeassignmentcost( const vector<int> &assignment, float &cost, const vector<float> &distMatrixIn, size_t nOfRows )
{
    for ( size_t row = 0; row < nOfRows; row++ )
    {
        const int col = assignment[row];
        if ( col >= 0 )
        {
            cost += distMatrixIn[row + nOfRows * col];
        }
    }
}
/* }}} */

/* void AssignmentProblemSolver::step2a {{{*/
void AssignmentProblemSolver::step2a( vector<int> &assignment, float *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim )
{
    bool *starMatrixTemp, *columnEnd;
    /* cover every column containing a starred zero */
    for ( size_t col = 0; col < nOfColumns; col++ )
    {
        starMatrixTemp = starMatrix + nOfRows * col;
        columnEnd = starMatrixTemp + nOfRows;
        while ( starMatrixTemp < columnEnd )
        {
            if ( *starMatrixTemp++ )
            {
                coveredColumns[col] = true;
                break;
            }
        }
    }
    /* move to step 3 */
    step2b( assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim );
}
/* }}} */

/* void AssignmentProblemSolver::step2b {{{*/
void AssignmentProblemSolver::step2b( vector<int> &assignment, float *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim )
{
    /* count covered columns */
    size_t nOfCoveredColumns = 0;
    for ( size_t col = 0; col < nOfColumns; col++ )
    {
        if ( coveredColumns[col] )
        {
            nOfCoveredColumns++;
        }
    }
    if ( nOfCoveredColumns == minDim )
    {
        /* algorithm finished */
        buildassignmentvector( assignment, starMatrix, nOfRows, nOfColumns );
    }
    else
    {
        /* move to step 3 */
        step3( assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim );
    }
}
/* }}} */

/* void AssignmentProblemSolver::step3 {{{*/
void AssignmentProblemSolver::step3( vector<int> &assignment, float *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim )
{
    bool zerosFound = true;
    while ( zerosFound )
    {
        zerosFound = false;
        for ( size_t col = 0; col < nOfColumns; col++ )
        {
            if ( !coveredColumns[col] )
            {
                for ( size_t row = 0; row < nOfRows; row++ )
                {
                    if ( ( !coveredRows[row] ) && ( distMatrix[row + nOfRows * col] == 0 ) )
                    {
                        /* prime zero */
                        primeMatrix[row + nOfRows * col] = true;
                        /* find starred zero in current row */
                        size_t starCol = 0;
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
                            step4( assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim, row, col );
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
    step5( assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim );
}
/* }}} */

/* void AssignmentProblemSolver::step4 {{{*/
void AssignmentProblemSolver::step4( vector<int> &assignment, float *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim, size_t row, size_t col )
{
    const size_t nOfElements = nOfRows * nOfColumns;
    /* generate temporary copy of starMatrix */
    for ( size_t n = 0; n < nOfElements; n++ )
    {
        newStarMatrix[n] = starMatrix[n];
    }
    /* star current zero */
    newStarMatrix[row + nOfRows * col] = true;
    /* find starred zero in current column */
    size_t starCol = col;
    size_t starRow = 0;
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
        size_t primeRow = starRow;
        size_t primeCol = 0;
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
    for ( size_t n = 0; n < nOfElements; n++ )
    {
        primeMatrix[n] = false;
        starMatrix[n] = newStarMatrix[n];
    }
    for ( size_t n = 0; n < nOfRows; n++ )
    {
        coveredRows[n] = false;
    }
    /* move to step 2a */
    step2a( assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim );
}
/* }}} */

/* void AssignmentProblemSolver::step5 {{{*/
void AssignmentProblemSolver::step5( vector<int> &assignment, float *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim )
{
    /* find smallest uncovered element h */
    float h = FLT_MAX;
    for ( size_t row = 0; row < nOfRows; row++ )
    {
        if ( !coveredRows[row] )
        {
            for ( size_t col = 0; col < nOfColumns; col++ )
            {
                if ( !coveredColumns[col] )
                {
                    const float value = distMatrix[row + nOfRows * col];
                    if ( value < h )
                    {
                        h = value;
                    }
                }
            }
        }
    }
    /* add h to each covered row */
    for ( size_t row = 0; row < nOfRows; row++ )
    {
        if ( coveredRows[row] )
        {
            for ( size_t col = 0; col < nOfColumns; col++ )
            {
                distMatrix[row + nOfRows * col] += h;
            }
        }
    }
    /* subtract h from each uncovered column */
    for ( size_t col = 0; col < nOfColumns; col++ )
    {
        if ( !coveredColumns[col] )
        {
            for ( size_t row = 0; row < nOfRows; row++ )
            {
                distMatrix[row + nOfRows * col] -= h;
            }
        }
    }
    /* move to step 3 */
    step3( assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim );
}
/* }}} */
