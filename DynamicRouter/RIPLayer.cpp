#include "StdAfx.h"
#include "RIPLayer.h"

CRIPLayer::CRIPLayer(char *pName) : CBaseLayer(pName)
{
}
CRIPLayer::~CRIPLayer(void)
{
}
BOOL CRIPLayer::Send(unsigned char* ppayload, int length, unsigned short dev_num)
{
	//SetTimer()�� ���۽��� �ð��� ������ table�� ������.
	return 0;
}
BOOL CRIPLayer::Receive( unsigned char* ppayload, unsigned short dev_num)
{
	PRIP pFrame = (PRIP)ppayload;
	/////////////////////////////////////
	//DVRP alorithm �� ���� ���̺� ����//
	/////////////////////////////////////
	return 0;
}