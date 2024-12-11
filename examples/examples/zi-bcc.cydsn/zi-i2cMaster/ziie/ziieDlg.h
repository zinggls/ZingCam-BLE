
// ziieDlg.h: 헤더 파일
//

#pragma once

#include <vector>

#define READ_BUFFER_SIZE	(185)
#define MAX_LIST_ITEMS		(1024*8)

typedef struct I2C_IVF_COMMAND {
	byte ScopeKind;
	byte ScopeOut;
	byte WirelessChannelMode;
	byte WirelessChannelInfo;
	byte OpmodeScope;
	byte OpmodeTx;
	byte OpmodeRx;
	byte TxImuType;
	byte TxImuCalib;
	byte RxImuType;
	byte RxImuCalib;
} I2C_IVF_COMMAND;

typedef struct I2C_STATE {
	byte ScopeStateKind;
	byte ScopeStateOut;
	byte ScopeStateBattery;
	byte ScopeStateIR;
	byte ScopeStateEO;
	byte TxStateBattery;
	byte TxStateModem;
	byte TxStateImu;
	byte RxStateModem;
	byte RxStateImu;
	byte BleState;
} I2C_STATE;

typedef struct IMU {
	short data1;
	short data2;
	short data3;
	short data4;
	short data5;
	short checksum;
} IMU;

typedef struct ZING_BASE {
	int kind;
	int usb;
	char bnd;
	unsigned int ppid;
	unsigned int devid;
	int fmt;
	int idx;
	char trt;
	char ack;
	char ppc;
	char run;
	unsigned int txid;
	unsigned int rxid;
	unsigned int cnt;
	unsigned int pos;
} ZING_BASE;

typedef struct ZXX {
	ZING_BASE zb;

	//IMU_FIELDS (무선영상송신기의 IMU 데이터들로 BLE를 통해 수신기로 전달되는 데이터들)
	IMU txImu;

	//USB_VND_PRD_FIELDS
	unsigned int vnd;
	unsigned int prd;

	//SCOPE_STATE_INFO_FIELDS
	char scopeStateKind;
	char scopeStateOut;
	char scopeStateBattery;
	char scopeStateIR;
	char scopeStateEO;

	//TX_STATE_INFO_FIELDS
    char txStateBattery;
	char txStateModem;
	char txStateIMU;
} ZXX;

typedef struct ZCD {
	ZING_BASE zb;

	unsigned int fps;
	char itf;
	unsigned int destIdErrCnt;
	int destIdDiff;
	unsigned int phyRxFrameCnt;
	int frameDiff;
} ZCD;

typedef struct IVF{
	I2C_IVF_COMMAND read;
	I2C_IVF_COMMAND write;
	I2C_STATE state;
	IMU txImu;
	IMU rxImu;
	ZXX zxx;
	ZCD zcd;
} IVF;

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
	void CreateWriteBuffer();
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
	void UpdateZxxGUI(ZXX z);
	size_t UpdateZcdListCtrl(std::vector<byte>& dataOUT, size_t zcdIndex);
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
	void UpdateRxState(byte dat1, byte dat2);
	void UpdateBleState(byte dat);
	void UpdateWriteBuffer();
	static size_t ParseCommandData(std::vector<byte>& dataOUT, size_t index, I2C_IVF_COMMAND& ic);
	void UpdateCommandGUI(I2C_IVF_COMMAND& ic);
	static size_t ParseStateData(std::vector<byte>& dataOUT, size_t index, I2C_STATE& is);
	void UpdateStateGUI(I2C_STATE& is);
	static CString RawString(std::vector<byte>& dataOUT);
	static short ToShort(byte high, byte low);
	static size_t ParseImuData(std::vector<byte>& dataOUT, size_t index, IMU& imu);
	static size_t ParseZxxData(std::vector<byte>& dataOUT, size_t index, ZXX& zxx);
	static size_t Parse_I2C(std::vector<byte>& dataOUT, IVF& ivf);
	static CString ShortToStr(short val);
	void UpdateImuGUI(CListCtrl& listCtrl, IMU& i);
	HRESULT Read_I2C_SCB_Slave(int deviceAddress);
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
	CMap<CString, LPCTSTR, byte, byte> m_writeMap;
	afx_msg void OnBnClickedI2cWriteButton();
	CBitmap m_bmpGreyCtrl;
	CBitmap m_bmpGreenCtrl;
	CBitmap m_bmpYelloCtrl;
	CBitmap m_bmpRedCtrl;
	CStatic m_bleStateCtrl;
	IVF m_ivf;
};
