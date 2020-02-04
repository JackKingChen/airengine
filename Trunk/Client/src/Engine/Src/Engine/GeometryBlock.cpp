#include "stdafx.h"
#include "GeometryBlock.h"
#include "VClouds.h"
#include "Engine.h"


//************************************
// Method:    GeometryBlock
// Returns:   none
// Qualifier: constructor
// Parameter: VClouds * vc - VClouds pointer
// Parameter: const float & Height - world coordinates
// Parameter: float AlphaRadian - Alpha angle
// Parameter: float BetaRadian - Beta angle
// Parameter: const float & Radius - Total radius
// Parameter: float PhiRadian - Phi Actimutal angle
// Parameter: const int & Na - Number of slices in A zone
// Parameter: const int & Nb - Number of slices in B zone
// Parameter: const int & Nc - Number of slices in C zone
// Parameter: const int & Position - position
//************************************
GeometryBlock::GeometryBlock(
	VClouds *vc,
	float Height, float AlphaRadian, float BetaRadian, 
	float Radius, float PhiRadian, int Na, 
	int Nb, int Nc, int Position)
	: mVClouds(vc)
	, mCreated(false)
	, mVertices(0)
	, mNumberOfTriangles(0)
	, mVertexCount(0)
	, mHeight(Height)
	, mAlpha(AlphaRadian)
	, mBeta(BetaRadian)
	, mRadius(Radius)
	, mPhi(PhiRadian)
	, mNa(Na) , mNb(Nb) , mNc(Nc)
	, mA(0) , mB(0) , mC(0)
	, mPosition(Position)
	, mDisplacement(0,0,0)
	, mWorldOffset(0,0)
	, mRenderObject(NULL)
{
	_calculateDataSize();
}


//************************************
// Method:    ~GeometryBlock
// Returns:   none
// Qualifier: destroctor
//************************************
GeometryBlock::~GeometryBlock()
{
	remove();
}


void GeometryBlock::create()
{
	remove();

	mRenderObject = MANUAL_RENDER_OBJECT_MGR->CreateManualRenderObject( "SkyX_VClouds_Block" );
	IMaterial* pMaterial = MATERIAL_MANAGER->CreateMaterial( "SkyX.xml/SkyX_VolClouds" );
	mRenderObject->SetMaterial( pMaterial );

	// Create mesh geometry
	_createGeometry();

	// Set bounds
	_buildAABox();

	mCreated = true;
	_updateGeometry();
}


void GeometryBlock::remove()
{
	if (!mCreated)
	{
		return;
	}

	MANUAL_RENDER_OBJECT_MGR->RemoveAndDestroyManualRenderObject( "SkyX_VClouds_Block" );
	mRenderObject = NULL;

	//Ogre::MeshManager::getSingleton().remove(mMesh->getName());
	//mVClouds->getSceneManager()->destroyEntity(mEntity);

	//mMesh.setNull();
	//mSubMesh = 0;
	//mEntity = 0;
	//mVertexBuffer.setNull();
	//mIndexBuffer.setNull();
	mVertexBuffer = NULL;
	mIndexBuffer = NULL;

	SafeDeleteArray( mVertices );

	mCreated = false;
}

//************************************
// Method:    update
// Returns:   void
// Qualifier: Call every frame
// Parameter: const float & offset - Amount of world units moved
//************************************
void GeometryBlock::update( float offset)
{
	if (!mCreated)
	{
		return;
	}

	mDisplacement += Vector3Df(offset);

	if (mDisplacement.z < 0 || mDisplacement.z > (mC-mB)/mNc)
	{
		mDisplacement.z -= ((mC-mB)/mNc) * (int)floor((mDisplacement.z)/((mC-mB)/mNc));
	}

	if (mDisplacement.y < 0 || mDisplacement.y > (mB-mA)/mNb)
	{
		mDisplacement.y -= ((mB-mA)/mNb) * (int)floor((mDisplacement.y)/((mB-mA)/mNb));
	}

	if (mDisplacement.x < 0 || mDisplacement.x > mA/mNa)
	{
		mDisplacement.x -= (mA/mNa) * (int)floor((mDisplacement.x)/(mA/mNa));
	}

	if (isInFrustum(mVClouds->getCamera()))
	{
		_updateGeometry();
	}
}



/** Is the geometry block inside the camera frustum?
    @param c Camera
	@return true if yes, false if not
 */
bool GeometryBlock::isInFrustum( CCamera *c ) 
{
	if (!mCreated)
	{
		return false;
	}

	if (!mRenderObject->GetNodeParent())
	{
		return false;
	}

	// TODO: See Ogre::PlaneBoundedVolume (A volume bounded by planes, Ogre::Ray intersection ;) )
	// Se contruye el planebvol y se lanza un rayo con cada esquina del frustum, si intersecta, est?dentro y tiene que ser visible
	// Tambien puede ocurrir que no intersecte porque todo el objeto est?dentro, entonces para ver si est?dentro
	// Frustum::isVisibile(Ogre::Vector3 vertice) con un vertice cualkiera, por ejemplo mVertices[0].xyz ;)
	return c->IsVisable( mRenderObject->GetNodeParent()->GetWorldBoundingBox() );
}


/** Build axis aligned box
 */
AABBBox3Df GeometryBlock::_buildAABox() 
{
	Vector2Df Center = Vector2Df(0,0);
	Vector2Df V1     = Vector2Df( asm_cos(mPhi*mPosition),		asm_sin(mPhi*mPosition)     ) * mRadius;
	Vector2Df V2     = Vector2Df( asm_cos(mPhi*(mPosition+1)),	asm_sin(mPhi*(mPosition+1)) ) * mRadius;

	Vector2Df Max    = Vector2Df( std::max<float>(std::max<float>(V1.x, V2.x), Center.x), std::max<float>(std::max<float>(V1.y, V2.y), Center.y) );
	Vector2Df Min    = Vector2Df( std::min<float>(std::min<float>(V1.x, V2.x), Center.x), std::min<float>(std::min<float>(V1.y, V2.y), Center.y) );

	return AABBBox3Df(
		// Min x,y,z
		Min.x, 0,       Min.y,
		// Max x,y,z
		Max.y, mHeight, Max.y);
}

/** Calculate data size
 */
void GeometryBlock::_calculateDataSize()
{
	mVertexCount = 7*mNa + 6*mNb + 4*mNc;
	mNumberOfTriangles = 5*mNa + 4*mNb + 2*mNc;

	mA = mHeight / asm_cos(HALF_PI - mBeta );
	mB = mHeight / asm_cos(HALF_PI - mAlpha);
	mC = mRadius;

	mV2Cos = Vector2Df(asm_cos(mPosition*mPhi), asm_cos((mPosition+1)*mPhi));
	mV2Sin = Vector2Df(asm_sin(mPosition*mPhi), asm_sin((mPosition+1)*mPhi));

	mBetaSin  = asm_sin( PI - mBeta );
	mAlphaSin = asm_sin( PI - mAlpha);
}

/** Create geometry
 */
void GeometryBlock::_createGeometry()
{
	// Vertex buffers
	//mSubMesh->vertexData = new Ogre::VertexData();
	//mSubMesh->vertexData->vertexStart = 0;
	//mSubMesh->vertexData->vertexCount = mVertexCount;
	//Ogre::VertexDeclaration* vdecl = mSubMesh->vertexData->vertexDeclaration;
	//Ogre::VertexBufferBinding* vbind = mSubMesh->vertexData->vertexBufferBinding;

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();
    /*
	CVertexDeclaration* pVertexDecl = pRenderer->CreateVertexDeclaration();

	size_t offset = 0;
	// Position
	pVertexDecl->AddVertexElement(0, offset, eVET_FLOAT3, eVEU_POSITION,            0);
	offset += sizeof(float) * 3;
	// 3D coords
	pVertexDecl->AddVertexElement(0, offset, eVET_FLOAT3, eVEU_TEXTURE_COORDINATES, 0);
	offset += sizeof(float) * 3;
	// Noise coords
	pVertexDecl->AddVertexElement(0, offset, eVET_FLOAT2, eVEU_TEXTURE_COORDINATES, 1);
	offset += sizeof(float) * 2;
	// Opacity
	pVertexDecl->AddVertexElement(0, offset, eVET_FLOAT1, eVEU_TEXTURE_COORDINATES, 2);
	offset += sizeof(float);
    */

	//mVertexBuffer = pRenderer->CreateVertexBuffer( pVertexDecl, mVertexCount, true );
    mVertexBuffer = pRenderer->CreateVertexBuffer( mVertexCount, 36, true );


	//mVertexBuffer = Ogre::HardwareBufferManager::getSingleton().
	//	createVertexBuffer(sizeof(VERTEX),
	//	mVertexCount,
	//	Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
	//vbind->setBinding(0, mVertexBuffer);

	u32  indexCount = mNumberOfTriangles * 3;
	bool   bUse16BitIndex = ( mVertexCount < 0xffff );
	u16*  _IndexBuffer16 = NULL;
	u32* _IndexBuffer32 = NULL;
	if ( bUse16BitIndex )
	{
		_IndexBuffer16 = new u16[ indexCount ];
	}
	else
	{
		_IndexBuffer32 = new u32[ indexCount ];
	}

	int IndexOffset = 0;
	int VertexOffset = 0;

	// C
	for (int k = 0; k < mNc; k++)
	{
		if ( bUse16BitIndex )
		{
			// First triangle
			_IndexBuffer16[IndexOffset]   = VertexOffset;
			_IndexBuffer16[IndexOffset+1] = VertexOffset+1;
			_IndexBuffer16[IndexOffset+2] = VertexOffset+3;

			// Second triangle
			_IndexBuffer16[IndexOffset+3] = VertexOffset;
			_IndexBuffer16[IndexOffset+4] = VertexOffset+3;
			_IndexBuffer16[IndexOffset+5] = VertexOffset+2;
		}
		else
		{
			// First triangle
			_IndexBuffer32[IndexOffset]   = VertexOffset;
			_IndexBuffer32[IndexOffset+1] = VertexOffset+1;
			_IndexBuffer32[IndexOffset+2] = VertexOffset+3;

			// Second triangle
			_IndexBuffer32[IndexOffset+3] = VertexOffset;
			_IndexBuffer32[IndexOffset+4] = VertexOffset+3;
			_IndexBuffer32[IndexOffset+5] = VertexOffset+2;
		}

	

		IndexOffset  += 6;
		VertexOffset += 4;
	}

	// B
	for (int k = 0; k < mNb; k++)
	{
		if ( bUse16BitIndex )
		{
			// First triangle
			_IndexBuffer16[IndexOffset]   = VertexOffset;
			_IndexBuffer16[IndexOffset+1] = VertexOffset+1;
			_IndexBuffer16[IndexOffset+2] = VertexOffset+3;

			// Second triangle
			_IndexBuffer16[IndexOffset+3] = VertexOffset;
			_IndexBuffer16[IndexOffset+4] = VertexOffset+3;
			_IndexBuffer16[IndexOffset+5] = VertexOffset+2;

			// Third triangle
			_IndexBuffer16[IndexOffset+6] = VertexOffset+2;
			_IndexBuffer16[IndexOffset+7] = VertexOffset+3;
			_IndexBuffer16[IndexOffset+8] = VertexOffset+5;

			// Fourth triangle
			_IndexBuffer16[IndexOffset+9] = VertexOffset+2;
			_IndexBuffer16[IndexOffset+10] = VertexOffset+5;
			_IndexBuffer16[IndexOffset+11] = VertexOffset+4;
		}
		else
		{
			// First triangle
			_IndexBuffer32[IndexOffset]   = VertexOffset;
			_IndexBuffer32[IndexOffset+1] = VertexOffset+1;
			_IndexBuffer32[IndexOffset+2] = VertexOffset+3;

			// Second triangle
			_IndexBuffer32[IndexOffset+3] = VertexOffset;
			_IndexBuffer32[IndexOffset+4] = VertexOffset+3;
			_IndexBuffer32[IndexOffset+5] = VertexOffset+2;

			// Third triangle
			_IndexBuffer32[IndexOffset+6] = VertexOffset+2;
			_IndexBuffer32[IndexOffset+7] = VertexOffset+3;
			_IndexBuffer32[IndexOffset+8] = VertexOffset+5;

			// Fourth triangle
			_IndexBuffer32[IndexOffset+9] = VertexOffset+2;
			_IndexBuffer32[IndexOffset+10] = VertexOffset+5;
			_IndexBuffer32[IndexOffset+11] = VertexOffset+4;
		}


		IndexOffset  += 12;
		VertexOffset += 6;
	}

	// A
	for (int k = 0; k < mNa; k++)
	{
		if ( bUse16BitIndex )
		{
			// First triangle
			_IndexBuffer16[IndexOffset]   = VertexOffset;
			_IndexBuffer16[IndexOffset+1] = VertexOffset+1;
			_IndexBuffer16[IndexOffset+2] = VertexOffset+3;

			// Second triangle
			_IndexBuffer16[IndexOffset+3] = VertexOffset;
			_IndexBuffer16[IndexOffset+4] = VertexOffset+3;
			_IndexBuffer16[IndexOffset+5] = VertexOffset+2;

			// Third triangle
			_IndexBuffer16[IndexOffset+6]   = VertexOffset+2;
			_IndexBuffer16[IndexOffset+7] = VertexOffset+3;
			_IndexBuffer16[IndexOffset+8] = VertexOffset+5;

			// Fourth triangle
			_IndexBuffer16[IndexOffset+9] = VertexOffset+2;
			_IndexBuffer16[IndexOffset+10] = VertexOffset+5;
			_IndexBuffer16[IndexOffset+11] = VertexOffset+4;

			// Fifth triangle
			_IndexBuffer16[IndexOffset+12] = VertexOffset+4;
			_IndexBuffer16[IndexOffset+13] = VertexOffset+5;
			_IndexBuffer16[IndexOffset+14] = VertexOffset+6;
		}
		else
		{
			// First triangle
			_IndexBuffer32[IndexOffset]   = VertexOffset;
			_IndexBuffer32[IndexOffset+1] = VertexOffset+1;
			_IndexBuffer32[IndexOffset+2] = VertexOffset+3;

			// Second triangle
			_IndexBuffer32[IndexOffset+3] = VertexOffset;
			_IndexBuffer32[IndexOffset+4] = VertexOffset+3;
			_IndexBuffer32[IndexOffset+5] = VertexOffset+2;

			// Third triangle
			_IndexBuffer32[IndexOffset+6]   = VertexOffset+2;
			_IndexBuffer32[IndexOffset+7] = VertexOffset+3;
			_IndexBuffer32[IndexOffset+8] = VertexOffset+5;

			// Fourth triangle
			_IndexBuffer32[IndexOffset+9] = VertexOffset+2;
			_IndexBuffer32[IndexOffset+10] = VertexOffset+5;
			_IndexBuffer32[IndexOffset+11] = VertexOffset+4;

			// Fifth triangle
			_IndexBuffer32[IndexOffset+12] = VertexOffset+4;
			_IndexBuffer32[IndexOffset+13] = VertexOffset+5;
			_IndexBuffer32[IndexOffset+14] = VertexOffset+6;
		}


		IndexOffset  += 15;
		VertexOffset += 7;
	}

	//// Prepare buffer for indices
	//mIndexBuffer =
	//	Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
	//	Ogre::HardwareIndexBuffer::IT_32BIT,
	//	mNumberOfTriangles*3,
	//	Ogre::HardwareBuffer::HBU_STATIC, true);

	//mIndexBuffer->
	//	writeData(0,
	//	mIndexBuffer->getSizeInBytes(),
	//	indexbuffer,
	//	true);
	
	if ( bUse16BitIndex )
	{
		mIndexBuffer = pRenderer->CreateIndexBuffer( indexCount );
		mIndexBuffer->WriteData( 0, mIndexBuffer->GetWholeSize(), _IndexBuffer16, true );
		//void* pIndexData = mIndexBuffer->Lock(0,0);
		//memcpy( pIndexData, _IndexBuffer16, sizeof(u16) * indexCount );
		//mIndexBuffer->Unlock();
		SafeDeleteArray( _IndexBuffer16 );
	}
	else
	{
		mIndexBuffer = pRenderer->CreateIndexBuffer( mNumberOfTriangles*3, false, IT_32BIT, false );
		mIndexBuffer->WriteData( 0, mIndexBuffer->GetWholeSize(), _IndexBuffer32, true );
		//void* pIndexData = mIndexBuffer->Lock(0,0);
		//memcpy( pIndexData, _IndexBuffer32, sizeof(u32) * indexCount );
		//mIndexBuffer->Unlock();
		SafeDeleteArray( _IndexBuffer32 );
	}

	//// Set index buffer for this submesh
	//mSubMesh->indexData->indexBuffer = mIndexBuffer;
	//mSubMesh->indexData->indexStart = 0;
	//mSubMesh->indexData->indexCount = mNumberOfTriangles*3;

	// Create our internal buffer for manipulations
	mVertices = new VERTEX[mVertexCount];

	// Update geometry
	_updateGeometry();

}

/** Update geometry
 */
void GeometryBlock::_updateGeometry()
{
	// Update zone C
	for (int k = 0; k < mNc; k++)
	{
		_updateZoneCSlice(k);
	}

	// Update zone B
	for (int k = 0; k < mNb; k++)
	{
		_updateZoneBSlice(k);
	}

	// Update zone A
	for (int k = 0; k < mNa; k++)
	{
		_updateZoneASlice(k);
	}

	// Upload changes
	//mVertexBuffer->
	//	writeData(0,
	//	mVertexBuffer->getSizeInBytes(),
	//	mVertices,
	//	true);
	mVertexBuffer->WriteData( 0, mVertexBuffer->GetWholeSize(), mVertices, true );
}

/** Update zone C slice
    @param n Number of slice
 */
void GeometryBlock::_updateZoneCSlice( int n)
{
	int VertexOffset = n*4;

	// TODO, calculate constants by zone, not by slice
	float Radius = mB+((mC-mB)/mNc)*(mNc-n);

	Radius += mDisplacement.z;

	float opacity = 1;

	if (n == 0)
	{
		opacity = 1 - mDisplacement.z/((mC-mB)/mNc);
	}
	else if (n == mNc-1)
	{
		opacity = mDisplacement.z/((mC-mB)/mNc);
	}

	Vector2Df x1 = mV2Cos * Radius,
		x2 = mV2Cos * Radius * mBetaSin,
		z1 = mV2Sin * Radius,
		z2 = mV2Sin * Radius * mBetaSin;

	Vector3Df or0 = Vector3Df(x1.x, 0, z1.x),	or1 = Vector3Df(x1.y, 0, z1.y);

	float y0 = Radius * asm_sin(mAlpha),
		d = Vector2Df(x1.x - x2.x, z1.x - z2.x).GetLength(),
		ang = asm_arctan(y0/d),
		hip = mHeight / asm_sin(ang);

	// Vertex 0
	_setVertexData(VertexOffset,	Vector3Df(x1.x, 0, z1.x), opacity);
	// Vertex 1
	_setVertexData(VertexOffset+1,	Vector3Df(x1.y, 0, z1.y), opacity);
	// Vertex 2
	_setVertexData(VertexOffset+2,	or0+(Vector3Df(x2.x, y0, z2.x)-or0).Normalize() * hip, opacity);
	// Vertex 3
	_setVertexData(VertexOffset+3,	or1+(Vector3Df(x2.y, y0, z2.y)-or1).Normalize() * hip, opacity);
}

/** Update zone B slice
    @param n Number of slice
 */
void GeometryBlock::_updateZoneBSlice( int n)
{
	int VertexOffset = mNc*4 + n*6;

	// TODO
	float Radius = mA+((mB-mA)/mNb)*(mNb-n);

	Radius += mDisplacement.y;

	float opacity = 1;

	if (n == 0)
	{
		opacity = 1-mDisplacement.y/((mB-mA)/mNb);
	}
	else if (n == mNb-1)
	{
		opacity = mDisplacement.y/((mB-mA)/mNb);
	}

	Vector2Df x1 = mV2Cos * Radius,
		x2 = mV2Cos*Radius*mBetaSin,
		z1 = mV2Sin*Radius,
		z2 = mV2Sin*Radius*mBetaSin;

	float y0 = Radius * asm_sin(mAlpha);

	// Vertex 0
	_setVertexData(VertexOffset,	Vector3Df(x1.x, 0,  z1.x), opacity);
	// Vertex 1
	_setVertexData(VertexOffset + 1,	Vector3Df(x1.y, 0,  z1.y), opacity);
	// Vertex 2
	_setVertexData(VertexOffset + 2,	Vector3Df(x2.x, y0, z2.x), opacity);
	// Vertex 3
	_setVertexData(VertexOffset + 3,	Vector3Df(x2.y, y0, z2.y), opacity);

	Vector2Df x3 = mV2Cos*Radius*mAlphaSin,
		z3 = mV2Sin*Radius*mAlphaSin;

	Vector3Df or0 = Vector3Df(x2.x, y0, z2.x),
		or1 = Vector3Df(x2.y, y0, z2.y);

	float y1 = Radius * asm_sin(mBeta),
		y3 = y1-y0,
		d = Vector2Df(x3.x - x2.x, z3.x - z2.x).GetLength(),
		ang = asm_arctan(y3/d),
		hip = (mHeight-y0) / asm_sin(ang);

	// Vertex 4
	_setVertexData(VertexOffset+4, or0 + (Vector3Df(x3.x, y1, z3.x)-or0).Normalize() * hip, opacity);
	// Vertex 5
	_setVertexData(VertexOffset+5, or1 + (Vector3Df(x3.y, y1, z3.y)-or1).Normalize() * hip, opacity);

}

/** Update zone A slice
    @param n Number of slice
 */
void GeometryBlock::_updateZoneASlice( int n)
{
	int VertexOffset = mNc*4 + mNb*6 +n*7;

	// TODO
	float Radius = (mA/mNa)*(mNa-n);

	Radius += mDisplacement.x;

	float opacity = (n == 0) ? (1-mDisplacement.x/(mA/mNa)) : 1.0f;

	Vector2Df x1 = mV2Cos*Radius,
		x2 = mV2Cos*Radius*mBetaSin,
		z1 = mV2Sin*Radius,
		z2 = mV2Sin*Radius*mBetaSin;

	float y0 = Radius * asm_sin(mAlpha);

	// Vertex 0
	_setVertexData(VertexOffset,   Vector3Df(x1.x, 0, z1.x), opacity);
	// Vertex 1
	_setVertexData(VertexOffset+1, Vector3Df(x1.y, 0, z1.y), opacity);
	// Vertex 2
	_setVertexData(VertexOffset+2, Vector3Df(x2.x, y0, z2.x), opacity);
	// Vertex 3
	_setVertexData(VertexOffset+3, Vector3Df(x2.y, y0, z2.y), opacity);

	Vector2Df x3 = mV2Cos * Radius * mAlphaSin,
		z3 = mV2Sin * Radius * mAlphaSin;

	float y1 = Radius * asm_sin(mBeta);

	// Vertex 4
	_setVertexData(VertexOffset+4, Vector3Df(x3.x, y1, z3.x), opacity);
	// Vertex 5
	_setVertexData(VertexOffset+5, Vector3Df(x3.y, y1, z3.y), opacity);

	// Vertex 6
	_setVertexData(VertexOffset+6, Vector3Df(0, Radius, 0), opacity);

}

/** Set vertex data
	@param index Vertex index
	@param o Slice opacity
	@param p Position
 */
void GeometryBlock::_setVertexData( int index, const Vector3Df& p,  float o)
{
	// Position
	mVertices[index].x = p.x;
	mVertices[index].y = p.y; // - mHeight*0.075*Ogre::Math::Sin(Ogre::Vector2(p.x,p.z).length()/mRadius);
	mVertices[index].z = p.z;

	// 3D coords (Z-UP)
	float scale = mVClouds->getCloudFieldScale()/mRadius;
	mVertices[index].xc = (p.x + mWorldOffset.x)*scale;
	mVertices[index].yc = (p.z + mWorldOffset.y)*scale;
	mVertices[index].zc = Clamp<float>((p.y / mHeight) * 0.5f, 0, 1);

	// Noise coords
	float noise_scale = mVClouds->getNoiseScale() / mRadius; //0.000175f;
	float xz_length_radius = Vector2Df(p.x, p.z).GetLength() / mRadius;
	Vector3Df origin = Vector3Df( 
		0,
		(mRenderObject != 0 && mRenderObject->GetNodeParent() != 0) ? - (mRenderObject->GetNodeParent()->GetDerivedPosition().y - mVClouds->getCamera()->GetPosition().y) - mRadius * ( 0.5f + 0.5f * Vector2Df( p.x, p.z ).GetLength() / mRadius) : -100,
		0 );
	Vector3Df dir = (p-origin).Normalize();
	float hip = asm_sqrt( pow(xz_length_radius * mRadius, 2) + pow(origin.y, 2) );
	Vector3Df uv = dir*hip; // Only x/z, += origin don't needed
	float far_scalemultiplier = 1 - 0.5f * xz_length_radius;
	if (xz_length_radius < 0.01)
		far_scalemultiplier -= 0.25 * 100 * (0.01 - xz_length_radius);
	mVertices[index].u = (uv.x * far_scalemultiplier + mWorldOffset.x) * noise_scale;
	mVertices[index].v = (uv.z * far_scalemultiplier + mWorldOffset.y) * noise_scale;

	// Opacity
	mVertices[index].o = o * mVClouds->getGlobalOpacity();
}