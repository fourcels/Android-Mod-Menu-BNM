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
            "Seekbar:Attack:1_20",
            "Seekbar:Defence:1_20",
    };
    return toJobjectArray(env, featList, std::size(featList));
}

int attack = 1;
int defence = 1;
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
            attack = toJint(env, value);
            break;
        }
        case 1: {
            defence = toJint(env, value);
            break;
        }
        default:
            break;
    }
}

BNM::Field<int> Camp{};

float (*old_GetSkillConditionAttack)(void *instance, void *inSource,
                                     void *inSkillProcessContext,
                                     void *inSourceSkillFunctionInfos);

float new_GetSkillConditionAttack(void *instance, void *inSource,
                                  void *inSkillProcessContext,
                                  void *inSourceSkillFunctionInfos) {
    auto ret = old_GetSkillConditionAttack(instance, inSource, inSkillProcessContext,
                                           inSourceSkillFunctionInfos);
    auto camp = Camp[inSource]();
    if (camp == 1) {
        ret = ret * attack;
    }
    return ret;
}

float (*old_GetDefenceRate)(void *instance, void *inSource,
                            void *inSkillProcessContext,
                            void *inSourceSkillFunctionInfos);

float new_GetDefenceRate(void *instance, void *inSource,
                         void *inSkillProcessContext,
                         void *inSourceSkillFunctionInfos) {
    auto ret = old_GetDefenceRate(instance, inSource, inSkillProcessContext,
                                  inSourceSkillFunctionInfos);
    auto camp = Camp[inSource]();
    if (camp == 2) {
        ret = ret * defence;
    }
    return ret;
}


// Example Game: [Ark Re:Code](https://www.nutaku.net/games/ark-recode/)
void OnLoaded() {
    LOGI("OnLoaded");
    auto AssemblyCSharp = BNM::Image("Assembly-CSharp");
    auto BattleRoleData = BNM::Class("Game", "BattleRoleData", AssemblyCSharp);
    Camp = BattleRoleData.GetField("Camp");
    auto BattleCalculator = BNM::Class("Game", "BattleCalculator", AssemblyCSharp);
    auto GetSkillConditionAttack = BattleCalculator.GetMethod("GetSkillConditionAttack");
    auto GetDefenceRate = BattleCalculator.GetMethod("GetDefenceRate");
    BNM::BasicHook(GetSkillConditionAttack, new_GetSkillConditionAttack,
                   old_GetSkillConditionAttack);
    BNM::BasicHook(GetDefenceRate, new_GetDefenceRate, old_GetDefenceRate);
}