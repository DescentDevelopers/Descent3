#include "RendererConfig.h"
#ifdef USE_SOFTWARE_TNL

#if defined(WIN32)
#include <windows.h>
#endif
#include <gl/gl.h>
#include "dyna_gl.h"

#include "IMeshBuilder.h"
#include "pserror.h"
#include <stddef.h>
#include <vector>
#if defined(WIN32)
#include "win/arb_extensions.h"
#endif

#if defined(WIN32)
extern PFNGLACTIVETEXTUREARBPROC oglActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC oglClientActiveTextureARB;
extern PFNGLMULTITEXCOORD4FARBPROC oglMultiTexCoord4f;
#endif


namespace RZ
{
	namespace Renderer
	{
		class OpenGLMeshBuilder : public IMeshBuilder
		{
		public:
			OpenGLMeshBuilder();

			// Begin
			// Begins the generation of a mesh. You must supply
			// the primitive type, size of the vertex buffer, and
			// the number of primitives.
			// vertexFlags are a combination of VertexType values
			// that represent what data should be stored in the verts.
			void Begin( PrimitiveType type, unsigned int vertexFlags, unsigned int numPrims, unsigned int numVerts, bool autogenNormals );

			// End
			// Ends the build process which will cause the generation of the mesh data. 
			// Returned will be a handle to the mesh
			RZ::Renderer::MeshHandle End( void );

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
			void SetStreamData( VertexType streamId, unsigned int startVertIndex, unsigned int numVerts, const float *dataPtr, unsigned int elementsPerField, unsigned int stride );

			// AddPrimitive
			// Adds another primitive to the buffer.
			void AddPrimitive( unsigned int numVerts, unsigned int *vertexIndices );

		private:
			void Reset();

			struct RawVertexData
			{
				float m_position[3];
				float m_color[4];
				float m_uv[2];
				float m_normal[3];
			};
			typedef std::vector<RawVertexData> RawVertexDataVector;
			typedef std::vector<unsigned int> PrimitiveIndexVector;
			typedef std::vector<std::pair<unsigned int, unsigned int>> PrimitiveDefinitionVector;

			RawVertexDataVector       m_rawVertex;
			PrimitiveIndexVector      m_rawIndicies;
			PrimitiveDefinitionVector m_rawPrims;

			PrimitiveType m_primType;
			unsigned int  m_vertexFlags;
			unsigned int  m_numPrims;
			unsigned int  m_numVerts;
			bool          m_autoGenNormals;
			bool          m_isBuilding;
		};

		class OpenGLMesh : public IMeshHandle
		{
		public:
			// execute the drawing process
			void Draw(void);
		private:
			friend class OpenGLMeshBuilder;
			typedef boost::scoped_array<float> ScopedFloatArray;
			typedef boost::scoped_array<unsigned int> ScopedUIntArray;
			ScopedFloatArray m_arrayPositions;
			ScopedFloatArray m_arrayNormals;
			ScopedFloatArray m_arrayColors;
			ScopedFloatArray m_arrayTexture1;
			ScopedUIntArray  m_arrayPrimSizes;
			ScopedUIntArray  m_arrayIndices;
			GLenum           m_mode;
			unsigned int     m_numPrims;
		};
	}
}

using namespace RZ;
using namespace RZ::Renderer;

void OpenGLMesh::Draw(void)
{
	float *ptr;

	// positions
	if( ptr = m_arrayPositions.get() )
	{
		dglEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, ptr );
	}
	else
	{
		dglDisableClientState( GL_VERTEX_ARRAY );
	}

	// colors
	if( ptr = m_arrayColors.get() )
	{
		dglEnableClientState( GL_COLOR_ARRAY );
		dglColorPointer( 4, GL_FLOAT, 0, ptr );
	}
	else
	{
		dglDisableClientState( GL_COLOR_ARRAY );
	}

	// normals
	if( ptr = m_arrayNormals.get() )
	{
		dglEnableClientState( GL_NORMAL_ARRAY );
		dglNormalPointer( 3, GL_FLOAT, 0, ptr );
	}
	else
	{
		dglDisableClientState( GL_NORMAL_ARRAY );
	}

	// texture
	if( ptr = m_arrayTexture1.get() )
	{
		oglClientActiveTextureARB( GL_TEXTURE0_ARB + 0 );
		dglEnableClientState( GL_TEXTURE_COORD_ARRAY );
		dglTexCoordPointer( 4, GL_FLOAT, 0, ptr );
	}
	else
	{
		oglClientActiveTextureARB( GL_TEXTURE0_ARB + 0 );
		dglDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}

	// draw all the primitives
	unsigned int i, indexOffset = 0, *indexList = m_arrayIndices.get();
	for( i = 0; i < m_numPrims; ++i )
	{
		unsigned int numVerts = m_arrayPrimSizes[i];
		dglDrawElements( m_mode, numVerts, GL_UNSIGNED_INT, &indexList[indexOffset] );
		indexOffset += numVerts;
	}
}

OpenGLMeshBuilder::OpenGLMeshBuilder() : m_isBuilding(false)
{
}

void OpenGLMeshBuilder::Reset(void)
{
	m_isBuilding = false;
	m_rawVertex.clear();
	m_rawIndicies.clear();
	m_rawPrims.clear();
}

// Begin
// Begins the generation of a mesh. You must supply
// the primitive type, size of the vertex buffer, and
// the number of primitives.
// vertexFlags are a combination of VertexType values
// that represent what data should be stored in the verts.
void OpenGLMeshBuilder::Begin( PrimitiveType type, unsigned int vertexFlags, unsigned int numPrims, unsigned int numVerts, bool autogenNormals )
{
	ASSERT( m_isBuilding == false );
	ASSERT( vertexFlags & kPosition ); // position must be set
	Reset();

	// initialize
	m_primType       = type;
	m_vertexFlags    = vertexFlags;
	m_numPrims       = numPrims;
	m_numVerts       = numVerts;
	m_autoGenNormals = autogenNormals;
	m_isBuilding     = true;

	// prepare our data
	m_rawVertex.reserve( m_numVerts );
	m_rawVertex.resize( m_numVerts );
	m_rawPrims.reserve( m_numPrims );
	m_rawIndicies.reserve( m_numPrims * 4 ); // rough estimate
}

// End
// Ends the build process which will cause the generation of the mesh data. 
// Returned will be a handle to the mesh
RZ::Renderer::MeshHandle OpenGLMeshBuilder::End( void )
{
	// make sure we are building
	ASSERT( m_isBuilding );

	// TODO: auto-generate normals if we are supposed to

	// create a mesh
	OpenGLMesh *glMesh = new OpenGLMesh;
	MeshHandle meshHandle( glMesh );

	// allocate the data
	if( m_vertexFlags & kPosition )
	{
		glMesh->m_arrayPositions.reset( new float[m_numVerts * 3] );
	}
	if( m_vertexFlags & kNormal )
	{
		glMesh->m_arrayNormals.reset( new float[m_numVerts * 3] );
	}
	if( m_vertexFlags & kColor )
	{
		glMesh->m_arrayColors.reset( new float[m_numVerts * 4] );
	}
	if( m_vertexFlags & kTexture1 )
	{
		glMesh->m_arrayTexture1.reset( new float[m_numVerts * 2] );
	}
	glMesh->m_arrayPrimSizes.reset( new unsigned int[m_numPrims] );
	glMesh->m_arrayIndices.reset( new unsigned int[m_rawIndicies.size()] );

	// initialize the data
	unsigned int i;
	for( i = 0; i < m_numVerts; ++i )
	{
		RawVertexData &rvd = m_rawVertex[ i ];

		if( m_vertexFlags & kPosition )
		{
			memcpy( glMesh->m_arrayPositions.get() + (m_numVerts*3), rvd.m_position, 3 * sizeof(float) );
		}
		if( m_vertexFlags & kNormal )
		{
			memcpy( glMesh->m_arrayNormals.get() + (m_numVerts*3), rvd.m_normal, 3 * sizeof(float) );
		}
		if( m_vertexFlags & kColor )
		{
			memcpy( glMesh->m_arrayColors.get() + (m_numVerts*4), rvd.m_color, 4 * sizeof(float) );
		}
		if( m_vertexFlags & kTexture1 )
		{
			memcpy( glMesh->m_arrayTexture1.get() + (m_numVerts*2), rvd.m_uv, 2 * sizeof(float) );
		}
	}

	for( i = 0; i < m_numPrims; ++i )
	{
		glMesh->m_arrayPrimSizes.get()[i] = m_rawPrims[i].second;
	}
	memcpy( glMesh->m_arrayIndices.get(), &m_rawIndicies[0], m_rawIndicies.size() * sizeof(unsigned int) );

	switch( m_primType )
	{
	case kTriangle:      glMesh->m_mode = GL_TRIANGLES; break;
	case kTriangleFan:   glMesh->m_mode = GL_TRIANGLE_FAN; break;
	case kTriangleStrip: glMesh->m_mode = GL_TRIANGLE_STRIP; break;
	}
	m_numPrims = m_numPrims;

	// we don't need the temporary data anymore
	Reset();

	// return the mesh handle
	return meshHandle;
}

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
void OpenGLMeshBuilder::SetStreamData( VertexType streamId, unsigned int startVertIndex, unsigned int numVerts, const float *dataPtr, unsigned int elementsPerField, unsigned int stride )
{
	// make sure we are building
	ASSERT( m_isBuilding );
	// make sure one, and only one, bit is set in the stream Id
	ASSERT( (streamId!=0) && ((streamId&(streamId-1))==0) );
	// make sure it is expected
	ASSERT( streamId & m_vertexFlags );
	// make sure we don't go beyond our vertex limit
	ASSERT( ((startVertIndex+numVerts) <= m_numVerts) );
	// check input pointers
	ASSERT( dataPtr != NULL );
	ASSERT( stride != 0 );
	unsigned int offset = 0;
	switch( streamId )
	{
	case kPosition: offset = 0; ASSERT( elementsPerField == 3 ); break;
	case kColor:    offset = 3; ASSERT( elementsPerField == 4 ); break;
	case kTexture1: offset = 7; ASSERT( elementsPerField == 2 ); break;
	case kNormal:   offset = 9; ASSERT( elementsPerField == 3 ); break;
	}

	// start looping through and updating the verts
	unsigned int vertIdx;
	for( vertIdx = 0; vertIdx < numVerts; ++vertIdx, dataPtr = (const float*)(((const unsigned char *)dataPtr) + stride) )
	{
		// get to the vert
		RawVertexData &rvd = m_rawVertex[ vertIdx + startVertIndex ];
		float *rawFloat = &rvd.m_position[0];

		// fill in the data
		memcpy( rawFloat, dataPtr, elementsPerField * sizeof(float) );
	}
}

// AddPrimitive
// Adds another primitive to the buffer.
void OpenGLMeshBuilder::AddPrimitive( unsigned int numVerts, unsigned int *vertexIndices )
{
	// make sure we are building
	ASSERT( m_isBuilding );
	// don't blow the buffer
	ASSERT( m_rawPrims.size() < m_numPrims );
	// check input pointers
	ASSERT( vertexIndices != NULL );
	ASSERT( numVerts >= 3 );

	// what is the index offset?
	unsigned int indexOffset = m_rawIndicies.size();

	// add all the vertex indices
	unsigned int i;
	for( i = 0; i < numVerts; ++i )
	{
		unsigned int index = vertexIndices[ i ];
		ASSERT( index < m_numVerts );
		m_rawIndicies.push_back( index );
	}

	// add the primitive
	m_rawPrims.push_back( std::pair<unsigned int, unsigned int>( numVerts, indexOffset ) );
}

RZ::Renderer::IMeshBuilder *rend_CreateMeshBuilder(void)
{
	return new RZ::Renderer::OpenGLMeshBuilder;
}

#endif
