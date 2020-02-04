
#include "stdafx.h"
#include "ProgressiveMesh.h"
#include <fstream>

#define PM_DEBUG_MODE 0

#if PM_DEBUG_MODE 
	std::ofstream ofdebug;
#endif


PMTriangle::PMTriangle() : removed(false)
{

}

void			PMTriangle::SetDetails(size_t newindex, PMFaceVertex *v0, PMFaceVertex *v1, PMFaceVertex *v2)
{
	assert(v0!=v1 && v1!=v2 && v2!=v0);

	// 面索引
	index = newindex;
	// 面顶点
	vertex[0] = v0;
	vertex[1] = v1;
	vertex[2] = v2;

	// 计算三角面法线
	ComputeNormal();

	for(int i=0;i<3;i++) 
	{
		// 记录邻面
		vertex[i]->commonVertex->face.insert(this);
		// 记录邻近点
		for(int j=0;j<3;j++) 
		{
			if(i!=j)	
			{
				vertex[i]->commonVertex->neighbor.insert(vertex[j]->commonVertex);
			}
		}
	}
}

void			PMTriangle::ComputeNormal(void)
{
	Vector3Df v0=vertex[0]->commonVertex->position;
	Vector3Df v1=vertex[1]->commonVertex->position;
	Vector3Df v2=vertex[2]->commonVertex->position;
	Vector3Df e1 = v1 - v0; 
	Vector3Df e2 = v2 - v1;
	normal = e1.CrossProduct(e2);
	normal.Normalize();
}

void			PMTriangle::ReplaceVertex(PMFaceVertex *vold, PMFaceVertex *vnew)
{
	assert(vold && vnew);
	assert(vold==vertex[0] || vold==vertex[1] || vold==vertex[2]);
	assert(vnew!=vertex[0] && vnew!=vertex[1] && vnew!=vertex[2]);
	if(vold==vertex[0])
	{
		vertex[0]=vnew;
	}
	else if(vold==vertex[1])
	{
		vertex[1]=vnew;
	}
	else 
	{
		assert(vold==vertex[2]);
		vertex[2]=vnew;
	}
	int i;
	vold->commonVertex->face.erase(this);
	vnew->commonVertex->face.insert(this);
	for(i=0;i<3;i++) 
	{
		vold->commonVertex->RemoveIfNonNeighbor(vertex[i]->commonVertex);
		vertex[i]->commonVertex->RemoveIfNonNeighbor(vold->commonVertex);
	}
	for(i=0;i<3;i++) 
	{
		assert(vertex[i]->commonVertex->face.find(this) != vertex[i]->commonVertex->face.end());
		for(int j=0;j<3;j++) if(i!=j) 
		{
#if PM_DEBUG_MODE 
			ofdebug << "Adding vertex " 
					<< (unsigned int)vertex[j]->commonVertex->index 
					<< " to the neighbor list ""of vertex " 
					<< (unsigned int)vertex[i]->commonVertex->index 
					<< std::endl;
#endif 
			vertex[i]->commonVertex->neighbor.insert(vertex[j]->commonVertex);
		}
	}
	ComputeNormal();
}

bool			PMTriangle::HasCommonVertex(PMVertex *v) const
{
	// 三角面是否含有顶点v
	return (
		v == vertex[0]->commonVertex ||
		v == vertex[1]->commonVertex || 
		v == vertex[2]->commonVertex	);
}

bool			PMTriangle::HasFaceVertex(PMFaceVertex *v) const
{
	// 三角面是否含有面顶点
	return (
		v == vertex[0] ||
		v == vertex[1] || 
		v == vertex[2]	);
}

PMFaceVertex*	PMTriangle::GetFaceVertexFromCommon(PMVertex* commonVert)
{
	// 获得含有顶点commonVert的面顶点
	if (vertex[0]->commonVertex == commonVert) return vertex[0];
	if (vertex[1]->commonVertex == commonVert) return vertex[1];
	if (vertex[2]->commonVertex == commonVert) return vertex[2];
	return NULL;
}

void			PMTriangle::NotifyRemoved(void)
{
	int i;
	for(i=0; i<3; i++) 
	{
		// remove this tri from the vertices
		// 将三角面从顶点的面信息中移除
		if(vertex[i]) 
		{
			vertex[i]->commonVertex->face.erase(this);
		}
	}
	for(i=0; i<3; i++) 
	{
		int i2 = (i+1)%3;
		if(!vertex[i] || !vertex[i2]) continue;
		vertex[i ]->commonVertex->RemoveIfNonNeighbor(vertex[i2]->commonVertex);
		vertex[i2]->commonVertex->RemoveIfNonNeighbor(vertex[i ]->commonVertex);
	}

	removed = true;
}










//------------------------------------------------------------------------------------

PMVertex::PMVertex() : removed(false)
{

}

void PMVertex::SetDetails(const Vector3Df& v, size_t newindex)
{
	position = v;
	index = newindex;
}

void PMVertex::RemoveIfNonNeighbor(PMVertex *n)
{
	NeighborList::iterator i = neighbor.find(n);
	if (i == neighbor.end())
		return; 

	FaceList::iterator f, fend;
	fend = face.end();
	for(f = face.begin(); f != fend; ++f) 
	{
		if((*f)->HasCommonVertex(n)) return; 
	}

#if PM_DEBUG_MODE 
	ofdebug << "Vertex " 
			<< (unsigned int)n->index 
			<< " is no longer a neighbour of vertex " 
			<< (unsigned int)this->index 
			<< " so has been removed from the latter's neighbor list." 
			<< std::endl;
#endif

	neighbor.erase(n);

	if (neighbor.empty() && !toBeRemoved)
	{
		this->NotifyRemoved();
	}
}



bool PMVertex::IsBorder(void)
{
	NeighborList::iterator i, iend;
	iend = neighbor.end();
	for(i = neighbor.begin(); i != iend; ++i) 
	{
		u16 count = 0;
		FaceList::iterator j, jend;
		jend = face.end();
		for(j = face.begin(); j != jend; ++j) 
		{
			if((*j)->HasCommonVertex(*i))
			{
				count ++;
			}
		}
		if(count == 1) 
		{
			return true;
		}
	}
	return false;
}


bool PMVertex::IsManifoldEdgeWith(PMVertex* v)
{
	u16 sidesCount = 0;
	FaceList::iterator i, iend;
	iend = face.end();
	for (i = face.begin(); i != iend; ++i)
	{
		if ((*i)->HasCommonVertex(v))
		{
			sidesCount++;
		}
	}

	return (sidesCount == 1);
}


void PMVertex::NotifyRemoved(void)
{
	NeighborList::iterator i, iend;
	iend = neighbor.end();
	for (i = neighbor.begin(); i != iend; ++i)
	{
		(*i)->neighbor.erase(this);
	}
	removed = true;
	this->collapseTo = NULL;
	this->collapseCost = NEVER_COLLAPSE_COST;
}








//------------------------------------------------------------------------------------

ProgressiveMesh::ProgressiveMesh( CAIRSubMesh* pSrcSubMesh )
{
	m_pOriginSubMesh = pSrcSubMesh;
	AddWorkingData();
	mWorstCosts.resize( m_pOriginSubMesh->GetVerticesNum() );
}

ProgressiveMesh::~ProgressiveMesh( void )
{

}

void ProgressiveMesh::AddWorkingData()
{
	MESH_LOD_LIST& pLodList = m_pOriginSubMesh->GetLodList();

	// 取得网格的顶点总数
	size_t nVertexCount = m_pOriginSubMesh->GetVerticesNum();

	// 创建一个空的工作数据组并放进工作数据组容器
	mWorkingData.push_back(PMWorkingData());

	// 取最新的工作数据组
	PMWorkingData& work = mWorkingData.back();

	// 初始化工作数据组的顶点列表
	work.mFaceVertList.resize( nVertexCount );
	work.mVertList.resize( nVertexCount );

	Vector3Df pos;
	// 保存复制的顶点列表的容器
	typedef std::map<Vector3Df, size_t, vectorLess> CommonVertexMap;
	CommonVertexMap commonVertexMap;
	CommonVertexMap::iterator iCommonVertex;
	size_t numCommon = 0;
	size_t i = 0;

	SOFTWARE_BLEND_VERTEX_LIST& vtSoftwareBlendVertex = (SOFTWARE_BLEND_VERTEX_LIST&)((pLodList[0]->m_pvtSoftwareBlendVertex));//m_pOriginSubMesh->GetSoftwareBlendVertexList();
	HARDWARE_BLEND_VERTEX_LIST& vtHardwareBlendVertex = (HARDWARE_BLEND_VERTEX_LIST&)((pLodList[0]->m_pvtHardwareBlendVertex));//m_pOriginSubMesh->GetHardwareBlendVertexList();
	
	LPMESHVERTNORMAL pMeshNormalVertex = NULL;
	LPHARDWAREMESHVERTEX pHardwareMeshVertex = NULL;

	// 遍历网格的所有顶点
	for ( i = 0; i < nVertexCount; ++i )
	{
		// 获得网格的类型,根据网格类型取得当前顶点的坐标
		switch ( m_pOriginSubMesh->GetAnimationType() )
		{
			// 静态网格
		case eMAT_NoAnimation:
			// 有顶点动画的网格
		case eMAT_VertexAnimation:
			{
				pMeshNormalVertex = vtSoftwareBlendVertex[i];
				pos = pMeshNormalVertex->vertex.vPos;
			}
			break;
			// 有骨骼动画的网格
		case eMAT_SkeletionAnimation:
			{
				// 当前使用硬件蒙皮
				if ( ENGINE_INST->IsHardwareSkeleton() )
				{
					pHardwareMeshVertex = vtHardwareBlendVertex[i];
					pos = pHardwareMeshVertex->vertex.vertex.vPos;
				}
				// 当前使用软件蒙皮
				else
				{
					pMeshNormalVertex = vtSoftwareBlendVertex[i];
					pos = pMeshNormalVertex->vertex.vPos;
				}
			}
			break;
		}

		// 查看在顶点容器中是否已存在该顶点
		iCommonVertex = commonVertexMap.find(pos);

		// 如果容器中没有该顶点
		if (iCommonVertex == commonVertexMap.end())
		{
			// 构建一个新的渐进网格顶点结构体
			PMVertex* commonVert = &(work.mVertList[numCommon]);
			// 设置顶点细节信息
			commonVert->SetDetails(pos, numCommon);
			commonVert->removed = false;
			commonVert->toBeRemoved = false;
			commonVert->seam = false;

			// 将顶点信息放进容器
			commonVertexMap.insert(CommonVertexMap::value_type(pos, numCommon) );

			++numCommon;

			work.mFaceVertList[i].commonVertex = commonVert;
			work.mFaceVertList[i].realIndex = i;
		}
		else
		{
			// 如果已经存在, 取出来
			PMVertex* existingVert = &(work.mVertList[iCommonVertex->second]);
			work.mFaceVertList[i].commonVertex = existingVert;
			work.mFaceVertList[i].realIndex = i;

			// 设置接缝属性为true
			work.mFaceVertList[i].commonVertex->seam = true;
		}
	}


	mNumCommonVertices = numCommon;

	// 获得网格的三角形列表信息
	//FACE_LIST& vtFaces = m_pOriginSubMesh->GetFaceList();
	FACE_LIST& vtFaces = (FACE_LIST&)((pLodList[0]->m_pvtFaces));

	// 计算三角形个数
	size_t numTris = vtFaces.size();
	work.mTriList.resize(numTris); 

	// 遍历所有的三角面
	for (i = 0; i < numTris; ++i)
	{
		// 取得三角形的3个顶点的索引值
		PMFaceVertex *v0, *v1, *v2;
		int* vindex = (int*)(&vtFaces[i]);
		v0 = &(work.mFaceVertList[*vindex++]);
		v1 = &(work.mFaceVertList[*vindex++]);
		v2 = &(work.mFaceVertList[*vindex++]);

		work.mTriList[i].SetDetails(i, v0, v1, v2);

		work.mTriList[i].removed = false;

	}
}

void ProgressiveMesh::InitialiseEdgeCollapseCosts(void)
{
	WorkingDataList::iterator i, iend;
	iend = mWorkingData.end();
	for (i = mWorkingData.begin(); i != iend; ++i)
	{
		CommonVertexList::iterator v, vend;
		vend = i->mVertList.end();
		for (v = i->mVertList.begin(); v != vend; ++v)
		{
			v->collapseTo = NULL;
			v->collapseCost = NEVER_COLLAPSE_COST;
		}
	}
}


float ProgressiveMesh::ComputeEdgeCollapseCost(PMVertex *src, PMVertex *dest)
{
	Vector3Df edgeVector = src->position - dest->position;

	float cost;
	float curvature = 0.001f;

	FaceList sides;
	FaceList::iterator srcface, srcfaceEnd;
	srcfaceEnd = src->face.end();
	// 遍历源点src的所有邻接面
	for(srcface = src->face.begin(); srcface != srcfaceEnd; ++srcface)
	{
		// 把含有顶点dest的邻接面记录下来
		if( (*srcface)->HasCommonVertex(dest) )
		{
			sides.insert(*srcface);
		}
	}

	// 检查顶点src是否网格边界上的顶点
	if(src->IsBorder())
	{
		// 如果src是边界点并且与dest组成的边邻接2个以上的三角形
		if (sides.size() > 1) 
		{
			cost = 1.0f;
		}
		// 如果src是边界点并且与dest组成的边邻接1个三角形
		else
		{
			Vector3Df collapseEdge, otherBorderEdge;
			float kinkiness, maxKinkiness;
			NeighborList::iterator n, nend;
			nend = src->neighbor.end();
			maxKinkiness = 0.0f;
			edgeVector.Normalize();
			collapseEdge = edgeVector;
			// 遍历src的所有邻接顶点
			for (n = src->neighbor.begin(); n != nend; ++n)
			{
				// 如果邻接点不是dest并且src与邻接点组成的边只有1个邻接三角形
				if (*n != dest && (*n)->IsManifoldEdgeWith(src))
				{
					otherBorderEdge = src->position - (*n)->position;
					otherBorderEdge.Normalize();

					// 计算源边(src->dest)与边(src->n)的相似度
					kinkiness = (otherBorderEdge.DotProduct(collapseEdge) + 1.002f) * 0.5f;
					maxKinkiness = AIR::Max(kinkiness, maxKinkiness);

				}
			}

			// 以所有邻接边的最大相似度作为点塌陷的值
			cost = maxKinkiness; 

		}
	} 
	else 
	{

		// 遍历源点src的所有邻接三角形
		for(srcface = src->face.begin(); srcface != srcfaceEnd; ++srcface) 
		{
			float mincurv = 1.0f;

			FaceList::iterator sidesFace, sidesFaceEnd;
			sidesFaceEnd = sides.end();
			// 遍历所有含有边(src->dest)的三角形
			for(sidesFace = sides.begin(); sidesFace != sidesFaceEnd; ++sidesFace) 
			{
				// 计算2个三角形法线的点积
				float dotprod = (*srcface)->normal.DotProduct( (*sidesFace)->normal );
				mincurv =  AIR::Min(mincurv,(1.002f - dotprod) * 0.5f);
			}
			curvature = AIR::Max(curvature, mincurv);
		}
		cost = curvature;
	}

	if (src->seam && !dest->seam)
	{
		cost = 1.0f;
	}

	// 如果源点src与目标点dest的邻接三角形都是1个,那么src不能塌陷到dest
	if (src->face.size() == 1 && dest->face.size() == 1)
	{
		cost = NEVER_COLLAPSE_COST;
	}


	// 遍历源点src的所有邻接面
	for(srcface = src->face.begin(); srcface != srcfaceEnd; ++srcface) 
	{
		// 当前三角形不含边(src->dest)
		if( !(*srcface)->HasCommonVertex(dest) )
		{
			// 计算src变为dest后的新三角形
			PMVertex *v0, *v1, *v2;
			v0 = ( (*srcface)->vertex[0]->commonVertex == src) ? dest : (*srcface)->vertex[0]->commonVertex;
			v1 = ( (*srcface)->vertex[1]->commonVertex == src) ? dest : (*srcface)->vertex[1]->commonVertex;
			v2 = ( (*srcface)->vertex[2]->commonVertex == src) ? dest : (*srcface)->vertex[2]->commonVertex;

			// 计算新三角形的法线
			Vector3Df e1 = v1->position - v0->position; 
			Vector3Df e2 = v2->position - v1->position;
			Vector3Df newNormal = e1.CrossProduct(e2);
			newNormal.Normalize();
			// 如果就三角面的法线与新三角面的法线的夹角超过90度
			if (newNormal.DotProduct( (*srcface)->normal ) < 0.0f )
			{
				// 点src不能塌陷到dest
				cost = NEVER_COLLAPSE_COST;
				break; 
			}
		}
	}

	assert (cost >= 0);
	return cost;
}


float ProgressiveMesh::ComputeEdgeCostAtVertexForBuffer(WorkingDataList::iterator idata, size_t vertIndex)
{
	// 取得数据集中的第vertIndex个顶点
	CommonVertexList::iterator v = idata->mVertList.begin();
	v += vertIndex;

	// 如果该点没有邻居点
	if(v->neighbor.empty()) 
	{
		// 该点不用做塌陷处理
		v->NotifyRemoved();
		return v->collapseCost;
	}

	v->collapseCost = NEVER_COLLAPSE_COST;
	v->collapseTo = NULL;

	NeighborList::iterator n, nend;
	nend = v->neighbor.end();
	float cost;
	// 遍历该点的所有邻接点
	for(n = v->neighbor.begin(); n != nend; ++n) 
	{
		// 计算点v塌陷到邻接点n的花费值
		cost = ComputeEdgeCollapseCost(&(*v), *n);
		// 如果v当前没有塌陷的目标点或者塌陷的花费值比当前的塌陷值更小
		if( (!v->collapseTo) || cost < v->collapseCost) 
		{
			// 记录候选塌陷点
			v->collapseTo = *n;  
			// 记录塌陷值
			v->collapseCost = cost;           
		}
	}

	// 返回该点当前的最小塌陷值
	return v->collapseCost;
}


void ProgressiveMesh::ComputeEdgeCostAtVertex(size_t vertIndex)
{
	float worstCost = -0.01f;
	WorkingDataList::iterator i, iend;
	iend = mWorkingData.end();
	for (i = mWorkingData.begin(); i != iend; ++i)
	{
		worstCost = AIR::Max( worstCost, ComputeEdgeCostAtVertexForBuffer(i, vertIndex) );
	}
	// 计算第vertIndex个顶点的最大的塌陷花费
	mWorstCosts[vertIndex] = worstCost;
}

void ProgressiveMesh::ComputeAllCosts(void)
{
	InitialiseEdgeCollapseCosts();
	size_t i;
	// 计算每个顶点的塌陷花费值
	for (i = 0; i < m_pOriginSubMesh->GetVerticesNum(); ++i)
	{
		ComputeEdgeCostAtVertex(i);
	}
}


size_t ProgressiveMesh::GetNextCollapser(void)
{
	float bestVal = NEVER_COLLAPSE_COST;
	size_t i, bestIndex;
	bestIndex = 0;
	// 遍历所有的顶点
	for (i = 0; i < mNumCommonVertices; ++i)
	{
		if (mWorstCosts[i] < bestVal)
		{
			bestVal = mWorstCosts[i];
			bestIndex = i;
		}
	}
	// 取出塌陷花费最小的顶点
	return bestIndex;
}



void ProgressiveMesh::Collapse( PMVertex *src )
{
	// 取得源点src的目标塌陷点dest
    PMVertex *dest = src->collapseTo;
	std::set<PMVertex*> recomputeSet;

	// 如果塌陷花费为NEVER_COLLAPSE_COST,终止塌陷处理
	if (src->collapseCost == NEVER_COLLAPSE_COST) 
		return;

	// 设置源点的塌陷信息
	src->collapseTo = NULL;
	src->collapseCost = NEVER_COLLAPSE_COST;
	mWorstCosts[src->index] = NEVER_COLLAPSE_COST;

	// 如果塌陷目标点不存在,立刻返回
    if(!dest) 
	{
#if PM_DEBUG_MODE 
			ofdebug << "Aborting collapse, orphan vertex. " << std::endl;
#endif
		return;
    }

	// 将目标塌陷点dest添加进重计算列表
	recomputeSet.insert(dest);
	NeighborList::iterator n, nend;
    nend = src->neighbor.end();

	PMVertex* temp;
	// 遍历源点src的所有邻接顶点
    for(n = src->neighbor.begin(); n != nend; ++n)
    {
		// 将邻接点加进重计算列表
		temp = *n;
		recomputeSet.insert( *n );
	}
    nend = dest->neighbor.end();
    for(n = dest->neighbor.begin(); n != nend; ++n)
    {
		temp = *n;
		// 将塌陷目标点的所有邻接点加进重计算列表
		recomputeSet.insert( *n );
	}

    FaceList::iterator f, fend;
    fend = src->face.end();
	FaceList faceRemovalList, faceReplacementList;
	// 遍历源点的所有邻接面
    for(f = src->face.begin(); f != fend; ++f) 
    {
		// 如果邻接面含有塌陷目标点
	    if((*f)->HasCommonVertex(dest)) 
        {
			// 记录可删除面
			faceRemovalList.insert(*f);
			// 索引数减3
            mCurrNumIndexes -= 3;
	    }
        else
        {
			// 记录置换面
			faceReplacementList.insert(*f);
        }
    }

	// 设置源点为将要删除
	src->toBeRemoved = true;
	// 遍历置换面列表
    for(f = faceReplacementList.begin(); f != faceReplacementList.end(); ++f) 
	{
		// 获得置换面含有源点src的面顶点
		PMFaceVertex* srcFaceVert = (*f)->GetFaceVertexFromCommon(src);
		PMFaceVertex* destFaceVert = NULL;
		FaceList::iterator iremoved;
		// 遍历所有的可删除面
		for(iremoved = faceRemovalList.begin(); iremoved != faceRemovalList.end(); ++iremoved) 
		{
			// 取得可删除面上含有塌陷点的面顶点
			destFaceVert = (*iremoved)->GetFaceVertexFromCommon(dest); 
		}
		
		assert(destFaceVert);

#if PM_DEBUG_MODE 
		ofdebug << "Replacing vertex on face " << (unsigned int)(*f)->index << std::endl;
#endif
		// 将置换面的源面顶点置换成目标面顶点
        (*f)->ReplaceVertex(srcFaceVert, destFaceVert);
	}


	// 移除所有的可删除面
    for(f = faceRemovalList.begin(); f != faceRemovalList.end(); ++f) 
	{
#if PM_DEBUG_MODE 
		ofdebug << "Removing face " << (unsigned int)(*f)->index << std::endl;
#endif
		(*f)->NotifyRemoved();
	}

	// 移除目标点
    src->NotifyRemoved();

	std::set<PMVertex*>::iterator irecomp, irecompend;
	irecompend = recomputeSet.end();
	// 遍历所有的重计算顶点
	for (irecomp = recomputeSet.begin(); irecomp != irecompend; ++irecomp)
	{
		temp = (*irecomp);
		// 计算该点的塌陷花费
		ComputeEdgeCostAtVertex( (*irecomp)->index );
	}
	
}


void ProgressiveMesh::Build( int numLevels, MESH_LOD_LIST& outList, VertexReductionType quota /* = VRT_PROPORTIONAL */, float reductionValue /* = 0.5f */ )
{
	//在创建lod数据之前,判断一下lod数据列表是否为空;
	//第0层的lod保存的是原始网格数据,是必需的.
	if ( outList.size() == 0 )
	{
		DT_TO_MSGBOX( LT_WARN, "mesh base data not exist!" );
		return;
	}

	//取得原始数据
	LodData* pLod0 = outList[0];

	//当前的lod层
	LodData* newLod = NULL;

	// 计算所有顶点的塌陷花费值以及目标塌陷点
	ComputeAllCosts();

#ifdef PM_DEBUG_MODE
	DumpContent("pm_before.log");
#endif

	//获得原始网格的索引数量
	mCurrNumIndexes = m_pOriginSubMesh->GetFacesNum() * 3;
	size_t numVerts, numCollapses;
	numVerts = mNumCommonVertices;

#if PM_DEBUG_MODE 
	ofdebug.open("progressivemesh.log");
#endif

	numCollapses = 0;
	bool abandon = false;
	// 当前lod级别
	while (numLevels--)
	{
		if (!abandon)
		{
			// 计算需要塌陷的顶点个数
			if (quota == VRT_PROPORTIONAL)
			{
				numCollapses = static_cast<size_t>(numVerts * reductionValue);
			}
			else 
			{
				numCollapses = static_cast<size_t>(reductionValue);
			}
			if ( (numVerts - numCollapses) < 3) 
			{
				numCollapses = numVerts - 3;
			}
			// 计算当前lod级别下的顶点个数
			numVerts = numVerts - numCollapses;

			while(numCollapses-- && !abandon)
			{
				// 取得塌陷值最小的顶点
				size_t nextIndex = GetNextCollapser();
				WorkingDataList::iterator idata, idataend;
				idataend = mWorkingData.end();
				for (idata = mWorkingData.begin(); idata != idataend; ++idata)
				{
					// 取得塌陷点
					PMVertex* collapser = &( idata->mVertList.at( nextIndex ) );
					// 如果当前塌陷点没有目标点
					if (collapser->collapseTo == NULL)
					{
						// 停止塌陷
						abandon = true;
						break;
					}
#if PM_DEBUG_MODE 
					ofdebug << "Collapsing index " 
							<< (unsigned int)collapser->index 
							<< "(border: "
							<< collapser->IsBorder() 
							<< ") to " 
							<< (unsigned int)collapser->collapseTo->index 
							<< "(border: "
							<< collapser->collapseTo->IsBorder() 
							<< ")"
							<< std::endl;
#endif
					assert(collapser->collapseTo->removed == false);

					Collapse(collapser);
				}

			}
		}
#if PM_DEBUG_MODE
		DumpContent( GetString("pm_level_%d.log",numLevels) );
#endif
		

		// 创建当前lod层的数据
		newLod = AIR_NEW_T( LodData );
		outList.push_back(newLod);

		// 使用共享VB的方式创建lod数据
		// 在这种情况下,除了索引缓冲是new出来的,其他的数据都是引用第0层lod的数据
		if ( ENGINE_INST->GetAutoLodShareVB() )
		{
			//填充当前lod层次下的索引数据
			BakeNewLODShareVB( newLod, pLod0 );
		}
		// 使用独立VB的方式创建lod数据;
		// 在这种情况下, 除了权重数据是引用第0层lod的权重数据之外,其他的全部都是new出来的
		else
		{
			BakeNewLODIndependentVB( newLod, pLod0 );
		}		
	}

}



void ProgressiveMesh::BakeNewLODShareVB( LodData* pLodData, LodData* pLod0 )
{
	assert(mCurrNumIndexes > 0 && "No triangles to bake!");

	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	//创建索引缓冲
	pLodData->m_VertexData.m_pMeshIB = pRenderer->CreateIndexBuffer( mCurrNumIndexes );

	//锁定并填充索引数据
	u16* pIndexData = static_cast<u16*>(pLodData->m_VertexData.m_pMeshIB->Lock( 0, 0 ));
	TriangleList::iterator tri, triend;
	WorkingDataList::iterator pWork = mWorkingData.begin();
	triend = pWork->mTriList.end();
	for (tri = pWork->mTriList.begin(); tri != triend; ++tri)
	{
		if (!tri->removed)
		{
			*pIndexData++ = static_cast<u16>(tri->vertex[0]->realIndex);
			*pIndexData++ = static_cast<u16>(tri->vertex[1]->realIndex);
			*pIndexData++ = static_cast<u16>(tri->vertex[2]->realIndex);
		}
	}
	pLodData->m_VertexData.m_pMeshIB->Unlock();

	pLodData->m_VertexData.m_pMeshVB = pLod0->m_VertexData.m_pMeshVB;
	pLodData->m_pvtFaces = pLod0->m_pvtFaces;
	pLodData->m_pvtHardwareBlendVertex = pLod0->m_pvtHardwareBlendVertex;
	pLodData->m_pvtSoftwareBlendVertex = pLod0->m_pvtSoftwareBlendVertex;
	//pLodData->m_pvtSoftwareBlendWeight = pLod0->m_pvtSoftwareBlendWeight;
}


void ProgressiveMesh::BakeNewLODIndependentVB( LodData* pLodData, LodData* pLod0 )
{
	IRenderer* pRenderer = ENGINE_INST->GetRenderer();

	VEC_VERTEX_INFO vtOriginVertexInfo, vtLatestVertexInfo;
	VertexInfo vi;
	size_t nMaxVertexCount = m_pOriginSubMesh->GetVerticesNum();

	// 记录原始的顶点信息
	for ( size_t i = 0; i < nMaxVertexCount; ++i )
	{
		vi.m_nLatestID = vi.m_nRealID = i;
		vi.m_bRemoved = true;
		vtOriginVertexInfo.push_back( vi );
	}

	WorkingDataList::iterator pWork = mWorkingData.begin();
	TriangleList::iterator tri, triend;
	triend = pWork->mTriList.end();
	size_t nCurVertex;
	// 遍历所有的三角面
	for (tri = pWork->mTriList.begin(); tri != triend; ++tri)
	{
		// 如果当前三角面有效
		if (!tri->removed)
		{
			// 将该三角面的所有顶点全部置为不删除
			nCurVertex = static_cast<size_t>(tri->vertex[0]->realIndex);
			vtOriginVertexInfo[ nCurVertex ].m_bRemoved = false;
			nCurVertex = static_cast<size_t>(tri->vertex[1]->realIndex);
			vtOriginVertexInfo[ nCurVertex ].m_bRemoved = false;
			nCurVertex = static_cast<size_t>(tri->vertex[2]->realIndex);
			vtOriginVertexInfo[ nCurVertex ].m_bRemoved = false;
		}
	}

	// 遍历所有的顶点
	nCurVertex = 0;
	for ( size_t i = 0; i < nMaxVertexCount; ++i )
	{
		// 如果当前顶点不是要删除的
		if ( !vtOriginVertexInfo[i].m_bRemoved )
		{
			// 更新其最新的顶点ID
			vtOriginVertexInfo[i].m_nLatestID = nCurVertex;
			vtLatestVertexInfo.push_back( vtOriginVertexInfo[i] );
			++nCurVertex;
		}
	}

	// 创建当前lod等级下的索引缓冲
	pLodData->m_VertexData.m_pMeshIB = pRenderer->CreateIndexBuffer( mCurrNumIndexes );
	u16* pIndexData = static_cast<u16*>(pLodData->m_VertexData.m_pMeshIB->Lock( 0, 0 ));
	// 创建面表
	//pLodData->m_pvtFaces = new FACE_LIST;
	AIRMESHTRI face;
	// 遍历所有的三角面
	for (tri = pWork->mTriList.begin(); tri != triend; ++tri)
	{
		if (!tri->removed)
		{
			// 计算三角面所有顶点的新的ID
			// 填充索引数据
			face.nIndex[0] = *pIndexData++ = static_cast<u16>( vtOriginVertexInfo[ tri->vertex[0]->realIndex ].m_nLatestID );
			face.nIndex[1] = *pIndexData++ = static_cast<u16>( vtOriginVertexInfo[ tri->vertex[1]->realIndex ].m_nLatestID );
			face.nIndex[2] = *pIndexData++ = static_cast<u16>( vtOriginVertexInfo[ tri->vertex[2]->realIndex ].m_nLatestID );
			// 填充三角面数据
			pLodData->m_pvtFaces.push_back( face );
		}
	}
	pLodData->m_VertexData.m_pMeshIB->Unlock();

	// 取得网格的初始软件蒙皮顶点列表
	SOFTWARE_BLEND_VERTEX_LIST& vtSoftwareBlendVertex = (SOFTWARE_BLEND_VERTEX_LIST&)((pLod0->m_pvtSoftwareBlendVertex));
	// 取得网格的初始硬件蒙皮顶点列表
	HARDWARE_BLEND_VERTEX_LIST& vtHardwareBlendVertex = (HARDWARE_BLEND_VERTEX_LIST&)((pLod0->m_pvtHardwareBlendVertex));
	// 取得网格的初始软件蒙皮混合权重数据列表
	//SOFTWARE_BLEND_WEIGHT_LIST& vtSoftwareBlendWeight = (SOFTWARE_BLEND_WEIGHT_LIST&)((pLod0->m_pvtSoftwareBlendWeight));
	size_t curVertexCount = vtLatestVertexInfo.size();

	// 根据网格类型创建当前lod等级下的顶点数据
	switch( m_pOriginSubMesh->GetAnimationType() )
	{
		//静态网格
	case eMAT_NoAnimation:

		//顶点动画
	case eMAT_VertexAnimation:
		{
			// 创建静态网格或顶点动画网格的顶点数据
			//pLodData->m_pvtSoftwareBlendVertex = new SOFTWARE_BLEND_VERTEX_LIST;
			//pLodData->m_pvtSoftwareBlendWeight = pLod0->m_pvtSoftwareBlendWeight;

			// 填充当前lod的软件蒙皮顶点列表
			LPMESHVERTNORMAL pMeshVertex = NULL, pOriginMeshVertex = NULL;
			for ( size_t i = 0; i < curVertexCount; ++i )
			{
				pMeshVertex = AIR_NEW_T( MESHVERTNORMAL );
				pOriginMeshVertex = vtSoftwareBlendVertex[ vtLatestVertexInfo[i].m_nRealID ];
				memcpy( pMeshVertex, pOriginMeshVertex, sizeof(MESHVERTNORMAL) );
				pLodData->m_pvtSoftwareBlendVertex.push_back( pMeshVertex );
			}

			// 创建并填充当前lod的顶点缓冲
			//pLodData->m_VertexData.m_pMeshVB = pRenderer->CreateVertexBuffer(E_VB_NORMAL, curVertexCount);
            pLodData->m_VertexData.m_pMeshVB = pRenderer->CreateVertexBuffer(curVertexCount, sizeof(VERTEX_NORMAL));
			LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)pLodData->m_VertexData.m_pMeshVB->Lock(true);
			LPVERTEX_NORMAL pVertex = NULL;
			for (size_t i = 0; i < curVertexCount; i++)
			{
				pVertex = &(((pLodData->m_pvtSoftwareBlendVertex))[i]->vertex);
				memcpy(pVertices, pVertex, sizeof(VERTEX_NORMAL));
				pVertices++;
			}
			pLodData->m_VertexData.m_pMeshVB->Unlock();
		}
		break;

		//骨骼动画
	case eMAT_SkeletionAnimation:
		{
			// 硬件蒙皮
			if ( ENGINE_INST->IsHardwareSkeleton() )
			{
				// 创建硬件蒙皮的骨骼动画顶点数据
				//pLodData->m_pvtHardwareBlendVertex = new HARDWARE_BLEND_VERTEX_LIST;
				//pLodData->m_pvtSoftwareBlendWeight = pLod0->m_pvtSoftwareBlendWeight;

				LPHARDWAREMESHVERTEX pMeshVertex = NULL, pOriginMeshVertex = NULL;

				// 填充当前lod的硬件蒙皮顶点列表
				for ( size_t i = 0; i < curVertexCount; ++i )
				{
					pMeshVertex = AIR_NEW_T( HARDWAREMESHVERTEX );
					pOriginMeshVertex = vtHardwareBlendVertex[ vtLatestVertexInfo[i].m_nRealID ];
					memcpy( pMeshVertex, pOriginMeshVertex, sizeof(HARDWAREMESHVERTEX) );
					pLodData->m_pvtHardwareBlendVertex.push_back( pMeshVertex );
				}

				// 创建并填充当前lod的顶点缓冲
				//pLodData->m_VertexData.m_pMeshVB = pRenderer->CreateVertexBuffer(E_VB_NORMAL_BONE_WEIGHT, curVertexCount);
                pLodData->m_VertexData.m_pMeshVB = pRenderer->CreateVertexBuffer(curVertexCount, sizeof(VERTEXNORMALBONEWEIGHTS));
				LPVERTEXNORMALBONEWEIGHTS pVertices = (LPVERTEXNORMALBONEWEIGHTS)pLodData->m_VertexData.m_pMeshVB->Lock(true);
				LPVERTEXNORMALBONEWEIGHTS pVertex = NULL;
				for (size_t i = 0; i < curVertexCount; i++)
				{
					pVertex = &(((pLodData->m_pvtHardwareBlendVertex))[i]->vertex);
					memcpy(pVertices, pVertex, sizeof(VERTEXNORMALBONEWEIGHTS));
					pVertices++;
				}
				pLodData->m_VertexData.m_pMeshVB->Unlock();
			}
			else
			{
				// 创建软件蒙皮的骨骼动画顶点数据
				//pLodData->m_pvtSoftwareBlendVertex = new SOFTWARE_BLEND_VERTEX_LIST;
				//pLodData->m_pvtSoftwareBlendWeight = pLod0->m_pvtSoftwareBlendWeight;

				LPMESHVERTNORMAL pMeshVertex = NULL, pOriginMeshVertex = NULL;

				// 填充当前lod的软件蒙皮顶点列表
				for ( size_t i = 0; i < curVertexCount; ++i )
				{
					pMeshVertex = AIR_NEW_T( MESHVERTNORMAL );
					pOriginMeshVertex = vtSoftwareBlendVertex[ vtLatestVertexInfo[i].m_nRealID ];
					memcpy( pMeshVertex, pOriginMeshVertex, sizeof(MESHVERTNORMAL) );
					pLodData->m_pvtSoftwareBlendVertex.push_back( pMeshVertex );
				}

				// 创建并填充当前lod的顶点缓冲
				//pLodData->m_VertexData.m_pMeshVB = pRenderer->CreateVertexBuffer(E_VB_NORMAL, curVertexCount);
                pLodData->m_VertexData.m_pMeshVB = pRenderer->CreateVertexBuffer(curVertexCount, sizeof(VERTEX_NORMAL));
				LPVERTEX_NORMAL pVertices = (LPVERTEX_NORMAL)pLodData->m_VertexData.m_pMeshVB->Lock(true);
				LPVERTEX_NORMAL pVertex = NULL;
				for (size_t i = 0; i < curVertexCount; i++)
				{
					pVertex = &(((pLodData->m_pvtSoftwareBlendVertex))[i]->vertex);
					memcpy(pVertices, pVertex, sizeof(VERTEX_NORMAL));
					pVertices++;
				}
				pLodData->m_VertexData.m_pMeshVB->Unlock();
			}
		}
		break;
	}
}


void ProgressiveMesh::DumpContent( const AIRString& log )
{
	std::ofstream ofdump(log.c_str());

	WorkingDataList::iterator worki = mWorkingData.begin();

	CommonVertexList::iterator vi, vend;
	vend = worki->mVertList.end();
	ofdump << "-------== VERTEX LIST ==-----------------" << std::endl;
	size_t i;
	for (vi = worki->mVertList.begin(), i = 0; i < mNumCommonVertices; ++vi, ++i)
	{
		ofdump	<< "Vertex " 
				<< (unsigned int)vi->index 
				<< " pos: " 
				<< vi->position.x 
				<< "," 
				<< vi->position.y 
				<< "," 
				<<vi->position.z 
				<< " removed: " 
				<< vi->removed 
				<< " isborder: " 
				<< vi->IsBorder() 
				<< std::endl;

		ofdump << "    Faces:" << std::endl;

		FaceList::iterator f, fend;
		fend = vi->face.end();
		for(f = vi->face.begin(); f != fend; ++f)
		{
			ofdump << "    Triangle index " << (unsigned int)(*f)->index << std::endl;
		}
		ofdump << "    Neighbours:" << std::endl;
		NeighborList::iterator n, nend;
		nend = vi->neighbor.end();
		for (n = vi->neighbor.begin(); n != nend; ++n)
		{
			ofdump << "    Vertex index " << (unsigned int)(*n)->index << std::endl;
		}

	}

	TriangleList::iterator ti, tend;
	tend = worki->mTriList.end();
	ofdump << "-------== TRIANGLE LIST ==-----------------" << std::endl;
	for(ti = worki->mTriList.begin(); ti != tend; ++ti)
	{
		ofdump	<< "Triangle " 
				<< (unsigned int)ti->index 
				<< " norm: " 
				<< ti->normal.x 
				<< "," 
				<< ti->normal.y 
				<< "," 
				<< ti->normal.z 
				<< " removed: " 
				<< ti->removed 
				<< std::endl;

		ofdump << "    Vertex 0: " << (unsigned int)ti->vertex[0]->realIndex << std::endl;
		ofdump << "    Vertex 1: " << (unsigned int)ti->vertex[1]->realIndex << std::endl;
		ofdump << "    Vertex 2: " << (unsigned int)ti->vertex[2]->realIndex << std::endl;
	}

	ofdump << "-------== COLLAPSE COST LIST ==-----------------" << std::endl;
	for (size_t ci = 0; ci < mNumCommonVertices; ++ci)
	{
		ofdump << "Vertex " << (unsigned int)ci << ": " << mWorstCosts[ci] << std::endl;
	}

	ofdump << "--------------------real indexs--------------------\n";
	i = 0;
	for(ti = worki->mTriList.begin(); ti != tend; ++ti)
	{
		if ( !ti->removed )
		{
			ofdump	<< i++ 
					<< "\t: " 
					<< (unsigned int)ti->vertex[0]->realIndex 
					<< "," 
					<< (unsigned int)ti->vertex[1]->realIndex 
					<< "," 
					<< (unsigned int)ti->vertex[2]->realIndex 
					<< std::endl;
		}
	}
	ofdump << "triangle count : " << i << "\n";

	ofdump.close();
}