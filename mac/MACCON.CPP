/*
 * $Logfile: /DescentIII/Main/mac/MACCON.CPP $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:14 $
 * $Author: kevinb $
 *
 *
 *
 * $Log: MACCON.CPP,v $
 * Revision 1.1.1.1  2003/08/26 03:58:14  kevinb
 * initial 1.5 import
 *
 *
 * 2     10/21/99 1:55p Kevin
 * Mac Merge!
 *
 * 1     7/28/99 2:31p Kevin
 * Mac only stuff
 *
 *
 * $NoKeywords: $
 */
#include "AppConsole.h"
#include <stdarg.h>
void con_Printf(const char *fmt, ...) {
  char buf[256];
  va_list args;
  //	filter out messages
  va_start(args, fmt);
  vsprintf(buf, fmt, args);
#ifdef FIXED
  int len = strlen(buf);
  if (len >= CON_MAX_STRINGLEN) {
    // we overflowed our buffer!!!
    // we need to do some sort of error here!!!!
    buf[CON_MAX_STRINGLEN - 1] = '\0';
  }
  //	filter out unprintable characters
  char *p, *fp, filter_buf[CON_MAX_STRINGLEN];
  p = buf;
  fp = filter_buf;
  while (*p) {
    if (*p == 0x01) {
      // this is a color, skip the next 3
      p += 4;
    } else {
      if (isalnum(*p) || ispunct(*p) || (*p == ' ') || (*p == '\n') || (*p == '\r') || (*p == '\b')) {
        *fp = *p;
        fp++;
      }
      p++;
    }
  }
  *fp = '\0';
  con_Puts(filter_buf);
  if (hConWnd) {
    InvalidateRect(hConWnd, NULL, TRUE);
    UpdateWindow(hConWnd);
  }
#endif
}
bool con_Input(char *buf, int buflen) {
#ifdef FIXED
  if (Con_read_buf[0]) {
    strncpy(buf, Con_read_buf, buflen - 1);
    buf[buflen - 1] = 0;
    Con_read_buf[0] = 0;
    return true;
  }
#endif
  return false;
}
void con_Defer() {
#ifdef FIXED
  if (Con_newline) {
    Con_newline = false;
    con_Puts("%");
    if (hConWnd) {
      InvalidateRect(hConWnd, NULL, TRUE);
      UpdateWindow(hConWnd);
    }
  }
#endif
}
// console window helper functions
void con_Create(WindowPtr hWnd /*, LPCREATESTRUCT lpcs*/) {
#ifdef FIXED
  oeWin32Application *app = (oeWin32Application *)lpcs->lpCreateParams;
  HDC hdc;
  HFONT oldfont;
  RECT rect;
  TEXTMETRIC tm;
  // create console region
  Con_row = 0;
  Con_col = 0;
  Con_buffer = new char[CON_SCROLL_ROWS * (CON_SCROLL_COLS + 1)];
  Con_read_buf[0] = 0;
  Con_inp_pos = 0;
  memset(Con_inp_buf, 0, sizeof(Con_inp_buf));
  memset(Con_buffer, 0, CON_SCROLL_ROWS * (CON_SCROLL_COLS + 1));
  memset(Con_last_command, 0, sizeof(Con_last_command));
  con_Puts("Outrage PC Console v1.0\n");
  Con_newline = true;
  // get font width and height
  hdc = GetDC(hWnd);
  oldfont = (HFONT)SelectObject(hdc, (HFONT)GetStockObject(ANSI_FIXED_FONT));
  GetTextMetrics(hdc, &tm);
  Con_ch_w = tm.tmMaxCharWidth;
  Con_ch_h = tm.tmHeight;
  rect.left = lpcs->x;
  rect.top = lpcs->y;
  rect.right = rect.left + Con_ch_w * CON_SCROLL_COLS;
  rect.bottom = rect.top + Con_ch_h * CON_SCROLL_ROWS;

  AdjustWindowRect(&rect, lpcs->style, FALSE);
  MoveWindow(hWnd, lpcs->x, lpcs->y, rect.right - rect.left, rect.bottom - rect.top, TRUE);
  app->m_X = lpcs->x;
  app->m_Y = lpcs->y;
  app->m_W = rect.right - rect.left;
  app->m_H = rect.bottom - rect.top;
  SelectObject(hdc, oldfont);
  ReleaseDC(hWnd, hdc);
  EnableMenuItem(GetSystemMenu(hWnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
  hConWnd = hWnd;
#endif
}
void con_Paint(WindowPtr hWnd) {
#ifdef FIXED
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hWnd, &ps);
  int i;
  HFONT hfont;
  // use a fixed font.
  hfont = (HFONT)SelectObject(hdc, (HFONT)GetStockObject(ANSI_FIXED_FONT));
  // draw buffer at 0 to Con_row to end of buffer.
  for (i = 0; i < CON_SCROLL_ROWS; i++) {
    char *ptr = &Con_buffer[i * (CON_SCROLL_COLS + 1)];
    TextOut(hdc, 0, i * Con_ch_h, ptr, strlen(ptr));
  }
  // end
  SelectObject(hdc, hfont);
  EndPaint(hWnd, &ps);
#endif
}
void con_Destroy(WindowPtr hWnd) {
#ifdef FIXED
  if (Con_buffer)
    delete[] Con_buffer;
  hConWnd = NULL;
#endif
}
void con_Puts(const char *str) {
#ifdef FIXED
  int i, len = strlen(str);
  char *row = &Con_buffer[Con_row * (CON_SCROLL_COLS + 1)];
  bool endline = false;
  for (i = 0; i < len; i++) {
    switch (str[i]) {
    case '\n':
      endline = true;
      break;
    case '\b':
      if (Con_col > 0) {
        row[Con_col] = ' ';
        Con_col--;
      }
      break;
    default:
      row[Con_col] = str[i];
      Con_col++;
      break;
    }
    if (Con_col == CON_SCROLL_COLS) {
      endline = true;
    }
    if (endline) {
      Con_row++;
      if (Con_row == CON_SCROLL_ROWS) {
        con_Scroll();
        Con_row--;
      }
      row = &Con_buffer[Con_row * (CON_SCROLL_COLS + 1)];
      memset(row, 0, CON_SCROLL_COLS + 1);
      Con_col = 0;
      endline = false;
    }
  }
  SetCaretPos(Con_ch_w * Con_col, Con_ch_h * Con_row);
#endif
}
void con_Scroll() {
#ifdef FIXED
  int i;
  for (i = 1; i < CON_SCROLL_ROWS; i++)
    memcpy(&Con_buffer[(i - 1) * (CON_SCROLL_COLS + 1)], &Con_buffer[i * (CON_SCROLL_COLS + 1)], CON_SCROLL_COLS + 1);
  memset(&Con_buffer[(i - 1) * (CON_SCROLL_COLS + 1)], 0, CON_SCROLL_COLS + 1);
#endif
}
int con_KeyDown(WindowPtr hWnd, int vkey) {
#ifdef FIXED
  // HDC hdc;
  int x;
  char *ptr = &Con_buffer[Con_row * (CON_SCROLL_COLS + 1)];
  switch (vkey) {
  case VK_LEFT: // Left arrow
    Con_col = max(Con_col - 1, 0);
    Con_inp_pos = max(Con_inp_pos - 1, 0);
    break;
  case VK_RIGHT: // Right arrow
    Con_col = min(Con_col + 1, CON_SCROLL_COLS - 1);
    Con_inp_pos = min(Con_inp_pos + 1, CON_SCROLL_COLS - 1);
    break;
  case VK_UP: // Up arrow
    // Replace the current buffer that is being typed with the last completed command (if there was one)
    if (Con_last_command[0] != 0) {
      memset(Con_inp_buf, 0, sizeof(Con_inp_buf));
      strcpy(Con_inp_buf, Con_last_command);
      ptr[0] = '%';
      strcpy(&ptr[1], Con_last_command); // add for %
      Con_inp_pos = strlen(Con_last_command);
      Con_col = Con_inp_pos + 1; // add for % prompt
      // The application will draw outside the
      // WM_PAINT message processing, so hide the caret.
      HideCaret(hWnd);
      // Redraw the line, adjusted for the
      // deleted character.
      InvalidateRect(hWnd, NULL, TRUE);
      UpdateWindow(hWnd);
      // Display the caret.
      ShowCaret(hWnd);
    }
    break;
  case VK_DELETE: // Delete
                  // Move all the characters that followed the
                  // deleted character (on the same line) one
                  // space back (to the left) in the matrix.
    for (x = Con_inp_pos; x < CON_SCROLL_COLS; x++)
      Con_inp_buf[x] = Con_inp_buf[x + 1];

    for (x = Con_col; x < CON_SCROLL_COLS; x++)
      ptr[x] = ptr[x + 1];
    // The application will draw outside the
    // WM_PAINT message processing, so hide the caret.
    HideCaret(hWnd);
    // Redraw the line, adjusted for the
    // deleted character.
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    // Display the caret.
    ShowCaret(hWnd);
    break;
  }
  // Adjust the caret position based on the
  // virtual-key processing.
  SetCaretPos(Con_ch_w * Con_col, Con_ch_h * Con_row);
#endif
  return 0;
}
int con_Char(WindowPtr hWnd, int vkey) {
#ifdef FIXED
  HDC hdc;
  char *ptr = &Con_buffer[Con_row * (CON_SCROLL_COLS + 1)];
  switch (vkey) {
  case 0x08: // Backspace
             // Move the caret back one space, and then
             // process this like the DEL key.
    if (Con_inp_pos > 0)
      Con_inp_pos--;

    if (Con_col > 0) {
      Con_col--;
      SendMessage(hWnd, WM_KEYDOWN, VK_DELETE, 1L);
    }
    break;
  case 0x0D: // Carriage return
  {
    // Go to the beginning of the next line.
    // The bottom line wraps around to the top.
    strcpy(Con_read_buf, Con_inp_buf);
    Con_col = 0;
    Con_row++;
    if (Con_row == CON_SCROLL_ROWS) {
      con_Scroll();
      Con_row--;
    }
    ptr = &Con_buffer[Con_row * (CON_SCROLL_COLS + 1)];
    memset(ptr, 0, CON_SCROLL_COLS + 1);
    // only save the buffer if there is text in the buffer
    char *p = Con_inp_buf;
    while (*p) {
      if (isalnum(*p)) {
        strcpy(Con_last_command, Con_inp_buf);
        break;
      }
      p++;
    }
    memset(Con_inp_buf, 0, sizeof(Con_inp_buf));
    Con_inp_pos = 0;
    //		con_Printf("Command is %s.\n", Con_read_buf);
    Con_newline = true;
  } break;
  case 0x1B: // Escape
  {
    memset(Con_inp_buf, 0, sizeof(Con_inp_buf));
    memset(ptr, 0, CON_SCROLL_COLS + 1);
    ptr[0] = '%';
    Con_inp_pos = 0;
    Con_col = 1; // for % sign
    // The application will draw outside the
    // WM_PAINT message processing, so hide the caret.
    HideCaret(hWnd);
    // Redraw the line, adjusted for the
    // deleted character.
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);
    // Display the caret.
    ShowCaret(hWnd);
  } break;
  case 0x0A: // Linefeed
    break;
  default:
    if (Con_inp_pos < (CON_SCROLL_COLS - 2)) {
      // Add the character to the text buffer.
      int oldcol = Con_col, oldrow = Con_row;
      char str[2];
      str[0] = (char)vkey;
      str[1] = 0;
      con_Puts(str);
      Con_inp_buf[Con_inp_pos] = (char)vkey;
      // The application will draw outside the
      // WM_PAINT message processing, so hide the caret.
      HideCaret(hWnd);
      // Draw the character on the screen.
      hdc = GetDC(hWnd);
      SelectObject(hdc, GetStockObject(ANSI_FIXED_FONT));
      TextOut(hdc, Con_ch_w * oldcol, Con_ch_h * oldrow, str, 1);
      ReleaseDC(hWnd, hdc);
      // Display the caret.
      ShowCaret(hWnd);
      Con_inp_pos++;
    }
    break;
  }
  SetCaretPos(Con_col * Con_ch_w, Con_row * Con_ch_h);
#endif
  return 0;
}
