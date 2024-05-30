#pragma once
#include "gdal_priv.h"												//����GDAL��

struct ParameterNode												//����ڵ�ṹ
{
	CString sDataName = "";											//ͼ������
	double dMPSNR = 0;												//MPSNR����
	double dMSSIM = 0;												//MSSIM����
	double dMSAD = 0;												//MSAD����
	ParameterNode* NextNode = NULL;									//ָ����һ������
};

class Parameter														//����������
{
public:
	Parameter();													//���캯��
	~Parameter();													//��������

	void Init();													//��ʼ�����������ø��ֱ���
	
	double Cal_Para();												//���������ܺ�����������������������ֲ���
	double Get_MPSNR();												//MPSNR���㺯��
	double Get_MSSIM();												//MSSIM���㺯��
	double Get_MSAD();												//MSAD���㺯��

	int Set_pDataOri(CString sPathInOri);							//ԭʼͼ�����뺯��
	int Set_pDataPre(CString sPathInPre);							//ȥ��ͼ�����뺯��

	int Set_Node(CString sInputName);								//����ڵ㺯��������������Ϊ�ڵ���뵽�������ĩβ
	ParameterNode* Get_Node();										//��ȡ����ͷ������������ͷ���أ������������չʾ
	CString Get_NodeName(int iNum);									//��ȡ������ĳһ���ڵ������
	int DeleteNode(int iNum);										//ɾ��������������ɾ����NodeΪͷ����������
	int ChangeNode(int iNum, CString sChangeName);					//�޸Ľڵ㺯�����޸Ķ�Ӧλ�õĽڵ������
		
	int StripeSpec(CString sPathOut, int iNum);						//����ĳһ����ÿ����Ԫ�Ҷ�ƽ��ֵ�������Ĺ�ϵ
	int StripeSpecMean(CString sPathOut);							//�������в���ÿ����Ԫ�Ҷ�ƽ��ֵ�������Ĺ�ϵ

	int PixelSpec(CString sPathOut, int iHeightNum, int iWidthNum);	//����ĳһ��Ԫ����ֵ��Ҷ�ֵ�Ĺ�ϵ

	int WriteData(CString sPathOut);								//�����������������

private:		
	int iflag;														//�����ж����ڶ�ȡԭʼͼ����ȥ��ͼ��ı�־

	double** pDataOri;												//���ԭʼͼ��Ķ���ָ��
	double** pDataPre;												//���ȥ��ͼ��Ķ���ָ��

	double *dMaxValue;												//ԭʼͼ������лҶ�ֵ�����ֵ�����ڹ�һ��
	double *dMinValue;												//ԭʼͼ������лҶ�ֵ����Сֵ�����ڹ�һ��

	int iHeight;													//ͼ��߶�
	int iWidth;														//ͼ����
	int iBandNum;													//ͼ�񲨶�����
		
	int iGrayscale;													//ͼ��ɫ���һ����Ĭ��Ϊ1

	int iNodeNum;													//��Ų���������������ڵ����

	ParameterNode* ParaHeadNode;									//�������������ͷ
	ParameterNode* ParaPreNode;										//ָ���������β���Ľṹָ�룬��������½ڵ�
	ParameterNode* ParaSet1Node;									//�����ṹָ��1�����ָ��2����޸ĺ�ɾ������
	ParameterNode* ParaSet2Node;									//�����ṹָ��2�����ָ��1����޸ĺ�ɾ������

	double MPSNR;													//MPSNR����
	double MSSIM;													//MSSIM����
	double MSAD;													//MSAD����
	CString sName;													//��������

	void NormDataOri(double** pDataOri);							//ԭʼͼ�����ݹ�һ������
	void NormDataPre(double** pDataPre);							//ȥ��ͼ�����ݹ�һ������

	double Cal_MPSNR(double** pDataOri, double** pDataPre);			//MPSNR���㺯��
	double Cal_MSSIM(double** pDataOri, double** pDataPre);			//MSSIM���㺯��
	double Cal_MSAD(double** pDataOri, double** pDataPre);			//MSAD���㺯��

	int DeleteNodeList(ParameterNode *Node);						//ɾ��������������ɾ����NodeΪͷ����������

	double** MallocFunction(int iHei, int iWid, int iBNm);			//�����ڴ�ռ亯��
	void DeleteFunction(double** pData, int iBNm);					//ɾ���ڴ�ռ亯��
		
	double** ReadTifIn(CString sTifPathIn);							//��ȡtif�ļ�����

	/*	��	��	��	��	*/
	/*****************************************************************************************************************/
	void Copy(BYTE** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(short** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(unsigned short** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(int** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(unsigned long** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(float** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Copy(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
};

inline Parameter::Parameter():								//���캯������ʼ�����ֲ���
	MPSNR(0),MSSIM(0),MSAD(0),iHeight(0),iWidth(0),iBandNum(0),sName(""),iNodeNum(0),iGrayscale(1),iflag(0),pDataOri(NULL),pDataPre(NULL),dMaxValue(NULL),dMinValue(NULL)
{
	GDALAllRegister();										//��ʼ��GDAL��
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");		//ʹ��GDAL��Ĭ��ʹ��UTF8���룬��֧������·��

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
		AfxMessageBox("�����ڴ�ռ����");
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
		AfxMessageBox("�����ڴ�ռ����");
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
		AfxMessageBox("�����ڴ�ռ����");
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
		AfxMessageBox("�����ڴ�ռ����");
		return;
	}
}

inline Parameter::~Parameter()								//�����������ͷſռ�	
{
	DeleteNodeList(ParaHeadNode);
	free(ParaPreNode);

	Init();
}


