#pragma  once
#include "lastFmPane.h"
#include "Last_fm_user.h"
#include "Last_fm_api.h"
#include "StringConvertions.h"
#include "PlayList.h"

void CLastFmPaneView::TrackChanged(LPCPlayListItem *item)
{
	char *cArtist = Unicode2UTF8((LPWSTR)(*item)->GetArtist().c_str());
	
	string artist(cArtist);

	LFArtist lfArtist;
	if (artist_getInfo(artist, lfArtist))
	{
		cout << lfArtist.name << endl;
	}

	
	infoDisplay = UTF82Unicode( (LPSTR)lfArtist.bio.summary.c_str() );

}

