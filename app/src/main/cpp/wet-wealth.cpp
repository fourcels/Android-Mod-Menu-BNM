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
            "Toggle:Characters",
            "Seekbar:Reward:1_20",
    };
    return toJobjectArray(env, feats);
}

struct Feature {
    bool characters{false};
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
            feature.characters = value;
            break;
        }
        case 1: {
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

void (*old_Load)(void *instance);

void new_Load(BNM::IL2CPP::Il2CppObject *instance) {
    old_Load(instance);
    if (feature.characters) {
        auto userCharacterFactory = GetField<void *>(instance, "userCharacterFactory");
        auto characterStaticUsecase = GetField<void *>(userCharacterFactory,
                                                       "characterStaticUsecase");
        auto characters = GetProperty<void *>(characterStaticUsecase,
                                              "Characters");
        auto count = GetMethod<int>(characters, "get_Count")();
        for (int i = 0; i < count; i++) {
            auto character = GetMethod<void *>(characters, "get_Item")(i);
            auto id = GetProperty<int>(character, "Id");
            GetMethod<void>(instance, "AddCharacterOrExperience")(id, 100);
        }
    }
}

bool (*old_TryBuyVip)(void *instance);

bool new_TryBuyVip(void *instance) {
    auto monthlyVipUsecase = GetField<void *>(instance, "monthlyVipUsecase");
    auto userInventoryUsecase = GetField<void *>(monthlyVipUsecase, "userInventoryUsecase");
    auto vipAccessKey = GetMethod<void *>(instance, "GetVipAccessKey")();
    GetMethod<void>(userInventoryUsecase, "AddItem")(vipAccessKey, 1);
    return true;
}

void (*old_TryBuy)(void *instance, void *config);

void new_TryBuy(void *instance, void *config) {
    auto key = GetProperty<void *>(config, "Key");
    auto usecase = GetField<void *>(instance, "usecase");
    auto inventoryUsecase = GetField<void *>(usecase, "inventoryUsecase");
    GetMethod<void>(inventoryUsecase, "AddItem")(key, 1);

    auto OnPurchased = GetField<void *>(instance, "OnPurchased");
    GetMethod<void>(OnPurchased, "Invoke")(config);
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
    auto Load = UserCharactersUsecase.GetMethod("Load");
    auto AddCharacterTemptation = UserCharactersUsecase.GetMethod("AddCharacterTemptation");

    auto MonthlyVipPresenter = BNM::Class("WetWealth.SlotMachine",
                                          "MonthlyVipPresenter",
                                          AssemblyCSharp);
    auto TryBuyVip = MonthlyVipPresenter.GetMethod("TryBuyVip");

    auto BuildingRepairPresenter = BNM::Class("WetWealth.BuildingRepair",
                                              "BuildingRepairPresenter",
                                              AssemblyCSharp);
    auto TryBuy = BuildingRepairPresenter.GetMethod("TryBuy");

    BNM::BasicHook(AddItem, new_AddItem, old_AddItem);
    BNM::BasicHook(Load, new_Load, old_Load);
    BNM::BasicHook(AddCharacterTemptation, new_AddCharacterTemptation, old_AddCharacterTemptation);
    BNM::BasicHook(TryBuyVip, new_TryBuyVip, old_TryBuyVip);
    BNM::BasicHook(TryBuy, new_TryBuy, old_TryBuy);
}