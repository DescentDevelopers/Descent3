/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <X11/Xlib.h>
#include <X11/extensions/Xext.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/extensions/xf86dga.h>

// interface functions to what we need
extern "C" {
Bool VidModeQueryVersion(Display *dpy, int *majorversion, int *minorversion);
Bool VidModeQueryExtension(Display *dpy, int *event_base, int *error_base);
Bool VidModeGetModeLine(Display *dpy, int screen, int *dotclock, XF86VidModeModeLine *modeline);
Bool VidModeGetAllModeLines(Display *dpy, int screen, int *modecount, XF86VidModeModeInfo ***modelinesPtr);
Bool VidModeAddModeLine(Display *dpy, int screen, XF86VidModeModeInfo *modeline, XF86VidModeModeInfo *aftermodeline);
Bool VidModeDeleteModeLine(Display *dpy, int screen, XF86VidModeModeInfo *modeline);
Bool VidModeModModeLine(Display *dpy, int screen, XF86VidModeModeLine *modeline);
Status VidModeValidateModeLine(Display *dpy, int screen, XF86VidModeModeInfo *modeline);
Bool VidModeSwitchMode(Display *dpy, int screen, int zoom);
Bool VidModeSwitchToMode(Display *dpy, int screen, XF86VidModeModeInfo *modeline);
Bool VidModeLockModeSwitch(Display *dpy, int screen, int lock);
Bool VidModeGetMonitor(Display *dpy, int screen, XF86VidModeMonitor *monitor);
Bool VidModeGetViewPort(Display *dpy, int screen, int *xreturn, int *yreturn);
Bool VidModeSetViewPort(Display *dpy, int screen, int x, int y);
void VidModeFreeData(int num_modes, XF86VidModeModeInfo **modelinePtr);

Bool DGAQueryVersion(Display *pa, int *pb, int *pc);
Bool DGAQueryExtension(Display *pa, int *pb, int *pc);
Status DGAGetVideoLL(Display *pa, int pb, int *pc, int *pd, int *pe, int *pf);
Status DGAGetVideo(Display *pa, int pb, char **pc, int *pd, int *pe, int *pf);
Status DGADirectVideo(Display *pa, int pb, int pc);
Status DGADirectVideoLL(Display *pa, int pb, int pc);
Status DGAGetViewPortSize(Display *pa, int pb, int *pc, int *pd);
Status DGASetViewPort(Display *pa, int pb, int x, int y);
Status DGAGetVidPage(Display *pa, int pb, int *pc);
Status DGASetVidPage(Display *pa, int pb, int pc);
Status DGAInstallColormap(Display *, int pa, Colormap);
int DGAForkApp(int screen);
Status DGAQueryDirectVideo(Display *pa, int pb, int *pc);
Bool DGAViewPortChanged(Display *pa, int pb, int pc);
Bool DGACopyArea(Display *pa, int pb, Drawable pc, GC pd, int pe, int pf, unsigned int pg, unsigned int ph, int pi,
                 int pj);
Bool DGAFillRectangle(Display *pa, int pb, Drawable pc, GC pd, int pe, int pf, unsigned int pg, unsigned int ph);
}

Bool VidModeQueryVersion(Display *dpy, int *majorversion, int *minorversion) {
  return XF86VidModeQueryVersion(dpy, majorversion, minorversion);
}

Bool VidModeQueryExtension(Display *dpy, int *event_base, int *error_base) {
  return XF86VidModeQueryExtension(dpy, event_base, error_base);
}

Bool VidModeGetModeLine(Display *dpy, int screen, int *dotclock, XF86VidModeModeLine *modeline) {
  return XF86VidModeGetModeLine(dpy, screen, dotclock, modeline);
}

Bool VidModeGetAllModeLines(Display *dpy, int screen, int *modecount, XF86VidModeModeInfo ***modelinesPtr) {
  return XF86VidModeGetAllModeLines(dpy, screen, modecount, modelinesPtr);
}

Bool VidModeAddModeLine(Display *dpy, int screen, XF86VidModeModeInfo *modeline, XF86VidModeModeInfo *aftermodeline) {
  return XF86VidModeAddModeLine(dpy, screen, modeline, aftermodeline);
}

Bool VidModeDeleteModeLine(Display *dpy, int screen, XF86VidModeModeInfo *modeline) {
  return XF86VidModeDeleteModeLine(dpy, screen, modeline);
}

Bool VidModeModModeLine(Display *dpy, int screen, XF86VidModeModeLine *modeline) {
  return XF86VidModeModModeLine(dpy, screen, modeline);
}

Status VidModeValidateModeLine(Display *dpy, int screen, XF86VidModeModeInfo *modeline) {
  return XF86VidModeValidateModeLine(dpy, screen, modeline);
}

Bool VidModeSwitchMode(Display *dpy, int screen, int zoom) { return XF86VidModeSwitchMode(dpy, screen, zoom); }

Bool VidModeSwitchToMode(Display *dpy, int screen, XF86VidModeModeInfo *modeline) {
  return XF86VidModeSwitchToMode(dpy, screen, modeline);
}

Bool VidModeLockModeSwitch(Display *dpy, int screen, int lock) { return XF86VidModeLockModeSwitch(dpy, screen, lock); }

Bool VidModeGetMonitor(Display *dpy, int screen, XF86VidModeMonitor *monitor) {
  return XF86VidModeGetMonitor(dpy, screen, monitor);
}

Bool VidModeGetViewPort(Display *dpy, int screen, int *xreturn, int *yreturn) {
  return XF86VidModeGetViewPort(dpy, screen, xreturn, yreturn);
}

Bool VidModeSetViewPort(Display *dpy, int screen, int x, int y) { return XF86VidModeSetViewPort(dpy, screen, x, y); }

void VidModeFreeData(int num_modes, XF86VidModeModeInfo **modelinePtr) {
  int i;
  for (i = 0; i < num_modes; i++) {
    if (modelinePtr[i]->privsize > 0) {
      XFree(modelinePtr[i]->c_private);
      modelinePtr[i]->c_private = NULL;
      modelinePtr[i]->privsize = 0;
    }
  }

  XFree(modelinePtr);
}

Bool DGAQueryVersion(Display *pa, int *pb, int *pc) { return XF86DGAQueryVersion(pa, pb, pc); }

Bool DGAQueryExtension(Display *pa, int *pb, int *pc) { return XF86DGAQueryExtension(pa, pb, pc); }

Status DGAGetVideoLL(Display *pa, int pb, int *pc, int *pd, int *pe, int *pf) {
  return XF86DGAGetVideoLL(pa, pb, (unsigned int *)pc, pd, pe, pf);
}

Status DGAGetVideo(Display *pa, int pb, char **pc, int *pd, int *pe, int *pf) {
  return XF86DGAGetVideo(pa, pb, pc, pd, pe, pf);
}

Status DGADirectVideo(Display *pa, int pb, int pc) { return XF86DGADirectVideo(pa, pb, pc); }

Status DGADirectVideoLL(Display *pa, int pb, int pc) { return XF86DGADirectVideoLL(pa, pb, pc); }

Status DGAGetViewPortSize(Display *pa, int pb, int *pc, int *pd) { return XF86DGAGetViewPortSize(pa, pb, pc, pd); }

Status DGASetViewPort(Display *pa, int pb, int x, int y) { return XF86DGASetViewPort(pa, pb, x, y); }

Status DGAGetVidPage(Display *pa, int pb, int *pc) { return XF86DGAGetVidPage(pa, pb, pc); }

Status DGASetVidPage(Display *pa, int pb, int pc) { return XF86DGASetVidPage(pa, pb, pc); }

Status DGAInstallColormap(Display *pa, int pb, Colormap pc) { return XF86DGAInstallColormap(pa, pb, pc); }

int DGAForkApp(int screen) { return XF86DGAForkApp(screen); }

Status DGAQueryDirectVideo(Display *pa, int pb, int *pc) { return XF86DGAQueryDirectVideo(pa, pb, pc); }

Bool DGAViewPortChanged(Display *pa, int pb, int pc) { return XF86DGAViewPortChanged(pa, pb, pc); }
/*
Bool DGACopyArea(Display *pa,int pb,Drawable pc,GC pd,int pe,int pf,unsigned int pg,unsigned int ph,int pi,int pj)
{
        return XDGACopyArea(pa,pb,pc,pd,pe,pf,pg,ph,pi,pj);
}

Bool DGAFillRectangle(Display *pa,int pb,Drawable pc,GC pd,int pe,int pf,unsigned int pg,unsigned int ph)
{
        return XDGAFillRectangle(pa,pb,pc,pd,pe,pf,pg,ph);
}
*/
