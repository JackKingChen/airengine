#########################################################
# Copyright (C) 2013, hjj. All rights reserved.
# airengine static lib
#########################################################

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := engine_static

LOCAL_MODULE_FILENAME := libengine

# math module source files 
LIB_MATH_MODULE_SRC := \
	Src/Math/Quaternion.cpp
	
# engine module source files
LIB_ENGINE_MODULE_SRC := \
	Src/Engine/AIRAnimation.cpp \
	Src/Engine/AIRAnimationBlend.cpp \
	Src/Engine/AIRAnimationMixer.cpp \
	Src/Engine/AIRAnimationMixRes.cpp \
	Src/Engine/AIRAnimationState.cpp \
	Src/Engine/AIRBone.cpp \
	Src/Engine/AIRBoneInstance.cpp \
	Src/Engine/AIRImage.cpp \
	Src/Engine/AIRKeyframeAnimationRes.cpp \
	Src/Engine/AIRModel.cpp \
	Src/Engine/AIRSkeleton.cpp \
	Src/Engine/AIRSkeletonAnimation.cpp \
	Src/Engine/AIRSkeletonRes.cpp \
	Src/Engine/AIRSubMesh.cpp \
	Src/Engine/AlphaSegmentAffector.cpp \
	Src/Engine/AnimationTrack.cpp \
	Src/Engine/AreaEmitter.cpp \
	Src/Engine/Atmosphere.cpp \
	Src/Engine/AxisRotateAffector.cpp \
	Src/Engine/BaseShape.cpp \
	Src/Engine/Billboard.cpp \
	Src/Engine/BillBoardChain.cpp \
	Src/Engine/BillboardComposer.cpp \
	Src/Engine/BillboardRenderer.cpp \
	Src/Engine/BillboardSet.cpp \
	Src/Engine/BillboardSetManager.cpp \
	Src/Engine/BoundingBoxEntity.cpp \
	Src/Engine/BoxCollider.cpp \
	Src/Engine/BoxEmitter.cpp \
	Src/Engine/BoxRenderable.cpp \
	Src/Engine/BoxShape.cpp \
	Src/Engine/Camera.cpp \
	Src/Engine/Camera3DArcBall.cpp \
	Src/Engine/CfgReader.cpp \
	Src/Engine/Circle3DRenderable.cpp \
	Src/Engine/CloudsManager.cpp \
	Src/Engine/Collision.cpp \
	Src/Engine/ColorAffector.cpp \
	Src/Engine/ColorGradient.cpp \
	Src/Engine/ColorImageAffector.cpp \
	Src/Engine/ColorSegmentAffector.cpp \
	Src/Engine/ColorValue.cpp \
	Src/Engine/Console.cpp \
	Src/Engine/ConsoleWindow.cpp \
	Src/Engine/CSoundSystem.cpp \
	Src/Engine/CylinderEmitter.cpp \
	Src/Engine/DataManager.cpp \
	Src/Engine/DataStream.cpp \
	Src/Engine/DebugNew.cpp \
	Src/Engine/DebugTools.cpp \
	Src/Engine/DecalManager.cpp \
	Src/Engine/DeflectorPlaneAffector.cpp \
	Src/Engine/DirectionRandomiserAffector.cpp \
	Src/Engine/DirectionRenderable.cpp \
	Src/Engine/ElectroLightening.cpp \
	Src/Engine/Ellipsoid.cpp \
	Src/Engine/EllipsoidEmitter.cpp \
	Src/Engine/Engine.cpp \
	Src/Engine/EngineDefination.cpp \
	Src/Engine/EngineResource.cpp \
	Src/Engine/EngineStringConverter.cpp \
	Src/Engine/EventListener.cpp \
	Src/Engine/EventReader.cpp \
	Src/Engine/FastFakeRandom.cpp \
	Src/Engine/FileArchive.cpp \
	Src/Engine/FileData.cpp \
	Src/Engine/FileDataManager.cpp \
	Src/Engine/FileDataStream.cpp \
	Src/Engine/FileSystem.cpp \
	Src/Engine/FlareSpot.cpp \
	Src/Engine/FolderInfo.cpp \
	Src/Engine/FontManager.cpp \
	Src/Engine/FPSCounter.cpp \
	Src/Engine/FrameController.cpp \
	Src/Engine/Frustum.cpp \
	Src/Engine/FTFont.cpp \
	Src/Engine/FullScaleSegmentAffector.cpp \
	Src/Engine/FullScreenQuad.cpp \
	Src/Engine/GDIFont.cpp \
	Src/Engine/GEffect.cpp \
	Src/Engine/GeometryBlock.cpp \
	Src/Engine/GeometryManager.cpp \
	Src/Engine/GPUDataParam.cpp \
	Src/Engine/GPUParameters.cpp \
	Src/Engine/GPUProgramManager.cpp \
	Src/Engine/GroundObject.cpp \
	Src/Engine/HardwareInfoSystem.cpp \
	Src/Engine/HeightMap.cpp \
	Src/Engine/IGPUProgram.cpp \
	Src/Engine/ImageOperator.cpp \
	Src/Engine/ImageSet.cpp \
	Src/Engine/IMaterial.cpp \
	Src/Engine/InstancingObjectManager.cpp \
	Src/Engine/IRenderable.cpp \
	Src/Engine/IRenderer.cpp \
	Src/Engine/ISceneManager.cpp \
	Src/Engine/IScriptSystem.cpp \
	Src/Engine/ISound.cpp \
	Src/Engine/ITexture.cpp \
	Src/Engine/LensFlare.cpp \
	Src/Engine/Light.cpp \
	Src/Engine/LightingManager.cpp \
	Src/Engine/LinearForceAffector.cpp \
	Src/Engine/LineObject.cpp \
	Src/Engine/ManualGPUDataParamSource.cpp \
	Src/Engine/ManualParticleSystem.cpp \
	Src/Engine/MarkupSTL.cpp \
	Src/Engine/MaterialManager.cpp \
	Src/Engine/MemoryDataStream.cpp \
	Src/Engine/MemoryNedAlloc.cpp \
	Src/Engine/MeshInstance.cpp \
	Src/Engine/MeshInstancingObject.cpp \
	Src/Engine/ModelInstance.cpp \
	Src/Engine/MoonObject.cpp \
	Src/Engine/NavEditor.cpp \
	Src/Engine/Navigater.cpp \
	Src/Engine/NavPathFinder.cpp \
	Src/Engine/NodeAnimation.cpp \
	Src/Engine/Obstruct.cpp \
	Src/Engine/Octree.cpp \
	Src/Engine/OctreeNode.cpp \
	Src/Engine/OTSceneManager.cpp \
	Src/Engine/PageStaticGeometory.cpp \
	Src/Engine/Particle.cpp \
	Src/Engine/ParticleAffectorFactory.cpp \
	Src/Engine/ParticleEmitter.cpp \
	Src/Engine/ParticleEmitterFactory.cpp \
	Src/Engine/ParticleIterator.cpp \
	Src/Engine/ParticleSystem.cpp \
	Src/Engine/ParticleSystemManager.cpp \
	Src/Engine/ParticleSystemResource.cpp \
	Src/Engine/Pass.cpp \
	Src/Engine/PerlinNoise.cpp \
	Src/Engine/PluginDLL.cpp \
	Src/Engine/PointAttractAffector.cpp \
	Src/Engine/PointEmitter.cpp \
	Src/Engine/PostProcessManager.cpp \
	Src/Engine/PostScreenCompositor.cpp \
	Src/Engine/Primitive.cpp \
	Src/Engine/ProgressiveMesh.cpp \
	Src/Engine/QuadTree.cpp \
	Src/Engine/QuadTreeNode.cpp \
	Src/Engine/QuadTreeSceneManager.cpp \
	Src/Engine/RenderableFactory.cpp \
	Src/Engine/RenderControl.cpp \
	Src/Engine/RenderData.cpp \
	Src/Engine/RenderQueue.cpp \
	Src/Engine/RenderQueueGroup.cpp \
	Src/Engine/RenderTarget.cpp \
	Src/Engine/RenderTargetDepthMap.cpp \
	Src/Engine/RenderTargetGBuffer.cpp \
	Src/Engine/RenderTargetLightBuffer.cpp \
	Src/Engine/RenderTargetReflect.cpp \
	Src/Engine/RenderTargetRefract.cpp \
	Src/Engine/RenderTargetScene.cpp \
	Src/Engine/RenderTargetScreenCompositor.cpp \
	Src/Engine/RibbonTrail.cpp \
	Src/Engine/RingEmitter.cpp \
	Src/Engine/RotationAffector.cpp \
	Src/Engine/RotationRenderable .cpp \
	Src/Engine/ScaleAffector.cpp \
	Src/Engine/ScaleSegmentAffector.cpp \
	Src/Engine/SceneDebug.cpp \
	Src/Engine/SceneNode.cpp \
	Src/Engine/ScreenCompositor.cpp \
	Src/Engine/SEFps.cpp \
	Src/Engine/ShadowManager.cpp \
	Src/Engine/Shadowmap.cpp \
	Src/Engine/Skybox.cpp \
	Src/Engine/SkyDome.cpp \
	Src/Engine/Spline.cpp \
	Src/Engine/stdafx.cpp \
	Src/Engine/StringUtil.cpp \
	Src/Engine/SunLight.cpp \
	Src/Engine/Technique.cpp \
	Src/Engine/TechniqueSelector.cpp \
	Src/Engine/Terrain.cpp \
	Src/Engine/TerrainBlock.cpp \
	Src/Engine/TerrainDecal.cpp \
	Src/Engine/TerrainDetailLayer.cpp \
	Src/Engine/TerrainLayer.cpp \
	Src/Engine/TerrainQuadTree.cpp \
	Src/Engine/TerrainQuadTreeNode.cpp \
	Src/Engine/TextureAnimationTrack.cpp \
	Src/Engine/TextureMgr.cpp \
	Src/Engine/TextureRenderGroup.cpp \
	Src/Engine/TextureUnit.cpp \
	Src/Engine/ThirdPersonCamera.cpp \
	Src/Engine/Thread.cpp \
	Src/Engine/TimeBaseProfile.cpp \
	Src/Engine/Timer.cpp \
	Src/Engine/VClouds.cpp \
	Src/Engine/VCloudsManager.cpp \
	Src/Engine/VertexAnimation.cpp \
	Src/Engine/VertexDeclaration.cpp \
	Src/Engine/VertexDeclarationManager.cpp \
	Src/Engine/Water.cpp \
	Src/Engine/WeatherManager.cpp \
	Src/Engine/WeatherManualGPUParamSource.cpp \
	Src/Engine/WeatherTechniqueSelecter.cpp \
	Src/Engine/XMLFileData.cpp \
	Src/Engine/ZipArchive.cpp \
	Src/Engine/ZipDataStream.cpp

# gui module source files
LIB_AIRGUI_MODULE_SRC := \
	Src/AIRGUI/AIRGUI.cpp \
	Src/AIRGUI/CIME.cpp \
	Src/AIRGUI/GUIAnimation.cpp \
	Src/AIRGUI/GUIAutoSizeFrame.cpp \
	Src/AIRGUI/GUIBuilder.cpp \
	Src/AIRGUI/GUIBuilderMgr.cpp \
	Src/AIRGUI/GUIButton.cpp \
	Src/AIRGUI/GUICheckBox.cpp \
	Src/AIRGUI/GUICombobox.cpp \
	Src/AIRGUI/GUIDragImage.cpp \
	Src/AIRGUI/GUIEditBox.cpp \
	Src/AIRGUI/GUIEditCtrl.cpp \
	Src/AIRGUI/GUIElement.cpp \
	Src/AIRGUI/GUIElementIterator.cpp \
	Src/AIRGUI/GUIEntityList.cpp \
	Src/AIRGUI/GUIFactory.cpp \
	Src/AIRGUI/GUIHyperLink.cpp \
	Src/AIRGUI/GUIImage.cpp \
	Src/AIRGUI/GUIListCtrl.cpp \
	Src/AIRGUI/GUIListItem.cpp \
	Src/AIRGUI/GUIMenu.cpp \
	Src/AIRGUI/GUIProgressBar.cpp \
	Src/AIRGUI/GUIRenderer.cpp \
	Src/AIRGUI/GUIRenderLine.cpp \
	Src/AIRGUI/GUISchemeManager.cpp \
	Src/AIRGUI/GUIScriptObject.cpp \
	Src/AIRGUI/GUIScrollBar.cpp \
	Src/AIRGUI/GUISlider.cpp \
	Src/AIRGUI/GUIStaticText.cpp \
	Src/AIRGUI/GUISystem.cpp \
	Src/AIRGUI/GUITabCtrl.cpp \
	Src/AIRGUI/GUITemplateManager.cpp \
	Src/AIRGUI/GUITreeCtrl.cpp \
	Src/AIRGUI/GUITreeItem.cpp \
	Src/AIRGUI/GUIWindow.cpp \
	Src/AIRGUI/GUIWord.cpp \
	Src/AIRGUI/HyperLinkManager.cpp \
	Src/AIRGUI/IGUIFormater.cpp \
	Src/AIRGUI/ImageSetManager.cpp \
	Src/AIRGUI/stdafx.cpp \
	Src/AIRGUI/UILayer.cpp

# all module source for compile 
LOCAL_SRC_FILES := \
	$(LIB_MATH_MODULE_SRC) \
	$(LIB_ENGINE_MODULE_SRC) \
	$(LIB_AIRGUI_MODULE_SRC)


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/include/Common \
                    $(LOCAL_PATH)/include/Engine \
					$(LOCAL_PATH)/include/Math


LOCAL_EXPORT_LDLIBS := -llog\
                       -lz \
                       -lGLESv2

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/include/Common \
                    $(LOCAL_PATH)/include/Engine \
					$(LOCAL_PATH)/include/Math

LOCAL_LDLIBS := -lGLESv2 \
                -lEGL \
                -llog \
                -lz 

# define the macro to compile through support/zip_support/ioapi.c                
LOCAL_CFLAGS := -DUSE_FILE32API
LOCAL_EXPORT_CFLAGS := -DUSE_FILE32API

include $(BUILD_STATIC_LIBRARY)

