/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
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

#include "debug.h"
#include "debug_platform.h"
#include "log.h"

bool Debug_break = false;

bool Debug_Init(bool debugger) {
#ifndef RELEASE
  Debug_break = debugger;
  LOG_DEBUG_IF(Debug_break) << "Debug Break enabled.";
#endif

  return true;
}

static DebugMessageBoxHandlers Message_box_handlers = {nullptr, nullptr};

DebugMessageBoxHandlers Debug_SetMessageBoxHandlers(DebugMessageBoxHandlers handlers) {
  DebugMessageBoxHandlers previous = Message_box_handlers;
  Message_box_handlers = handlers;
  return previous;
}

int Debug_ErrorBox(int type, const char *topstring, const char *title, const char *bottomstring) {
  if (Message_box_handlers.error_box)
    return Message_box_handlers.error_box(type, topstring, title, bottomstring);
  return Debug_PlatformErrorBox(type, topstring, title, bottomstring);
}

int Debug_MessageBox(int type, const char *title, const char *str) {
  if (Message_box_handlers.message_box)
    return Message_box_handlers.message_box(type, title, str);
  return Debug_PlatformMessageBox(type, title, str);
}
