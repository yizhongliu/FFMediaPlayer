package pri.tool.ffmediaplayer;

import android.view.SurfaceHolder;

import java.io.IOException;

public class MediaPlayer {

    private OnPreparedListener mOnPreparedListener;
    private OnCompletionListener mOnCompletionListener;
    private OnErrorListener mOnErrorListener;

    public MediaPlayer() {

    }

    public void setDisplay(SurfaceHolder sh) {

    }

    public void setDataSource(String path)
            throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {

    }

    public void start() throws IllegalStateException {

    }

    public native void prepareAsync() throws IllegalStateException;

    public interface OnPreparedListener {
        void onPrepared(MediaPlayer mp);
    }

    public void setOnPreparedListener(OnPreparedListener listener) {
        mOnPreparedListener = listener;
    }

    public interface OnErrorListener {
        boolean onError(MediaPlayer mp, int what, int extra);
    }

    public void setOnErrorListener(OnErrorListener listener) {
        mOnErrorListener = listener;
    }

    public interface OnCompletionListener {
        void onCompletion(MediaPlayer mp);
    }

    public void setOnCompletionListener(OnCompletionListener listener) {
        mOnCompletionListener = listener;
    }
}
