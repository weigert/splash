//Frame Extents
/*
unsigned char *data = 0;
Atom a = XInternAtom(Xdisplay, "_NET_FRAME_EXTENTS", True);
Atom _t;
int f;
unsigned long n, b;
XGetWindowProperty(Xdisplay, Xwindow, a, 0, 4, False, AnyPropertyType,
                 &_t, &f,
                 &n, &b, &data);
long* extents = (long*) data;
XMoveWindow(Xdisplay, Xwindow, -extents[0], -extents[2]);
*/
