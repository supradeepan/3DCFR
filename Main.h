/*Om Sri Sai Ram
 * Main.h
 * 3DCFRUNS
 *  Created on: 02-Oct-2012
 *      Author: supradeepan
 */

 #ifndef Sai_3DMAIN_H_
 #define Sai_3DMAIN_H_
 
 
 #ifdef _WIN32
     #define COMP 1
 #else
     #ifdef __UNIX__
         #define COMP 2
     #else
         #define COMP 0
     #endif
 #endif
 
 #include <stdio.h>
 
 #define MAXSHAREELEMENT 60
 #define MAXFACES 4
 #define MAXCOMPONENTS 3
 
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /*
  Every Node is represented P(x,y)
  The Type represents the location of the point like,
  DOM --> Fully immersed in the flow
  TOP --> point lies on the Top Boundary
  BOTTOM --> point lies on the Bottom Boundary
  INLET --> point lies on the Inlet Boundary
  OUTLET --> point lies on the Outlet Boundary
  BODY1 --> Point lies on the Body. The Number represents the Body Number.
  */
 
 typedef struct
 {
     double x,y,z;
     int Type;
     int ShareElements[MAXSHAREELEMENT];
     int ShareCount;
     //double CC2NodeDist[MAXSHAREELEMENT];
     //double Sum;
     //double u,v,w;
 }NODE;
 
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /*
  *Every Triangular Cell is represented by the connection of 3 nodes named as "a", "b", "c"
  *The connectivity information is stored in Connect[3]. Arranged in counterclockwise orientation
 
  Xab, Xbc, Xca represents the X component distance of the line joining ab, bc and ca respectively.		<-------------------
  Yab, Ybc, Yca represents the Y component distance of the line joining ab, bc and ca respectively.		<---------------	|
                                                                                                                         |	|
  Area Represents the area of the cell.																					|	|
                                                                                                                         |	|
  Neigh[3] holds the three neighbouring cells. if any neighour is available it will be assigned to NEIGHUNAVAILABLE		|	|
  CellD, CellE, CellG, CellF, CellI, CellJ, CellL represents the cell informations in the stencil						|	|
                                                                                                                         |	|
  Ybd, Ydc, Ycj, Yja, Yag, Ygb; Represents similar to --------------------------------------------------------------------	|
  Xbd, Xdc, Xcj, Xja, Xag, Xgb; Represents similar to ------------------------------------------------------------------------
  Apa, Apb, Apc represents Area of parent cell + Area of CELLA,Area of parent cell + Area of CELLB, Area of parent cell + Area of CELLCrespectively
 
  Index4, Index5, Index11, Index12, Index18, Index19 represents the location of the parematers in the stencil
 
  Ka, Kb, Kc, Kd, Kf, Kg, Ki, Kj, Kl, Kp represents the Constants used in the final form of the Pressure poisson Equation.
 
  The Type represents the location of the Cell like,
  DOM --> Fully immersed in the flow
  TOP --> one cell face (FACE1) lies on the Top Boundary
  BOTTOM --> one cell face (FACE1) lies on the Bottom Boundary
  INLET --> one cell face (FACE1) lies on the Inlet Boundary
  OUTLET --> one cell face (FACE1) lies on the Outlet Boundary
  BODY1 --> one cell face (FACE1) lies on the Body. The Number represents the Body Number.
 
 
  Note:
      If Two of the cell faces are lying on the boundary the code crashes.
  */
 
 
 
 typedef struct
 {
     int Connect[MAXFACES];
     int Neighbour[MAXFACES];
     double Normals[MAXFACES][MAXCOMPONENTS];
     int Type;
     double FaceArea[MAXFACES];
     double Volume;
 
     //STENCIL PARAMETERS
     int CellE, CellF, CellG, CellH, CellI, CellJ, CellK, CellL, CellM, CellN, CellO, CellQ;
     int FaceE, FaceF, FaceG, FaceH, FaceI, FaceJ, FaceK, FaceL, FaceM, FaceN, FaceO, FaceP;
     //PPE Co-efficients
     double KA, KB, KC, KD, KE, KF, KG, KH, KI, KJ, KK, KL, KM, KN, KO, KP, KQ;
     //Least square derivative constants
     double Ap, Bp, Cp, Dp, Ep, Fp;
     double Gpa, Gpb, Gpc, Gpd;
     double Hpa, Hpb, Hpc, Hpd;
     double Ipa, Ipb, Ipc, Ipd;
 
 }CELLDETAILS;
 
 typedef struct
 {
     double CCVel[MAXCOMPONENTS];
     double FCVel[MAXFACES][MAXCOMPONENTS];
     double CCP;
     double FCP[MAXFACES];
     
     //double CCT;
     //double FCT[MAXFACES];
     //double FCdT_dx[MAXFACES], FCdT_dy[MAXFACES], FCdT_dz[MAXFACES];
     
     double CCdu_dx, CCdv_dx, CCdw_dx;
     double CCdu_dy, CCdv_dy, CCdw_dy;
     double CCdu_dz, CCdv_dz, CCdw_dz;
     double FCdu_dx[MAXFACES], FCdu_dy[MAXFACES], FCdu_dz[MAXFACES];
     double FCdv_dx[MAXFACES], FCdv_dy[MAXFACES], FCdv_dz[MAXFACES];
     double FCdw_dx[MAXFACES], FCdw_dy[MAXFACES], FCdw_dz[MAXFACES];
 
     double Source;
 
     double XCFLUX[MAXFACES], YCFLUX[MAXFACES], ZCFLUX[MAXFACES], XDFLUX[MAXFACES], YDFLUX[MAXFACES], ZDFLUX[MAXFACES];
     double XPFLUX[MAXFACES], YPFLUX[MAXFACES], ZPFLUX[MAXFACES];
 }CELLDATAS;
 
 typedef struct
 {
     int Elem;
     int Face[MAXFACES];
 
     double Ap[3], Bp[3], Cp[3], Dp[3], Ep[3], Fp[3];
     double Gpa[3], Gpb[3], Gpc[3], Gpd[3];
     double Hpa[3], Hpb[3], Hpc[3], Hpd[3];
     double Ipa[3], Ipb[3], Ipc[3], Ipd[3];
 
 }BOUNDARYELEMENTS;
 
 typedef struct
 {
     double Ua, Ub, Uc, Ud, Ue, Uf, Ug, Uh, Ui, Uj, Uk, Ul, Um, Un, Uo, Up;
     double Va, Vb, Vc, Vd, Ve, Vf, Vg, Vh, Vi, Vj, Vk, Vl, Vm, Vn, Vo, Vp;
     double Wa, Wb, Wc, Wd, We, Wf, Wg, Wh, Wi, Wj, Wk, Wl, Wm, Wn, Wo, Wp;
 
     //double Pa, Pb, Pc, Pd, Pe, Pf, Pg, Ph, Pi, Pj, Pk, Pl, Pm, Pn, Po, Pp;
     
     double dUdXa, dUdXb, dUdXc, dUdXd, dUdXe, dUdXf, dUdXg, dUdXh, dUdXi, dUdXj, dUdXk, dUdXl, dUdXm, dUdXn, dUdXo, dUdXp;
     double dVdXa, dVdXb, dVdXc, dVdXd, dVdXe, dVdXf, dVdXg, dVdXh, dVdXi, dVdXj, dVdXk, dVdXl, dVdXm, dVdXn, dVdXo, dVdXp;
     double dWdXa, dWdXb, dWdXc, dWdXd, dWdXe, dWdXf, dWdXg, dWdXh, dWdXi, dWdXj, dWdXk, dWdXl, dWdXm, dWdXn, dWdXo, dWdXp;
 
     double dUdYa, dUdYb, dUdYc, dUdYd, dUdYe, dUdYf, dUdYg, dUdYh, dUdYi, dUdYj, dUdYk, dUdYl, dUdYm, dUdYn, dUdYo, dUdYp;
     double dVdYa, dVdYb, dVdYc, dVdYd, dVdYe, dVdYf, dVdYg, dVdYh, dVdYi, dVdYj, dVdYk, dVdYl, dVdYm, dVdYn, dVdYo, dVdYp;
     double dWdYa, dWdYb, dWdYc, dWdYd, dWdYe, dWdYf, dWdYg, dWdYh, dWdYi, dWdYj, dWdYk, dWdYl, dWdYm, dWdYn, dWdYo, dWdYp;
 
     double dUdZa, dUdZb, dUdZc, dUdZd, dUdZe, dUdZf, dUdZg, dUdZh, dUdZi, dUdZj, dUdZk, dUdZl, dUdZm, dUdZn, dUdZo, dUdZp;
     double dVdZa, dVdZb, dVdZc, dVdZd, dVdZe, dVdZf, dVdZg, dVdZh, dVdZi, dVdZj, dVdZk, dVdZl, dVdZm, dVdZn, dVdZo, dVdZp;
     double dWdZa, dWdZb, dWdZc, dWdZd, dWdZe, dWdZf, dWdZg, dWdZh, dWdZi, dWdZj, dWdZk, dWdZl, dWdZm, dWdZn, dWdZo, dWdZp;
     
     double Nx_a, Nx_b, Nx_c, Nx_d, Nx_e, Nx_f, Nx_g, Nx_h, Nx_i, Nx_j, Nx_k, Nx_l, Nx_m, Nx_n, Nx_o, Nx_p;
     double Ny_a, Ny_b, Ny_c, Ny_d, Ny_e, Ny_f, Ny_g, Ny_h, Ny_i, Ny_j, Ny_k, Ny_l, Ny_m, Ny_n, Ny_o, Ny_p;
     double Nz_a, Nz_b, Nz_c, Nz_d, Nz_e, Nz_f, Nz_g, Nz_h, Nz_i, Nz_j, Nz_k, Nz_l, Nz_m, Nz_n, Nz_o, Nz_p;
 
 }STENCIL;
 
 typedef struct
 {
     double Pa, Pb, Pc, Pd, Pe, Pf, Pg, Ph, Pi, Pj, Pk, Pl, Pm, Pn, Po, Pp;
     double Nx_a, Nx_b, Nx_c, Nx_d, Nx_e, Nx_f, Nx_g, Nx_h, Nx_i, Nx_j, Nx_k, Nx_l, Nx_m, Nx_n, Nx_o, Nx_p;
     double Ny_a, Ny_b, Ny_c, Ny_d, Ny_e, Ny_f, Ny_g, Ny_h, Ny_i, Ny_j, Ny_k, Ny_l, Ny_m, Ny_n, Ny_o, Ny_p;
     double Nz_a, Nz_b, Nz_c, Nz_d, Nz_e, Nz_f, Nz_g, Nz_h, Nz_i, Nz_j, Nz_k, Nz_l, Nz_m, Nz_n, Nz_o, Nz_p;
 }PRESS_STENCIL;
 
 
 #define INLET 1
 #define OUTLET 2
 #define TOP 3
 #define BOTTOM 4
 #define FRONT 5
 #define BACK 6
 #define BODY 7
 #define DOM 0
 
 #define INTOP 13 
 #define INBOT 14 
 #define INFRO 15 
 #define INBAK 16 
 #define OUTTOP 23 
 #define OUTBOT 24 
 #define OUTFRO 25 
 #define OUTBAK 26 
 #define TOPFRO 35 
 #define TOPBAK 36 
 #define BOTFRO 45 
 #define BOTBAK 46
 #define INTOPFRO 135
 #define INTOPBAK 136
 #define INBOTFRO 145
 #define INBOTBAK 146
 #define OUTTOPFRO 235
 #define OUTTOPBAK 236
 #define OUTBOTFRO 245
 #define OUTBOTBAK 246
 
 #define U 0
 #define V 1
 #define W 2
 
 #define FACE_A 0
 #define FACE_B 1
 #define FACE_C 2
 #define FACE_D 3
 
 
 #define Nx 0
 #define Ny 1
 #define Nz 2
 
 #define CELL_A 0
 #define CELL_B 1
 #define CELL_C 2
 #define CELL_D 3
 
 #define NODE_1 0
 #define NODE_2 1
 #define NODE_3 2
 #define NODE_4 3
 
 #define NEIGHUNAVAILABLE -100
 #define NEIGHNEIGHUNAVAILABLE -200
 #define UNAVAILABLEDATA -100
 
 #define Uinit 1.0
 #define Vinit 0.0
 #define Winit 0.0
 #define Pinit 0.0
 #define Tinit 0.0
 
 #define Uinf 1.0
 #define Vinf 0.0
 #define Winf 0.0
 #define Pinf 0.0
 #define Tinf 0.0
 
 
 #define DELT 0.0001
 #define RE 1000
 #define Pr 0.71
 #define MAXITER 400000
 
 
 void GetNoOfElemAndNodes(int *NElem, int *NNode);
 //void WriteBoundaryPLTFILE(int NoOfElements, int NoOfNodes, CELLDETAILS *CC, NODE *Node);
 void InitialConditions(int NoOfElements, int NoOfNodes, CELLDATAS *CD, CELLDATAS *CDO, NODE *Node);
 int RestartCheck(int NoOfElements, int NoOfNodes);
 
 #endif