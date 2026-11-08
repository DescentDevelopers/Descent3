/*
 * Descent 3
 * Copyright (C) 2026 Descent Developers
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

#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "lanclient.h"

namespace lanclient_test {

struct SearchCall {
  uint32_t addr; // network byte order, as returned by inet_addr
  uint16_t port; // network byte order
};

struct FakeHost {
  int multi_game_starting = -1;
  int game_is_master_tracker_game = 1;
  int num_network_games_known = 0;
  bool multi_gamelist_changed = false;
  uint8_t newui_alpha = 0;
  char auto_login_addr[100] = "";
  char auto_login_port[100] = "";
  network_game network_games[MAX_NET_GAMES] = {};

  // Games the fake network "discovers" once the game list is polled.
  int games_on_network = 0;

  std::deque<int> poll_results;
  int polls_after_script = 0;
  std::string edit_box_text;
  int selected_index = -1;

  std::map<std::string, std::string> database;

  int join_result = 1;
  int mission_check_result = 1;

  std::vector<network_address> join_attempts;
  std::vector<SearchCall> searches;
  std::vector<std::string> edit_box_set_texts;
  int message_boxes = 0;
};

inline FakeHost *host = nullptr;

// Fills in the auto-login address and port the game would pass with MT_AUTO_LOGIN.
inline void SetAutoLogin(FakeHost &fake, const char *addr, const char *port) {
  strcpy(fake.auto_login_addr, addr);
  strcpy(fake.auto_login_port, port);
}

// Adds a game that the fake network discovers once the game list is polled.
inline void AddNetworkGame(FakeHost &fake, const char *name, const char *ip, uint16_t port) {
  network_game &g = fake.network_games[fake.games_on_network++];
  strcpy(g.name, name);
  uint32_t a = inet_addr(ip);
  memcpy(g.addr.address, &a, sizeof(a));
  g.addr.port = port;
  // Deliberately not TCP: joining from this DLL must always use TCP.
  g.addr.connection_type = NP_IPX;
}

// Returns the recorded searches aimed at a specific address rather than broadcast.
inline std::vector<SearchCall> DirectedSearches(const FakeHost &fake) {
  std::vector<SearchCall> out;
  for (const auto &s : fake.searches)
    if (s.addr != INADDR_BROADCAST)
      out.push_back(s);
  return out;
}

// Returns the IPv4 address of a, in network byte order.
inline uint32_t AddressOf(const network_address &a) {
  uint32_t v;
  memcpy(&v, a.address, sizeof(v));
  return v;
}

// Distinct, non-null handles for UI objects. The DLL only passes them back.
inline char ui_handle_storage[4];
inline UIObject *FakeUIObject() { return reinterpret_cast<UIObject *>(&ui_handle_storage[0]); }
inline UIItem *FakeUIItem() { return reinterpret_cast<UIItem *>(&ui_handle_storage[1]); }

inline int FakeTryToJoinServer(network_address *addr) {
  host->join_attempts.push_back(*addr);
  return host->join_result;
}
inline void FakeMultiStartClient(char *) {}
inline int FakeMsnCheckGetMission(network_address *, char *) { return host->mission_check_result; }
inline bool FakeDoPlayerMouselookCheck(uint32_t) { return true; }

inline int FakeSearchForLocalGamesTCP(uint32_t ask, uint16_t port) {
  if (ask != 0)
    host->searches.push_back({ask, port});
  // Like the real game, replies arriving while polling populate the shared game list.
  if (host->num_network_games_known != host->games_on_network) {
    host->num_network_games_known = host->games_on_network;
    host->multi_gamelist_changed = true;
  }
  return host->num_network_games_known;
}
inline void FakeUpdateAndPackGameList() {}

inline void FakeDoMessageBox(const char *, const char *, int, ddgr_color, ddgr_color) { host->message_boxes++; }

inline int FakePollUI() {
  if (!host->poll_results.empty()) {
    int r = host->poll_results.front();
    host->poll_results.pop_front();
    return r;
  }
  if (++host->polls_after_script > 1000)
    throw std::runtime_error("menu did not exit after UID_CANCEL");
  return UID_CANCEL;
}

inline void FakeEditGetText(void *, char *buff, int len) {
  strncpy(buff, host->edit_box_text.c_str(), len);
  buff[len - 1] = '\0';
}
inline void FakeEditSetText(void *, const char *buff) { host->edit_box_set_texts.emplace_back(buff); }
inline int FakeListGetSelectedIndex(void *) { return host->selected_index; }

inline void FakeDatabaseWrite(const char *label, const char *entry, int entrylen) {
  host->database[label] = std::string(entry, strnlen(entry, entrylen));
}
inline void FakeDatabaseRead(const char *label, char *entry, int *entrylen) {
  auto it = host->database.find(label);
  if (it == host->database.end())
    return;
  int n = std::min<int>(static_cast<int>(it->second.size()), *entrylen - 1);
  memcpy(entry, it->second.data(), n);
  entry[n] = '\0';
  *entrylen = n + 1;
}

inline float FakeTimerGetTime() { return 0.0f; }

inline UIItem *FakeCreateNewUITextItem(const char *, uint32_t, int) { return FakeUIItem(); }
inline void FakeRemoveUITextItem(void *) {}
inline UIObject *FakeNewUIWindowCreate(int, int, int, int, int) { return FakeUIObject(); }
inline void FakeWindowOp(void *) {}
inline void FakeLoadBackgroundImage(void *, const char *) {}
inline UIObject *FakeTextCreate(void *, void *, int, int, int) { return FakeUIObject(); }
inline UIObject *FakeGadgetCreate(void *, int, int, int, int, int, int) { return FakeUIObject(); }
inline UIObject *FakeHotSpotCreate(void *, int, int, void *, void *, int, int, int, int, int) { return FakeUIObject(); }
inline void FakeListAddItem(void *, void *) {}
inline void FakeDeleteUIItem(UIObject *) {}
inline void FakeSetScreenMode(int) {}

inline int msn_frees = 0;
inline void CountingFree(void *p) {
  msn_frees++;
  free(p);
}

// Allocates a mission list item called name and adds it to the DLL's list.
inline msn_list *AddMission(const char *name) {
  auto *m = static_cast<msn_list *>(calloc(1, sizeof(msn_list)));
  strcpy(m->msn_name, name);
  AddMsnItem(m);
  return m;
}

} // namespace lanclient_test
