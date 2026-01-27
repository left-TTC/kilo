/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_shields/filter_list_service_factory.h"

#include "base/android/jni_android.h"
#include "chrome/android/chrome_jni_headers/FilterListServiceFactory_jni.h"
#include "chrome/browser/profiles/profile.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"

#include "base/android/jni_string.h"
#include "base/android/jni_array.h"

#include "brave/components/decentralized_dns/core/pref_names.h"
#include "components/prefs/pref_service.h"
#include "chrome/browser/browser_process.h"

#include "base/logging.h"  
#include "brave/components/decentralized_dns/core/utils.h" 

namespace chrome {
    namespace android {

        static jlong JNI_FilterListServiceFactory_GetInterfaceToFilterListService(
            JNIEnv* env,
            const base::android::JavaParamRef<jobject>& profile_android
        ) {
                

            auto* profile = Profile::FromJavaObject(profile_android);
            auto pending = brave_shields::FilterListServiceFactory::GetInstance()
                                ->GetRemoteForProfile(profile);

            return static_cast<jlong>(pending.PassPipe().release().value());
        }



        static base::android::ScopedJavaLocalRef<jstring> JNI_FilterListServiceFactory_GetGateway(JNIEnv* env) {

            PrefService* pref_service = g_browser_process->local_state();
            std::string value = pref_service->GetString(decentralized_dns::kWnsResolveMethodGateWay);

            LOG(INFO) << "get FANMOCHENG ipfs gate way: " << value;
            return base::android::ConvertUTF8ToJavaString(env, value);
        }

        static void JNI_FilterListServiceFactory_SetGateway(
            JNIEnv* env,
            const jni_zero::JavaRef<jstring>& value) {

            PrefService* pref_service = g_browser_process->local_state();

            std::string str = base::android::ConvertJavaStringToUTF8(env, value);
            LOG(INFO) << "set FANMOCHENG ipfs gate way: " << str;
            pref_service->SetString(decentralized_dns::kWnsResolveMethodGateWay, str);
        }


        static base::android::ScopedJavaLocalRef<jstring> JNI_FilterListServiceFactory_GetRPCGateway(JNIEnv* env) {

            PrefService* pref_service = g_browser_process->local_state();
            std::string value = pref_service->GetString(decentralized_dns::kWnsRpcResolveWay);

            LOG(INFO) << "get FANMOCHENG rpc gate way: " << value;
            return base::android::ConvertUTF8ToJavaString(env, value);
        }

        static void JNI_FilterListServiceFactory_SetRPCGateway(
            JNIEnv* env,
            const jni_zero::JavaRef<jstring>& value
        ) {

            PrefService* pref_service = g_browser_process->local_state();

            std::string str = base::android::ConvertJavaStringToUTF8(env, value);
            LOG(INFO) << "set FANMOCHENG rpc gate way: " << str;
            pref_service->SetString(decentralized_dns::kWnsRpcResolveWay, str);
        }


        static jni_zero::ScopedJavaLocalRef<jobjectArray> JNI_FilterListServiceFactory_GetRootNames(JNIEnv* env) {
            PrefService* pref_service = g_browser_process->local_state();
            if (!pref_service) {
                return jni_zero::ScopedJavaLocalRef<jobjectArray>();
            }

            std::vector<std::string> roots = decentralized_dns::GetWnsRootNames(pref_service);

            return base::android::ToJavaArrayOfStrings(env, roots);
        }

    }  // namespace android
}  // namespace chrome
