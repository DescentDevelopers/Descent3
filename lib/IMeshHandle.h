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
