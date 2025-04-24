/*om Sri Sai Ram
 * Main.c
 * 3DCFRUNS
 * Created on: 02-Oct-2012
 * Author: supradeepan
 */

 #include<stdio.h>
 #include<math.h>
 #include<stdlib.h>
 #include<ctype.h>
 #include<string.h>
 #include<time.h>
 #include"Main.h"
 #include"PreSolver.h"
 #include"Solver.h"
 #include"PostSolver.h"
 
 
 int main()
 {
     time_t start,end;
     NODE *Node;
     CELLDETAILS *CC;
     CELLDATAS *CD,*CDO; 
     BOUNDARYELEMENTS *BoundElems;
     double dif;
     int NoOfElements,NoOfNodes, NoOfBoundaryElements;
     int Iter;
     
     time(&start);
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //PreProcessing...
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
     BoundElems = BoundaryElements(&NoOfBoundaryElements);
     CalcBoundaryElementsLSCoeffs(NoOfBoundaryElements, BoundElems, CC, Node);
 
     Iter = RestartCheck(NoOfElements, NoOfNodes);
     if(Iter)
     {
         FILE *Restart;
         char FileRestart[100];
         int Elem;
         printf("\n%d Iter Restart File Found...",Iter);
         //getch();
         switch (COMP)
         {
         case 1:
             sprintf(FileRestart,"Restart\\CD.dat");
             break;
         case 2:
             sprintf(FileRestart,"//home//supradeepan//PCFR_TaylorDerivative//Restart//CD.dat");
             break;
         default:
             printf("\n OS version and Compiler Version Error.\nProgram Terminated.\n\n");
             exit(0);
         }
         
         if( (Restart = fopen(FileRestart,"r+")) == NULL)
         {
             printf("\nUnable to open Restart File\nProgram Terminated\n");
             exit(0);
         }
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         for(Elem=0; Elem<NoOfElements; Elem++)
         {
             
             fscanf(Restart,"%lf\t",&CD[Elem].CCP);
             fscanf(Restart,"%lf\t",&CD[Elem].CCVel[U]);
             fscanf(Restart,"%lf\t",&CD[Elem].CCVel[V]);
             fscanf(Restart,"%lf\t",&CD[Elem].CCVel[W]);
             
         }
         fclose(Restart);
         for(Elem=0; Elem<NoOfElements; Elem++)
         {
             CDO[Elem].CCP = CD[Elem].CCP;
             CDO[Elem].CCVel[U] = CD[Elem].CCVel[U];
             CDO[Elem].CCVel[V] = CD[Elem].CCVel[V];
             CDO[Elem].CCVel[W] = CD[Elem].CCVel[W];			
         }
         
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////
         printf("\nDatas Succesfully Taken and Restarted\n");
     }
     else
     {
         Iter = 0;
         InitialConditions(NoOfElements, NoOfNodes, CD, CDO, Node);
     }
     
     //WriteBoundaryPLTFILE(NoOfElements, NoOfNodes, CC, Node);
     
 //End of Preprocessing...
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 //Core Solver...
     do
     {
         Iter++;
         printf("\nIter: %d ", Iter);
         //Solver(NoOfElements, NoOfNodes, NoOfBoundaryElements, CC, CD, CDO, Node, BoundElems, Iter);
         //PostSolver(NoOfElements, NoOfNodes, CC, CD, CDO, Node, Iter);
     }while(Iter < MAXITER);
 
 //End of Core Solver
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     
     printf("\nJai Sai Krishna\n");
 
     free(Node);
     free(CC);
     free(CD);
     free(CDO);
     free(BoundElems);
     time (&end);
     dif = difftime (end,start);
     printf ("It took you %e seconds to run your code.\n", dif );
     printf("Program Terminated...\nPress any key to exit\n");
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
 
 /*
 void WriteBoundaryPLTFILE(int NoOfElements, int NoOfNodes, CELLDETAILS *CC, NODE *Node)
 {
     FILE *Fwrite;
     int i;
     int count=0;
     char Filename[100];
     sprintf(Filename,"BoundaryPLT.plt");
 
     if( (Fwrite = fopen(Filename,"wt")) == NULL)
     {
         printf("\nUnable to open Plt File");
         return;
     }
 
     i=34;
 
     fprintf(Fwrite,"TITLE = %c2D Flow Past Circular Cylinder%c\n",i,i);
     
     fprintf(Fwrite, "VARIABLES = \"X\", \"Y\",\"Z\",\n");
     //fprintf(Fwrite, "ZONE N=%d, E=%d, DATAPACKING=BLOCK, ZONETYPE=FETETRAHEDRON\n",NoOfNodes,NoOfElements);
     fprintf(Fwrite, "ZONE N=%d, E=1, DATAPACKING=BLOCK, ZONETYPE=FETETRAHEDRON\n",NoOfNodes);
 
         
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
     
     
 
     fprintf(Fwrite,"\n\n");
     for(i=0; i<NoOfElements; i++)
     {
         //fprintf(Fwrite,"%d\t%d\t%d\t%d\n", CC[i].Connect[0], CC[i].Connect[1], CC[i].Connect[2], CC[i].Connect[3]);
         //i=1000000;
         if(CC[i].Type == OUTLET)
         {
             fprintf(Fwrite,"%d\t%d\t%d\t%d", CC[i].Connect[0], CC[i].Connect[1], CC[i].Connect[2], CC[i].Connect[3]);
             count++;
         }
     }
     printf("\n\n\nOutletCount = %d\n\n",count);
     fclose(Fwrite);
     //exit(0);
 }*/
 
 
 void InitialConditions(int NoOfElements, int NoOfNodes, CELLDATAS *CD, CELLDATAS *CDO, NODE *Node)
 {
     int i;
     printf("\nDefinig Initial Conditions...");
     for(i=0;i<NoOfElements;i++)
     {
         CD[i].CCP = CDO[i].CCP = Pinit;
 //		CD[i].CCT = CDO[i].CCT = Tinit;
         CD[i].CCVel[U] = CDO[i].CCVel[U] = Uinit;
         CD[i].CCVel[V] = CDO[i].CCVel[V] = Vinit;
         CD[i].CCVel[W] = CDO[i].CCVel[W] = Winit;
 
         CD[i].FCP[FACE_A] = CDO[i].FCP[FACE_A] = Pinit;
         CD[i].FCP[FACE_B] = CDO[i].FCP[FACE_B] = Pinit;
         CD[i].FCP[FACE_C] = CDO[i].FCP[FACE_C] = Pinit;
         CD[i].FCP[FACE_D] = CDO[i].FCP[FACE_D] = Pinit;
 /*
         CD[i].FCT[FACE_A] = CDO[i].FCT[FACE_A] = Tinit;
         CD[i].FCT[FACE_B] = CDO[i].FCT[FACE_B] = Tinit;
         CD[i].FCT[FACE_C] = CDO[i].FCT[FACE_C] = Tinit;
         CD[i].FCT[FACE_D] = CDO[i].FCT[FACE_D] = Tinit;
 */
         CD[i].FCVel[FACE_A][U] = CD[i].FCVel[FACE_A][U] = Uinit;
         CD[i].FCVel[FACE_B][U] = CD[i].FCVel[FACE_B][U] = Uinit;
         CD[i].FCVel[FACE_C][U] = CD[i].FCVel[FACE_C][U] = Uinit;
         CD[i].FCVel[FACE_D][U] = CD[i].FCVel[FACE_D][U] = Uinit;
 
         CD[i].FCVel[FACE_A][V] = CD[i].FCVel[FACE_A][V] = Vinit;
         CD[i].FCVel[FACE_B][V] = CD[i].FCVel[FACE_B][V] = Vinit;
         CD[i].FCVel[FACE_C][V] = CD[i].FCVel[FACE_C][V] = Vinit;
         CD[i].FCVel[FACE_D][V] = CD[i].FCVel[FACE_D][V] = Vinit;
 
         CD[i].FCVel[FACE_A][W] = CD[i].FCVel[FACE_A][W] = Winit;
         CD[i].FCVel[FACE_B][W] = CD[i].FCVel[FACE_B][W] = Winit;
         CD[i].FCVel[FACE_C][W] = CD[i].FCVel[FACE_C][W] = Winit;
         CD[i].FCVel[FACE_D][W] = CD[i].FCVel[FACE_D][W] = Winit;
 
         CDO[i].CCP = CDO[i].CCP = Pinit;
 //		CDO[i].CCT = CDO[i].CCT = Tinit;
         CDO[i].CCVel[U] = CDO[i].CCVel[U] = Uinit;
         CDO[i].CCVel[V] = CDO[i].CCVel[V] = Vinit;
         CDO[i].CCVel[W] = CDO[i].CCVel[W] = Winit;
 
         CDO[i].FCP[FACE_A] = CDO[i].FCP[FACE_A] = Pinit;
         CDO[i].FCP[FACE_B] = CDO[i].FCP[FACE_B] = Pinit;
         CDO[i].FCP[FACE_C] = CDO[i].FCP[FACE_C] = Pinit;
         CDO[i].FCP[FACE_D] = CDO[i].FCP[FACE_D] = Pinit;
 /*
         CDO[i].FCT[FACE_A] = CDO[i].FCT[FACE_A] = Tinit;
         CDO[i].FCT[FACE_B] = CDO[i].FCT[FACE_B] = Tinit;
         CDO[i].FCT[FACE_C] = CDO[i].FCT[FACE_C] = Tinit;
         CDO[i].FCT[FACE_D] = CDO[i].FCT[FACE_D] = Tinit;
 */
         CDO[i].FCVel[FACE_A][U] = CD[i].FCVel[FACE_A][U] = Uinit;
         CDO[i].FCVel[FACE_B][U] = CD[i].FCVel[FACE_B][U] = Uinit;
         CDO[i].FCVel[FACE_C][U] = CD[i].FCVel[FACE_C][U] = Uinit;
         CDO[i].FCVel[FACE_D][U] = CD[i].FCVel[FACE_D][U] = Uinit;
 
         CDO[i].FCVel[FACE_A][V] = CD[i].FCVel[FACE_A][V] = Vinit;
         CDO[i].FCVel[FACE_B][V] = CD[i].FCVel[FACE_B][V] = Vinit;
         CDO[i].FCVel[FACE_C][V] = CD[i].FCVel[FACE_C][V] = Vinit;
         CDO[i].FCVel[FACE_D][V] = CD[i].FCVel[FACE_D][V] = Vinit;
 
         CDO[i].FCVel[FACE_A][W] = CD[i].FCVel[FACE_A][W] = Winit;
         CDO[i].FCVel[FACE_B][W] = CD[i].FCVel[FACE_B][W] = Winit;
         CDO[i].FCVel[FACE_C][W] = CD[i].FCVel[FACE_C][W] = Winit;
         CDO[i].FCVel[FACE_D][W] = CD[i].FCVel[FACE_D][W] = Winit;
     }
     printf("\nInitial Conditions Defined.");
 }
 
 BOUNDARYELEMENTS* BoundaryElements(int *NoOfBoundaryElements)
 {
     BOUNDARYELEMENTS *BE;
     FILE *FPElem;
     char FileBoundaryElems[100];
     int NElems, Elem;
     int Type;
     switch (COMP)
     {
     case 1:
         sprintf(FileBoundaryElems,"Mesh\\BoundElems.dat");
         break;
     case 2:
         sprintf(FileBoundaryElems,".//Mesh//BoundElems.dat");
         break;
     default:
         printf("\n OS version and Compiler Version Error.\nProgram Terminated.\n\n");
         exit(0);
     }
 
     if( (FPElem = fopen(FileBoundaryElems,"rt") )==NULL)
     {
         printf("Unable to Open %s File\n",FileBoundaryElems);
         printf("Program Terminated.");
         exit(3);
     }
     fscanf(FPElem,"%d\n", &NElems);
 
     if((BE =(BOUNDARYELEMENTS*)malloc((NElems)*sizeof(BOUNDARYELEMENTS)))==NULL)
     {
         printf("Not Enough Memory for Allocating Nodes. \nProgram Terminated");
         exit(2);
     }
 
     for(Elem=0;Elem<NElems;Elem++)
     {
         fscanf(FPElem,"%d\t%d\t%d\t%d\t%d\t%d\n", &BE[Elem].Elem, &Type, &BE[Elem].Face[FACE_A], &BE[Elem].Face[FACE_B], &BE[Elem].Face[FACE_C], &BE[Elem].Face[FACE_D]);		
     }
     *NoOfBoundaryElements = NElems;
     return BE;
 }
 
 
 
 int RestartCheck(int NoOfElements, int NoOfNodes)
 {
     FILE *FP;
     int Iter;
     int ConElements, ConNodes, ConRe;
     char FileRestart[100];
     
     switch (COMP)
     {
     case 1:
         sprintf(FileRestart,"Restart\\Config.cnf");
         break;
     case 2:
         sprintf(FileRestart,".//Restart//Config.cnf");
         break;
     default:
         printf("\n OS version and Compiler Version Error.\nProgram Terminated.\n\n");
         exit(0);
     }
     system("del Restart\\Config.cnf");
     printf("\n\n\nChecking...\n\n\n");
 
     if( (FP = fopen(FileRestart,"r+")) == NULL)
     {
         printf("\nUnable to open Restart config File\n");
         return 0;
     }
     fscanf(FP, "%d%d%d%d", &ConElements, &ConNodes, &ConRe, &Iter);
     //printf("%d\n",Iter);
     //getch();
     fclose(FP);
     //if(ConElements == NoOfElements && ConNodes == NoOfNodes && ConRe == RE)
     //	return Iter;
     //else
         return 0;
 }