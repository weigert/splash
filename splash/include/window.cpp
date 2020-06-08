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

//Supported Desktop Environments
enum DE{
  NADE,   //Not-A-Desktop-Environment (unrecognized)
  OBX,    //Openbox
  XFCE,   //XFCE
  GNOME,  //Gnome
  I3      //I3
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
    glViewport(0, 0, parse::in.w, parse::in.h);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  }

  Display *Xdisplay;
  Window Xroot, Xwindow;

  GLXWindow gWindow;
  GLXContext gContext;
  GLXFBConfig fbconfig;

  //Positional Data (default)
  int x = 100, y = 100;
  int w = 50, h = 50;

  //Properties
  bool interact = true;
  bool background = false;
  bool all = false;

  void makeWindow(string title);
  void makeContext();

  void desetup();   //Setup specific for the DE
  string getDE(){
    if(const char* env_p = std::getenv("DESKTOP_SESSION"))
      return std::string(env_p);
    else return "NADE";
  }

  bool property(string type, string prop, int set);
  bool property(string type, unsigned long prop, int set);

  ~Splash(){
  	XDestroyWindow(Xdisplay, Xwindow);
  	XCloseDisplay(Xdisplay);
  };
};

void Splash::desetup(){
  string de = getDE();
  logger::write("DE:", de);

  Atom a = XInternAtom(Xdisplay, "_NET_WM_WINDOW_TYPE", 0);
  Atom b = XInternAtom(Xdisplay, "_NET_WM_WINDOW_TYPE_SPLASH", 0);
  XChangeProperty(Xdisplay, Xwindow, a, XA_ATOM, 32,
          PropModeReplace, (unsigned char*)&b, 1);

  //Disable Compositor Shading
  /* example; https://www.systutorials.com/docs/linux/man/1-compton/ */
  a = XInternAtom(Xdisplay, "SPLASH_SHADOW", 0);
  unsigned long i = (parse::in.shade == true)?1:0;
  XChangeProperty(Xdisplay, Xwindow, a, XA_CARDINAL, 32,
          PropModeReplace, (unsigned char*)&i, 1);

  property("_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR", 1);    //Does not appear in Alt+Tab
  property("_NET_WM_STATE", "_NET_WM_STATE_SKIP_PAGER", 1);

  //Set the Class Hint
  XClassHint *class_hints = XAllocClassHint();
  string _name = "SPLASH";
  string _class = "SPLASH";
  class_hints->res_name = (char*)_name.c_str();
  class_hints->res_class = (char*)_class.c_str();
  XSetClassHint(Xdisplay, Xwindow, class_hints);
  XFree(class_hints);
}

/*
    These two can be unified...
    just add the compton flag anyway.
*/


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
  if(type == None)
    return logger::err("Failed to find atom", _type);
  if(prop == None)
    return logger::err("Failed to find atom", _prop);

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
    logger::fatal("Couldn't connect to X server");

	int Xscreen = DefaultScreen(Xdisplay);
	Xroot = RootWindow(Xdisplay, Xscreen);

  const int vdata[] = {
    GLX_RENDER_TYPE, GLX_RGBA_BIT, GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT, GLX_DOUBLEBUFFER, True,
    GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_ALPHA_SIZE, 8, GLX_DEPTH_SIZE, 16,
    None
  };
	fbconfig = 0;
  int nfbcfgs;
  GLXFBConfig *fbconfigs = glXChooseFBConfig(Xdisplay, Xscreen, vdata, &nfbcfgs);
  XVisualInfo* visual;
  XRenderPictFormat * pformat;
	for(int i = 0; i < nfbcfgs; i++) {
		visual = (XVisualInfo*) glXGetVisualFromFBConfig(Xdisplay, fbconfigs[i]);
		if(!visual)
			continue;

		pformat = XRenderFindVisualFormat(Xdisplay, visual->visual);
		if(!pformat)
			continue;

		fbconfig = fbconfigs[i];
		if(pformat->direct.alphaMask > 0) {
			break;
		}
	}

	if(!fbconfig) logger::fatal("No matching FB config found");

  XSetWindowAttributes attr = {0,};
	attr.colormap = XCreateColormap(Xdisplay, Xroot, visual->visual, AllocNone);
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

  int attr_mask = CWBackPixmap | CWColormap | CWBorderPixel | CWEventMask | CWCursor;
	Xwindow = XCreateWindow(Xdisplay, Xroot,
					x, y, w, h, 0,
          visual->depth, InputOutput, visual->visual,
					attr_mask, &attr);
	if( !Xwindow )
    logger::fatal("Couldn't create window");
  gWindow = Xwindow;

  XTextProperty textprop;
	textprop.value = (unsigned char*)t.c_str();
	textprop.encoding = XA_STRING;
	textprop.format = 8;
	textprop.nitems = t.length();

  XSizeHints hints;
  hints.x = x;        //Required for i3!
	hints.y = y;
	hints.width = w;
	hints.height = h;
	hints.flags = USPosition | USSize;

  XWMHints *startup_state = XAllocWMHints();
	startup_state->initial_state = NormalState;
	startup_state->flags = StateHint;
	XSetWMProperties(Xdisplay, Xwindow, &textprop, &textprop,
			NULL, 0, &hints, startup_state, NULL);
	XFree(startup_state);

  desetup();

  if(all){
    Atom a = XInternAtom(Xdisplay, "_NET_WM_STATE", 0);
    Atom b = XInternAtom(Xdisplay, "_NET_WM_STATE_STICKY", 0);
    XChangeProperty(Xdisplay, Xwindow, a, XA_ATOM, 32,
            PropModeReplace, (unsigned char*)&b, 1);
  }

  XMapWindow(Xdisplay, Xwindow);

  //After Mapping
  if(!background) property("_NET_WM_STATE", "_NET_WM_STATE_ABOVE", 1);
  else            property("_NET_WM_STATE", "_NET_WM_STATE_BELOW", 1);

  //Interaction removing needs to happen after mapping
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
    logger::fatal("X server does not support OpenGL");

  gContext = glXCreateNewContext(Xdisplay, fbconfig, GLX_RGBA_TYPE, 0, True);
  if (!gContext)
    logger::fatal("Failed to create OpenGL context");

	if (!glXMakeContextCurrent(Xdisplay, gWindow, gWindow, gContext))
    logger::fatal("Couldn't make context current");
}
