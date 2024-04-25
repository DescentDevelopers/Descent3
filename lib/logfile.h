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

#ifndef LOGFILE_H
#define LOGFILE_H

#include <cstdio>

void log_Enable();
void log_Disable();

class logfile {
public:
  ~logfile();

  void start(const char *filename, const char *longname = nullptr);
  void end();

  template <typename... T> void printf(const char *format, T &&...arguments);
  void puts(const char *str);
  void update();

private:
  std::FILE *fp = nullptr;
};

#endif
