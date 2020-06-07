#include <GL/glew.h>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

using namespace std;

namespace event {
  bool active = true;
}

struct MwmHints {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
};

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

  Splash(){};

  Splash(parse::data in):
  x{in.x}, y{in.y}, w{in.w}, h{in.h},
  interact{in.interact}, background{in.bg},
  all{in.all}
  { setup(); }

  void setup(){
    makeWindow("SPLASH");
    makeContext();

    glewExperimental = GL_TRUE;     //Launch GLEW
    glewInit();

    //Options
    glLineWidth(2.0);
    glViewport(0, 0, 1920, 1080);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  }

  int Xscreen;
  Atom del_atom;
  Colormap cmap;
  Display *Xdisplay;
  XVisualInfo *visual;
  XRenderPictFormat *pict_format;
  GLXFBConfig *fbconfigs, fbconfig;
  GLXContext render_context;
  Window Xroot, Xwindow;
  GLXWindow gWindow;

  //Positional Data (default)
  int x = 100, y = 100;
  int w = 50, h = 50;

  //Properties
  bool interact = true;
  bool background = false;
  bool all = false;

  void makeWindow(string title);
  void makeContext();

  bool property(string type, string prop, int set);
  bool property(string type, unsigned long prop, int set);

  ~Splash(){
  	XDestroyWindow(Xdisplay, Xwindow);
  	XCloseDisplay(Xdisplay);
  };

  template<typename F, typename... Args>
  void render(F function, Args&&... args);
};

bool Splash::property(string type, unsigned long prop, int set){
  Atom _type = XInternAtom(Xdisplay, type.c_str(), 1);
  if(_type == None) return false;
  int attempt = XChangeProperty(Xdisplay, Xwindow, _type, XA_CARDINAL, 32,
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
  xclient.window = Xwindow;
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
    std::cout<<"Couldn't Connect to X-Server"<<std::endl;

	Xscreen = DefaultScreen(Xdisplay);
	Xroot = RootWindow(Xdisplay, Xscreen);

	XEvent event;
	int attr_mask;
	XWMHints *startup_state;
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
    std::cout<<"No Matching FB Config Found"<<std::endl;

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

	Xwindow = XCreateWindow(Xdisplay, Xroot,
					x, y, w, h, 0,
          visual->depth, InputOutput, visual->visual,
					attr_mask, &attr);
	if( !Xwindow )
    std::cout<<"Couldn't create window"<<std::endl;
  gWindow = Xwindow;

  XTextProperty textprop;
	textprop.value = (unsigned char*)t.c_str();
	textprop.encoding = XA_STRING;
	textprop.format = 8;
	textprop.nitems = t.length();

  XSizeHints hints;
	hints.flags = USPosition | USSize;

	startup_state = XAllocWMHints();
	startup_state->initial_state = NormalState;
	startup_state->flags = StateHint;

	XSetWMProperties(Xdisplay, Xwindow, &textprop, &textprop,
			NULL, 0, &hints, startup_state, NULL);

	XFree(startup_state);

  //Set all Desktops Pre-Map
  if(all) property("_NET_WM_DESKTOP", 0xFFFFFFFF, 1);

  Atom a = XInternAtom(Xdisplay, "_NET_WM_WINDOW_TYPE", 0);
  Atom b;

  if(background) b = XInternAtom(Xdisplay, "_NET_WM_WINDOW_TYPE_DESKTOP", 0);
  else b = XInternAtom(Xdisplay, "_NET_WM_WINDOW_TYPE_DOCK", 0);
  XChangeProperty(Xdisplay, Xwindow, a, XA_ATOM, 32,
          PropModeReplace, (unsigned char*)&b, 1);

  XMapWindow(Xdisplay, Xwindow);
  XIfEvent(Xdisplay, &event, WaitForMapNotify, (char*)&Xwindow);
  if ((del_atom = XInternAtom(Xdisplay, "WM_DELETE_WINDOW", 0)) != None) {
    XSetWMProtocols(Xdisplay, Xwindow, &del_atom, 1);
  }

  if(!background)
    property("_NET_WM_STATE", "_NET_WM_STATE_ABOVE", 1);
  else
    property("_NET_WM_STATE", "_NET_WM_STATE_BELOW", 1);
  property("_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR", 1);    //Does not appear in Alt+Tab
  property("_NET_WM_STATE", "_NET_WM_STATE_SKIP_PAGER", 1);

  //No Inputs
  if(!interact){
    XRectangle rect;
    XserverRegion region = XFixesCreateRegion(Xdisplay, &rect, 1);
    XFixesSetWindowShapeRegion(Xdisplay, Xwindow, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(Xdisplay, region);
  }
}

void Splash::makeContext(){
	int dummy;
	if (!glXQueryExtension(Xdisplay, &dummy, &dummy))
		err("OpenGL not supported by X server\n");

  render_context = glXCreateNewContext(Xdisplay, fbconfig, GLX_RGBA_TYPE, 0, True);
  if (!render_context)
    err("Failed to create a GL context\n");

	if (!glXMakeContextCurrent(Xdisplay, gWindow, gWindow, render_context))
		err("glXMakeCurrent failed for window\n");
}

template<typename F, typename... Args>
void Splash::render(F function, Args&&... args){

  while (event::active) {

    function(args...);

  }

}
