
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <iostream>

#include <sys/types.h>
#include <time.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

using namespace std;

struct MwmHints {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
};
enum {
    MWM_HINTS_FUNCTIONS = (1L << 0),
    MWM_HINTS_DECORATIONS =  (1L << 1),

    MWM_FUNC_ALL = (1L << 0),
    MWM_FUNC_RESIZE = (1L << 1),
    MWM_FUNC_MOVE = (1L << 2),
    MWM_FUNC_MINIMIZE = (1L << 3),
    MWM_FUNC_MAXIMIZE = (1L << 4),
    MWM_FUNC_CLOSE = (1L << 5)
};

//#define USE_CHOOSE_FBCONFIG

/*
  Note: A splash is just an X11 window with special properties.

  I couple it with an OpenGl context to allow for nice data rendering.
  That is all.
*/

static void err(const char *_err){
  fprintf(stderr, "%s", _err);
  exit(EXIT_FAILURE);
}

static int VisData[] = {
GLX_RENDER_TYPE, GLX_RGBA_BIT,
GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
GLX_DOUBLEBUFFER, True,
GLX_RED_SIZE, 8,
GLX_GREEN_SIZE, 8,
GLX_BLUE_SIZE, 8,
GLX_ALPHA_SIZE, 8,
GLX_DEPTH_SIZE, 16,
None
};

class Splash{
public:
  int Xscreen;
  Atom del_atom;
  Colormap cmap;
  Display *Xdisplay;
  XVisualInfo *visual;
  XRenderPictFormat *pict_format;
  GLXFBConfig *fbconfigs, fbconfig;
  GLXContext render_context;
  Window Xroot, window_handle;
  GLXWindow glX_window_handle;
  int width, height;

  bool input = false;

  void makeWindow(string title);
  void makeContext();

  bool property(string type, string prop, int set);
  bool property(string type, unsigned long prop, int set);
};

bool Splash::property(string type, unsigned long prop, int set){
  Atom _type = XInternAtom(Xdisplay, type.c_str(), 1);
  if(_type == None) return false;
  int attempt = XChangeProperty(Xdisplay, window_handle, _type, XA_CARDINAL, 32,
                  PropModeReplace, (unsigned char *)&prop, 1);
  return (attempt != 0);
}

static Bool WaitForMapNotify(Display *d, XEvent *e, char *arg){
	return d && e && arg && (e->type == MapNotify) && (e->xmap.window == *(Window*)arg);
}

bool Splash::property(string _type, string _prop, int set){
  Atom type = XInternAtom( Xdisplay, _type.c_str(), 1 );
  Atom prop = XInternAtom( Xdisplay, _prop.c_str(), 1 );
  if(type == None){
    std::cout<<"Failed to find atom "<<_type<<std::endl;
    return false;
  }
  if(prop == None){
    std::cout<<"Failed to find atom "<<_prop<<std::endl;
    return false;
  }

  XClientMessageEvent xclient;
  memset( &xclient, 0, sizeof (xclient) );
  xclient.type = ClientMessage;
  xclient.window = window_handle;
  xclient.message_type = type;
  xclient.format = 32;
  xclient.data.l[0] = set;
  xclient.data.l[1] = prop;
  xclient.data.l[2] = 0;
  xclient.data.l[3] = 0;
  xclient.data.l[4] = 0;
  XSendEvent( Xdisplay, Xroot, False, SubstructureRedirectMask | SubstructureNotifyMask,
    (XEvent *)&xclient );

  return true;
}

void Splash::makeWindow(string t){

  Xdisplay = XOpenDisplay(NULL);
	if (!Xdisplay)
		err("Couldn't connect to X server\n");

	Xscreen = DefaultScreen(Xdisplay);
	Xroot = RootWindow(Xdisplay, Xscreen);

	XEvent event;
	int x,y, attr_mask;
	XSizeHints hints;
	XWMHints *startup_state;
	XTextProperty textprop;
	XSetWindowAttributes attr = {0,};

  int numfbconfigs;
	fbconfigs = glXChooseFBConfig(Xdisplay, Xscreen, VisData, &numfbconfigs);
	fbconfig = 0;
	for(int i = 0; i < numfbconfigs; i++) {
		visual = (XVisualInfo*) glXGetVisualFromFBConfig(Xdisplay, fbconfigs[i]);
		if(!visual)
			continue;

		pict_format = XRenderFindVisualFormat(Xdisplay, visual->visual);
		if(!pict_format)
			continue;

		fbconfig = fbconfigs[i];
		if(pict_format->direct.alphaMask > 0) {
			break;
		}
	}

	if(!fbconfig)
		err("No matching FB config found");

	cmap = XCreateColormap(Xdisplay, Xroot, visual->visual, AllocNone);

	attr.colormap = cmap;
	attr.background_pixmap = None;
	attr.border_pixmap = None;
	attr.border_pixel = 0;
	attr.event_mask =
		StructureNotifyMask |
		EnterWindowMask |
		LeaveWindowMask |
		ExposureMask |
		ButtonPressMask |
		ButtonReleaseMask |
		OwnerGrabButtonMask |
    ButtonMotionMask |
		KeyPressMask |
		KeyReleaseMask;

	attr_mask =
		CWBackPixmap|
		CWColormap|
		CWBorderPixel|
		CWEventMask | CWCursor;

  int swidth =  DisplayWidth(Xdisplay, DefaultScreen(Xdisplay));
  int sheight = DisplayHeight(Xdisplay, DefaultScreen(Xdisplay));

  width = swidth;
  height = sheight;

  x = 0; y = 0;//(swidth-width)/2, y = (sheight-height)/2;

	window_handle = XCreateWindow(	Xdisplay,
					Xroot,
					x, y, width, height,
					0,
					visual->depth,
					InputOutput,
					visual->visual,
					attr_mask, &attr);

	if( !window_handle ) {
		err("Couldn't create the window\n");
	}

  glX_window_handle = window_handle;

  //Set the Window Title
	textprop.value = (unsigned char*)t.c_str();
	textprop.encoding = XA_STRING;
	textprop.format = 8;
	textprop.nitems = t.length();

	hints.x = x;
	hints.y = y;
	hints.width = width;
	hints.height = height;
	hints.flags = PPosition|USSize;

	startup_state = XAllocWMHints();
	startup_state->initial_state = NormalState;
	startup_state->flags = StateHint;

	XSetWMProperties(Xdisplay, window_handle, &textprop, &textprop,
			NULL, 0,
			&hints,
			startup_state,
			NULL);

	XFree(startup_state);

  //Visisble on all desktops
  property("_NET_WM_DESKTOP", 0xFFFFFFFF, 1);

  //True Fullscreen - Call before Mapping
  /*
    Atom wm_state       = XInternAtom (Xdisplay, "_NET_WM_STATE", true );
    Atom wm_fullscreen  = XInternAtom (Xdisplay, "_NET_WM_STATE_FULLSCREEN", true );
    XChangeProperty(Xdisplay, window_handle, wm_state, XA_ATOM, 32,
                    PropModeReplace, (unsigned char *)&wm_fullscreen, 1);
  */

  //Map
  XMapWindow(Xdisplay, window_handle);
  XIfEvent(Xdisplay, &event, WaitForMapNotify, (char*)&window_handle);

  if ((del_atom = XInternAtom(Xdisplay, "WM_DELETE_WINDOW", 0)) != None) {
    XSetWMProtocols(Xdisplay, window_handle, &del_atom, 1);
  }

  //No Border
  Atom mwmHintsProperty = XInternAtom(Xdisplay, "_MOTIF_WM_HINTS", 0);
  struct MwmHints _hints;
  _hints.flags = MWM_HINTS_DECORATIONS;
  _hints.decorations = 0;
  XChangeProperty(Xdisplay, window_handle, mwmHintsProperty, mwmHintsProperty, 32,
          PropModeReplace, (unsigned char *)&hints, 5);

  //After Mapping
  property("_NET_WM_STATE", "_NET_WM_STATE_BELOW", 1);           //_NET_WM_STATE_ABOVE
  property("_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR", 1);    //Does not appear in Alt+Tab
  property("_NET_WM_STATE", "_NET_WM_STATE_SKIP_PAGER", 1);

  //No Inputs
  /*
  XRectangle rect;
  XserverRegion region = XFixesCreateRegion(Xdisplay, &rect, 1);
  XFixesSetWindowShapeRegion(Xdisplay, window_handle, ShapeInput, 0, 0, region);
  XFixesDestroyRegion(Xdisplay, region); */
}

void Splash::makeContext(){
	int dummy;
	if (!glXQueryExtension(Xdisplay, &dummy, &dummy))
		err("OpenGL not supported by X server\n");

  render_context = glXCreateNewContext(Xdisplay, fbconfig, GLX_RGBA_TYPE, 0, True);
  if (!render_context)
    err("Failed to create a GL context\n");

	if (!glXMakeContextCurrent(Xdisplay, glX_window_handle, glX_window_handle, render_context))
		err("glXMakeCurrent failed for window\n");
}
