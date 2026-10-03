package org.libsdl.app;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.DocumentsContract;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.atomic.AtomicInteger;

public class AssetImportActivity extends Activity {
    private static final int REQUEST_TREE = 1;
    private static final int PROGRESS_STEP = 20;

    private Button button;
    private TextView status;

    static boolean hasAssets(Context context) {
        File dir = context.getExternalFilesDir(null);
        return dir != null && new File(dir, "Data").isDirectory();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (hasAssets(this)) {
            startGame();
            return;
        }

        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setGravity(Gravity.CENTER);
        int pad = (int) (24 * getResources().getDisplayMetrics().density);
        layout.setPadding(pad, pad, pad, pad);

        TextView message = new TextView(this);
        message.setText(R.string.asset_import_message);
        message.setGravity(Gravity.CENTER);
        message.setTextSize(16);
        layout.addView(message);

        button = new Button(this);
        button.setText(R.string.asset_import_button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivityForResult(new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE), REQUEST_TREE);
            }
        });
        layout.addView(button);

        status = new TextView(this);
        status.setGravity(Gravity.CENTER);
        layout.addView(status);

        setContentView(layout);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_TREE && resultCode == RESULT_OK && data != null && data.getData() != null) {
            importTree(data.getData());
        }
    }

    private void startGame() {
        startActivity(new Intent(this, SDLActivity.class));
        finish();
    }

    private void importTree(final Uri tree) {
        button.setEnabled(false);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        final File target = getExternalFilesDir(null);
        final AtomicInteger copied = new AtomicInteger();

        new Thread(new Runnable() {
            @Override
            public void run() {
                String error = null;

                try {
                    copyTree(tree, DocumentsContract.getTreeDocumentId(tree), target, copied);
                } catch (IOException | RuntimeException e) {
                    error = String.valueOf(e.getMessage());
                }

                final String failure = error;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                        button.setEnabled(true);

                        if (failure != null) {
                            status.setText(getString(R.string.asset_import_failed, failure));
                        } else if (hasAssets(AssetImportActivity.this)) {
                            startGame();
                        } else {
                            status.setText(R.string.asset_import_no_data);
                        }
                    }
                });
            }
        }).start();
    }

    private void copyTree(Uri tree, String documentId, File dir, AtomicInteger copied) throws IOException {
        Uri children = DocumentsContract.buildChildDocumentsUriUsingTree(tree, documentId);
        String[] columns = {
            DocumentsContract.Document.COLUMN_DOCUMENT_ID,
            DocumentsContract.Document.COLUMN_DISPLAY_NAME,
            DocumentsContract.Document.COLUMN_MIME_TYPE,
            DocumentsContract.Document.COLUMN_SIZE
        };

        try (Cursor cursor = getContentResolver().query(children, columns, null, null, null)) {
            while (cursor != null && cursor.moveToNext()) {
                String id = cursor.getString(0);
                String name = cursor.getString(1);
                String mime = cursor.getString(2);
                long size = cursor.isNull(3) ? -1 : cursor.getLong(3);

                if (name == null || name.indexOf('/') >= 0 || name.equals("..") || name.equals(".")) {
                    continue;
                }

                File dest = new File(dir, name);

                if (DocumentsContract.Document.MIME_TYPE_DIR.equals(mime)) {
                    if (!dest.isDirectory() && !dest.mkdirs()) {
                        throw new IOException("cannot create " + dest.getPath());
                    }
                    copyTree(tree, id, dest, copied);
                } else {
                    copyFile(DocumentsContract.buildDocumentUriUsingTree(tree, id), dest, size, copied);
                }
            }
        }
    }

    private void copyFile(Uri source, File dest, long size, AtomicInteger copied) throws IOException {
        if (size >= 0 && dest.isFile() && dest.length() == size) {
            return;
        }

        try (InputStream in = getContentResolver().openInputStream(source);
             OutputStream out = new FileOutputStream(dest)) {
            if (in == null) {
                throw new IOException("cannot open " + source);
            }

            byte[] buffer = new byte[65536];
            int read;
            while ((read = in.read(buffer)) > 0) {
                out.write(buffer, 0, read);
            }
        }

        final int count = copied.incrementAndGet();
        if (count % PROGRESS_STEP == 0) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    status.setText(getString(R.string.asset_import_progress, count));
                }
            });
        }
    }
}
