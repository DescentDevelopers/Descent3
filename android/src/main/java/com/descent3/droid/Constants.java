package com.descent3.droid;

import android.content.Context;

import java.io.File;
import java.nio.file.Path;

public class Constants {
  private static Path getGameDataDirFrom(File base) {
    return base.toPath().resolve("gamedata");
  }

  public static Path getCacheGameDataDir(Context ctx) {
    return getGameDataDirFrom(ctx.getCacheDir());
  }

  public static Path getFileGameDataDir(Context ctx) {
    return getGameDataDirFrom(ctx.getFilesDir());
  }

  public static boolean isGameDataPresent(Context ctx) {
    return getCacheGameDataDir(ctx).resolve("d3.hog").toFile().exists();
  }
}
