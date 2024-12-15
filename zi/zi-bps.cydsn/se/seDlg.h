
// seDlg.h: 헤더 파일
//

#pragma once

#include <vector>

#define WRITE_BUFFER_SIZE	(8)
#define READ_BUFFER_SIZE	(3)
#define MAX_LIST_ITEMS		(1024*8)

typedef struct SCOPE {
	byte scopeKindChangeNotify;
	byte scopeOutChangeNotify;
	byte scopeOperationMode;
} SCOPE;

typedef struct SCOPE_STATE {
	byte kind;
	byte out;
	byte battery;
	byte ir;
	byte eo;
} SCOPE_STATE;

typedef struct SCOPE_WRITE_BUFFER {
	SCOPE scope;
	SCOPE_STATE scope_state;
} SCOPE_WRITE_BUFFER;

class CCom;

// CseDlg 대화 상자
class CseDlg : public CDialogEx
{
// 생성입니다.
public:
	CseDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SE_DIALOG };
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
	CListBox m_log;
	CComboBox m_portsCombo;
	static BOOL bRead;
	CWinThread* m_pReadThread;
	CCom* m_pCom;
	std::vector<byte> m_devices;
	SCOPE_WRITE_BUFFER m_scope_write_buffer;
	BOOL m_bSendWriteBuffer;

	void L(const TCHAR* str, ...);
	BOOL COM_Init();
	void COM_UnInit();
	void ResetI2CReadButton();
	void FillPortsCombo();
	void CreateWriteBuffer();
	void EnableCombos(BOOL b);
	CComboBox m_scopeKindChangeNotiCombo;
	CComboBox m_scopeOutChangeNotiCombo;
	CComboBox m_scopeOperationModeCombo;
	CComboBox m_scopeStateKindCombo;
	CComboBox m_scopeStateBatteryCombo;
	CComboBox m_scopeStateIrCombo;
	CComboBox m_scopeStateEoCombo;
	CComboBox m_scopeStateOutCombo;
	CString m_strScopeKindChangeNoti;
	CString m_strScopeOutChangeNoti;
	CString m_strScopeOperationMode;
	CListCtrl m_writeBufferListCtrl;
	BOOL m_bReadBuffer;
	BOOL m_bWriteBuffer;
	afx_msg void OnBnClickedRawClearButton();
	afx_msg void OnBnClickedReadBufferCheck();
	afx_msg void OnBnClickedWriteBufferCheck();
	BOOL COM_OpenPort();
	BOOL SetPowerVoltage();
	BOOL PowerOn();
	BOOL SetProtocol();
	BOOL I2C_ResetBus();
	BOOL I2C_SetSpeed();
	BOOL I2C_GetSpeed();
	BOOL I2C_GetDeviceList();
	size_t Parse_I2C(std::vector<byte>& dataOUT, SCOPE& sc);
	void UpdateScopeKindChangeNoti(byte kind);
	void UpdateScopeOutChangeNoti(byte out);
	void UpdateScopeOperationMode(byte mode);
	void UpdateGUI(SCOPE_WRITE_BUFFER& swb);
	CString RawString(std::vector<byte>& dataOUT);
	HRESULT Send_I2C_WriteBuffer(int deviceAddress);
	HRESULT Read_I2C_SCB_Slave(int deviceAddress);
	static UINT I2C_Read(LPVOID pParam);
	void InitWriteBufferCombo(SCOPE& sc);
	afx_msg void OnBnClickedI2cReadButton();
	afx_msg void OnBnClickedI2cWriteButton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeScopeKindChangeNotiCombo();
	afx_msg void OnCbnSelchangeScopeOutChangeNotiCombo();
	afx_msg void OnCbnSelchangeOpmodeScopeCombo();
	afx_msg void OnCbnSelchangeScopeStateKindCombo();
	afx_msg void OnCbnSelchangeScopeStateOutCombo();
	afx_msg void OnCbnSelchangeScopeStateBatteryCombo();
	afx_msg void OnCbnSelchangeScopeStateIrCombo();
	afx_msg void OnCbnSelchangeScopeStateEoCombo();
};
