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

#ifdef DECLARE_POINTERS
#define FEXTERN
#else
#define FEXTERN extern
#endif
#include <pthread.h>

#define pthread_create sopthread_create
#define pthread_exit sopthread_exit
#define pthread_detach sopthread_detach
#define pthread_self sopthread_self

typedef int (*pthread_create_fp)(pthread_t *__thread, __const pthread_attr_t *__attr, void *(*__start_routine)(void *),
                                 void *__arg);
FEXTERN pthread_create_fp sopthread_create;

typedef void (*pthread_exit_fp)(void *__retval);
FEXTERN pthread_exit_fp sopthread_exit;

typedef int (*pthread_detach_fp)(pthread_t __th);
FEXTERN pthread_detach_fp sopthread_detach;

typedef pthread_t (*pthread_self_fp)(void);
FEXTERN pthread_self_fp sopthread_self;

#ifndef DECLARE_POINTERS
bool LoadPThreadLib(bool load = true);
#else
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
void LoadPThreadLibSetNULL(void) {
  sopthread_create = NULL;
  sopthread_exit = NULL;
  sopthread_detach = NULL;
  sopthread_self = NULL;
}

bool LoadPThreadLib(bool load) {
#define PTHREADLIB "libpthread.so"
  static void *handle = NULL;

  if (!load) {
    LoadPThreadLibSetNULL();
    if (handle) {
      dlclose(handle);
      handle = NULL;
      return true;
    }
    fprintf(stderr, "Library Unload Failed: %s\n", PTHREADLIB);
    return false;
  }

  if (handle)
    return true;

  // Load the library
  handle = dlopen(PTHREADLIB, RTLD_LAZY | RTLD_GLOBAL);
  if (!handle) {
    fprintf(stderr, "Library Load Failed: %s\n", PTHREADLIB);
    return false;
  }

  sopthread_create = (pthread_create_fp)dlsym(handle, "pthread_create");
  if (!sopthread_create)
    goto load_error;

  sopthread_exit = (pthread_exit_fp)dlsym(handle, "pthread_exit");
  if (!sopthread_exit)
    goto load_error;

  sopthread_detach = (pthread_detach_fp)dlsym(handle, "pthread_detach");
  if (!sopthread_detach)
    goto load_error;

  sopthread_self = (pthread_self_fp)dlsym(handle, "pthread_self");
  if (!sopthread_self)
    goto load_error;

  return true;

load_error:
  LoadPThreadLibSetNULL();
  fprintf(stderr, "Library Symbol Resolve Error: %s\n", PTHREADLIB);
  dlclose(handle);
  handle = NULL;
  return false;
}
#endif
