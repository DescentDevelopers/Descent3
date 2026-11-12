package com.descent3.droid;

import android.content.Context;

import java.nio.file.Path;

public class Constants {
  /**
   * Retail game data uploaded by the user (d3.hog, missions, ...). Kept in no-backup storage: it's
   * far too big to back up, and unlike the cache dir the OS won't evict it under storage pressure.
   */
  public static Path getUploadedGameDataDir(Context ctx) {
    return ctx.getNoBackupFilesDir().toPath().resolve("gamedata");
  }

  /**
   * Staging area for in-progress uploads. Must be on the same filesystem as
   * {@link #getUploadedGameDataDir}, so a finished upload can be moved into place with a rename.
   */
  public static Path getUploadStagingDir(Context ctx) {
    return ctx.getNoBackupFilesDir().toPath().resolve("upload");
  }

  /** Game data extracted from the APK's assets; regenerable, so kept out of backups. */
  public static Path getBuiltinGameDataDir(Context ctx) {
    return ctx.getNoBackupFilesDir().toPath().resolve("builtin");
  }

  public static boolean isGameDataPresent(Context ctx) {
    return getUploadedGameDataDir(ctx).resolve("d3.hog").toFile().exists();
  }
}
