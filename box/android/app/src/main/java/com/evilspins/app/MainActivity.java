package com.evilspins.app;

import android.app.Activity;
import android.app.admin.DevicePolicyManager;
import android.content.ComponentName;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.getcapacitor.BridgeActivity;
import com.evilspins.app.permissions.PermissionsPlugin;
import com.evilspins.app.nfc.NfcPlugin;
import com.evilspins.app.sensor.LightSensorPlugin;
import com.evilspins.app.storage.FileStoragePlugin;
import com.evilspins.app.kiosk.KioskDeviceAdminReceiver;
import com.evilspins.app.kiosk.KioskPlugin;

public class MainActivity extends BridgeActivity {
    private static final String TAG = "MainActivity";
    private DevicePolicyManager devicePolicyManager;
    private ComponentName adminComponent;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        // Register plugins BEFORE super.onCreate() which calls load()
        registerPlugin(PermissionsPlugin.class);
        registerPlugin(NfcPlugin.class);
        registerPlugin(LightSensorPlugin.class);
        registerPlugin(FileStoragePlugin.class);
        registerPlugin(KioskPlugin.class);

        super.onCreate(savedInstanceState);

        // Setup kiosk mode
        setupKioskMode();

        // Setup immersive fullscreen
        setupImmersiveMode();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            setupImmersiveMode();
        }
    }

    private void setupKioskMode() {
        devicePolicyManager = (DevicePolicyManager) getSystemService(Context.DEVICE_POLICY_SERVICE);
        adminComponent = new ComponentName(this, KioskDeviceAdminReceiver.class);

        // Check if this app is device owner
        boolean isDeviceOwner = false;
        if (devicePolicyManager != null) {
            isDeviceOwner = devicePolicyManager.isDeviceOwnerApp(getPackageName());
        }

        Log.d(TAG, "isDeviceOwner: " + isDeviceOwner);

        if (isDeviceOwner && Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            // Whitelist this package for lock task mode
            String[] packages = {getPackageName()};
            devicePolicyManager.setLockTaskPackages(adminComponent, packages);

            // Start lock task mode
            startLockTask();
            Log.d(TAG, "Lock task mode started");
        } else {
            Log.w(TAG, "Not device owner - lock task mode not available");
            Log.w(TAG, "Run provisioning command: adb shell dpm set-device-owner com.evilspins.app/.kiosk.KioskDeviceAdminReceiver");
        }
    }

    private void setupImmersiveMode() {
        View decorView = getWindow().getDecorView();
        int uiOptions = View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE;

        decorView.setSystemUiVisibility(uiOptions);

        // Keep screen on
        getWindow().addFlags(android.view.WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    public void onResume() {
        super.onResume();
        setupImmersiveMode();

        // Re-enter lock task if needed
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            boolean isDeviceOwner = devicePolicyManager != null &&
                devicePolicyManager.isDeviceOwnerApp(getPackageName());
            if (isDeviceOwner) {
                // startLockTask() is safe to call multiple times
                startLockTask();
            }
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, android.view.KeyEvent event) {
        // Exit kiosk mode: long press Volume Down (or Volume Up + Power)
        if (keyCode == android.view.KeyEvent.KEYCODE_VOLUME_DOWN) {
            if (event.getRepeatCount() == 0) {
                // Start timer for long press
                android.os.Handler handler = new android.os.Handler();
                handler.postDelayed(() -> {
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                        stopLockTask();
                        Log.d(TAG, "Lock task mode exited via long press Volume Down");
                    }
                }, 2000); // 2 second long press
            }
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public boolean onKeyUp(int keyCode, android.view.KeyEvent event) {
        if (keyCode == android.view.KeyEvent.KEYCODE_VOLUME_DOWN) {
            return true;
        }
        return super.onKeyUp(keyCode, event);
    }
}