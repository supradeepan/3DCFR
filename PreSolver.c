#include"PreSolver.h"
#include"Main.h"
#include<math.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
//#include<process.h>
//#include<conio.h>
#include <stdio.h>


void PreSolver(int NoOfElements, int NoOfNodes, CELLDETAILS *CC, NODE *Node, int Comp)
{
	char FileElemName[100], FileNeighName[100];
	int Elem;
	FILE *Fwrite;
	int nElem, nNode;

	switch (Comp)
	{
	case 1:
		sprintf(FileElemName,"Mesh\\Elements.neu");
		sprintf(FileNeighName, "Mesh\\Neigh.dat");
		break;

	case 2:
		sprintf(FileElemName,".//Mesh//Elements.neu");
		sprintf(FileNeighName, ".//Mesh//Neigh.dat");
		break;

	default:
		printf("\n OS version and Compiler Version Error.\nProgram Terminated.\n\n");
		exit(0);
	}
	ReadNeutralFile(FileElemName, NoOfElements, NoOfNodes, CC, Node);
	WritePLTFILE(NoOfElements, NoOfNodes, CC, Node);
	WriteNodalPLTFILE(NoOfElements, NoOfNodes, CC, Node);
	
    printf("\nFinding Neighbours...");
	
	if( (Fwrite = fopen(FileNeighName,"r")) == NULL)
	{
		printf("\nNeigh.dat File not Found...\n");
		for(Elem=0;Elem<NoOfElements;Elem++)
		{
			FindNeighbour(Elem, NoOfElements, CC);		
		}
		if( (Fwrite = fopen(FileNeighName,"w+")) == NULL)
		{
			printf("Unable to open Out File");
			exit(0) ;
		}
		fprintf(Fwrite,"%d\t%d\n",NoOfElements, NoOfNodes);
		for(Elem=0;Elem<NoOfElements;Elem++)
		{
			fprintf(Fwrite,"%d\t%d\t%d\t%d\n", CC[Elem].Neighbour[0], CC[Elem].Neighbour[1], CC[Elem].Neighbour[2], CC[Elem].Neighbour[3]);
		}
	}
	else
	{
		fscanf(Fwrite,"%d\t%d\n",&nElem, &nNode);
		if(nElem == NoOfElements && nNode == NoOfNodes)
		{
			for(Elem=0;Elem<NoOfElements;Elem++)
			{
				fscanf(Fwrite,"%d\t%d\t%d\t%d\n", &CC[Elem].Neighbour[0], &CC[Elem].Neighbour[1], &CC[Elem].Neighbour[2], &CC[Elem].Neighbour[3]);
			}
		}
		else
		{
			fclose(Fwrite);
			if(Comp == 1)
				system ("del Mesh\\Neigh.dat");
			if(Comp == 2)
				system ("rm -f .//Mesh//Neigh.dat");

			printf("\nWrong Neigh.dat File Found...\n");
			for(Elem=0;Elem<NoOfElements;Elem++)
			{
				FindNeighbour(Elem, NoOfElements, CC);		
			}
			if( (Fwrite = fopen(FileNeighName,"w+")) == NULL)
			{
				printf("Unable to open Out File");
				exit(0) ;
			}
			fprintf(Fwrite,"%d\t%d\n",NoOfElements, NoOfNodes);
			for(Elem=0;Elem<NoOfElements;Elem++)
			{
				fprintf(Fwrite,"%d\t%d\t%d\t%d\n", CC[Elem].Neighbour[0], CC[Elem].Neighbour[1], CC[Elem].Neighbour[2], CC[Elem].Neighbour[3]);
			}

		}
	}
	printf("\nNeighbours Found.");
	fclose(Fwrite);
	
    CalculateOutwordNormals(NoOfElements, CC, Node);
    
	CalculateVolume(NoOfElements, CC, Node);
	CalculateFaceArea(NoOfElements, CC, Node);
	FormStencil(NoOfElements, CC, Node);
	FindKs(NoOfElements, CC);
	//FindLSCoeffienents(NoOfElements, CC, Node);
	//Test(NoOfElements, CC);
	//PointsOnBody(NoOfNodes, Node);
}


void ReadNeutralFile(char *ElementFileName, int NoOfElements, int NoOfNodes, CELLDETAILS *CD, NODE *Node)
{
	
	FILE *FPElem, *FPBCell;
	unsigned int E_NUMNP, E_NELEM, E_NGRPS, E_NBSETS, E_NDFCD, E_NDFVL;
	char E_HEADNUT[100];
	int DomCount , BCellCount;
	
	typedef struct 
	{
		int FaceType[4];
	}FACE1;
	FACE1 *Face;
	

	int Elem, Id, t;
	int a, b;
	char *t1;

	unsigned int i;
	unsigned j;
	unsigned int ELEM, ELEMTYPE, FACE;
	char Code[CODE][7]={"INLET", "OUTLET", "TOP", "BOTTOM","FRONT", "BACK", "BODY"};

	DomCount = BCellCount = 0;
	if((Face =(FACE1*)malloc((NoOfElements)*sizeof(FACE1)))==NULL)
	{
		printf("Not Enough Memory for Allocating Nodes. \nProgram Terminated");
		exit(2);
	}


	if( (FPElem = fopen(ElementFileName,"rt") )==NULL)
	{
		printf("Unable to Open %s",ElementFileName);
		printf("Program Terminated.");
		exit(3);
	}

	printf("File %s Opened Succesfully\n", ElementFileName);

	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);
	t = fscanf(FPElem, "%d %d %d %d %d %d", &E_NUMNP, &E_NELEM, &E_NGRPS, &E_NBSETS, &E_NDFCD, &E_NDFVL);

	printf("Total number of Elements is %d\nTotal number of Nodes is %d\n",E_NELEM, E_NUMNP);
	
	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);
	t1 = fgets(E_HEADNUT,100, FPElem);
	


	for(Elem=0;Elem<NoOfNodes;Elem++)
	{
		t = fscanf(FPElem, "%d %lf %lf %lf", &Id, &Node[Elem].x, &Node[Elem].y, &Node[Elem].z);
		
		//printf("%d\t%lf\t%lf\t%lf\n", Id, Node[Elem].x, Node[Elem].y, Node[Elem].z );
		//if(Elem%1000==0)
		//getch();
		Node[Elem].Type = 0;
		for(a=0;a<MAXSHAREELEMENT;a++)
			Node[Elem].ShareElements[a] = 0;
		Node[Elem].ShareCount = 0;
		
	}
	
	t1 = fgets(E_HEADNUT,100, FPElem);
	//puts(E_HEADNUT);
	t1 = fgets(E_HEADNUT,100, FPElem);
	//puts(E_HEADNUT);
	t1 = fgets(E_HEADNUT,100, FPElem);
	//puts(E_HEADNUT);
	//getch();
	for(Elem=0;Elem<NoOfElements;Elem++)
	{
		t = fscanf(FPElem, "%d %d %d %d %d %d %d", &Id, &a, &b, &CD[Elem].Connect[0], &CD[Elem].Connect[1], &CD[Elem].Connect[2], &CD[Elem].Connect[3]);
		
		
		
		
		
		if(Node[CD[Elem].Connect[0]-1].ShareCount >= MAXSHAREELEMENT)
		{
			printf("Program cannot Proceed...\n Please Increase the \"MAXSHAREELEMENT\" in Main.h");
			printf("\nProgram Terminated...");
			exit(0);
		}
		Node[CD[Elem].Connect[0]-1].ShareElements[Node[CD[Elem].Connect[0]-1].ShareCount] = Elem;
		Node[CD[Elem].Connect[0]-1].ShareCount ++;
		
		if(Node[CD[Elem].Connect[1]-1].ShareCount >= MAXSHAREELEMENT)
		{
			printf("Program cannot Proceed...\n Please Increase the \"MAXSHAREELEMENT\" in Main.h");
			printf("\nProgram Terminated...");
			exit(0);
		}
		Node[CD[Elem].Connect[1]-1].ShareElements[Node[CD[Elem].Connect[1]-1].ShareCount] = Elem;
		Node[CD[Elem].Connect[1]-1].ShareCount ++;
		
		
		if(Node[CD[Elem].Connect[2]-1].ShareCount >= MAXSHAREELEMENT)
		{
			printf("Program cannot Proceed...\n Please Increase the \"MAXSHAREELEMENT\" in Main.h");
			printf("\nProgram Terminated...");
			exit(0);
		}
		Node[CD[Elem].Connect[2]-1].ShareElements[Node[CD[Elem].Connect[2]-1].ShareCount] = Elem;
		Node[CD[Elem].Connect[2]-1].ShareCount ++;
		
		
		if(Node[CD[Elem].Connect[3]-1].ShareCount >= MAXSHAREELEMENT)
		{
			printf("Program cannot Proceed...\n Please Increase the \"MAXSHAREELEMENT\" in Main.h");
			printf("\nProgram Terminated...");
			exit(0);
		}
		Node[CD[Elem].Connect[3]-1].ShareElements[Node[CD[Elem].Connect[3]-1].ShareCount] = Elem;
		Node[CD[Elem].Connect[3]-1].ShareCount ++;
		
		
		CD[Elem].Neighbour[0] = CD[Elem].Neighbour[2] = CD[Elem].Neighbour[3] =CD[Elem].Neighbour[1] = NEIGHUNAVAILABLE;
		CD[Elem].Type = 0;
		Face[Elem].FaceType[0] = Face[Elem].FaceType[1] = Face[Elem].FaceType[2] = Face[Elem].FaceType[3] = 0;
		//printf("%d\t %d\t %d\t %d\t %d\t %d\t %d\n", Id, a, b, CD[Elem].Connect[0], CD[Elem].Connect[1], CD[Elem].Connect[2], CD[Elem].Connect[3]);
		//getch();
	}
	
	a = 0;
	for(Elem=0;Elem<NoOfNodes;Elem++)
	{
		
		if (a<Node[Elem].ShareCount)
		{
			a = Node[Elem].ShareCount;
			//printf("\n%d\t%d\t%d",Elem,Node[Elem].ShareCount,a);
		}
	}
	printf("\nMax Share count for a node: %d", a);

	if (a>=MAXSHAREELEMENT)
	{
		printf("Program cannot Proceed...\n Please Increase the \"MAXSHAREELEMENT\" in Main.h");
		printf("\nProgram Terminated...");
		exit(0);
	}

	t1 = fgets(E_HEADNUT,100, FPElem);
	//puts(E_HEADNUT);
	t1 = fgets(E_HEADNUT,100, FPElem);
	//puts(E_HEADNUT);
	t1 = fgets(E_HEADNUT,100, FPElem);
	//puts(E_HEADNUT);
	t1 = fgets(E_HEADNUT,100, FPElem);
	//puts(E_HEADNUT);
	t1 = fgets(E_HEADNUT,100, FPElem);
	//puts(E_HEADNUT);
	t1 = fgets(E_HEADNUT,100, FPElem);
	//puts(E_HEADNUT);
	
	for(Elem=0;Elem<NoOfElements;Elem++)
	{
		t = fscanf(FPElem, "%d", &Id);
	}
	
	for(i=0;i<E_NBSETS;i++)
	{
		unsigned int ITYPE, NENTRY, NVALUES, IBCODE1;
		char NAME[20];

		t1 = fgets(E_HEADNUT,100, FPElem);
		t1 = fgets(E_HEADNUT,100, FPElem);
		t1 = fgets(E_HEADNUT,100, FPElem);
		t1 = fgets(E_HEADNUT,100, FPElem);

		//puts(E_HEADNUT);
		sscanf(E_HEADNUT,"%s %d %d %d %d",NAME, &ITYPE, &NENTRY, &NVALUES, &IBCODE1);
		//printf("\n%s %d %d %d %d ",NAME, ITYPE, NENTRY, NVALUES, IBCODE1);
		for(j=0;j<strlen(NAME);j++)
		{
			NAME[j]= toupper(NAME[j]);
		}
		for(j=0;j<CODE;j++)
		{
			if(strcmp(NAME,Code[j])==0)
			{
				IBCODE1 = j+1;
				break;
			}
		}		
		for(j=0;j<NENTRY;j++)
		{
			int a, b, c, d;
			t = fscanf(FPElem,"%d %d %d", &ELEM, &ELEMTYPE, &FACE);
			ELEM = ELEM-1;

			CD[ELEM].Type = CD[ELEM].Type * 10;
			CD[ELEM].Type = CD[ELEM].Type + IBCODE1;
			Face[ELEM].FaceType[FACE-1] = IBCODE1;
			/*if(ELEM == 0 ||ELEM == 2 ||ELEM == 3 ||ELEM == 289 ||ELEM == 291 ||ELEM == 396 ||ELEM == 397 ||ELEM == 398 ||ELEM == 1327 ||ELEM == 1328 ||ELEM == 6039 ||ELEM == 6040 )
			{
				a = a;
			}*/
		
			a = CD[ELEM].Connect[0];
			b = CD[ELEM].Connect[1];
			c = CD[ELEM].Connect[2];
			d = CD[ELEM].Connect[3];
			
			switch(FACE)
			{
			case 1:
				Node[a-1].Type = IBCODE1;
				Node[b-1].Type = IBCODE1;
				Node[c-1].Type = IBCODE1;
				break;

			case 2:
				Node[a-1].Type = IBCODE1;
				Node[b-1].Type = IBCODE1;
				Node[d-1].Type = IBCODE1;
				break;

			case 3:
				Node[b-1].Type = IBCODE1;
				Node[c-1].Type = IBCODE1;
				Node[d-1].Type = IBCODE1;
				break;

			case 4:
				Node[a-1].Type = IBCODE1;
				Node[c-1].Type = IBCODE1;
				Node[d-1].Type = IBCODE1;
				break;

			default:
				printf("Default Case Entering...");
			}
		}
	}

	fclose(FPElem);
	if((NoOfNodes != E_NUMNP) ||(NoOfElements != E_NELEM))
	{
		printf("Miss match in the end of presolver File reading.\nProgram Terminated");
		exit(1);
	}

	
	for(Elem=0; Elem<NoOfElements;Elem++)
	{
		int type1, type2, type3, type4;
		int Con1, Con2, Con3, Con4;
		//int TCon1, TCon2, TCon3, TCon4;
		Con1 = CD[Elem].Connect[0];
		Con2 = CD[Elem].Connect[1];
		Con3 = CD[Elem].Connect[2];
		Con4 = CD[Elem].Connect[3];
		type1 = Face[Elem].FaceType[0];
		type2 = Face[Elem].FaceType[1];
		type3 = Face[Elem].FaceType[2];
		type4 = Face[Elem].FaceType[3];

		switch(CD[Elem].Type)
		{
		case DOM:
			break;
		case INLET:
		case OUTLET:
		case BODY:
		case TOP:
		case BOTTOM:
		case FRONT:
		case BACK:
			if(Face[Elem].FaceType[1] !=0)
			{
				CD[Elem].Connect[0] = Con1;
				CD[Elem].Connect[1] = Con2;
				CD[Elem].Connect[2] = Con4;
				CD[Elem].Connect[3] = Con3;

				type1 = Face[Elem].FaceType[1];
				Face[Elem].FaceType[1] = 0;
			}
			if(Face[Elem].FaceType[2] !=0)
			{
				CD[Elem].Connect[0] = Con2;
				CD[Elem].Connect[1] = Con3;
				CD[Elem].Connect[2] = Con4;
				CD[Elem].Connect[3] = Con1;
				type1 = Face[Elem].FaceType[2];
				Face[Elem].FaceType[2] = 0;
			}
			if(Face[Elem].FaceType[3] !=0)
			{
				CD[Elem].Connect[0] = Con1;
				CD[Elem].Connect[1] = Con3;
				CD[Elem].Connect[2] = Con4;
				CD[Elem].Connect[3] = Con2;
				type1 = Face[Elem].FaceType[3];
				Face[Elem].FaceType[3] = 0;
			}
			Face[Elem].FaceType[0] = type1;
			break;
		case 13:
		case 14:
		case 15:
		case 16:
		case 23:
		case 24:
		case 25:
		case 26:
		case 35:
		case 36:
		case 45:
		case 46:
			if(Face[Elem].FaceType[0] !=0 && Face[Elem].FaceType[2] !=0) // Face 1 & 3
			{
				CD[Elem].Connect[0] = Con2;
				CD[Elem].Connect[1] = Con3;
				CD[Elem].Connect[2] = Con1;
				CD[Elem].Connect[3] = Con4;

				Face[Elem].FaceType[0] = type1;
				Face[Elem].FaceType[1] = type3;
				Face[Elem].FaceType[2] = type4;
				Face[Elem].FaceType[3] = type2;
			}

			if(Face[Elem].FaceType[0] !=0 && Face[Elem].FaceType[3] !=0) // Face 1 & 4
			{
				CD[Elem].Connect[0] = Con1;
				CD[Elem].Connect[1] = Con3;
				CD[Elem].Connect[2] = Con2;
				CD[Elem].Connect[3] = Con4;

				Face[Elem].FaceType[0] = type1;
				Face[Elem].FaceType[1] = type4;
				Face[Elem].FaceType[2] = type3;
				Face[Elem].FaceType[3] = type2;
			}

			if(Face[Elem].FaceType[1] !=0 && Face[Elem].FaceType[2] !=0) // Face 2 & 3
			{
				CD[Elem].Connect[0] = Con2;
				CD[Elem].Connect[1] = Con4;
				CD[Elem].Connect[2] = Con3;
				CD[Elem].Connect[3] = Con1;

				Face[Elem].FaceType[0] = type3;
				Face[Elem].FaceType[1] = type2;
				Face[Elem].FaceType[2] = type4;
				Face[Elem].FaceType[3] = type1;
			}

			if(Face[Elem].FaceType[1] !=0 && Face[Elem].FaceType[3] !=0) // Face 2 & 4
			{
				CD[Elem].Connect[0] = Con1;
				CD[Elem].Connect[1] = Con4;
				CD[Elem].Connect[2] = Con3;
				CD[Elem].Connect[3] = Con2;

				Face[Elem].FaceType[0] = type4;
				Face[Elem].FaceType[1] = type2;
				Face[Elem].FaceType[2] = type3;
				Face[Elem].FaceType[3] = type1;
			}

			if(Face[Elem].FaceType[2] !=0 && Face[Elem].FaceType[3] !=0) // Face 3 & 4
			{
				CD[Elem].Connect[0] = Con3;
				CD[Elem].Connect[1] = Con4;
				CD[Elem].Connect[2] = Con2;
				CD[Elem].Connect[3] = Con1;

				Face[Elem].FaceType[0] = type3;
				Face[Elem].FaceType[1] = type4;
				Face[Elem].FaceType[2] = type2;
				Face[Elem].FaceType[3] = type1;
			}
			break;
		case 135:
		case 136:
		case 145:
		case 146:
		case 235:
		case 236:
		case 245:
		case 246:
			if(Face[Elem].FaceType[0] == 0)
			{
				CD[Elem].Connect[0] = Con1;
				CD[Elem].Connect[1] = Con4;
				CD[Elem].Connect[2] = Con3;
				CD[Elem].Connect[3] = Con2;

				type1 = Face[Elem].FaceType[0];
				Face[Elem].FaceType[0] = Face[Elem].FaceType[3];
				Face[Elem].FaceType[3] = type1;
			}
			if(Face[Elem].FaceType[1] == 0)
			{
				CD[Elem].Connect[0] = Con1;
				CD[Elem].Connect[1] = Con3;
				CD[Elem].Connect[2] = Con2;
				CD[Elem].Connect[3] = Con4;
				type1 = Face[Elem].FaceType[1];
				Face[Elem].FaceType[1] = Face[Elem].FaceType[3];
				Face[Elem].FaceType[3] = type1;
				
			}
			if(Face[Elem].FaceType[2] == 0)
			{
				CD[Elem].Connect[0] = Con2;
				CD[Elem].Connect[1] = Con1;
				CD[Elem].Connect[2] = Con3;
				CD[Elem].Connect[3] = Con4;
				type1 = Face[Elem].FaceType[2];
				Face[Elem].FaceType[2] = Face[Elem].FaceType[3];
				Face[Elem].FaceType[3] = type1;
			}
			break;
		case 31:
			CD[Elem].Type = 13;
			Elem--;
			break;
		case 41:
			CD[Elem].Type = 14;
			Elem--;
			break;
		case 51:
			CD[Elem].Type = 15;
			Elem--;
			break;
		case 61:
			CD[Elem].Type = 16;
			Elem--;
			break;
		case 32:
			CD[Elem].Type = 23;
			Elem--;
			break;
		case 42:
			CD[Elem].Type = 24;
			Elem--;
			break;
		case 52:
			CD[Elem].Type = 25;
			Elem--;
			break;
		case 62:
			CD[Elem].Type = 26;
			Elem--;
			break;
		case 53:
			CD[Elem].Type = 35;
			Elem--;
			break;
		case 54:
			CD[Elem].Type = 45;
			Elem--;
			break;
		case 63:
			CD[Elem].Type = 36;
			Elem--;
			break;
		case 64:
			CD[Elem].Type = 46;
			Elem--;
			break;
		case 153:
		case 315:
		case 351:
		case 513:
		case 531:
			CD[Elem].Type = 135;
			Elem--;
			break;
		case 163:
		case 316:
		case 361:
		case 613:
		case 631:
			CD[Elem].Type = 136;
			Elem--;
			break;
		case 164:
		case 416:
		case 461:
		case 614:
		case 641:
			CD[Elem].Type = 146;
			Elem--;
			break;
		case 154:
		case 415:
		case 451:
		case 514:
		case 541:
			CD[Elem].Type = 145;
			Elem--;
			break;
		case 253:
		case 325:
		case 352:
		case 523:
		case 532:
			CD[Elem].Type = 235;
			Elem--;
			break;
		case 263:
		case 326:
		case 362:
		case 623:
		case 632:
			CD[Elem].Type = 236;
			Elem--;
			break;
		case 264:
		case 426:
		case 462:
		case 624:
		case 642:
			CD[Elem].Type = 246;
			Elem--;
			break;
		case 254:
		case 425:
		case 452:
		case 524:
		case 542:
			CD[Elem].Type = 245;
			Elem--;
			break;
		default:
			printf("Default Case Entering in Read Neutral File Function in Data Organising...\n");
			printf("Program Terminated...");
			exit(0);
		}
	}
	for(Elem=0;Elem<NoOfElements;Elem++)
	{
		switch(CD[Elem].Type)
		{
		case DOM:
			DomCount++;
			break;
		case TOP:
		case BOTTOM:
		case INLET:
		case OUTLET:
		case FRONT:
		case BACK:
		case BODY:
		case INTOP:
		case INBOT:
		case INFRO:
		case INBAK:
		case OUTTOP: 
		case OUTBOT: 
		case OUTFRO:
		case OUTBAK:
		case TOPFRO:
		case TOPBAK:
		case BOTFRO:
		case BOTBAK:	
		case INTOPFRO:
		case INTOPBAK:
		case INBOTFRO:
		case INBOTBAK:
		case OUTTOPFRO:
		case OUTTOPBAK:
		case OUTBOTFRO:
		case OUTBOTBAK:
			BCellCount++;
			break;
		}
	}
	switch (COMP)
	{
	case 1:
		sprintf(ElementFileName,"Mesh\\BoundElems.dat");
		break;

	case 2:
		sprintf(ElementFileName,".//Mesh//BoundElems.dat");
		break;

	default:
		printf("\n OS version and Compiler Version Error.\nProgram Terminated.\n\n");
		exit(0);
	}
	if( (FPBCell = fopen(ElementFileName,"w+") )==NULL)
	{
		printf("Unable to Open %s",ElementFileName);
		printf("Program Terminated.");
		exit(3);
	}
	fprintf(FPBCell,"%d\n",BCellCount);
	for(Elem=0;Elem<NoOfElements;Elem++)
	{
		switch(CD[Elem].Type)
		{
		case DOM:
			break;
		case TOP:
		case BOTTOM:
		case INLET:
		case OUTLET:
		case FRONT:
		case BACK:
		case BODY:
		case INTOP:
		case INBOT:
		case INFRO:
		case INBAK:
		case OUTTOP: 
		case OUTBOT: 
		case OUTFRO:
		case OUTBAK:
		case TOPFRO:
		case TOPBAK:
		case BOTFRO:
		case BOTBAK:	
		case INTOPFRO:
		case INTOPBAK:
		case INBOTFRO:
		case INBOTBAK:
		case OUTTOPFRO:
		case OUTTOPBAK:
		case OUTBOTFRO:
		case OUTBOTBAK:
			fprintf(FPBCell,"%d\t%d\t%d\t%d\t%d\t%d\n",Elem, CD[Elem].Type,Face[Elem].FaceType[0],Face[Elem].FaceType[1],Face[Elem].FaceType[2],Face[Elem].FaceType[3]);
			break;
		}
	}
	free(Face);
	fclose(FPBCell);
}

void WritePLTFILE(int NoOfElements, int NoOfNodes, CELLDETAILS *CC, NODE *Node)
{
	FILE *Fwrite;
	int i;

	char Filename[100];
	sprintf(Filename,"CellCenteredPLT.plt");

	if( (Fwrite = fopen(Filename,"wt")) == NULL)
	{
		printf("\nUnable to open Plt File");
		return;
	}

	i=34;

	fprintf(Fwrite,"TITLE = %c2D Flow Past Circular Cylinder%c\n",i,i);
	
	fprintf(Fwrite, "VARIABLES = \"X\", \"Y\",\"Z\",\"Type\"\n");
	fprintf(Fwrite, "ZONE N=%d, E=%d, DATAPACKING=BLOCK, VARLOCATION=(4=CELLCENTERED),ZONETYPE=FETETRAHEDRON\n",NoOfNodes,NoOfElements);

		
	for(i=0;i<NoOfNodes;i++)
	{
		fprintf(Fwrite,"%lf\n", Node[i].x);
	}
	fprintf(Fwrite,"\n\n\n");
	for(i=0;i<NoOfNodes;i++)
	{
		fprintf(Fwrite,"%lf\n", Node[i].y);
	}
	fprintf(Fwrite,"\n\n\n");
	for(i=0;i<NoOfNodes;i++)
	{
		fprintf(Fwrite,"%lf\n", Node[i].z);
	}
	fprintf(Fwrite,"\n\n\n");
	
	for(i=0;i<NoOfElements;i++)
	{
		fprintf(Fwrite,"%d\n", CC[i].Type);
	}
	
	

	fprintf(Fwrite,"\n\n");
	for(i=0; i<NoOfElements; i++)
	{
		fprintf(Fwrite,"%d\t%d\t%d\t%d\n", CC[i].Connect[0], CC[i].Connect[1], CC[i].Connect[2], CC[i].Connect[3]);
	}
	
	fclose(Fwrite);
	//exit(0);
}

void WriteNodalPLTFILE(int NoOfElements, int NoOfNodes, CELLDETAILS *CC, NODE *Node)
{
	FILE *Fwrite;
	int i;

	char Filename[100];
	sprintf(Filename,"PLTNODE.plt");

	if( (Fwrite = fopen(Filename,"wt")) == NULL)
	{
		printf("\nUnable to open Plt File");
		return;
	}

	i=34;

	fprintf(Fwrite,"TITLE = %c2D Flow Past Circular Cylinder%c\n",i,i);
	
	fprintf(Fwrite, "VARIABLES = \"X\", \"Y\",\"Z\",\"Type\"\n");
	fprintf(Fwrite, "ZONE N=%d, E=%d, DATAPACKING=POINT, ZONETYPE=FETETRAHEDRON\n",NoOfNodes,NoOfElements);

		
	for(i=0;i<NoOfNodes;i++)
	{
		fprintf(Fwrite,"%lf\t%lf\t%lf\t%d\n", Node[i].x, Node[i].y, Node[i].z, Node[i].Type);
	}
		

	fprintf(Fwrite,"\n\n");
	for(i=0; i<NoOfElements; i++)
	{
		fprintf(Fwrite,"%d\t%d\t%d\t%d\n", CC[i].Connect[0], CC[i].Connect[1], CC[i].Connect[2], CC[i].Connect[3]);
	}
	
	fclose(Fwrite);
}

void FindNeighbour(int Element, int NoOfElements, CELLDETAILS *CC)
{
	int i,j,k;
	int count = 0,Assigned = 0;
	int face[4];
	printf("Element %d of %d\n",Element, NoOfElements);
	for(i=0;i<NoOfElements;i++)
	{
		if(i != Element)
		{
			face[0] = face[1] = face[2] = face[3] =0;
			for(j=0;j<4;j++)
			{
				for(k=0;k<4;k++)
				{
					if(CC[i].Connect[j] == CC[Element].Connect[k])
					{
						count++;
						face[k]=1;
					}
				}
			}
			if(count == 3)
			{
				if(face[0] == 0)
					CC[Element].Neighbour[CELL_C] = i;  // node a absent → Face 3 (b,c,d)
				if(face[1] == 0)
					CC[Element].Neighbour[CELL_D] = i;  // node b absent → Face 4 (a,c,d)
				if(face[2] == 0)
					CC[Element].Neighbour[CELL_B] = i;  // node c absent → Face 2 (a,b,d)
				if(face[3] == 0)
					CC[Element].Neighbour[CELL_A] = i;  // node d absent → Face 1 (a,b,c)
				Assigned ++;
			}
			count = 0;
			if(Assigned == 4)
				i = NoOfElements;
		}
	}
}

void CalculateOutwordNormals(int NoOfElements, CELLDETAILS *CC, NODE *Node)
{
	printf("\nCompuring Outward Normals for Each Face of Each Tetrahedron...\n");
    for (int i = 0; i < NoOfElements; i++) {
        // Compute the centroid of the tetrahedron
        double centroid[3] = {
            (Node[CC[i].Connect[0]-1].x + Node[CC[i].Connect[1]-1].x + Node[CC[i].Connect[2]-1].x + Node[CC[i].Connect[3]-1].x) / 4.0,
            (Node[CC[i].Connect[0]-1].y + Node[CC[i].Connect[1]-1].y + Node[CC[i].Connect[2]-1].y + Node[CC[i].Connect[3]-1].y) / 4.0,
            (Node[CC[i].Connect[0]-1].z + Node[CC[i].Connect[1]-1].z + Node[CC[i].Connect[2]-1].z + Node[CC[i].Connect[3]-1].z) / 4.0
        };

        // Loop through each face of the tetrahedron
        // Face convention (Gambit): Face1(CELL_A)={a,b,c}, Face2(CELL_B)={a,b,d}, Face3(CELL_C)={b,c,d}, Face4(CELL_D)={a,c,d}
        // Node indices: a=Connect[0], b=Connect[1], c=Connect[2], d=Connect[3]
        static const int faceNodes[4][3] = {
            {0, 1, 2},  // CELL_A: Face 1 = a,b,c
            {0, 1, 3},  // CELL_B: Face 2 = a,b,d
            {1, 2, 3},  // CELL_C: Face 3 = b,c,d
            {0, 2, 3}   // CELL_D: Face 4 = a,c,d
        };
        for (int face = 0; face < MAXFACES; face++) {
            // Get the indices of the three nodes forming the current face
            int n1 = CC[i].Connect[faceNodes[face][0]]-1;
            int n2 = CC[i].Connect[faceNodes[face][1]]-1;
            int n3 = CC[i].Connect[faceNodes[face][2]]-1;

            // Compute two edge vectors of the face
            double edge1[3] = {Node[n2].x - Node[n1].x, Node[n2].y - Node[n1].y, Node[n2].z - Node[n1].z};
            double edge2[3] = {Node[n3].x - Node[n1].x, Node[n3].y - Node[n1].y, Node[n3].z - Node[n1].z};

            // Compute the cross product of the two edge vectors
            double normal[3];
            normal[Nx] = edge1[Ny] * edge2[Nz] - edge1[Nz] * edge2[Ny];
            normal[Ny] = edge1[Nz] * edge2[Nx] - edge1[Nx] * edge2[Nz];
            normal[Nz] = edge1[Nx] * edge2[Ny] - edge1[Ny] * edge2[Nx];

            // Normalize the normal vector
            double magnitude = sqrt(normal[Nx] * normal[Nx] + normal[Ny] * normal[Ny] + normal[Nz] * normal[Nz]);
            if (magnitude != 0) {
                normal[Nx] /= magnitude;
                normal[Ny] /= magnitude;
                normal[Nz] /= magnitude;
            }

            // Compute a vector from the centroid to a point on the face (e.g., Node[n1])
            double faceToCentroid[3] = {
                centroid[0] - Node[n1].x,
                centroid[1] - Node[n1].y,
                centroid[2] - Node[n1].z
            };

            // Compute the dot product of the face normal and the faceToCentroid vector
            double dotProduct = normal[Nx] * faceToCentroid[0] + normal[Ny] * faceToCentroid[1] + normal[Nz] * faceToCentroid[2];

            // faceToCentroid points inward (face node → centroid).
            // If dotProduct > 0, the normal points inward too → flip it to get outward normal.
            if (dotProduct > 0) {
                normal[Nx] = -normal[Nx];
                normal[Ny] = -normal[Ny];
                normal[Nz] = -normal[Nz];
            }

            // Store the normal vector in CC[i].Normals
            CC[i].Normals[face][Nx] = normal[Nx];
            CC[i].Normals[face][Ny] = normal[Ny];
            CC[i].Normals[face][Nz] = normal[Nz];
        }
    }
}


void CalculateVolume(int NoOfElements, CELLDETAILS *CC, NODE *Node)
{
	for (int i = 0; i < NoOfElements; i++) {
		int n1 = CC[i].Connect[0]-1;
		int n2 = CC[i].Connect[1]-1;
		int n3 = CC[i].Connect[2]-1;
		int n4 = CC[i].Connect[3]-1;

		double x1 = Node[n1].x, y1 = Node[n1].y, z1 = Node[n1].z;
		double x2 = Node[n2].x, y2 = Node[n2].y, z2 = Node[n2].z;
		double x3 = Node[n3].x, y3 = Node[n3].y, z3 = Node[n3].z;
		double x4 = Node[n4].x, y4 = Node[n4].y, z4 = Node[n4].z;

		double ax = x2 - x1, ay = y2 - y1, az = z2 - z1;
		double bx = x3 - x1, by = y3 - y1, bz = z3 - z1;
		double cx = x4 - x1, cy = y4 - y1, cz = z4 - z1;

		CC[i].Volume = fabs(
			ax * (by * cz - bz * cy)
			- ay * (bx * cz - bz * cx)
			+ az * (bx * cy - by * cx)
		) / 6.0;
	}
}

void CalculateFaceArea(int NoOfElements, CELLDETAILS *CC, NODE *Node)
{
	for (int i = 0; i < NoOfElements; i++) {
		// Face convention (Gambit): Face1(CELL_A)={a,b,c}, Face2(CELL_B)={a,b,d}, Face3(CELL_C)={b,c,d}, Face4(CELL_D)={a,c,d}
		// Node indices: a=Connect[0], b=Connect[1], c=Connect[2], d=Connect[3]
		static const int faceNodes[4][3] = {
			{0, 1, 2},  // CELL_A: Face 1 = a,b,c
			{0, 1, 3},  // CELL_B: Face 2 = a,b,d
			{1, 2, 3},  // CELL_C: Face 3 = b,c,d
			{0, 2, 3}   // CELL_D: Face 4 = a,c,d
		};
		for (int face = 0; face < MAXFACES; face++) {
			int n1 = CC[i].Connect[faceNodes[face][0]]-1;
			int n2 = CC[i].Connect[faceNodes[face][1]]-1;
			int n3 = CC[i].Connect[faceNodes[face][2]]-1;

			
			double x1 = Node[n1].x, y1 = Node[n1].y, z1 = Node[n1].z;
			double x2 = Node[n2].x, y2 = Node[n2].y, z2 = Node[n2].z;
			double x3 = Node[n3].x, y3 = Node[n3].y, z3 = Node[n3].z;

			

			// Compute edge vectors
			double edge1[3] = {x2 - x1, y2 - y1, z2 - z1};
			double edge2[3] = {x3 - x1, y3 - y1, z3 - z1};

			// Compute the cross product of the edge vectors
			double cross[3];
			cross[Nx] = edge1[Ny] * edge2[Nz] - edge1[Nz] * edge2[Ny];
			cross[Ny] = edge1[Nz] *	edge2[Nx] - edge1[Nx] * edge2[Nz];
			cross[Nz] = edge1[Nx] * edge2[Ny] - edge1[Ny] * edge2[Nx];

			// Compute the area of the triangle (half the magnitude of the cross product)
			double area = 0.5 * sqrt(cross[Nx] * cross[Nx] + cross[Ny] * cross[Ny] + cross[Nz] * cross[Nz]);
			
			CC[i].FaceArea[face] = area;	
		}
	}
}

static int FaceContainsNode(const int faceNodes[3], int nodeId)
{
	return (faceNodes[0] == nodeId || faceNodes[1] == nodeId || faceNodes[2] == nodeId);
}

static int FindFaceByEdgeAndExcludedNode(CELLDETAILS *CC, int elem, int edgeNode1, int edgeNode2, int excludedNode)
{
	static const int localFaceNodes[4][3] = {
		{0, 1, 2},
		{0, 1, 3},
		{1, 2, 3},
		{0, 2, 3}
	};

	for (int face = 0; face < MAXFACES; face++) {
		int fNodes[3] = {
			CC[elem].Connect[localFaceNodes[face][0]],
			CC[elem].Connect[localFaceNodes[face][1]],
			CC[elem].Connect[localFaceNodes[face][2]]
		};

		if (FaceContainsNode(fNodes, edgeNode1)
			&& FaceContainsNode(fNodes, edgeNode2)
			&& !FaceContainsNode(fNodes, excludedNode))
		{
			return face;
		}
	}

	return UNAVAILABLEDATA;
}

void FormStencil(int NoOfElements, CELLDETAILS *CC, NODE *Node)
{
	printf("\nForming Stencil for Each Cell...\n");

	for (int i = 0; i < NoOfElements; i++) {
		CC[i].FaceE = CC[i].FaceF = CC[i].FaceG = UNAVAILABLEDATA;
		CC[i].FaceH = CC[i].FaceI = CC[i].FaceJ = UNAVAILABLEDATA;
		CC[i].FaceK = CC[i].FaceL = CC[i].FaceM = UNAVAILABLEDATA;
		CC[i].FaceN = CC[i].FaceO = CC[i].FaceP = UNAVAILABLEDATA;
		CC[i].CellE = CC[i].CellF = CC[i].CellG = NEIGHUNAVAILABLE;
		CC[i].CellH = CC[i].CellI = CC[i].CellJ = NEIGHUNAVAILABLE;
		CC[i].CellK = CC[i].CellL = CC[i].CellM = NEIGHUNAVAILABLE;
		CC[i].CellN = CC[i].CellO = CC[i].CellQ = NEIGHUNAVAILABLE;

		/*
		 * Stencil convention from stecil.png:
		 * P = (1,2,3,4), A = neighbour across Face A(1,2,3)
		 * e=(1,2,5), f=(2,3,5), g=(1,3,5)
		 */
		int cellA = CC[i].Neighbour[CELL_A];
		if (cellA >= 0) {
			int n1 = CC[i].Connect[0];
			int n2 = CC[i].Connect[1];
			int n3 = CC[i].Connect[2];

			CC[i].FaceE = FindFaceByEdgeAndExcludedNode(CC, cellA, n1, n2, n3);
			CC[i].FaceF = FindFaceByEdgeAndExcludedNode(CC, cellA, n2, n3, n1);
			CC[i].FaceG = FindFaceByEdgeAndExcludedNode(CC, cellA, n1, n3, n2);

			if (CC[i].FaceE >= 0)
				CC[i].CellE = CC[cellA].Neighbour[CC[i].FaceE];
			if (CC[i].FaceF >= 0)
				CC[i].CellF = CC[cellA].Neighbour[CC[i].FaceF];
			if (CC[i].FaceG >= 0)
				CC[i].CellG = CC[cellA].Neighbour[CC[i].FaceG];
		}

		/*
		 * Stencil convention from stecil.png:
		 * B = neighbour across Face B(1,2,4)
		 * h=(1,4,6), i=(2,4,6), j=(1,2,6)
		 */
		int cellB = CC[i].Neighbour[CELL_B];
		if (cellB >= 0) {
			int n1 = CC[i].Connect[0];
			int n2 = CC[i].Connect[1];
			int n4 = CC[i].Connect[3];

			CC[i].FaceH = FindFaceByEdgeAndExcludedNode(CC, cellB, n1, n4, n2);
			CC[i].FaceI = FindFaceByEdgeAndExcludedNode(CC, cellB, n2, n4, n1);
			CC[i].FaceJ = FindFaceByEdgeAndExcludedNode(CC, cellB, n1, n2, n4);

			if (CC[i].FaceH >= 0)
				CC[i].CellH = CC[cellB].Neighbour[CC[i].FaceH];
			if (CC[i].FaceI >= 0)
				CC[i].CellI = CC[cellB].Neighbour[CC[i].FaceI];
			if (CC[i].FaceJ >= 0)
				CC[i].CellJ = CC[cellB].Neighbour[CC[i].FaceJ];
		}

		/*
		 * Stencil convention from stencil.png:
		 * C = neighbour across Face C(2,3,4)
		 * k=(2,3,7), l=(2,4,7), m=(3,4,7)
		 */
		int cellC = CC[i].Neighbour[CELL_C];
		if (cellC >= 0) {
			int n2 = CC[i].Connect[1];
			int n3 = CC[i].Connect[2];
			int n4 = CC[i].Connect[3];

			CC[i].FaceK = FindFaceByEdgeAndExcludedNode(CC, cellC, n2, n3, n4);
			CC[i].FaceL = FindFaceByEdgeAndExcludedNode(CC, cellC, n2, n4, n3);
			CC[i].FaceM = FindFaceByEdgeAndExcludedNode(CC, cellC, n3, n4, n2);

			if (CC[i].FaceK >= 0)
				CC[i].CellK = CC[cellC].Neighbour[CC[i].FaceK];
			if (CC[i].FaceL >= 0)
				CC[i].CellL = CC[cellC].Neighbour[CC[i].FaceL];
			if (CC[i].FaceM >= 0)
				CC[i].CellM = CC[cellC].Neighbour[CC[i].FaceM];
		}

		/*
		 * Stencil convention from stencil.png:
		 * D = neighbour across Face D(1,3,4)
		 * n=(3,4,8), o=(1,4,8), p=(1,3,8)
		 */
		int cellD = CC[i].Neighbour[CELL_D];
		if (cellD >= 0) {
			int n1 = CC[i].Connect[0];
			int n3 = CC[i].Connect[2];
			int n4 = CC[i].Connect[3];

			CC[i].FaceN = FindFaceByEdgeAndExcludedNode(CC, cellD, n3, n4, n1);
			CC[i].FaceO = FindFaceByEdgeAndExcludedNode(CC, cellD, n1, n4, n3);
			CC[i].FaceP = FindFaceByEdgeAndExcludedNode(CC, cellD, n1, n3, n4);

			if (CC[i].FaceN >= 0)
				CC[i].CellN = CC[cellD].Neighbour[CC[i].FaceN];
			if (CC[i].FaceO >= 0)
				CC[i].CellO = CC[cellD].Neighbour[CC[i].FaceO];
			if (CC[i].FaceP >= 0)
				CC[i].CellQ = CC[cellD].Neighbour[CC[i].FaceP];
		}
	}

}

void FindKs(int NoOfElements, CELLDETAILS *CC)
{
	printf("\nFinding stencil Coefficients (Ks) for Each Cell...\n");
	int Elem;
	for(Elem=0;Elem<NoOfElements;Elem++)
	{
		switch(CC[Elem].Type)
		{
		case DOM:
			
			break;
		case TOP:
		case BOTTOM:
		case INLET:
		case OUTLET:
		case FRONT:
		case BACK:
		case BODY:

		case INTOP:
		case INBOT:
		case INFRO:
		case INBAK:
		case OUTTOP: 
		case OUTBOT: 
		case OUTFRO:
		case OUTBAK:
		case TOPFRO:
		case TOPBAK:
		case BOTFRO:
		case BOTBAK:	
		
		case INTOPFRO:
		case INTOPBAK:
		case INBOTFRO:
		case INBOTBAK:
		case OUTTOPFRO:
		case OUTTOPBAK:
		case OUTBOTFRO:
		case OUTBOTBAK:
			break;
		}
	}
}