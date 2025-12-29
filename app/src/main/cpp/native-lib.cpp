#include <jni.h>
#include <string>
#include <thread>

#include "logger.h"
#include <BNM/Image.hpp>
#include <BNM/Class.hpp>
#include <BNM/Field.hpp>
#include <BNM/Method.hpp>
#include <BNM/Loading.hpp>
#include "utils.h"


void OnLoaded();

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    BNM::Loading::AddOnLoadedEvent(OnLoaded);
    BNM::Loading::TryLoadByJNI(env);
    return JNI_VERSION_1_6;
}

// FeatureTypes: Toggle, Seekbar, Category
// Examples:
// Toggle:ToggleName:true
// Seekbar:SeekbarName:1_20_10
// Category:CategoryName
extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_android_support_Menu_getFeatureList(JNIEnv *env, jobject thiz) {
    std::string featList[] = {
            "Toggle:Infinite Moves",
    };
    return toJobjectArray(env, featList, std::size(featList));
}

bool moves = false;
extern "C" JNIEXPORT void JNICALL
Java_com_android_support_Menu_valueChange(
        JNIEnv *env,
        jobject thiz,
        jint featIdx,
        jstring featName,
        jobject value
) {
    // featIdx: index in feature list
    switch (featIdx) {
        case 0: {
            moves = toJboolean(env, value);
            break;
        }
        default:
            break;
    }
}

BNM::Method<void> AddMoves{};

float (*old_PlayerMove)();

float new_PlayerMove() {
    if (moves) {
        AddMoves(1);
    }
    return old_PlayerMove();
}


// Example Game: [Mafia Queens](https://www.nutaku.net/games/mafia-queens/)
void OnLoaded() {
    LOGI("OnLoaded");
    auto AssemblyCSharp = BNM::Image("Assembly-CSharp");
    auto GameBoardGame = BNM::Class("BlmbM23SDK", "GameBoardGame", AssemblyCSharp);
    auto PlayerMove = GameBoardGame.GetMethod("PlayerMove");
    AddMoves = GameBoardGame.GetMethod("AddMoves");
    BNM::BasicHook(PlayerMove, new_PlayerMove, old_PlayerMove);
}