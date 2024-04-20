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

#ifndef __IMESH_HANDLE_H__
#define __IMESH_HANDLE_H__

#include <boost/config.hpp>
#include <boost/smart_ptr.hpp>

namespace RZ {
namespace Renderer {
class IMeshHandle {
public:
  virtual ~IMeshHandle() {}
};

typedef boost::shared_ptr<IMeshHandle> MeshHandle;
} // namespace Renderer
} // namespace RZ

#endif
