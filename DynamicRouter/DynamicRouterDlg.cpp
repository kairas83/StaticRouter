// DynamicRouterDlg.cpp : ���� ����
//

#include <time.h>
#include "stdafx.h"
#include "pcap.h"
#include "DynamicRouter.h"
#include "DynamicRouterDlg.h"
#include "RoutingEntryDlg.h"
#include "process.h"
#include "packet32.h"
#include "NILayer.h"
#include "IPLayer.h"
#include "EthernetLayer.h"
#include "UDPLayer.h"
#include "RIPLayer.h"
#include "ARPLayer.h"

#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "packet.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.
extern RoutingEntryDlg EntryDlg;
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDynamicRouterDlg ��ȭ ����




CDynamicRouterDlg::CDynamicRouterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDynamicRouterDlg::IDD, pParent),CBaseLayer( "DynamicRoutingDlg" )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_LayerMgr.AddLayer( new CRIPLayer( "RIP" ) ) ;
	m_LayerMgr.AddLayer( new CUDPLayer( "UDP" ) ) ;
	m_LayerMgr.AddLayer( new CARPLayer( "ARP" ) ) ;
	m_LayerMgr.AddLayer( new CIPLayer( "IP" ) ) ;
	m_LayerMgr.AddLayer( new CEthernetLayer( "Ethernet" ) ) ;
	m_LayerMgr.AddLayer( new CNILayer( "NI" ) ) ;
	m_LayerMgr.AddLayer( this ) ;
	//      ���̾ �����Ѵ�.
	//		TCP���̾� ������ file,chat���̾ �ְ� �� ���� Dlg���̾�� �̾�����.
	// + : ���ʷ��̾��� UpperLayer. - : ���ʷ��̾��� UnderLayer.
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( +IP ( -ARP ( -Ethernet ) *UDP ( *RIP ( *DynamicRoutingDlg ) ) ) ) )");
	CARPLayer* ARP = (CARPLayer*)mp_UnderLayer->GetUnderLayer()->GetUnderLayer()->GetUnderLayer();
	CIPLayer* IP = (CIPLayer*)mp_UnderLayer->GetUnderLayer()->GetUnderLayer();
	IP->setRouting_entry(routing_entry);
	IP->setRouting_top(&routing_top);
	setcache_entry(ARP->getcache_entry());
	cache_top = ARP->getCache_top();
}

void CDynamicRouterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_NIC1);
	DDX_Control(pDX, IDC_COMBO2, m_NIC2);
	DDX_Control(pDX, IDC_LIST1, routing_table);
	DDX_Control(pDX, IDC_LIST2, m_CacheTable);
}

BEGIN_MESSAGE_MAP(CDynamicRouterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	////////////////////////////////////////////////////////////////
	ON_WM_TIMER()				//�̰� �־�� timer�� �۵��Ѵ� !!!
	////////////////////////////////////////////////////////////////
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CDynamicRouterDlg::OnPacketRecv)
	ON_BN_CLICKED(IDC_BUTTON2, &CDynamicRouterDlg::OnAddRoutingEntry)
	ON_BN_CLICKED(IDC_BUTTON5, &CDynamicRouterDlg::OnCleanCacheTable)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST3, &CDynamicRouterDlg::OnLvnItemchangedList3)
	ON_BN_CLICKED(IDC_BUTTON6, &CDynamicRouterDlg::OnBnClickedButton6)
	ON_LBN_SELCHANGE(IDC_LIST4, &CDynamicRouterDlg::OnLbnSelchangeList4)
	ON_BN_CLICKED(IDC_BUTTON7, &CDynamicRouterDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CDynamicRouterDlg::OnBnClickedButton8)
	ON_LBN_SELCHANGE(IDC_LIST4, &CDynamicRouterDlg::OnLbnSelchangeList4)
END_MESSAGE_MAP()


// CDynamicRouterDlg �޽��� ó����

BOOL CDynamicRouterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	EntryDlg.setDlg(this);
	routing_top = 0;
	exitFlag = 0;
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_if_t* devs;
	pcap_findalldevs(&alldevs, errbuf);
	for(devs = alldevs; devs; devs = devs->next)
	{
		m_NIC1.AddString(devs->name);
		m_NIC2.AddString(devs->name);
	}
	LV_COLUMN lvColumn;
	char *Routing_table[6] = {"Destination", "NetMask" ,"Gateway","Flag","Device","Metric"};
	int nWidth_routing[6] = {100,100,120,55,55,55};
	for(int i=0; i<6; i++)
	{
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_CENTER; //�߾�����
		lvColumn.pszText = Routing_table[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = nWidth_routing[i];
		routing_table.InsertColumn(i, &lvColumn);
	}
	char *list[3] = {"IP", "MAC address" ,"State"};
	int nWidth[3] = {110,120,80};
	for(int i=0; i<3; i++)
	{
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_CENTER; //�߾�����
		lvColumn.pszText = list[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = nWidth[i];
		m_CacheTable.InsertColumn(i, &lvColumn);
	}
	routing_table.SetExtendedStyle(routing_table.GetExtendedStyle() | LVS_EX_GRIDLINES);
	hTimer = (HANDLE)_beginthread(TimerThread,0,this);
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CDynamicRouterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CDynamicRouterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CDynamicRouterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CDynamicRouterDlg::OnPacketRecv()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	hRecv_1 = (HANDLE)_beginthread(Receive_1_Thread,0,this);
	hRecv_2 = (HANDLE)_beginthread(Receive_2_Thread,0,this);
}

void CDynamicRouterDlg::Receive_1_Thread(void* param)
{
	struct pcap_pkthdr info;
	const unsigned char* Data;
	CDynamicRouterDlg* dlg = (CDynamicRouterDlg*)param;
	CIPLayer *m_ip = (CIPLayer*)dlg->mp_UnderLayer->GetUnderLayer()->GetUnderLayer();
	char errbuf[PCAP_ERRBUF_SIZE];
	char dev_name[100];

	dlg->m_NIC1.GetLBText(dlg->m_NIC1.GetCurSel(),(LPTSTR)(LPCTSTR)dev_name);
	m_ip->setDev_1_name(dev_name);
	unsigned char *mac = (unsigned char*)dlg->getDevMac(dev_name);

	CIPLayer* IP = (CIPLayer*)dlg->mp_UnderLayer->GetUnderLayer()->GetUnderLayer();
	pcap_if_t* devs;
	for(devs = dlg->alldevs; devs; devs = devs->next)
	{
		if(!memcmp(devs->name,dev_name,strlen(dev_name)))
		{
			IP->setDev_1_IP((unsigned char*)(devs->addresses->addr->sa_data+2));
			break;
		}
	}
	CEthernetLayer* Ether = (CEthernetLayer*)dlg->mp_UnderLayer->GetUnderLayer()->GetUnderLayer()->GetUnderLayer()->GetUnderLayer();
	Ether->SetMac_Dev_1(mac);
	dlg->Dev_1 = pcap_open_live(dev_name,1500,1,1,errbuf);
	pcap_t* Dev_1 = dlg->getDev_1();

	CNILayer* NI = (CNILayer*)dlg->mp_UnderLayer->GetUnderLayer()->GetUnderLayer()->GetUnderLayer()->GetUnderLayer()->GetUnderLayer();
	NI->setDev_1(Dev_1);
	while(true)
	{
		//�ش� ��Ŷ�� ���������� loop
		do
		{
			Data = pcap_next(Dev_1,&info);
		}while(!Data);
		// �ش� ��Ŷ�� �ִٸ� �����ϱ� ���ؼ� NILayer���� Receive�ϱ� �����Ѵ�.
		NI->Receive((unsigned char*)(LPTSTR)(LPCTSTR)Data,eDev_1);
	}
}
void CDynamicRouterDlg::Receive_2_Thread(void* param)
{
	struct pcap_pkthdr info;
	const unsigned char* Data;
	CDynamicRouterDlg* dlg = (CDynamicRouterDlg*)param;
	char errbuf[PCAP_ERRBUF_SIZE];
	char dev_name[100];
	CIPLayer *m_ip = (CIPLayer*)dlg->mp_UnderLayer->GetUnderLayer()->GetUnderLayer();

	dlg->m_NIC2.GetLBText(dlg->m_NIC2.GetCurSel(),(LPTSTR)(LPCTSTR)dev_name);
	m_ip->setDev_2_name(dev_name);
	unsigned char *mac = (unsigned char*)dlg->getDevMac(dev_name);

	CIPLayer* IP = (CIPLayer*)dlg->mp_UnderLayer->GetUnderLayer()->GetUnderLayer();
	pcap_if_t* devs;
	for(devs = dlg->alldevs; devs; devs = devs->next)
	{
		if(!memcmp(devs->name,dev_name,strlen(dev_name)))
		{
			IP->setDev_2_IP((unsigned char*)(devs->addresses->addr->sa_data+2));
			break;
		}
	}
	CEthernetLayer* Ether = (CEthernetLayer*)dlg->mp_UnderLayer->GetUnderLayer()->GetUnderLayer()->GetUnderLayer()->GetUnderLayer();
	Ether->SetMac_Dev_2(mac);

	dlg->Dev_2 = pcap_open_live(dev_name,1500,1,1,errbuf);
	pcap_t* Dev_2 = dlg->getDev_2();
	CNILayer* NI = (CNILayer*)dlg->mp_UnderLayer->GetUnderLayer()->GetUnderLayer()->GetUnderLayer()->GetUnderLayer()->GetUnderLayer();
	NI->setDev_2(Dev_2);
	while(true)
	{
		//�ش� ��Ŷ�� ���������� loop
		do
		{
			Data = pcap_next(Dev_2,&info);
		}while(!Data);
		// �ش� ��Ŷ�� �ִٸ� �����ϱ� ���ؼ� NILayer���� Receive�ϱ� �����Ѵ�.
		NI->Receive((unsigned char*)(LPTSTR)(LPCTSTR)Data,eDev_2);		//���ڸ� �ϳ� �� �־ ��� device�� ������ ���
	}
}
void CDynamicRouterDlg::TimerThread(void* param)
{
	CDynamicRouterDlg* dlg = (CDynamicRouterDlg*)param;
	dlg->SetTimer(0,1000,NULL);
}
void CDynamicRouterDlg::OnTimer(UINT nIDEvent)
{
	if(exitFlag == 1)
		return;
	m_CacheTable.DeleteAllItems();
	char formatIP[16];
	char formatMAC[18];
	char buf[11];
	for(int i = 0; i<*cache_top; i++)
	{
		if(pcache_entry[i].cache_ttl != 0)
		{
			sprintf_s(formatIP,16,"%d.%d.%d.%d",pcache_entry[i].cache_ipaddr.S_un.S_un_b.s_b1,pcache_entry[i].cache_ipaddr.S_un.S_un_b.s_b2,pcache_entry[i].cache_ipaddr.S_un.S_un_b.s_b3,pcache_entry[i].cache_ipaddr.S_un.S_un_b.s_b4);
			sprintf_s(formatMAC,18,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",pcache_entry[i].cache_enetaddr.addr0,pcache_entry[i].cache_enetaddr.addr1,pcache_entry[i].cache_enetaddr.addr2,pcache_entry[i].cache_enetaddr.addr3,pcache_entry[i].cache_enetaddr.addr4,pcache_entry[i].cache_enetaddr.addr5);
			int nItemNum = m_CacheTable.GetItemCount();
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT; /* ù ��° */
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = 0;
			lvItem.pszText = (LPTSTR)(LPCSTR)formatIP;
			m_CacheTable.InsertItem(&lvItem);

			lvItem.mask = LVIF_TEXT; /* �� ��° */
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = 1;
			lvItem.pszText = (LPTSTR)(LPCSTR)formatMAC;
			m_CacheTable.SetItem(&lvItem);

			_itoa_s(pcache_entry[i].cache_type,buf,2,10);
			if(!memcmp(buf,"0",2))
				memcpy(buf,"Incomplete",11);
			else
				memcpy(buf,"Complete",9);
			lvItem.mask = LVIF_TEXT; /* �� ��° */
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = 2;
			lvItem.pszText = (LPTSTR)(LPCSTR)buf;
			m_CacheTable.SetItem(&lvItem);
		}
	}
	routing_table.DeleteAllItems();
	char formatDstIP[16];
	char formatNetmask[16];
	char formatGateway[16];
	char parseflag[4];
	char formatMetric[2];
	for(int i = 0; i<routing_top; i++)
	{
		if(routing_entry[i].routing_ttl != 0)
		{
			sprintf_s(formatDstIP,16,"%d.%d.%d.%d",routing_entry[i].DesIp.S_un.S_un_b.s_b1,routing_entry[i].DesIp.S_un.S_un_b.s_b2,routing_entry[i].DesIp.S_un.S_un_b.s_b3,routing_entry[i].DesIp.S_un.S_un_b.s_b4);
			sprintf_s(formatNetmask,16,"%d.%d.%d.%d",routing_entry[i].Netmask.S_un.S_un_b.s_b1,routing_entry[i].Netmask.S_un.S_un_b.s_b2,routing_entry[i].Netmask.S_un.S_un_b.s_b3,routing_entry[i].Netmask.S_un.S_un_b.s_b4);
			sprintf_s(formatGateway,16,"%d.%d.%d.%d",routing_entry[i].Gateway.S_un.S_un_b.s_b1,routing_entry[i].Gateway.S_un.S_un_b.s_b2,routing_entry[i].Gateway.S_un.S_un_b.s_b3,routing_entry[i].Gateway.S_un.S_un_b.s_b4);
			sprintf_s(formatMetric,2,"%d",routing_entry[i].metric);
			if(routing_entry[i].flag == 1)
				memcpy(parseflag,"U",2);
			else if(routing_entry[i].flag == 2)
				memcpy(parseflag,"G",2);
			else if(routing_entry[i].flag == 3)
				memcpy(parseflag,"UG",3);
			else if(routing_entry[i].flag == 4)
				memcpy(parseflag,"H",2);
			else if(routing_entry[i].flag == 5)
				memcpy(parseflag,"UH",3);
			else if(routing_entry[i].flag == 6)
				memcpy(parseflag,"GH",3);
			else if(routing_entry[i].flag == 7)
				memcpy(parseflag,"UGH",4);
			

			int nItemNum = routing_table.GetItemCount();
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = 0;
			lvItem.pszText = (LPTSTR)(LPCSTR)formatDstIP;
			routing_table.InsertItem(&lvItem);

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = 1;
			lvItem.pszText = (LPTSTR)(LPCSTR)formatNetmask;
			routing_table.SetItem(&lvItem);

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = 2;
			lvItem.pszText = (LPTSTR)(LPCSTR)formatGateway;
			routing_table.SetItem(&lvItem);

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = 3;
			lvItem.pszText = (LPTSTR)(LPCSTR)parseflag;
			routing_table.SetItem(&lvItem);

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = 4;
			lvItem.pszText = (LPTSTR)(LPCSTR)routing_entry[i].div_name;
			routing_table.SetItem(&lvItem);

			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = 5;
			lvItem.pszText = (LPTSTR)(LPCSTR)formatMetric;
			routing_table.SetItem(&lvItem);
		}
	}
	memcpy(formatDstIP,"0.0.0.0",8);
	memcpy(formatNetmask,"0.0.0.0",8);
	memcpy(formatGateway,"0.0.0.0",8);
	memcpy(parseflag,"-",2);
	memcpy(formatMetric,"-",2);
	int nItemNum = routing_table.GetItemCount();
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItemNum;
	lvItem.iSubItem = 0;
	lvItem.pszText = (LPTSTR)(LPCSTR)formatDstIP;
	routing_table.InsertItem(&lvItem);

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItemNum;
	lvItem.iSubItem = 1;
	lvItem.pszText = (LPTSTR)(LPCSTR)formatNetmask;
	routing_table.SetItem(&lvItem);

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItemNum;
	lvItem.iSubItem = 2;
	lvItem.pszText = (LPTSTR)(LPCSTR)formatGateway;
	routing_table.SetItem(&lvItem);

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItemNum;
	lvItem.iSubItem = 3;
	lvItem.pszText = (LPTSTR)(LPCSTR)parseflag;
	routing_table.SetItem(&lvItem);

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItemNum;
	lvItem.iSubItem = 4;
	lvItem.pszText = (LPTSTR)(LPCSTR)"-";
	routing_table.SetItem(&lvItem);

	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nItemNum;
	lvItem.iSubItem = 5;
	lvItem.pszText = (LPTSTR)(LPCSTR)formatMetric;
	routing_table.SetItem(&lvItem);
}
pcap_t* CDynamicRouterDlg::getDev_1()
{
	return Dev_1;
}
pcap_t* CDynamicRouterDlg::getDev_2()
{
	return Dev_2;
}
char AdapterList[Max_Num_Adapter][1024];
char* CDynamicRouterDlg::getDevMac(char *dev_name)
{
	LPADAPTER	lpAdapter = 0;
	int			i;
	char		AdapterName[8192];
	char		*temp,*temp1;
	int			AdapterNum=0;
	ULONG		AdapterLength;
	PPACKET_OID_DATA  OidData;
	BOOLEAN		Status;
	char *src;

	i=0;	

	AdapterLength = sizeof(AdapterName);

	PacketGetAdapterNames(AdapterName,&AdapterLength);
	temp=AdapterName;
	temp1=AdapterName;

	while ((*temp!='\0')||(*(temp-1)!='\0'))
	{
		if (*temp=='\0') 
		{
			memcpy(AdapterList[i],temp1,temp-temp1);
			if(!strcmp(AdapterList[i],dev_name))
				break;				
			temp1=temp+1;
			i++;
		}
		temp++;
	}

	AdapterNum=i;

	lpAdapter =   PacketOpenAdapter(AdapterList[i]);
	OidData = (PPACKET_OID_DATA)malloc(0x12);

	OidData->Oid = 0x01010102;

	OidData->Length = 6;
	ZeroMemory(OidData->Data, 6);

	Status = PacketRequest(lpAdapter, FALSE, OidData);

	PacketCloseAdapter(lpAdapter);
	OidData->Data[6] = '\0';
	src = (char*)OidData->Data;
	return src;
}
void CDynamicRouterDlg::setcache_entry(cache_entry* _cache_entry)
{
	pcache_entry = _cache_entry;
}
void CDynamicRouterDlg::OnAddRoutingEntry()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(EntryDlg.DoModal() == IDOK)
		return;
}
void CDynamicRouterDlg::addrouting_entry(unsigned char* dstip, unsigned char* Netmask, unsigned char* Gateway, int flag, unsigned char* dev_name,int m_metric)
{
	for(int i = 0; i<routing_top; i++)
	{
		if(!memcmp(routing_entry[i].DesIp.S_un.S_ip_addr,dstip,4))
		{
			AfxMessageBox("�̹� �����ϴ� entry�Դϴ�");
			return;
		}
	}
	for(int i = 0; i<routing_top; i++)
	{
		if(routing_entry[i].routing_ttl == 0)
		{
			memcpy(routing_entry[i].DesIp.S_un.S_ip_addr,dstip,4);
			memcpy(routing_entry[i].Netmask.S_un.S_ip_addr,Netmask,4);
			memcpy(routing_entry[i].Gateway.S_un.S_ip_addr,Gateway,4);
			routing_entry[i].flag = flag;
			memcpy(routing_entry[i].div_name,dev_name,strlen((char*)dev_name));
			routing_entry[i].metric = m_metric;
			routing_entry[i].routing_ttl = 300;
			return;
		}
	}
	memcpy(routing_entry[routing_top].DesIp.S_un.S_ip_addr,dstip,4);
	memcpy(routing_entry[routing_top].Netmask.S_un.S_ip_addr,Netmask,4);
	memcpy(routing_entry[routing_top].Gateway.S_un.S_ip_addr,Gateway,4);
	routing_entry[routing_top].flag = flag;
	memcpy(routing_entry[routing_top].div_name,dev_name,strlen((char*)dev_name)+1);
	routing_entry[routing_top].metric = m_metric;
	routing_entry[routing_top].routing_ttl = 300;
	routing_top++;
}
void CDynamicRouterDlg::OnCleanCacheTable()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_CacheTable.DeleteAllItems();
	for(int i = 0; i<*cache_top; i++)
		pcache_entry[i].cache_ttl = 0;
	*cache_top = 0;
}

void CDynamicRouterDlg::OnLvnItemchangedList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}

// ���Ͻ� ���̺� â�� ADD ��ư ������
void CDynamicRouterDlg::OnBnClickedButton6()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CProxyAddDlg dlg;

	// dlg���� ������ content�� ���� �ּҸ�� �߰�
	if(dlg.DoModal() == IDOK) 
	{
		LPPROXY_LIST proxy_element = (LPPROXY_LIST)malloc(sizeof(PROXY_LIST));
		
		Proxy_BufferAddr temp_BufferAddr = dlg.GetBufferAddr();
		
		proxy_element->proxy_entry.proxy_device = new CString(temp_BufferAddr.device);
		memcpy(proxy_element->proxy_entry.proxy_enetaddr, temp_BufferAddr.buffer_EtherAddr, 6);
		memcpy(proxy_element->proxy_entry.proxy_ipaddr, temp_BufferAddr.buffer_IPAddr, 4);
		proxy_element->next = NULL;
		
		m_ARPLayer->AddProxy(proxy_element);
		
		Invalidate();
	}
}

//����ִ�.
void CDynamicRouterDlg::OnLbnSelchangeListProxyTable()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

// ���Ͻ� ���̺� â�� DELETE ��ư ������
void CDynamicRouterDlg::OnBnClickedButton7()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(m_ProxyList.GetCurSel() == LB_ERR)
		return;

	CString temp_IPAddr;
	unsigned char IPAddr[10];
	m_ProxyList.GetText(m_ProxyList.GetCurSel(), temp_IPAddr);
	
	int offset = temp_IPAddr.Find("IP:");
	temp_IPAddr = temp_IPAddr.Right(temp_IPAddr.GetLength() - offset -4);
	offset = temp_IPAddr.Find("[MAC:");
	temp_IPAddr = temp_IPAddr.Left(offset);
	offset = temp_IPAddr.Find("]");
	temp_IPAddr = temp_IPAddr.Left(offset);
	
	for(int i = 0; i < 10; i++) 
	{
		CString temp;
		unsigned char char_temp[10];
		
		AfxExtractSubString(temp, temp_IPAddr, i, '.');
		sscanf(temp,"%d", &char_temp[i]);
		IPAddr[i] = char_temp[i];
	}
	
	m_ARPLayer->DeleteProxy(IPAddr);
	
	Invalidate();
}

// ���Ͻ� ���̺� â�� DELETE ALL ��ư ������
void CDynamicRouterDlg::OnBnClickedButton8()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_ARPLayer->AllDeleteProxy();// ��� element ����
	Invalidate();
}


void CDynamicRouterDlg::OnLbnSelchangeList4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
