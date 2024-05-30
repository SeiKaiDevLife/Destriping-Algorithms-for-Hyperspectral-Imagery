#pragma once
#include "pch.h"
#include "Cal.h"

#define PI 3.1415926535

/*	��	ʼ	��	*/
/******************************************************************************************************************************************************************************************************************************************************/

//��ʼ�����������ø��ֱ���
void Parameter::Init()
{
	if (pDataOri)
		DeleteFunction(pDataOri, iBandNum);

	if (pDataPre)
		DeleteFunction(pDataPre, iBandNum);

	iHeight = 0;
	iWidth = 0;
	iBandNum = 0;

	free(dMaxValue);
	dMaxValue = NULL;
	free(dMinValue);
	dMinValue = NULL;

	MPSNR = 0;
	MSSIM = 0;
	MSAD = 0;
	sName = "";
}




/*	��	��	��	��	*/
/******************************************************************************************************************************************************************************************************************************************************/

//ԭʼͼ�����뺯��
//sPathInOri��ԭʼͼ��Ķ�ȡ·��
//����ֵ��0�����ȡʧ�ܣ�1�����ȡ�ɹ�
int Parameter::Set_pDataOri(CString sPathInOri)
{
	iflag = 1;

	if (pDataOri)
		DeleteFunction(pDataOri, iBandNum);

	pDataOri = ReadTifIn(sPathInOri);
	if (pDataOri != NULL)
		NormDataOri(pDataOri);
	else
		return 0;

	return 1;
}

//ԭʼͼ�����ݹ�һ������
//pDataOri�����ԭʼͼ��Ķ���ָ��
void Parameter::NormDataOri(double** pDataOri)
{
	if (dMaxValue != NULL)
	{
		free(dMaxValue);
		dMaxValue = NULL;
	}
	if (dMinValue != NULL)
	{
		free(dMinValue);
		dMinValue = NULL;
	}

	dMaxValue = (double*)malloc(iBandNum * sizeof(double));
	dMinValue = (double*)malloc(iBandNum * sizeof(double));

	for (int ii = 0; ii < iBandNum; ii++)
	{
		*(dMaxValue + ii) = pDataOri[ii][0];
		*(dMinValue + ii) = pDataOri[ii][0];
	}

	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			if (dMaxValue[ii] < pDataOri[ii][jj])
				dMaxValue[ii] = pDataOri[ii][jj];
			if (dMinValue[ii] > pDataOri[ii][jj])
				dMinValue[ii] = pDataOri[ii][jj];
		}
	}

	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			pDataOri[ii][jj] = (pDataOri[ii][jj] - dMinValue[ii]) / (dMaxValue[ii] - dMinValue[ii]);
		}
	}
}

//ȥ��ͼ�����뺯��
//sPathInPre��ȥ��ͼ��Ķ�ȡ·��
//����ֵ��0�����ȡʧ�ܣ�1�����ȡ�ɹ�
int Parameter::Set_pDataPre(CString sPathInPre)
{
	iflag = 0;

	if (pDataPre)
		DeleteFunction(pDataPre, iBandNum);

	pDataPre = ReadTifIn(sPathInPre);
	if (pDataPre != NULL)
		NormDataPre(pDataPre);
	else
		return 0;

	return 1;
}

//ȥ��ͼ�����ݹ�һ������
//pDataPre�����ȥ��ͼ��Ķ���ָ��
void Parameter::NormDataPre(double** pDataPre)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			pDataPre[ii][jj] = (pDataPre[ii][jj] - dMinValue[ii]) / (dMaxValue[ii] - dMinValue[ii]);
		}
	}
}




/*	��	��	��	��	*/
/******************************************************************************************************************************************************************************************************************************************************/

//���������ܺ�����������������������ֲ���
//����ֵ��0�������ʧ�ܣ�1�������ɹ�
double Parameter::Cal_Para()
{
	if (pDataOri != NULL && pDataPre != NULL)
	{
		MPSNR = Cal_MPSNR(pDataOri, pDataPre);
		MSSIM = Cal_MSSIM(pDataOri, pDataPre);
		MSAD = Cal_MSAD(pDataOri, pDataPre);
	}
	else
		return 0;

	return 1;
}

//MPSNR���㺯��
//pDataOri�����ԭʼͼ��Ķ���ָ�룻 pDataPre�����ȥ��ͼ��Ķ���ָ��
//����ֵ��MPSNR������ֵ
double Parameter::Cal_MPSNR(double** pDataOri, double** pDataPre)
{
	double dResult = 0;

	for (int ii = 0; ii < iBandNum; ii++)
	{
		double dSum = 0;
		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				dSum = dSum + 
					(pDataOri[ii][jj * iWidth + kk] - pDataPre[ii][jj * iWidth + kk]) * 
					(pDataOri[ii][jj * iWidth + kk] - pDataPre[ii][jj * iWidth + kk]);
			}
		}
		double dMSE = dSum / (iHeight * iWidth);
		dResult = dResult + 10 * log10((pow(2, iGrayscale) - 1) * (pow(2, iGrayscale) - 1) / dMSE);
	}

	return dResult / iBandNum;
}

//MSSIM���㺯��
//pDataOri�����ԭʼͼ��Ķ���ָ�룻 pDataPre�����ȥ��ͼ��Ķ���ָ��
//����ֵ��MSSIM������ֵ
double Parameter::Cal_MSSIM(double** pDataOri, double** pDataPre)
{
	double dResult = 0;
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double dSumOri = 0;
		double dSum2Ori = 0;
		double dSumPre = 0;
		double dSum2Pre = 0;
		double dSumPlus = 0;
		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				dSumOri = dSumOri + pDataOri[ii][jj * iWidth + kk];
				dSum2Ori = dSum2Ori + pDataOri[ii][jj * iWidth + kk] * pDataOri[ii][jj * iWidth + kk];
				dSumPre = dSumPre + pDataPre[ii][jj * iWidth + kk];
				dSum2Pre = dSum2Pre + pDataPre[ii][jj * iWidth + kk] * pDataPre[ii][jj * iWidth + kk];
				dSumPlus = dSumPlus + pDataOri[ii][jj * iWidth + kk] * pDataPre[ii][jj * iWidth + kk];
			}
		}

		double dAveOri = dSumOri / (iHeight * iWidth);
		double dAve2Ori = dSum2Ori / (iHeight * iWidth);
		double dAvePre = dSumPre / (iHeight * iWidth);
		double dAve2Pre = dSum2Pre / (iHeight * iWidth);
		double dAvePlus = dSumPlus / (iHeight * iWidth);

		double dVarOri = dAve2Ori - dAveOri * dAveOri;
		double dVarPre = dAve2Pre - dAvePre * dAvePre;

		double dCov = dAvePlus - dAveOri * dAvePre;

		dResult = dResult + ((2 * dAveOri * dAvePre + 0.0001) * (2 * dCov + 0.0003)) / 
			((dAveOri * dAveOri + dAvePre * dAvePre + 0.0001) * (dVarOri + dVarPre + 0.0003));
	}

	return dResult / iBandNum;
}

//MSAD���㺯��
//pDataOri�����ԭʼͼ��Ķ���ָ�룻 pDataPre�����ȥ��ͼ��Ķ���ָ��
//����ֵ��MSAD������ֵ
double Parameter::Cal_MSAD(double** pDataOri, double** pDataPre)
{
	double dResult = 0;

	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		double xy = 0;
		double xx = 0;
		double yy = 0;

		for (int jj = 0; jj < iBandNum; jj++)
		{
			xy = xy + pDataOri[jj][ii] * pDataPre[jj][ii];
			xx = xx + pDataOri[jj][ii] * pDataOri[jj][ii];
			yy = yy + pDataPre[jj][ii] * pDataPre[jj][ii];
		}

		xx = sqrt(xx);
		yy = sqrt(yy);

		dResult = dResult + acos(xy / (xx * yy)) / PI * 180;
	}

	return dResult / (iHeight * iWidth);
}




/*	��	��	��	��	*/
/******************************************************************************************************************************************************************************************************************************************************/

//MPSNR������ȡ����
//����ֵ��MPSNR������ֵ
double Parameter::Get_MPSNR()
{
	return MPSNR;
}

//MSSIM������ȡ����
//����ֵ��MSSIM������ֵ
double Parameter::Get_MSSIM()
{
	return MSSIM;
}

//MSAD������ȡ����
//����ֵ��MSAD������ֵ
double Parameter::Get_MSAD()
{
	return MSAD;
}




/*	��	��	��	��	��	��	*/
/******************************************************************************************************************************************************************************************************************************************************/

//����ڵ㺯��������������Ϊ�ڵ���뵽�������ĩβ
//sInputName��������¼������
//����ֵ�����óɹ�δ1������ʧ��Ϊ0
int Parameter::Set_Node(CString sInputName)
{
	ParameterNode* NewNode = new ParameterNode;
	if (NewNode)
	{
		NewNode->sDataName = sInputName;
		NewNode->dMPSNR = MPSNR;
		NewNode->dMSSIM = MSSIM;
		NewNode->dMSAD = MSAD;
		NewNode->NextNode = NULL;
	}
	else
	{
		AfxMessageBox("�ڴ��������");
		return 0;
	}

	ParaPreNode->NextNode->NextNode = NewNode;
	ParaPreNode->NextNode = NewNode;

	iNodeNum++;

	return 1;
}

//ɾ���ڵ㺯����ɾ����Ӧλ�õĽڵ�
//iNum����Ҫɾ���Ľڵ�����
//����ֵ��0����ɾ��ʧ�ܣ�1����ɾ���ɹ�
int Parameter::DeleteNode(int iNum)
{
	if (iNum > iNodeNum)
	{
		return 0;
	}

	else if (iNum < iNodeNum)
	{
		ParaSet1Node->NextNode = ParaHeadNode;
		for (int ii = 0; ii < iNum - 1; ii++)
		{
			ParaSet1Node->NextNode = ParaSet1Node->NextNode->NextNode;
		}
		ParaSet2Node->NextNode = ParaSet1Node->NextNode->NextNode;

		ParaSet1Node->NextNode->NextNode = ParaSet2Node->NextNode->NextNode;

		ParaSet2Node->NextNode->NextNode = NULL;

		free(ParaSet2Node->NextNode);

		ParaSet1Node->NextNode = NULL;
		ParaSet2Node->NextNode = NULL;

		iNodeNum--;
	}

	else if (iNum == iNodeNum)
	{
		ParaSet1Node->NextNode = ParaHeadNode;
		for (int ii = 0; ii < iNum - 1; ii++)
		{
			ParaSet1Node->NextNode = ParaSet1Node->NextNode->NextNode;
		}
		ParaSet1Node->NextNode->NextNode = NULL;

		free(ParaPreNode->NextNode);

		ParaPreNode->NextNode = ParaSet1Node->NextNode;

		iNodeNum--;
	}

	return 1;

}

//�޸Ľڵ㺯�����޸Ķ�Ӧλ�õĽڵ������
//iNum����Ҫ�޸ĵĽڵ����ţ� sChangeName��������¼��������
//����ֵ��0�����޸�ʧ�ܣ�1�����޸ĳɹ�
int Parameter::ChangeNode(int iNum, CString sChangeName)
{
	if (iNum > iNodeNum)
	{
		return 0;
	}

	else if (iNum <= iNodeNum)
	{
		ParaSet1Node->NextNode = ParaHeadNode;
		for (int ii = 0; ii < iNum; ii++)
		{
			ParaSet1Node->NextNode = ParaSet1Node->NextNode->NextNode;
		}
		ParaSet1Node->NextNode->sDataName = sChangeName;
	}
	return 1;
}

//��ȡ����ͷ������������ͷ���أ������������չʾ
//����ֵ��ParameterNode��ʽ������ͷָ��
ParameterNode* Parameter::Get_Node()
{
	return ParaHeadNode;
}

//��ȡ������ĳһ���ڵ������
//iNum����Ҫ��ȡ�Ľڵ�����
//����ֵ����Ӧ��Žڵ������
CString Parameter::Get_NodeName(int iNum)
{
	ParaSet1Node->NextNode = ParaHeadNode;
	for (int ii = 0; ii < iNum; ii++)
	{
		ParaSet1Node->NextNode = ParaSet1Node->NextNode->NextNode;
	}

	return ParaSet1Node->NextNode->sDataName;
}

//ɾ��������������ɾ����NodeΪͷ����������
//Node����Ҫɾ�������������ͷ
//����ֵ��0
int Parameter::DeleteNodeList(ParameterNode* Node)
{
	if (Node->NextNode != NULL)
		DeleteNodeList(Node->NextNode);

	free(Node);
	return 0;
}




/*	��	��	��	ֵ	��	��	��	*/
/******************************************************************************************************************************************************************************************************************************************************/

//����ĳһ����ÿ����Ԫ�Ҷ�ƽ��ֵ�������Ĺ�ϵ
//sPathOut���ļ�������·���� iNum��������
//����ֵ��0�������ɹ���1����iNum���ڲ�������2����iNumС�ڲ�������3�����ļ�����ʧ��
int Parameter::StripeSpec(CString sPathOut, int iNum)
{
	if (iNum > iBandNum)
	{
		return 1;
	}

	if (iNum < 1)
	{
		return 2;
	}

	CStdioFile cfLogFile;
	if (PathFileExists(sPathOut))
	{
		DeleteFile(sPathOut);
	}

	if (!cfLogFile.Open(sPathOut, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, NULL))
	{
		return 3;
	}

	CString Text;

	Text = "Column";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\t\t");

	Text = "ValueOri";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\t\t");

	Text = "ValuePre";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\r\n");

	for (int ii = 0; ii < iWidth; ii++)
	{
		Text.Format("%d", ii + 1);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\t\t");

		double dValueOri = 0;
		double dValuePre = 0;

		for (int jj = 0; jj < iHeight; jj++)
		{
			dValueOri = dValueOri + pDataOri[iNum - 1][jj * iWidth + ii];
			dValuePre = dValuePre + pDataPre[iNum - 1][jj * iWidth + ii];
		}

		dValueOri = dValueOri / iHeight;
		dValuePre = dValuePre / iHeight;

		dValueOri = dValueOri * (dMaxValue[iNum - 1] - dMinValue[iNum - 1]) + dMinValue[iNum - 1];
		dValuePre = dValuePre * (dMaxValue[iNum - 1] - dMinValue[iNum - 1]) + dMinValue[iNum - 1];

		Text.Format("%.2f", dValueOri);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\t\t");

		Text.Format("%.2f", dValuePre);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\r\n");
	}

	return 0;
}

//�������в���ÿ����Ԫ�Ҷ�ƽ��ֵ�������Ĺ�ϵ
//sPathOut���ļ�������·��
//����ֵ��0�������ɹ���1�����ļ�����ʧ��
int Parameter::StripeSpecMean(CString sPathOut)
{
	CStdioFile cfLogFile;
	if (PathFileExists(sPathOut))
	{
		DeleteFile(sPathOut);
	}

	if (!cfLogFile.Open(sPathOut, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, NULL))
	{
		return 1;
	}

	CString Text;

	Text = "Column";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\t\t");

	Text = "ValueOri";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\t\t");

	Text = "ValuePre";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\r\n");

	for (int ii = 0; ii < iWidth; ii++)
	{
		Text.Format("%d", ii + 1);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\t\t");

		double dValueOri = 0;
		double dValuePre = 0;

		for (int jj = 0; jj < iBandNum; jj++)
		{
			double TempOri = 0;
			double TempPre = 0;

			for (int kk = 0; kk < iHeight; kk++)
			{
				TempOri = TempOri + pDataOri[jj][kk * iWidth + ii];
				TempPre = TempPre + pDataPre[jj][kk * iWidth + ii];
			}

			TempOri = TempOri / iHeight;
			TempPre = TempPre / iHeight;

			TempOri = TempOri * (dMaxValue[jj] - dMinValue[jj]) + dMinValue[jj];
			TempPre = TempPre * (dMaxValue[jj] - dMinValue[jj]) + dMinValue[jj];

			dValueOri = dValueOri + TempOri;
			dValuePre = dValuePre + TempPre;
		}

		dValueOri = dValueOri / iBandNum;
		dValuePre = dValuePre / iBandNum;

		Text.Format("%.2f", dValueOri);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\t\t");

		Text.Format("%.2f", dValuePre);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\r\n");
	}

	return 0;
}

//����ĳһ��Ԫ����ֵ��Ҷ�ֵ�Ĺ�ϵ
//sPathOut���ļ�������·���� iHeightNum���߶�λ�ã� iWidthNum�����λ��
//����ֵ��0�������ɹ���1������ʧ��
int Parameter::PixelSpec(CString sPathOut, int iHeightNum, int iWidthNum)
{
	if (iHeightNum > iHeight)
	{
		return 1;
	}

	if (iHeightNum < 1)
	{
		return 1;
	}

	if (iWidthNum > iWidth)
	{
		return 1;
	}

	if (iWidthNum < 1)
	{
		return 1;
	}

	CStdioFile cfLogFile;
	if (PathFileExists(sPathOut))
	{
		DeleteFile(sPathOut);
	}

	if (!cfLogFile.Open(sPathOut, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, NULL))
	{
		return 1;
	}

	CString Text;

	Text = "BandNum";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\t\t");

	Text = "ValueOri";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\t\t");

	Text = "ValuePre";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\r\n");

	for (int ii = 0; ii < iBandNum; ii++)
	{
		double dDataOri = pDataOri[ii][iHeightNum * iWidth + iWidthNum]
			* (dMaxValue[ii] - dMinValue[ii]) + dMinValue[ii];
		double dDataPre = pDataPre[ii][iHeightNum * iWidth + iWidthNum]
			* (dMaxValue[ii] - dMinValue[ii]) + dMinValue[ii];

		Text.Format("%d", ii + 1);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\t\t");

		Text.Format("%.2f", dDataOri);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\t\t");

		Text.Format("%.2f", dDataPre);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\r\n");
	}

	return 0;
}


/*	��	��	��	��	��	��	��	��	*/
/******************************************************************************************************************************************************************************************************************************************************/

//�����ڴ�ռ亯��
//iHei���߶ȣ� iWid����ȣ� iBNm����������
//����ֵ��double���͵Ķ���ָ�룬Ϊ���ٵ��ڴ�ռ��ָ��
double** Parameter::MallocFunction(int iHei, int iWid, int iBNm)
{
	double** pData = (double**)malloc(iBNm * sizeof(double*));
	if (pData)
	{
		for (int ii = 0; ii < iBNm; ii++)
		{
			double* TempData;
			TempData = (double*)malloc(iHei * iWid * sizeof(double));
			if (TempData)
			{
				for (int jj = 0; jj < iHei * iWid; jj++)
				{
					*(TempData + jj) = 0;
				}
				*(pData + ii) = TempData;
			}
			else
			{
				AfxMessageBox("�����ڴ�ռ����");
				return NULL;
			}
		}
		return pData;
	}

	AfxMessageBox("�����ڴ�ռ����");
	return NULL;
}

//ɾ���ڴ�ռ亯��
//pData����Ҫɾ���ڴ�ռ�Ķ���ָ�룻 iBNm����������
void Parameter::DeleteFunction(double** pData, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		free(pData[ii]);
		pData[ii] = NULL;
	}

	free(pData);
	pData = NULL;

	return;
}




/*	��	��	��	��	��	��	��	��	*/
/******************************************************************************************************************************************************************************************************************************************************/

//��ȡtif�ļ�����
//sTifPathIn����ȡtif�ļ���·��
//����ֵ��double���͵Ķ���ָ�룬��Ŷ�ȡ��ͼ����Ϣ
//�������ֵΪNULL��������˸��ֿ��ܵĶ�ȡ����
double** Parameter::ReadTifIn(CString sTifPathIn)
{
	double** pData;

	GDALDataset* pDatasetInput;
	pDatasetInput = (GDALDataset*)GDALOpen(sTifPathIn, GA_ReadOnly);
	if (!pDatasetInput)
	{
		AfxMessageBox("Can not open the image file!");
		return NULL;
	}

	if (iflag == 1)
	{
		iWidth = pDatasetInput->GetRasterXSize();
		iHeight = pDatasetInput->GetRasterYSize();
		iBandNum = pDatasetInput->GetRasterCount();
	}
	else if (iflag == 0)
	{
		int iTempWidth = pDatasetInput->GetRasterXSize();
		if (iTempWidth != iWidth)
		{
			AfxMessageBox("��ͼ���Ȳ�һ��");
			return NULL;
		}
			
		int iTempHeight = pDatasetInput->GetRasterYSize();
		if (iTempHeight != iHeight)
		{
			AfxMessageBox("��ͼ��߶Ȳ�һ��");
			return NULL;
		}

		int iTempBandNum = pDatasetInput->GetRasterCount();
		if (iTempBandNum != iBandNum)
		{
			AfxMessageBox("��ͼ�񲨶�������һ��");
			return NULL;
		}
	}

	GDALDataType eDataType = pDatasetInput->GetRasterBand(1)->GetRasterDataType();	//GDT_Int16

	switch (eDataType)
	{
	case GDT_Byte:
	{
		BYTE** pInputData;
		pInputData = (BYTE**)malloc(iBandNum * sizeof(BYTE*));
		if (pInputData)
		{
			for (int ii = 0; ii < iBandNum; ii++)
			{
				BYTE* TempData;
				TempData = (BYTE*)malloc(iWidth * iHeight * sizeof(BYTE));
				if (TempData)
				{
					for (int jj = 0; jj < iHeight * iWidth; jj++)
					{
						*(TempData + jj) = 0;
					}
					*(pInputData + ii) = TempData;
				}
				else
				{
					AfxMessageBox("�����ڴ�ռ����");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//�����ȡʧ�ܣ��򷵻�
				{
					AfxMessageBox("��ȡͼ�����");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("�����ڴ�ռ����");
			return NULL;
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(*(pInputData + ii));
			*(pInputData + ii) = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_Int16:
	{
		short** pInputData;
		pInputData = (short**)malloc(iBandNum * sizeof(short*));
		if (pInputData)
		{
			for (int ii = 0; ii < iBandNum; ii++)
			{
				short* TempData;
				TempData = (short*)malloc(iWidth * iHeight * sizeof(short));
				if (TempData)
				{
					for (int jj = 0; jj < iHeight * iWidth; jj++)
					{
						*(TempData + jj) = 0;
					}
					*(pInputData + ii) = TempData;
				}
				else
				{
					AfxMessageBox("�����ڴ�ռ����");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//�����ȡʧ�ܣ��򷵻�
				{
					AfxMessageBox("��ȡͼ�����");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("�����ڴ�ռ����");
			return NULL;
		}
		
		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_UInt16:
	{
		unsigned short** pInputData;
		pInputData = (unsigned short**)malloc(iBandNum * sizeof(unsigned short*));
		if (pInputData)
		{
			for (int ii = 0; ii < iBandNum; ii++)
			{
				unsigned short* TempData;
				TempData = (unsigned short*)malloc(iWidth * iHeight * sizeof(unsigned short));
				if (TempData)
				{
					for (int jj = 0; jj < iHeight * iWidth; jj++)
					{
						*(TempData + jj) = 0;
					}
					*(pInputData + ii) = TempData;
				}
				else
				{
					AfxMessageBox("�����ڴ�ռ����");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//�����ȡʧ�ܣ��򷵻�
				{
					AfxMessageBox("��ȡͼ�����");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("�����ڴ�ռ����");
			return NULL;
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_Int32:
	{
		int** pInputData;
		pInputData = (int**)malloc(iBandNum * sizeof(int*));
		if (pInputData)
		{
			for (int ii = 0; ii < iBandNum; ii++)
			{
				int* TempData;
				TempData = (int*)malloc(iWidth * iHeight * sizeof(int));
				if (TempData)
				{
					for (int jj = 0; jj < iHeight * iWidth; jj++)
					{
						*(TempData + jj) = 0;
					}
					*(pInputData + ii) = TempData;
				}
				else
				{
					AfxMessageBox("�����ڴ�ռ����");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//�����ȡʧ�ܣ��򷵻�
				{
					AfxMessageBox("��ȡͼ�����");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("�����ڴ�ռ����");
			return NULL;
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_UInt32:
	{
		unsigned long** pInputData;
		pInputData = (unsigned long**)malloc(iBandNum * sizeof(unsigned long*));
		if (pInputData)
		{
			for (int ii = 0; ii < iBandNum; ii++)
			{
				unsigned long* TempData;
				TempData = (unsigned long*)malloc(iWidth * iHeight * sizeof(unsigned long));
				if (TempData)
				{
					for (int jj = 0; jj < iHeight * iWidth; jj++)
					{
						*(TempData + jj) = 0;
					}
					*(pInputData + ii) = TempData;
				}
				else
				{
					AfxMessageBox("�����ڴ�ռ����");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//�����ȡʧ�ܣ��򷵻�
				{
					AfxMessageBox("��ȡͼ�����");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("�����ڴ�ռ����");
			return NULL;
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_Float32:
	{
		float** pInputData;
		pInputData = (float**)malloc(iBandNum * sizeof(float*));
		if (pInputData)
		{
			for (int ii = 0; ii < iBandNum; ii++)
			{
				float* TempData;
				TempData = (float*)malloc(iWidth * iHeight * sizeof(float));
				if (TempData)
				{
					for (int jj = 0; jj < iHeight * iWidth; jj++)
					{
						*(TempData + jj) = 0;
					}
					*(pInputData + ii) = TempData;
				}
				else
				{
					AfxMessageBox("�����ڴ�ռ����");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//�����ȡʧ�ܣ��򷵻�
				{
					AfxMessageBox("��ȡͼ�����");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("�����ڴ�ռ����");
			return NULL;
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	case GDT_Float64:
	{
		double** pInputData;
		pInputData = (double**)malloc(iBandNum * sizeof(double*));
		if (pInputData)
		{
			for (int ii = 0; ii < iBandNum; ii++)
			{
				double* TempData;
				TempData = (double*)malloc(iWidth * iHeight * sizeof(double));
				if (TempData)
				{
					for (int jj = 0; jj < iHeight * iWidth; jj++)
					{
						*(TempData + jj) = 0;
					}
					*(pInputData + ii) = TempData;
				}
				else
				{
					AfxMessageBox("�����ڴ�ռ����");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//�����ȡʧ�ܣ��򷵻�
				{
					AfxMessageBox("��ȡͼ�����");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("�����ڴ�ռ����");
			return NULL;
		}

		pData = MallocFunction(iHeight, iWidth, iBandNum);

		Copy(pInputData, pData, iHeight, iWidth, iBandNum);

		for (int ii = 0; ii < iBandNum; ii++)
		{
			free(pInputData[ii]);
			pInputData[ii] = NULL;
		}
		free(pInputData);
		pInputData = NULL;

		break;
	}
	default:
	{
		AfxMessageBox("ͼ���������ʹ�������ͼ��");
		GDALClose(pDatasetInput);
		return NULL;
	}
	}

	GDALClose(pDatasetInput);

	return pData;
}

//�����������������
//sPathOut�������������ļ���·��
//����ֵ��0�������ʧ�ܣ�1��������ɹ�
int Parameter::WriteData(CString sPathOut)
{
	CStdioFile cfLogFile;
	if (PathFileExists(sPathOut))
	{
		DeleteFile(sPathOut);
	}

	if (!cfLogFile.Open(sPathOut, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, NULL))
	{
		return 0;
	}
	ParameterNode* DataNode = new ParameterNode;
	if (DataNode)
	{
		DataNode->NextNode = ParaHeadNode->NextNode;
	}
	else
	{
		return 0;
	}

	CString Text;
	Text = "Name";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\t\t");

	Text = "MPSNR";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\t\t");

	Text = "MSSIM";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\t\t");

	Text = "MSAD";
	cfLogFile.WriteString(Text);
	cfLogFile.WriteString("\r\n");

	while (DataNode->NextNode != NULL)
	{

		Text = DataNode->NextNode->sDataName;
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\t\t");

		Text.Format("%.2f", DataNode->NextNode->dMPSNR);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\t\t");

		Text.Format("%.2f", DataNode->NextNode->dMSSIM);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\t\t");

		Text.Format("%.2f", DataNode->NextNode->dMSAD);
		cfLogFile.WriteString(Text);
		cfLogFile.WriteString("\r\n");

		DataNode->NextNode = DataNode->NextNode->NextNode;
	}

	delete(DataNode);

	cfLogFile.Close();

	return 1;
}




/*	��	��	��	��	*/
/******************************************************************************************************************************************************************************************************************************************************/

//�������ƺ�����BYTE->double��
//pInputData������������ pOutputData����������� iHei���ߣ� iWid���� iBNm����������
void Parameter::Copy(BYTE** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = (double)pInputData[ii][jj];
		}
	}

	return;
}

//�������ƺ�����short->double��
//pInputData������������ pOutputData����������� iHei���ߣ� iWid���� iBNm����������
void Parameter::Copy(short** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = (double)pInputData[ii][jj];
		}
	}

	return;
}

//�������ƺ�����unsigned short->double��
//pInputData������������ pOutputData����������� iHei���ߣ� iWid���� iBNm����������
void Parameter::Copy(unsigned short** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = (double)pInputData[ii][jj];
		}
	}

	return;
}

//�������ƺ�����int->double��
//pInputData������������ pOutputData����������� iHei���ߣ� iWid���� iBNm����������
void Parameter::Copy(int** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = (double)pInputData[ii][jj];
		}
	}

	return;
}

//�������ƺ�����unsigned long->double��
//pInputData������������ pOutputData����������� iHei���ߣ� iWid���� iBNm����������
void Parameter::Copy(unsigned long** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = (double)pInputData[ii][jj];
		}
	}

	return;
}

//�������ƺ�����float->double��
//pInputData������������ pOutputData����������� iHei���ߣ� iWid���� iBNm����������
void Parameter::Copy(float** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = (double)pInputData[ii][jj];
		}
	}

	return;
}

//�������ƺ�����double->double��
//pInputData������������ pOutputData����������� iHei���ߣ� iWid���� iBNm����������
void Parameter::Copy(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm)
{
	for (int ii = 0; ii < iBNm; ii++)
	{
		for (int jj = 0; jj < iHei * iWid; jj++)
		{
			pOutputData[ii][jj] = (double)pInputData[ii][jj];
		}
	}

	return;
}
