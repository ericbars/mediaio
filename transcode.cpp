#include "cn_mediaio_mediaio_ffmpeg_Transcode.h"
#include <android/log.h>
#include <pthread.h>
#include "ffmpeg.h"
#include "config.h"


#define TAG "transcode"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)


static JavaVM *mJavaVM = NULL;
static jclass globalClass = NULL;
static jobject globalObject = NULL;
static jint mArgc;
static const char *mArgvs[256];


void *do_ffmpeg(void *argv) {
    LOGV("thread %lld, load_ffmpeg", pthread_self());
    load_ffmpeg(mArgc, (char **) mArgvs);
    return nullptr;
}


void *do_ffprobe(void *argv) {
    LOGV("thread %lld, load_ffprobe", pthread_self());
    load_ffprobe(mArgc, (char **) mArgvs);
    return nullptr;
}

void probeCallbackAndroid(int key, const char *value) {
    //LOGV("probeCallbackAndroid key=%d, value=%s", key, value);
    JNIEnv *jniEnv;

    mJavaVM->AttachCurrentThread(&jniEnv, NULL);

    jmethodID getMethodId = jniEnv->GetMethodID(globalClass, "probeCallback",
                                                "(ILjava/lang/String;)V");
    if (NULL == getMethodId) {
        LOGV("probeCallback , GetMethodID failure. ");
        return;
    }

    jstring jstr = jniEnv->NewStringUTF(value);

    jniEnv->CallVoidMethod(globalObject, getMethodId, key, jstr);
    mJavaVM->DetachCurrentThread();
}

void progressCallbackAndroid(int progress) {
    //LOGV("progressCallbackAndroid %d", progress);
    JNIEnv *jniEnv;

    mJavaVM->AttachCurrentThread(&jniEnv, NULL);

    jmethodID getMethodId = jniEnv->GetMethodID(globalClass, "progressCallback", "(I)V");
    if (NULL == getMethodId) {
        LOGV("progressCallback , GetMethodID failure. ");
        return;
    }

    jniEnv->CallVoidMethod(globalObject, getMethodId, progress);
    mJavaVM->DetachCurrentThread();
}

jint get_jni_global_parameter(JNIEnv *env, jobject obj, jobjectArray argv, jint argc) {

    jboolean isCopy;

    jclass localClass = env->FindClass("cn/mediaio/mediaio/ffmpeg/Transcode");
    if (NULL == localClass) {
        LOGV("FindClass Transcode failure.");
        return -1;
    }

    globalClass = (jclass) env->NewGlobalRef(localClass);
    if (NULL == globalClass) {
        LOGV("localClass to globalClass failure.");
        return -1;
    }

    globalObject = (jclass) env->NewGlobalRef(obj);
    if (NULL == globalObject) {
        LOGV("obj to globalObject failure.");
        return -1;
    }
    mArgc = argc;
    for (int i = 0; i < argc; i++) {
        jstring str = (jstring) env->GetObjectArrayElement(argv, i);
        mArgvs[i] = env->GetStringUTFChars(str, &isCopy);
    }

    return 0;
}


void start_probe_thread(JNIEnv *env, jobject obj, jobjectArray argv, jint argc) {
    pthread_t worker_thread;

    get_jni_global_parameter(env, obj, argv, argc);

    registerProbeCallback(probeCallbackAndroid);

    pthread_create(&worker_thread, NULL, do_ffprobe, NULL);
    pthread_detach(worker_thread);

    return;
}

void start_worker_thread(JNIEnv *env, jobject obj, jobjectArray argv, jint argc) {
    pthread_t worker_thread;

    get_jni_global_parameter(env, obj, argv, argc);

    registerCallback(progressCallbackAndroid);

    pthread_create(&worker_thread, NULL, do_ffmpeg, NULL);
    pthread_detach(worker_thread);

    return;
}


/*
 * Class:     cn_mediaio_mediaio_ffmpeg_Transcode
 * Method:    doProbe
 * Signature: ([Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_cn_mediaio_mediaio_ffmpeg_Transcode_doProbe
        (JNIEnv *env, jobject obj, jobjectArray argv, jint argc) {
    LOGV("native : do probe");
    start_probe_thread(env, obj, argv, argc);
    return 0;
}


/*
 * Class:     cn_mediaio_mediaio_ffmpeg_Transcode
 * Method:    doTranscode
 * Signature: ([Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_cn_mediaio_mediaio_ffmpeg_Transcode_doTranscode
        (JNIEnv *env, jobject obj, jobjectArray argv, jint argc) {

    LOGV("native : do transcode");
    start_worker_thread(env, obj, argv, argc);
    return 0;
}


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    mJavaVM = vm;
    return JNI_VERSION_1_4;
}

