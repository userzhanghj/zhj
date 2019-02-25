#include "interface.h"
#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include <s_win.h>
#include "resource.h"
#include <s_img.h>
#include <Mil.h>

#define BUFFERING_SIZE_MAX  30			//最大Buffer List数目



long CALLBACK ProcessingFunction(long HookType, MIL_ID HookId, void MPTYPE *HookDataPtr);	//MdigProcess回调函数

typedef struct
{
	MIL_ID  MilImageDisp;
	long    ProcessedImageCount;
} HookDataStruct;						//自定义的要传递给回调函数的参数结构

MIL_ID	MilApplication	= M_NULL,		/* Application identifier.  */
	MilSystem		= M_NULL,		/* System identifier.       */
	MilDisplay		= M_NULL,		/* Display identifier.      */
	MilDigitizer	= M_NULL,		//采集器标识
	MilBufferImage  = M_NULL;		
long	BufferLocation = M_NULL;				//要分配的内存位置标识
MIL_ID MilGrabBufferList[BUFFERING_SIZE_MAX];	//多Buffer采集时的buffer list数组
long   m; 
long   LastAllocatedM = 0;
long   MilGrabBufferListSize = 0;			//实际分配的Buffer List数目
HookDataStruct UserHookData;					//传递给回调函数的参数
int img_w=0;
int img_h = 0;
int img_ch = 0;

BYTE bmp_buf_c[MAX_IMG_SIZE*3]= {0};

long CALLBACK ProcessingFunction(long HookType, MIL_ID HookId, void MPTYPE *HookDataPtr)
{
	//HookDataStruct *UserHookDataPtr = (HookDataStruct *)HookDataPtr;
	MIL_ID ModifiedBufferId;
	//得到buffer list获得采集数据的buffer号
	SHOW_IMG = !MdigGetHookInfo(HookId, M_MODIFIED_BUFFER+M_BUFFER_ID, &ModifiedBufferId);
	//log("show img:%d",SHOW_IMG);
	//if(!SHOW_IMG)
	//	MdigProcess(MilDigitizer, MilGrabBufferList, MilGrabBufferListSize,
	//	M_STOP, M_DEFAULT, ProcessingFunction, &UserHookData);
	if(SHOW_IMG){
		MbufGet2d(ModifiedBufferId,0,0,img_w,img_h,s_aa::bmp_buf_mil);
		return 0;
	}
	return 1;
}

int Img_Mil::init(int id){
	if(!do_init){
		log("Cmd:[Mil_init]");
		if(MilApplication==M_NULL)
			MappAlloc(M_DEFAULT, &MilApplication);
		//MappAlloc(M_QUIET, &MilApplication);
		if(MilSystem==M_NULL)
			MsysAlloc(M_SYSTEM_SOLIOS, M_DEF_SYSTEM_NUM, M_SETUP, &MilSystem);
		//分配采集器
		if(MilDigitizer==M_NULL)
			MdigAlloc(MilSystem, M_DEFAULT, "d:\\Solios CL Single Base 02.dcf", M_DEFAULT, &MilDigitizer);
		img_w = ((unsigned int) MdigInquire( MilDigitizer, M_SIZE_X, M_NULL));
		img_h = ((unsigned int) MdigInquire( MilDigitizer, M_SIZE_Y, M_NULL));
		img_ch = 1;
		IMG_WIDTH 	=img_w ;
		IMG_HEIGHT	=img_h ;
		IMG_DEPTH	=img_ch;
		MappControl(M_ERROR, M_PRINT_DISABLE);

		//初始化buffer list
		for(m = 0; m < BUFFERING_SIZE_MAX; m++)
		{
			MilGrabBufferList[m] = M_NULL;
		}

		//分配尽可能多的buffer list
		if (MsysInquire(MilSystem, M_SYSTEM_TYPE, M_NULL) == M_SYSTEM_HELIOS_TYPE)
		{
			BufferLocation = M_ON_BOARD;
		}
		MilGrabBufferListSize=0;
		for(m = 0; m < BUFFERING_SIZE_MAX; m++)
		{
			//分配一个Buffer
			MbufAlloc2d(MilSystem,img_w,img_h,8L, 
				M_IMAGE+M_GRAB+M_PROC+BufferLocation, 
				&MilGrabBufferList[m]);

			if (MilGrabBufferList[m])//分配成功则初始化
			{
				MbufClear(MilGrabBufferList[m], 0xFF);

				LastAllocatedM = m;
				MilGrabBufferListSize++;
			}
			else//分配失败则停止分配
			{
				log("set buffer  error///////////////////////////");
				break;
			}
		}
		MappControl(M_ERROR, M_PRINT_ENABLE);

		//防止占完内存空间,释放最后一个buffer
		MbufFree(MilGrabBufferList[LastAllocatedM]);
		MilGrabBufferList[LastAllocatedM] = M_NULL;
		MilGrabBufferListSize--;//注意这里释放后一定要将相应的size-1,否则调用MdigProcess检测
		//到实际可用buffer size和传入的size参数不符，会报错

		is_stop=0;
		do_init=1;
	}
	return TRUE;
}

int Img_Mil::run(){
	/************************************************************************/
	/*采集和处理*/
	/************************************************************************/
	//设置待传递的数据
	log("Cmd:[Mil_Run]");
	UserHookData.MilImageDisp        = MilBufferImage;
	UserHookData.ProcessedImageCount = 0;
	//    if(!CAP_STOP)
	if(MilApplication	== M_NULL||MilSystem == M_NULL||MilDigitizer	== M_NULL)
		return FALSE;
	
	MdigProcess(MilDigitizer, MilGrabBufferList, MilGrabBufferListSize,
		M_START, M_DEFAULT, ProcessingFunction, &UserHookData); 
	return TRUE;
}

int Img_Mil::cap(BYTE *buf, int buf_size){
	int i,j;
	for(i=0;i<IMG_HEIGHT;i++){
		for(j=0;j<IMG_WIDTH;j++){
			s_aa::bmp_buf_t[3*(i*IMG_WIDTH+j)]=s_aa::bmp_buf_mil[i*IMG_WIDTH+j];
			s_aa::bmp_buf_t[3*(i*IMG_WIDTH+j)+1]=s_aa::bmp_buf_mil[i*IMG_WIDTH+j];
			s_aa::bmp_buf_t[3*(i*IMG_WIDTH+j)+2]=s_aa::bmp_buf_mil[i*IMG_WIDTH+j];
		}
	}
//	memcpy(buf,bmp_buf_c, IMG_WIDTH*IMG_HEIGHT*3);
	return TRUE;
}
int Img_Mil::stop(){
	/*停止采集和处理*/
	/************************************************************************/
	MdigProcess(MilDigitizer, MilGrabBufferList, MilGrabBufferListSize,
		M_STOP, M_DEFAULT, ProcessingFunction, &UserHookData);

	//is_stop=1;
	//if(is_stop&&do_init){
	//	log("Cmd:[Mil_Stop]");
	//	MappControl(M_ERROR, M_PRINT_DISABLE);
	//	for (m = 0; m < BUFFERING_SIZE_MAX; m++)
	//	{
	//		if(M_NULL != MilGrabBufferList[m])
	//		{
	//			MbufFree(MilGrabBufferList[m]);
	//			MilGrabBufferList[m] = M_NULL;
	//		}
	//	}
	//	if (M_NULL != MilBufferImage)
	//	{
	//		MbufFree(MilBufferImage);log("Cmd:[Mil_Stop5]");
	//	}
	//	
	//	if (M_NULL != MilDisplay)
	//	{
	//		MdispFree(MilDisplay);log("Cmd:[Mil_Stop6]");
	//	}
	//	
	//	if (M_NULL != MilDigitizer)
	//	{
	//		MdigFree(MilDigitizer);log("Cmd:[Mil_Stop7]");
	//	}
	//	MappControl(M_ERROR, M_PRINT_ENABLE);
	//	if ((MilSystem) != M_NULL)                                                
	//	{                                                                      
	//		MsysFree(MilSystem); 
	//		log("Cmd:[Mil_Stop8]");
	//	}                                                                              
	//	/* free the application */                                               
	//	if ((MilApplication) != M_NULL)                                          
	//	{                                                                      
	//		MappFree(MilApplication);
	//		log("Cmd:[Mil_Stop9]");
	//	}
	//}
	do_close=1;
	return TRUE;
}
int Img_Mil::destroy(){
	MdigProcess(MilDigitizer, MilGrabBufferList, MilGrabBufferListSize,
		M_STOP, M_DEFAULT, ProcessingFunction, &UserHookData);
	return TRUE;
}
