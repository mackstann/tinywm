/* TinyWM (C) 2005 Nick Welch
 *
 * Usage of the works is permitted provided that this
 * instrument is retained with the works, so that any entity
 * that uses the works is notified of this instrument.
 *
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 *
 * [2004, Fair License; rhid.com/fair]
 */

/* much of tinywm's purpose is to serve as a very basic example of how to do X
 * stuff and/or understand window managers, so i wanted to put comments in the
 * code explaining things, but i really hate wading through code that is
 * over-commented -- and for that matter, tinywm is supposed to be as concise
 * as possible, so having lots of comments just wasn't really fitting for it.
 * i want tinywm.c to be something you can just look at and go "wow, that's
 * it?  cool!"  so what i did was just copy it over to annotated.c and comment
 * the hell out of it.  ahh, but now i have to make every code change twice!
 * oh well.  i could always use some sort of script to process the comments out
 * of this and write it to tinywm.c ... nah.
 */

/* most X stuff will be included with Xlib.h, but a few things require other
 * headers, like Xmd.h, keysym.h, etc.
 */
#include <X11/Xlib.h>

/* return whichever is smaller */
#define MIN(a, b) ((a) > (b) ? (a) : (b))

/* one annoying thing about Xlib is that function argument lists tend to
 * include everything but the kitchen sink, and a lot of times you don't have a
 * use for every single argument.  so you end up passing a lot of 'dummy' or
 * 'junk' (i.e. temporary, unused) pointers to variables that you'll never
 * inspect after the function call.  that's what these are.
 */
Window w;
int i;         /* <- unused dummies */
unsigned u;

/* these are the three "modes" our little window manager has.  nothing about X
 * or Xlib really mandates this; it's just a simple way of keeping track of
 * what exactly is happening.
 */
enum { NORMAL, MOVING, RESIZING };

int main(void)
{
    Display * dpy = XOpenDisplay(0); /* returns NULL if it can't connect to X */
    Window root;
    KeyCode f1;

    if(!dpy) return 1; /* return failure status if we can't connect */

    /* you'll usually be referencing the root window a lot.  this is a somewhat
     * naive approach that will only work on the default screen.  most people
     * only have one screen, but not everyone.  if you run multi-head without
     * xinerama then you quite possibly have multiple screens. (i'm not sure
     * about vendor-specific implementations, like nvidia's)
     *
     * many, probably most window managers only handle one screen, so in
     * reality this isn't really *that* naive.
     *
     * if you wanted to get the root window of a specific screen you'd use
     * RootWindow(), but the user can also control which screen is our default:
     * if they set $DISPLAY to ":0.foo", then our default screen number is
     * whatever they specify "foo" as.
     */
    root = DefaultRootWindow(dpy);

    /* you could also include keysym.h and use the XK_F1 constant instead of the
     * call to XStringToKeysym, but this method is more "dynamic."  imagine you
     * have config files which specify key bindings.  instead of parsing the key
     * names and having a huge table or whatever to map strings to XK_*
     * constants, you can just take the user-specified string and hand it off
     * to XStringToKeysym.  XStringToKeysym will give you back the appropriate
     * keysym or tell you if it's an invalid key name.
     *
     * a keysym is basically a platform-independent numeric representation of a
     * key, like "F1", "a", "b", "L", "5", "Shift", etc.  a keycode is a
     * numeric representation of a key on the keyboard sent by the keyboard
     * driver (or something along those lines -- i'm no hardware/driver expert)
     * to X.  so we never want to hard-code keycodes, because they can and will
     * differ between systems.
     */
    f1 = XKeysymToKeycode(dpy, XStringToKeysym("F1"));

    /* XGrabKey and XGrabButton are basically ways of saying "when this
     * combination of modifiers and key/button is pressed, send me the events."
     * so we can safely assume that we'll receive Alt+F1 events, Alt+Button1
     * events, and Alt+Button3 events, but no others.  You can either do
     * individual grabs like these for key/mouse combinations, or you can use
     * XSelectInput with KeyPressMask/ButtonPressMask/etc to catch all events
     * of those types and filter them as you receive them.
     */
    XGrabKey(dpy, f1, Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabButton(dpy, 1, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync,
            GrabModeAsync, None, None);
    XGrabButton(dpy, 3, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync,
            GrabModeAsync, None, None);

    for(;;)
    {
        /* these are declared static down here simply because i hate having
         * declarations so far away from where variables actually get used.
         *
         * initialpx and initialpy are where we save the position of our pointer
         * when you first enter moving/resizing mode.
         */
        static int mode = NORMAL, initialpx, initialpy;

        /* we save the window's position and size in this at the same point
         * that we save initialp{x,y}.
         */
        static XWindowAttributes initial;

        /* this is the most basic way of looping through X events; you can be
         * more flexible by using XPending(), or ConnectionNumber() along with
         * select() (or poll() or whatever floats your boat).
         */
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

