// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

package org.quickbox.startertool;

import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.BitmapFactory;
import android.app.NotificationChannel;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;

import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.driver.UsbSerialPort;

import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;

public class SerialPort
{
	private enum UsbPermission { Unknown, Requested, Granted, Denied }
	private static final String INTENT_ACTION_GRANT_USB = BuildConfig.APPLICATION_ID + ".GRANT_USB";
	//private final BroadcastReceiver broadcastReceiver;
	private UsbSerialPort usbSerialPort;
	private UsbPermission usbPermission = UsbPermission.Unknown;
	private boolean connected = false;

	static class Info {
		UsbDevice device;
		int port;
		UsbSerialDriver driver;

		Info(UsbDevice device, int port, UsbSerialDriver driver) {
			this.device = device;
			this.port = port;
			this.driver = driver;
		}
	}
	//public SerialPort() {
	//	broadcastReceiver = new BroadcastReceiver() {
	//		@Override
	//		public void onReceive(Context context, Intent intent) {
	//			if(INTENT_ACTION_GRANT_USB.equals(intent.getAction())) {
	//				usbPermission = intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)
	//						? UsbPermission.Granted : UsbPermission.Denied;
	//				connect();
	//			}
	//		}
	//	};
	//	mainLooper = new Handler(Looper.getMainLooper());
	//}
	public static void testSerialPort(Context context)
	{
		try {
			// Find all available drivers from attached devices.
			UsbManager usbManager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
			System.err.println("USB usbManager: " + usbManager);
			System.err.println("device list: " + usbManager.getDeviceList());

			UsbSerialProber usbDefaultProber = UsbSerialProber.getDefaultProber();
			UsbSerialProber usbCustomProber = CustomProber.getCustomProber();
			ArrayList<Info> availableDrivers = new ArrayList<>();
			for(UsbDevice device : usbManager.getDeviceList().values()) {
				UsbSerialDriver driver = usbDefaultProber.probeDevice(device);
				if(driver == null) {
					driver = usbCustomProber.probeDevice(device);
				}
				if(driver != null) {
					for(int port = 0; port < driver.getPorts().size(); port++)
						availableDrivers.add(new Info(device, port, driver));
				}
			}
			System.out.println("Drivers available: " + availableDrivers);
			if (availableDrivers.isEmpty()) {
				return;
			}
			// Open a connection to the first available driver.
			Info info = availableDrivers.get(0);
			System.out.println("Opennig device: " + info.driver.getDevice());
			UsbDeviceConnection usbConnection = usbManager.openDevice(info.driver.getDevice());
			if(usbConnection == null /*&& usbPermission == UsbPermission.Unknown*/ && !usbManager.hasPermission(info.driver.getDevice())) {
				//usbPermission = UsbPermission.Requested;
				int flags = Build.VERSION.SDK_INT >= Build.VERSION_CODES.M ? PendingIntent.FLAG_MUTABLE : 0;
				Intent intent = new Intent(INTENT_ACTION_GRANT_USB);
				intent.setPackage(context.getPackageName());
				PendingIntent usbPermissionIntent = PendingIntent.getBroadcast(context, 0, intent, flags);
				usbManager.requestPermission(info.driver.getDevice(), usbPermissionIntent);
				return;
			}
			if(usbConnection == null) {
				if (!usbManager.hasPermission(info.driver.getDevice()))
					System.err.println("connection failed: permission denied");
				else
					System.err.println("connection failed: open failed");
				return;
			}
			try {
				UsbSerialPort usbSerialPort = info.driver.getPorts().get(info.port);
				usbSerialPort.open(usbConnection);
				try{
					usbSerialPort.setParameters(38400, 8, 1, UsbSerialPort.PARITY_NONE);
				}
				catch (UnsupportedOperationException e){
					System.err.println("unsupport setparameters");
				}
				//if(withIoManager) {
				//	usbIoManager = new SerialInputOutputManager(usbSerialPort, this);
				//	usbIoManager.start();
				//}
				System.err.println("connected");
				//connected = true;
				//controlLines.start();
			}
			catch (Exception e) {
				System.err.println("connection failed: " + e.getMessage());
				//disconnect();
			}
			//port.close();

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static String findSerialPort(Context context) {
		// System.err.println("FFFFFFFFFFFFFFFFFF+++++++++++++++++++++++++++++++++fff");
		UsbManager usbManager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
		UsbSerialProber usbDefaultProber = UsbSerialProber.getDefaultProber();
		UsbSerialProber usbCustomProber = CustomProber.getCustomProber();
		HashMap<String, UsbDevice> devices = usbManager.getDeviceList();
		for(UsbDevice device : devices.values()) {
			if (device.getVendorId() == 0x10c4 && device.getProductId() == 0x800a) {
				// sportident
				return device.getDeviceName();
			}
		}
		return "NOT FOUND";
	}
}
