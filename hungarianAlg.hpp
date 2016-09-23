#include "header.hpp"

class assignmentProblemSolver
{
private:
    void assignmentOptimal( vector<int> &assignment );
    void buildAssignmentVector( vector<int> &assignment );
    void step2a( vector<int> &assignment );
    void step2b( vector<int> &assignment );
    void step3( vector<int> &assignment );
    void step4( vector<int> &assignment, int row, int col );
    void step5( vector<int> &assignment );
    int nOfRows;
    int nOfColumns;
    int nOfElements;
    int minDim;
    bool *coveredColumns;
    bool *coveredRows;
    bool *starMatrix;
    bool *primeMatrix;
    bool *newStarMatrix; /* used in step4 */
    float *costMatrix;
public:
    assignmentProblemSolver( int nOfRows, int nOfColumns );
    ~assignmentProblemSolver();
    void Solve( vector<float> &costMatrixIn, vector<int> &assignment );
};
