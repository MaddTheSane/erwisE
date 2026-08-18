static char *rcsid = "$Id$";

#include "UiIncludes.h"


static Widget uicreatefsformdialog(void);
static Widget
 uicreatefsfsbox(Widget formwdg,
		  void (*callback) (char *topaddress,
				     HText_t * htext,
				     HTextObject_t * htextobject,
				     void *parameter));
static void uifileselectionokcb(Widget wdg, void *callback,
				 void * calldata);
static void uifileselectioncancelcb(Widget wdg, void *ignored,
				  void * calldata);
static void uifileselectionclickcb(Widget wdg, caddr_t ignored,
				    XmListCallbackStruct * calldata);


//uiTopLevel_t uiTopLevel;


int UiDisplayFileSelection(void (*callback) (char *topaddress, HText_t * htext, HTextObject_t * htextobject,
											 void *parameter))
{
    uiFileSelectionGfx_t *fsgfx = &uiTopLevel.FSGfx;

    if (fsgfx->FormWdg) {
	XtMapWidget(XtParent(fsgfx->FormWdg));

	return UI_OK;
    }
    fsgfx->FormWdg = uicreatefsformdialog();
    fsgfx->FSBoxWdg = uicreatefsfsbox(fsgfx->FormWdg, callback);

    XtManageChild(fsgfx->FormWdg);
    XtRealizeWidget(XtParent(fsgfx->FormWdg));

    return UI_OK;
}


static Widget
 uicreatefsformdialog(void)
{
    ArgList args;
    Cardinal nargs;
    Widget formwdg;
    Widget topwdg;

    topwdg = XtCreateApplicationShell("FileSelection",
				      topLevelShellWidgetClass,
				      NULL, 0);

    args = uiVaSetArgs(&nargs,
		       XmNresizePolicy, XmRESIZE_NONE,
		       XmNautoUnmanage, false, NULL);
    formwdg = XmCreateForm(topwdg, "FileSelection",
			   args, nargs);

    return formwdg;
}


static Widget
 uicreatefsfsbox(Widget formwdg, void (*callback) (char *topaddress, HText_t * htext,
												   HTextObject_t * htextobject, void *parameter))
{
    ArgList args;
    Cardinal nargs;
    Widget fsboxwdg, textwdg, okwdg, cancelwdg;

    args = uiVaSetArgs(&nargs,
		       XmNtopAttachment, XmATTACH_FORM,
		       XmNtopOffset, 1,
		       XmNleftAttachment, XmATTACH_FORM,
		       XmNleftOffset, 1,
		       XmNrightAttachment, XmATTACH_FORM,
		       XmNrightOffset, 1,
		       XmNbottomOffset, 1,
		       XmNbottomAttachment, XmATTACH_FORM, NULL);
    fsboxwdg = XmCreateFileSelectionBox(formwdg, "FSBox",
					args, nargs);
    XtUnmanageChild(XmFileSelectionBoxGetChild(fsboxwdg,
					       XmDIALOG_HELP_BUTTON));

    textwdg = XmFileSelectionBoxGetChild(fsboxwdg, XmDIALOG_TEXT);
    XmTextSetString(textwdg, "");

    okwdg = XmFileSelectionBoxGetChild(fsboxwdg, XmDIALOG_OK_BUTTON);
    XtAddCallback(okwdg, XmNactivateCallback, uifileselectionokcb,
		  (caddr_t) callback);

    cancelwdg = XmFileSelectionBoxGetChild(fsboxwdg, XmDIALOG_CANCEL_BUTTON);
    XtAddCallback(cancelwdg, XmNactivateCallback, uifileselectioncancelcb,
		  (caddr_t) NULL);
    XtManageChild(fsboxwdg);

    return fsboxwdg;
}


static void uifileselectionokcb(Widget wdg, void *callback, void *calldata1)
{
    Widget textwdg;
    char *selection;
	XmPushButtonCallbackStruct *calldata = calldata1;

    textwdg = XmFileSelectionBoxGetChild(uiTopLevel.FSGfx.FSBoxWdg,
					 XmDIALOG_TEXT);
    selection = XmTextGetString(textwdg);
    XtUnmapWidget(XtParent(uiTopLevel.FSGfx.FormWdg));
    (*(void (*) (char *, HText_t *, HTextObject_t *, void *)) callback)
	(uiPageInfo.CurrentPage->Hierarchy->Address,
      uiPageInfo.CurrentPage->HText, uiPageInfo.CurrentPage->HTextObject,
	 (void *) selection);

    XtFree(selection);
}


static void uifileselectioncancelcb(Widget wdg, void *ignored, void *calldata1)
{
    XtUnmapWidget(XtParent(uiTopLevel.FSGfx.FormWdg));
}


static void uifileselectionclickcb(Widget wdg, caddr_t ignored, XmListCallbackStruct *calldata)
{
    Widget textwdg;
    char *selection;

    XmStringGetLtoR(calldata->item, XmSTRING_DEFAULT_CHARSET, &selection);
    textwdg = XmFileSelectionBoxGetChild(uiTopLevel.FSGfx.FSBoxWdg,
					 XmDIALOG_TEXT);
    XmTextSetString(textwdg, selection);
    XtFree(selection);
}
