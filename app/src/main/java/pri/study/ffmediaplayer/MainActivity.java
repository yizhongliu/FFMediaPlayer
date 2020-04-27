package pri.study.ffmediaplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import pri.tool.ffmediaplayer.MediaPlayer;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        MediaPlayer mediaPlayer = new MediaPlayer();
        mediaPlayer.testCallback(true);
    }
}
