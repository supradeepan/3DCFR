#include"PreSolver.h"
#include"Main.h"
#include<math.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<process.h>
#include<conio.h>
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
    /*
	CalculateVolume(NoOfElements, CC, Node);
	CalculateFaceArea(NoOfElements, CC, Node);
	FormStencil(NoOfElements, CC, Node);
	FindKs(NoOfElements, CC);
	FindLSCoeffienents(NoOfElements, CC, Node);
	Test(NoOfElements, CC);
	//PointsOnBody(NoOfNodes, Node);

    */
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
			if(IBCODE1 == 3)
			{
				if(a==524 || b ==524|| c ==524|| c ==524)
				{
					a = a;
				}
			}
			/*
			if(IBCODE1 != OUTLET)
				CD[ELEM].Neighbour[FACE-1] = ELEM;
			else 
				CD[ELEM].Neighbour[FACE-1] = NEIGHUNAVAILABLE;
				*/
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
					CC[Element].Neighbour[CELL_C] = i;
				if(face[1] == 0)
					CC[Element].Neighbour[CELL_D] = i;
				if(face[2] == 0)
					CC[Element].Neighbour[CELL_B] = i;
				if(face[3] == 0)
					CC[Element].Neighbour[CELL_A] = i;
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
    for (int i = 0; i < NoOfElements; i++) {
        // Compute the centroid of the tetrahedron
        double centroid[3] = {
            (Node[CC[i].Connect[0]].x + Node[CC[i].Connect[1]].x + Node[CC[i].Connect[2]].x + Node[CC[i].Connect[3]].x) / 4.0,
            (Node[CC[i].Connect[0]].y + Node[CC[i].Connect[1]].y + Node[CC[i].Connect[2]].y + Node[CC[i].Connect[3]].y) / 4.0,
            (Node[CC[i].Connect[0]].z + Node[CC[i].Connect[1]].z + Node[CC[i].Connect[2]].z + Node[CC[i].Connect[3]].z) / 4.0
        };

        // Loop through each face of the tetrahedron
        for (int face = 0; face < MAXFACES; face++) {
            // Get the indices of the three nodes forming the current face
            int n1 = CC[i].Connect[(face + 1) % MAXNODES];
            int n2 = CC[i].Connect[(face + 2) % MAXNODES];
            int n3 = CC[i].Connect[(face + 3) % MAXNODES];

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

            // If the dot product is negative, the normal is pointing inward; reverse it
            if (dotProduct < 0) {
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
