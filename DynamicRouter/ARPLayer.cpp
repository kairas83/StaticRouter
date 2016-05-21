#include "StdAfx.h"
#include <process.h>
#include "ARPLayer.h"
#include "IPLayer.h"
#include "EthernetLayer.h"

CARPLayer::CARPLayer(char* pName) : CBaseLayer(pName)
{
	msg_flag = 0;
	cache_top = 0;
	_beginthread(timeCountThread,0, this);					// timeCountThread ����.
}

CARPLayer::~CARPLayer(void)
{
}
BOOL CARPLayer::Send(unsigned char* ppayload, int nlength, unsigned short dev_num)
{
	//cache table���� gateway�� �ش��ϴ� mac�� �� �������� ������.
	//���ٸ� ARP
	ARP arpFrame;
	PIP pFrame = (PIP)ppayload;
	unsigned char broadcast[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	CEthernetLayer* m_Ether = (CEthernetLayer*)mp_UnderLayer;
	CIPLayer *m_IP = (CIPLayer*)m_Ether->GetUpperLayer(0);
	unsigned char* pNextHopMac = getNextHopMac(forwardingIp);
	if(pNextHopMac != NULL)		//cache table�� ������ ���� ���, �ش� mac���� msg ����
	{
		//�ϴ� ���� ���� �� ��û
		if(dev_num == 0)
			m_Ether->SetDstMac_Dev_1(pNextHopMac);
		else if(dev_num == 1)
			m_Ether->SetDstMac_Dev_2(pNextHopMac);
		mp_UnderLayer->Send(ppayload,nlength,0x0800,dev_num);
	}
	else						//cache table�� ������ ���� ���, ARP ��û
	{
		memcpy(storedMsg,ppayload,MAX_IP_SIZE);
		msg_flag = 1;
		arpFrame.apr_prototype = htons(0x0800);
		arpFrame.arp_dhtype = htons(0x0001);
		arpFrame.arp_hdlength = 6;
		arpFrame.arp_protolength = 4;
		arpFrame.arp_op = htons(0x0001);
		if(dev_num == 0)
		{
//			dev_num = 1;
			m_Ether->SetDstMac_Dev_1(broadcast);
			memcpy(&arpFrame.arp_srchdaddr,m_Ether->GetMac_Dev_1(),6);
			memcpy(&arpFrame.arp_srcprotoaddr,m_IP->getDev_1_IP(),4);
		}
		else if(dev_num == 1)
		{
//			dev_num = 0;
			m_Ether->SetDstMac_Dev_2(broadcast);							//ethernet dstmac����
			memcpy(&arpFrame.arp_srchdaddr,m_Ether->GetMac_Dev_2(),6);	//srcmac����
			memcpy(&arpFrame.arp_srcprotoaddr,m_IP->getDev_2_IP(),4);
		}
		memcpy(&arpFrame.arp_destprotoaddr,forwardingIp,4);		//dstip����
		ETHERNET_ADDR nullmac;
		IP_ADDR dstip;
		memset(&nullmac,0,sizeof(nullmac));
		memcpy(dstip.S_un.S_ip_addr,pFrame->ip_dst,4);
		UpdateCacheTable(dstip,nullmac,180,0);
		return mp_UnderLayer->Send((unsigned char*)&arpFrame,MAX_ARP_SIZE,0x0806,dev_num);
	}
	return false;
}
BOOL CARPLayer::Receive(unsigned char* ppayload, unsigned short dev_num)
{
	PARP pFrame = (PARP)ppayload;			// ���� ��Ŷ
	CIPLayer *IpLayer = (CIPLayer*)mp_UnderLayer->GetUpperLayer(0);
	CEthernetLayer* m_Ether = (CEthernetLayer*)IpLayer->GetUnderLayer()->GetUnderLayer();
	
	//broad�� ������ ���� ���� �ʵ���
	if(!memcmp(pFrame->arp_srchdaddr.addr_,m_Ether->GetMac_Dev_1(),6) || !memcmp(pFrame->arp_srchdaddr.addr_,m_Ether->GetMac_Dev_2(),6))
		return false;

	/*
		��û�̰� �����̰� ĳ�����̺� ĳ�ð� �ִٸ� ����, ������ �߰�. ������ ��쿡�� ����.
	*/
	UpdateCacheTable(pFrame->arp_srcprotoaddr,pFrame->arp_srchdaddr,1200,1);		//complement
	if(pFrame->arp_op == htons(0x0001))		//��û �޽��� �� ���
	{
		//���� �� ����
		char tmp_ip[4];
		CEthernetLayer* m_Ether = (CEthernetLayer*)mp_UnderLayer;
		memcpy(&pFrame->arp_desthdaddr,&pFrame->arp_srchdaddr,6);
		memcpy(tmp_ip,&pFrame->arp_srcprotoaddr,4);
		memcpy(&pFrame->arp_srcprotoaddr,&pFrame->arp_destprotoaddr,4);
		memcpy(&pFrame->arp_destprotoaddr,tmp_ip,4);
		if(dev_num == 0)
		{
			m_Ether->SetDstMac_Dev_1(pFrame->arp_srchdaddr.S_un.s_ether_addr);
			memcpy(&pFrame->arp_srchdaddr,m_Ether->GetMac_Dev_1(),6);
		}
		else if(dev_num == 1)
		{
			m_Ether->SetDstMac_Dev_2(pFrame->arp_srchdaddr.S_un.s_ether_addr);
			memcpy(&pFrame->arp_srchdaddr,m_Ether->GetMac_Dev_2(),6);
		}
		pFrame->arp_op = htons(0x0002);
		return mp_UnderLayer->Send((unsigned char*)pFrame,MAX_ARP_SIZE,0x0806,dev_num);		// ���ҵ� ARP��Ŷ�� ����޽����� ����
	}
	else if(pFrame->arp_op == htons(0x0002) && msg_flag == 1)		//���� �޽��� �� ���		//flag �ᵵ �ǳ�....?????
	{
		return IpLayer->Send(storedMsg,MAX_IP_SIZE,dev_num);		//�³�...???????
		msg_flag = 0;
	}			
	return false;
}
//ĳ�����̺��� ���� ��� �ִ� ĳ�ø� ã�� �����Ѵ�. ���� ����ִ� entry�� ���ٸ� ���� entry�� ã�´�. �����͵� ���ٸ� �߰��Ѵ�.
void CARPLayer::UpdateCacheTable(IP_ADDR ip, ETHERNET_ADDR mac, int time, int type)
{
	for(int i = 0; i<cache_top; i++)
	{
		if(!memcmp(&cache_entry[i].cache_ipaddr,&ip,4))
		{
			cache_entry[i].cache_enetaddr = mac;
			cache_entry[i].cache_ttl = time;
			cache_entry[i].cache_type = type;
			return;
		}
	}
	for(int i = 0; i<cache_top; i++)
	{
		if(cache_entry[i].cache_ttl == 0)
		{
			cache_entry[i].cache_ipaddr = ip;
			cache_entry[i].cache_enetaddr = mac;
			cache_entry[i].cache_ttl = time;
			cache_entry[i].cache_type = type;
			return;
		}
	}
	cache_entry[cache_top].cache_ipaddr = ip;
	cache_entry[cache_top].cache_enetaddr = mac;
	cache_entry[cache_top].cache_ttl = time;
	cache_entry[cache_top].cache_type = type;
	cache_top++;
}
void CARPLayer::timeCountThread(void* param)
{
	CARPLayer *ARP = (CARPLayer*)param;

	/* ttl�� 0�� ������ �ʸ� ����. 0�̵Ǹ� �������� ���� */
	while(1)
	{
		Sleep(1000);
		for(int i = 0; i<ARP->cache_top; i++)
		{
			if(ARP->cache_entry[i].cache_ttl != 0)
				ARP->cache_entry[i].cache_ttl--;
		}
	}		
}
int* CARPLayer::getCache_top(void)
{
	return &cache_top;
}
cache_entry* CARPLayer::getcache_entry(void)
{
	return cache_entry;
}
void CARPLayer::setForwardingIp(unsigned char* forwardingIp)
{
	memcpy(this->forwardingIp,forwardingIp,4);
}
/*
	�ִٸ� �ش� mac�� return
	���ٸ� NULL return
*/
unsigned char* CARPLayer::getNextHopMac(unsigned char* gateway)
{
	for(int i = 0; i<cache_top; i++)
	{
		if(!memcmp(cache_entry[i].cache_ipaddr.S_un.S_ip_addr,gateway,4))
			return cache_entry[i].cache_enetaddr.addr_;
	}
	return NULL;
}