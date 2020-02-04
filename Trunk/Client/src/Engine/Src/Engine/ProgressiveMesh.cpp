
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

	// ������
	index = newindex;
	// �涥��
	vertex[0] = v0;
	vertex[1] = v1;
	vertex[2] = v2;

	// ���������淨��
	ComputeNormal();

	for(int i=0;i<3;i++) 
	{
		// ��¼����
		vertex[i]->commonVertex->face.insert(this);
		// ��¼�ڽ���
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
	// �������Ƿ��ж���v
	return (
		v == vertex[0]->commonVertex ||
		v == vertex[1]->commonVertex || 
		v == vertex[2]->commonVertex	);
}

bool			PMTriangle::HasFaceVertex(PMFaceVertex *v) const
{
	// �������Ƿ����涥��
	return (
		v == vertex[0] ||
		v == vertex[1] || 
		v == vertex[2]	);
}

PMFaceVertex*	PMTriangle::GetFaceVertexFromCommon(PMVertex* commonVert)
{
	// ��ú��ж���commonVert���涥��
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
		// ��������Ӷ��������Ϣ���Ƴ�
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

	// ȡ������Ķ�������
	size_t nVertexCount = m_pOriginSubMesh->GetVerticesNum();

	// ����һ���յĹ��������鲢�Ž���������������
	mWorkingData.push_back(PMWorkingData());

	// ȡ���µĹ���������
	PMWorkingData& work = mWorkingData.back();

	// ��ʼ������������Ķ����б�
	work.mFaceVertList.resize( nVertexCount );
	work.mVertList.resize( nVertexCount );

	Vector3Df pos;
	// ���渴�ƵĶ����б������
	typedef std::map<Vector3Df, size_t, vectorLess> CommonVertexMap;
	CommonVertexMap commonVertexMap;
	CommonVertexMap::iterator iCommonVertex;
	size_t numCommon = 0;
	size_t i = 0;

	SOFTWARE_BLEND_VERTEX_LIST& vtSoftwareBlendVertex = (SOFTWARE_BLEND_VERTEX_LIST&)((pLodList[0]->m_pvtSoftwareBlendVertex));//m_pOriginSubMesh->GetSoftwareBlendVertexList();
	HARDWARE_BLEND_VERTEX_LIST& vtHardwareBlendVertex = (HARDWARE_BLEND_VERTEX_LIST&)((pLodList[0]->m_pvtHardwareBlendVertex));//m_pOriginSubMesh->GetHardwareBlendVertexList();
	
	LPMESHVERTNORMAL pMeshNormalVertex = NULL;
	LPHARDWAREMESHVERTEX pHardwareMeshVertex = NULL;

	// ������������ж���
	for ( i = 0; i < nVertexCount; ++i )
	{
		// ������������,������������ȡ�õ�ǰ���������
		switch ( m_pOriginSubMesh->GetAnimationType() )
		{
			// ��̬����
		case eMAT_NoAnimation:
			// �ж��㶯��������
		case eMAT_VertexAnimation:
			{
				pMeshNormalVertex = vtSoftwareBlendVertex[i];
				pos = pMeshNormalVertex->vertex.vPos;
			}
			break;
			// �й�������������
		case eMAT_SkeletionAnimation:
			{
				// ��ǰʹ��Ӳ����Ƥ
				if ( ENGINE_INST->IsHardwareSkeleton() )
				{
					pHardwareMeshVertex = vtHardwareBlendVertex[i];
					pos = pHardwareMeshVertex->vertex.vertex.vPos;
				}
				// ��ǰʹ�������Ƥ
				else
				{
					pMeshNormalVertex = vtSoftwareBlendVertex[i];
					pos = pMeshNormalVertex->vertex.vPos;
				}
			}
			break;
		}

		// �鿴�ڶ����������Ƿ��Ѵ��ڸö���
		iCommonVertex = commonVertexMap.find(pos);

		// ���������û�иö���
		if (iCommonVertex == commonVertexMap.end())
		{
			// ����һ���µĽ������񶥵�ṹ��
			PMVertex* commonVert = &(work.mVertList[numCommon]);
			// ���ö���ϸ����Ϣ
			commonVert->SetDetails(pos, numCommon);
			commonVert->removed = false;
			commonVert->toBeRemoved = false;
			commonVert->seam = false;

			// ��������Ϣ�Ž�����
			commonVertexMap.insert(CommonVertexMap::value_type(pos, numCommon) );

			++numCommon;

			work.mFaceVertList[i].commonVertex = commonVert;
			work.mFaceVertList[i].realIndex = i;
		}
		else
		{
			// ����Ѿ�����, ȡ����
			PMVertex* existingVert = &(work.mVertList[iCommonVertex->second]);
			work.mFaceVertList[i].commonVertex = existingVert;
			work.mFaceVertList[i].realIndex = i;

			// ���ýӷ�����Ϊtrue
			work.mFaceVertList[i].commonVertex->seam = true;
		}
	}


	mNumCommonVertices = numCommon;

	// ���������������б���Ϣ
	//FACE_LIST& vtFaces = m_pOriginSubMesh->GetFaceList();
	FACE_LIST& vtFaces = (FACE_LIST&)((pLodList[0]->m_pvtFaces));

	// ���������θ���
	size_t numTris = vtFaces.size();
	work.mTriList.resize(numTris); 

	// �������е�������
	for (i = 0; i < numTris; ++i)
	{
		// ȡ�������ε�3�����������ֵ
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
	// ����Դ��src�������ڽ���
	for(srcface = src->face.begin(); srcface != srcfaceEnd; ++srcface)
	{
		// �Ѻ��ж���dest���ڽ����¼����
		if( (*srcface)->HasCommonVertex(dest) )
		{
			sides.insert(*srcface);
		}
	}

	// ��鶥��src�Ƿ�����߽��ϵĶ���
	if(src->IsBorder())
	{
		// ���src�Ǳ߽�㲢����dest��ɵı��ڽ�2�����ϵ�������
		if (sides.size() > 1) 
		{
			cost = 1.0f;
		}
		// ���src�Ǳ߽�㲢����dest��ɵı��ڽ�1��������
		else
		{
			Vector3Df collapseEdge, otherBorderEdge;
			float kinkiness, maxKinkiness;
			NeighborList::iterator n, nend;
			nend = src->neighbor.end();
			maxKinkiness = 0.0f;
			edgeVector.Normalize();
			collapseEdge = edgeVector;
			// ����src�������ڽӶ���
			for (n = src->neighbor.begin(); n != nend; ++n)
			{
				// ����ڽӵ㲻��dest����src���ڽӵ���ɵı�ֻ��1���ڽ�������
				if (*n != dest && (*n)->IsManifoldEdgeWith(src))
				{
					otherBorderEdge = src->position - (*n)->position;
					otherBorderEdge.Normalize();

					// ����Դ��(src->dest)���(src->n)�����ƶ�
					kinkiness = (otherBorderEdge.DotProduct(collapseEdge) + 1.002f) * 0.5f;
					maxKinkiness = AIR::Max(kinkiness, maxKinkiness);

				}
			}

			// �������ڽӱߵ�������ƶ���Ϊ�����ݵ�ֵ
			cost = maxKinkiness; 

		}
	} 
	else 
	{

		// ����Դ��src�������ڽ�������
		for(srcface = src->face.begin(); srcface != srcfaceEnd; ++srcface) 
		{
			float mincurv = 1.0f;

			FaceList::iterator sidesFace, sidesFaceEnd;
			sidesFaceEnd = sides.end();
			// �������к��б�(src->dest)��������
			for(sidesFace = sides.begin(); sidesFace != sidesFaceEnd; ++sidesFace) 
			{
				// ����2�������η��ߵĵ��
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

	// ���Դ��src��Ŀ���dest���ڽ������ζ���1��,��ôsrc�������ݵ�dest
	if (src->face.size() == 1 && dest->face.size() == 1)
	{
		cost = NEVER_COLLAPSE_COST;
	}


	// ����Դ��src�������ڽ���
	for(srcface = src->face.begin(); srcface != srcfaceEnd; ++srcface) 
	{
		// ��ǰ�����β�����(src->dest)
		if( !(*srcface)->HasCommonVertex(dest) )
		{
			// ����src��Ϊdest�����������
			PMVertex *v0, *v1, *v2;
			v0 = ( (*srcface)->vertex[0]->commonVertex == src) ? dest : (*srcface)->vertex[0]->commonVertex;
			v1 = ( (*srcface)->vertex[1]->commonVertex == src) ? dest : (*srcface)->vertex[1]->commonVertex;
			v2 = ( (*srcface)->vertex[2]->commonVertex == src) ? dest : (*srcface)->vertex[2]->commonVertex;

			// �����������εķ���
			Vector3Df e1 = v1->position - v0->position; 
			Vector3Df e2 = v2->position - v1->position;
			Vector3Df newNormal = e1.CrossProduct(e2);
			newNormal.Normalize();
			// �����������ķ�������������ķ��ߵļнǳ���90��
			if (newNormal.DotProduct( (*srcface)->normal ) < 0.0f )
			{
				// ��src�������ݵ�dest
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
	// ȡ�����ݼ��еĵ�vertIndex������
	CommonVertexList::iterator v = idata->mVertList.begin();
	v += vertIndex;

	// ����õ�û���ھӵ�
	if(v->neighbor.empty()) 
	{
		// �õ㲻�������ݴ���
		v->NotifyRemoved();
		return v->collapseCost;
	}

	v->collapseCost = NEVER_COLLAPSE_COST;
	v->collapseTo = NULL;

	NeighborList::iterator n, nend;
	nend = v->neighbor.end();
	float cost;
	// �����õ�������ڽӵ�
	for(n = v->neighbor.begin(); n != nend; ++n) 
	{
		// �����v���ݵ��ڽӵ�n�Ļ���ֵ
		cost = ComputeEdgeCollapseCost(&(*v), *n);
		// ���v��ǰû�����ݵ�Ŀ���������ݵĻ���ֵ�ȵ�ǰ������ֵ��С
		if( (!v->collapseTo) || cost < v->collapseCost) 
		{
			// ��¼��ѡ���ݵ�
			v->collapseTo = *n;  
			// ��¼����ֵ
			v->collapseCost = cost;           
		}
	}

	// ���ظõ㵱ǰ����С����ֵ
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
	// �����vertIndex��������������ݻ���
	mWorstCosts[vertIndex] = worstCost;
}

void ProgressiveMesh::ComputeAllCosts(void)
{
	InitialiseEdgeCollapseCosts();
	size_t i;
	// ����ÿ����������ݻ���ֵ
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
	// �������еĶ���
	for (i = 0; i < mNumCommonVertices; ++i)
	{
		if (mWorstCosts[i] < bestVal)
		{
			bestVal = mWorstCosts[i];
			bestIndex = i;
		}
	}
	// ȡ�����ݻ�����С�Ķ���
	return bestIndex;
}



void ProgressiveMesh::Collapse( PMVertex *src )
{
	// ȡ��Դ��src��Ŀ�����ݵ�dest
    PMVertex *dest = src->collapseTo;
	std::set<PMVertex*> recomputeSet;

	// ������ݻ���ΪNEVER_COLLAPSE_COST,��ֹ���ݴ���
	if (src->collapseCost == NEVER_COLLAPSE_COST) 
		return;

	// ����Դ���������Ϣ
	src->collapseTo = NULL;
	src->collapseCost = NEVER_COLLAPSE_COST;
	mWorstCosts[src->index] = NEVER_COLLAPSE_COST;

	// �������Ŀ��㲻����,���̷���
    if(!dest) 
	{
#if PM_DEBUG_MODE 
			ofdebug << "Aborting collapse, orphan vertex. " << std::endl;
#endif
		return;
    }

	// ��Ŀ�����ݵ�dest��ӽ��ؼ����б�
	recomputeSet.insert(dest);
	NeighborList::iterator n, nend;
    nend = src->neighbor.end();

	PMVertex* temp;
	// ����Դ��src�������ڽӶ���
    for(n = src->neighbor.begin(); n != nend; ++n)
    {
		// ���ڽӵ�ӽ��ؼ����б�
		temp = *n;
		recomputeSet.insert( *n );
	}
    nend = dest->neighbor.end();
    for(n = dest->neighbor.begin(); n != nend; ++n)
    {
		temp = *n;
		// ������Ŀ���������ڽӵ�ӽ��ؼ����б�
		recomputeSet.insert( *n );
	}

    FaceList::iterator f, fend;
    fend = src->face.end();
	FaceList faceRemovalList, faceReplacementList;
	// ����Դ��������ڽ���
    for(f = src->face.begin(); f != fend; ++f) 
    {
		// ����ڽ��溬������Ŀ���
	    if((*f)->HasCommonVertex(dest)) 
        {
			// ��¼��ɾ����
			faceRemovalList.insert(*f);
			// ��������3
            mCurrNumIndexes -= 3;
	    }
        else
        {
			// ��¼�û���
			faceReplacementList.insert(*f);
        }
    }

	// ����Դ��Ϊ��Ҫɾ��
	src->toBeRemoved = true;
	// �����û����б�
    for(f = faceReplacementList.begin(); f != faceReplacementList.end(); ++f) 
	{
		// ����û��溬��Դ��src���涥��
		PMFaceVertex* srcFaceVert = (*f)->GetFaceVertexFromCommon(src);
		PMFaceVertex* destFaceVert = NULL;
		FaceList::iterator iremoved;
		// �������еĿ�ɾ����
		for(iremoved = faceRemovalList.begin(); iremoved != faceRemovalList.end(); ++iremoved) 
		{
			// ȡ�ÿ�ɾ�����Ϻ������ݵ���涥��
			destFaceVert = (*iremoved)->GetFaceVertexFromCommon(dest); 
		}
		
		assert(destFaceVert);

#if PM_DEBUG_MODE 
		ofdebug << "Replacing vertex on face " << (unsigned int)(*f)->index << std::endl;
#endif
		// ���û����Դ�涥���û���Ŀ���涥��
        (*f)->ReplaceVertex(srcFaceVert, destFaceVert);
	}


	// �Ƴ����еĿ�ɾ����
    for(f = faceRemovalList.begin(); f != faceRemovalList.end(); ++f) 
	{
#if PM_DEBUG_MODE 
		ofdebug << "Removing face " << (unsigned int)(*f)->index << std::endl;
#endif
		(*f)->NotifyRemoved();
	}

	// �Ƴ�Ŀ���
    src->NotifyRemoved();

	std::set<PMVertex*>::iterator irecomp, irecompend;
	irecompend = recomputeSet.end();
	// �������е��ؼ��㶥��
	for (irecomp = recomputeSet.begin(); irecomp != irecompend; ++irecomp)
	{
		temp = (*irecomp);
		// ����õ�����ݻ���
		ComputeEdgeCostAtVertex( (*irecomp)->index );
	}
	
}


void ProgressiveMesh::Build( int numLevels, MESH_LOD_LIST& outList, VertexReductionType quota /* = VRT_PROPORTIONAL */, float reductionValue /* = 0.5f */ )
{
	//�ڴ���lod����֮ǰ,�ж�һ��lod�����б��Ƿ�Ϊ��;
	//��0���lod�������ԭʼ��������,�Ǳ����.
	if ( outList.size() == 0 )
	{
		DT_TO_MSGBOX( LT_WARN, "mesh base data not exist!" );
		return;
	}

	//ȡ��ԭʼ����
	LodData* pLod0 = outList[0];

	//��ǰ��lod��
	LodData* newLod = NULL;

	// �������ж�������ݻ���ֵ�Լ�Ŀ�����ݵ�
	ComputeAllCosts();

#ifdef PM_DEBUG_MODE
	DumpContent("pm_before.log");
#endif

	//���ԭʼ�������������
	mCurrNumIndexes = m_pOriginSubMesh->GetFacesNum() * 3;
	size_t numVerts, numCollapses;
	numVerts = mNumCommonVertices;

#if PM_DEBUG_MODE 
	ofdebug.open("progressivemesh.log");
#endif

	numCollapses = 0;
	bool abandon = false;
	// ��ǰlod����
	while (numLevels--)
	{
		if (!abandon)
		{
			// ������Ҫ���ݵĶ������
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
			// ���㵱ǰlod�����µĶ������
			numVerts = numVerts - numCollapses;

			while(numCollapses-- && !abandon)
			{
				// ȡ������ֵ��С�Ķ���
				size_t nextIndex = GetNextCollapser();
				WorkingDataList::iterator idata, idataend;
				idataend = mWorkingData.end();
				for (idata = mWorkingData.begin(); idata != idataend; ++idata)
				{
					// ȡ�����ݵ�
					PMVertex* collapser = &( idata->mVertList.at( nextIndex ) );
					// �����ǰ���ݵ�û��Ŀ���
					if (collapser->collapseTo == NULL)
					{
						// ֹͣ����
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
		

		// ������ǰlod�������
		newLod = AIR_NEW_T( LodData );
		outList.push_back(newLod);

		// ʹ�ù���VB�ķ�ʽ����lod����
		// �����������,��������������new������,���������ݶ������õ�0��lod������
		if ( ENGINE_INST->GetAutoLodShareVB() )
		{
			//��䵱ǰlod����µ���������
			BakeNewLODShareVB( newLod, pLod0 );
		}
		// ʹ�ö���VB�ķ�ʽ����lod����;
		// �����������, ����Ȩ�����������õ�0��lod��Ȩ������֮��,������ȫ������new������
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

	//������������
	pLodData->m_VertexData.m_pMeshIB = pRenderer->CreateIndexBuffer( mCurrNumIndexes );

	//�����������������
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

	// ��¼ԭʼ�Ķ�����Ϣ
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
	// �������е�������
	for (tri = pWork->mTriList.begin(); tri != triend; ++tri)
	{
		// �����ǰ��������Ч
		if (!tri->removed)
		{
			// ��������������ж���ȫ����Ϊ��ɾ��
			nCurVertex = static_cast<size_t>(tri->vertex[0]->realIndex);
			vtOriginVertexInfo[ nCurVertex ].m_bRemoved = false;
			nCurVertex = static_cast<size_t>(tri->vertex[1]->realIndex);
			vtOriginVertexInfo[ nCurVertex ].m_bRemoved = false;
			nCurVertex = static_cast<size_t>(tri->vertex[2]->realIndex);
			vtOriginVertexInfo[ nCurVertex ].m_bRemoved = false;
		}
	}

	// �������еĶ���
	nCurVertex = 0;
	for ( size_t i = 0; i < nMaxVertexCount; ++i )
	{
		// �����ǰ���㲻��Ҫɾ����
		if ( !vtOriginVertexInfo[i].m_bRemoved )
		{
			// ���������µĶ���ID
			vtOriginVertexInfo[i].m_nLatestID = nCurVertex;
			vtLatestVertexInfo.push_back( vtOriginVertexInfo[i] );
			++nCurVertex;
		}
	}

	// ������ǰlod�ȼ��µ���������
	pLodData->m_VertexData.m_pMeshIB = pRenderer->CreateIndexBuffer( mCurrNumIndexes );
	u16* pIndexData = static_cast<u16*>(pLodData->m_VertexData.m_pMeshIB->Lock( 0, 0 ));
	// �������
	//pLodData->m_pvtFaces = new FACE_LIST;
	AIRMESHTRI face;
	// �������е�������
	for (tri = pWork->mTriList.begin(); tri != triend; ++tri)
	{
		if (!tri->removed)
		{
			// �������������ж�����µ�ID
			// �����������
			face.nIndex[0] = *pIndexData++ = static_cast<u16>( vtOriginVertexInfo[ tri->vertex[0]->realIndex ].m_nLatestID );
			face.nIndex[1] = *pIndexData++ = static_cast<u16>( vtOriginVertexInfo[ tri->vertex[1]->realIndex ].m_nLatestID );
			face.nIndex[2] = *pIndexData++ = static_cast<u16>( vtOriginVertexInfo[ tri->vertex[2]->realIndex ].m_nLatestID );
			// �������������
			pLodData->m_pvtFaces.push_back( face );
		}
	}
	pLodData->m_VertexData.m_pMeshIB->Unlock();

	// ȡ������ĳ�ʼ�����Ƥ�����б�
	SOFTWARE_BLEND_VERTEX_LIST& vtSoftwareBlendVertex = (SOFTWARE_BLEND_VERTEX_LIST&)((pLod0->m_pvtSoftwareBlendVertex));
	// ȡ������ĳ�ʼӲ����Ƥ�����б�
	HARDWARE_BLEND_VERTEX_LIST& vtHardwareBlendVertex = (HARDWARE_BLEND_VERTEX_LIST&)((pLod0->m_pvtHardwareBlendVertex));
	// ȡ������ĳ�ʼ�����Ƥ���Ȩ�������б�
	//SOFTWARE_BLEND_WEIGHT_LIST& vtSoftwareBlendWeight = (SOFTWARE_BLEND_WEIGHT_LIST&)((pLod0->m_pvtSoftwareBlendWeight));
	size_t curVertexCount = vtLatestVertexInfo.size();

	// �����������ʹ�����ǰlod�ȼ��µĶ�������
	switch( m_pOriginSubMesh->GetAnimationType() )
	{
		//��̬����
	case eMAT_NoAnimation:

		//���㶯��
	case eMAT_VertexAnimation:
		{
			// ������̬����򶥵㶯������Ķ�������
			//pLodData->m_pvtSoftwareBlendVertex = new SOFTWARE_BLEND_VERTEX_LIST;
			//pLodData->m_pvtSoftwareBlendWeight = pLod0->m_pvtSoftwareBlendWeight;

			// ��䵱ǰlod�������Ƥ�����б�
			LPMESHVERTNORMAL pMeshVertex = NULL, pOriginMeshVertex = NULL;
			for ( size_t i = 0; i < curVertexCount; ++i )
			{
				pMeshVertex = AIR_NEW_T( MESHVERTNORMAL );
				pOriginMeshVertex = vtSoftwareBlendVertex[ vtLatestVertexInfo[i].m_nRealID ];
				memcpy( pMeshVertex, pOriginMeshVertex, sizeof(MESHVERTNORMAL) );
				pLodData->m_pvtSoftwareBlendVertex.push_back( pMeshVertex );
			}

			// ��������䵱ǰlod�Ķ��㻺��
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

		//��������
	case eMAT_SkeletionAnimation:
		{
			// Ӳ����Ƥ
			if ( ENGINE_INST->IsHardwareSkeleton() )
			{
				// ����Ӳ����Ƥ�Ĺ���������������
				//pLodData->m_pvtHardwareBlendVertex = new HARDWARE_BLEND_VERTEX_LIST;
				//pLodData->m_pvtSoftwareBlendWeight = pLod0->m_pvtSoftwareBlendWeight;

				LPHARDWAREMESHVERTEX pMeshVertex = NULL, pOriginMeshVertex = NULL;

				// ��䵱ǰlod��Ӳ����Ƥ�����б�
				for ( size_t i = 0; i < curVertexCount; ++i )
				{
					pMeshVertex = AIR_NEW_T( HARDWAREMESHVERTEX );
					pOriginMeshVertex = vtHardwareBlendVertex[ vtLatestVertexInfo[i].m_nRealID ];
					memcpy( pMeshVertex, pOriginMeshVertex, sizeof(HARDWAREMESHVERTEX) );
					pLodData->m_pvtHardwareBlendVertex.push_back( pMeshVertex );
				}

				// ��������䵱ǰlod�Ķ��㻺��
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
				// ���������Ƥ�Ĺ���������������
				//pLodData->m_pvtSoftwareBlendVertex = new SOFTWARE_BLEND_VERTEX_LIST;
				//pLodData->m_pvtSoftwareBlendWeight = pLod0->m_pvtSoftwareBlendWeight;

				LPMESHVERTNORMAL pMeshVertex = NULL, pOriginMeshVertex = NULL;

				// ��䵱ǰlod�������Ƥ�����б�
				for ( size_t i = 0; i < curVertexCount; ++i )
				{
					pMeshVertex = AIR_NEW_T( MESHVERTNORMAL );
					pOriginMeshVertex = vtSoftwareBlendVertex[ vtLatestVertexInfo[i].m_nRealID ];
					memcpy( pMeshVertex, pOriginMeshVertex, sizeof(MESHVERTNORMAL) );
					pLodData->m_pvtSoftwareBlendVertex.push_back( pMeshVertex );
				}

				// ��������䵱ǰlod�Ķ��㻺��
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