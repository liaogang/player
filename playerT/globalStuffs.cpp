#include "globalStuffs.h"


static PlayList *pActivePlaylist=0;
PlayList* ActivePlaylist(){return pActivePlaylist;}
void SetActivePlaylist(PlayList* p){pActivePlaylist=p;}

static PlayListItem* playingItem=0;
PlayListItem * GetPlayingItem(){return playingItem;}
void  SetPlayingItem(PlayListItem *item){playingItem=item;}

