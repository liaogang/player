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

		auto f = that->infoDisplay.find(L"<a href=\"");
		auto refEnd = that->infoDisplay.find(L"\"",f+9);
		if (f != tstring::npos)
		{
			auto e = that->infoDisplay.find(L"</a",f);
			if (e != tstring::npos)
			{
				tstring text = that->infoDisplay.substr(refEnd + 2, e - 2 - refEnd );
				tstring link = that->infoDisplay.substr(f + 9, refEnd -f - 9);

				that->link.SetLabel(text.c_str());
				that->link.SetHyperLink(link.c_str());
			}
		}

		that->infoDisplay = that->infoDisplay.substr( 0, f-1);


		that->label1.SetWindowText(that->infoDisplay.c_str());

		delete cSummary;
	}

}

void _scrobble(void *arg)
{
	CLastFmPaneView *that = (CLastFmPaneView*)arg;
	that->scrobble();
}


void CLastFmPaneView::scrobble()
{
	char *cArtist = Unicode2UTF8((LPWSTR)(*(item))->GetArtist().c_str());
	string artist(cArtist);
	delete cArtist;


	char *cTrack= Unicode2UTF8((LPWSTR)(*(item))->GetTitle().c_str());
	string track(cTrack);
	delete cTrack;

	LFUser user;
	if (auth(user))
	{
		vector<string> artists({ artist });
		vector<string> tracks({ track });
		vector<string> times({ "" });

		if (track_scrobble(user.sessionKey, artists, tracks, times))
		{

		}
	}
}

void CLastFmPaneView::TrackChanged(LPCPlayListItem *item)
{
	infoDisplay = L"";
	Invalidate();
	this->item = item;
	job = Job1(_TrackChanged , (void*)this);
	doJob(&job);

	job2 = Job1(_scrobble, (void*)this);
	doJob(&job2);

}

