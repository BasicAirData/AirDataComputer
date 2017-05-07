package eu.basicairdata.graziano.adctester;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.os.Handler;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Html;
import android.text.InputType;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Set;
import java.util.Timer;

public class ADCTestActivity extends AppCompatActivity {

    private static final int REQUEST_ENABLE_BT = 12;

    private ArrayList<String> BTDevices = new ArrayList<>();
    private ArrayList<String> MessagesChoice = new ArrayList<>();

    BluetoothHelper mBluetooth = new BluetoothHelper();
    BluetoothAdapter mBluetoothAdapter = null;

    private ArrayAdapter mArrayAdapterDevices;
    private ArrayAdapter mArrayAdapterMessages;
    private Spinner mSpinnerDevices;
    private Spinner mSpinnerMessages;
    private TextView mTextViewStatus;
    private TextView mTextViewChat;
    private ScrollView mScrollViewChat;

    private int PreviousMessageSelection = 0;                   // The last message selected

    // It receive the system notification about Bluetooth state changes
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
        final String action = intent.getAction();

        if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
            final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,
                    BluetoothAdapter.ERROR);
            switch (state) {
                case BluetoothAdapter.STATE_OFF:
                    mTextViewStatus.setText("Bluetooth disabled");
                    break;
                case BluetoothAdapter.STATE_TURNING_OFF:
                    if (mBluetooth.isConnected()) mBluetooth.Disconnect();
                    PopulateSpinnerAndLists();
                    break;
                case BluetoothAdapter.STATE_ON:
                    PopulateSpinnerAndLists();
                    mTextViewStatus.setText("Disconnected");
                    break;
                case BluetoothAdapter.STATE_TURNING_ON:
                    break;
            }
        }
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);   // Keeps the screen on
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);      // Force orientation of screen
        //setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        setContentView(R.layout.adctestactivity);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();               // Check for BT adapter
        if (mBluetoothAdapter == null) {
            // Device does not support Bluetooth
            finish(); // Closes if a BT adapter is not found
        }

        // Register the Broadcast Receiver for Bluetooth state changes
        IntentFilter filter1 = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(mReceiver, filter1);

        mTextViewStatus = (TextView)findViewById(R.id.id_textviewstatus);
        mTextViewChat = (TextView)findViewById(R.id.id_textviewchat);
        mScrollViewChat = (ScrollView)findViewById(R.id.id_scrollviewchat);

        MessagesChoice.add("$HBQ,ADCTESTER,1");                                 // Set default messages
        MessagesChoice.add("$TMS,<Time>");      // <- Please leave it in second position
        MessagesChoice.add("$TMQ");
     /* MessagesChoice.add("$STS,1,1,1,1,1,1,1,N,1");
        MessagesChoice.add("$STQ");
        MessagesChoice.add("$$DTS,1,1,1,1,1,1,0");
        MessagesChoice.add("$DTQ,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1");
        MessagesChoice.add("$SFS,2");
        MessagesChoice.add("$SFQ");
        MessagesChoice.add("$DFS,5");
        MessagesChoice.add("$DFQ"); */
        MessagesChoice.add("Add message ...");  // <- Please leave it in the last position

        mSpinnerMessages = (Spinner)findViewById(R.id.id_spinnermsg);
        mSpinnerMessages.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int pos, long id) {
                if (pos == adapterView.getCount()-1) {                          // "Add message ..." is selected
                    mSpinnerMessages.post(new Runnable() {
                        @Override
                        public void run() {
                            mSpinnerMessages.setSelection(PreviousMessageSelection);
                        }
                    });

                    Log.w("myApp", "[#] ADCTestActivity: Add message");

                    final AlertDialog.Builder builder = new AlertDialog.Builder(ADCTestActivity.this);
                    builder.setTitle("Add Message");

                    // Set up the input
                    final EditText input = new EditText(ADCTestActivity.this);
                    input.setInputType(InputType.TYPE_CLASS_TEXT);
                    builder.setView(input);

                    // Show soft keyboard
                    input.postDelayed(new Runnable()
                    {
                        public void run()
                        {
                            input.requestFocus();
                            InputMethodManager mgr = (InputMethodManager) ADCTestActivity.this.getSystemService(Context.INPUT_METHOD_SERVICE);
                            mgr.showSoftInput(input, InputMethodManager.SHOW_IMPLICIT);
                        }
                    }, 200);

                    // Set up the buttons
                    builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            mSpinnerMessages.post(new Runnable() {
                                @Override
                                public void run() {
                                    MessagesChoice.add(MessagesChoice.size()-1, input.getText().toString().toUpperCase().trim());
                                    mArrayAdapterMessages.notifyDataSetChanged();
                                    mSpinnerMessages.setSelection(MessagesChoice.size()-2);
                                    PreviousMessageSelection = MessagesChoice.size()-2;
                                }
                            });
                        }
                    });
                    builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            mSpinnerMessages.post(new Runnable() {
                                @Override
                                public void run() {
                                    mSpinnerMessages.setSelection(PreviousMessageSelection);
                                }
                            });
                            dialog.cancel();
                        }
                    });

                    builder.show();
                } else PreviousMessageSelection = pos;
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });

        mSpinnerDevices = (Spinner)findViewById(R.id.id_spinnerdevices);
        mSpinnerDevices.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int pos, long id) {
                if (pos != 0) {
                    mTextViewStatus.setText("Connecting to " + adapterView.getItemAtPosition(pos).toString() + " ...");
                    mBluetooth.Connect(adapterView.getItemAtPosition(pos).toString());
                } else {
                    mBluetooth.Disconnect();
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        mArrayAdapterMessages = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, MessagesChoice);
        mArrayAdapterMessages.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mSpinnerMessages.setAdapter(mArrayAdapterMessages);

        mArrayAdapterDevices = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, BTDevices);
        mArrayAdapterDevices.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mSpinnerDevices.setAdapter(mArrayAdapterDevices);

        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter.isEnabled()) {
            PopulateSpinnerAndLists();
            mTextViewStatus.setText("Disconnected");
        }

        // Setup listener for Bluetooth helper;
        mBluetooth.setBluetoothHelperListener(new BluetoothHelper.BluetoothHelperListener() {
            @Override
            public void onBluetoothHelperMessageReceived(BluetoothHelper bluetoothhelper, final String message) {
                // Do something with the message received
                new Handler().post(new Runnable() {
                    @Override
                    public void run() {
                        String text = "<font color='gray'>" + message + "</font><br>";
                        mTextViewChat.append(Html.fromHtml(text));
                        mScrollViewChat.fullScroll(View.FOCUS_DOWN);
                    }
                });
            }

            @Override
            public void onBluetoothHelperConnectionStateChanged(BluetoothHelper bluetoothhelper, boolean isConnected) {
                if (isConnected) {
                    mTextViewStatus.setText("Connected");
                } else {
                    mTextViewStatus.setText("Disconnected");
                    mSpinnerDevices.setSelection(0);
                    // Auto reconnect
                    //if (mSpinnerDevices.getSelectedItem().toString() != "Disconnected") mBluetooth.Connect(mSpinnerDevices.getSelectedItem().toString());
                }
            }
        });

        //String text = "<font color='gray'>--- START</font><br>";
        //mTextViewChat.append(Html.fromHtml(text));
    }


    @Override
    protected void onDestroy(){
        super.onDestroy();
        mSpinnerDevices.setOnItemSelectedListener(null);
        unregisterReceiver(mReceiver);                          // Unregister broadcast listeners
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        PopulateSpinnerAndLists();
    }


    public void sendMessage(View view) {
        String text = "<font color='red'>" + mSpinnerMessages.getSelectedItem() + "</font><br>";
        String textmessage = mSpinnerMessages.getSelectedItem().toString();

        Log.w("myApp", "[#] ADCTestActivity: Send message: " + mSpinnerMessages.getSelectedItem());
        if (mSpinnerMessages.getSelectedItemId() == 1) {                        // TMS - TIME_SET
            text = "<font color='red'>$TMS," + System.currentTimeMillis() / 1000L + "</font><br>";      // Requested unix time
            textmessage = "$TMS," + System.currentTimeMillis() / 1000L;
        }
        mTextViewChat.append(Html.fromHtml(text));
        mScrollViewChat.fullScroll(View.FOCUS_DOWN);

        mBluetooth.SendMessage(textmessage);
    }



    // ---------------------------------------------------------------------------- SERVICE METHODS

    void PopulateSpinnerAndLists ()
    {
        mArrayAdapterDevices.clear();           // Clear the devices list
        mArrayAdapterDevices.add("Disconnected");
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        Set<BluetoothDevice> devices = mBluetoothAdapter.getBondedDevices();
        for(BluetoothDevice device : devices) mArrayAdapterDevices.add(device.getName());
    }
}
