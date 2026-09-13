package org.libsdl.app;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;

public class TouchControlsView extends View {
    private static TouchControlsView sInstance;

    private float padZoneRight;
    private float padCx, padCy, padRadius;

    private int padPointerId = -1;
    private int padActiveH = -1;
    private int padActiveV = -1;

    private static final float DEADZONE_RATIO = 0.25f;
    private static final float DIAGONAL_RATIO = 0.4f;

    private final Paint ringPaint;
    private final Paint dotPaint;

    public TouchControlsView(Context context) {
        super(context);
        setFocusable(false);
        sInstance = this;

        ringPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        ringPaint.setColor(Color.argb(70, 255, 255, 255));
        ringPaint.setStyle(Paint.Style.STROKE);
        ringPaint.setStrokeWidth(4);

        dotPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        dotPaint.setColor(Color.argb(100, 255, 255, 255));
        dotPaint.setStyle(Paint.Style.FILL);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        canvas.drawCircle(padCx, padCy, padRadius, ringPaint);
        canvas.drawCircle(padCx, padCy, padRadius * DEADZONE_RATIO, ringPaint);
        if (padPointerId != -1)
            canvas.drawCircle(padCx, padCy, padRadius * 0.15f, dotPaint);
    }

    public static void setVisible(final boolean visible) {
        final TouchControlsView v = sInstance;
        if (v == null) return;
        v.post(new Runnable() {
            @Override
            public void run() {
                v.setVisibility(visible ? VISIBLE : GONE);
            }
        });
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        padZoneRight = w * 0.5f;
        padRadius = Math.min(w, h) * 0.12f;
        padCx = padRadius * 1.6f;
        padCy = h - padRadius * 1.6f;
    }

    private int padHorizontalKeycode(float x, float y) {
        float dx = x - padCx;
        float dy = y - padCy;
        float dist = (float) Math.sqrt(dx * dx + dy * dy);
        if (dist < padRadius * DEADZONE_RATIO)
            return -1;
        if (Math.abs(dx) < dist * DIAGONAL_RATIO)
            return -1;
        return dx < 0 ? KeyEvent.KEYCODE_DPAD_LEFT : KeyEvent.KEYCODE_DPAD_RIGHT;
    }

    private int padVerticalKeycode(float x, float y) {
        float dx = x - padCx;
        float dy = y - padCy;
        float dist = (float) Math.sqrt(dx * dx + dy * dy);
        if (dist < padRadius * DEADZONE_RATIO)
            return -1;
        if (Math.abs(dy) < dist * DIAGONAL_RATIO)
            return -1;
        return dy < 0 ? KeyEvent.KEYCODE_DPAD_UP : KeyEvent.KEYCODE_DPAD_DOWN;
    }

    private void setPadAxis(boolean horizontal, int keycode) {
        int current = horizontal ? padActiveH : padActiveV;
        if (current == keycode)
            return;
        if (current != -1)
            SDLActivity.onNativeKeyUp(current);
        if (horizontal) padActiveH = keycode; else padActiveV = keycode;
        if (keycode != -1)
            SDLActivity.onNativeKeyDown(keycode);
    }

    private void updatePadDirection(float x, float y) {
        setPadAxis(true, padHorizontalKeycode(x, y));
        setPadAxis(false, padVerticalKeycode(x, y));
    }

    private void clearPadDirection() {
        setPadAxis(true, -1);
        setPadAxis(false, -1);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getActionMasked();
        int index = event.getActionIndex();

        switch (action) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN: {
                int id = event.getPointerId(index);
                float x = event.getX(index);
                float y = event.getY(index);

                if (x < padZoneRight && padPointerId == -1) {
                    padPointerId = id;
                    updatePadDirection(x, y);
                    invalidate();
                    return true;
                }
                return false;
            }
            case MotionEvent.ACTION_MOVE: {
                for (int i = 0; i < event.getPointerCount(); i++) {
                    int id = event.getPointerId(i);
                    if (id == padPointerId)
                        updatePadDirection(event.getX(i), event.getY(i));
                }
                return padPointerId != -1;
            }
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP: {
                int id = event.getPointerId(index);
                if (id == padPointerId) {
                    padPointerId = -1;
                    clearPadDirection();
                    invalidate();
                    return true;
                }
                return false;
            }
            case MotionEvent.ACTION_CANCEL: {
                clearPadDirection();
                padPointerId = -1;
                invalidate();
                return true;
            }
        }

        return false;
    }
}
