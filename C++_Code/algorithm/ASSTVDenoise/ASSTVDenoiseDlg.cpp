
// ASSTVDenoiseDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ASSTVDenoise.h"
#include "ASSTVDenoiseDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CASSTVDenoiseDlg 对话框



CASSTVDenoiseDlg::CASSTVDenoiseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ASSTVDENOISE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CASSTVDenoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CASSTVDenoiseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ASSTV, &CASSTVDenoiseDlg::OnBnClickedButtonAsstv)
END_MESSAGE_MAP()


// CASSTVDenoiseDlg 消息处理程序

BOOL CASSTVDenoiseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//使得GDAL不默认使用UTF8编码，以支持中文路径

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CASSTVDenoiseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CASSTVDenoiseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CASSTVDenoiseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CASSTVDenoiseDlg::OnBnClickedButtonAsstv()
{
	/*程序开始，读取基本信息
	*********************************************************************************************************************************************************************************/

	//设置读取图像的路径和输出图像的路径
	CString sPathTifIn = "D:\\TestData\\TestImage\\60P.tif";
	//CString sPathTifIn = "D:\\TestData\\Result\\Test_NL_SN.tif";
	CString sPathTifOut = "D:\\TestData\\TestImage\\Out\\ASSTV";
	//CString sPathTifOut = "D:\\TestData\\TVDenoise\\Result\\Test_NL_SN.tif";

	//以只读方式打开图像
	GDALDataset* pDatasetInput;
	pDatasetInput = (GDALDataset*)GDALOpen(sPathTifIn, GA_ReadOnly);
	if (!pDatasetInput)
	{
		AfxMessageBox("Can not open the image file!");
		return;
	}

	//读取影像的基本信息
	iWidth = pDatasetInput->GetRasterXSize();	//宽度：512
	iHeight = pDatasetInput->GetRasterYSize();	//高度：512
	iBandNum = pDatasetInput->GetRasterCount();	//波段数量：1
	GDALDataType eDataType = pDatasetInput->GetRasterBand(1)->GetRasterDataType();	//GDT_Byte

	/*开辟内存用来存放输入和输出数据
	*********************************************************************************************************************************************************************************/

	//开辟存放图像数据的储存空间
	//BYTE* pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (BYTE*)malloc(iHeight * iWidth * sizeof(BYTE));
	//for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	//{
	//	pDataInput[ii] = NULL;
	//}

	//short** pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (short**)malloc(iBandNum * sizeof(short*));
	//for (int ii = 0; ii < iBandNum; ii++)	//初始化
	//{
	//	short* pTemp = NULL;
	//	pTemp = (short*)malloc(iHeight * iWidth * sizeof(short));
	//	for (int jj = 0; jj < iHeight * iWidth; jj++)
	//	{
	//		pTemp[jj] = 0;
	//	}
	//	pDataInput[ii] = pTemp;
	//}

	//int* pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (int*)malloc(iHeight * iWidth * sizeof(int));
	//for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
	//{
	//	pDataInput[ii] = NULL;
	//}

	//float** pDataInput = NULL;	//定义空间用来存放整个遥感数据
	//pDataInput = (float**)malloc(iBandNum * sizeof(float*));
	//for (int ii = 0; ii < iBandNum; ii++)	//初始化
	//{
	//	float* pTemp = NULL;
	//	pTemp = (float*)malloc(iHeight * iWidth * sizeof(float));
	//	for (int jj = 0; jj < iHeight * iWidth; jj++)
	//	{
	//		pTemp[jj] = 0;
	//	}
	//	pDataInput[ii] = pTemp;
	//}

	double** pDataInput = NULL;	//定义空间用来存放整个遥感数据
	pDataInput = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)	//初始化
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			pTemp[jj] = 0;
		}
		pDataInput[ii] = pTemp;
	}

	/*将需要读取的图像读入内存
	********************************************************************************************************************************************************************************/

	//读取图像
	for (int ii = 1; ii <= iBandNum; ii++)
	{
		GDALRasterBand* pBandInput = pDatasetInput->GetRasterBand(ii);
		double *pTemp = pDataInput[ii - 1];

		CPLErr ReadErr = pBandInput->RasterIO(GF_Read, 0, 0, iWidth, iHeight, pTemp, iWidth, iHeight, eDataType, 0, 0);
		if (ReadErr == CE_Failure)	//如果读取失败，则返回
		{
			AfxMessageBox("读取图像错误");
			return;
		}
	}
	

	/*为变量开辟内存空间
	********************************************************************************************************************************************************************************/
	MallocData();


	/*初始化所有变量
	********************************************************************************************************************************************************************************/
	Copy(pDataInput, uk1);
	Copy(pDataInput, f);
	Copy(pDataInput, vk);
	//WriteTIfOut(uk1, sPathTifOut);
	Grad_xx(f, f_ddx);

	λ1 = 10;
	λ2 = 10;
	λ3 = 3;
	α = 0.1;
	β = 0.1;
	γ = 0.03;
	iter = 100;

	for (int aa = 0; aa < 20; aa++)
	{
		Clear(dx);
		Clear(dy);
		Clear(dz);
		Clear(bx);
		Clear(by);
		Clear(bz);

		for (int ii = 0; ii < 10; ii++)
		{
			Copy(uk1, uk);

			Grad_x(dx, ddx);
			Grad_y(dy, ddy);
			Grad_z(dz, ddz);
			Grad_x(bx, dbx);
			Grad_y(by, dby);
			Grad_z(bz, dbz);

			for (int jj = 0; jj < 20; jj++)
			{
				Copy(uk1, uk);
				Gaussian_method();
				//WriteTIfOut(uk1, "D:\\TestData\\ASSTVDnoise\\ASSTV_OUT\\uk1.tif");
			}
			//WriteTIfOut(uk1, "D:\\TestData\\ASSTVDnoise\\ASSTV_OUT\\uk1.tif");

			Subtract(uk1, vk, uk_f);
			Grad_x(uk_f, uk_f_dx);

			Grad_x(uk1, uk_x);
			Grad_y(uk1, uk_y);
			Grad_z(uk1, uk_z);

			Shrink_x(uk_f_dx, bx, dx);
			Shrink_y(uk_y, by, dy);
			Shrink_z(uk_z, bz, dz);

			Bx_iter(uk_f_dx, dx, bx);
			By_iter(uk_y, dy, by);
			Bz_iter(uk_z, dz, bz);

			//for (int jj = 0; jj < 20; jj++)
			//{
			//	Copy(uk1, uk);
			//	Gaussian_method();
			//	//WriteTIfOut(uk1, "D:\\TestData\\ASSTVDnoise\\ASSTV_OUT\\uk1.tif");
			//}

			////将迭代结果uk加入到u图像中
			//Copy(u, u_temp);
			//Add(u_temp, uk1, u);

			////从原图中减去u图像，剩下的vk图像作为下一次迭代的输入数据
			//Subtract(f, u, vk);
			//WriteTIfOut(vk, "D:\\TestData\\ASSTVDnoise\\ASSTV_OUT\\vk.tif");

			////Lambda变为原来的一半
			
			//γ = γ / 2;
			//

			////将vk的数值赋值给uk1，进入下一次G-S不动点迭代法
			//Copy(vk, uk1);

			////暂时输出当前的图像
			//CString sii;
			//sii.Format("%d", ii);
			//WriteTIfOut(vk, sPathTifOut + "_vk" + sii + ".tif");
			//WriteTIfOut(u, sPathTifOut + "_" + sii + ".tif");
		}

		//将迭代结果uk加入到u图像中
		Copy(u, u_temp);
		Add(u_temp, uk1, u);


		//从原图中减去u图像，剩下的vk图像作为下一次迭代的输入数据
		Subtract(f, u, vk);

		//将vk的数值赋值给uk1，进入下一次G-S不动点迭代法
		Copy(vk, uk1);

		β = β / 2;

		CString sii;
		sii.Format("%d", aa);
		CString Path = sPathTifOut + "_" + sii + ".tif";
		WriteTIfOut(u, Path);
	}

	

	/*结束程序，释放空间
	********************************************************************************************************************************************************************************/
	//WriteTIfOut(uk1, sPathTifOut);
	DeleteData();
	for (int ii = 0; ii < iBandNum; ii++)
	{
		free(pDataInput[ii]);
		pDataInput[ii] = NULL;
	}
	free(pDataInput);
	pDataInput = NULL;
	AfxMessageBox("Done");
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CASSTVDenoiseDlg::Copy(BYTE** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		BYTE* pInput = pInputData[ii];
		double* pOutput = pOutputData[ii];
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			pOutput[jj] = (double)pInput[jj];
		}
	}
}

void CASSTVDenoiseDlg::Copy(short** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		short* pInput = pInputData[ii];
		double* pOutput = pOutputData[ii];
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			pOutput[jj] = (double)pInput[jj];
		}
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CASSTVDenoiseDlg::Copy(int** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		int* pInput = pInputData[ii];
		double* pOutput = pOutputData[ii];
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			pOutput[jj] = (double)pInput[jj];
		}
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CASSTVDenoiseDlg::Copy(float** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		float* pInput = pInputData[ii];
		double* pOutput = pOutputData[ii];
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			pOutput[jj] = (double)pInput[jj];
		}
	}
}

//复制函数
//将pInputData的数据复制给pOutputData
//输入：pInputData：复制数据来源方；pOutputData：复制数据接收方
void CASSTVDenoiseDlg::Copy(double** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pInput = pInputData[ii];
		double* pOutput = pOutputData[ii];
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			pOutput[jj] = (double)pInput[jj];
		}
	}
}

//范数
//范数求法：各项平方和相加然后开平方
//输入：pInputData：需要求范数的数据
//输出：dResult：范数数值
double CASSTVDenoiseDlg::Norm(double** pInputData)
{
	double dResult = 0;
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pInput = pInputData[ii];
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			dResult += pInput[jj] * pInput[jj];
		}
	}
	dResult = sqrt(dResult);
	return dResult;
}

//相减函数
//输入：pInputData1：被减数；pInputData2：减数；pOutputData：减法结果
void CASSTVDenoiseDlg::Subtract(double** pInputData1, double** pInputData2, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pInput1 = pInputData1[ii];
		double* pInput2 = pInputData2[ii];
		double* pOutput = pOutputData[ii];
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			pOutput[jj] = pInput1[jj] - pInput2[jj];
		}
	}
}

void CASSTVDenoiseDlg::Add(double** pInputData1, double** pInputData2, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pInput1 = pInputData1[ii];
		double* pInput2 = pInputData2[ii];
		double* pOutput = pOutputData[ii];
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			pOutput[jj] = pInput1[jj] + pInput2[jj];
		}
	}
}

//横向偏导
//输入：pInputData：输入图像；pOutputData：偏导结果
void CASSTVDenoiseDlg::Grad_x(double** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pInput = pInputData[ii];
		double* pOutput = pOutputData[ii];
		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth - 1; kk++)
			{
				pOutput[jj * iWidth + kk] = pInput[jj * iWidth + (kk + 1)] - pInput[jj * iWidth + kk];
			}
		}
		for (int jj = 0; jj < iHeight; jj++)
		{
			pOutput[jj * iWidth + (iWidth - 1)] = 0.0;
		}
	}
}

//纵向偏导
//输入：pInputData：输入图像；pOutputData：偏导结果
void CASSTVDenoiseDlg::Grad_y(double** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pInput = pInputData[ii];
		double* pOutput = pOutputData[ii];
		for (int jj = 0; jj < iHeight - 1; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				pOutput[jj * iWidth + kk] = pInput[(jj + 1) * iWidth + kk] - pInput[jj * iWidth + kk];
			}
		}
		for (int jj = 0; jj < iWidth; jj++)
		{
			pOutput[(iHeight - 1) * iWidth + jj] = 0.0;
		}
	}
}

//波段向偏导
//输入：pInputData：输入图像；pOutputData：偏导结果
void CASSTVDenoiseDlg::Grad_z(double** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum - 1; ii++)
	{
		double* pInput1 = pInputData[ii];
		double* pInput2 = pInputData[ii + 1];
		double* pOutput = pOutputData[ii];
		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				pOutput[jj * iWidth + kk] = pInput2[jj * iWidth + kk] - pInput1[jj * iWidth + kk];
			}
		}
	}

	double* pOutput = pOutputData[iBandNum - 1];
	for (int ii = 0; ii < iHeight * iWidth; ii++)
	{
		pOutput[ii] = 0.0;
	}
}

void CASSTVDenoiseDlg::Grad_xx(double** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* Temp_Input = pInputData[ii];
		double* Temp_Output = pOutputData[ii];

		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				int jj_left = jj;
				int kk_left = kk - 1;
				if (kk_left < 0)
					kk_left = iWidth - 1;

				int jj_right = jj;
				int kk_right = kk + 1;
				if (kk_right == iWidth)
					kk_right = 0;

				Temp_Output[jj * iWidth + kk] =
					Temp_Input[jj_left * iWidth + kk_left] +
					Temp_Input[jj_right * iWidth + kk_right] -
					Temp_Input[jj * iWidth + kk] * 2;
			}
		}
	}
}

void CASSTVDenoiseDlg::Grad_yy(double** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* Temp_Input = pInputData[ii];
		double* Temp_Output = pOutputData[ii];

		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				int jj_up = jj + 1;
				int kk_up = kk;
				if (jj_up == iHeight)
					jj_up = 0;

				int jj_down = jj - 1;
				int kk_down = kk;
				if (jj_down < 0)
					jj_down = iHeight - 1;

				Temp_Output[jj * iWidth + kk] =
					Temp_Input[jj_up * iWidth + kk_up] +
					Temp_Input[jj_down * iWidth + kk_down] -
					Temp_Input[jj * iWidth + kk] * 2;
			}
		}
	}
}

void CASSTVDenoiseDlg::Grad_zz(double** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* Temp_Input = pInputData[ii];
		double* Temp_Output = pOutputData[ii];

		int ii_forward = ii + 1;
		if (ii_forward == iBandNum)
			ii_forward = 0;
		int ii_backward = ii - 1;
		if (ii_backward < 0)
			ii_backward = iBandNum - 1;

		double* Temp_Input_forward = pInputData[ii_forward];
		double* Temp_Input_backward = pInputData[ii_backward];

		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				Temp_Output[jj * iWidth + kk] =
					Temp_Input_forward[jj * iWidth + kk] +
					Temp_Input_backward[jj * iWidth + kk] - 
					Temp_Input[jj * iWidth + kk] * 2;
			}
		}
	}
}

//求邻域之和
//输入：pInputData：输入图像；pOutputData：求和结果
void CASSTVDenoiseDlg::Neighbors(double** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* Temp_Input = pInputData[ii];
		double* Temp_Output = pOutputData[ii];

		int ii_forward = ii + 1;
		if (ii_forward == iBandNum)
			ii_forward = 0;
		int ii_backward = ii - 1;
		if (ii_backward < 0)
			ii_backward = iBandNum - 1;

		double* Temp_Input_forward = pInputData[ii_forward];
		double* Temp_Input_backward = pInputData[ii_backward];

		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				int jj_left = jj;
				int kk_left = kk - 1;
				if (kk_left < 0)
					kk_left = iWidth - 1;

				int jj_right = jj;
				int kk_right = kk + 1;
				if (kk_right == iWidth)
					kk_right = 0;

				int jj_up = jj + 1;
				int kk_up = kk;
				if (jj_up == iHeight)
					jj_up = 0;

				int jj_down = jj - 1;
				int kk_down = kk;
				if (jj_down < 0)
					jj_down = iHeight - 1;

				Temp_Output[jj * iWidth + kk] =
					α * (Temp_Input[jj_left * iWidth + kk_left] + Temp_Input[jj_right * iWidth + kk_right]) +
					β * (Temp_Input[jj_up * iWidth + kk_up] + Temp_Input[jj_down * iWidth + kk_down]) + 
					γ * (Temp_Input_forward[jj * iWidth + kk] + Temp_Input_backward[jj * iWidth + kk]);
			}
		}
	}
}

void CASSTVDenoiseDlg::NeighborsV(double** pInputData, double** pOutputData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* Temp_Input = pInputData[ii];
		double* Temp_Output = pOutputData[ii];

		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				int jj_left = jj;
				int kk_left = kk - 1;
				if (kk_left < 0)
					kk_left = iWidth - 1;

				int jj_right = jj;
				int kk_right = kk + 1;
				if (kk_right == iWidth)
					kk_right = 0;

				Temp_Output[jj * iWidth + kk] =
					Temp_Input[jj * iWidth + kk] +
					α * (Temp_Input[jj * iWidth + kk] * 2 - Temp_Input[jj_left * iWidth + kk_left] - Temp_Input[jj_right * iWidth + kk_right]);
			}
		}
	}
}

//G-S迭代
//输入：各个参数
void CASSTVDenoiseDlg::Gaussian_method()
{
	double mul = 1 / (2 + 2 * α + 2 * β + 2 * γ);
	Neighbors(uk, nei);
	NeighborsV(vk, neiV);
	//WriteTIfOut(nei, "D:\\TestData\\ASSTVDnoise\\ASSTV_OUT\\nei.tif");
	//WriteTIfOut(neiV, "D:\\TestData\\ASSTVDnoise\\ASSTV_OUT\\neiV.tif");
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* Temp_uk1 = uk1[ii];
		double* Temp_nei = nei[ii];
		double* Temp_neiV = neiV[ii];

		double* Temp_ddx = ddx[ii];
		double* Temp_ddy = ddy[ii];
		double* Temp_ddz = ddz[ii];

		double* Temp_dbx = dbx[ii];
		double* Temp_dby = dby[ii];
		double* Temp_dbz = dbz[ii];

		double* Temp_f = f[ii];
		double* Temp_f_ddx = f_ddx[ii];

		for (int jj = 0; jj < iHeight; jj++)
		{
			for(int kk = 0;kk < iWidth; kk++)
			{
				Temp_uk1[jj * iWidth + kk] =
					mul * (
						Temp_nei[jj * iWidth + kk] +
						α * (Temp_dbx[jj * iWidth + kk] - Temp_ddx[jj * iWidth + kk]) +
						β * (Temp_dby[jj * iWidth + kk] - Temp_ddy[jj * iWidth + kk]) +
						γ * (Temp_dbz[jj * iWidth + kk] - Temp_ddz[jj * iWidth + kk]) + 
						Temp_neiV[jj * iWidth + kk]
						);
			}
		}
	}
}

//Shrink算子
//输入：uk_x、bx：存放输入数据，dx：存放计算结果
void CASSTVDenoiseDlg::Shrink_x(double** uk_x, double** bx, double** dx)
{
	double thr = λ1 / α;

	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* Temp_uk_x = uk_x[ii];
		double* Temp_bx = bx[ii];
		double* Temp_dx = dx[ii];

		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				double Temp = Temp_uk_x[jj * iWidth + kk] + Temp_bx[jj * iWidth + kk];
				if (Temp > thr)
					Temp_dx[jj * iWidth + kk] = Temp - thr;
				else if (Temp < 0 - thr)
					Temp_dx[jj * iWidth + kk] = Temp + thr;
				else
					Temp_dx[jj * iWidth + kk] = 0;
			}
		}
	}
}

//Shrink算子
//输入：uk_y、by：存放输入数据，dy：存放计算结果
void CASSTVDenoiseDlg::Shrink_y(double** uk_y, double** by, double** dy)
{
	double thr = λ2 / β;

	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* Temp_uk_y = uk_y[ii];
		double* Temp_by = by[ii];
		double* Temp_dy = dy[ii];

		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				double Temp = Temp_uk_y[jj * iWidth + kk] + Temp_by[jj * iWidth + kk];
				if (Temp > thr)
					Temp_dy[jj * iWidth + kk] = Temp - thr;
				else if (Temp < 0 - thr)
					Temp_dy[jj * iWidth + kk] = Temp + thr;
				else
					Temp_dy[jj * iWidth + kk] = 0;
			}
		}
	}
}

//Shrink算子
//输入：uk_z、bz：存放输入数据，dz：存放计算结果
void CASSTVDenoiseDlg::Shrink_z(double** uk_y, double** by, double** dy)
{
	double thr = λ3 / γ;

	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* Temp_uk_z = uk_z[ii];
		double* Temp_bz = bz[ii];
		double* Temp_dz = dz[ii];

		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				double Temp = Temp_uk_z[jj * iWidth + kk] + Temp_bz[jj * iWidth + kk];
				if (Temp > thr)
					Temp_dz[jj * iWidth + kk] = Temp - thr;
				else if (Temp < 0 - thr)
					Temp_dz[jj * iWidth + kk] = Temp + thr;
				else
					Temp_dz[jj * iWidth + kk] = 0;
			}
		}
	}
}

//bx迭代函数
//输入：uk_x：uk1的横向偏导；dx、bx：对应的变量
void CASSTVDenoiseDlg::Bx_iter(double** uk_x, double** dx, double** bx)
{
	for(int ii = 0;ii < iBandNum;ii++)
	{
		double* Temp_uk_x = uk_x[ii];
		double* Temp_dx = dx[ii];
		double* Temp_bx = bx[ii];
		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				Temp_bx[jj * iWidth + kk] = Temp_bx[jj * iWidth + kk] + Temp_uk_x[jj * iWidth + kk] - Temp_dx[jj * iWidth + kk];
			}
		}
	}
}

//by迭代函数
//输入：uk_y：uk1的纵向偏导；dy、by：对应的变量
void CASSTVDenoiseDlg::By_iter(double** uk_y, double** dy, double** by)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* Temp_uk_y = uk_y[ii];
		double* Temp_dy = dy[ii];
		double* Temp_by = by[ii];
		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				Temp_by[jj * iWidth + kk] = Temp_by[jj * iWidth + kk] + Temp_uk_y[jj * iWidth + kk] - Temp_dy[jj * iWidth + kk];
			}
		}
	}
}

//bz迭代函数
//输入：uk_z：uk1的波段向偏导；dz、bz：对应的变量
void CASSTVDenoiseDlg::Bz_iter(double** uk_z, double** dz, double** bz)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* Temp_uk_z = uk_z[ii];
		double* Temp_dz = dz[ii];
		double* Temp_bz = bz[ii];
		for (int jj = 0; jj < iHeight; jj++)
		{
			for (int kk = 0; kk < iWidth; kk++)
			{
				Temp_bz[jj * iWidth + kk] = Temp_bz[jj * iWidth + kk] + Temp_uk_z[jj * iWidth + kk] - Temp_dz[jj * iWidth + kk];
			}
		}
	}
}

//输出图像函数
//输入：pData：需要输出的数据；sTifPathOut：输出路径
void CASSTVDenoiseDlg::WriteTIfOut(double** pData, CString sTifPathOut)
{
	//创建输出图像
	GDALDataset* pDatasetOutput;
	GDALDriver* pDriver = NULL;
	pDriver = GetGDALDriverManager()->GetDriverByName("GTIFF");
	pDatasetOutput = pDriver->Create(sTifPathOut, iWidth, iHeight, iBandNum, GDT_Float64, NULL);
	if (!pDatasetOutput)
	{
		AfxMessageBox("创建输出文件失败");
		return;
	}

	//将输出数据写入到输出图像中
	for (int ii = 1; ii <= iBandNum; ii++)
	{
		GDALRasterBand* pBandOutput = pDatasetOutput->GetRasterBand(ii);
		double* pTempData = pData[ii - 1];
		CPLErr WriteErr = pBandOutput->RasterIO(GF_Write, 0, 0, iWidth, iHeight, pTempData, iWidth, iHeight, GDT_Float64, 0, 0);
		if (WriteErr == CE_Failure)	//如果输出失败，则返回
		{
			AfxMessageBox("输出图像错误");
			return;
		}
	}

	GDALClose(pDatasetOutput);

	return;
}

void CASSTVDenoiseDlg::Clear(double** pData)
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* TempData = pData[ii];
		for (int jj = 0; jj < iHeight * iWidth; jj++)
		{
			TempData[jj] = 0;
		}
	}
}

void CASSTVDenoiseDlg::MallocData()
{
	f = NULL;	//f
	f = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		f[ii] = pTemp;
	}

	nei = NULL;	//nei
	nei = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		nei[ii] = pTemp;
	}

	neiV = NULL;	//neiV
	neiV = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		neiV[ii] = pTemp;
	}

	uk = NULL;	//uk
	uk = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		uk[ii] = pTemp;
	}

	uk1 = NULL;	//uk+1
	uk1 = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		uk1[ii] = pTemp;
	}

	uk1_uk = NULL;	//uk+1 - uk
	uk1_uk = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		uk1_uk[ii] = pTemp;
	}

	uk_f = NULL;	//uk - f
	uk_f = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		uk_f[ii] = pTemp;
	}

	uk_f_dx = NULL;	//dx(uk - f)
	uk_f_dx = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		uk_f_dx[ii] = pTemp;
	}

	u = NULL;	//u
	u = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		u[ii] = pTemp;
	}

	u_temp = NULL;	//u_temp
	u_temp = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		u_temp[ii] = pTemp;
	}

	vk = NULL;	//vk
	vk = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		vk[ii] = pTemp;
	}

	f_dx = NULL;	//f_dx
	f_dx = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		f_dx[ii] = pTemp;
	}

	f_ddx = NULL;	//f_ddx
	f_ddx = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		f_ddx[ii] = pTemp;
	}

	dx = NULL;	//dx
	dx = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		dx[ii] = pTemp;
	}

	dy = NULL;	//dy
	dy = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		dy[ii] = pTemp;
	}

	dz = NULL;	//dz
	dz = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		dz[ii] = pTemp;
	}

	bx = NULL;	//bx
	bx = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		bx[ii] = pTemp;
	}

	by = NULL;	//by
	by = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		by[ii] = pTemp;
	}

	bz = NULL;	//bz
	bz = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		bz[ii] = pTemp;
	}

	uk_x = NULL;	//uk_x
	uk_x = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		uk_x[ii] = pTemp;
	}

	uk_y = NULL;	//uk_y
	uk_y = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		uk_y[ii] = pTemp;
	}

	uk_z = NULL;	//uk_z
	uk_z = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		uk_z[ii] = pTemp;
	}

	ddx = NULL;	//ddx
	ddx = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		ddx[ii] = pTemp;
	}

	ddy = NULL;	//ddy
	ddy = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		ddy[ii] = pTemp;
	}

	ddz = NULL;	//ddz
	ddz = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		ddz[ii] = pTemp;
	}

	dbx = NULL;	//dbx
	dbx = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		dbx[ii] = pTemp;
	}

	dby = NULL;	//dby
	dby = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		dby[ii] = pTemp;
	}

	dbz = NULL;	//dbz
	dbz = (double**)malloc(iBandNum * sizeof(double*));
	for (int ii = 0; ii < iBandNum; ii++)
	{
		double* pTemp = NULL;
		pTemp = (double*)malloc(iHeight * iWidth * sizeof(double));
		for (int ii = 0; ii < iHeight * iWidth; ii++)	//初始化
		{
			pTemp[ii] = NULL;
		}
		dbz[ii] = pTemp;
	}
}

void CASSTVDenoiseDlg::DeleteData()
{
	for (int ii = 0; ii < iBandNum; ii++)
	{
		free(f[ii]);
		free(uk[ii]);
		free(uk1[ii]);
		free(uk1_uk[ii]);
		free(nei[ii]);
		free(neiV[ii]);

		free(u[ii]);
		free(u_temp[ii]);
		free(vk[ii]);

		free(uk_f[ii]);
		free(uk_f_dx[ii]);
		free(f_dx[ii]);
		free(f_ddx[ii]);

		free(uk_x[ii]);
		free(uk_y[ii]);
		free(uk_z[ii]);

		free(dx[ii]);
		free(dy[ii]);
		free(dz[ii]);

		free(bx[ii]);
		free(by[ii]);
		free(bz[ii]);

		free(ddx[ii]);
		free(ddy[ii]);
		free(ddz[ii]);

		free(dbx[ii]);
		free(dby[ii]);
		free(dbz[ii]);

		f[ii] = NULL;
		uk[ii] = NULL;
		uk1[ii] = NULL;
		uk1_uk[ii] = NULL;
		nei[ii] = NULL;
		neiV[ii] = NULL;

		u[ii] = NULL;
		u_temp[ii] = NULL;
		vk[ii] = NULL;

		uk_f[ii] = NULL;
		uk_f_dx[ii] = NULL;
		f_dx[ii] = NULL;
		f_ddx[ii] = NULL;

		uk_x[ii] = NULL;
		uk_y[ii] = NULL;
		uk_z[ii] = NULL;

		dx[ii] = NULL;
		dy[ii] = NULL;
		dz[ii] = NULL;

		bx[ii] = NULL;
		by[ii] = NULL;
		bz[ii] = NULL;

		ddx[ii] = NULL;
		ddy[ii] = NULL;
		ddz[ii] = NULL;

		dbx[ii] = NULL;
		dby[ii] = NULL;
		dbz[ii] = NULL;
	}

	{
		free(f);
		free(uk);
		free(uk1);
		free(uk1_uk);
		free(nei);
		free(neiV);

		free(u);
		free(u_temp);
		free(vk);

		free(uk_f);
		free(uk_f_dx);
		free(f_dx);
		free(f_ddx);

		free(uk_x);
		free(uk_y);
		free(uk_z);

		free(dx);
		free(dy);
		free(dz);

		free(bx);
		free(by);
		free(bz);

		free(ddx);
		free(ddy);
		free(ddz);

		free(dbx);
		free(dby);
		free(dbz);

		f = NULL;
		uk = NULL;
		uk1 = NULL;
		uk1_uk = NULL;
		nei = NULL;
		neiV = NULL;

		u = NULL;
		u_temp = NULL;
		vk = NULL;

		uk_f = NULL;
		uk_f_dx = NULL;
		f_dx = NULL;
		f_ddx = NULL;

		uk_x = NULL;
		uk_y = NULL;
		uk_z = NULL;

		dx = NULL;
		dy = NULL;
		dz = NULL;

		bx = NULL;
		by = NULL;
		bz = NULL;

		ddx = NULL;
		ddy = NULL;
		ddz = NULL;

		dbx = NULL;
		dby = NULL;
		dbz = NULL;
	}
}
