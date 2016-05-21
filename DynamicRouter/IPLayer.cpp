#pragma once

#include "StdAfx.h"
#include "IPLayer.h"
#include "ARPLayer.h"

CIPLayer::CIPLayer(char* pName) : CBaseLayer(pName)
{
	memset(defaultGateway,0,4);
}
CIPLayer::~CIPLayer(void)
{
}
BOOL CIPLayer::Send(unsigned char* ppayload, int nlength, unsigned short dev_num)
{
	PIP pFrame;
	pFrame = (PIP)ppayload;
	CARPLayer *ARP = (CARPLayer*)mp_UnderLayer;
	ARP->setForwardingIp(getForwardingIp(pFrame->ip_dst));
	return mp_UnderLayer->Send((unsigned char*)pFrame,MAX_IP_SIZE,dev_num);
}
BOOL CIPLayer::Receive(unsigned char* ppayload, unsigned short type, unsigned short dev_num)
{
	//Routing Table�� ���� ����������� �������� �����Ѵ�.
	//���� cache�� masking �� �ּ��� mac�� ������ ARP->Send�� �Ѵ�.
	//									   ������ �ٷ� Ethernet->Send
	CARPLayer *ARP = (CARPLayer*)mp_UnderLayer;
	PIP pFrame = (PIP)ppayload;
	if(ntohs(type) == 0x0800)
	{
		//routing table���� gateway�� cache table���� ��ġ�ϴ� mac�� ã�´�.
		//�ִٸ� ethernet dstmac���ϰ� srcmac(��ġ������)���ϰ� srcip,dstip�� �״�� EthernetLayer�� ������.
		//���ٸ� �ϴ� �����ϰ� ARP->Send�� �˾ƿ����� ���� ���� ��, EthernetLayer�� ������.
		
		//dstip�� �̿��Ͽ� routing table���� ��ġ�� ���� dev_num ����
		dev_num = getForwardDev(pFrame->ip_dst);
		
		pFrame->ip_verlen = 69;
		pFrame->ip_tos = 0x0;
		ARP->setForwardingIp(getForwardingIp(pFrame->ip_dst));
		return mp_UnderLayer->Send((unsigned char*)pFrame,MAX_IP_DATA,dev_num);
	}
	else if(ntohs(type) == 0x0806)
	{
		return mp_UnderLayer->Receive((unsigned char*)pFrame,dev_num);
	}
	else if(ntohs(type) == 0x0520)
		return mp_aUpperLayer[0]->Receive(pFrame->ip_data,dev_num);
	return false;
}
void CIPLayer::setDev_1_IP(unsigned char* ip)
{
	memcpy(Dev_1_IP,ip,4);
}
void CIPLayer::setDev_2_IP(unsigned char* ip)
{
	memcpy(Dev_2_IP,ip,4);
}
unsigned char* CIPLayer::getDev_1_IP(void)
{
	return Dev_1_IP;
}
unsigned char* CIPLayer::getDev_2_IP(void)
{
	return Dev_2_IP;
}
/*
	Flag�� G�� �ִٸ� return gateway, H�� �ִٸ� return dstip
*/
unsigned char* CIPLayer::getForwardingIp(unsigned char* dstip)
{
	unsigned char maskResult[4];
	for(int i = 0;i<*routing_top; i++)
	{
		for(int j = 0; j<4; j++)
			maskResult[j] = dstip[j]&routing_entry[i].Netmask.S_un.S_ip_addr[j];
		if(!memcmp(routing_entry[i].DesIp.S_un.S_ip_addr,maskResult,4))
		{
			if(routing_entry[i].flag == 2 || routing_entry[i].flag == 3 || routing_entry[i].flag == 6)
				return routing_entry[i].Gateway.S_un.S_ip_addr;
			else
				return dstip;
		}
	}
	return defaultGateway;
}
void CIPLayer::setRouting_entry(Routing_table *routing_entry)
{
	this->routing_entry = routing_entry;
}
void CIPLayer::setRouting_top(int *top)
{
	routing_top = top;
}
int CIPLayer::getForwardDev(unsigned char* dstip)
{
	unsigned char maskResult[4];
	for(int i = 0;i<*routing_top; i++)
	{
		for(int j = 0; j<4; j++)
			maskResult[j] = dstip[j]&routing_entry[i].Netmask.S_un.S_ip_addr[j];
		if(!memcmp(routing_entry[i].DesIp.S_un.S_ip_addr,maskResult,4))
		{
			if(!memcmp(routing_entry[i].div_name,dev_1_name,100))
				return 0;
			else if(!memcmp(routing_entry[i].div_name,dev_2_name,100))
				return 1;
			break;
		}
	}
	return 2;
}
void CIPLayer::setDev_1_name(char *dev_name)
{
	memcpy(dev_1_name,dev_name,100);
}
void CIPLayer::setDev_2_name(char *dev_name)
{
	memcpy(dev_2_name,dev_name,100);
}