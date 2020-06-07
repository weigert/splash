
namespace event {

  struct eventdata{
    bool clicked = false;
    int mx, my = 0;
    int rx, ry = 0;
  } data;

  //Input Events
  bool input(Splash& s){

    XEvent event;
  	XConfigureEvent *xc;

  	while (XPending(s.Xdisplay)){
  		XNextEvent(s.Xdisplay, &event);
  		switch (event.type){

        /*
    		case ClientMessage:
    			if (event.xclient.data.l[0] == s.del_atom)
    				return false;
            break;
        */

        case ButtonPress:
            data.clicked = true;
            data.mx = event.xbutton.x;
            data.my = event.xbutton.y;
            data.rx = 0;
            data.ry = 0;
    		    break;
        case ButtonRelease:
            data.clicked = false;
            data.mx = event.xbutton.x;
            data.my = event.xbutton.y;
            data.rx = 0;
            data.ry = 0;
            break;
        case MotionNotify:
            data.rx = event.xmotion.x - data.mx;
            data.ry = event.xmotion.y - data.my;
            data.mx = event.xmotion.x;
            data.my = event.xmotion.y;
            break;
    		case ConfigureNotify:
      			xc = &(event.xconfigure);
      			s.w = xc->width;
      			s.h = xc->height;
      			break;
  		}
  	}

    return true;
  }

} //End of Namespace
