package com.descent3.droid;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

/**
 * Entry point: sends the user to the game, or to the upload screen if there's no game data yet.
 *
 * This must not be done from MainActivity itself: SDLActivity exits the process when it's created
 * a second time, so MainActivity redirecting to the upload screen and later being started again
 * would kill the app (and the upload server, mid-cleanup).
 */
public class LauncherActivity extends Activity {
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    Class<? extends Activity> next =
      Constants.isGameDataPresent(this) ? MainActivity.class : GameDataUploadActivity.class;
    startActivity(new Intent(this, next));
    finish();
  }
}
