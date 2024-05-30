
// UTVDenoiseDlg.h: 头文件
//

#pragma once

#include "gdal_priv.h"

// CUTVDenoiseDlg 对话框
class CUTVDenoiseDlg : public CDialogEx
{
// 构造
public:
	CUTVDenoiseDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UTVDENOISE_DIALOG };
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
	int iWidth;	//宽度：5000
	int iHeight;	//高度：5000
	int iBandNum;	//波段数量：1
	double lambd;
	double iter;
	double* f;
	double* u;
	double* utemp;
	double* vk;
	double* uk;
	double* NeiU;
	double* NeiV;
	double* uk1;

	afx_msg void OnBnClickedButtonUtv();
	void Copy(BYTE* InputData, double* pOutputData);
	void Copy(short* InputData, double* pOutputData);
	void Copy(int* InputData, double* pOutputData);
	void Copy(float* pInputData, double* pOutputData);
	void Copy(double* pInputData, double* pOutputData);
	void Subtract(double* pInputData1, double* pInputData2, double* pOutputData);
	void Add(double* pInputData1, double* pInputData2, double* pOutputData);
	void NeighborsU(double* pInputData, double* pOutputData);
	void NeighborsV(double* pInputData, double* pOutputData);
	void Gaussian_method();
	void WriteTIfOut(double* pData, CString sTifPathOut);
	void MallocData();
	void DeleteData();
};
