# TinyWM is written by Nick Welch <mack@incise.org>, 2005.
#
# This software is in the public domain
# and is provided AS IS, with NO WARRANTY.

from Xlib.display import Display
from Xlib import X, XK

dpy = Display()
root = dpy.screen().root

root.grab_key(XK.string_to_keysym("F1"), X.Mod1Mask, 1,
        X.GrabModeAsync, X.GrabModeAsync)
root.grab_button(1, X.Mod1Mask, 1, X.ButtonPressMask,
        X.GrabModeAsync, X.GrabModeAsync, X.NONE, X.NONE)
root.grab_button(3, X.Mod1Mask, 1, X.ButtonPressMask,
        X.GrabModeAsync, X.GrabModeAsync, X.NONE, X.NONE)

while 1:
    ev = root.display.next_event()

    if ev.type == X.KeyPress and ev.child != X.NONE:
        ev.window.circulate(X.RaiseLowest)
    elif ev.type == X.ButtonPress and ev.child != X.NONE:
        ev.child.grab_pointer(1, X.PointerMotionMask|X.ButtonReleaseMask,
                X.GrabModeAsync, X.GrabModeAsync, X.NONE, X.NONE, X.CurrentTime)
        attr = ev.child.get_geometry()
        start = ev
    elif ev.type == X.MotionNotify:
        #while(XCheckTypedEvent(dpy, MotionNotify, &ev));
        xdiff = ev.root_x - start.root_x
        ydiff = ev.root_y - start.root_y
        ev.window.configure(
            x = attr.x + (start.detail == 1 and xdiff or 0),
            y = attr.y + (start.detail == 1 and ydiff or 0),
            width = max(1, attr.width + (start.detail == 3 and xdiff or 0)),
            height = max(1, attr.height + (start.detail == 3 and ydiff or 0)))
    elif ev.type == X.ButtonRelease:
        dpy.ungrab_pointer(X.CurrentTime)

