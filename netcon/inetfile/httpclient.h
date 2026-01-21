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

#pragma once

#include <istream>
#include <memory>
#include <string>

// Include httplib.h before windows.h
#include <httplib.h>
#ifdef WIN32
#include <windows.h>
#endif

namespace D3 {

class HttpClient {
public:
  using Progress = std::function<bool(size_t current, size_t total)>;
  /**
   * Constructor for HttpClient
   * @param URL request URL, should be in form of http://example.com. Don't add "/" on the end as it's part of URIPath.
   * Port can be specified with ":" (http://example.com:8080)
   */
  explicit HttpClient(const std::string &URL);

  httplib::Result Get(const std::string &URIPath);

  /**
   * Get data from URIPath
   * @param URIPath requested path (for example, "/some_dir/my_file.txt")
   * @return standard HTTP code. 200 means is OK.
   */
  httplib::Result Get(const std::string &URIPath, const HttpClient::Progress &progress);

  httplib::Result Get(const std::string &URIPath, const httplib::ContentReceiver &content_receiver);

  httplib::Result Get(const std::string &URIPath, const httplib::ContentReceiver &content_receiver,
                      const HttpClient::Progress &progress);

  void SetProxy(const std::string &proxy_host, uint16_t port);

private:
  std::unique_ptr<httplib::Client> m_client;
};

}
