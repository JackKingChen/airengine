#include "stdafx.h"
#include "android/AndroidJniHelper.h"
#include "android/AndroidSensor.h"
#include "android/AndroidInputManager.h"
#include "android/AndroidMultiTouch.h"
#include "AISInputSystem.h"
#include "AISInputManager.h"

#ifdef AIS_ANDROID_PLATFORM

#include <stdlib.h>
#include <android/log.h>
#include <string.h>

#if 1
#define  LOG_TAG        "AndroidJniHelper"
#define  DroidLOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  DroidLOGD(...)
#endif


#define	 JAVAVM    AIS::AndroidJniHelper::getJavaVM()

static EditTextCallback s_pfEditTextCallback = NULL;
static void* s_ctx = NULL;

#define  CLASS_NAME "org/age/lib/AndroidHelper"

using namespace AIS;
using namespace std;

extern "C"
{

	//////////////////////////////////////////////////////////////////////////
	// java vm helper function
	//////////////////////////////////////////////////////////////////////////

	static bool getEnv(JNIEnv **env)
	{
		bool bRet = false;

		do 
		{
			if (JAVAVM->GetEnv((void**)env, JNI_VERSION_1_4) != JNI_OK)
			{
				break;
			}

			if (JAVAVM->AttachCurrentThread(env, 0) < 0)
			{
				break;
			}

			bRet = true;
		} while (0);        

		return bRet;
	}

	static jclass getClassID_(const char *className, JNIEnv *env)
	{
		JNIEnv *pEnv = env;
		jclass ret = 0;

		do 
		{
			if (! pEnv)
			{
				if (! getEnv(&pEnv))
				{
					break;
				}
			}

			ret = pEnv->FindClass(className);
			if (! ret)
			{
				break;
			}
		} while (0);

		return ret;
	}

	static bool getStaticMethodInfo_(AIS::JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
	{
		jmethodID methodID = 0;
		JNIEnv *pEnv = 0;
		bool bRet = false;

		do 
		{
			if (! getEnv(&pEnv))
			{
				break;
			}

			jclass classID = getClassID_(className, pEnv);

			methodID = pEnv->GetStaticMethodID(classID, methodName, paramCode);
			if (! methodID)
			{
				break;
			}

			methodinfo.classID = classID;
			methodinfo.env = pEnv;
			methodinfo.methodID = methodID;

			bRet = true;
		} 
		while (0);

		return bRet;
	}

	static bool getMethodInfo_(AIS::JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
	{
		jmethodID methodID = 0;
		JNIEnv *pEnv = 0;
		bool bRet = false;

		do 
		{
			if (! getEnv(&pEnv))
			{
				break;
			}

			jclass classID = getClassID_(className, pEnv);

			methodID = pEnv->GetMethodID(classID, methodName, paramCode);
			if (! methodID)
			{
				break;
			}

			methodinfo.classID = classID;
			methodinfo.env = pEnv;
			methodinfo.methodID = methodID;

			bRet = true;
		} while (0);

		return bRet;
	}

	static string jstring2string_(jstring jstr)
	{
		if (jstr == NULL)
		{
			return "";
		}

		JNIEnv *env = 0;

		if (! getEnv(&env))
		{
			return 0;
		}

		const char* chars = env->GetStringUTFChars(jstr, NULL);
		string ret(chars);
		env->ReleaseStringUTFChars(jstr, chars);

		return ret;
	}
}


extern "C" 
{
	string g_apkPath;

	void Java_com_android_util_AndroidHelper_nativeSetApkPath(JNIEnv*  env, jobject thiz, jstring apkPath) 
	{
		g_apkPath = AIS::AndroidJniHelper::jstring2string(apkPath);
	}

	const char * getApkPath() 
	{
		return g_apkPath.c_str();
	}

	void Java_com_android_util_AndroidHelper_nativeSetExternalAssetPath(JNIEnv*  env, jobject thiz, jstring externalAssetPath) 
	{
		const char* externalAssetPathChars = env->GetStringUTFChars(externalAssetPath, NULL);
		AIS::AndroidJniHelper::setExternalAssetPath(externalAssetPathChars);
		env->ReleaseStringUTFChars(externalAssetPath, externalAssetPathChars);
	}

	void showDialogJNI(const char * pszMsg, const char * pszTitle) 
	{
		if (!pszMsg) 
		{
			return;
		}

		JniMethodInfo t;
		if (AIS::AndroidJniHelper::getStaticMethodInfo(t, CLASS_NAME, "showDialog", "(Ljava/lang/String;Ljava/lang/String;)V")) 
		{
			jstring stringArg1;

			if (!pszTitle) 
			{
				stringArg1 = t.env->NewStringUTF("");
			}
			else 
			{
				stringArg1 = t.env->NewStringUTF(pszTitle);
			}

			jstring stringArg2 = t.env->NewStringUTF(pszMsg);
			t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);

			t.env->DeleteLocalRef(stringArg1);
			t.env->DeleteLocalRef(stringArg2);
			t.env->DeleteLocalRef(t.classID);
		}
	}

	void showEditTextDialogJNI(const char* pszTitle, const char* pszMessage, int nInputMode, int nInputFlag, int nReturnType, int nMaxLength, EditTextCallback pfEditTextCallback, void* ctx) 
	{
		if (pszMessage == NULL) 
		{
			return;
		}

		s_pfEditTextCallback = pfEditTextCallback;
		s_ctx = ctx;

		JniMethodInfo t;
		if (AIS::AndroidJniHelper::getStaticMethodInfo(t, CLASS_NAME, "showEditTextDialog", "(Ljava/lang/String;Ljava/lang/String;IIII)V")) 
		{
			jstring stringArg1;

			if (!pszTitle) 
			{
				stringArg1 = t.env->NewStringUTF("");
			} 
			else 
			{
				stringArg1 = t.env->NewStringUTF(pszTitle);
			}

			jstring stringArg2 = t.env->NewStringUTF(pszMessage);

			t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2, nInputMode, nInputFlag, nReturnType, nMaxLength);

			t.env->DeleteLocalRef(stringArg1);
			t.env->DeleteLocalRef(stringArg2);
			t.env->DeleteLocalRef(t.classID);
		}
	}

	void Java_com_android_util_AndroidHelper_nativeSetEditTextDialogResult(JNIEnv * env, jobject obj, jbyteArray text) 
	{
		jsize  size = env->GetArrayLength(text);

		if (size > 0) 
		{
			jbyte * data = (jbyte*)env->GetByteArrayElements(text, 0);
			char* pBuf = (char*)malloc(size+1);
			if (pBuf != NULL) 
			{
				memcpy(pBuf, data, size);
				pBuf[size] = '\0';
				// pass data to edittext's delegate
				if (s_pfEditTextCallback) s_pfEditTextCallback(pBuf, s_ctx);
				free(pBuf);
			}
			env->ReleaseByteArrayElements(text, data, 0);
		} 
		else 
		{
			if (s_pfEditTextCallback) s_pfEditTextCallback("", s_ctx);
		}
	}

	void terminateProcessJNI() 
	{
		JniMethodInfo t;

		if (AIS::AndroidJniHelper::getStaticMethodInfo(t, CLASS_NAME, "terminateProcess", "()V")) 
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
			t.env->DeleteLocalRef(t.classID);
		}
	}

	void enableAccelerometerJNI() 
	{
		JniMethodInfo t;

		if (AIS::AndroidJniHelper::getStaticMethodInfo(t, CLASS_NAME, "enableAccelerometer", "()V")) 
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
			t.env->DeleteLocalRef(t.classID);
		}
	}

	void setAccelerometerIntervalJNI(float interval) 
	{
		JniMethodInfo t;

		if (AIS::AndroidJniHelper::getStaticMethodInfo(t, CLASS_NAME, "setAccelerometerInterval", "(I)V")) 
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID, interval);
			t.env->DeleteLocalRef(t.classID);
		}
	}

	void disableAccelerometerJNI()
	{
		JniMethodInfo t;

		if (AIS::AndroidJniHelper::getStaticMethodInfo(t, CLASS_NAME, "disableAccelerometer", "()V")) 
		{
			t.env->CallStaticVoidMethod(t.classID, t.methodID);
			t.env->DeleteLocalRef(t.classID);
		}
	}
}


extern "C" 
{
	JNIEXPORT void JNICALL  Java_com_android_util_AndroidHelper_nativeSensorChanged(JNIEnv*  env, jobject thiz, jfloat x, jfloat y, jfloat z, jlong timeStamp, jint orientation, jint rotate)
	{
		if ( AIS::AISInputSystem::GetInstance() == NULL )
			return;

		AIS::AndroidSensor* pSensor = (AIS::AndroidSensor*)AIS::AISInputSystem::GetInstance()->GetSensor();
		if ( pSensor )
		{
			AIS::SensorParam parm;
			parm.mSO = (AIS::SensorOrientation)orientation;
			parm.mSR = (AIS::SensorRotate)rotate;
			parm.mTimeStamp = (double)timeStamp;
			parm.mX = x;
			parm.mY = y;
			parm.mZ = z;

			pSensor->didAccelerate( parm );
		}
	}

	JNIEXPORT void JNICALL  Java_com_android_util_AndroidHelper_nativeTouchesBegan(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
	{
		if ( NULL == AIS::AISInputSystem::GetInstance() )
			return;
		AIS::AndroidMultiTouch* pTouchDevice = (AIS::AndroidMultiTouch*)AIS::AISInputSystem::GetInstance()->GetMultiTouchDevice();
		if ( pTouchDevice )
		{
			int size = env->GetArrayLength(ids);
			jint id[size];
			jfloat x[size];
			jfloat y[size];

			env->GetIntArrayRegion(ids, 0, size, id);
			env->GetFloatArrayRegion(xs, 0, size, x);
			env->GetFloatArrayRegion(ys, 0, size, y);

			pTouchDevice->handleTouchesBegan( size, id, x, y );
		}
	}

	JNIEXPORT void JNICALL  Java_com_android_util_AndroidHelper_nativeTouchesEnded(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
	{
		if ( NULL == AIS::AISInputSystem::GetInstance() )
			return;
		AIS::AndroidMultiTouch* pTouchDevice = (AIS::AndroidMultiTouch*)AIS::AISInputSystem::GetInstance()->GetMultiTouchDevice();
		if ( pTouchDevice )
		{
			int size = env->GetArrayLength(ids);
			jint id[size];
			jfloat x[size];
			jfloat y[size];

			env->GetIntArrayRegion(ids, 0, size, id);
			env->GetFloatArrayRegion(xs, 0, size, x);
			env->GetFloatArrayRegion(ys, 0, size, y);

			pTouchDevice->handleTouchesEnded( size, id, x, y );
		}
	}

	JNIEXPORT void JNICALL  Java_com_android_util_AndroidHelper_nativeTouchesMoved(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
	{
		if ( NULL == AIS::AISInputSystem::GetInstance() )
			return;
		AIS::AndroidMultiTouch* pTouchDevice = (AIS::AndroidMultiTouch*)AIS::AISInputSystem::GetInstance()->GetMultiTouchDevice();
		if ( pTouchDevice )
		{
			int size = env->GetArrayLength(ids);
			jint id[size];
			jfloat x[size];
			jfloat y[size];

			env->GetIntArrayRegion(ids, 0, size, id);
			env->GetFloatArrayRegion(xs, 0, size, x);
			env->GetFloatArrayRegion(ys, 0, size, y);

			pTouchDevice->handleTouchesMoved( size, id, x, y );
		}
	}

	JNIEXPORT void JNICALL  Java_com_android_util_AndroidHelper_nativeTouchesCanceled(JNIEnv * env, jobject thiz, jintArray ids, jfloatArray xs, jfloatArray ys)
	{
		if ( NULL == AIS::AISInputSystem::GetInstance() )
			return;
		AIS::AndroidMultiTouch* pTouchDevice = (AIS::AndroidMultiTouch*)AIS::AISInputSystem::GetInstance()->GetMultiTouchDevice();
		if ( pTouchDevice )
		{
			int size = env->GetArrayLength(ids);
			jint id[size];
			jfloat x[size];
			jfloat y[size];

			env->GetIntArrayRegion(ids, 0, size, id);
			env->GetFloatArrayRegion(xs, 0, size, x);
			env->GetFloatArrayRegion(ys, 0, size, y);

			pTouchDevice->handleTouchesCancelled( size, id, x, y );
		}
	}
}

//--------------------------------------------------------------------------------------------------

JavaVM* AndroidJniHelper::m_psJavaVM = NULL;
string AndroidJniHelper::m_externalAssetPath;

JavaVM* AndroidJniHelper::getJavaVM()
{
	return m_psJavaVM;
}

void AndroidJniHelper::setJavaVM(JavaVM *javaVM)
{
	m_psJavaVM = javaVM;
}

const char* AndroidJniHelper::getExternalAssetPath() 
{
	return m_externalAssetPath.c_str();
}

void AndroidJniHelper::setExternalAssetPath(const char * externalAssetPath) 
{
	m_externalAssetPath = externalAssetPath;
}

jclass AndroidJniHelper::getClassID(const char *className, JNIEnv *env)
{
	return getClassID_(className, env);
}

bool AndroidJniHelper::getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
{
	return getStaticMethodInfo_(methodinfo, className, methodName, paramCode);
}

bool AndroidJniHelper::getMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
{
	return getMethodInfo_(methodinfo, className, methodName, paramCode);
}

string AndroidJniHelper::jstring2string(jstring str)
{
	return jstring2string_(str);
}

//////////////////////////////////////////////////////////////////////////

void AndroidJniHelper::setAcclerometerInterval( float fInterval )
{

}



#endif
