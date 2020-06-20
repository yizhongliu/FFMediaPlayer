package pri.tool.ffmediaplayer;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

import java.io.File;
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
    private SurfaceHolder mSurfaceHolder;
    private EventHandler mEventHandler;

    static {
        System.loadLibrary("nativeffmpeg");
        native_init();
    }

    public MediaPlayer() {
        Log.e(TAG, "new MediaPlayer");
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
        Log.e(TAG, "setDisplay");
        mSurfaceHolder = sh;
        Surface surface;
        if (sh != null) {
            surface = sh.getSurface();
        } else {
            surface = null;
        }
        native_setSurface(surface);
    }

    public void setDataSource(String path)
            throws IOException, IllegalArgumentException, SecurityException, IllegalStateException {

        Log.e(TAG, "setDataSource:" + path);
        final File file = new File(path);
        if (file.exists()) {
            native_setDataSource(path);
        } else {
            throw new IOException("setDataSource failed.");
        }
    }

    public void start() throws IllegalStateException {
        try {
            native_start();
        } catch (IllegalStateException e) {

        }
    }

    public void pause() throws IllegalStateException {
        Log.e(TAG, "pause");
        native_pause();
    }

    public void prepareAsync() throws IllegalStateException {
        Log.e(TAG, "prepareAsync()");
        native_prepareAsync();
    }

    public void stop() throws IllegalStateException {
        Log.e(TAG, "stop");
        native_stop();
    }

    public void reset() {
        Log.e(TAG, "reset");
        native_reset();

        // make sure none of the listeners get called anymore
        if (mEventHandler != null) {
            mEventHandler.removeCallbacksAndMessages(null);
        }
    }

    public void release() {
        Log.e(TAG, "release");
        mOnPreparedListener = null;
        mOnCompletionListener = null;
        mOnErrorListener = null;

        native_release();
    }

    public interface OnPreparedListener {
        void onPrepared(MediaPlayer mp);
    }

    public void setOnPreparedListener(OnPreparedListener listener) {
        mOnPreparedListener = listener;
    }


    /* Do not change these values without updating their counterparts
     * in include/media/mediaplayer.h!
     */
    /** Unspecified media player error.
     * @see android.media.MediaPlayer.OnErrorListener
     */
    public static final int MEDIA_ERROR_UNKNOWN = 1;

    /** Media server died. In this case, the application must release the
     * MediaPlayer object and instantiate a new one.
     * @see android.media.MediaPlayer.OnErrorListener
     */
    public static final int MEDIA_ERROR_SERVER_DIED = 100;

    /** The video is streamed and its container is not valid for progressive
     * playback i.e the video's index (e.g moov atom) is not at the start of the
     * file.
     * @see android.media.MediaPlayer.OnErrorListener
     */
    public static final int MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 200;

    /** File or network related operation errors. */
    public static final int MEDIA_ERROR_IO = -1004;
    /** Bitstream is not conforming to the related coding standard or file spec. */
    public static final int MEDIA_ERROR_MALFORMED = -1007;
    /** Bitstream is conforming to the related coding standard or file spec, but
     * the media framework does not support the feature. */
    public static final int MEDIA_ERROR_UNSUPPORTED = -1010;
    /** Some operation takes too long to complete, usually more than 3-5 seconds. */
    public static final int MEDIA_ERROR_TIMED_OUT = -110;

    /**
     * Interface definition of a callback to be invoked when there
     * has been an error during an asynchronous operation (other errors
     * will throw exceptions at method call time).
     */
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

    /* Do not change these values without updating their counterparts
     * in macro.h!
     */
    private static final int MEDIA_NOP = 0; // interface test message
    private static final int MEDIA_PREPARED = 1;
    private static final int MEDIA_PLAYBACK_COMPLETE = 2;
    private static final int MEDIA_BUFFERING_UPDATE = 3;
    private static final int MEDIA_SEEK_COMPLETE = 4;
    private static final int MEDIA_SET_VIDEO_SIZE = 5;
    private static final int MEDIA_STARTED = 6;
    private static final int MEDIA_PAUSED = 7;
    private static final int MEDIA_STOPPED = 8;
    private static final int MEDIA_SKIPPED = 9;
    private static final int MEDIA_TIMED_TEXT = 99;
    private static final int MEDIA_ERROR = 100;
    private static final int MEDIA_INFO = 200;
    private static final int MEDIA_SUBTITLE_DATA = 201;
    private static final int MEDIA_META_DATA = 202;
    private static final int MEDIA_DRM_INFO = 210;


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
                case MEDIA_PREPARED:
                    if (mOnPreparedListener != null) {
                        mOnPreparedListener.onPrepared(mMediaPlayer);
                    }
                    break;
                case MEDIA_PLAYBACK_COMPLETE:
                    if (mOnCompletionListener != null) {
                        mOnCompletionListener.onCompletion(mMediaPlayer);
                    }
                    break;

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
    private native final void native_setDataSource(String filePath);
    private native final void native_testCallback(boolean bNewThread);
    private native final void native_prepareAsync();
    private native final void native_setSurface(Object surface);
    private native final void native_start() throws IllegalStateException;
    private native final void native_stop() throws IllegalStateException;
    private native final void native_reset() throws IllegalStateException;
    private native final void native_release() throws IllegalStateException;
    private native final void native_pause() throws IllegalStateException;
}
