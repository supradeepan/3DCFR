#include <stdio.h> // Include the standard I/O library
#include <time.h>
#include <stdlib.h>

#include "Main.h"
#include"PreSolver.h"
#include"PreSolver.c"

int main() {
    time_t start, end;
    NODE *Node;
    CELLDETAILS *CC;
    CELLDATAS *CD, *CDO; 
    BOUNDARYELEMENTS *BoundElems;
    double dif;
    int NoOfElements, NoOfNodes, NoOfBoundaryElements;
    int Iter;

    GetNoOfElemAndNodes(&NoOfElements, &NoOfNodes);

	printf("No of nodes %d\nNo of Elemetns %d\n",NoOfNodes, NoOfElements);

	if((Node =(NODE*)malloc((NoOfNodes)*sizeof(NODE)))==NULL)
	{
		printf("Not Enough Memory for Allocating Nodes. \nProgram Terminated");
		exit(2);
	}
	
	if((CC =(CELLDETAILS*)malloc((NoOfElements)*sizeof(CELLDETAILS)))==NULL)
	{
		printf("Not Enough Memory for Allocating Nodes. \nProgram Terminated");
		exit(2);
	}

	if((CD =(CELLDATAS*)malloc((NoOfElements)*sizeof(CELLDATAS)))==NULL)
	{
		printf("Not Enough Memory for Allocating Nodes. \nProgram Terminated");
		exit(2);
	}

	if((CDO =(CELLDATAS*)malloc((NoOfElements)*sizeof(CELLDATAS)))==NULL)
	{
		printf("Not Enough Memory for Allocating Nodes. \nProgram Terminated");
		exit(2);
	}
	
	PreSolver(NoOfElements, NoOfNodes, CC, Node, COMP); 


    free(Node);
	free(CC);
	free(CD);
	free(CDO);
    time (&end);
	dif = difftime (end,start);
	printf ("It took you %e seconds to run your code.\n", dif );
	printf("Program Terminated...\nPress any key to exit\n");
    return 0;
}


void GetNoOfElemAndNodes(int *NElem, int *NNode)
{
	FILE *FPElem;
	char FileElemName[100];
	int E_NUMNP, E_NELEM, E_NGRPS, E_NBSETS, E_NDFCD, E_NDFVL;
	char E_HEADNUT[100];
	int t;
	char *t1;

	switch (COMP)
	{
	case 1:
		sprintf(FileElemName,"Mesh\\Elements.neu");
		break;
	case 2:
		sprintf(FileElemName,".//Mesh//Elements.neu");
		break;
	default:
		printf("\n OS version and Compiler Version Error.\nProgram Terminated.\n\n");
		exit(0);
	}
	
	if( (FPElem = fopen(FileElemName,"rt") )==NULL)
	{
		printf("Unable to Open Mesh//Elements.neu\n");
		printf("Program Terminated.");
		exit(3);
	}

	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);

	t = fscanf(FPElem, "%d %d %d %d %d %d", &E_NUMNP, &E_NELEM, &E_NGRPS, &E_NBSETS, &E_NDFCD, &E_NDFVL);
	
	*NElem = E_NELEM;
	*NNode = E_NUMNP;
}