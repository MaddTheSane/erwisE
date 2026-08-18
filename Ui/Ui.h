/* $Id: Ui.h,v 1.3 1992/04/28 00:34:53 kny Exp kny $ */


#define UI_OK       0
#define UI_ERROR    1
#define UI_NOTFOUND 2

#define UI_NONE  0
#define UI_SHIFT 1
#define UI_CTRL  4


/*
 * Config defines
 */
#define C_GLOBALSETTINGS  "GlobalSettings"

#define C_TOPMARGIN      "topmargin"
#define C_BOTTOMMARGIN   "bottommargin"
#define C_LEFTMARGIN     "leftmargin"
#define C_RIGHTMARGIN    "rightmargin"
#define C_WIDTH          "width"
#define C_HEIGHT         "height"
#define C_ONEPAGEMODE     "onepagemode"
#define C_FIXEDWIDTHMODE  "fixedwidthmode"
#define C_FIXEDWIDTH      "fixedwidth"
#define C_DOUBLECLICKTIME "doubleclicktime"

#define C_SEARCH          "search"
#define C_CONTROLPANEL    "controlpanel"
#define C_LIST            "list"
#define C_RECALL          "recall"
#define C_PAGESETTINGS    "pagesettings"

#define C_DEFAULTS        "Defaults"
#define C_DEFAULTSTABLE   "defaultstable"


typedef enum {
    uiVTint,
    uiVTdouble,
    uiVTstring
} uiVarType_t;

typedef struct HText HText_t;
typedef struct HTextObject HTextObject_t;

extern int UiInitialize(int argc, char *argv[],
			 void *(*configpf) (void *table, char *item),
			 void *(*configsetpf) (void *table, char *item,
					        void *value));
extern void UiMainLoop(void);

extern int UiDisplaySelectionBox(void (*callback) (const char *filename));

extern int UiDisplayPage(const char *address, HText_t * prevhtext, HText_t * htext,
			  HTextObject_t * htextobject, const char *title);
extern int UiDeletePage(const char *address, HText_t * htext);
extern int UiSetCursor(const char *address, HText_t * htext,
		        HTextObject_t * htextobject);

extern int UiDisplaySearchDialog(int type);

extern int UiDisplayPageSettingsDialog(int type);

extern int UiDisplayListDialog(char **listitems, char **addresses, int nitems,
			        void (*callback) (const char *topaddress,
						   const char *address,
						   const char *parentaddress));

extern int UiDisplayRecallDialog(char **listitems, int nitems,
				  void (*callback) (const char *topaddress,
						     const char *address,
						   const char *parentaddress));

extern int UiAttachCallback(const char *actionname,
			    void (*callback) (const char *address,
					      HText_t * htext,
					      HTextObject_t * htextobject,
					      void *parameter),
			    void *parameter);
extern int UiBindKey(const char *keyname, int modifier,
		     void (*callback) (const char *address,
				       HText_t * htext,
				       HTextObject_t * htextobject,
				       void *parameter),
		     void *parameter);
extern int UiBindVariable(const char *varname, void *variable, uiVarType_t type);
extern int UiUpdateVariable(const char *varname);
extern void UiGetNextAction(void (*helponactioncb) (const char *actionstring));
extern uintptr_t UiAddTimeOut(int timeout, void (*callback) (void *data),
			 void *data);
extern void UiDeleteTimeOut(uintptr_t timoutid);
extern uintptr_t UiAddInputFD(int fd, void (*callback) (void *data), void *data);
extern void UiDeleteInputFD(uintptr_t inputid);
extern void UiAddStringToCutBuffer(char *data);
void UiDisplayPopup(void (*callback) (const char *address, const char *topaddress,
				      const char *parentaddress),
		    const char *topaddress, char **items, int nitems);

extern void UiShowInfo(void);

extern int UiDisplayControlPanel(void);

extern int UiDisplayFileSelection(void (*callback) (const char *topaddress,
						     HText_t * htext,
					     HTextObject_t * htextobject,
						     void *parameter));
extern int UiDisplayWarningDialog(const char *warning, void (*callback) (int button));
extern int UiDisplayDefaultsDialog(void);
extern int UiDisplayConnectionsDialog(char **listitems, void **connections, int nitems, void (*callback) (void *connection));
extern int UiDisplayPrintDialog(int type);
extern int UiConnectionsDialogDisplayed(void);



