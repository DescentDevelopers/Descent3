package com.descent3.droid;

import static fi.iki.elonen.NanoHTTPD.Response.Status.BAD_REQUEST;
import static fi.iki.elonen.NanoHTTPD.Response.Status.INTERNAL_ERROR;
import static fi.iki.elonen.NanoHTTPD.Response.Status.METHOD_NOT_ALLOWED;
import static fi.iki.elonen.NanoHTTPD.Response.Status.NOT_FOUND;
import static fi.iki.elonen.NanoHTTPD.Response.Status.OK;

import android.app.Activity;
import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.text.Html;
import android.util.Log;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Optional;
import java.util.stream.Stream;

import fi.iki.elonen.NanoHTTPD;

public class GameDataUploadActivity extends Activity {
  private GameDataHttpServer mServer;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    mServer = new GameDataHttpServer(
      getAssets(),
      Constants.getCacheGameDataDir(this),
      () -> {
        // TODO: there's gonna be a race here; should explicitly have the Success html trigger this?
        startActivity(new Intent(this, MainActivity.class));
        finish();
      });

    setContentView(R.layout.game_data_layout_activity);
  }

  @Override
  protected void onStart() {
    super.onStart();
    try {
      mServer.start();
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
  }

  @Override
  protected void onStop() {
    super.onStop();
    mServer.stop();
  }

  private static class GameDataHttpServer extends NanoHTTPD {
    private static final String INTERNAL_ERROR_STRING =
      "<html>" +
        "<head><title>Internal Error</title></head>" +
        "<body><center>" +
        "<h1>Internal Error</h1>" +
        "<div>%s</div>" +
        "</center></body>" +
        "</html>";
    private final AssetManager mAssetManager;
    private final Path mGameDataDir;
    private final Runnable mOnUploadSuccess;

    GameDataHttpServer(AssetManager assetManager, Path gameDataDir, Runnable onUploadSuccess) {
      super(null, 6464);
      mAssetManager = assetManager;
      mGameDataDir = gameDataDir;
      mOnUploadSuccess = onUploadSuccess;

      setTempFileManagerFactory(() -> {
        try {
          return new GameDataTempFileManager();
        } catch (IOException e) {
          throw new RuntimeException(e);
        }
      });
    }

    private static void deleteAll(Path dir) {
      if (!Files.exists(dir)) {
        return;
      }

      try (Stream<Path> dirStream = Files.walk(dir)) {
        dirStream
          .sorted(Comparator.reverseOrder())
          .map(Path::toFile)
          .filter(f -> !f.delete())
          .forEach(f -> Log.w("csarbora", "Failed to delete " + f.getAbsolutePath()));
      } catch (IOException e) {
        Log.e("csarbora", "unable to walk " + dir.toAbsolutePath(), e);
      }
    }

    @Override
    public Response serve(IHTTPSession session) {
      try {
        switch (session.getMethod()) {
          case GET: {
            try {
              return assetResponse(OK, session.getUri());
            } catch (IOException e) {
              return assetResponse(NOT_FOUND, "/errors/404.html");
            }
          }
          case POST: {
            deleteAll(mGameDataDir);

            HashMap<String, String> files = new HashMap<>();
            try {
              session.parseBody(files);
            } catch (ResponseException e) {
              throw new IOException(e);
            }

            Optional<Path> base = files.values().stream()
              .filter(path -> path.endsWith("d3.hog"))
              .findAny()
              .map(Paths::get)
              .map(Path::getParent);

            if (!base.isPresent()) {
              return assetResponse(BAD_REQUEST, "/errors/400_no_d3hog.html");
            }

            Files.move(base.get(), mGameDataDir);
            mOnUploadSuccess.run();
            return assetResponse(OK, "/success.html");
          }
          default:
            return assetResponse(METHOD_NOT_ALLOWED, "/errors/405.html");
        }
      } catch (IOException e) {
        return newFixedLengthResponse(
          INTERNAL_ERROR,
          "text/html",
          String.format(INTERNAL_ERROR_STRING, Html.escapeHtml(e.toString())));
      }
    }

    private Response assetResponse(Response.Status status, String uri) throws IOException {
      uri = uri.substring(1);
      if (uri.isEmpty()) {
        uri = "index.html";
      }

      return newChunkedResponse(
        status,
        getMimeTypeForFile(uri),
        mAssetManager.open(uri, AssetManager.ACCESS_STREAMING));
    }

    private static class GameDataFile implements TempFile {
      private final Path mPath;

      public GameDataFile(Path base, String filename) throws IOException {
        if (filename != null) {
          mPath = base.resolve(filename);
          Files.createDirectories(mPath.getParent());
          mPath.getParent().toFile().mkdirs();
        } else {
          mPath = Files.createTempFile(base, "tmp", null);
        }
      }

      @Override
      public void delete() {
        try {
          Files.delete(mPath);
        } catch (IOException ignored) {
        }
      }

      @Override
      public String getName() {
        return mPath.toAbsolutePath().toString();
      }

      @Override
      public OutputStream open() throws Exception {
        return Files.newOutputStream(mPath);
      }
    }

    private static class GameDataTempFileManager implements TempFileManager {
      private final Path mBase;

      GameDataTempFileManager() throws IOException {
        mBase = Files.createTempDirectory("gamedata");
      }

      @Override
      public void clear() {
        deleteAll(mBase);
      }

      @Override
      public TempFile createTempFile(String filename_hint) throws IOException {
        return new GameDataFile(mBase, filename_hint);
      }

      public Path getBase() {
        return mBase;
      }
    }
  }
}
