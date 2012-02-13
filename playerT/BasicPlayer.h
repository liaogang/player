#pragma once

class CBasicPlayer
{
private:
	CPlayerThread m_PlayerThread;
	CSpectrumAnalyser m_SpectrumAnalyser;
public:
	CBasicPlayer(void)
	{
		m_PlayerThread=new CPlayerThread;
		m_SpectrumAnalyser=new CSpectrumAnalyser;


	}

	~CBasicPlayer(void)
	{
		if(!m_PlayerThread) delete m_PlayerThread;
		if (!m_SpectrumAnalyser) delete m_SpectrumAnalyser; 
	}
};
