#pragma once

#include "Thread.h"
class CBasicPlayer;
class CPlayerThread : public CThread
{
private:
	CBasicPlayer *m_pPlayer;
public:
	CPlayerThread(CBasicPlayer *pPlayer);
	~CPlayerThread();

	void Excute();
};

