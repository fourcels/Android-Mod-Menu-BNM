//
// Created by Ken on 2025/11/30.
//

#ifndef ANDROID_MOD_MENU_BNM_UTILS_H
#define ANDROID_MOD_MENU_BNM_UTILS_H

#include <jni.h>
#include <string>
#include <vector>
#include <BNM/Class.hpp>
#include <BNM/Method.hpp>
#include <BNM/Field.hpp>
#include <BNM/Property.hpp>

template<std::size_t N>
jobjectArray toJobjectArray(JNIEnv *env, std::string (&feats)[N]) {
    jclass stringClass = env->FindClass("java/lang/String");
    if (stringClass == nullptr) {
        return nullptr;
    }
    jobjectArray ret = env->NewObjectArray(
            N,
            stringClass,
            nullptr
    );
    if (ret == nullptr) {
        return nullptr;
    }

    for (int i = 0; i < N; ++i) {
        jstring javaString = env->NewStringUTF(feats[i].c_str());
        if (javaString == nullptr) {
            // Handle error: String conversion failed
            // You might need to release previously created objects here
            return nullptr;
        }
        env->SetObjectArrayElement(ret, i, javaString);
        env->DeleteLocalRef(javaString); // Release local reference
    }

    return ret;
}


template<typename T>
BNM::Method<T> GetMethod(BNM::IL2CPP::Il2CppObject *instant, const std::string_view &name,
                         int parameters = -1) {
    BNM::Method<T> method = BNM::Class(instant->klass).GetMethod(name, parameters);
    return method[instant];
}

template<typename T>
BNM::Method<T> GetMethod(void *instant, const std::string_view &name,
                         int parameters = -1) {
    return GetMethod<T>(static_cast<BNM::IL2CPP::Il2CppObject *>(instant), name, parameters);
}

template<typename T>
BNM::Method<T> GetMethod(BNM::IL2CPP::Il2CppObject *instant, const std::string_view &name,
                         const std::initializer_list<std::string_view> &parameterNames) {
    BNM::Method<T> method = BNM::Class(instant->klass).GetMethod(name, parameterNames);
    return method[instant];
}

template<typename T>
BNM::Method<T> GetMethod(void *instant, const std::string_view &name,
                         const std::initializer_list<std::string_view> &parameterNames) {
    return GetMethod<T>(static_cast<BNM::IL2CPP::Il2CppObject *>(instant), name, parameterNames);
}


template<typename T>
BNM::Method<T> GetMethod(BNM::IL2CPP::Il2CppObject *instant, const std::string_view &name,
                         const std::initializer_list<BNM::CompileTimeClass> &parameterTypes) {
    BNM::Method<T> method = BNM::Class(instant->klass).GetMethod(name, parameterTypes);
    return method[instant];
}

template<typename T>
BNM::Method<T> GetMethod(void *instant, const std::string_view &name,
                         const std::initializer_list<BNM::CompileTimeClass> &parameterTypes) {
    return GetMethod<T>(static_cast<BNM::IL2CPP::Il2CppObject *>(instant), name, parameterTypes);
}

template<typename T>
T GetField(BNM::IL2CPP::Il2CppObject *instant, const std::string_view &name) {
    BNM::Field<T> field = BNM::Class(instant->klass).GetField(name);
    return field[instant]();
}

template<typename T>
T GetProperty(BNM::IL2CPP::Il2CppObject *instant, const std::string_view &name) {
    BNM::Property<T> property = BNM::Class(instant->klass).GetProperty(name);
    return property[instant]();
}

template<typename T>
void SetProperty(BNM::IL2CPP::Il2CppObject *instant, const std::string_view &name, T value) {
    BNM::Property<T> property = BNM::Class(instant->klass).GetProperty(name);
    property[instant]() = value;
}

#endif //ANDROID_MOD_MENU_BNM_UTILS_H
