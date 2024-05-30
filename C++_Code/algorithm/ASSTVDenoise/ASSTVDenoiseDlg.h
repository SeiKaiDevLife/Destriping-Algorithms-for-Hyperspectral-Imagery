
// ASSTVDenoiseDlg.h: 头文件
//

#pragma once

#include "gdal_priv.h"

// CASSTVDenoiseDlg 对话框
class CASSTVDenoiseDlg : public CDialogEx
{
// 构造
public:
	CASSTVDenoiseDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ASSTVDENOISE_DIALOG };
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
	double mu;
	double tol;
	double iter;

	double λ1;
	double λ2;
	double λ3;

	double α;
	double β;
	double γ;

	double** f;
	double** uk;
	double** nei;
	double** neiV;
	double** uk1;
	double** uk1_uk;

	double** u;
	double** u_temp;
	double** vk;

	double** uk_f;
	double** uk_f_dx;
	double** f_dx;
	double** f_ddx;

	double** dx;
	double** dy;
	double** dz;

	double** bx;
	double** by;
	double** bz;

	double** uk_x;
	double** uk_y;
	double** uk_z;

	double** ddx;
	double** ddy;
	double** ddz;

	double** dbx;
	double** dby;
	double** dbz;

	afx_msg void OnBnClickedButtonAsstv();

	void Copy(BYTE** pInputData, double** pOutputData);
	void Copy(short** pInputData, double** pOutputData);
	void Copy(int** pInputData, double** pOutputData);
	void Copy(float** pInputData, double** pOutputData);
	void Copy(double** pInputData, double** pOutputData);

	double Norm(double** pInputData);

	void Subtract(double** pInputData1, double** pInputData2, double** pOutputData);
	void Add(double** pInputData1, double** pInputData2, double** pOutputData);

	void Grad_x(double** pInputData, double** pOutputData);
	void Grad_y(double** pInputData, double** pOutputData);
	void Grad_z(double** pInputData, double** pOutputData);

	void Grad_xx(double** pInputData, double** pOutputData);
	void Grad_yy(double** pInputData, double** pOutputData);
	void Grad_zz(double** pInputData, double** pOutputData);

	void Neighbors(double** pInputData, double** pOutputData);
	void NeighborsV(double** pInputData, double** pOutputData);
	void Gaussian_method();

	void Shrink_x(double** uk_x, double** bx, double** dx);
	void Shrink_y(double** uk_y, double** by, double** dy);
	void Shrink_z(double** uk_z, double** bz, double** dz);

	void Bx_iter(double** uk_x, double** dx, double** bx);
	void By_iter(double** uk_y, double** dy, double** by);
	void Bz_iter(double** uk_z, double** dz, double** bz);

	void WriteTIfOut(double** pData, CString sTifPathOut);
	void Clear(double** pData);
	void MallocData();
	void DeleteData();
};
