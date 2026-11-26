package com.evilspins.app.kiosk;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.util.Log;

import com.getcapacitor.JSObject;
import com.getcapacitor.Plugin;
import com.getcapacitor.PluginCall;
import com.getcapacitor.PluginMethod;
import com.getcapacitor.annotation.CapacitorPlugin;

@CapacitorPlugin(name = "AppKiosk")
public class KioskPlugin extends Plugin {
    private static final String TAG = "KioskPlugin";
    private static final String ACTION_EXIT_KIOSK = "com.evilspins.app.EXIT_KIOSK";

    private BroadcastReceiver exitKioskReceiver;

    @PluginMethod
    public void exitLockTask(PluginCall call) {
        Activity activity = getActivity();
        if (activity == null) {
            call.reject("Activity not available");
            return;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            activity.stopLockTask();
            Log.d(TAG, "Lock task mode exited");
            JSObject result = new JSObject();
            result.put("success", true);
            call.resolve(result);
        } else {
            call.reject("Lock task not supported on this Android version");
        }
    }

    @PluginMethod
    public void isInLockTask(PluginCall call) {
        Activity activity = getActivity();
        if (activity == null) {
            call.reject("Activity not available");
            return;
        }

        boolean inLockTask = false;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            android.app.admin.DevicePolicyManager dpm =
                (android.app.admin.DevicePolicyManager) activity.getSystemService(Activity.DEVICE_POLICY_SERVICE);
            inLockTask = dpm != null && dpm.isDeviceOwnerApp(activity.getPackageName());
        }

        JSObject result = new JSObject();
        result.put("inLockTask", inLockTask);
        call.resolve(result);
    }

    @Override
    protected void handleOnDestroy() {
        super.handleOnDestroy();
        if (exitKioskReceiver != null) {
            try {
                getContext().unregisterReceiver(exitKioskReceiver);
            } catch (Exception e) {
                Log.w(TAG, "Error unregistering receiver", e);
            }
        }
    }

    public void registerExitKioskReceiver() {
        if (exitKioskReceiver != null) return;

        exitKioskReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                Log.d(TAG, "Received exit kiosk broadcast");
                Activity activity = getActivity();
                if (activity != null && Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    activity.stopLockTask();
                    Log.d(TAG, "Lock task mode exited via broadcast");
                }
            }
        };

        IntentFilter filter = new IntentFilter(ACTION_EXIT_KIOSK);
        getContext().registerReceiver(exitKioskReceiver, filter, Context.RECEIVER_EXPORTED);
        Log.d(TAG, "Exit kiosk receiver registered");
    }

    @PluginMethod
    public void setupExitReceiver(PluginCall call) {
        registerExitKioskReceiver();
        call.resolve(new JSObject());
    }
}