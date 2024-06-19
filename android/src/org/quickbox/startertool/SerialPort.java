package org.quickbox.startertool;

import android.content.Intent;
import android.os.Build;
import android.app.PendingIntent;
import android.content.Context;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;

import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import java.io.IOException;
import java.util.HashMap;

public class SerialPort implements SerialInputOutputManager.Listener
{
    //private enum UsbPermission { Unknown, Requested, Granted, Denied }
    private static final String INTENT_ACTION_GRANT_USB = BuildConfig.APPLICATION_ID + ".GRANT_USB";
    //private final BroadcastReceiver broadcastReceiver;
    private UsbSerialPort usbSerialPort = null;
    //private UsbPermission usbPermission = UsbPermission.Unknown;
    private boolean isopen = false;
    private static final boolean withIoManager = true;
    private SerialInputOutputManager usbIoManager;

    private static native void nativeDeviceException(String message);
    private static native void nativeDeviceNewData(byte[] data);

    @Override
    public void onNewData(byte[] data) {
        nativeDeviceNewData(data);
    }

    @Override
    public void onRunError(Exception e) {
        System.err.println("Run error: " + e.getMessage());
        nativeDeviceException(e.getMessage());
        //close();
    }

    public static String findSerialPort(Context context) {
        UsbManager usbManager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
        HashMap<String, UsbDevice> devices = usbManager.getDeviceList();
        for(UsbDevice device : devices.values()) {
            if (device.getVendorId() == 0x10c4 && device.getProductId() == 0x800a) {
                // sportident
                return device.getDeviceName();
            }
        }
        return "";
    }

    public static SerialPort create()
    {
        return new SerialPort();
    }

	public boolean isOpen()
	{
		return isopen;
	}

	public void test()
	{
		System.err.println("TEST");
	}

    public void open(Context context)
    {
        try {
            close();
            // Find all available drivers from attached devices.
            UsbManager usbManager = (UsbManager) context.getSystemService(Context.USB_SERVICE);
            System.err.println("USB usbManager: " + usbManager);
            System.err.println("device list: " + usbManager.getDeviceList());

            UsbSerialProber usbDefaultProber = UsbSerialProber.getDefaultProber();
            UsbSerialProber usbCustomProber = CustomProber.getCustomProber();
            UsbSerialDriver driver = null;
            for(UsbDevice device : usbManager.getDeviceList().values()) {
                driver = usbDefaultProber.probeDevice(device);
                if(driver == null) {
                    driver = usbCustomProber.probeDevice(device);
                }
                if(driver != null) {
                    break;
                }
            }
            if (driver == null) {
                throw new Exception("No serial port found.");
            }
            // Open a connection to the first available driver.
            System.err.println("Opennig device: " + driver.getDevice());
            UsbDeviceConnection usbConnection = usbManager.openDevice(driver.getDevice());
            if(usbConnection == null /*&& usbPermission == UsbPermission.Unknown*/ && !usbManager.hasPermission(driver.getDevice())) {
				System.err.println("Requesting permissions");
                //usbPermission = UsbPermission.Requested;
                int flags = Build.VERSION.SDK_INT >= Build.VERSION_CODES.M ? PendingIntent.FLAG_MUTABLE : 0;
                Intent intent = new Intent(INTENT_ACTION_GRANT_USB);
                intent.setPackage(context.getPackageName());
                PendingIntent usbPermissionIntent = PendingIntent.getBroadcast(context, 0, intent, flags);
                usbManager.requestPermission(driver.getDevice(), usbPermissionIntent);
                return;
            }
            if(usbConnection == null) {
                if (!usbManager.hasPermission(driver.getDevice()))
                    throw new Exception("connection failed: permission denied");
                else
                    throw new Exception("connection failed: open failed");
            }
            try {
                usbSerialPort = driver.getPorts().get(0);
                usbSerialPort.open(usbConnection);
                try{
                    usbSerialPort.setParameters(38400, 8, 1, UsbSerialPort.PARITY_NONE);
                }
                catch (UnsupportedOperationException e){
                    throw new Exception("unsupport setparameters");
                }
                if(withIoManager) {
                	usbIoManager = new SerialInputOutputManager(usbSerialPort, this);
                	usbIoManager.start();
                }
                System.err.println("connected");
                isopen = true;
                //controlLines.start();
            }
            catch (Exception e) {
                System.err.println("connection failed: " + e.getMessage());
				nativeDeviceException("connection failed: " + e.getMessage());
                close();
            }
            //port.close();
        } catch (Exception e) {
            System.err.println("open failed: " + e.getMessage());
			nativeDeviceException("open failed: " + e.getMessage());
            //e.printStackTrace();
        }
    }

    public void close()
    {
        isopen = false;
        if(usbIoManager != null) {
            usbIoManager.setListener(null);
            usbIoManager.stop();
        }
        usbIoManager = null;
        if (usbSerialPort != null) {
            try {
                usbSerialPort.close();
            } catch (IOException ignored) {}
        }
        usbSerialPort = null;
    }
}
