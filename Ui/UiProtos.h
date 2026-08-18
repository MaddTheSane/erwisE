/* $Id: UiProtos.h,v 1.4 1992/03/26 18:13:50 kny Exp kny $ */


/*
 *  Misc. prototypes (external to library)
 */



/*
 *  UiInit.c
 */

extern uiTopLevel_t uiTopLevel;
extern char **uiSelectionArray;
extern Cursor uiBusyCursor;
extern Cursor uiArrowCursor;
extern void *(*uiConfigPF) (void *table, char *item);
extern void *(*uiConfigSetPF) (void *table, char *item, void *value);


/*
 *  UiControlPanel.c
 */

void uiConnectionsUpdateDialog(void);
void uiPrintUpdateDialog(void);
void uiControlPanelUpdateDialog(void);


/*
 *  UiSelectionBox.c
 */


/*
 *  UiPage.c
 */

extern void uiPageUpdateWindow(uiPage_t * page);
extern void uiPageAttachCallbacks(void);
extern void uiPageDefineKeys(void);

extern uiPageInfo_t uiPageInfo;


/*
 *  UiSearch.c
 */

extern void uiSearchUpdateDialog(void);


/*
 *  UiPageSettings.c
 */

extern void uiPageSettingsUpdateDialog(void);


/*
 *  UiList.c
 */

extern void uiListUpdateDialog(uiPage_t * page);


/*
 *  UiRecall.c
 */

extern void uiRecallUpdateDialog(void);


/*
 *  UiMisc.c
 */

extern uiAction_t *uiFindAction(const char *actionname);
extern uiKey_t *uiFindKey(const char *keyname, int modifier);
extern uiVariable_t *uiFindVariable(const char *varname);
extern int uiAddWidgetInfo(const char *varname, Widget wdg, uiWdgType_t wdgtype);
extern void uiDialogActivateCB(Widget wdg, uiActionData_t * actiondata,
			        XmAnyCallbackStruct * calldata);
extern void uiDialogVariableCB(Widget wdg, caddr_t variablename,
			        XmAnyCallbackStruct * calldata);
extern void uiDefineCursor(Cursor cursor);
extern void uiUndefineCursor(void);
extern ArgList uiVaSetArgs(Cardinal *nargs, ...) __attribute__((sentinel(0,1)));
extern XtArgVal uiGetArg(Widget wdg, String resource);
extern void *uiMalloc(size_t size);
extern void *uiReAlloc(void *, size_t size);
extern void uiFree(void *ptr);
extern void uiDisplayWarning(const char *text);
extern void uiDisplayFatal(const char *text);
extern void uiWidgetPlacement(Widget wdg, int placement);
extern void (*uiHelpOnActionCB) (const char *actionstring);
