/*
 * Copyright 2010-2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include "MainListItem.h"
#include "QuickLaunch.h"
#include "QLSettings.h"
#include "SetupWindow.h"

#include <Catalog.h>
#include <ControlLook.h>
#include <LayoutBuilder.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "SetupWindow"


static int
compare_items(const void* a, const void* b)
{
	BStringItem* stringA = *(BStringItem**)a;
	BStringItem* stringB = *(BStringItem**)b;

	return strcmp(stringA->Text(), stringB->Text());
}


SetupWindow::SetupWindow(BRect frame)
	:
	BWindow(frame, B_TRANSLATE("Setup"), B_TITLED_WINDOW_LOOK,
		B_NORMAL_WINDOW_FEEL,
		B_NOT_ZOOMABLE | B_ASYNCHRONOUS_CONTROLS | B_CLOSE_ON_ESCAPE)
{
	fChkVersion = new BCheckBox("VersionChk",
		B_TRANSLATE("Show application version"),
		new BMessage(VERSION_CHK), B_WILL_DRAW | B_NAVIGABLE);
	fChkPath = new BCheckBox("PathChk", B_TRANSLATE("Show application path"),
		new BMessage(PATH_CHK), B_WILL_DRAW | B_NAVIGABLE);
	fChkDelay = new BCheckBox("DelayChk",
		B_TRANSLATE("Wait for a second letter before searching"),
		new BMessage(DELAY_CHK), B_WILL_DRAW | B_NAVIGABLE);
	fChkSaveSearch = new BCheckBox("SaveSearchChk",
		B_TRANSLATE("Remember last search term"),
		new BMessage(SAVESEARCH_CHK), B_WILL_DRAW | B_NAVIGABLE);
	fChkOnTop = new BCheckBox("OnTopChk",
		B_TRANSLATE("Window always on top"),
		new BMessage(ONTOP_CHK), B_WILL_DRAW | B_NAVIGABLE);
	fChkIgnore = new BCheckBox("IgnoreChk",
		B_TRANSLATE("Ignore these files & folders (and their subfolders):"),
		new BMessage(IGNORE_CHK), B_WILL_DRAW | B_NAVIGABLE);
	fIgnoreList = new SetupListView();
	fIgnoreScroll = new BScrollView("IgnoreList", fIgnoreList,
		B_FOLLOW_ALL_SIDES, false, true, B_FANCY_BORDER);
	fButAdd = new BButton(BRect(), "AddButton",
		B_TRANSLATE("Add" B_UTF8_ELLIPSIS), new BMessage(ADD_BUT),
		B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	fButRem = new BButton(BRect(), "RemButton", B_TRANSLATE("Remove"),
		new BMessage(REM_BUT), B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW
		| B_NAVIGABLE);
	fButRem->SetEnabled(false);

	fChkVersion->SetTarget(be_app);
	fChkPath->SetTarget(be_app);
	fChkDelay->SetTarget(be_app);
	fChkSaveSearch->SetTarget(be_app);
	fChkOnTop->SetTarget(be_app);
	fChkIgnore->SetTarget(be_app);

	// Build the layout

	float spacing = be_control_look->DefaultItemSpacing();

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.AddGroup(B_VERTICAL, 0)
			.Add(fChkVersion)
			.Add(fChkPath)
			.Add(fChkDelay)
			.Add(fChkSaveSearch)
			.Add(fChkOnTop)
			.SetInsets(spacing/2, spacing/2, spacing/2, 0)
		.End()
		.AddGroup(B_VERTICAL, 0)
			.Add(fChkIgnore)
			.Add(fIgnoreScroll)
			.SetInsets(spacing/2, spacing/2, spacing/2, 0)
		.End()
		.AddGroup(B_HORIZONTAL, spacing/2)
			.Add(fButAdd)
			.Add(fButRem)
			.SetInsets(0, spacing/2, spacing/2, spacing/2)
		.End();

	QLApp *app = dynamic_cast<QLApp *> (be_app);
	fChkVersion->SetValue(app->fSettings->GetShowVersion());
	fChkPath->SetValue(app->fSettings->GetShowPath());
	fChkDelay->SetValue(app->fSettings->GetDelay());
	fChkSaveSearch->SetValue(app->fSettings->GetSaveSearch());
	fChkOnTop->SetValue(app->fSettings->GetOnTop());
	fChkIgnore->SetValue(app->fSettings->GetShowIgnore());

	int32 value = app->fSettings->GetOnTop();
	SetFeel(value ?	B_MODAL_ALL_WINDOW_FEEL : B_NORMAL_WINDOW_FEEL);

	fOpenPanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL,
		B_FILE_NODE | B_DIRECTORY_NODE);
	fOpenPanel->SetTarget(this);
}


SetupWindow::~SetupWindow()
{
	delete fOpenPanel;
}


bool
SetupWindow::QuitRequested()
{
	this->Hide();
	return false;
}


void
SetupWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case ADD_BUT:
		{
			QLApp *app = dynamic_cast<QLApp *> (be_app);
			int32 value = app->fSettings->GetOnTop();
			fOpenPanel->Window()->SetFeel(value ?
				B_MODAL_ALL_WINDOW_FEEL : B_NORMAL_WINDOW_FEEL);

			fOpenPanel->Show();
			break;
		}
		case REM_BUT:
		{
			fIgnoreList->RemoveItem(fIgnoreList->CurrentSelection());
			be_app->PostMessage(FILEPANEL);
			break;
		}
		case B_REFS_RECEIVED:
		{
			int32		ref_num;
			entry_ref	ref;
			status_t	err;
			ref_num = 0;

	    	while ((err = message->FindRef("refs", ref_num, &ref)) == B_OK) {
				BPath path;
				BEntry *entry = new BEntry(&ref);
				entry->GetPath(&path);
				BStringItem *newitem = new BStringItem(path.Path());
				bool duplicate = false;

				for (int i = 0; i < fIgnoreList->CountItems(); i++)
				{
					BStringItem *sItem = dynamic_cast<BStringItem *>
						(fIgnoreList->ItemAt(i));
					if (strcmp(sItem->Text(), newitem->Text()) == 0)
						duplicate = true;
				}
				if (!duplicate)	{
					fIgnoreList->AddItem(newitem);
					fIgnoreList->SortItems(&compare_items);
				}
				ref_num++;
			}
			be_app->PostMessage(FILEPANEL);
		}
	}
}
