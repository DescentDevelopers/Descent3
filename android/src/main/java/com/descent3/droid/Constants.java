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

  /** Game data extracted from the APK's assets; regenerable, so kept out of backups. */
  public static Path getBuiltinGameDataDir(Context ctx) {
    return ctx.getNoBackupFilesDir().toPath().resolve("builtin");
  }

  public static boolean isGameDataPresent(Context ctx) {
    return getCacheGameDataDir(ctx).resolve("d3.hog").toFile().exists();
  }
}
