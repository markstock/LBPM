/* 
This class implements support for halo widths larger than 1
 */
#include "common/WideHalo.h"

ScaLBLWideHalo_Communicator::ScaLBLWideHalo_Communicator(std::shared_ptr <Domain> Dm, int width)
{
	//......................................................................................
	Lock=false; // unlock the communicator
	//......................................................................................
	// Create a separate copy of the communicator for the device
	MPI_Comm_dup(Dm->Comm,&MPI_COMM_SCALBL);
	//......................................................................................
	// Copy the domain size and communication information directly from Dm
	Nx = Dm->Nx;
	Ny = Dm->Ny;
	Nz = Dm->Nz;
	N = Nx*Ny*Nz;
	Nxh = Nx + 2*(width - 1);
	Nyh = Ny + 2*(width - 1);
	Nzh = Nz + 2*(width - 1);
	Nh = Nxh*Nyh*Nzh;
	
	Map.resize(Nx,Ny,Nz);
	
	rank=Dm->rank();
	iproc = Dm->iproc();
	jproc = Dm->jproc();
	kproc = Dm->kproc();
	nprocx = Dm->nprocx();
	nprocy = Dm->nprocy();
	nprocz = Dm->nprocz();
	rank_info = RankInfoStruct(rank,nprocx,nprocy,nprocz);
    rank = rank_info.rank[1][1][1]; 
    rank_X = rank_info.rank[2][1][1]; 
    rank_x = rank_info.rank[0][1][1]; 
    rank_Y = rank_info.rank[1][2][1]; 
    rank_y = rank_info.rank[1][0][1]; 
    rank_Z = rank_info.rank[1][1][2]; 
    rank_z = rank_info.rank[1][1][0]; 
    rank_XY = rank_info.rank[2][2][1]; 
    rank_xy = rank_info.rank[0][0][1]; 
    rank_Xy = rank_info.rank[2][0][1]; 
    rank_xY = rank_info.rank[0][2][1]; 
    rank_XZ = rank_info.rank[2][1][2]; 
    rank_xz = rank_info.rank[0][1][0]; 
    rank_Xz = rank_info.rank[2][1][0]; 
    rank_xZ = rank_info.rank[0][1][2]; 
    rank_YZ = rank_info.rank[1][2][2]; 
    rank_yz = rank_info.rank[1][0][0]; 
    rank_Yz = rank_info.rank[1][2][0]; 
    rank_yZ = rank_info.rank[1][0][2]; 
    rank_XYz = rank_info.rank[2][2][0]; 
    rank_xyz = rank_info.rank[0][0][0]; 
    rank_Xyz = rank_info.rank[2][0][0]; 
    rank_xYz = rank_info.rank[0][2][0]; 
    rank_XYZ = rank_info.rank[2][2][2]; 
    rank_xyZ = rank_info.rank[0][0][2]; 
    rank_XyZ = rank_info.rank[2][0][2]; 
    rank_xYZ = rank_info.rank[0][2][2]; 
 
	MPI_Barrier(MPI_COMM_SCALBL);
	
	/*  Fill in communications patterns for the lists */
	/*       Send lists    */
	sendCount_x = getHaloBlock(width,2*width,width,Nyh-width,width,Nzh-width,dvcSendList_x);
	sendCount_X =getHaloBlock(Nxh-2*width,Nxh-width,width,Nyh-width,width,Nzh-width,dvcSendList_X);
	sendCount_y =getHaloBlock(width,Nxh-width,width,2*width,width,Nzh-width,dvcSendList_y);
	sendCount_Y =getHaloBlock(width,Nxh-width,Nyh-2*width,Nyh-width,width,Nzh-width,dvcSendList_Y);
	sendCount_z =getHaloBlock(width,Nxh-width,width,Nyh-width,width,2*width,dvcSendList_z);
	sendCount_X =getHaloBlock(width,Nxh-width,width,Nyh-width,Nzh-2*width,Nzh-width,dvcSendList_Z);
	// xy
	sendCount_xy =getHaloBlock(width,2*width,width,2*width,width,Nzh-width,dvcSendList_xy);
	sendCount_xY =getHaloBlock(width,2*width,Nyh-2*width,Nyh-width,width,Nzh-width,dvcSendList_xY);
	sendCount_Xy =getHaloBlock(Nxh-2*width,Nxh-width,width,2*width,width,Nzh-width,dvcSendList_Xy);
	sendCount_XY =getHaloBlock(Nxh-2*width,Nxh-width,Nyh-2*width,Nyh-width,width,Nzh-width,dvcSendList_XY);
	// xz
	sendCount_xz =getHaloBlock(width,2*width,width,Nyh-width,width,2*width,dvcSendList_xz);
	sendCount_xZ =getHaloBlock(width,2*width,width,Nyh-width,Nzh-2*width,Nzh-width,dvcSendList_xZ);
	sendCount_Xz =getHaloBlock(Nxh-2*width,Nxh-width,width,Nyh-width,width,2*width,dvcSendList_Xz);
	sendCount_XZ =getHaloBlock(Nxh-2*width,Nxh-width,width,Nyh-width,Nzh-2*width,Nzh-width,dvcSendList_XZ);
	// yz
	sendCount_yz =getHaloBlock(width,Nxh-width,width,2*width,width,2*width,dvcSendList_yz);
	sendCount_yZ =getHaloBlock(width,Nxh-width,width,2*width,Nzh-2*width,Nzh-width,dvcSendList_yZ);
	sendCount_Yz =getHaloBlock(width,Nxh-width,Nyh-2*width,Nyh-width,width,2*width,dvcSendList_Yz);
	sendCount_YZ =getHaloBlock(width,Nxh-width,Nyh-2*width,Nyh-width,Nzh-2*width,Nzh-width,dvcSendList_YZ);
	// xyz
	sendCount_xyz =getHaloBlock(width,2*width,width,2*width,width,2*width,dvcSendList_xyz);
	sendCount_xyZ =getHaloBlock(width,2*width,width,2*width,Nzh-2*width,Nzh-width,dvcSendList_xyZ);
	sendCount_xYz =getHaloBlock(width,2*width,Nyh-2*width,Nyh-width,width,2*width,dvcSendList_xYz);
	sendCount_xYz =getHaloBlock(width,2*width,Nyh-2*width,Nyh-width,Nzh-2*width,Nzh-width,dvcSendList_xYZ);
	sendCount_Xyz =getHaloBlock(Nxh-2*width,Nxh-width,width,2*width,width,2*width,dvcSendList_Xyz);
	sendCount_XyZ =getHaloBlock(Nxh-2*width,Nxh-width,width,2*width,Nzh-2*width,Nzh-width,dvcSendList_XyZ);
	sendCount_XYz =getHaloBlock(Nxh-2*width,Nxh-width,Nyh-2*width,Nyh-width,width,2*width,dvcSendList_XYz);
	sendCount_XYZ =getHaloBlock(Nxh-2*width,Nxh-width,Nyh-2*width,Nyh-width,Nzh-2*width,Nzh-width,dvcSendList_XYZ);
	
	/*       Recv lists    */
	recvCount_x =getHaloBlock(0,width,width,Nyh-width,width,Nzh-width,dvcRecvList_x);
	recvCount_X =getHaloBlock(Nxh-width,Nxh,width,Nyh-width,width,Nzh-width,dvcRecvList_X);
	recvCount_y =getHaloBlock(width,Nxh-width,0,width,width,Nzh-width,dvcRecvList_y);
	recvCount_Y =getHaloBlock(width,Nxh-width,Nyh-width,Nyh,width,Nzh-width,dvcRecvList_Y);
	recvCount_z =getHaloBlock(width,Nxh-width,width,Nyh-width,0,width,dvcRecvList_z);
	recvCount_Z =getHaloBlock(width,Nxh-width,width,Nyh-width,Nzh-width,Nzh,dvcRecvList_Z);
	//xy
	recvCount_xy =getHaloBlock(0,width,0,width,width,Nzh-width,dvcRecvList_xy);
	recvCount_xY =getHaloBlock(0,width,Nyh-width,Nyh,width,Nzh-width,dvcRecvList_xY);
	recvCount_Xy =getHaloBlock(Nxh-width,Nxh,0,width,width,Nzh-width,dvcRecvList_Xy);
	recvCount_XY =getHaloBlock(Nxh-width,Nxh,Nyh-width,Nyh,width,Nzh-width,dvcRecvList_XY);
	//xz
	recvCount_xz =getHaloBlock(0,width,width,Nyh-width,0,width,dvcRecvList_xz);
	recvCount_xZ =getHaloBlock(0,width,width,Nyh-width,Nzh-width,Nzh,dvcRecvList_xZ);
	recvCount_Xz =getHaloBlock(Nxh-width,Nxh,width,Nyh-width,0,width,dvcRecvList_Xz);
	recvCount_XZ =getHaloBlock(Nxh-width,Nxh,width,Nyh-width,Nzh-width,Nzh,dvcRecvList_XZ);
	//yz
	recvCount_yz =getHaloBlock(width,Nxh-width,0,width,0,width,dvcRecvList_yz);
	recvCount_yZ =getHaloBlock(width,Nxh-width,0,width,Nzh-width,Nzh,dvcRecvList_yZ);
	recvCount_Yz =getHaloBlock(width,Nxh-width,Nyh-width,Nyh,0,width,dvcRecvList_Yz);
	recvCount_YZ =getHaloBlock(width,Nxh-width,Nyh-width,Nyh,Nzh-width,Nzh,dvcRecvList_YZ);
	//xyz
	recvCount_xyz =getHaloBlock(0,width,0,width,0,width,dvcRecvList_xyz);
	recvCount_xyZ =getHaloBlock(0,width,0,width,Nzh-width,Nzh,dvcRecvList_xyZ);
	recvCount_xYz =getHaloBlock(0,width,Nyh-width,Nyh,0,width,dvcRecvList_xYz);
	recvCount_xYZ =getHaloBlock(0,width,Nyh-width,Nyh,Nzh-width,Nzh,dvcRecvList_xYZ);
	recvCount_Xyz =getHaloBlock(Nxh-width,Nxh,0,width,0,width,dvcRecvList_Xyz);
	recvCount_XyZ =getHaloBlock(Nxh-width,Nxh,0,width,Nzh-width,Nzh,dvcRecvList_XyZ);
	recvCount_XYz =getHaloBlock(Nxh-width,Nxh,Nyh-width,Nyh,0,width,dvcRecvList_XYz);
	recvCount_XYZ =getHaloBlock(Nxh-width,Nxh,Nyh-width,Nyh,Nzh-width,Nzh,dvcRecvList_XYZ);
	
	//......................................................................................
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_x, sendCount_x*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_X, sendCount_X*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_y, sendCount_y*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_Y, sendCount_Y*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_z, sendCount_z*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_Z, sendCount_Z*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_xy, sendCount_xy*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_xY, sendCount_xY*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_Xy, sendCount_Xy*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_XY, sendCount_XY*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_xz, sendCount_xz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_xZ, sendCount_xZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_Xz, sendCount_Xz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_XZ, sendCount_XZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_yz, sendCount_yz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_yZ, sendCount_yZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_Yz, sendCount_Yz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_YZ, sendCount_YZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_xyz, sendCount_xyz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_xYz, sendCount_xYz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_Xyz, sendCount_Xyz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_XYz, sendCount_XYz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_xyZ, sendCount_xyZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_xYZ, sendCount_xYZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_XyZ, sendCount_XyZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &sendbuf_XYZ, sendCount_XYZ*sizeof(double));	// Allocate device memory
	//......................................................................................
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_x, recvCount_x*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_X, recvCount_X*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_y, recvCount_y*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_Y, recvCount_Y*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_z, recvCount_z*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_Z, recvCount_Z*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_xy, recvCount_xy*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_xY, recvCount_xY*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_Xy, recvCount_Xy*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_XY, recvCount_XY*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_xz, recvCount_xz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_xZ, recvCount_xZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_Xz, recvCount_Xz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_XZ, recvCount_XZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_yz, recvCount_yz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_yZ, recvCount_yZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_Yz, recvCount_Yz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_YZ, recvCount_YZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_xyz, recvCount_xyz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_xYz, recvCount_xYz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_Xyz, recvCount_Xyz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_XYz, recvCount_XYz*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_xyZ, recvCount_xyZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_xYZ, recvCount_xYZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_XyZ, recvCount_XyZ*sizeof(double));	// Allocate device memory
	ScaLBL_AllocateZeroCopy((void **) &recvbuf_XYZ, recvCount_XYZ*sizeof(double));	// Allocate device memory
	
	/* Set up a map to the halo width=1 data structure */
	for (k=width; k<Nzh-width; k++){
		for (j=width; j<Nyh-width; j++){
			for (i=width; i<Nxh-width; i++){
				int idx = k*Nxh*Nyh + j*Nxh + i;
				Map(i-width+1,j-width+1,k-width+1) = idx;
			}
		}
	}
	
}

void ScaLBLWideHalo_Communicator::Send(double *data){
	//...................................................................................
	if (Lock==true){
		ERROR("ScaLBL Error (SendHalo): ScaLBLWideHalo_Communicator is locked -- did you forget to match Send/Recv calls?");
	}
	else{
		Lock=true;
	}
	ScaLBL_DeviceBarrier();
	//...................................................................................
	sendtag = recvtag = 1;
	//...................................................................................
	ScaLBL_Scalar_Pack(dvcSendList_x, sendCount_x,sendbuf_x, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_y, sendCount_y,sendbuf_y, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_z, sendCount_z,sendbuf_z, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_X, sendCount_X,sendbuf_X, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_Y, sendCount_Y,sendbuf_Y, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_Z, sendCount_Z,sendbuf_Z, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_xy, sendCount_xy,sendbuf_xy, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_xY, sendCount_xY,sendbuf_xY, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_Xy, sendCount_Xy,sendbuf_Xy, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_XY, sendCount_XY,sendbuf_XY, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_xz, sendCount_xz,sendbuf_xz, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_xZ, sendCount_xZ,sendbuf_xZ, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_Xz, sendCount_Xz,sendbuf_Xz, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_XZ, sendCount_XZ,sendbuf_XZ, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_yz, sendCount_yz,sendbuf_yz, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_yZ, sendCount_yZ,sendbuf_yZ, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_Yz, sendCount_Yz,sendbuf_Yz, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_YZ, sendCount_YZ,sendbuf_YZ, data, Nh);
	/* corners */
	ScaLBL_Scalar_Pack(dvcSendList_xyz, sendCount_xyz,sendbuf_xyz, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_xyZ, sendCount_xyZ,sendbuf_xyZ, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_xYz, sendCount_xYz,sendbuf_xYz, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_xYZ, sendCount_xYZ,sendbuf_xYZ, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_Xyz, sendCount_Xyz,sendbuf_Xyz, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_XyZ, sendCount_XyZ,sendbuf_XyZ, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_XYz, sendCount_XYz,sendbuf_XYz, data, Nh);
	ScaLBL_Scalar_Pack(dvcSendList_XYZ, sendCount_XYZ,sendbuf_XYZ, data, Nh);
	//...................................................................................
	// Send / Recv all the phase indcator field values
	//...................................................................................

	MPI_Isend(sendbuf_x, sendCount_x,MPI_DOUBLE,rank_x,sendtag,MPI_COMM_SCALBL,&req1[0]);
	MPI_Irecv(recvbuf_X, recvCount_X,MPI_DOUBLE,rank_X,recvtag,MPI_COMM_SCALBL,&req2[0]);
	MPI_Isend(sendbuf_X, sendCount_X,MPI_DOUBLE,rank_X,sendtag,MPI_COMM_SCALBL,&req1[1]);
	MPI_Irecv(recvbuf_x, recvCount_x,MPI_DOUBLE,rank_x,recvtag,MPI_COMM_SCALBL,&req2[1]);
	MPI_Isend(sendbuf_y, sendCount_y,MPI_DOUBLE,rank_y,sendtag,MPI_COMM_SCALBL,&req1[2]);
	MPI_Irecv(recvbuf_Y, recvCount_Y,MPI_DOUBLE,rank_Y,recvtag,MPI_COMM_SCALBL,&req2[2]);
	MPI_Isend(sendbuf_Y, sendCount_Y,MPI_DOUBLE,rank_Y,sendtag,MPI_COMM_SCALBL,&req1[3]);
	MPI_Irecv(recvbuf_y, recvCount_y,MPI_DOUBLE,rank_y,recvtag,MPI_COMM_SCALBL,&req2[3]);
	MPI_Isend(sendbuf_z, sendCount_z,MPI_DOUBLE,rank_z,sendtag,MPI_COMM_SCALBL,&req1[4]);
	MPI_Irecv(recvbuf_Z, recvCount_Z,MPI_DOUBLE,rank_Z,recvtag,MPI_COMM_SCALBL,&req2[4]);
	MPI_Isend(sendbuf_Z, sendCount_Z,MPI_DOUBLE,rank_Z,sendtag,MPI_COMM_SCALBL,&req1[5]);
	MPI_Irecv(recvbuf_z, recvCount_z,MPI_DOUBLE,rank_z,recvtag,MPI_COMM_SCALBL,&req2[5]);
	MPI_Isend(sendbuf_xy, sendCount_xy,MPI_DOUBLE,rank_xy,sendtag,MPI_COMM_SCALBL,&req1[6]);
	MPI_Irecv(recvbuf_XY, recvCount_XY,MPI_DOUBLE,rank_XY,recvtag,MPI_COMM_SCALBL,&req2[6]);
	MPI_Isend(sendbuf_XY, sendCount_XY,MPI_DOUBLE,rank_XY,sendtag,MPI_COMM_SCALBL,&req1[7]);
	MPI_Irecv(recvbuf_xy, recvCount_xy,MPI_DOUBLE,rank_xy,recvtag,MPI_COMM_SCALBL,&req2[7]);
	MPI_Isend(sendbuf_Xy, sendCount_Xy,MPI_DOUBLE,rank_Xy,sendtag,MPI_COMM_SCALBL,&req1[8]);
	MPI_Irecv(recvbuf_xY, recvCount_xY,MPI_DOUBLE,rank_xY,recvtag,MPI_COMM_SCALBL,&req2[8]);
	MPI_Isend(sendbuf_xY, sendCount_xY,MPI_DOUBLE,rank_xY,sendtag,MPI_COMM_SCALBL,&req1[9]);
	MPI_Irecv(recvbuf_Xy, recvCount_Xy,MPI_DOUBLE,rank_Xy,recvtag,MPI_COMM_SCALBL,&req2[9]);
	MPI_Isend(sendbuf_xz, sendCount_xz,MPI_DOUBLE,rank_xz,sendtag,MPI_COMM_SCALBL,&req1[10]);
	MPI_Irecv(recvbuf_XZ, recvCount_XZ,MPI_DOUBLE,rank_XZ,recvtag,MPI_COMM_SCALBL,&req2[10]);
	MPI_Isend(sendbuf_XZ, sendCount_XZ,MPI_DOUBLE,rank_XZ,sendtag,MPI_COMM_SCALBL,&req1[11]);
	MPI_Irecv(recvbuf_xz, recvCount_xz,MPI_DOUBLE,rank_xz,recvtag,MPI_COMM_SCALBL,&req2[11]);
	MPI_Isend(sendbuf_Xz, sendCount_Xz,MPI_DOUBLE,rank_Xz,sendtag,MPI_COMM_SCALBL,&req1[12]);
	MPI_Irecv(recvbuf_xZ, recvCount_xZ,MPI_DOUBLE,rank_xZ,recvtag,MPI_COMM_SCALBL,&req2[12]);
	MPI_Isend(sendbuf_xZ, sendCount_xZ,MPI_DOUBLE,rank_xZ,sendtag,MPI_COMM_SCALBL,&req1[13]);
	MPI_Irecv(recvbuf_Xz, recvCount_Xz,MPI_DOUBLE,rank_Xz,recvtag,MPI_COMM_SCALBL,&req2[13]);
	MPI_Isend(sendbuf_yz, sendCount_yz,MPI_DOUBLE,rank_yz,sendtag,MPI_COMM_SCALBL,&req1[14]);
	MPI_Irecv(recvbuf_YZ, recvCount_YZ,MPI_DOUBLE,rank_YZ,recvtag,MPI_COMM_SCALBL,&req2[14]);
	MPI_Isend(sendbuf_YZ, sendCount_YZ,MPI_DOUBLE,rank_YZ,sendtag,MPI_COMM_SCALBL,&req1[15]);
	MPI_Irecv(recvbuf_yz, recvCount_yz,MPI_DOUBLE,rank_yz,recvtag,MPI_COMM_SCALBL,&req2[15]);
	MPI_Isend(sendbuf_Yz, sendCount_Yz,MPI_DOUBLE,rank_Yz,sendtag,MPI_COMM_SCALBL,&req1[16]);
	MPI_Irecv(recvbuf_yZ, recvCount_yZ,MPI_DOUBLE,rank_yZ,recvtag,MPI_COMM_SCALBL,&req2[16]);
	MPI_Isend(sendbuf_yZ, sendCount_yZ,MPI_DOUBLE,rank_yZ,sendtag,MPI_COMM_SCALBL,&req1[17]);
	MPI_Irecv(recvbuf_Yz, recvCount_Yz,MPI_DOUBLE,rank_Yz,recvtag,MPI_COMM_SCALBL,&req2[17]);
	/* Corners */
	MPI_Isend(sendbuf_xyz, sendCount_xyz,MPI_DOUBLE,rank_xyz,sendtag,MPI_COMM_SCALBL,&req1[18]);
	MPI_Irecv(recvbuf_XYZ, recvCount_XYZ,MPI_DOUBLE,rank_XYZ,recvtag,MPI_COMM_SCALBL,&req2[18]);
	MPI_Isend(sendbuf_XYZ, sendCount_XYZ,MPI_DOUBLE,rank_XYZ,sendtag,MPI_COMM_SCALBL,&req1[19]);
	MPI_Irecv(recvbuf_xyz, recvCount_xyz,MPI_DOUBLE,rank_xyz,recvtag,MPI_COMM_SCALBL,&req2[19]);
	MPI_Isend(sendbuf_xYz, sendCount_xYz,MPI_DOUBLE,rank_xYz,sendtag,MPI_COMM_SCALBL,&req1[20]);
	MPI_Irecv(recvbuf_XyZ, recvCount_XyZ,MPI_DOUBLE,rank_XyZ,recvtag,MPI_COMM_SCALBL,&req2[20]);
	MPI_Isend(sendbuf_XyZ, sendCount_XyZ,MPI_DOUBLE,rank_XyZ,sendtag,MPI_COMM_SCALBL,&req1[21]);
	MPI_Irecv(recvbuf_xYz, recvCount_xYz,MPI_DOUBLE,rank_xYz,recvtag,MPI_COMM_SCALBL,&req2[21]);
	MPI_Isend(sendbuf_Xyz, sendCount_Xyz,MPI_DOUBLE,rank_Xyz,sendtag,MPI_COMM_SCALBL,&req1[22]);
	MPI_Irecv(recvbuf_xYZ, recvCount_xYZ,MPI_DOUBLE,rank_xYZ,recvtag,MPI_COMM_SCALBL,&req2[22]);
	MPI_Isend(sendbuf_xYZ, sendCount_xYZ,MPI_DOUBLE,rank_xYZ,sendtag,MPI_COMM_SCALBL,&req1[23]);
	MPI_Irecv(recvbuf_Xyz, recvCount_Xyz,MPI_DOUBLE,rank_Xyz,recvtag,MPI_COMM_SCALBL,&req2[23]);
	MPI_Isend(sendbuf_XYz, sendCount_XYz,MPI_DOUBLE,rank_XYz,sendtag,MPI_COMM_SCALBL,&req1[24]);
	MPI_Irecv(recvbuf_xyZ, recvCount_xyZ,MPI_DOUBLE,rank_xyZ,recvtag,MPI_COMM_SCALBL,&req2[24]);
	MPI_Isend(sendbuf_xyZ, sendCount_xyZ,MPI_DOUBLE,rank_xyZ,sendtag,MPI_COMM_SCALBL,&req1[25]);
	MPI_Irecv(recvbuf_XYz, recvCount_XYz,MPI_DOUBLE,rank_XYz,recvtag,MPI_COMM_SCALBL,&req2[25]);
	//...................................................................................
}
void ScaLBLWideHalo_Communicator::Recv(double *data){

	//...................................................................................
	MPI_Waitall(26,req1,stat1);
	MPI_Waitall(26,req2,stat2);
	ScaLBL_DeviceBarrier();
	//...................................................................................
	//...................................................................................
	ScaLBL_Scalar_Unpack(dvcRecvList_x, recvCount_x,recvbuf_x, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_y, recvCount_y,recvbuf_y, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_X, recvCount_X,recvbuf_X, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_Y, recvCount_Y,recvbuf_Y, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_xy, recvCount_xy,recvbuf_xy, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_xY, recvCount_xY,recvbuf_xY, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_Xy, recvCount_Xy,recvbuf_Xy, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_XY, recvCount_XY,recvbuf_XY, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_z, recvCount_z,recvbuf_z, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_xz, recvCount_xz,recvbuf_xz, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_Xz, recvCount_Xz,recvbuf_Xz, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_yz, recvCount_yz,recvbuf_yz, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_Yz, recvCount_Yz,recvbuf_Yz, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_Z, recvCount_Z,recvbuf_Z, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_xZ, recvCount_xZ,recvbuf_xZ, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_XZ, recvCount_XZ,recvbuf_XZ, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_yZ, recvCount_yZ,recvbuf_yZ, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_YZ, recvCount_YZ,recvbuf_YZ, data, Nh);
	/* corners */
	ScaLBL_Scalar_Unpack(dvcRecvList_xyz, recvCount_xyz,recvbuf_xyz, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_xYz, recvCount_xYz,recvbuf_xYz, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_xyZ, recvCount_xyZ,recvbuf_xyZ, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_xYZ, recvCount_xYZ,recvbuf_xYZ, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_Xyz, recvCount_Xyz,recvbuf_Xyz, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_XYz, recvCount_XYz,recvbuf_XYz, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_XyZ, recvCount_XyZ,recvbuf_XyZ, data, Nh);
	ScaLBL_Scalar_Unpack(dvcRecvList_XYZ, recvCount_XYZ,recvbuf_XYZ, data, Nh);
	//...................................................................................
	Lock=false; // unlock the communicator after communications complete
	//...................................................................................

}

