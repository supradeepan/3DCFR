#include"Main.h"
#ifndef Sai_3DPRESOLVER_H_
#define Sai_3DPRESOLVER_H_

    void PreSolver(int NoOfElements, int NoOfNodes, CELLDETAILS *CC, NODE *Node, int Comp);
    void ReadNeutralFile(char *ElementFileName, int NoOfElements, int NoOfNodes, CELLDETAILS *CD, NODE *Node);
    void WritePLTFILE(int NoOfElements, int NoOfNodes, CELLDETAILS *CC, NODE *Node);
    void WriteNodalPLTFILE(int NoOfElements, int NoOfNodes, CELLDETAILS *CC, NODE *Node);
    void FindNeighbour(int Elem, int NoOfElements, CELLDETAILS *CC);
    void CalculateOutwordNormals(int NoOfElements, CELLDETAILS *CC, NODE *Node);
    #define CODE 7 // NoOf Boundaries
#endif