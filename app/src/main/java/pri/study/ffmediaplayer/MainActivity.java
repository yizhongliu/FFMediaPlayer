package pri.study.ffmediaplayer;


import androidx.appcompat.app.AppCompatActivity;

//import android.media.MediaPlayer;
import android.Manifest;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import com.tbruyelle.rxpermissions2.Permission;
import com.tbruyelle.rxpermissions2.RxPermissions;

import java.io.File;
import java.io.IOException;

import io.reactivex.functions.Consumer;
import pri.tool.ffmediaplayer.MediaPlayer;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private final static String TAG = "MainActivity";

    private MediaPlayer mediaPlayer;

    private SurfaceHolder surfaceHolder;
    private SurfaceView surfaceView;

    private OnCompletionListener onCompletionListener;
    private OnErrorListener onErrorListener;
    private OnPreparedListener onPreparedListener;

    Button stopButton;
    Button resetButton;
    Button releaseButton;
    Button setDataSourceButton;
    Button prepareButton;
    Button restarButton;
    Button pauseButton;
    Button startButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        checkRxPermission();

        initView();
        initMediaPlayer();
        initSurfaceView();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        mediaPlayer.reset();
        mediaPlayer.release();
    }

    private void initView() {
        surfaceView = findViewById(R.id.surfaceView);

        stopButton = findViewById(R.id.stop);
        stopButton.setOnClickListener(this);

        resetButton = findViewById(R.id.reset);
        resetButton.setOnClickListener(this);

        releaseButton = findViewById(R.id.release);
        releaseButton.setOnClickListener(this);

        setDataSourceButton = findViewById(R.id.setDataSource);
        setDataSourceButton.setOnClickListener(this);

        prepareButton = findViewById(R.id.prepare);
        prepareButton.setOnClickListener(this);

        restarButton = findViewById(R.id.restart);
        restarButton.setOnClickListener(this);

        pauseButton = findViewById(R.id.pause);
        pauseButton.setOnClickListener(this);

        startButton = findViewById(R.id.start);
        startButton.setOnClickListener(this);
    }

    private void initMediaPlayer() {
        onCompletionListener = new OnCompletionListener();
        onErrorListener = new OnErrorListener();
        onPreparedListener = new OnPreparedListener();

        mediaPlayer = new MediaPlayer();
        mediaPlayer.setOnCompletionListener(onCompletionListener);
        mediaPlayer.setOnErrorListener(onErrorListener);
        mediaPlayer.setOnPreparedListener(onPreparedListener);
    }

    private void initSurfaceView() {
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                surfaceHolder = holder;
                mediaPlayer.setDisplay(holder);
                preparePlayer(holder);
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                Log.e(TAG, "surfaceDestory");
            }
        });
    }

    public void preparePlayer(SurfaceHolder holder) {
        File file = new File(getExternalFilesDir(null), "test.mp4");
     //   File file = new File(Environment.getExternalStorageDirectory() + File.separator + "test.mp4");
        String path = file.getAbsolutePath();


        Log.e("test", "Path:" + path);

        try {
            mediaPlayer.setDataSource(path);
        } catch (IOException e) {
            e.printStackTrace();
        }

   //     mediaPlayer.setDisplay(holder);
        mediaPlayer.prepareAsync();
    }

    /*
    * 这里button只做测试, 没考虑接口是否可以在这个条件下调用, 主要是为了测试 以下成对的逻辑调用
    * stop  --- prepare  (stop后需要重新调用prepare)
    * reset --- setDataSource  (reset后需要重新调用setDataSource setDisplay prepare)
    * release --- restart (release后需要重新new MeidaPlayer)
    * pause --- start
    * */
    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.stop:
                mediaPlayer.stop();
                break;
            case R.id.reset:
                mediaPlayer.reset();
                break;
            case R.id.release:
                mediaPlayer.release();
                break;
            case R.id.setDataSource: {
             //   File file = new File(getExternalFilesDir(null), "test.mp4");
                File file = new File(Environment.getExternalStorageDirectory() + File.separator + "test.mp4");
                String path = file.getAbsolutePath();
                try {
                    mediaPlayer.setDataSource(path);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                mediaPlayer.setDisplay(surfaceHolder);
                mediaPlayer.prepareAsync();
                break;
            }
            case R.id.prepare:
                mediaPlayer.prepareAsync();
                break;
            case R.id.restart:{
                Log.e(TAG, "restart");
                mediaPlayer = new MediaPlayer();
                mediaPlayer.setOnCompletionListener(onCompletionListener);
                mediaPlayer.setOnErrorListener(onErrorListener);
                mediaPlayer.setOnPreparedListener(onPreparedListener);


                File file = new File(getExternalFilesDir(null), "test.mp4");
                String path = file.getAbsolutePath();
                try {
                    mediaPlayer.setDataSource(path);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                mediaPlayer.setDisplay(surfaceHolder);
                mediaPlayer.prepareAsync();
                break;
            }

            case R.id.pause: {
                mediaPlayer.pause();
                break;
            }

            case R.id.start: {
                mediaPlayer.start();
                break;
            }
        }
    }


    public class OnPreparedListener implements MediaPlayer.OnPreparedListener {

        @Override
        public void onPrepared(MediaPlayer mp) {
            Log.e(TAG, "onPrepared");
            mediaPlayer.start();
        }
    }

    public class OnErrorListener implements MediaPlayer.OnErrorListener {

        @Override
        public boolean onError(MediaPlayer mp, int what, int extra) {
            return false;
        }
    }

    public class OnCompletionListener implements MediaPlayer.OnCompletionListener {

        @Override
        public void onCompletion(MediaPlayer mp) {
            Log.e(TAG, "onCompletion");
        }
    }

    public void checkRxPermission() {
        RxPermissions rxPermission = new RxPermissions(this);
        rxPermission
                .requestEach(
                        Manifest.permission.READ_EXTERNAL_STORAGE,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE)
                .subscribe(new Consumer<Permission>() {
                    @Override
                    public void accept(Permission permission) throws Exception {
                        if (permission.granted) {
                            Log.d(TAG, " permission accept");
                        } else if (permission.shouldShowRequestPermissionRationale) {
                            // 用户拒绝了该权限，没有选中『不再询问』（Never ask again）,那么下次再次启动时，还会提示请求权限的对话框
                            Log.e(TAG, permission.name + " is denied. More info should be provided.");
                            finish();
                        } else {
                            // 用户拒绝了该权限，并且选中『不再询问』
                            Log.d(TAG, permission.name + " is denied.");
                            finish();
                        }
                    }
                });
    }
}
