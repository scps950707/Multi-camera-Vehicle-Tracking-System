#include "header.hpp"

class assignmentProblemSolver
{
private:
    void assignmentOptimal();
    void buildAssignmentVector();
    void step2a();
    void step2b();
    void step3();
    void step4( int row, int col );
    void step5();
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
    vector<int> assignment;
public:
    assignmentProblemSolver( int nOfRows, int nOfColumns );
    ~assignmentProblemSolver();
    void Solve( vector<float> &costMatrixIn );
    vector<int> getAssignment();
};
