/* TinyWM (C) 2005 Nick Welch
 *
 * Usage of the works is permitted provided that this
 * instrument is retained with the works, so that any entity
 * that uses the works is notified of this instrument.
 *
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */

#include <X11/Xlib.h>

#define MIN(a, b) ((a) > (b) ? (a) : (b))

Window w;
int i;         /* <- unused dummies */
unsigned u;

enum { NORMAL, MOVING, RESIZING };

int main(void)
{
    Display * dpy = XOpenDisplay(0);
    Window root;
    KeyCode f1;

    if(!dpy) return 1;

    root = DefaultRootWindow(dpy);
    f1 = XKeysymToKeycode(dpy, XStringToKeysym("F1"));

    XGrabKey(dpy, f1, Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabButton(dpy, 1, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync,
            GrabModeAsync, None, None);
    XGrabButton(dpy, 3, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync,
            GrabModeAsync, None, None);

    for(;;)
    {
        static int mode = NORMAL, initialpx, initialpy;
        static XWindowAttributes initial;
        static XEvent ev;
        XNextEvent(dpy, &ev);

        if(ev.type == ButtonPress && ev.xbutton.subwindow != None)
        {
            mode = (ev.xbutton.button == 1) ? MOVING : RESIZING;
            XGrabPointer(dpy, ev.xbutton.subwindow, True,
                    PointerMotionMask|ButtonReleaseMask, GrabModeAsync,
                    GrabModeAsync, None, None, CurrentTime);
            XQueryPointer(dpy, root, &w, &w, &initialpx, &initialpy, &i, &i,&u);
            XGetWindowAttributes(dpy, ev.xbutton.subwindow, &initial);
        }
        else if(ev.type == MotionNotify)
        {
            while(XCheckTypedEvent(dpy, MotionNotify, &ev));
            if(mode == MOVING)
                XMoveWindow(dpy, ev.xmotion.window,
                        initial.x + ev.xmotion.x_root - initialpx,
                        initial.y + ev.xmotion.y_root - initialpy);
            else /* mode == RESIZING */
                XResizeWindow(dpy, ev.xmotion.window,
                        MIN(1, initial.width + ev.xmotion.x_root - initialpx),
                        MIN(1, initial.height + ev.xmotion.y_root - initialpy));
        }
        else if(ev.type == ButtonRelease)
        {
            mode = NORMAL;
            XUngrabPointer(dpy, CurrentTime);
        }
        else if(ev.type == KeyPress && ev.xkey.subwindow != None &&
                ev.xkey.keycode == f1 && ev.xkey.state == Mod1Mask)
            XRaiseWindow(dpy, ev.xkey.subwindow);
    }
}

