package com.box.app;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.getcapacitor.BridgeActivity;
import com.box.app.permissions.PermissionsPlugin;
import com.box.app.nfc.NfcPlugin;
import com.box.app.sensor.LightSensorPlugin;
import com.box.app.storage.FileStoragePlugin;

public class MainActivity extends BridgeActivity {
    private static final String TAG = "MainActivity";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        // Register plugins BEFORE super.onCreate() which calls load()
        registerPlugin(PermissionsPlugin.class);
        registerPlugin(NfcPlugin.class);
        registerPlugin(LightSensorPlugin.class);
        registerPlugin(FileStoragePlugin.class);

        super.onCreate(savedInstanceState);

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
    }
}