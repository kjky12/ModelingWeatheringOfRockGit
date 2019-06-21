#ifndef _MACRO_H
#define _MACRO_H

//Macro Defines
#define M_DELETE(p)								{ if(p) { delete p;	p = NULL; } }
#define M_A_DELETE(p)							{ if(p) { delete [] p;	p = NULL; } }
#define M_W_DELETE(p)							{ if(p) { p->DestroyWindow(); delete p;	p = NULL; } }
#define M_N_DELETE(p)							{ if(p) { p->DestroyWindow(); p = NULL; } }

//Cursor Destroy
#define M_C_DELETE(h)							{ if(h) { ::DestroyCursor(h); h = NULL; } }

//
#define M_OBJ_DELETE(p)							{ if(p) {p->DeleteObject(); delete p; p = NULL; } }
#define M_GDI_DELETE(p)							{ if(p) { ::DeleteObject(p); p = NULL; } }

//
#define M_FREE(p)					{ if (p) { free(p); p = NULL; } }
#define M_DC_DELETE(p)				{ if (p) { p->DeleteDC(); delete p; p = NULL; } }




#endif