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

  private Path mFilesGameData;
  private Path mCacheGameData;
  private Path mTempDir;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    try {
      mFilesGameData = Constants.getFileGameDataDir(this);
      Files.createDirectories(mFilesGameData);
      mCacheGameData = Constants.getCacheGameDataDir(this);
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
  protected String[] getLibraries() {
    return new String[]{
      "Descent3",
    };
  }

  @Override
  protected String[] getArguments() {
    ArrayList<String> args = new ArrayList<>(Arrays.asList(super.getArguments()));
    args.add("-setdir");
    args.add(mFilesGameData.toAbsolutePath().toString());
    args.add("-additionaldir");
    args.add(mCacheGameData.toAbsolutePath().toString());
    args.add("-tempdir");
    args.add(mTempDir.toAbsolutePath().toString());
    return args.toArray(new String[0]);
  }
}
