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
		


// 
// 		if(DAUDIO_Start((void*)m_Player->m_info, TRUE))
// 		{
// 			printf("start play ...\n");
// 			char buffer[buffersize];
// 			DWORD len = 0;
// 			DWORD offset = 0;
// 			DWORD written = 0;
// 			BOOL EndOfInput = FALSE;
// 			while(!m_Stop)
// 			{
// 				memset(buffer, 0, buffersize);
// 				pInput->FillBuffer((void*)buffer, buffersize, &EndOfInput);
// 
// 				len = buffersize;
// 				offset = 0;
// 				written = 0;
// 
// 				while(TRUE)
// 				{
// 					m_cs->Enter();
// 					int thisWritten = DAUDIO_Write((void*)m_Player->m_info, buffer+offset, len);
// 					if(thisWritten < 0) break;
// 					m_Player->m_bytePosition += thisWritten;
// 					m_cs->Leave();
// 
// 					len -= thisWritten;
// 					written += thisWritten;
// 					if(len > 0)
// 					{
// 						offset += thisWritten;
// 						m_cs->Enter();
// 						Sleep(waitTime);
// 						m_cs->Leave();
// 					}
// 					else break;
// 				}
// 
// 
// 				if(EndOfInput)
// 					break;
// 			}
// 
// 			m_Player->m_SpectrumAnalyser->Stop();
// 			DAUDIO_Stop((void*)m_Player->m_info, TRUE);
// 			DAUDIO_Close((void*)m_Player->m_info, TRUE);
// 			m_Player->m_bytePosition = 0;
// 
// 			printf("stop play.\n");
// 		}
// 
// 		m_Player->m_info = NULL;
// 
// 
// 	}
};

