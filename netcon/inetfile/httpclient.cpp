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

#include "httpclient.h"

namespace D3 {

HttpClient::HttpClient(const std::string &URL) {
  m_client = std::make_unique<httplib::Client>(URL);
  m_client->set_follow_location(true);  // Follow redirects
}

int HttpClient::Get(const std::string &URIPath, std::iostream &receiver, httplib::Progress progress) {
  if (auto res = m_client->Get(URIPath, progress)) {
    if (res->status == httplib::StatusCode::OK_200) {
      receiver << res->body;
    }
    return res->status;
  } else {
    // auto err = res.error();
    // std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
    return static_cast<int>(res.error());
  }
}

void HttpClient::SetProxy(const std::string &proxy_host, uint16_t port) {
  m_client->set_proxy(proxy_host, port);
}

}
