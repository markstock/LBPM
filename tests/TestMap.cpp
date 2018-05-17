
//*************************************************************************
// Lattice Boltzmann Simulator for Single Phase Flow in Porous Media
// James E. McCLure
//*************************************************************************
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "common/ScaLBL.h"
#include "common/MPI_Helpers.h"

using namespace std;

//***************************************************************************************
int main(int argc, char **argv)
{
	//*****************************************
	// ***** MPI STUFF ****************
	//*****************************************
	// Initialize MPI
	int rank,nprocs;
	MPI_Init(&argc,&argv);
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Comm_rank(comm,&rank);
	MPI_Comm_size(comm,&nprocs);
	int check;
	{
		// parallel domain size (# of sub-domains)
		int nprocx,nprocy,nprocz;
		int iproc,jproc,kproc;


		if (rank == 0){
			printf("********************************************************\n");
			printf("Running Color Model: TestColor	\n");
			printf("********************************************************\n");
		}

		// BGK Model parameters
		string FILENAME;
		unsigned int nBlocks, nthreads;
		int timestepMax, interval;
		double Fx,Fy,Fz,tol;
		// Domain variables
		double Lx,Ly,Lz;
		int nspheres;
		int Nx,Ny,Nz;
		int i,j,k,n;
		int dim=5;
		
		static int D3Q19[18][3]={{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1},
				{1,1,0},{-1,-1,0},{1,-1,0},{-1,1,0},
				{1,0,1},{-1,0,-1},{1,0,-1},{-1,0,1},
				{0,1,1},{0,-1,-1},{0,1,-1},{0,-1,1}};

		if (rank==0){
			//.......................................................................
			// Reading the domain information file
			//.......................................................................
			ifstream domain("Domain.in");
			if (domain.good()){
				domain >> nprocx;
				domain >> nprocy;
				domain >> nprocz;
				domain >> Nx;
				domain >> Ny;
				domain >> Nz;
				domain >> nspheres;
				domain >> Lx;
				domain >> Ly;
				domain >> Lz;
			}
			else if (nprocs==1){
				nprocx=nprocy=nprocz=1;
				Nx=Ny=Nz=5;
				nspheres=0;
				Lx=Ly=Lz=1;
			}
			else if (nprocs==2){
				nprocx=2; nprocy=1;
				nprocz=1;
				Nx=Ny=Nz=dim;
				Nx = dim; Ny = dim; Nz = dim;
				nspheres=0;
				Lx=Ly=Lz=1;
			}
			else if (nprocs==4){
				nprocx=nprocy=2;
				nprocz=1;
				Nx=Ny=Nz=dim;
				nspheres=0;
				Lx=Ly=Lz=1;
			}
			else if (nprocs==8){
				nprocx=nprocy=nprocz=2;
				Nx=Ny=Nz=dim;
				nspheres=0;
				Lx=Ly=Lz=1;
			}
			//.......................................................................
		}
		// **************************************************************
		// Broadcast simulation parameters from rank 0 to all other procs
		MPI_Barrier(comm);
		//.................................................
		MPI_Bcast(&Nx,1,MPI_INT,0,comm);
		MPI_Bcast(&Ny,1,MPI_INT,0,comm);
		MPI_Bcast(&Nz,1,MPI_INT,0,comm);
		MPI_Bcast(&nprocx,1,MPI_INT,0,comm);
		MPI_Bcast(&nprocy,1,MPI_INT,0,comm);
		MPI_Bcast(&nprocz,1,MPI_INT,0,comm);
		MPI_Bcast(&nspheres,1,MPI_INT,0,comm);
		MPI_Bcast(&Lx,1,MPI_DOUBLE,0,comm);
		MPI_Bcast(&Ly,1,MPI_DOUBLE,0,comm);
		MPI_Bcast(&Lz,1,MPI_DOUBLE,0,comm);
		//.................................................
		MPI_Barrier(comm);
		// **************************************************************
		// **************************************************************

		if (nprocs != nprocx*nprocy*nprocz){
			printf("nprocx =  %i \n",nprocx);
			printf("nprocy =  %i \n",nprocy);
			printf("nprocz =  %i \n",nprocz);
			INSIST(nprocs == nprocx*nprocy*nprocz,"Fatal error in processor count!");
		}

		if (rank==0){
			printf("********************************************************\n");
			printf("Sub-domain size = %i x %i x %i\n",Nx,Ny,Nz);
			printf("********************************************************\n");
		}

		MPI_Barrier(comm);
		int BoundaryCondition=0;

		Domain Dm(Nx,Ny,Nz,rank,nprocx,nprocy,nprocz,Lx,Ly,Lz,BoundaryCondition);

		Nx += 2;
		Ny += 2;
		Nz += 2;
		int N = Nx*Ny*Nz;

		//.......................................................................
		int Np = 0;
		for (k=1;k<Nz-1;k++){
			for (j=1;j<Ny-1;j++){
				for (i=1;i<Nx-1;i++){
					n = k*Nx*Ny+j*Nx+i;
					Dm.id[n] = 1;
					Np++;
				}
			}
		}
		Dm.CommInit(comm);

		// Create a communicator for the device (will use optimized layout)
		ScaLBL_Communicator ScaLBL_Comm(Dm);
		//Create a second communicator based on the regular data layout
		ScaLBL_Communicator ScaLBL_Comm_Regular(Dm);

		// Check that domain information properly copied to ScaLBL communicator
		if (ScaLBL_Comm.sendCount_x != Dm.sendCount_x) printf("domain error x \n");
		if (ScaLBL_Comm.sendCount_X != Dm.sendCount_X) printf("domain error X \n");
		if (ScaLBL_Comm.sendCount_y != Dm.sendCount_y) printf("domain error y \n");
		if (ScaLBL_Comm.sendCount_Y != Dm.sendCount_Y) printf("domain error Y \n");
		if (ScaLBL_Comm.sendCount_z != Dm.sendCount_z) printf("domain error z \n");
		if (ScaLBL_Comm.sendCount_Z != Dm.sendCount_Z) printf("domain error Z \n");
		if (ScaLBL_Comm.sendCount_xy != Dm.sendCount_xy) printf("domain error xy \n");
		if (ScaLBL_Comm.sendCount_xY != Dm.sendCount_xY) printf("domain error xY \n");
		if (ScaLBL_Comm.sendCount_Xy != Dm.sendCount_Xy) printf("domain error Xy \n");
		if (ScaLBL_Comm.sendCount_XY != Dm.sendCount_XY) printf("domain error XY \n");
		if (ScaLBL_Comm.sendCount_xz != Dm.sendCount_xz) printf("domain error xz \n");
		if (ScaLBL_Comm.sendCount_xZ != Dm.sendCount_xZ) printf("domain error xZ \n");
		if (ScaLBL_Comm.sendCount_Xz != Dm.sendCount_Xz) printf("domain error Xz \n");
		if (ScaLBL_Comm.sendCount_XZ != Dm.sendCount_XZ) printf("domain error XZ \n");
		if (ScaLBL_Comm.sendCount_yz != Dm.sendCount_yz) printf("domain error yz \n");
		if (ScaLBL_Comm.sendCount_yZ != Dm.sendCount_yZ) printf("domain error yZ \n");
		if (ScaLBL_Comm.sendCount_Yz != Dm.sendCount_Yz) printf("domain error Yz \n");
		if (ScaLBL_Comm.sendCount_YZ != Dm.sendCount_YZ) printf("domain error YZ \n");		
		
		if (rank==0){
			printf("Total domain size = %i \n",N);
			printf("Reduced domain size = %i \n",Np);
		}

		// LBM variables
		if (rank==0)	printf ("Set up the neighborlist \n");
		int Npad=Np+32;
		int neighborSize=18*Npad*sizeof(int);
		int *neighborList;
		IntArray Map(Nx,Ny,Nz);
		neighborList= new int[18*Npad];

		Np = ScaLBL_Comm.MemoryOptimizedLayoutAA(Map,neighborList,Dm.id,Np);
		MPI_Barrier(comm);
		
		// Check the neighborlist
		printf("Check neighborlist: exterior %i, first interior %i last interior %i \n",ScaLBL_Comm.LastExterior(),ScaLBL_Comm.FirstInterior(),ScaLBL_Comm.LastInterior());
		for (int idx=0; idx<ScaLBL_Comm.LastExterior(); idx++){
			for (int q=0; q<18; q++){
				int nn = neighborList[q*Np+idx]%Np;
				if (nn>Np) printf("neighborlist error (exterior) at q=%i, idx=%i \n",q,idx);
		      
			}
		}
		for (int idx=ScaLBL_Comm.FirstInterior(); idx<ScaLBL_Comm.LastInterior(); idx++){
			for (int q=0; q<18; q++){
				int nn = neighborList[q*Np+idx]%Np;
				if (nn>Np) printf("neighborlist error (exterior) at q=%i, idx=%i \n",q,idx);
		      
			}
		}

		//......................device distributions.................................
		int dist_mem_size = Np*sizeof(double);
		if (rank==0)	printf ("Allocating distributions \n");

		int *NeighborList;
		int *dvcMap;
		
		//...........................................................................
		ScaLBL_AllocateDeviceMemory((void **) &NeighborList, neighborSize);
		ScaLBL_AllocateDeviceMemory((void **) &dvcMap, sizeof(int)*Npad);
		
		//...........................................................................
		// Update GPU data structures
		if (rank==0)	printf ("Setting up device map and neighbor list \n");
		int *TmpMap;
		TmpMap=new int[Np*sizeof(int)];
		for (k=1; k<Nz-1; k++){
			for (j=1; j<Ny-1; j++){
				for (i=1; i<Nx-1; i++){
					int idx=Map(i,j,k);
					if (!(idx < 0))
						TmpMap[idx] = k*Nx*Ny+j*Nx+i;
				}
			}
		}
		ScaLBL_CopyToDevice(dvcMap, TmpMap, sizeof(int)*Np);
		ScaLBL_DeviceBarrier();
		
		// Create a dummy distribution data structure
		double *fq;
		fq = new double[19*Np];
		if (rank==0)	printf ("Setting up distributions \n");
		for (k=1; k<Nz-1; k++){
			for (j=1; j<Ny-1; j++){
				for (i=1; i<Nx-1; i++){
					int idx=Map(i,j,k);
					if (!(idx<0)){
					  for (int q=0; q<19; q++){
					    fq[q*Np+idx]=k*100.f+j*10.f+i*1.f+0.01*q;
					  }
					}
				}
			}
		}
		/*		for (int idx=0; idx<Np; idx++){
			n = TmpMap[idx];
			// back out the 3D indices
			k = n/(Nx*Ny);
			j = (n-Nx*Ny*k)/Nx;
			i = n-Nx*Ny*k-Nx*j;
			for (int q=0; q<19; q++){
				fq[q*Np+idx]=k*100.f+j*10.f+i*1.f+0.01*q;
			}
		}
				if (rank==0)	printf ("Setting up distributions \n");
		
		*/
		// Loop over the distributions for interior lattice sites
		if (rank==0)	printf ("Loop over distributions \n");

		for (int idx=ScaLBL_Comm.first_interior; idx<ScaLBL_Comm.last_interior; idx++){
			n = TmpMap[idx];
			k = n/(Nx*Ny);
			j = (n-Nx*Ny*k)/Nx;
			i = n-Nx*Ny*k-Nx*j;
			for (int q=1; q<19; q++){
				int nn = neighborList[(q-1)*Np+idx];
				double value=fq[nn];
				// 3D index of neighbor
				int iq=i-D3Q19[q-1][0];
				int jq=j-D3Q19[q-1][1];
				int kq=k-D3Q19[q-1][2];
				if (iq==0) iq=1;
				if (jq==0) jq=1;
				if (kq==0) kq=1;
				if (iq==Nx-1) iq=Nx-2;
				if (jq==Ny-1) jq=Ny-2;
				if (kq==Nz-1) kq=Nz-2;
				double check = kq*100.f+jq*10.f+iq*1.f+q*0.01;
				if (value != check)
				  printf("Neighbor q=%i, i=%i,j=%i,k=%i: %f \n",q,iq,jq,kq,value);
			}
		}

		delete [] TmpMap;

	}
	// ****************************************************
	MPI_Barrier(comm);
	MPI_Finalize();
	// ****************************************************

	return check;
}

