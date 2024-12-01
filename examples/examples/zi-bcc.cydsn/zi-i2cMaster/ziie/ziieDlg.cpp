
// ziieDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ziie.h"
#include "ziieDlg.h"
#include "afxdialogex.h"
#include "com.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CZiieDlg 대화 상자



CZiieDlg::CZiieDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ZIIE_DIALOG, pParent), m_pCom(NULL), m_pReadThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CZiieDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG_LIST, m_log);
	DDX_Control(pDX, IDC_ZCD_LIST, m_zcdListCtrl);
}

BEGIN_MESSAGE_MAP(CZiieDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_I2C_READ_BUTTON, &CZiieDlg::OnBnClickedI2cReadButton)
END_MESSAGE_MAP()


// CZiieDlg 메시지 처리기

BOOL CZiieDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_zcdListCtrl.InsertColumn(0, _T("Kind"), LVCFMT_RIGHT, 35);
	m_zcdListCtrl.InsertColumn(1, _T("USB"), LVCFMT_RIGHT, 35);
	m_zcdListCtrl.InsertColumn(2, _T("PPID"), LVCFMT_RIGHT, 85);
	m_zcdListCtrl.InsertColumn(3, _T("DeviceID"), LVCFMT_RIGHT, 85);
	m_zcdListCtrl.InsertColumn(4, _T("FMT"), LVCFMT_RIGHT, 35);
	m_zcdListCtrl.InsertColumn(5, _T("IDX"), LVCFMT_RIGHT, 35);
	m_zcdListCtrl.InsertColumn(6, _T("FPS"), LVCFMT_RIGHT, 85);
	m_zcdListCtrl.InsertColumn(7, _T("TRT"), LVCFMT_RIGHT, 35);
	m_zcdListCtrl.InsertColumn(8, _T("ACK"), LVCFMT_RIGHT, 35);
	m_zcdListCtrl.InsertColumn(9, _T("PPC"), LVCFMT_RIGHT, 35);
	m_zcdListCtrl.InsertColumn(10, _T("RUN"), LVCFMT_RIGHT, 35);
	m_zcdListCtrl.InsertColumn(11, _T("ITF"), LVCFMT_RIGHT, 35);
	m_zcdListCtrl.InsertColumn(12, _T("TXID"), LVCFMT_RIGHT, 85);
	m_zcdListCtrl.InsertColumn(13, _T("RXID"), LVCFMT_RIGHT, 85);
	m_zcdListCtrl.InsertColumn(14, _T("DestID_ERR_CNT"), LVCFMT_RIGHT, 120);
	m_zcdListCtrl.InsertColumn(15, _T("PHY_RX_FRAME_CNT"), LVCFMT_RIGHT, 130);
	m_zcdListCtrl.InsertColumn(16, _T("MFIR"), LVCFMT_RIGHT, 65);
	m_zcdListCtrl.InsertColumn(17, _T("CNT"), LVCFMT_RIGHT, 85);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CZiieDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CZiieDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CZiieDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CZiieDlg::COM_Init()
{
	L(_T("Initializing COM"));
	if (FAILED(CoInitialize(NULL)))
	{
		L(_T("Unable to initialize COM"));
		return FALSE;
	}
	L(_T("COM Initialized"));
	return TRUE;
}

void CZiieDlg::COM_UnInit()
{
	CoUninitialize();
	L(_T("COM Uninitialized"));
}


BOOL CZiieDlg::COM_OpenPort()
{
	m_pCom = new CCom(_T("PSoCProgrammerCOM.PSoCProgrammerCOM_Object"));
	ASSERT(m_pCom);

	HRESULT hr = m_pCom->OpenPort();
	if (!SUCCEEDED(hr)) {
		L(_T("COM OpenPort failed, HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	else {
		L(_T("COM OpenPort OK"));
		return TRUE;
	}
}

void CZiieDlg::L(const TCHAR* str, ...)
{
	va_list args;
	va_start(args, str);

	int len = _vsctprintf(str, args) + 1;	//_vscprintf doesn't count terminating '\0'
	TCHAR* buffer = new TCHAR[len];
	_vstprintf(buffer, len, str, args);
	va_end(args);

	m_log.AddString(buffer);
	delete[](buffer);

	m_log.SetTopIndex(m_log.GetCount() - 1);
}

void CZiieDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_pCom->ClosePort();
	CoUninitialize();
	delete m_pCom;
}

BOOL CZiieDlg::SetPowerVoltage()
{
	//Setup Power - "5.0V" and internal
	HRESULT hr = m_pCom->SetPowerVoltage(_T("5.0"));
	if (!SUCCEEDED(hr)) {
		L(_T("Setup Power error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Setup Power - 5.0V and internal done"));
	return TRUE;
}

BOOL CZiieDlg::PowerOn()
{
	HRESULT hr = m_pCom->PowerOn();
	if (!SUCCEEDED(hr)) {
		L(_T("Power On error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Power On"));
	return TRUE;
}

BOOL CZiieDlg::SetProtocol()
{
	//Set protocol, connector and frequency
	HRESULT hr = m_pCom->SetProtocol(enumInterfaces::I2C); //I2C-protocol
	if (!SUCCEEDED(hr)) {
		L(_T("SetProtocol error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Set protocol, connector and frequency"));
	return TRUE;
}

BOOL CZiieDlg::I2C_ResetBus()
{
	//Reset bus
	HRESULT hr = m_pCom->I2C_ResetBus();
	if (!SUCCEEDED(hr)) {
		L(_T("Reset bus error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Reset bus!"));
	return TRUE;
}

BOOL CZiieDlg::I2C_SetSpeed()
{
	//Set I2C speed
	HRESULT hr = m_pCom->I2C_SetSpeed(enumI2Cspeed::CLK_100K);
	if (!SUCCEEDED(hr)) {
		L(_T("Set speed: 100K error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Set speed: 100K"));
	return TRUE;
}

BOOL CZiieDlg::I2C_GetSpeed()
{
	//Get I2C speed
	long speed = 0;
	CString val = _T("");
	HRESULT hr = m_pCom->I2C_GetSpeed(speed);
	if (!SUCCEEDED(hr)) {
		L(_T("Get speed error,HRESULT: 0x%08X"), hr);
		return FALSE;
	}

	if (speed == 1) {
		val = _T("100K");
	}
	else if (speed == 4) {
		val = _T("50K");
	}
	else if (speed == 2) {
		val = _T("400K");
	}
	L(_T("Get speed: %s"), val);
	return TRUE;
}

BOOL CZiieDlg::I2C_GetDeviceList()
{
	//Get device list
	HRESULT hr = m_pCom->I2C_GetDeviceList(m_devices);
	if (!SUCCEEDED(hr))
	{
		L(_T("Failed to enumerate I2C devices,HRESULT: 0x%08X"), hr);
		return FALSE;
	}
	L(_T("Enumerate I2C devices"));

	//Show devices
	if (m_devices.size() == 0)
	{
		L(_T("No devices found"));
		return FALSE;
	}

	L(_T("Devices list:  8bit  7bit"));
	for (size_t i = 0; i < m_devices.size(); i++) L(_T("     address:  %02x    %02x"), m_devices[i] << 1, m_devices[i]);
	return TRUE;
}

HRESULT CZiieDlg::Control_I2C_SCB_Slave(int deviceAddress)
{
	HRESULT hr;
	std::vector<byte> dataIN;
	dataIN.resize(11);
	hr = m_pCom->writeI2C(deviceAddress, dataIN);
	if (!SUCCEEDED(hr)) {
		L(_T("Failed writeI2C,HRESULT: 0x%08X"), hr);
		return hr;
	}
	L(_T("Write %dbytes to device"), dataIN.size());

	Read_I2C_SCB_Slave(deviceAddress, 100);

	return S_OK;
}

int CZiieDlg::InsertItem(CListCtrl& listCtrl, const CString& newItem)
{
	int itemCount = listCtrl.GetItemCount();
	if (itemCount >= MAX_LIST_ITEMS) listCtrl.DeleteItem(0);	// Remove the first item (FIFO behavior)

	int index = listCtrl.InsertItem(itemCount, newItem); // Add new item at the end

	// Ensure the new item is visible
	listCtrl.EnsureVisible(index, FALSE);
	return index;
}

void CZiieDlg::UpdateZcdListCtrl(std::vector<byte>& dataOUT)
{
	int nItem = InsertItem(m_zcdListCtrl, _T("ZCD"));

	size_t zcdIndex = 99;
	m_zcdListCtrl.SetItemText(nItem, 0, ToIntStr(dataOUT, zcdIndex)); zcdIndex += 4;	//Kind
	m_zcdListCtrl.SetItemText(nItem, 1, ToIntStr(dataOUT, zcdIndex)); zcdIndex += 4;	//USB

	//firmware setZcdBuffer내에서 bnd값을 보내는데 이는 chatgpt가 생성한 코드를 쓰다보니 생긴 오류로 보임. zcd에는 bnd가 정의되지 않았다 따라서 bnd는 무시
	CString strBnd = ToCharStr(dataOUT, zcdIndex); zcdIndex += 1;

	m_zcdListCtrl.SetItemText(nItem, 2, ToHex(dataOUT, zcdIndex)); zcdIndex += 4;		//PPID
	m_zcdListCtrl.SetItemText(nItem, 3, ToHex(dataOUT, zcdIndex)); zcdIndex += 4;		//DeviceID
	m_zcdListCtrl.SetItemText(nItem, 4, ToIntStr(dataOUT, zcdIndex)); zcdIndex += 4;	//FMT
	m_zcdListCtrl.SetItemText(nItem, 5, ToIntStr(dataOUT, zcdIndex)); zcdIndex += 4;	//IDX

	CString strTrt = ToCharStr(dataOUT, zcdIndex);	zcdIndex += 1;
	CString strAck = ToCharStr(dataOUT, zcdIndex);	zcdIndex += 1;
	CString strPpc = ToCharStr(dataOUT, zcdIndex);	zcdIndex += 1;
	CString strRun = ToCharStr(dataOUT, zcdIndex);	zcdIndex += 1;
	CString strTxid = ToHex(dataOUT, zcdIndex); zcdIndex += 4;
	CString strRxid = ToHex(dataOUT, zcdIndex); zcdIndex += 4;
	CString strCnt = ToIntStr(dataOUT, zcdIndex); zcdIndex += 4;
	CString strPos = ToIntStr(dataOUT, zcdIndex); zcdIndex += 4;
	CString strFps = ToHex(dataOUT, zcdIndex); zcdIndex += 4;
	CString strItf = ToCharStr(dataOUT, zcdIndex);	zcdIndex += 1;
	CString strDestErr = ToIntStr(dataOUT, zcdIndex); zcdIndex += 4;
	CString strDestDif = ToIntStr(dataOUT, zcdIndex); zcdIndex += 4;
	CString strPhyRx = ToIntStr(dataOUT, zcdIndex); zcdIndex += 4;
	CString strFrameDif = ToIntStr(dataOUT, zcdIndex); zcdIndex += 4;

	m_zcdListCtrl.SetItemText(nItem, 6, strFps);										//FPS
	m_zcdListCtrl.SetItemText(nItem, 7, strTrt);										//TRT
	m_zcdListCtrl.SetItemText(nItem, 8, strAck);										//ACK
	m_zcdListCtrl.SetItemText(nItem, 9, strPpc);										//PPC
	m_zcdListCtrl.SetItemText(nItem, 10, strRun);										//RUN
	m_zcdListCtrl.SetItemText(nItem, 11, strItf);										//ITF
	m_zcdListCtrl.SetItemText(nItem, 12, strTxid);										//TXID
	m_zcdListCtrl.SetItemText(nItem, 13, strRxid);										//RXID
	m_zcdListCtrl.SetItemText(nItem, 14, strDestErr);									//DestID_ERR_CNT
	m_zcdListCtrl.SetItemText(nItem, 15, strPhyRx);										//PHY_RX_FRAME_CNT
	m_zcdListCtrl.SetItemText(nItem, 16, _T("-"));										//MFIR
	m_zcdListCtrl.SetItemText(nItem, 17, strCnt);										//CNT
}

HRESULT CZiieDlg::Read_I2C_SCB_Slave(int deviceAddress, DWORD dwMilliseconds)
{
	HRESULT hr;
	CString str;
	std::vector<byte> dataOUT;
	while (1) {
		hr = m_pCom->readI2C(deviceAddress, READ_BUFFER_SIZE, dataOUT);
		if (!SUCCEEDED(hr)) {
			L(_T("Failed readI2C,HRESULT: 0x%08X"), hr);
			return hr;
		}

		UpdateZcdListCtrl(dataOUT);

		str.Format(_T("[%Iu] "), dataOUT.size());
		for (size_t i = 0; i < dataOUT.size(); i++) {
			CString tmp;
			tmp.Format(_T("%02x "), dataOUT[i]);
			str += tmp;
		}
		L(str);
		Sleep(dwMilliseconds);
	}
	return S_OK;
}

UINT CZiieDlg::I2C_Read(LPVOID pParam)
{
	CZiieDlg* pDlg = (CZiieDlg*)pParam;

	if (pDlg->COM_Init() != TRUE) return 1;
	if (pDlg->COM_OpenPort() != TRUE) return 1;

	if (pDlg->SetPowerVoltage() != TRUE) return 1;
	if (pDlg->PowerOn() != TRUE) return 1;
	if (pDlg->SetProtocol() != TRUE) return 1;
	if (pDlg->I2C_ResetBus() != TRUE) return 1;

	//Sleep script for 100 milliseconds
	Sleep(100);

	if (pDlg->I2C_SetSpeed() != TRUE) return 1;
	if (pDlg->I2C_GetSpeed() != TRUE) return 1;

	if (pDlg->I2C_GetDeviceList() != TRUE) return 1;

	pDlg->Read_I2C_SCB_Slave(pDlg->m_devices[0], 100);	//무한 루프함수
	return 0;
}

void CZiieDlg::OnBnClickedI2cReadButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pReadThread = AfxBeginThread(I2C_Read, this);
	if (m_pReadThread == NULL) {
		L(_T("Read thread AfxBeginThread failed"));
		return;
	}
	GetDlgItem(IDC_I2C_READ_BUTTON)->EnableWindow(FALSE);
}
