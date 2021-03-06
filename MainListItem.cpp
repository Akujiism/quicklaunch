/*
 * Copyright 2010-2015. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include <Application.h>
#include <ControlLook.h>

#include "MainListItem.h"
#include "QuickLaunch.h"

MainListItem::MainListItem(BEntry *entry)
		  :BListItem()
{
	BNode node;
	BNodeInfo node_info;

	// try to get node info for this entry
	if ((node.SetTo(entry) == B_NO_ERROR) &&
		(node_info.SetTo(&node) == B_NO_ERROR)) {

		// cache name and path
		entry->GetName(fName);
		entry->GetPath(&fPath);
		
		// create bitmap large enough for icon
		fIcon = new BBitmap(
			BRect(0, 0, kBitmapSize - 1, kBitmapSize - 1), 0, B_RGBA32);

		// cache the icon
		node_info.GetTrackerIcon(fIcon);
		
		// cache ref
		entry->GetRef(&fRef);
		
		// cache version info
		BFile file(entry, B_READ_ONLY);
		if (file.InitCheck() != B_OK)
			return;
		BAppFileInfo info(&file);
		if (info.InitCheck() != B_OK)
			return;
		info.GetVersionInfo(&fVersionInfo, B_APP_VERSION_KIND);

	}
	else {
		fIcon = NULL;
		strcpy(fName, "<Lost File>");
	}
}


MainListItem::~MainListItem()
{
	delete fIcon;
}


void
MainListItem::DrawItem(BView *view, BRect rect, bool complete)
{
	QLApp *app = dynamic_cast<QLApp *> (be_app);
	float spacing = be_control_look->DefaultLabelSpacing();
	float offset = spacing;
	BFont appfont;
	BFont pathfont;
	font_height finfo;

	// set background color

    rgb_color bgColor;
	if (IsSelected())
		bgColor = ui_color(B_LIST_SELECTED_BACKGROUND_COLOR);
	else
		bgColor = ui_color(B_LIST_BACKGROUND_COLOR);

	view->SetHighColor(bgColor);
	view->SetLowColor(bgColor);
    view->FillRect(rect);

	// if we have an icon, draw it

	if (fIcon) {
        view->SetDrawingMode(B_OP_OVER);
        view->DrawBitmap(fIcon, BPoint(rect.left + spacing / 2,
			rect.top + (rect.Height() - kBitmapSize) / 2));
        view->SetDrawingMode(B_OP_COPY);
		offset = fIcon->Bounds().Width() + offset + spacing;
    }

	// application name

    if (IsSelected())
    	view->SetHighColor(ui_color(B_LIST_SELECTED_ITEM_TEXT_COLOR));
    else
    	view->SetHighColor(ui_color(B_LIST_ITEM_TEXT_COLOR));

//	if (appfont.Size() > 18)
//		appfont.SetSize(18);

    appfont.GetHeight(&finfo);
    appfont.SetFace(B_BOLD_FACE);
    view->SetFont(&appfont);

	if (app->fSettings->GetShowVersion() || app->fSettings->GetShowPath()) {
	    view->MovePenTo(offset,
	        rect.top + ((rect.Height() - (finfo.ascent +
	        finfo.descent + finfo.leading)) / 2) +
	        (finfo.ascent + finfo.descent) - appfont.Size() + 2 + 3);
	} else {
		view->MovePenTo(offset,
	        rect.top - 2 + ((rect.Height() - (finfo.ascent +
	        finfo.descent + finfo.leading)) / 2) +
	        (finfo.ascent + finfo.descent));
	}

    float width, height;
    view->GetPreferredSize(&width, &height);
    BString string(fName);
    view->TruncateString(&string, B_TRUNCATE_MIDDLE, width - kBitmapSize
    	- offset / 2);
    view->DrawString(string.String());

	// application path and version

    if (IsSelected())
    	view->SetHighColor(tint_color(ui_color(B_LIST_SELECTED_ITEM_TEXT_COLOR),
			0.6));
    else
		view->SetHighColor(tint_color(ui_color(B_LIST_ITEM_TEXT_COLOR), 0.6));
    	
	pathfont.SetSize(appfont.Size() - 2);
    pathfont.GetHeight(&finfo);
    view->SetFont(&pathfont);

    view->MovePenTo(offset,
        rect.top + appfont.Size() - pathfont.Size() + 3 + ((rect.Height()
        - (finfo.ascent + finfo.descent + finfo.leading)) / 2)
        + (finfo.ascent + finfo.descent));

	BPath parent;
    fPath.GetParent(&parent);
    string = "";
    
    char text[256];
    if (app->fSettings->GetShowVersion()) {
    	snprintf(text, sizeof(text), "%ld", fVersionInfo.major);
    	string << "v" << text << ".";
	    snprintf(text, sizeof(text), "%ld", fVersionInfo.middle);
	    string << text << ".";
	    snprintf(text, sizeof(text), "%ld", fVersionInfo.minor);
	    string << text;
    }
    if (app->fSettings->GetShowVersion() && app->fSettings->GetShowPath())
	    string << " - ";
	    
    if (app->fSettings->GetShowPath()) {
    	string << parent.Path();
    	string << "/";
    }
    view->TruncateString(&string, B_TRUNCATE_MIDDLE, width - kBitmapSize
    	- offset/2);
    view->DrawString(string.String());

	// draw lines

	view->SetHighColor(tint_color(ui_color(B_CONTROL_BACKGROUND_COLOR),
		B_DARKEN_1_TINT));
	view->StrokeLine(rect.LeftBottom(), rect.RightBottom());
}


void MainListItem::Update(BView *owner, const BFont *finfo)
{
	// we need to override the update method so we can make sure the
	// list item size doesn't change
	BListItem::Update(owner, finfo);

	font_height	fheight;
	finfo->GetHeight(&fheight);
	float spacing = be_control_look->DefaultLabelSpacing();
	float height = ceilf(fheight.ascent + 2 + fheight.leading / 2
		+ fheight.descent);

	if (Height() < 18)
		SetHeight(fIcon->Bounds().Height() + spacing + 4);
	else
		SetHeight(height * 2 + 4);
}
