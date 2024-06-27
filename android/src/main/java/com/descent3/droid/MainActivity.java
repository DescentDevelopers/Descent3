package com.descent3.droid;

import android.os.storage.OnObbStateChangeListener;
import android.os.storage.StorageManager;

import org.libsdl.app.SDLActivity;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.concurrent.CountDownLatch;

public class MainActivity extends SDLActivity {
    @Override
    protected String[] getLibraries() {
        return new String[] {
            "Descent3",
        };
    }

    private String obbFile() {
      return new File(getObbDir(), "d3_gamedata.obb").getAbsolutePath();
    }

    @Override
    protected String[] getArguments() {
      ArrayList<String> args = new ArrayList<>(Arrays.asList(super.getArguments()));
      args.add("-setdir");
      StorageManager storageManager = getSystemService(StorageManager.class);
      Thread t = new Thread(() -> {
        CountDownLatch latch = new CountDownLatch(1);
        storageManager.mountObb(obbFile(), null, new OnObbStateChangeListener() {
          @Override
          public void onObbStateChange(String path, int state) {
            latch.countDown();
          }
        });
        try {
          latch.await();
        } catch (InterruptedException e) {
          throw new RuntimeException(e);
        }
      });
      t.start();
      try {
        t.join();
      } catch (InterruptedException e) {
        throw new RuntimeException(e);
      }
      args.add(storageManager.getMountedObbPath(obbFile()));
      args.add("-tempdir");
      args.add(getFilesDir().getAbsolutePath());

      return args.toArray(new String[0]);
    }
}
