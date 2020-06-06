/*
    Event - Handler User Inputs on the Splash
*/

namespace event {

  //Event Data
  bool clicked = false;
  int mx, my = 0;
  int rx, ry = 0;

  //Handle Events
  Handle handle;

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
            clicked = true;
            mx = event.xbutton.x;
            my = event.xbutton.y;
            rx = 0;
            ry = 0;
    		    break;
        case ButtonRelease:
            clicked = false;
            mx = event.xbutton.x;
            my = event.xbutton.y;
            rx = 0;
            ry = 0;
            break;
        case MotionNotify:
            rx = event.xmotion.x - mx;
            ry = event.xmotion.y - my;
            mx = event.xmotion.x;
            my = event.xmotion.y;
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
