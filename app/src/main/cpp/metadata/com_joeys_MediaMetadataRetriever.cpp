//
// Created by 王越 on 2020/7/5.
//

#include <jni.h>
#include "../XLog.h"
#include "MediaMetadataRetriever.h"

static const char *const RETRIEVER_CLASS_NAME = "com/joeys/xplay/metadata/MediaMetadataRetriever";

struct retriever_fields_t {
    jfieldID context;
};
static retriever_fields_t fields;
static MediaMetadataRetriever *retriever;

static void setRetriever(JNIEnv *env, jobject thiz, long retriever) {
    MediaMetadataRetriever *old = (MediaMetadataRetriever *) env->GetLongField(thiz,
                                                                               fields.context);
    env->SetLongField(thiz, fields.context, retriever);
}

static MediaMetadataRetriever *getRetriever(JNIEnv *env, jobject thiz) {
    MediaMetadataRetriever *retriever = (MediaMetadataRetriever *) env->GetLongField(thiz,
                                                                                     fields.context);
    return retriever;
}

void throwException(JNIEnv *env, const char *className, const char *msg) {
    jclass exception = env->FindClass(className);
    env->ThrowNew(exception, msg);
}

/**
 * 创建新的jstring对象
 * @param env
 * @param data
 * @return
 */
static jstring newUTFString(JNIEnv *env, const char *data) {
    jstring str = NULL;

    int size = strlen(data);

    jbyteArray array = NULL;
    array = env->NewByteArray(size);
    if (!array) {
        LOGE("convertString: OutOfMemoryError is thrown.");
    } else {
        jbyte *bytes = env->GetByteArrayElements(array, NULL);
        if (bytes != NULL) {
            memcpy(bytes, data, size);
            env->ReleaseByteArrayElements(array, bytes, 0);

            jclass clazz = env->FindClass("java/lang/String");
            jmethodID pMethodID = env->GetMethodID(clazz, "<init>", "([BLjava/lang/String;)V");
            jstring utf = env->NewStringUTF("UTF-8");
            str = (jstring) env->NewObject(clazz, pMethodID, array, utf);

            env->DeleteLocalRef(utf);
        }
    }
    env->DeleteLocalRef(array);

    return str;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_metadata_MediaMetadataRetriever_native_1setup(JNIEnv *env, jobject thiz) {
    LOGI("Java_com_joeys_xplay_MediaMetadataRetriever_native_1setup");

    retriever = new MediaMetadataRetriever();
//    setRetriever(env, thiz, (long) retriever);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_metadata_MediaMetadataRetriever_setDataSource(JNIEnv *env, jobject thiz,
                                                                   jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);
    LOGI("Java_com_joeys_xplay_MediaMetadataRetriever_setDataSource -> %s", url);
//    MediaMetadataRetriever *retriever = getRetriever(env, thiz);

    if (retriever) {
        retriever->setDataSource(url);
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_joeys_xplay_metadata_MediaMetadataRetriever__1getAllMetadata(JNIEnv *env, jobject thiz) {

//    MediaMetadataRetriever *retriever = getRetriever(env, thiz);
    if (retriever == NULL) {
        throwException(env, "java/lang/IllegalStateException", "No retriever available");
        return NULL;
    }

    AVDictionary *metadata = NULL;
    int ret = retriever->getMetadata(&metadata);
    if (ret == 0) {
        jclass hashMapClass = env->FindClass("java/util/HashMap");
        jmethodID hashMap_init = env->GetMethodID(hashMapClass, "<init>", "()V");
        // 创建一个HashMap对象
        jobject hashMap = env->NewObject(hashMapClass, hashMap_init);
        jmethodID hashMap_put = env->GetMethodID(hashMapClass, "put",
                                                 "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
        // 将metadata的参数复制到HashMap中
        for (int i = 0; i < metadata->count; ++i) {
            jstring key = newUTFString(env, metadata->elements[i].key);
            jstring value = newUTFString(env, metadata->elements[i].value);
            env->CallObjectMethod(hashMap, hashMap_put, key, value);
        }

        if (metadata) {
            av_dict_free(&metadata);
        }

        return hashMap;
    }
}