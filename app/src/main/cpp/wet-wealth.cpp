#include <jni.h>
#include <string>
#include <thread>
#include <vector>

#include "logger.h"
#include <BNM/Image.hpp>
#include <BNM/Class.hpp>
#include <BNM/Field.hpp>
#include <BNM/Method.hpp>
#include <BNM/Loading.hpp>
#include <BNM/Property.hpp>
#include <algorithm>
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
    std::string feats[] = {
            "Seekbar:Reward:1_20",
    };
    return toJobjectArray(env, feats);
}

struct Feature {
    int reward{1};
};

Feature feature{};

extern "C" JNIEXPORT void JNICALL
Java_com_android_support_Menu_valueChange(
        JNIEnv *env,
        jobject thiz,
        jint featIdx,
        jstring featName,
        jint value
) {
    // featIdx: index in feature list
    switch (featIdx) {
        case 0: {
            feature.reward = value;
            break;
        }
        default:
            break;
    }
}

void (*old_AddItem)(void *instance, void *item, int count);

void new_AddItem(void *instance, void *item, int count) {
    std::vector<int> ids = {550, 551, 1270, 1253, 10267, 10432, 12131, 12307, 14633, 16708};
    auto unnyId = GetProperty<int>(item, "IntUnnyId");
    if (std::ranges::contains(ids, unnyId)) {
        count *= feature.reward;
    }
    old_AddItem(instance, item, count);
}

void (*old_AddCharacterTemptation)(void *instance, void *character, int added);

void new_AddCharacterTemptation(void *instance, void *character, int added) {
    return old_AddCharacterTemptation(instance, character, added * feature.reward);
}


// [Wet Wealth](https://www.nutaku.net/games/wet-wealth/)
void OnLoaded() {
    LOGI("OnLoaded");
    auto AssemblyCSharp = BNM::Image("Assembly-CSharp");

    auto UserInventoryUsecase = BNM::Class("WetWealth.UserInventory", "UserInventoryUsecase",
                                           AssemblyCSharp);
    auto AddItem = UserInventoryUsecase.GetMethod("AddItem");

    auto UserCharactersUsecase = BNM::Class("WetWealth.Characters", "UserCharactersUsecase",
                                            AssemblyCSharp);
    auto AddCharacterTemptation = UserCharactersUsecase.GetMethod("AddCharacterTemptation");

    BNM::BasicHook(AddItem, new_AddItem, old_AddItem);
    BNM::BasicHook(AddCharacterTemptation, new_AddCharacterTemptation, old_AddCharacterTemptation);
}