#include "header.hpp"

class AssignmentProblemSolver
{
private:
    void assignmentoptimal( vector<int> &assignment, float &cost, const vector<float> &distMatrixIn, size_t nOfRows, size_t nOfColumns );
    void buildassignmentvector( vector<int> &assignment, bool *starMatrix, size_t nOfRows, size_t nOfColumns );
    void computeassignmentcost( const vector<int> &assignment, float &cost, const vector<float> &distMatrixIn, size_t nOfRows );
    void step2a( vector<int> &assignment, float *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim );
    void step2b( vector<int> &assignment, float *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim );
    void step3( vector<int> &assignment, float *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim );
    void step4( vector<int> &assignment, float *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim, size_t row, size_t col );
    void step5( vector<int> &assignment, float *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, size_t nOfRows, size_t nOfColumns, size_t minDim );
public:
    AssignmentProblemSolver();
    float Solve( const vector<float> &distMatrixIn, size_t nOfRows, size_t nOfColumns, vector<int> &assignment );
};
