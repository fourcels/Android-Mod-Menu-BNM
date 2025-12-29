//
// Created by Ken on 2025/11/30.
//

#include "utils.h"

// jobject -> jboolean (JNI_TRUE/JNI_FALSE)
jboolean toJboolean(JNIEnv *env, jobject obj) {
    if (obj == nullptr) return JNI_FALSE;

    jclass booleanCls = env->FindClass("java/lang/Boolean");
    if (booleanCls && env->IsInstanceOf(obj, booleanCls)) {
        jmethodID mid = env->GetMethodID(booleanCls, "booleanValue", "()Z");
        jboolean v = env->CallBooleanMethod(obj, mid);
        env->DeleteLocalRef(booleanCls);
        return v;
    }
    if (booleanCls) env->DeleteLocalRef(booleanCls);

    jclass stringCls = env->FindClass("java/lang/String");
    if (stringCls && env->IsInstanceOf(obj, stringCls)) {
        jclass boolCls = env->FindClass("java/lang/Boolean");
        jmethodID parse = env->GetStaticMethodID(boolCls, "parseBoolean", "(Ljava/lang/String;)Z");
        jboolean v = env->CallStaticBooleanMethod(boolCls, parse, obj);
        env->DeleteLocalRef(stringCls);
        env->DeleteLocalRef(boolCls);
        return v;
    }
    if (stringCls) env->DeleteLocalRef(stringCls);

    return JNI_FALSE; // unsupported type; choose your policy
}

// jobject -> jint
jint toJint(JNIEnv *env, jobject obj) {
    if (obj == nullptr) return 0;

    jclass integerCls = env->FindClass("java/lang/Integer");
    if (integerCls && env->IsInstanceOf(obj, integerCls)) {
        jmethodID mid = env->GetMethodID(integerCls, "intValue", "()I");
        jint v = env->CallIntMethod(obj, mid);
        env->DeleteLocalRef(integerCls);
        return v;
    }
    if (integerCls) env->DeleteLocalRef(integerCls);

    return 0; // unsupported type; choose your policy
}

// string[] -> jobjectArray
jobjectArray toJobjectArray(JNIEnv *env, std::string arr[], int size) {
    jclass stringClass = env->FindClass("java/lang/String");
    if (stringClass == nullptr) {
        return nullptr;
    }
    jobjectArray ret = env->NewObjectArray(
            size,          // length(),
            stringClass,          // elementClass
            nullptr                  // initialElement (can be NULL or a default String object)
    );
    if (ret == nullptr) {
        return nullptr;
    }

    for (int i = 0; i < size; ++i) {
        jstring javaString = env->NewStringUTF(arr[i].c_str());
        if (javaString == nullptr) {
            // Handle error: String conversion failed
            // You might need to release previously created objects here
            return nullptr;
        }
        env->SetObjectArrayElement(ret, i, javaString);
        env->DeleteLocalRef(javaString); // Release local reference
    }

    // 5. Return the Java array
    return ret;
}