/* TinyWM (C) 2005 Nick Welch
 *
 * Usage of the works is permitted provided that this
 * instrument is retained with the works, so that any entity
 * that uses the works is notified of this instrument.
 *
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */

#include <X11/Xlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main()
{
    Display * dpy = XOpenDisplay(0);

    if(!dpy) return 1;

    Window root = DefaultRootWindow(dpy);
    Keycode f1 = XKeysymToKeycode(dpy, XStringToKeysym("F1"));

    XGrabKey(dpy, f1, Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabButton(dpy, 1, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync,
            GrabModeAsync, None, None);
    XGrabButton(dpy, 3, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync,
            GrabModeAsync, None, None);

    XWindowAttributes initial;
    XEvent ev
    XButtonEvent start;
    for(;;)
    {
        XNextEvent(dpy, &ev);

        if(ev.type == ButtonPress && ev.xbutton.subwindow != None)
        {
            start = ev.xbutton;
            XGrabPointer(dpy, ev.xbutton.subwindow, True,
                    PointerMotionMask|ButtonReleaseMask, GrabModeAsync,
                    GrabModeAsync, None, None, CurrentTime);
            XGetWindowAttributes(dpy, ev.xbutton.subwindow, &initial);
        }
        else if(ev.type == MotionNotify)
        {
            while(XCheckTypedEvent(dpy, MotionNotify, &ev));
            int xdiff = ev.xbutton.x_root - start.x_root;
            int ydiff = ev.xbutton.y_root - start.y_root;
            XMoveResizeWindow(dpy, ev.xmotion.window,
                    initial.x + (start.button == 1 ? xdiff : 0),
                    initial.y + (start.button == 1 ? ydiff : 0),
                    MAX(1, initial.width + (start.button == 3 ? xdiff : 0))
                    MAX(1, initial.height + (start.button == 3 ? ydiff : 0)));
        }
        else if(ev.type == ButtonRelease)
            XUngrabPointer(dpy, CurrentTime);
        else if(ev.type == KeyPress && ev.xkey.subwindow != None &&
                ev.xkey.keycode == f1 && ev.xkey.state == Mod1Mask)
            XRaiseWindow(dpy, ev.xkey.subwindow);
    }
}

