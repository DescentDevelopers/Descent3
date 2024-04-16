#ifndef __IMESH_BUILDER_H__
#define __IMESH_BUILDER_H__

#include "IMeshHandle.h"

namespace RZ
{
	namespace Renderer
	{
		class IMeshBuilder
		{
		public:
			enum PrimitiveType
			{
				kTriangle,
				kTriangleFan,
				kTriangleStrip,
			};

			enum VertexType
			{
				kPosition = 0x01,
				kColor    = 0x02,
				kTexture1 = 0x04,
				//kTexture2 = 0x08,
				kNormal   = 0x10,
			};

			virtual ~IMeshBuilder(){ }

			// Begin
			// Begins the generation of a mesh. You must supply
			// the primitive type, size of the vertex buffer, and
			// the number of primitives.
			// vertexFlags are a combination of VertexType values
			// that represent what data should be stored in the verts.
			virtual void Begin( PrimitiveType type, unsigned int vertexFlags, unsigned int numPrims, unsigned int numVerts, bool autogenNormals ) = 0;

			// End
			// Ends the build process which will cause the generation of the mesh data. 
			// Returned will be a handle to the mesh
			virtual MeshHandle End( void ) = 0;

			// SetStreamData
			// Stores stream data for one or more vertex fields.
			//	streamId: Which field we are setting data for
			//	startVertIndex: Which vertex in vertex buffer are we starting at
			//	numVerts: How many full verts of data are we processing
			//	dataPtr: Incoming data. All data is float based.
			//	elementsPerField: How many floats-per-field for this data.
			//					  NOTE: This is for verification purposes. Positions and
			//					  Normals are 3 elements. Texture UVs are 2 elements. Colors
			//					  are 4 elements.
			//	stride: The number of bytes from the START of one field to the next on the incoming stream.
			virtual void SetStreamData( VertexType streamId, unsigned int startVertIndex, unsigned int numVerts, const float *dataPtr, unsigned int elementsPerField, unsigned int stride ) = 0;

			// AddPrimitive
			// Adds another primitive to the buffer.
			virtual void AddPrimitive( unsigned int numVerts, unsigned int *vertexIndices ) = 0;
		};
	}
}

#endif
