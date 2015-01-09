#pragma  once
#include "lastFmPane.h"
#include "Last_fm_user.h"
#include "Last_fm_api.h"
#include "StringConvertions.h"
#include "PlayList.h"


void _TrackChanged(void *arg)
{
	CLastFmPaneView *that= (CLastFmPaneView*)arg;

	char *cArtist = Unicode2UTF8((LPWSTR)(*(that->item))->GetArtist().c_str());
	string artist(cArtist);
	delete cArtist;

	LFArtist lfArtist;
	if (artist_getInfo(artist, lfArtist))
	{
		LPWSTR cSummary =UTF82Unicode((LPSTR)lfArtist.bio.summary.c_str());
		that->infoDisplay = cSummary;
		delete cSummary;
		that->Invalidate();
	}

}

void CLastFmPaneView::TrackChanged(LPCPlayListItem *item)
{
	infoDisplay = L"";
	Invalidate();
	this->item = item;
	job = Job1(_TrackChanged , (void*)this);
	doJob(&job);
}

