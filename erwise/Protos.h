/* $Id: Protos.h,v 1.1 1992/05/18 21:43:03 tvr Exp $ */

typedef struct HTextObject HTextObject_t;
typedef struct HText HText_t;

/*
 * From other objects/libraries
 */

extern HTParentAnchor *HTMainAnchor;	/* Can you spell kludge? */
extern struct HText *HtLocalText;


/*
 * Main.c
 */

extern FILE *OpenErwiserc(char *type);


/*
 * Setup.c
 */

extern void AttachCallbacks(void);
extern void BindKeys(void);
extern void BindVariables(void);


/*
 * TopLevel.c
 */

extern void TopQuitCB(const char *topaddress, HText_t * htext,
		       HTextObject_t * htextobject, void *parameter);
extern void TopInfoCB(const char *topaddress, HText_t * htext,
		       HTextObject_t * htextobject, void *parameter);
extern void TopOpenCB(const char *topaddress, HText_t * htext,
		       HTextObject_t * htextobject, void *parameter);
extern void TopHelpCB(const char *topaddress, HText_t * htext,
		       HTextObject_t * htextobject, void *parameter);
extern void GetPageCB(const char *nodename);

extern char FindText[256];
extern char SearchText[256];
extern int SearchDepth;
extern int SearchCase;


/*
 * Page.c
 */

extern void PageSearchCB(const char *topaddress, HText_t * htext,
			  HTextObject_t * htextobject, void *parameter);
extern void PageCopyCB(const char *topaddress, HText_t * htext,
		        HTextObject_t * htextobject, void *parameter);
extern void PageListCB(const char *topaddress, HText_t * htext,
		        HTextObject_t * htextobject, void *parameter);
extern void PageLoadToFileCB(const char *topaddress, HText_t * htext,
			   HTextObject_t * htextobject, void *parameter);
extern void PagePrintCB(const char *topaddress, HText_t * htext,
			 HTextObject_t * htextobject, void *parameter);
extern void PageSettingsCB(const char *topaddress, HText_t * htext,
			    HTextObject_t * htextobject, void *parameter);
extern void PageCloseCB(const char *topaddress, HText_t * htext,
			 HTextObject_t * htextobject, void *parameter);
extern void PagePrevWordCB(const char *topaddress, HText_t * htext,
			    HTextObject_t * htextobject, void *parameter);
extern void PageNextWordCB(const char *topaddress, HText_t * htext,
			    HTextObject_t * htextobject, void *parameter);
extern void PagePrevTagCB(const char *topaddress, HText_t * htext,
			   HTextObject_t * htextobject, void *parameter);
extern void PageNextTagCB(const char *topaddress, HText_t * htext,
			   HTextObject_t * htextobject, void *parameter);
extern void PageHomeCB(const char *topaddress, HText_t * htext,
		        HTextObject_t * htextobject, void *parameter);
extern void PageRecallCB(const char *topaddress, HText_t * htext,
			  HTextObject_t * htextobject, void *parameter);
extern void PageBackCB(const char *topaddress, HText_t * htext,
		        HTextObject_t * htextobject, void *parameter);
extern void PagePrevPageCB(const char *topaddress, HText_t * htext,
			    HTextObject_t * htextobject, void *parameter);
extern void PageNextPageCB(const char *topaddress, HText_t * htext,
			    HTextObject_t * htextobject, void *parameter);
extern void PageGetPageCB(const char *topaddress, HText_t * htext,
			   HTextObject_t * htextobject, void *parameter);
extern void PageClickCB(const char *topaddress, HText_t * htext,
			 HTextObject_t * htextobject, void *parameter);
extern void IndexFindCB(const char *topaddress, HText_t * htext,
			 HTextObject_t * htextobject, void *parameter);
extern void HierarchyCloseCB(const char *topaddress, HText_t * htext,
			   HTextObject_t * htextobject, void *parameter);
extern void HierarchyNukeCB(int button);
extern void HierarchyClose(const char *topaddress, HText_t * htext,
			    HTextObject_t * htextobject, void *parameter);
extern void SearchBackwardCB(const char *topaddress, HText_t * htext,
			   HTextObject_t * htextobject, void *parameter);
extern void SearchForwardCB(const char *topaddress, HText_t * htext,
			   HTextObject_t * htextobject, void *parameter);
extern void ConnectionsCB(const char *topaddress, HText_t * htext,
			   HTextObject_t * htextobject, void *parameter);
extern void KillCB(void *connection);
extern void ControlPanelCB(const char *topaddress, HText_t * htext,
			    HTextObject_t * htextobject, void *parameter);
extern void DefaultsCB(const char *topaddress, HText_t * htext,
		        HTextObject_t * htextobject, void *parameter);


/*
 * Help.c
 */

extern void HelpOnFunctionCB(const char *topaddress, HText_t * htext,
			   HTextObject_t * htextobject, void *parameter);
extern void HelpManualCB(const char *topaddress, HText_t * htext,
			  HTextObject_t * htextobject, void *parameter);


/*
 * Print.c
 */

extern int PrintTopMargin;
extern int PrintBottomMargin;
extern int PrintLeftMargin;
extern int PrintWidth;
extern char PrintCommand[1024];
extern int PrintToFile;
extern char PrintFileName[1024];


/*
 * Misc.c
 */

extern Page_t *FindPage(Page_t * hierarchy, const char *address);
extern Page_t *GlobalFindPage(const char *address);
extern Page_t *AddPage(Page_t ** page, const char *address, HText_t * htext,
		        Page_t * toppage);
extern void DeletePage(Page_t ** page, char *address);
extern void DisplayWarning(const char *text);
extern void DisplayFatal(const char *text);
extern int CanBeCursor(HTextObject_t * htextobject);
extern void *Malloc(size_t size);
extern void *ReAlloc(void *ptr, size_t size);
extern void Free(void *ptr);
extern void StartLoading(const char *address, const char *topaddress, const char *parentaddress);
void PollConnection(Connection_t * connection);
Connection_t *AddConnection(const char *address, Page_t * toppage, Page_t * parentpage,
			     ClConnection_t * clconnection);
Connection_t *FindConnection(const char *address);
void DeleteConnection(char *address);

extern Page_t *Pages;
extern Connection_t *Connections;


/*
 * Config.c
 */

extern void ConfigInit(void);
extern int ConfigRestore(FILE * fp);
extern int ConfigSave(FILE * fp);
extern void *ConfigGetValue(void *table, char *id);
extern void *ConfigSetValue(void *table, char *id, void *value);
