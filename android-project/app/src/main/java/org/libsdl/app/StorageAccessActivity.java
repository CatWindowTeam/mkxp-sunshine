package org.libsdl.app;

import android.Manifest;
import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.io.File;

public class StorageAccessActivity extends Activity {
    private static final int REQUEST_STORAGE = 1;
    private static final String GAME_DIR_NAME = "Sunshine";

    private boolean requested;
    private boolean gameStarted;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setGravity(Gravity.CENTER);
        int pad = (int) (24 * getResources().getDisplayMetrics().density);
        layout.setPadding(pad, pad, pad, pad);

        TextView message = new TextView(this);
        message.setText(R.string.storage_access_message);
        message.setGravity(Gravity.CENTER);
        message.setTextSize(16);
        layout.addView(message);

        Button button = new Button(this);
        button.setText(R.string.storage_access_button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                requestAccess();
            }
        });
        layout.addView(button);

        setContentView(layout);
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (hasAccess()) {
            startGame();
        } else if (!requested) {
            requested = true;
            requestAccess();
        }
    }

    private boolean hasAccess() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            return Environment.isExternalStorageManager();
        }

        return checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED
            && checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
    }

    private void requestAccess() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            Uri uri = Uri.parse("package:" + getPackageName());
            try {
                startActivityForResult(new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION, uri), REQUEST_STORAGE);
            } catch (ActivityNotFoundException e) {
                startActivityForResult(new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION), REQUEST_STORAGE);
            }
            return;
        }

        requestPermissions(new String[] {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
        }, REQUEST_STORAGE);
    }

    private void startGame() {
        if (gameStarted) {
            return;
        }
        gameStarted = true;

        new File(Environment.getExternalStorageDirectory(), GAME_DIR_NAME).mkdirs();

        startActivity(new Intent(this, SDLActivity.class));
        finish();
    }
}
