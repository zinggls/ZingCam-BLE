
// ziieDlg.h: 헤더 파일
//

#pragma once

#include <vector>

#define READ_BUFFER_SIZE	174

class CCom;

// CZiieDlg 대화 상자
class CZiieDlg : public CDialogEx
{
// 생성입니다.
public:
	CZiieDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ZIIE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CCom *m_pCom;
	CListBox m_log;
	std::vector<byte> m_devices;
	CWinThread* m_pReadThread;

	BOOL COM_Init();
	void COM_UnInit();
	BOOL COM_OpenPort();
	void L(const TCHAR* str, ...);
	BOOL SetPowerVoltage();
	BOOL PowerOn();
	BOOL SetProtocol();
	BOOL I2C_ResetBus();
	BOOL I2C_SetSpeed();
	BOOL I2C_GetSpeed();
	BOOL I2C_GetDeviceList();
	HRESULT Control_I2C_SCB_Slave(int deviceAddress);
	void UpdateZcdListCtrl(std::vector<byte>& dataOUT);
	HRESULT Read_I2C_SCB_Slave(int deviceAddress, DWORD dwMilliseconds);
	static UINT I2C_Read(LPVOID pParam);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedI2cReadButton();
	CListCtrl m_zcdListCtrl;
};
