package eu.basicairdata.graziano.bluetoothhelperexample;

/**
 * BluetoothHelper Java Helper Class for Android - Example app
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
import android.content.pm.ActivityInfo;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;


public class BluetoothHelperExample extends AppCompatActivity {


    // Definition of strings to be sent ---------------
    String string_1 = "$STR,1";
    String string_2 = "$STR,2";
    // ------------------------------------------------

    BluetoothAdapter mBluetoothAdapter = null;
    BluetoothHelper mBluetooth = new BluetoothHelper();
    private TextView mTextViewStatus;
    private Button mButton1;
    private Button mButton2;


    private String DEVICE_NAME = "RNBT-729D";           // The name of the remote device (BlueSMIRF Gold)

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);  //force portrait screen
        setContentView(R.layout.activity_bluetooth_helper_example);

        // Link objects to UI
        mTextViewStatus = (TextView) findViewById(R.id.ID_STATUSTEXT);
        mButton1 = (Button) findViewById(R.id.ID_BUTTON1);
        mButton2 = (Button) findViewById(R.id.ID_BUTTON2);

        mButton1.setText(string_1);
        mButton2.setText(string_2);

        // Check if Bluetooth is supported by the device
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            // Device does not support Bluetooth
            finish();
        }

        // Start Bluetooth connection with the paired "RNBT-729D" device (BlueSMIRF Gold)
        mTextViewStatus.setText("Connecting to " + DEVICE_NAME);
        mBluetooth.Connect(DEVICE_NAME);


        // Setup listener for Bluetooth helper;
        mBluetooth.setBluetoothHelperListener(new BluetoothHelper.BluetoothHelperListener() {
            @Override
            public void onBluetoothHelperMessageReceived(BluetoothHelper bluetoothhelper, final String message) {
                Log.w("myApp", "[#] Received: " + message);
                mTextViewStatus.post(new Runnable() {
                                  public void run() {
                                      mTextViewStatus.setText(message);
                                  }
                              });

                // Do your stuff with the message received !!!
            }

            @Override
            public void onBluetoothHelperConnectionStateChanged(BluetoothHelper bluetoothhelper, boolean isConnected) {
                if (isConnected) {
                    mTextViewStatus.setText("Connected");
                    Log.w("myApp", "[#] Connected!");
                } else {
                    mTextViewStatus.setText("Disconnected");
                    Log.w("myApp", "[#] AutoReconnecting...");
                    // Auto reconnect!
                    mBluetooth.Connect(DEVICE_NAME);
                }
            }
        });
    }

    // The event fired when you click the button
    public void onButton1Click(View view) {
        if (mBluetooth.isConnected()) {
            mBluetooth.SendMessage(string_1);
            Log.w("myApp", "[#] Sent: " + string_1);
        }
    }

    // The event fired when you click the button
    public void onButton2Click(View view) {
        if (mBluetooth.isConnected()) {
            mBluetooth.SendMessage(string_2);
            Log.w("myApp", "[#] Sent: " + string_2);
        }
    }
}