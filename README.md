# This is a player looks like foobar2000 in windows.


##Main features

* Supported audio formats: MP3 and  wave.
* (More format will be open support in the near futhure)
* Easily customizable user interface layout.   

##主要特性

* 支持 MP3 及  wave 音频 格式 . (更多格式会在不久的将来被支持)
* 简洁及容易定制的用户界面
* 提供托盘版精简程序,删除了程序中与界面相关的模块与库. 
* 播放列队及搜索歌曲列表 
* 封面显示  
* 歌词  
	* 支持从千千静听歌词库下载歌词  
	* 搜索并加载指定目录下的相应歌词  

##How to build it

This is a msvc project.
download the library player will relay to:  

* download wtl from SourceForge or use the ./player/headers/wtl instead.
* change directory to player/jsoncpp/makefiles/ , and build it with visual studio.
* update the git submodules in folder :  
	* Last.fm/

##about the configure files in root directory

* core.cfg , contains informations about current player's volume and track duration and library diretory and lyrics directory and so on.
* ui.cfg , contains informations about the ui interface.
* keymaps.cfg , the key bindings format in json.
* lastFmUser.cfg , Last.fm user auth information caches.
* localRecords.cfg, records not yet committed to last.fm user profiles.







