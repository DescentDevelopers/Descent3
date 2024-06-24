package org.libsdl.app;

import android.hardware.usb.UsbDevice;

interface HIDDevice {
  int getId();

  int getVendorId();

  int getProductId();

  String getSerialNumber();

  int getVersion();

  String getManufacturerName();

  String getProductName();

  UsbDevice getDevice();

  boolean open();

  int sendFeatureReport(byte[] report);

  int sendOutputReport(byte[] report);

  boolean getFeatureReport(byte[] report);

  void setFrozen(boolean frozen);

  void close();

  void shutdown();
}
