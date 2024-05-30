#pragma once
#include "gdal_priv.h"												//调用GDAL库

struct ParameterNode												//链表节点结构
{
	CString sDataName = "";											//图像名称
	double dMPSNR = 0;												//MPSNR参数
	double dMSSIM = 0;												//MSSIM参数
	double dMSAD = 0;												//MSAD参数
	ParameterNode* NextNode = NULL;									//指向下一个数据
};

class Parameter														//参数计算类
{
public:
	Parameter();													//构造函数
	~Parameter();													//析构函数

	void Init();													//初始化函数：重置各种变量
	
	double Cal_Para();												//参数计算总函数：调用其他函数计算各种参数
	double Get_MPSNR();												//MPSNR计算函数
	double Get_MSSIM();												//MSSIM计算函数
	double Get_MSAD();												//MSAD计算函数

	int Set_pDataOri(CString sPathInOri);							//原始图像输入函数
	int Set_pDataPre(CString sPathInPre);							//去噪图像输入函数

	int Set_Node(CString sInputName);								//加入节点函数：将新数据作为节点加入到链表的最末尾
	ParameterNode* Get_Node();										//获取链表头函数：将链表头返回，用以输出或者展示
	CString Get_NodeName(int iNum);									//获取链表中某一个节点的名称
	int DeleteNode(int iNum);										//删除整条链表函数：删除以Node为头的整条链表
	int ChangeNode(int iNum, CString sChangeName);					//修改节点函数：修改对应位置的节点的名字
		
	int StripeSpec(CString sPathOut, int iNum);						//导出某一波段每列像元灰度平均值与列数的关系
	int StripeSpecMean(CString sPathOut);							//导出所有波段每列像元灰度平均值与列数的关系

	int PixelSpec(CString sPathOut, int iHeightNum, int iWidthNum);	//导出某一像元波段值与灰度值的关系

	int WriteData(CString sPathOut);								//输出参数计算结果函数

private:		
	int iflag;														//用于判断正在读取原始图像还是去噪图像的标志

	double** pDataOri;												//存放原始图像的二重指针
	double** pDataPre;												//存放去噪图像的二重指针

	double *dMaxValue;												//原始图像的所有灰度值的最大值，用于归一化
	double *dMinValue;												//原始图像的所有灰度值的最小值，用于归一化

	int iHeight;													//图像高度
	int iWidth;														//图像宽度
	int iBandNum;													//图像波段数量
		
	int iGrayscale;													//图像色深，归一化后默认为1

	int iNodeNum;													//存放参数计算结果的链表节点个数

	ParameterNode* ParaHeadNode;									//参数链表的链表头
	ParameterNode* ParaPreNode;										//指向参数链表尾部的结构指针，用于添加新节点
	ParameterNode* ParaSet1Node;									//操作结构指针1，配合指针2完成修改和删除操作
	ParameterNode* ParaSet2Node;									//操作结构指针2，配合指针1完成修改和删除操作

	double MPSNR;													//MPSNR参数
	double MSSIM;													//MSSIM参数
	double MSAD;													//MSAD参数
	CString sName;													//操作名字

	void NormDataOri(double** pDataOri);							//原始图像数据归一化函数
	void NormDataPre(double** pDataPre);							//去噪图像数据归一化函数

	double Cal_MPSNR(double** pDataOri, double** pDataPre);			//MPSNR计算函数
	double Cal_MSSIM(double** pDataOri, double** pDataPre);			//MSSIM计算函数
	double Cal_MSAD(double** pDataOri, double** pDataPre);			//MSAD计算函数

	int DeleteNodeList(ParameterNode *Node);						//删除整条链表函数：删除以Node为头的整条链表

	double** MallocFunction(int iHei, int iWid, int iBNm);			//开辟内存空间函数
	void DeleteFunction(double** pData, int iBNm);					//删除内存空间函数
		
	double** ReadTifIn(CString sTifPathIn);							//读取tif文件函数

	/*	张	量	复	制	*/
	/*****************************************************************************************************************/
	void Copy(BYTE** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(short** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(unsigned short** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(int** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(unsigned long** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(float** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
};

inline Parameter::Parameter():								//构造函数，初始化各种参数
	MPSNR(0),MSSIM(0),MSAD(0),iHeight(0),iWidth(0),iBandNum(0),sName(""),iNodeNum(0),iGrayscale(1),iflag(0),pDataOri(NULL),pDataPre(NULL),dMaxValue(NULL),dMinValue(NULL)
{
	GDALAllRegister();										//初始化GDAL库
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");		//使得GDAL不默认使用UTF8编码，以支持中文路径

	ParaHeadNode = new ParameterNode; 
	ParaPreNode = new ParameterNode;
	ParaSet1Node = new ParameterNode;
	ParaSet2Node = new ParameterNode;

	if (ParaHeadNode)
	{
		ParaHeadNode->sDataName = "";
		ParaHeadNode->dMSAD = 0;
		ParaHeadNode->dMSSIM = 0;
		ParaHeadNode->dMPSNR = 0;
		ParaHeadNode->NextNode = NULL;
	}
	else
	{
		AfxMessageBox("开辟内存空间错误");
		return;
	}

	if (ParaPreNode)
	{
		ParaPreNode->dMSAD = 0;
		ParaPreNode->dMSSIM = 0;
		ParaPreNode->dMPSNR = 0;
		ParaPreNode->NextNode = ParaHeadNode;
	}
	else
	{
		AfxMessageBox("开辟内存空间错误");
		return;
	}

	if (ParaSet1Node)
	{
		ParaSet1Node->dMSAD = 0;
		ParaSet1Node->dMSSIM = 0;
		ParaSet1Node->dMPSNR = 0;
		ParaSet1Node->NextNode = ParaHeadNode;
	}
	else
	{
		AfxMessageBox("开辟内存空间错误");
		return;
	}

	if (ParaSet2Node)
	{
		ParaSet2Node->dMSAD = 0;
		ParaSet2Node->dMSSIM = 0;
		ParaSet2Node->dMPSNR = 0;
		ParaSet2Node->NextNode = ParaHeadNode;
	}
	else
	{
		AfxMessageBox("开辟内存空间错误");
		return;
	}
}

inline Parameter::~Parameter()								//析构函数，释放空间	
{
	DeleteNodeList(ParaHeadNode);
	free(ParaPreNode);

	Init();
}


