package eu.basicairdata.graziano.bluetoothhelperexample;

/**
 * BluetoothHelper Java Helper Class for Android
 * Created by G.Capelli (BasicAirData) on 06/02/16.
 * v.1.0.2
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 **/

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Set;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;


public class BluetoothHelper {

    private BluetoothAdapter mBluetoothAdapter = null;
    private boolean isInStreamConnected = false;
    private boolean isOutStreamConnected = false;
    private ConnectThread CT = null;
    private ConnectedThreadClass_Read readThread = null;
    private ConnectedThreadClass_Write writeThread = null;
    private Handler handler = new Handler();
    private BlockingQueue<String> inputMessagesQueue = new LinkedBlockingQueue<String>();
    private BlockingQueue<String> outputMessagesQueue = new LinkedBlockingQueue<String>();


    // -------------------------------------------------------------------------------------------- The thread class that takes care of connection process
    private class ConnectThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final BluetoothDevice mmDevice;
        private BluetoothSocket tmp = null;

        public ConnectThread(BluetoothDevice device) {
            // Use a temporary object that is later assigned to mmSocket,
            // because mmSocket is final
            mmDevice = device;
            try {
                Method m = mmDevice.getClass().getMethod("createRfcommSocket", new Class[]{int.class});
                try {
                    tmp = (BluetoothSocket) m.invoke(mmDevice, 1);
                } catch (IllegalAccessException e) {
                    //Log.w("myApp", "[!] Unable to connect socket (IllegalAccessException): " + e);
                } catch (InvocationTargetException e) {
                    //Log.w("myApp", "[!] Unable to connect socket (InvocationTargetException): " + e);
                }
            } catch (NoSuchMethodException e) {
                //Log.w("myApp", "[!] Unable to connect socket (NoSuchMethodException): " + e);
            }
            mmSocket = tmp;
        }

        public void run() {
            // Cancel discovery because it will slow down the connection
            mBluetoothAdapter.cancelDiscovery();
            try {
                mmSocket.connect();
            } catch (IOException connectException) {
                //Log.w("myApp", "[!] Connection through socket failed: " + connectException);
                //Log.w("myApp", "[!] Trying fallback method");
                try {
                    // fallback method for android >= 4.2
                    tmp = (BluetoothSocket) mmDevice.getClass().getMethod("createRfcommSocket", new Class[]{int.class}).invoke(mmDevice, 1);
                } catch (IllegalAccessException e) {
                    //Log.w("myApp", "[!] Failed to create fallback Illegal Access: " + e);
                    return;
                } catch (IllegalArgumentException e) {
                    //Log.w("myApp", "[!] Failed to create fallback Illegal Argument: " + e);
                    return;
                } catch (InvocationTargetException e) {
                    //Log.w("myApp", "[!] Failed to create fallback Invocation Target" + e);
                    return;
                } catch (NoSuchMethodException e) {
                    //Log.w("myApp", "[!] Failed to create fallback No Such Method" + e);
                    return;
                }
                try {
                    // linked to tmp, so basicly a new socket
                    mmSocket.connect();
                } catch (IOException e) {
                    //Log.w("myApp", "[!] Failed to connect with fallback socket: " + e);
                    handler.post(new Runnable() {
                        @Override
                        public void run() {
                            InCaseFireonBluetoothHelperConnectionStateChanged();
                        }
                    });
                    return;
                }
                //Log.w("myApp", "[#] Succesfully connected with fallback socket");
            }
            // Do work to manage the connection (in a separate thread)
            // manageConnectedSocket(mmSocket);
            //Log.w("myApp", "[#] Socket connected. Opening streams....");
            readThread = new ConnectedThreadClass_Read(mmSocket);
            readThread.start();
            writeThread = new ConnectedThreadClass_Write(mmSocket);
            writeThread.start();

            boolean oldConnectionStatus = false;
            // Check for termination request
            do {
                //checkConnectionStatus();
                try {
                    sleep(100);
                } catch (InterruptedException e) {
                    //Log.w("myApp", "[!] ConnectThread Interrupted");
                    break;
                }
                // check the status of the connection and send listeners in case of changes
                if (isConnected() != oldConnectionStatus) {
                    oldConnectionStatus = isConnected();
                    handler.post(new Runnable() {
                        @Override
                        public void run() {
                            InCaseFireonBluetoothHelperConnectionStateChanged();
                        }
                    });
                }
            } while (isConnected());
            cancel();
            //checkConnectionStatus();
            //Log.w("myApp", "[#] Socket closed");
        }

        //Will cancel an connection and close streams and socket
        public void cancel() {
            isInStreamConnected = false;
            isOutStreamConnected = false;
            try {
                readThread.interrupt();
                writeThread.interrupt();
                if (mmSocket.isConnected()) mmSocket.close();
            } catch (IOException e) {
            }
        }
    }


    private void InCaseFireonBluetoothHelperConnectionStateChanged() {
        if (listener != null) {
            //if (isConnected()) Log.w("myApp", "[#] Listener fired: onBluetoothHelperConnectionStateChanged = true");
            //else Log.w("myApp", "[#] Listener fired: onBluetoothHelperConnectionStateChanged = false");
            listener.onBluetoothHelperConnectionStateChanged(this, isConnected()); // <---- fire listener
        }
    }


    // -------------------------------------------------------------------------------------------- The thread class that READS messages from BT
    private class ConnectedThreadClass_Read extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;

        public ConnectedThreadClass_Read(BluetoothSocket socket) {
            mmSocket = socket;
            InputStream tmpIn = null;
            // Get the input and output streams, using temp objects because
            // member streams are final
            try {
                tmpIn = mmSocket.getInputStream();
            } catch (IOException e) {
            }
            mmInStream = tmpIn;
            isInStreamConnected = true;
        }

        public void run() {
            //Log.w("myApp", "[#] Input Stream opened");
            // Keep listening to the InputStream until an exception occurs
            while (true) {
                byte[] buffer = new byte[1024];  // buffer store for the stream
                byte ch;
                int i = 0;
                try {
                    // Read from the InputStream until DELIMITER found
                    while ((ch = (byte) mmInStream.read()) != Delimiter) {
                        buffer[i++] = ch;
                    }
                    buffer[i] = '\0';
                    final String msg = new String(buffer);
                    MessageReceived(msg.trim());
                } catch (IOException e) {
                    isInStreamConnected = false;
                    break;
                }
            }
            isInStreamConnected = false;
            //Log.w("myApp", "[#] Input stream closed");
        }
    }

    // Service function: Message received!
    private void MessageReceived(String msg) {
        // if the listener is attached fire it
        // else put the message into buffer to be read
        //Log.w("myApp", "[#] Message received: " + msg);
        try {
            if (listener != null) {
                //Log.w("myApp", "[#]  Listener fired: onBluetoothHelperMessageReceived");
                listener.onBluetoothHelperMessageReceived(this, msg); // <---- fire listener
            } else if (!inputMessagesQueue.offer(msg))
                //Log.w("myApp", "[!] Message thrown (unable to store into buffer): " + msg)
                ;
        } catch (Exception e) {
            //Log.w("myApp", "[!] Failed to receive message: " + e.getMessage());
        }
    }


    // -------------------------------------------------------------------------------------------- The thread class that WRITES messages to BT
    private class ConnectedThreadClass_Write extends Thread {
        private final BluetoothSocket mmSocket;
        private final OutputStream mmOutStream;

        public ConnectedThreadClass_Write(BluetoothSocket socket) {
            mmSocket = socket;
            OutputStream tmpOut = null;
            // Get the input and output streams, using temp objects because
            // member streams are final
            try {
                tmpOut = mmSocket.getOutputStream();
            } catch (IOException e) {
            }
            mmOutStream = tmpOut;
            isOutStreamConnected = true;
        }

        public void run() {
            //Log.w("myApp", "[#] Output Stream opened");
            // Keep sending messages to OutputStream until an exception occurs
            while (true) {
                String msg;
                try {
                    msg = outputMessagesQueue.take();
                } catch (InterruptedException e) {
                    isOutStreamConnected = false;
                    //Log.w("myApp", "[!] Buffer not available: " + e.getMessage());
                    break;
                }
                try {
                    mmOutStream.write(msg.getBytes());
                    mmOutStream.write(Delimiter);
                    //Log.w("myApp", "[#] Message send: " + msg);
                } catch (IOException e) {
                    isOutStreamConnected = false;
                    //Log.w("myApp", "[!] Unable to write data to output stream: " + e.getMessage());
                    break;
                }
            }
            isOutStreamConnected = false;
            //Log.w("myApp", "[#] Output stream closed");
        }
    }


    // -------------------------------------------------------------------------------------------- Listeners interface
    public interface BluetoothHelperListener {
        // These methods are the different events and
        // need to pass relevant arguments related to the event triggered
        public void onBluetoothHelperMessageReceived(BluetoothHelper bluetoothhelper, String message);

        public void onBluetoothHelperConnectionStateChanged(BluetoothHelper bluetoothhelper, boolean isConnected);
    }


    // -------------------------------------------------------------------------------------------- MAIN CLASS METHODS

    // This variable represents the listener passed in by the owning object
    // The listener must implement the events interface and passes messages up to the parent.
    private BluetoothHelperListener listener;

    // The messages delimiter. That character is the separator between every
    // incoming and send messages.
    public char Delimiter = '\n';


    // Assign the listener implementing events interface that will receive the events
    public void setBluetoothHelperListener(BluetoothHelperListener listener) {
        this.listener = listener;
    }


    // It returns true if the connection is up, false otherwise.
    public boolean isConnected() {
        return (isInStreamConnected && isOutStreamConnected);
    }


    // It bootstraps the connection to the paired device with corresponding name.
    // The function does return immediately, when the connecting process is in progress.
    public void Connect(String DeviceName) {
        if (!isConnected()) {
            Disconnect(false);

            mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();                               // Find adapter
            if (mBluetoothAdapter.isEnabled()) {                                                    // Adapter found
                Set<BluetoothDevice> devices = mBluetoothAdapter.getBondedDevices();                // Collect all bonded devices
                for (BluetoothDevice bt : devices) {
                    if (DeviceName.equals(bt.getName())) {                                          // Find requested device name
                        //Log.w("myApp", "[#] Devicename match found: " + bt.getName());
                        CT = new ConnectThread(bt);
                        CT.start();
                    } //else Log.w("myApp", "[#] Devicename doesn't match: " + bt.getName());
                }
            }
        }
    }

    // It cleans the in/out buffers
    public void ClearBuffer() {
        inputMessagesQueue.clear();                          // Clear the input message queue;
        outputMessagesQueue.clear();                         // Clear the output message queue;
    }


    // Disconnect, It cleans also the in/out buffers by default
    public void Disconnect() {
        if (CT != null) {
            if (CT.isAlive()) CT.cancel();
        }
        ClearBuffer();
    }

    // Disconnect, specifying if you want to clean the in/out buffers
    public void Disconnect(boolean ClearBuffer) {
        if (CT != null) {
            if (CT.isAlive()) CT.cancel();
        }
        if (ClearBuffer) ClearBuffer();
    }


    // Receive a message from the INPUT buffer (and delete it from RECEIVE queue);
    // Use this function if you don't have listener attached.
    // Listener method is the preferred one.
    public String ReceiveMessage() {
        String m = inputMessagesQueue.poll();
        return (m != null ? m : "");
    }


    // Send a message to the OUTPUT buffer (add it to SEND queue)
    public boolean SendMessage(String msg) {
        if (isConnected()) {
            return (outputMessagesQueue.offer(msg));
        } else return false;
    }
}
