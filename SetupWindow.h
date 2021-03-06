/*
 * Copyright 2010-2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef SETUP_WINDOW_H
#define SETUP_WINDOW_H

#include "SetupListView.h"

#include <Application.h>
#include <Button.h>
#include <CheckBox.h>
#include <Entry.h>
#include <FilePanel.h>
#include <FindDirectory.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <ListView.h>
#include <ListItem.h>
#include <Message.h>
#include <Path.h>
#include <Screen.h>
#include <ScrollView.h>
#include <Window.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define VERSION_CHK		'chve'
#define PATH_CHK		'chpa'
#define DELAY_CHK		'chde'
#define SAVESEARCH_CHK	'chss'
#define ONTOP_CHK		'chot'
#define IGNORE_CHK		'chig'
#define ADD_BUT			'addb'
#define REM_BUT			'remb'
#define FILEPANEL		'file'


class SetupWindow : public BWindow {
public:
					SetupWindow(BRect rect);
	virtual			~SetupWindow();

	bool			QuitRequested();
	void			MessageReceived(BMessage* message);

	BButton			*fButRem;
	BCheckBox		*fChkVersion;
	BCheckBox		*fChkPath;
	BCheckBox		*fChkDelay;
	BCheckBox		*fChkSaveSearch;
	BCheckBox		*fChkOnTop;
	BCheckBox		*fChkIgnore;
	SetupListView	*fIgnoreList;

private:
	BScrollView		*fIgnoreScroll;
	BButton			*fButAdd;
	BFilePanel		*fOpenPanel;

};

#endif // SETUP_WINDOW_H
