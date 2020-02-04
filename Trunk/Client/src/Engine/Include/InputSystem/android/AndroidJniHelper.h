
#pragma once

#include "AISPrereqs.h"

#ifdef AIS_ANDROID_PLATFORM

#include <jni.h>
#include <string>

namespace AIS
{

	typedef struct JniMethodInfo_
	{
		JNIEnv *    env;
		jclass      classID;
		jmethodID   methodID;
	} 
	JniMethodInfo;

	class AIR_INPUT_API AndroidJniHelper
	{
	public:
		static JavaVM* getJavaVM();
		static void setJavaVM(JavaVM *javaVM);
		static const char* getExternalAssetPath();
		static void setExternalAssetPath(const char* externalAssetPath);
		static jclass getClassID(const char *className, JNIEnv *env=0);
		static bool getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode);
		static bool getMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode);
		static std::string jstring2string(jstring str);

		static void setAcclerometerInterval( float fInterval );
	
	protected:
		static JavaVM *m_psJavaVM;
		static std::string m_externalAssetPath;
	};

};

typedef void (*EditTextCallback)(const char* pText, void* ctx);

extern "C"
{
	extern const char * getApkPath();
	extern void showDialogJNI(const char * pszMsg, const char * pszTitle);
	extern void showEditTextDialogJNI(const char* pszTitle, const char* pszContent, int nInputMode, int nInputFlag, int nReturnType, int nMaxLength, EditTextCallback pfEditTextCallback, void* ctx);
	extern void terminateProcessJNI();
	extern void enableAccelerometerJNI();
	extern void disableAccelerometerJNI();
	extern void setAccelerometerIntervalJNI(float interval);

	extern JNIEXPORT void JNICALL  Java_com_android_util_AndroidHelper_nativeSensorChanged(JNIEnv*  env, jobject thiz, jfloat x, jfloat y, jfloat z, jlong timeStamp, jint orientation, jint rotate);
	extern JNIEXPORT void JNICALL  Java_com_android_util_AndroidHelper_nativeTouchesBegan(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys);
	extern JNIEXPORT void JNICALL  Java_com_android_util_AndroidHelper_nativeTouchesEnded(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys);
	extern JNIEXPORT void JNICALL  Java_com_android_util_AndroidHelper_nativeTouchesMoved(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys);
	extern JNIEXPORT void JNICALL  Java_com_android_util_AndroidHelper_nativeTouchesCanceled(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys);
}

#endif

