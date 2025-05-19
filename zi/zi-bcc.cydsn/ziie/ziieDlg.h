
// ziieDlg.h: 헤더 파일
//

#pragma once

#include <vector>
#include "ivf.h"

#define WRITE_BUFFER_SIZE	(13)
#define READ_BUFFER_SIZE	(ICD_IVF_SIZE+ZING_ZXX_SIZE+ZING_ZCD_SIZE )
#define VERSION_SIZE		(25)
#define MAX_LIST_ITEMS		(1024*8)
#define BT_ADDRESS_SIZE		(7)
#define ITF_CRITERIA_SIZE	(1)
#define MODE_OPER			(0x01)
#define MODE_WAIT			(0x02)
#define MODE_PSAVE			(0x04)

class CCom;


// CZiieDlg 대화 상자
class CZiieDlg : public CDialogEx
{
// 생성입니다.
public:
	CZiieDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	class COperMode
	{
	public:
		COperMode() {}

		CZiieDlg* m_pDlg;

		void onWirelessVideoTransmitterOperationMode(uint8_t mode);
		void onWirelessVideoReceiverOperationMode(uint8_t mode);
	};

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

	void EnableCombos(BOOL b);
	void FillPortsCombo();
	void CreateWriteBuffer();
	void CreateColumnsIMU(CListCtrl& listCtrl);
	void CreateColumnsZXX(CListCtrl& listCtrl);
	void CreateColumnsZCD(CListCtrl& listCtrl);
	BOOL COM_Init(BOOL bSilent = FALSE);
	void COM_UnInit(BOOL bSilent = FALSE);
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
	BOOL IsNULL(ZING_BASE& z);
	BOOL IsNULL(ZXX& z);
	void UpdateZxxGUI(ZXX& z);
	BOOL IsNULL(ZCD& z);
	void UpdateZcdGUI(ZCD& z);
	void UpdateWriteBufferGUI(I2C_IVF_COMMAND& ic);
	void UpdateScopeKind(byte kind);
	void UpdateScopeOut(byte out);
	void UpdateScope(byte scopeKind, byte scopeOut);
	void UpdateWirelessChannel(byte mode, byte info);
	CString Opmode(byte mode);
	void UpdateOpmode(byte scope, byte tx, byte rx);
	static void UpdateXIMU(BOOL bTx, CString& strImuType, CString& strCalib, byte type, byte calib);
	void UpdatePower(byte txPower,byte rxPower);
	void UpdateScopeStateKind(byte kind);
	void UpdateScopeStateOut(byte out);
	void UpdateScopeDetect(byte det);
	static void BatteryInfo(CString& str, byte val);
	void UpdateScopeStateBattery(byte val);
	CString ModuleSanity(CString strName, byte code, byte errCode);
	void UpdateScopeStateIR(byte code);
	void UpdateScopeStateEO(byte code);
	void UpdateScopeState(byte kind, byte out, byte det, byte val, byte irCode, byte eoCode);
	void UpdateTxState(byte val, byte modemCode, byte imuCode);
	void UpdateRxState(byte modemCode, byte imuCode);
	void UpdateBleState(byte dat);
	static size_t ParseCommandData(std::vector<byte>& dataOUT, size_t index, I2C_IVF_COMMAND& ic);
	void UpdateCommandGUI(I2C_IVF_COMMAND& ic);
	static size_t ParseStateData(std::vector<byte>& dataOUT, size_t index, I2C_STATE& is);
	void UpdateStateGUI(I2C_STATE& is);
	static CString RawString(std::vector<byte>& dataOUT, size_t len);
	static short ToShort(byte high, byte low);
	static size_t ParseImuData(std::vector<byte>& dataOUT, size_t index, IMU& imu);
	static size_t ParseZxxData(std::vector<byte>& dataOUT, size_t index, ZXX& zxx);
	static size_t ParseZcdData(std::vector<byte>& dataOUT, size_t index, ZCD& zcd);
	static size_t Parse_I2C(std::vector<byte>& dataOUT, IVF& ivf);
	static CString ShortToStr(short val);
	void UpdateImuGUI(CListCtrl& listCtrl, IMU& i);
	void UpdateGUI(IVF& ivf);
	HRESULT Send_I2C_Buffer(std::vector<byte>& dataIN, int deviceAddress);
	HRESULT Send_I2C_WriteBuffer(int deviceAddress);
	HRESULT Send_ParingReset_Command(int deviceAddress);
	static BOOL AllValues(std::vector<byte>& dataOUT, byte value);
	void ResetWriteBufferList();
	static void ConvertVectorToCString(const std::vector<byte>& data, CString& result);
	void GetBccVersion(std::vector<byte>& data);
	void GetZcdVersion(std::vector<byte>& data);
	void GetBpsVersion(std::vector<byte>& data);
	void GetZxxVersion(std::vector<byte>& data);
	void GetBtAddress(std::vector<byte>& data);
	void GetItfCriteria(std::vector<byte>& data);
	HRESULT Read_I2C_SCB_Slave(int deviceAddress);
	static BOOL bRead;
	static UINT I2C_Read(LPVOID pParam);
	afx_msg void OnDestroy();
	void InitWriteBufferCombo(I2C_IVF_COMMAND& ic);
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
	CString m_strScopeDetect;
	CString m_strTxStateBattery;
	CString m_strTxStateModem;
	CString m_strTxStateImu;
	CString m_strRxStateModem;
	CString m_strRxStateImu;
	CString m_strBleState;
	CComboBox m_scopeKindCombo;
	CComboBox m_scopeOutCombo;
	CComboBox m_wirelessChannelModeCombo;
	CComboBox m_wirelessChannelInfoCombo;
	CComboBox m_opmodeScopeCombo;
	CComboBox m_opmodeTxCombo;
	CComboBox m_opmodeRxCombo;
	CComboBox m_txImuTypeCombo;
	CComboBox m_txImuCalibCombo;
	CComboBox m_rxImuTypeCombo;
	CComboBox m_rxImuCalibCombo;
	CListCtrl m_writeBufferListCtrl;
	afx_msg void OnBnClickedI2cWriteButton();
	CBitmap m_bmpGreyCtrl;
	CBitmap m_bmpGreenCtrl;
	CBitmap m_bmpYelloCtrl;
	CBitmap m_bmpRedCtrl;
	CStatic m_bleStateCtrl;
	IVF m_ivf;
	afx_msg void OnSelchangeScopeKindCombo();
	afx_msg void OnSelchangeScopeOutCombo();
	afx_msg void OnSelchangeWirelessChannelModeCombo();
	afx_msg void OnSelchangeWirelessChannelInfoCombo();
	afx_msg void OnCbnSelchangeOpmodeScopeCombo();
	afx_msg void OnCbnSelchangeOpmodeTxCombo();
	afx_msg void OnCbnSelchangeOpmodeRxCombo();
	afx_msg void OnCbnSelchangeTxImuTypeCombo();
	afx_msg void OnCbnSelchangeTxImuCalibCombo();
	afx_msg void OnCbnSelchangeRxImuTypeCombo();
	afx_msg void OnCbnSelchangeRxImuCalibCombo();
	afx_msg void OnCbnSelchangeTxPowerCombo();
	afx_msg void OnCbnSelchangeRxPowerCombo();
	static CString DecStr(int val);
	static CString HexStr(int val);
	static CString CharStr(char c);
	static CString IntHexStr(int val);
	BOOL m_bReadBuffer;
	BOOL m_bWriteBuffer;
	afx_msg void OnBnClickedReadBufferCheck();
	afx_msg void OnBnClickedWriteBufferCheck();
	afx_msg void OnBnClickedRawClearButton();
	BOOL m_bSendWriteBuffer;
	BOOL m_bSendPairingReset;
	afx_msg void OnCbnDropdownPortsCombo();
	afx_msg void OnBnClickedI2cResetButton();
	CString m_strFwHbleVer;
	CString m_strFwDbleVer;
	CString m_strFwZxxVer;
	CString m_strFwZcdVer;
	CListBox m_icdLog;
	CListBox m_zxxLog;
	CListBox m_zcdLog;
	CListBox m_dbleVerLog;
	CListBox m_zcdVerLog;
	CListBox m_hbleVerLog;
	CListBox m_zxxVerLog;
	CString m_zxxVerRawStr;
	CString m_strHbleBtAddress;
	afx_msg void OnBnClickedPairingResetButton();
	CString m_strItfCriteriaValue;
	CComboBox m_itfCriteriaCombo;
	byte m_itfCriteria;
	afx_msg void OnCbnSelchangeItfCriteriaCombo();
	CComboBox m_i2cWriteSleepCombo;
	CComboBox m_txPowerCombo;
	CComboBox m_rxPowerCombo;
	CString m_strTxPower;
	CString m_strRxPower;
	CComboBox m_rxPowerHemt1Combo;
	CComboBox m_rxPowerHemt2Combo;
	afx_msg void OnCbnSelchangeRxPowerHemt1Combo();
	afx_msg void OnCbnSelchangeRxPowerHemt2Combo();
	COperMode m_operMode;
};
