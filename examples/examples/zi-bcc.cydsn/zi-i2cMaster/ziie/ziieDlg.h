
// ziieDlg.h: 헤더 파일
//

#pragma once

#include <vector>

#define READ_BUFFER_SIZE	(174+12)
#define MAX_LIST_ITEMS		(1024*8)

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

	void FillPortsCombo();
	void CreateColumnsIMU(CListCtrl& listCtrl);
	void CreateColumnsZXX(CListCtrl& listCtrl);
	void CreateColumnsZCD(CListCtrl& listCtrl);
	BOOL COM_Init();
	void COM_UnInit();
	void ResetI2CReadButton();
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
	int InsertItem(CListCtrl& listCtrl, const CString& newItem);
	size_t UpdateImuListCtrl(CListCtrl& listCtrl, std::vector<byte>& dataOUT, size_t index);
	void UpdateZxxListCtrl(std::vector<byte>& dataOUT);
	void UpdateZcdListCtrl(std::vector<byte>& dataOUT);
	void UpdateScopeKind(byte dat);
	void UpdateScopeOut(byte dat);
	void UpdateScope(byte dat1, byte dat2);
	void UpdateWirelessChannel(byte dat1, byte dat2);
	CString Opmode(byte dat);
	void UpdateOpmode(byte dat1, byte dat2, byte dat3);
	static void UpdateXIMU(CString& strImuType, CString& strCalib, byte dat1, byte dat2);
	void UpdateScopeStateKind(byte dat);
	void UpdateScopeStateOut(byte dat);
	static void BatteryInfo(CString& str, byte dat);
	void UpdateScopeStateBattery(byte dat);
	CString ModuleSanity(CString strName, byte dat, byte errCode);
	void UpdateScopeStateIR(byte dat);
	void UpdateScopeStateEO(byte dat);
	void UpdateScopeState(byte dat1, byte dat2, byte dat3, byte dat4, byte dat5);
	void UpdateTxState(byte dat1, byte dat2, byte dat3);
	HRESULT Read_I2C_SCB_Slave(int deviceAddress, DWORD dwMilliseconds);
	static BOOL bRead;
	static UINT I2C_Read(LPVOID pParam);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedI2cReadButton();
	CListCtrl m_zcdListCtrl;
	CListCtrl m_zxxListCtrl;
	CListCtrl m_dImuListCtrl;
	CListCtrl m_hImuListCtrl;
	CComboBox m_portsCombo;
	CString m_strScopeKind;
	CString m_strScopeOut;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString m_strWirelessChannelMode;
	CString m_strWirelessChannelInfo;
	CString m_strOpmodeScope;
	CString m_strOpmodeTx;
	CString m_strOpmodeRx;
	CString m_strTxImuType;
	CString m_strTxImuCalib;
	CString m_strRxImuType;
	CString m_strRxImuCalib;
	CString m_strScopeStateKind;
	CString m_strScopeStateOut;
	CString m_strScopeStateBattery;
	CString m_strScopeStateIR;
	CString m_strScopeStateEO;
	CString m_strTxStateBattery;
	CString m_strTxStateModem;
	CString m_strTxStateImu;
};
