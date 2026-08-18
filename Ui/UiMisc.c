static char *rcsid = "$Id: UiMisc.c,v 1.3 1992/03/26 18:13:50 kny Exp kny $";

#include "HTAnchor.h"
#include "HTStyle.h"
#include "../HText/HText.h"
#include "UiIncludes.h"
#include <X11/IntrinsicP.h>
#include <stdarg.h>


static void uitimeouthandler(XtPointer data, XtIntervalId * id);
static void uifdinputhandler(XtPointer data, int *fd, XtInputId * id);
static void uipopupcb(Widget wdg, char *address,
		       XmAnyCallbackStruct * calldata);


void (*uiHelpOnActionCB) (char *actionstring) = NULL;

static void (*uitimeoutcallback) (void *data);
static void (*uifdinputcallback) (void *data);

static XtInputId uiinputid;
static void (*uipopupcallback) (char *address, char *topaddress,
				 char *parentaddress);
static char *uipopuptopaddress;


int UiAttachCallback(const char *actionname,
		     void (*callback) (char *address, HText_t * htext,
				       HTextObject_t * htextobject, void *parameter), void *parameter)
{
    uiAction_t *tmpaction;

    if (!(tmpaction = uiFindAction(actionname))) {
	tmpaction = uiTopLevel.Actions;
	if (!tmpaction)
	    tmpaction =
		uiTopLevel.Actions =
		(uiAction_t *) uiMalloc(sizeof(*tmpaction));
	else {
	    while (tmpaction->Next)
		tmpaction = tmpaction->Next;
	    tmpaction =
		tmpaction->Next = (uiAction_t *) uiMalloc(sizeof(*tmpaction));
	}
	tmpaction->Name = strdup(actionname);
	tmpaction->Next = (uiAction_t *) NULL;
    }
    tmpaction->Callback = callback;
    tmpaction->Parameter = parameter;

    return UI_OK;
}


int UiBindKey(const char *keyname, int modifier,
	      void (*callback) (char *address, HText_t * htext,
				HTextObject_t * htextobject, void *parameter), void *parameter)
{
    uiKey_t *tmpkey;

    if (!(tmpkey = uiFindKey(keyname, modifier))) {
	tmpkey = uiTopLevel.Keys;
	if (!tmpkey)
	    tmpkey =
		uiTopLevel.Keys =
		(uiKey_t *) uiMalloc(sizeof(*tmpkey));
	else {
	    while (tmpkey->Next)
		tmpkey = tmpkey->Next;
	    tmpkey =
		tmpkey->Next = (uiKey_t *) uiMalloc(sizeof(*tmpkey));
	}
	tmpkey->Name = strdup(keyname);
	tmpkey->Next = (uiKey_t *) NULL;
    }
    tmpkey->Modifier = modifier;
    tmpkey->Callback = callback;
    tmpkey->Parameter = parameter;

    return UI_OK;
}


int UiBindVariable(const char *varname, void *variable, uiVarType_t type)
{
    uiVariable_t *tmpvar;

    if (!(tmpvar = uiFindVariable(varname))) {
	tmpvar = uiTopLevel.Variables;
	if (!tmpvar)
	    tmpvar =
		uiTopLevel.Variables =
		(uiVariable_t *) uiMalloc(sizeof(*tmpvar));
	else {
	    while (tmpvar->Next)
		tmpvar = tmpvar->Next;
	    tmpvar =
		tmpvar->Next = (uiVariable_t *) uiMalloc(sizeof(*tmpvar));
	}
	tmpvar->Name = strdup(varname);
	tmpvar->Next = (uiVariable_t *) NULL;
    }
    tmpvar->Value = variable;
    tmpvar->VarType = type;

    return UI_OK;
}


int UiUpdateVariable(const char *varname)
{
    uiVariable_t *tmpvar;
    char *tmpstr;
    static char staticstr[100];	/* Is this overkill? */

    tmpstr = staticstr;
    if ((tmpvar = uiFindVariable(varname))) {
	switch (tmpvar->WdgType) {
	case uiWTtext:
	    switch (tmpvar->VarType) {
	    case uiVTint:
		sprintf(staticstr, "%d", *(int *) tmpvar->Value);
		break;
	    case uiVTdouble:
		sprintf(staticstr, "%d", *(int *) tmpvar->Value);
		break;
	    case uiVTstring:
		tmpstr = (char *) tmpvar->Value;
	    }
	    XmTextSetString(tmpvar->Wdg, tmpstr);
	    break;
	case uiWToptionmenu:
	    /* Not implemented yet */
	    break;
	case uiWTradiobox:
	    /* Not implemented yet */
	    break;
	case uiWTcheckbutton:
	    XtVaSetValues(tmpvar->Wdg,
			  XmNset, (*(int *) tmpvar->Value != 0), NULL);
	    break;
	case uiWTscale:
	    /* Not implemented yet */
	    break;
	}
	return UI_OK;
    }
    return UI_NOTFOUND;
}


void UiGetNextAction(void (*helponactioncb) (char *actionstring))
{
    uiHelpOnActionCB = helponactioncb;
}


uintptr_t UiAddTimeOut(int timeout, void (*callback) (void *data), void *data)
{
    uitimeoutcallback = callback;

    return XtAddTimeOut(timeout, uitimeouthandler, (caddr_t) data);
}


void UiDeleteTimeOut(uintptr_t timeoutid)
{
    XtRemoveTimeOut((XtIntervalId) timeoutid);
}


uintptr_t UiAddInputFD(int fd, void (*callback) (void *data), void *data)
{
    uifdinputcallback = callback;

    return XtAddInput(fd, (XtPointer)(size_t)(XtInputReadMask | XtInputExceptMask),
			    uifdinputhandler, (caddr_t) data);
}


void UiDeleteInputFD(uintptr_t inputid)
{
    XtRemoveInput((XtInputId) inputid);
}


void UiAddStringToCutBuffer(char *data)
{
    static char *cutbuffer = (char *) NULL;
    size_t oldlen;

    if (data) {
	oldlen = cutbuffer ? strlen(cutbuffer) : 0;

	cutbuffer = (char *) uiReAlloc((void *) cutbuffer,
				       oldlen + strlen(data) + 1);
	strcpy(&cutbuffer[oldlen], data);
    } else if (cutbuffer) {
	XStoreBytes(XtDisplay(uiTopLevel.TopGfx.TopWdg), cutbuffer,
		    strlen(cutbuffer));
	uiFree(cutbuffer);
	cutbuffer = (char *) NULL;
    } else
	XStoreBytes(XtDisplay(uiTopLevel.TopGfx.TopWdg), "", 0);
}


void UiDisplayPopup(void (*callback) (char *address, char *topaddress, char *parentaddress), char *topaddress, char **items, int nitems)
{
    Widget topwdg = uiPageInfo.CurrentPage->Gfx.TopWdg;
    ArgList args;
    Cardinal nargs;
    Widget pmwdg, rcwdg, buttonwdg;
    Window root, child;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;

    uipopupcallback = callback;
    uipopuptopaddress = topaddress;

    if (XQueryPointer(XtDisplay(topwdg), XtWindow(topwdg), &root, &child,
		      &root_x, &root_y, &win_x, &win_y, &mask)) {
	args = uiVaSetArgs(&nargs,
			   XmNborderWidth, 0,
			   XmNwidth, 100,
			   XmNheight, 100, NULL);
	pmwdg = XmCreateMenuShell(topwdg, "popupmenu", args, nargs);

	args = uiVaSetArgs(&nargs,
			   XmNx, (Position) (root_x - 20),
			   XmNy, (Position) (root_y - 5),
			   XmNspacing, 0,
			   XmNmarginWidth, 0,
			   XmNmarginHeight, 0, NULL);
	rcwdg = XmCreateRowColumn(pmwdg, "rowcol", args, nargs);
	while (nitems--) {
	    args = uiVaSetArgs(&nargs,
			       XmNhighlightThickness, 0, NULL);
	    buttonwdg = XmCreatePushButtonGadget(rcwdg, items[nitems],
						 args, nargs);
	    XtAddCallback(buttonwdg, XmNactivateCallback,
		    (XtCallbackProc) uipopupcb, (caddr_t) items[nitems]);
	    XtManageChild(buttonwdg);
	}

	XtManageChild(rcwdg);
	XtRealizeWidget(pmwdg);
	XtMapWidget(pmwdg);

	XGrabPointer(XtDisplay(pmwdg), XtWindow(pmwdg), true, 0,
		     GrabModeAsync, GrabModeAsync, XtWindow(pmwdg),
		     uiArrowCursor, CurrentTime);

	XFlush(XtDisplay(pmwdg));
    }
}


uiAction_t *
 uiFindAction(const char *actionname)
{
    uiAction_t *tmpaction = uiTopLevel.Actions;

    while (tmpaction) {
	if (!strcmp(actionname, tmpaction->Name))
	    return tmpaction;

	tmpaction = tmpaction->Next;
    }

    return (uiAction_t *) NULL;
}


uiKey_t *
 uiFindKey(const char *keyname, int modifier)
{
    uiKey_t *tmpkey = uiTopLevel.Keys;

    while (tmpkey) {
	if (!strcmp(keyname, tmpkey->Name) && modifier == tmpkey->Modifier)
	    return tmpkey;

	tmpkey = tmpkey->Next;
    }

    return (uiKey_t *) NULL;
}


uiVariable_t *
 uiFindVariable(const char *varname)
{
    uiVariable_t *tmpvar = uiTopLevel.Variables;

    while (tmpvar) {
	if (!strcmp(varname, tmpvar->Name)) {
	    return tmpvar;
	}

	tmpvar = tmpvar->Next;
    }

    return (uiVariable_t *) NULL;
}


int uiAddWidgetInfo(const char *varname, Widget wdg, uiWdgType_t wdgtype)
{
    uiVariable_t *tmpvar;

    if ((tmpvar = uiFindVariable(varname))) {
	tmpvar->Wdg = wdg;
	tmpvar->WdgType = wdgtype;

	return UI_OK;
    }
    return UI_NOTFOUND;
}


void uiDialogActivateCB(Widget wdg, uiActionData_t *actiondata, XmAnyCallbackStruct *calldata)
{
    uiAction_t *tmpaction;

    actiondata->Page = uiPageInfo.CurrentPage;
    uiPageInfo.Wdg = wdg;
    uiPageInfo.CallData = (void *) calldata;

    if ((tmpaction = uiFindAction(actiondata->ActionName))) {
	uiDefineCursor(uiBusyCursor);
	if (uiHelpOnActionCB) {
	    (*uiHelpOnActionCB) (actiondata->ActionName);
	    uiHelpOnActionCB = (void (*) (char *actionstring)) NULL;
	} else
	    (*tmpaction->Callback) (actiondata->Page->Hierarchy->Address,
				    actiondata->Page->HText,
				    actiondata->Page->HTextObject,
				    tmpaction->Parameter);
	uiUndefineCursor();
    }
}


void uiDialogVariableCB(Widget wdg, caddr_t varname, XmAnyCallbackStruct *calldata)
{
    uiVariable_t *tmpvar;
    char *tmpstr;

    if ((tmpvar = uiFindVariable((char *) varname))) {
	switch (tmpvar->WdgType) {
	case uiWTtext:
	    tmpstr = XmTextGetString(tmpvar->Wdg);
	    switch (tmpvar->VarType) {
	    case uiVTint:
		*(int *) tmpvar->Value = atoi(tmpstr);
		break;
	    case uiVTdouble:
		*(double *) tmpvar->Value = atof(tmpstr);
		break;
	    case uiVTstring:
		strcpy((char *) tmpvar->Value, tmpstr);
	    }
	    XtFree(tmpstr);
	    break;
	case uiWToptionmenu:
	    /* Not implemented yet */
	    break;
	case uiWTradiobox:
	    /* Not implemented yet */
	    break;
	case uiWTcheckbutton:
	    *(uintptr_t *) tmpvar->Value = uiGetArg(tmpvar->Wdg, XmNset);
	    break;
	case uiWTscale:
	    /* Not implemented yet */
	    break;
	}
    }
}


void uiDefineCursor(Cursor cursor)
{
    uiHierarchy_t *tmphierarchy = uiTopLevel.Hierarchies;
    uiPage_t *tmppage;

    while (tmphierarchy) {
	tmppage = tmphierarchy->Pages;
	while (tmppage) {
	    if (tmppage->Visible)
		XDefineCursor(XtDisplay(uiTopLevel.TopGfx.TopWdg),
			      XtWindow(tmppage->Gfx.TopWdg),
			      cursor);
	    tmppage = tmppage->Next;
	}
	tmphierarchy = tmphierarchy->Next;
    }

    XDefineCursor(XtDisplay(uiTopLevel.TopGfx.TopWdg),
		  XtWindow(uiTopLevel.TopGfx.TopWdg),
		  cursor);
    if (uiTopLevel.PageSettingsGfx.FormWdg)
	XDefineCursor(XtDisplay(uiTopLevel.TopGfx.TopWdg),
		      XtWindow(uiTopLevel.PageSettingsGfx.FormWdg),
		      cursor);
    if (uiTopLevel.SearchGfx.FormWdg)
	XDefineCursor(XtDisplay(uiTopLevel.TopGfx.TopWdg),
		      XtWindow(uiTopLevel.SearchGfx.FormWdg),
		      cursor);
    if (uiTopLevel.ListGfx.FormWdg)
	XDefineCursor(XtDisplay(uiTopLevel.TopGfx.TopWdg),
		      XtWindow(uiTopLevel.ListGfx.FormWdg),
		      cursor);
}


void uiUndefineCursor(void)
{
    uiHierarchy_t *tmphierarchy = uiTopLevel.Hierarchies;
    uiPage_t *tmppage;

    while (tmphierarchy) {
	tmppage = tmphierarchy->Pages;
	while (tmppage) {
	    if (tmppage->Visible)
		XUndefineCursor(XtDisplay(uiTopLevel.TopGfx.TopWdg),
				XtWindow(tmppage->Gfx.TopWdg));
	    tmppage = tmppage->Next;
	}
	tmphierarchy = tmphierarchy->Next;
    }

    XUndefineCursor(XtDisplay(uiTopLevel.TopGfx.TopWdg),
		    XtWindow(uiTopLevel.TopGfx.TopWdg));
    if (uiTopLevel.PageSettingsGfx.FormWdg)
	XUndefineCursor(XtDisplay(uiTopLevel.TopGfx.TopWdg),
			XtWindow(uiTopLevel.PageSettingsGfx.FormWdg));
    if (uiTopLevel.SearchGfx.FormWdg)
	XUndefineCursor(XtDisplay(uiTopLevel.TopGfx.TopWdg),
			XtWindow(uiTopLevel.SearchGfx.FormWdg));
    if (uiTopLevel.ListGfx.FormWdg)
	XUndefineCursor(XtDisplay(uiTopLevel.TopGfx.TopWdg),
			XtWindow(uiTopLevel.ListGfx.FormWdg));
}


ArgList
uiVaSetArgs(Cardinal *nargs, ...)
{
    static Arg args[50];
    String tmpstr;
    va_list pvar;

    *nargs = 0;

    va_start(pvar, nargs);
    tmpstr = va_arg(pvar, String);
    while (tmpstr) {
	XtSetArg(args[(int) *nargs], tmpstr, va_arg(pvar, XtArgVal));
	*nargs += 1;
	tmpstr = va_arg(pvar, String);
    }
    va_end(pvar);

    return args;
}


XtArgVal
uiGetArg(Widget wdg, String resource)
{
    Arg args;
    XtArgVal aVal = 0;

    XtSetArg(args, resource, (XtArgVal) &aVal);
    XtGetValues(wdg, &args, 1);

    return aVal;
}


void *
 uiMalloc(size_t size)
{
    void *tmpptr;

    if (!(tmpptr = (void *) malloc(size))) {
	uiDisplayFatal("No swap, buy a computer");
	exit(1);
    }				/* Not reached */
    return tmpptr;
}


void *
 uiReAlloc(void *ptr, size_t size)
{
    void *tmpptr;

    if (!ptr)
	return uiMalloc(size);
    else if (!(tmpptr = (void *) realloc((char *) ptr, size))) {
	uiDisplayFatal("No swap, buy a computer");
	exit(1);
    }				/* Not reached */
    return tmpptr;
}


void uiFree(void *ptr)
{
    if (ptr)
	free(ptr);
}


void uiDisplayWarning(const char *text)
{
    (void) fprintf(stderr, "Ui-warning: %s\n", text);
}


void uiDisplayFatal(const char *text)
{
    (void) fprintf(stderr, "Ui-fatal: %s\n", text);

    abort();
}


void uiWidgetPlacement(Widget wdg, int placement)
{
    Window root, child;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;

    if (placement) {
	if (XQueryPointer(XtDisplay(wdg),
			  XtWindow(wdg), &root, &child,
			  &root_x, &root_y, &win_x, &win_y, &mask))
	    XtMoveWidget(wdg, root_x - 50, root_y - 50);
    }
}


static void uitimeouthandler(XtPointer data, XtIntervalId *id)
{
    (*uitimeoutcallback) (data);
}


static void uifdinputhandler(XtPointer data, int *fd, XtInputId *id)
{
    uiinputid = *id;

    (*uifdinputcallback) (data);
}


static void uipopupcb(Widget wdg, char *address, XmAnyCallbackStruct *calldata)
{
    XtDestroyWidget(XtParent(XtParent(wdg)));

    uipopupcallback(address, uipopuptopaddress, address);
}
