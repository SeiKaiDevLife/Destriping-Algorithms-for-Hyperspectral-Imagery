
// ATVDLRDenoiseDlg.h: 头文件
//

#pragma once

#include "gdal_priv.h"

// CATVDLRDenoiseDlg 对话框
class CATVDLRDenoiseDlg : public CDialogEx
{
// 构造
public:
	CATVDLRDenoiseDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ATVDLRDENOISE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRun();


	/*	参	数	*/
	/*****************************************************************************************************************/
	double** pHSI;
	int iHeight;
	int iWidth;
	int iBandNum;

	double μ = 0.01;
	double μmax = 1000000;
	double ρ = 1.1;
	
	double dIter = 10;

	double ω12 = 0.5;
	double ω13 = 0.25;
	double ω23 = 0.25;

	double λ = 1;
	double β = 5;
	double τ1 = 0.01;
	double τ2 = 0.01;
	double τ3 = 0.01;

	/*	存	放	图	像	的	空	间	*/
	/*****************************************************************************************************************/
	
	/*真实可求的数据*/
	/**********************************************************/
	double** H;//原图像
	double** X;//纯净图像
	double** S;//死线、死像素
	double** B;//条带噪声
	double** N;//高斯噪声

	double** H_X;//H-X图像

	double** DhX;//纯净图像横向导数
	double** DvB;//条带噪声纵向导数
	double** DsX;//纯净图像波段向导数

	double** X12;//纯净图像的12fold图像
	double** X13;//纯净图像的13fold图像
	double** X23;//纯净图像的23fold图像

	/*计算假设的数据*/
	/**********************************************************/
	double** P;//DhX的替换值
	double** Q;//DsX的替换值
	double** T;//B的替换值
	double** J;//DvB的替换值

	double** DvT;//DvB的替换值

	double** Z;//X的替换值
	double** Z12f;//Z的12fold图像
	double** Z13f;//Z的13fold图像
	double** Z23f;//Z的23fold图像

	double** Z12uf;//Z的12unfold图像
	double** Z13uf;//Z的13unfold图像
	double** Z23uf;//Z的23unfold图像

	/*拉格朗日参数*/
	/**********************************************************/
	double** A;//X与Zk1k2的拉格朗日乘子
	double** A12f;//A的12fold图像
	double** A13f;//A的13fold图像
	double** A23f;//A的23fold图像

	double** A12uf;//A的12unfold图像
	double** A13uf;//A的13unfold图像
	double** A23uf;//A的23unfold图像

	double** Mp;//P与DhX的拉格朗日乘子
	double** Mq;//Q与DsX的拉格朗日乘子
	double** Mb;//T与B的拉格朗日乘子
	double** Mt;//J与DvT的拉格朗日乘子
	double** Mh;//H与(X+S+B)的拉格朗日乘子

	/*计算辅助的数据*/
	/**********************************************************/
	double** A12_μ;//A12÷μ的结果
	double** A13_μ;//A13÷μ的结果
	double** A23_μ;//A23÷μ的结果

	double** X12_A12_μ;//X12+A12÷μ的结果
	double** X13_A13_μ;//X13+A13÷μ的结果
	double** X23_A23_μ;//X23+A23÷μ的结果
	
	double** DhtP;//P的左求导
	double** DhtMp;//Mp的左求导
	double** DstQ;//Q的前求导
	double** DstMq;//Mq的前求导
	
	double** DvtJ;//J的上求导
	double** DvtMt;//Mt的上求导

	double** CX;//更新X时FFT函数分子
	double** CT;//更新T时FFT函数分子


	/*	Z	参	数	更	新	*/
	/*****************************************************************************************************************/
	void Update_Z();
	void Update_Z12();
	void Update_Z13();
	void Update_Z23();


	/*	B	参	数	更	新	*/
	/*****************************************************************************************************************/
	void Update_B();


	/*	Gauss-Seidel  操  作  更  新  */
	/*****************************************************************************************************************/
	void GaussSeidelUpdata();
	void Update_X();
	void Update_T();


	/*	Shrink  操  作	更	新	*/
	/*****************************************************************************************************************/
	void ShrinkUpdata();
	void Update_S();
	void Update_P();
	void Update_Q();
	void Update_J();


	/*	拉	格	朗	日	乘	子	更	新	*/
	/*****************************************************************************************************************/
	void LagrangianMultiplierUpdate();
	void Update_Ak1k2();
	void Update_Mp();
	void Update_Mq();
	void Update_Mb();
	void Update_Mt();
	void Update_Mh();


	/*	张	量	复	制	*/
	/*****************************************************************************************************************/
	template <class DataType>
	void Copy(DataType** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);


	/*	张	量	数	值	提	取	与	设	置	*/
	/*****************************************************************************************************************/
	double GetTensorData(double** pInputData, int i, int j, int k, int iHei, int iWid, int iBNm);
	void SetTensorData(double** pInputData, int i, int j, int k, double dData, int iHei, int iWid, int iBNm);


	/*	张	量	运	算	*/
	/*****************************************************************************************************************/
	void Subtract(double** pInputData1, double** pInputData2, double** pOutputData, int iHei, int iWid, int iBNm);
	void Add(double** pInputData1, double** pInputData2, double** pOutputData, int iHei, int iWid, int iBNm);
	void Divide(double** pInputData1, double pInputData2, double** pOutputData, int iHei, int iWid, int iBNm);
	void CalN();


	/*	求	导	*/
	/*****************************************************************************************************************/
	void Grad_x(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Grad_y(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Grad_z(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);

	void Grad_x_t(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Grad_y_t(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Grad_z_t(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);

	void Grad_xx(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Grad_yy(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Grad_zz(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);

	/*	Fold  &  unFold  操  作  */
	/*****************************************************************************************************************/
	void Fold_Mode_12(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Fold_Mode_13(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void Fold_Mode_23(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);

	void unFold_Mode_12(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void unFold_Mode_13(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);
	void unFold_Mode_23(double** pInputData, double** pOutputData, int iHei, int iWid, int iBNm);

	/*	图	像	读	取	输	出	*/
	/*****************************************************************************************************************/
	double** ReadTifIn(CString sTifPathIn);
	void WriteTifOut(double** pData, CString sTifPathOut, int iHei, int iWid, int iBNm);


	/*	内	存	管	理	*/
	/*****************************************************************************************************************/
	void MallocData();
	double** MallocFunction(int iHei, int iWid, int iBNm);
	void DeleteData();
	void DeleteFunction(double** pData, int iBNm);
};
