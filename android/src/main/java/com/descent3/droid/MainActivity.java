package com.descent3.droid;

import android.content.Intent;
import android.os.Bundle;

import org.libsdl.app.SDLActivity;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;

public class MainActivity extends SDLActivity {

  private Path mUploadedGameData;
  private Path mBuiltinGameData;
  private Path mTempDir;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    try {
      mUploadedGameData = Constants.getUploadedGameDataDir(this);
      mBuiltinGameData = Constants.getBuiltinGameDataDir(this);
      mTempDir = Files.createTempDirectory("d3");
    } catch (IOException ex) {
      throw new RuntimeException(ex);
    }

    if (!Constants.isGameDataPresent(this)) {
      startActivity(new Intent(this, GameDataUploadActivity.class));
      finish();
    }
  }

  @Override
  protected void main() {
    // Runs on the SDL thread, so the (large) extraction doesn't block the UI thread.
    try {
      BuiltinGameData.extract(this, mBuiltinGameData);
    } catch (IOException ex) {
      throw new RuntimeException(ex);
    }
    super.main();
  }

  @Override
  protected String[] getLibraries() {
    return new String[]{
      "Descent3",
    };
  }

  @Override
  protected String[] getArguments() {
    ArrayList<String> args = new ArrayList<>(Arrays.asList(super.getArguments()));
    // The writable base dir (pilots, saves, config) is SDL_GetPrefPath(), i.e. filesDir.
    // Later -additionaldir entries are searched first, so builtin files (e.g. the Android netcon
    // .d3c modules) take precedence over same-named retail ones in the uploaded data.
    args.add("-additionaldir");
    args.add(mUploadedGameData.toAbsolutePath().toString());
    args.add("-additionaldir");
    args.add(mBuiltinGameData.toAbsolutePath().toString());
    args.add("-tempdir");
    args.add(mTempDir.toAbsolutePath().toString());
    return args.toArray(new String[0]);
  }
}
