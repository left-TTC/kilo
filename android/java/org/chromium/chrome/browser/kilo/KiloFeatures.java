package org.chromium.chrome.browser.kilo;

import org.chromium.base.Log;
import org.chromium.chrome.browser.shields.FilterListServiceFactory;

import androidx.annotation.Keep;
import android.util.Pair;

public class KiloFeatures {

    private static final String TAG = "FANMOCHENG";

    private static String[] sRootNames;
    private static boolean geted = false;

    public static boolean isBraveAIEnable() {
        return false;
    }

    @Keep
    private static Pair<String, Boolean> extractTargetDomain(String url) {
        Log.d(TAG, "extract origin: " + url);

        if (url != null && url.contains("search?q=")) {
            int pos = url.indexOf("q=");
            if (pos >= 0) {
                String target = url.substring(pos + 2); 
                int ampPos = target.indexOf('&');       
                if (ampPos >= 0) {
                    target = target.substring(0, ampPos);
                }

                Log.d(TAG, "extracted target: " + target);

                int slashPos = target.indexOf('/');
                String domain;
                if (slashPos >= 0) {
                    domain = target.substring(0, slashPos);
                } else {
                    domain = target;
                }

                String[] parts = domain.split("\\.");
                if (parts.length > 0) {
                    String rootDomain = parts[parts.length - 1];
                    Log.d(TAG, "root domain: " + rootDomain);

                    boolean found = false;
                    for (String s : sRootNames) {
                        if (s.equals(rootDomain)) {   
                            found = true;
                            break;
                        }
                    }

                    return new Pair<>(target, found);
                }

            }
        }

        return new Pair<>(url, false);
    }

    @Keep
    public static String IfSearchWeb3Name(String url){
        if(geted){
            Pair<String, Boolean> result = extractTargetDomain(url);

            if(result.second){
                return "https://" + result.first;
            } else {
                return url; 
            }
        } else {
            FilterListServiceFactory factory = FilterListServiceFactory.getInstance();
            String[] roots = factory.getRootNames();

            if(roots.length != 0){
                geted = true;
                sRootNames = roots;

                return IfSearchWeb3Name(url); 
            } else {
                return url; 
            }
        }
    }
}