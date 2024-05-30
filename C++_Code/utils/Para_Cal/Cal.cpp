#pragma once
#include "pch.h"
#include "Cal.h"

#define PI 3.1415926535

/*	初	始	化	*/
/******************************************************************************************************************************************************************************************************************************************************/

//初始化函数：重置各种变量
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




/*	数	据	输	入	*/
/******************************************************************************************************************************************************************************************************************************************************/

//原始图像输入函数
//sPathInOri：原始图像的读取路径
//返回值：0代表读取失败，1代表读取成功
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

//原始图像数据归一化函数
//pDataOri：存放原始图像的二重指针
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

//去噪图像输入函数
//sPathInPre：去噪图像的读取路径
//返回值：0代表读取失败，1代表读取成功
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

//去噪图像数据归一化函数
//pDataPre：存放去噪图像的二重指针
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




/*	参	数	计	算	*/
/******************************************************************************************************************************************************************************************************************************************************/

//参数计算总函数：调用其他函数计算各种参数
//返回值：0代表计算失败，1代表计算成功
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

//MPSNR计算函数
//pDataOri：存放原始图像的二重指针； pDataPre：存放去噪图像的二重指针
//返回值：MPSNR参数数值
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

//MSSIM计算函数
//pDataOri：存放原始图像的二重指针； pDataPre：存放去噪图像的二重指针
//返回值：MSSIM参数数值
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

//MSAD计算函数
//pDataOri：存放原始图像的二重指针； pDataPre：存放去噪图像的二重指针
//返回值：MSAD参数数值
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




/*	参	数	传	输	*/
/******************************************************************************************************************************************************************************************************************************************************/

//MPSNR参数提取函数
//返回值：MPSNR参数数值
double Parameter::Get_MPSNR()
{
	return MPSNR;
}

//MSSIM参数提取函数
//返回值：MSSIM参数数值
double Parameter::Get_MSSIM()
{
	return MSSIM;
}

//MSAD参数提取函数
//返回值：MSAD参数数值
double Parameter::Get_MSAD()
{
	return MSAD;
}




/*	链	表	操	作	函	数	*/
/******************************************************************************************************************************************************************************************************************************************************/

//加入节点函数：将新数据作为节点加入到链表的最末尾
//sInputName：参数记录的名字
//返回值：设置成功未1，设置失败为0
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
		AfxMessageBox("内存申请错误");
		return 0;
	}

	ParaPreNode->NextNode->NextNode = NewNode;
	ParaPreNode->NextNode = NewNode;

	iNodeNum++;

	return 1;
}

//删除节点函数：删除对应位置的节点
//iNum：需要删除的节点的序号
//返回值：0代表删除失败，1代表删除成功
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

//修改节点函数：修改对应位置的节点的名字
//iNum：需要修改的节点的序号； sChangeName：参数记录的新名字
//返回值：0代表修改失败，1代表修改成功
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

//获取链表头函数：将链表头返回，用以输出或者展示
//返回值：ParameterNode格式的链表头指针
ParameterNode* Parameter::Get_Node()
{
	return ParaHeadNode;
}

//获取链表中某一个节点的名称
//iNum：需要获取的节点的序号
//返回值：对应序号节点的名称
CString Parameter::Get_NodeName(int iNum)
{
	ParaSet1Node->NextNode = ParaHeadNode;
	for (int ii = 0; ii < iNum; ii++)
	{
		ParaSet1Node->NextNode = ParaSet1Node->NextNode->NextNode;
	}

	return ParaSet1Node->NextNode->sDataName;
}

//删除整条链表函数：删除以Node为头的整条链表
//Node：需要删除的链表的链表头
//返回值：0
int Parameter::DeleteNodeList(ParameterNode* Node)
{
	if (Node->NextNode != NULL)
		DeleteNodeList(Node->NextNode);

	free(Node);
	return 0;
}




/*	像	素	数	值	谱	导	出	*/
/******************************************************************************************************************************************************************************************************************************************************/

//导出某一波段每列像元灰度平均值与列数的关系
//sPathOut：文件导出的路径； iNum：波段数
//返回值：0代表导出成功，1代表iNum大于波段数，2代表iNum小于波段数，3代表文件创建失败
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

//导出所有波段每列像元灰度平均值与列数的关系
//sPathOut：文件导出的路径
//返回值：0代表导出成功，1代表文件创建失败
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

//导出某一像元波段值与灰度值的关系
//sPathOut：文件导出的路径； iHeightNum：高度位置； iWidthNum：宽度位置
//返回值：0代表导出成功，1代表导出失败
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


/*	内	存	空	间	管	理	函	数	*/
/******************************************************************************************************************************************************************************************************************************************************/

//开辟内存空间函数
//iHei：高度； iWid：宽度； iBNm：波段数量
//返回值：double类型的二重指针，为开辟的内存空间的指针
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
				AfxMessageBox("开辟内存空间错误");
				return NULL;
			}
		}
		return pData;
	}

	AfxMessageBox("开辟内存空间错误");
	return NULL;
}

//删除内存空间函数
//pData：需要删除内存空间的二重指针； iBNm：波段数量
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




/*	数	据	输	入	输	出	函	数	*/
/******************************************************************************************************************************************************************************************************************************************************/

//读取tif文件函数
//sTifPathIn：读取tif文件的路径
//返回值：double类型的二重指针，存放读取的图像信息
//如果返回值为NULL，则出现了各种可能的读取错误
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
			AfxMessageBox("两图像宽度不一致");
			return NULL;
		}
			
		int iTempHeight = pDatasetInput->GetRasterYSize();
		if (iTempHeight != iHeight)
		{
			AfxMessageBox("两图像高度不一致");
			return NULL;
		}

		int iTempBandNum = pDatasetInput->GetRasterCount();
		if (iTempBandNum != iBandNum)
		{
			AfxMessageBox("两图像波段数量不一致");
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
					AfxMessageBox("开辟内存空间错误");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//如果读取失败，则返回
				{
					AfxMessageBox("读取图像错误");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("开辟内存空间错误");
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
					AfxMessageBox("开辟内存空间错误");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//如果读取失败，则返回
				{
					AfxMessageBox("读取图像错误");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("开辟内存空间错误");
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
					AfxMessageBox("开辟内存空间错误");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//如果读取失败，则返回
				{
					AfxMessageBox("读取图像错误");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("开辟内存空间错误");
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
					AfxMessageBox("开辟内存空间错误");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//如果读取失败，则返回
				{
					AfxMessageBox("读取图像错误");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("开辟内存空间错误");
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
					AfxMessageBox("开辟内存空间错误");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//如果读取失败，则返回
				{
					AfxMessageBox("读取图像错误");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("开辟内存空间错误");
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
					AfxMessageBox("开辟内存空间错误");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//如果读取失败，则返回
				{
					AfxMessageBox("读取图像错误");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("开辟内存空间错误");
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
					AfxMessageBox("开辟内存空间错误");
					return NULL;
				}
			}

			GDALRasterBand* pBandInput;
			for (int ii = 1; ii <= iBandNum; ii++)
			{
				pBandInput = pDatasetInput->GetRasterBand(ii);
				CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, *(pInputData + ii - 1), iWidth, iHeight, eDataType, 0, 0);
				if (ReadErr == CE_Failure)	//如果读取失败，则返回
				{
					AfxMessageBox("读取图像错误");
					return NULL;
				}
			}
		}
		else
		{
			AfxMessageBox("开辟内存空间错误");
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
		AfxMessageBox("图像数据类型错误，请检查图像");
		GDALClose(pDatasetInput);
		return NULL;
	}
	}

	GDALClose(pDatasetInput);

	return pData;
}

//输出参数计算结果函数
//sPathOut：输出参数结果文件的路径
//返回值：0代表输出失败，1代表输出成功
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




/*	张	量	复	制	*/
/******************************************************************************************************************************************************************************************************************************************************/

//张量复制函数（BYTE->double）
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
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

//张量复制函数（short->double）
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
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

//张量复制函数（unsigned short->double）
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
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

//张量复制函数（int->double）
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
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

//张量复制函数（unsigned long->double）
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
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

//张量复制函数（float->double）
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
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

//张量复制函数（double->double）
//pInputData：输入张量； pOutputData：输出张量； iHei：高； iWid：宽； iBNm：波段数量
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
