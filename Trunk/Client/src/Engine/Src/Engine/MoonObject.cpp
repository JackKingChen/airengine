#include "stdafx.h"
#include "MoonObject.h"
#include "IRenderer.h"
#include "Camera.h"
#include "WeatherManager.h"
#include "Billboard.h"
#include "Engine.h"
#include "Atmosphere.h"
#include "SceneNode.h"
#include "ISceneManager.h"

MoonObject::MoonObject(WeatherManager *s)
: mSkyX(s)
, mMoonBillboard(0)
, mCreated(false)
, mMoonSize(0.225f)
, mMoonRelDist(0.8f)
{
}

MoonObject::~MoonObject()
{

}

void MoonObject::Create()
{
	if (mCreated)
	{
		return;
	}

	ISceneManager* pSceneMgr = ENGINE_INST->GetSceneMgr();

	mMoonBillboard = pSceneMgr->CreateRenderable<CBillboard>();
	mMoonBillboard->SetName( "MoonObject" );
	mMoonBillboard->SetComColor( SColor(0xffffffff) );
	mMoonBillboard->SetUseComColor( true );
	mMoonBillboard->SetBillBoardCreateType( eBBCT_Center );
	mMoonBillboard->SetBillBoardFaceType( eBBFT_AlwaysFaceCamera );
	mMoonBillboard->SetBBSize( mMoonSize, mMoonSize );
	mMoonBillboard->SetUseRenderer( false );


	CSceneNode* pMoonNode = pSceneMgr->CreateSceneNode( "MoonObjectNode", true, mSkyX->GetCamera()->GetPosition() );
	pMoonNode->AttachRenderable( mMoonBillboard );

	IMaterial* pMat = MATERIAL_MANAGER->CreateMaterial( "WeatherSystemMaterial.xml/MoonObject" );
	mMoonBillboard->SetMaterial( pMat );
	if ( !pMat->IsReady() )
	{
		if ( pMat->Cache() )
		{
			//pMat->SetCurrentTechnique( "MoonObject" );
		}		
	}

	mCreated = true;

	_UpdateMoonBounds();
	Update();
}

void MoonObject::Update()
{
	if (!mCreated)
	{
		return;
	}

	if ( !mMoonBillboard->GetMaterial()->IsReady() )
	{
		if ( mMoonBillboard->GetMaterial()->Cache() )
		{
			//mMoonBillboard->GetMaterial()->SetCurrentTechnique( "MoonObject" );
		}		
	}

	float radius = mSkyX->GetCamera()->GetViewFar()*mMoonRelDist,
		size = radius*mMoonSize;

	AIR::Vector3Df moonRelativePos = mSkyX->GetAtmosphereManager()->GetSunDirection()*
		AIR::asm_cos(AIR::asm_arcsin((size/2)/radius)) * radius * mMoonRelDist;

	mMoonBillboard->GetNodeParent()->SetPosition(mSkyX->GetCamera()->GetPosition() + moonRelativePos);

	if (moonRelativePos.y < -size/2)
	{
		mMoonBillboard->SetVisable( false );
	}
	else
	{
		mMoonBillboard->SetVisable( true );
	}
}


void MoonObject::Clear()
{
	CSceneNode* pMoonNode = mMoonBillboard->GetNodeParent();
	mSkyX->GetSceneManager()->DestroySceneNode( pMoonNode );
	mSkyX->GetSceneManager()->DestroyRenderable( mMoonBillboard );
}

void MoonObject::SetMoonSize(f32 MoonSize)
{
	mMoonSize = MoonSize;
	if (!mCreated)
	{
		return;
	}
	_UpdateMoonBounds();
}

void MoonObject::SetMoonRelDist(f32 relDist)
{
	mMoonRelDist = relDist;
	if (!mCreated)
	{
		return;
	}
	_UpdateMoonBounds();
}

void MoonObject::_UpdateMoonBounds()
{
	float radius = mSkyX->GetCamera()->GetViewFar() * mMoonRelDist,
		size = radius*mMoonSize;
	mMoonBillboard->SetBBSize( size, size );

}