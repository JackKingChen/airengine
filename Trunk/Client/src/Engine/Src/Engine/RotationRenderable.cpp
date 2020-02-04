#include "stdafx.h"
#include "RotationRenderable.h"
#include "Line2D.h"
#include "MathUtil.h"
#include "VertexDeclarationManager.h"
#include "Primitive.h"
#include "SceneNode.h"
RotationRenderable::RotationRenderable() : m_nNumT(0)
{
	//这部分计算要修改
	m_vtXLines.resize(NUM_SEGMENT+1);
	m_vtYLines.resize(NUM_SEGMENT+1);
	m_vtZLines.resize(NUM_SEGMENT+1);
	m_vtBackLines.resize(2*NUM_SEGMENT+1);
	
	m_scXColor      = SColor(255, 255, 0, 0);
	m_scYColor      = SColor(255, 0, 255, 0);
	m_scZColor      = SColor(255, 0, 0, 255);
	m_scBackColor   = SColor(255, 50,50,50);
	m_scSelectColor = SColor(255, 0, 0, 0);
	
	float fSegment = 2.0f * AIR::PI / NUM_SEGMENT;
	for (int i = 0 ; i <= NUM_SEGMENT ; ++i)
	{
		mSinCos[i].x = sin(fSegment * i);
		mSinCos[i].y = cos(fSegment * i);
	}
	m_fRadius = 100;
	m_transformSpace = CSceneNode::TS_PARENT;
	ResetAngles();

	m_strVertexDecl = "E_VB_RHW_NOTEX";
}

RotationRenderable::~RotationRenderable()
{

}


void RotationRenderable::Render(CCamera* pCamera)
{
	
}

void RotationRenderable::OnMouseMove(int x, int y)
{
	Vector2Df mousePoint(x, y);
	Line2Df tmpLine;
	float  fMinDis = 999;
	RotationSelection    nSelect = RS_NONE;
	for (size_t i = 0 ; i < m_vtXLines.size() - 1 ; ++i)
	{
		tmpLine.setLine(m_vtXLines[i], m_vtXLines[i+1]);
		if (fMinDis > tmpLine.GetDistanceFromPoint(mousePoint))
		{
			m_v2SelectPoint = tmpLine.GetClosestPoint(mousePoint);
			fMinDis = tmpLine.GetDistanceFromPoint(mousePoint);
			nSelect = RS_X;
		}
	}

	for (size_t i = 0 ; i < m_vtYLines.size() - 1 ; ++i)
	{
		tmpLine.setLine(m_vtYLines[i], m_vtYLines[i+1]);
		if (fMinDis > tmpLine.GetDistanceFromPoint(mousePoint))
		{
			m_v2SelectPoint = tmpLine.GetClosestPoint(mousePoint);
			fMinDis = tmpLine.GetDistanceFromPoint(mousePoint);
			nSelect = RS_Y;
		}
	}

	for (size_t i = 0 ; i < m_vtZLines.size() - 1 ; ++i)
	{
		tmpLine.setLine(m_vtZLines[i], m_vtZLines[i+1]);
		if (fMinDis > tmpLine.GetDistanceFromPoint(mousePoint))
		{
			m_v2SelectPoint = tmpLine.GetClosestPoint(mousePoint);
			fMinDis = tmpLine.GetDistanceFromPoint(mousePoint);
			nSelect = RS_Z;
		}
	}
	if (fMinDis > 1)
	{
		nSelect = RS_NONE;
	}
	
	//if (m_nRSelection == RS_NONE)
	{
		SetArcSelected(nSelect);
	}
}

void RotationRenderable::OnLButtonDown(int x, int y)
{
	m_v2BeginClickPoint.x = x;
	m_v2BeginClickPoint.y = y;
}

void RotationRenderable::Caculate2DArc(CCamera* pCamera)
{
	
	//圆心位置
	const Vector3Df& centerPos = m_pNode->GetPosition();
	const Vector3Df& cameraPos = pCamera->GetPosition();
	Vector3Df v3Lookdir = (cameraPos - centerPos);
	float len = v3Lookdir.GetLength();
	float scale = m_fRadius * len / 1000;
	int x,y;
	pCamera->Translate3Dto2D(centerPos, x, y);
	m_v2Center.x = x;
	m_v2Center.y = y;
	Vector2Df v2Temp;
	//计算绕x 轴的圆
	Vector3Df localPos;
	Vector3Df worldPos;
	int screenX;
	int screenY;
	switch(m_transformSpace)
	{
	case CSceneNode::TS_PARENT:
		CSceneNode* pParent = m_pNode->GetParent();
		if (pParent)
		{	
			//分别计算x y z 轴
			//计算x轴
			localPos.x = scale;
			localPos.y = 0;
			localPos.z = 0;
			worldPos = localPos + centerPos;
			worldPos = pParent->ConvertLocalToWorldPosition(worldPos);
			pCamera->Translate3Dto2D(worldPos, screenX, screenY);
			m_v2XEnd.x = screenX;
			m_v2XEnd.y = screenY;
			//计算y轴
			localPos.x = 0;
			localPos.y = scale;
			localPos.z = 0;
			worldPos = localPos + centerPos;
			worldPos = pParent->ConvertLocalToWorldPosition(worldPos);
			pCamera->Translate3Dto2D(worldPos, screenX, screenY);
			m_v2YEnd.x = screenX;
			m_v2YEnd.y = screenY;
			//计算z轴
			localPos.x = 0;
			localPos.y = 0;
			localPos.z = scale ;
			worldPos = localPos + centerPos;
			worldPos = pParent->ConvertLocalToWorldPosition(worldPos);
			pCamera->Translate3Dto2D(worldPos, screenX, screenY);
			m_v2ZEnd.x = screenX;
			m_v2ZEnd.y = screenY;


			//计算绕x轴的半圆弧
			Vector3Df v3XTmp = v3Lookdir;
			v3XTmp.x = 0.0f;
			v3XTmp.Normalize();
			if (fabsf(v3XTmp.y) < 0.001 && fabsf(v3XTmp.z) < 0.001)
			{
				v3XTmp.y = 1.0f;
				v3XTmp.z = 0.0f;
			}
			Vector3Df v3Tmp ;
			v3Tmp.x = 0.0f;
			v3Tmp.y = 0.0f;
			v3Tmp.z = 1.0f;
			Quaternion qRotate;
			qRotate.FromAngleAxis( AIR::PI *0.5, Vector3Df::UNIT_X );
			v3XTmp = qRotate * v3XTmp;
			float fQ = CMathUtil::ACos(v3XTmp.DotProduct(v3Tmp));
			float fSg = AIR::PI / NUM_SEGMENT;
			for (int i = 0 ; i <= NUM_SEGMENT ; i++)
			{
				localPos.x = 0;
				localPos.y = scale * sin(fQ + fSg * i);
				localPos.z = scale * cos(fQ + fSg * i);
				worldPos = localPos + centerPos;
				worldPos = pParent->ConvertLocalToWorldPosition(worldPos);
				pCamera->Translate3Dto2D(worldPos, screenX, screenY);
				m_vtXLines[i].x = screenX;
				m_vtXLines[i].y = screenY;
			}

			//计算绕Y轴半圆弧
			Vector3Df v3YTmp = v3Lookdir;
			v3YTmp.y = 0.0f;
			v3YTmp.Normalize();
			if (fabsf(v3YTmp.x) < 0.001 && fabsf(v3YTmp.z) < 0.001)
			{
				v3YTmp.x = 1.0f;
				v3YTmp.z = 0.0f;
			}
			v3Tmp.x = 1.0f;
			v3Tmp.y = 0.0f;
			v3Tmp.z = 0.0f;
			qRotate.FromAngleAxis( AIR::PI *0.5, Vector3Df::UNIT_Y );
			v3YTmp = qRotate * v3YTmp;
			fQ = CMathUtil::ACos(v3YTmp.DotProduct(v3Tmp));
			fSg = AIR::PI / NUM_SEGMENT;
			for (int i = 0 ; i <= NUM_SEGMENT ; i++)
			{
				localPos.x = scale * cos(fQ + fSg * i);
				localPos.y = 0;
				localPos.z = scale * sin(fQ + fSg * i);
				worldPos = localPos + centerPos;
				worldPos = pParent->ConvertLocalToWorldPosition(worldPos);
				pCamera->Translate3Dto2D(worldPos, screenX, screenY);
				m_vtYLines[i].x = screenX;
				m_vtYLines[i].y = screenY;
			}

			//计算绕Z轴半圆弧
			Vector3Df v3ZTmp = v3Lookdir;
			v3ZTmp.z = 0.0f;
			v3ZTmp.Normalize();
			if (fabsf(v3ZTmp.x) < 0.001 && fabsf(v3ZTmp.y) < 0.001)
			{
				v3ZTmp.x = 0.0f;
				v3ZTmp.y = 1.0f;
			}
			v3Tmp.x = 1.0f;
			v3Tmp.y = 0.0f;
			v3Tmp.z = 0.0f;
			qRotate.FromAngleAxis( -AIR::PI *0.5, Vector3Df::UNIT_Z);
			v3ZTmp = qRotate * v3ZTmp;
			fQ = CMathUtil::ACos(v3ZTmp.DotProduct(v3Tmp));
			fSg = AIR::PI / NUM_SEGMENT;
			for (int i = 0 ; i <= NUM_SEGMENT ; i++)
			{
				localPos.x = scale * cos(fQ + fSg * i);
				localPos.y = scale * sin(fQ + fSg * i);
				localPos.z = 0;
				worldPos = localPos + centerPos;
				worldPos = pParent->ConvertLocalToWorldPosition(worldPos);
				pCamera->Translate3Dto2D(worldPos, screenX, screenY);
				m_vtZLines[i].x = screenX;
				m_vtZLines[i].y = screenY;
			}

			//计算黑色背景圆
			Vector3Df v3D0 = v3Lookdir.Perpendicular();
			Vector3Df v3D1 = (v3Lookdir.CrossProduct(v3D0)).Normalize();
			float fk = v3D0.DotProduct(v3D1);
			float fBackRadius =(m_vtZLines[0] - m_vtZLines[NUM_SEGMENT]).GetLength() * 0.5;
			Vector2Df v2Center = (m_vtZLines[0] + m_vtZLines[NUM_SEGMENT]) * 0.5;
			float fR = AIR::PI / NUM_SEGMENT;
			for (int i = 0 ; i <= 2*NUM_SEGMENT ; i++)
			{

				localPos = v3D0 * scale * cos(fR * i) + v3D1 * scale * sin(fR * i);

				worldPos = localPos + centerPos;
				worldPos = pParent->ConvertLocalToWorldPosition(worldPos);
				pCamera->Translate3Dto2D(worldPos, screenX, screenY);
				m_vtBackLines[i].x = screenX;
				m_vtBackLines[i].y = screenY;
			}

			//计算旋转是扇形
			AIR::Plane3Df plane;
			Vector3Df n;
			float angle = 0.0f;
			bool bIsRotating = false;
			float fD = 0;
			n.x = 1.0f;
			if (m_v3RotationAngle.x != 0)
			{
				n.x = 1.0f;
				n.y = 0.0f;
				n.z = 0.0f;
				angle = m_v3RotationAngle.x;
				bIsRotating = true;
				fD = centerPos.x;
			}
			else if(m_v3RotationAngle.y != 0)
			{
				n.x = 0.0f;
				n.y = 1.0f;
				n.z = 0.0f;
				angle = m_v3RotationAngle.y;
				bIsRotating = true;
				fD = centerPos.y;
			}
			else if (m_v3RotationAngle.z != 0)
			{
				n.x = 0.0f;
				n.y = 0.0f;
				n.z = 1.0f;
				angle = m_v3RotationAngle.z;
				bIsRotating = true;
				fD = centerPos.z;
			}
			m_nNumT = 0;

			if (fabsf(angle) > 0.01)
			{
				plane.SetPlane(n, -fD);
				Vector3Df v3BeginClickPoint;
				Get3DPoint(pCamera, m_v2BeginClickPoint.x, m_v2BeginClickPoint.y, v3BeginClickPoint, plane);
				Vector3Df init2point = v3BeginClickPoint;
				v3BeginClickPoint = v3BeginClickPoint - centerPos;

				//开始旋转点
				v3BeginClickPoint.Normalize();
				v3BeginClickPoint *= scale;
				v3BeginClickPoint += centerPos;
				worldPos = pParent->ConvertLocalToWorldPosition(v3BeginClickPoint);
				pCamera->Translate3Dto2D(v3BeginClickPoint, screenX, screenY);
				m_v2BeginLine.x = screenX;
				m_v2BeginLine.y = screenY;

				//结束旋转点
				qRotate.FromAngleAxis( -angle, n);
				init2point = init2point - centerPos;
				init2point = qRotate * init2point;
				init2point.Normalize();
				init2point *= scale;
				init2point += centerPos;
				worldPos = pParent->ConvertLocalToWorldPosition(init2point);
				pCamera->Translate3Dto2D(init2point, screenX, screenY);
				m_v2EndLine.x = screenX;
				m_v2EndLine.y = screenY;

				//计算扇形数据
				float offsetAngle = AIR::PI/32;
				int per = fabsf(angle) / offsetAngle;
				m_nNumT = per +1;
				m_fanshapedVertex[0].z = 1.0f;
				m_fanshapedVertex[0].x = m_v2Center.x;
				m_fanshapedVertex[0].y = m_v2Center.y;
				m_fanshapedVertex[0].rhw = 1.0f;
				m_fanshapedVertex[0].color = m_scXColor;
				int sign = angle > 0 ? 1 : -1;
				for (int i = 0 ; i <=  per ; i++)
				{	
					Vector3Df tmp = v3BeginClickPoint - centerPos;
					qRotate.FromAngleAxis( -i * sign * offsetAngle, n);
					tmp = qRotate * tmp;
					tmp.Normalize();
					tmp *= scale;
					tmp += centerPos;
					worldPos = pParent->ConvertLocalToWorldPosition(tmp);
					pCamera->Translate3Dto2D(tmp, screenX, screenY);
					m_fanshapedVertex[i+1].z = 1.0f;
					m_fanshapedVertex[i+1].x = screenX;
					m_fanshapedVertex[i+1].y = screenY;
					m_fanshapedVertex[i+1].rhw = 1.0f;
					m_fanshapedVertex[i+1].color = m_scXColor;
				}
				//写扇形最后一个数据
				Vector3Df tmp = v3BeginClickPoint - centerPos;
				qRotate.FromAngleAxis( -angle, n);
				tmp = qRotate * tmp;
				tmp.Normalize();
				tmp *= scale;
				tmp += centerPos;
				worldPos = pParent->ConvertLocalToWorldPosition(tmp);
				pCamera->Translate3Dto2D(tmp, screenX, screenY);
				m_fanshapedVertex[per+2].z = 1.0f;
				m_fanshapedVertex[per+2].x = screenX;
				m_fanshapedVertex[per+2].y = screenY;
				m_fanshapedVertex[per+2].rhw = 1.0f;
				m_fanshapedVertex[per+2].color = m_scXColor;
			}
		}
		else
		{
			//分别计算x y z 轴
			//计算x轴
			localPos.x = scale;
			localPos.y = 0;
			localPos.z = 0;
			worldPos = localPos + centerPos;
			pCamera->Translate3Dto2D(worldPos, screenX, screenY);
			m_v2XEnd.x = screenX;
			m_v2XEnd.y = screenY;
			//计算y轴
			localPos.x = 0;
			localPos.y = scale;
			localPos.z = 0;
			worldPos = localPos + centerPos;
			pCamera->Translate3Dto2D(worldPos, screenX, screenY);
			m_v2YEnd.x = screenX;
			m_v2YEnd.y = screenY;
			//计算z轴
			localPos.x = 0;
			localPos.y = 0;
			localPos.z = scale ;
			worldPos = localPos + centerPos;
			pCamera->Translate3Dto2D(worldPos, screenX, screenY);
			m_v2ZEnd.x = screenX;
			m_v2ZEnd.y = screenY;


			//计算绕x轴的半圆弧
			Vector3Df v3XTmp = v3Lookdir;
			v3XTmp.x = 0.0f;
			v3XTmp.Normalize();
			if (fabsf(v3XTmp.y) < 0.001 && fabsf(v3XTmp.z) < 0.001)
			{
				v3XTmp.y = 1.0f;
				v3XTmp.z = 0.0f;
			}
			Vector3Df v3Tmp ;
			v3Tmp.x = 0.0f;
			v3Tmp.y = 0.0f;
			v3Tmp.z = 1.0f;
			Quaternion qRotate;
			qRotate.FromAngleAxis( AIR::PI *0.5, Vector3Df::UNIT_X );
			v3XTmp = qRotate * v3XTmp;
			float fQ = CMathUtil::ACos(v3XTmp.DotProduct(v3Tmp));
			float fSg = AIR::PI / NUM_SEGMENT;
			for (int i = 0 ; i <= NUM_SEGMENT ; i++)
			{
				localPos.x = 0;
				localPos.y = scale * sin(fQ + fSg * i);
				localPos.z = scale * cos(fQ + fSg * i);
				worldPos = localPos + centerPos;
				pCamera->Translate3Dto2D(worldPos, screenX, screenY);
				m_vtXLines[i].x = screenX;
				m_vtXLines[i].y = screenY;
			}

			//计算绕Y轴半圆弧
			Vector3Df v3YTmp = v3Lookdir;
			v3YTmp.y = 0.0f;
			v3YTmp.Normalize();
			if (AIR::Abs(v3YTmp.x) < 0.001 && AIR::Abs(v3YTmp.z) < 0.001)
			{
				v3YTmp.x = 1.0f;
				v3YTmp.z = 0.0f;
			}
			v3Tmp.x = 1.0f;
			v3Tmp.y = 0.0f;
			v3Tmp.z = 0.0f;
			qRotate.FromAngleAxis( AIR::PI *0.5, Vector3Df::UNIT_Y );
			v3YTmp = qRotate * v3YTmp;
			fQ = CMathUtil::ACos(v3YTmp.DotProduct(v3Tmp));
			fSg = AIR::PI / NUM_SEGMENT;
			for (int i = 0 ; i <= NUM_SEGMENT ; i++)
			{
				localPos.x = scale * cos(fQ + fSg * i);
				localPos.y = 0;
				localPos.z = scale * sin(fQ + fSg * i);
				worldPos = localPos + centerPos;
				pCamera->Translate3Dto2D(worldPos, screenX, screenY);
				m_vtYLines[i].x = screenX;
				m_vtYLines[i].y = screenY;
			}

			//计算绕Z轴半圆弧
			Vector3Df v3ZTmp = v3Lookdir;
			v3ZTmp.z = 0.0f;
			v3ZTmp.Normalize();
			if (AIR::Abs(v3ZTmp.x) < 0.001 && AIR::Abs(v3ZTmp.y) < 0.001)
			{
				v3ZTmp.x = 0.0f;
				v3ZTmp.y = 1.0f;
			}
			v3Tmp.x = 1.0f;
			v3Tmp.y = 0.0f;
			v3Tmp.z = 0.0f;
			qRotate.FromAngleAxis( -AIR::PI *0.5, Vector3Df::UNIT_Z);
			v3ZTmp = qRotate * v3ZTmp;
			fQ = CMathUtil::ACos(v3ZTmp.DotProduct(v3Tmp));
			fSg = AIR::PI / NUM_SEGMENT;
			for (int i = 0 ; i <= NUM_SEGMENT ; i++)
			{
				localPos.x = scale * cos(fQ + fSg * i);
				localPos.y = scale * sin(fQ + fSg * i);
				localPos.z = 0;
				worldPos = localPos + centerPos;
				pCamera->Translate3Dto2D(worldPos, screenX, screenY);
				m_vtZLines[i].x = screenX;
				m_vtZLines[i].y = screenY;
			}

			//计算黑色背景圆
			Vector3Df v3D0 = v3Lookdir.Perpendicular();
			Vector3Df v3D1 = (v3Lookdir.CrossProduct(v3D0)).Normalize();
			float fk = v3D0.DotProduct(v3D1);
			float fBackRadius =(m_vtZLines[0] - m_vtZLines[NUM_SEGMENT]).GetLength() * 0.5;
			Vector2Df v2Center = (m_vtZLines[0] + m_vtZLines[NUM_SEGMENT]) * 0.5;
			float fR = AIR::PI / NUM_SEGMENT;
			for (int i = 0 ; i <= 2*NUM_SEGMENT ; i++)
			{

				localPos = v3D0 * scale * cos(fR * i) + v3D1 * scale * sin(fR * i);
				
				worldPos = localPos + centerPos;
				pCamera->Translate3Dto2D(worldPos, screenX, screenY);
				m_vtBackLines[i].x = screenX;
				m_vtBackLines[i].y = screenY;
			}
			
			//计算旋转是扇形
			AIR::Plane3Df plane;
			Vector3Df n;
			float angle = 0.0f;
			bool bIsRotating = false;
			float fD = 0;
			n.x = 1.0f;
			if (m_v3RotationAngle.x != 0)
			{
				n.x = 1.0f;
				n.y = 0.0f;
				n.z = 0.0f;
				angle = m_v3RotationAngle.x;
				bIsRotating = true;
				fD = centerPos.x;
			}
			else if(m_v3RotationAngle.y != 0)
			{
				n.x = 0.0f;
				n.y = 1.0f;
				n.z = 0.0f;
				angle = m_v3RotationAngle.y;
				bIsRotating = true;
				fD = centerPos.y;
			}
			else if (m_v3RotationAngle.z != 0)
			{
				n.x = 0.0f;
				n.y = 0.0f;
				n.z = 1.0f;
				angle = m_v3RotationAngle.z;
				bIsRotating = true;
				fD = centerPos.z;
			}
			m_nNumT = 0;

			if (AIR::Abs(angle) > 0.01)
			{
				plane.SetPlane(n, -fD);
				Vector3Df v3BeginClickPoint;
				Get3DPoint(pCamera, m_v2BeginClickPoint.x, m_v2BeginClickPoint.y, v3BeginClickPoint, plane);
				Vector3Df init2point = v3BeginClickPoint;
				v3BeginClickPoint = v3BeginClickPoint - centerPos;
				
				//开始旋转点
				v3BeginClickPoint.Normalize();
				v3BeginClickPoint *= scale;
				v3BeginClickPoint += centerPos;
				pCamera->Translate3Dto2D(v3BeginClickPoint, screenX, screenY);
				m_v2BeginLine.x = screenX;
				m_v2BeginLine.y = screenY;

				//结束旋转点
				qRotate.FromAngleAxis( -angle, n);
				init2point = init2point - centerPos;
				init2point = qRotate * init2point;
				init2point.Normalize();
				init2point *= scale;
				init2point += centerPos;
				pCamera->Translate3Dto2D(init2point, screenX, screenY);
				m_v2EndLine.x = screenX;
				m_v2EndLine.y = screenY;
				
				//计算扇形数据
				float offsetAngle = AIR::PI/32;
				int per = AIR::Abs(angle) / offsetAngle;
				m_nNumT = per +1;
				m_fanshapedVertex[0].z = 1.0f;
				m_fanshapedVertex[0].x = m_v2Center.x;
				m_fanshapedVertex[0].y = m_v2Center.y;
				m_fanshapedVertex[0].rhw = 1.0f;
				m_fanshapedVertex[0].color = m_scXColor;
				int sign = angle > 0 ? 1 : -1;
				for (int i = 0 ; i <=  per ; i++)
				{	
					Vector3Df tmp = v3BeginClickPoint - centerPos;
					qRotate.FromAngleAxis( -i * sign * offsetAngle, n);
					tmp = qRotate * tmp;
					tmp.Normalize();
					tmp *= scale;
					tmp += centerPos;
					pCamera->Translate3Dto2D(tmp, screenX, screenY);
					m_fanshapedVertex[i+1].z = 1.0f;
					m_fanshapedVertex[i+1].x = screenX;
					m_fanshapedVertex[i+1].y = screenY;
					m_fanshapedVertex[i+1].rhw = 1.0f;
					m_fanshapedVertex[i+1].color = m_scXColor;
				}
				//写扇形最后一个数据
				Vector3Df tmp = v3BeginClickPoint - centerPos;
				qRotate.FromAngleAxis( -angle, n);
				tmp = qRotate * tmp;
				tmp.Normalize();
				tmp *= scale;
				tmp += centerPos;
				pCamera->Translate3Dto2D(tmp, screenX, screenY);
				m_fanshapedVertex[per+2].z = 1.0f;
				m_fanshapedVertex[per+2].x = screenX;
				m_fanshapedVertex[per+2].y = screenY;
				m_fanshapedVertex[per+2].rhw = 1.0f;
				m_fanshapedVertex[per+2].color = m_scXColor;
			}
		}
	}
}

void RotationRenderable::SetArcSelected(RotationSelection arcSelection)
{
	m_nRSelection = arcSelection;
	switch (m_nRSelection)
	{
	case RS_NONE:
		m_scXColor = SColor(255, 255, 0, 0);
		m_scYColor = SColor(255, 0, 255, 0);
		m_scZColor = SColor(255, 0, 0, 255);
		break;
	case RS_X:
		m_scXColor = SColor(255, 255, 255, 0);
		m_scYColor = SColor(255, 0, 255, 0);
		m_scZColor = SColor(255, 0, 0, 255);
		break;
	case RS_Y:
		m_scXColor = SColor(255, 255, 0, 0);
		m_scYColor = SColor(255, 255, 255, 0);
		m_scZColor = SColor(255, 0, 0, 255);
		break;
	case RS_Z:
		m_scXColor = SColor(255, 255, 0, 0);
		m_scYColor = SColor(255, 0, 255, 0);
		m_scZColor = SColor(255, 255, 255, 0);
		break;
	}

}

RotationSelection RotationRenderable::GetArcSelected()
{
	return m_nRSelection;
}


//绕x轴
void RotationRenderable::AddUpAngleX(float fAngle)
{
	if (fAngle != 0)
	{
		m_v3RotationAngle.x += fAngle;
	}
	while(m_v3RotationAngle.x > 2 * AIR::PI)
	{
		m_v3RotationAngle.x -=  2*AIR::PI;
	}
}
//绕y轴
void RotationRenderable::AddUpAngleY(float fAngle)
{
	m_v3RotationAngle.y += fAngle;
	while(m_v3RotationAngle.y > 2 * AIR::PI)
	{
		m_v3RotationAngle.y -=  2*AIR::PI;
	}
}
//绕z轴
void RotationRenderable::AddUpAngleZ(float fAngle)
{
	m_v3RotationAngle.z += fAngle;
	while(m_v3RotationAngle.z > 2 * AIR::PI)
	{
		m_v3RotationAngle.z -=  2*AIR::PI;
	}
}

//重置
void RotationRenderable::ResetAngles()
{
	m_v3RotationAngle.x = 0.0f;
	m_v3RotationAngle.y = 0.0f;
	m_v3RotationAngle.z = 0.0f;
}

void RotationRenderable::PreRender(CCamera* pCamera)
{
	_GetPrimitive();
}

void RotationRenderable::_GetPrimitive()
{
	if (!m_pPrimitive)
	{
		m_pPrimitive = new Primitive(Primitive::Primitive_TriangleFan/*, m_pVertexDecl*/);
		m_pPrimitive->m_nVerticesStride = sizeof(RHWVERTEXNOTEX);
		m_pPrimitive->m_pSrcRenderable = this;
		m_pPrimitive->SetUseUserPrimitive(true);
	}
	m_pPrimitive->m_pVertexData = &m_fanshapedVertex[0];
	m_pPrimitive->m_nVerticesStride = sizeof(RHWVERTEXNOTEX);
	m_pPrimitive->SetPrimitiveCount(m_nNumT);
}

void RotationRenderable::RebuildRenderData()
{

	////圆心位置
	//const Vector3Df& centerPos = m_pNode->GetPosition();
	//m_v2SelectPoint
	//if (m_v3RotationAngle.x != 0 )
	//{
	//	
	//}
	//else if(m_v3RotationAngle.y !=0 )
	//{

	//}
	//else if (m_v3RotationAngle.z !=0 )
	//{
	//}
}

void RotationRenderable::Get3DPoint(CCamera *pCamera, int x, int y, AIR::Vector3Df &intersection, const AIR::Plane3Df &plane)
{
	Vector3Df start, end;
	pCamera->GetRay(x, y, start, end);
	plane.GetIntersectionWithLine(start, end-start, intersection);
}
void RotationRenderable::RenderRotation(CCamera* pCamera)
{
	
	if (m_pNode)
	{
		Caculate2DArc(pCamera);
		IRenderer* pRenderer = ENGINE_INST->GetRenderer();
		pRenderer->SetZWriteEnable(false);
		pRenderer->SetDepthEnable(false);
		
		//渲染三个坐标轴
		//x 轴
		pRenderer->Draw2DLine(m_v2Center.x, m_v2Center.y, m_v2XEnd.x, m_v2XEnd.y, 2.0f, m_scXColor);
		//y 轴
		pRenderer->Draw2DLine(m_v2Center.x, m_v2Center.y, m_v2YEnd.x, m_v2YEnd.y, 2.0f, m_scYColor);
		//z 轴
		pRenderer->Draw2DLine(m_v2Center.x, m_v2Center.y, m_v2ZEnd.x, m_v2ZEnd.y, 2.0f, m_scZColor);

		//渲染背景圆
		for (size_t i = 0 ; i < m_vtBackLines.size() - 1 ; ++i)
		{
			Vector2Df point1 = m_vtBackLines[i];
			Vector2Df point2 = m_vtBackLines[i+1];
			pRenderer->Draw2DLine(point1.x, point1.y, point2.x, point2.y, 1.0f, m_scBackColor);
		}

		//渲染三个半圆弧
		for (size_t i = 0 ; i < m_vtXLines.size() - 1 ; ++i)
		{
			Vector2Df point1 = m_vtXLines[i];
			Vector2Df point2 = m_vtXLines[i+1];
			pRenderer->Draw2DLine(point1.x, point1.y, point2.x, point2.y, 2.0f, m_scXColor);
		}
		for (size_t i = 0 ; i < m_vtYLines.size() - 1 ; ++i)
		{
			Vector2Df point1 = m_vtYLines[i];
			Vector2Df point2 = m_vtYLines[i+1];
			pRenderer->Draw2DLine(point1.x, point1.y, point2.x, point2.y, 2.0f, m_scYColor);
		}
		for (size_t i = 0 ; i < m_vtZLines.size() - 1 ; ++i)
		{
			Vector2Df point1 = m_vtZLines[i];
			Vector2Df point2 = m_vtZLines[i+1];
			pRenderer->Draw2DLine(point1.x, point1.y, point2.x, point2.y, 2.0f, m_scZColor);
		}

		//渲染当前正在旋转的扇形
		if (m_nNumT !=  0)
		{
			pRenderer->Draw2DLine(m_v2Center.x, m_v2Center.y, m_v2BeginLine.x, m_v2BeginLine.y, 2.0f, m_scZColor);
			pRenderer->Draw2DLine(m_v2Center.x, m_v2Center.y, m_v2EndLine.x, m_v2EndLine.y, 2.0f, m_scZColor);

			pRenderer->SetFillMode(false);
			pRenderer->SetCullMode(CM_None);
			PreRender(pCamera);
			_GetPrimitive();
            pRenderer->SetVertexDeclaration(m_pVertexDecl);
			//m_pPrimitive->Draw(NULL);
		}

		
		//int nXAngle = m_v3RotationAngle.x * 180 / AIR::PI;
		//int nYAngle = m_v3RotationAngle.y * 180 / AIR::PI;
		//int nZAngle = m_v3RotationAngle.z * 180 / AIR::PI;
		////ENGINE_INST->GetFPSCounter()->AddTextInfo(
		////	GetString("(%d, %d, %d)", nXAngle, nYAngle, nZAngle),
		////	m_v2InfoPos.x, m_v2InfoPos.y,
		////	m_scBackColor);
		//ENGINE_INST->GetFPSCounter()->AddTextInfo(
		//	GetString("(%.1f, %.1f, %.1f, %.1f)", m_v3RotationAngle.x, m_v3RotationAngle.y, m_v3RotationAngle.z, 0.1f),
		//	m_v2InfoPos.x, m_v2InfoPos.y,
		//	m_scBackColor);
	
		
		/**/
		
	}

	Renderable::AfterRender();

}