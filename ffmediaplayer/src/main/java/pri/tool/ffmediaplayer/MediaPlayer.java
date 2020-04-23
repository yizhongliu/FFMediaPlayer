package pri.tool.ffmediaplayer;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;

import java.io.IOException;
import java.lang.ref.WeakReference;

public class MediaPlayer{
    private static final String TAG = "MediaPlayer";

    /*
    *  jni层用static的变量指向这两个变量
    *  jni在创建了native层的player后会将native player设置给mNativeContext，mNativeContext跟实例相关
    *  Android 的MediaPlayer接口有可能同时被多个实例创建， 需要保存对应的实例，而jni层用了static，跟对象无关
    *  后续jni 对native player进行调用时，需要获取到对应的实例对象， 通过mNativeContext获取，即获取到对应的nativie player实例
    * */
    private long mNativeContext; // accessed by native method, java层保存的nativie层 mediaplayer对象
    private long mNativeSurfaceTexture;  // accessed by native methods

    private OnPreparedListener mOnPreparedListener;
    private OnCompletionListener mOnCompletionListener;
    private OnErrorListener mOnErrorListener;

    private EventHandler mEventHandler;

    static {
        System.loadLibrary("nativeffmpeg");
        native_init();
    }

    public MediaPlayer() {
        Looper looper;
        if ((looper = Looper.myLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else if ((looper = Looper.getMainLooper()) != null) {
            mEventHandler = new EventHandler(this, looper);
        } else {
            mEventHandler = null;
        }

        /* Native setup requires a weak reference to our object.
         * It's easier to create it here than in C++.
         */
        native_setup(new WeakReference<MediaPlayer>(this));
    }

    public void setDisplay(SurfaceHolder sh) {

    }

    public void setDataSource(String path)
            throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {

    }

    public void start() throws IllegalStateException {

    }

    public void prepareAsync() throws IllegalStateException {

    }

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

    private class EventHandler extends Handler
    {
        private MediaPlayer mMediaPlayer;

        public EventHandler(MediaPlayer mp, Looper looper) {
            super(looper);
            mMediaPlayer = mp;
        }

        @Override
        public void handleMessage(Message msg) {
            if (mMediaPlayer.mNativeContext == 0) {
                Log.w(TAG, "mediaplayer went away with unhandled events");
                return;
            }
            switch(msg.what) {

                default:
                    Log.e(TAG, "Unknown message type " + msg.what);
                    return;
            }
        }
    }

    //Jni 层回调的函数
    private static void postEventFromNative(Object mediaplayer_ref,
                                            int what, int arg1, int arg2)
    {
        final MediaPlayer mp = (MediaPlayer)((WeakReference)mediaplayer_ref).get();
        if (mp == null) {
            return;
        }

        if (mp.mEventHandler != null) {
            Message m = mp.mEventHandler.obtainMessage(what, arg1, arg2);
            mp.mEventHandler.sendMessage(m);
        }
    }

    public void testCallback(boolean bNewThread) {
        native_testCallback(bNewThread);
    }

    private static native final void native_init();
    private native final void native_setup(Object mediaplayer_this);
    private native final void native_testCallback(boolean bNewThread);
}
