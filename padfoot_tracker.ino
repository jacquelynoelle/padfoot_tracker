#include <bluefruit.h>

/* GATT SERVICE AND CHARACTERISTICS:
 * Running Speed and Cadence Service:  0x1814 or 00001814-0000-1000-8000-00805f9b34fb
 * Step Count Characteristic (Custom): 0x2A56 or 00002a56-0000-1000-8000-00805f9b34fb
 */
BLEService        rscs = BLEService(UUID16_SVC_RUNNING_SPEED_AND_CADENCE);
BLECharacteristic stepc = BLECharacteristic(0x2A56);

BLEDis bledis;    // DIS (Device Information Service) helper class instance
BLEBas blebas;    // BAS (Battery Service) helper class instance

/* PIN CONSTANTS
 * Analog pins can be treated as digital pins.
 */
const byte groundpin = A5;   // analog input pin 5 -- ground 
const byte powerpin = A4;    // analog input pin 4 -- voltage
const byte xpin = A3;        // x-axis
const byte ypin = A2;        // y-axis
const byte zpin = A1;        // z-axis

// data to send over ble:
uint8_t  bps = 0;
uint16_t steps = 0; // for storing step count

// Advanced function prototypes
void startAdv(void);
void setupRSC(void);
void connect_callback(uint16_t conn_handle);
void disconnect_callback(uint16_t conn_handle, uint8_t reason);

void setup()
{
  // Set up ground pin and power pin from A5 and A4.
  pinMode(groundpin, OUTPUT);
  pinMode(powerpin, OUTPUT);
  digitalWrite(groundpin, LOW);
  digitalWrite(powerpin, HIGH);
  
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Padfoot Testing");
  Serial.println("-----------------------\n");

  // Initialize the Bluefruit module
  Bluefruit.begin();

  // Set device name to Padfoot
  Bluefruit.setName("Padfoot");

  // Set the connect/disconnect callback handlers
  Bluefruit.setConnectCallback(connect_callback);
  Bluefruit.setDisconnectCallback(disconnect_callback);

  // Configure and Start the Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Start the BLE Battery Service and set it to 100%
  blebas.begin();
  blebas.write(100);

  // Setup the Heart Rate Monitor service using
  // BLEService and BLECharacteristic classes
  Serial.println("Configuring the Running Speed and Cadence Service");
  setupRSC();

  // Setup the advertising packet(s)
  startAdv();

  Serial.println("Advertising...\n");
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include RSC Service UUID and Device Name
  Bluefruit.Advertising.addService(rscs);
  Bluefruit.Advertising.addName();
  
  // Setup advertising rules and timeouts
  Bluefruit.Advertising.restartOnDisconnect(true);  // enables auto advertising if disconnected
  Bluefruit.Advertising.setInterval(32, 244);       // in unit of 0.625 ms, Interval:  fast mode = 20 ms, slow mode = 152.5 ms
  Bluefruit.Advertising.setFastTimeout(30);         // number of seconds to timeout in fast mode
  Bluefruit.Advertising.start(0);                   // Start(timeout) with timeout = 0 will advertise forever (until connected)
}

void setupRSC(void) {
  // Begin the BLEService (must be done before beginning characteristics)
  rscs.begin();

  // Configure the Running Speed and Cadence Step Count custom characteristic
  //    Properties  = Notify
  //    Len         = 2 (will need to up this if includes timestamp)
  //    B0:1        = UINT16 - 16-bit heart rate measurement value in BPM
  stepc.setProperties(CHR_PROPS_NOTIFY);
  stepc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  stepc.setFixedLen(2);
  stepc.setCccdWriteCallback(cccd_callback);  // Optionally capture CCCD updates
  stepc.begin();
}

/**
 * Callback invoked when a connection is made
 * @param conn_handle connection where this event happens
 */
void connect_callback(uint16_t conn_handle)
{
  char central_name[32] = { 0 };
  Bluefruit.Gap.getPeerName(conn_handle, central_name, sizeof(central_name));

  Serial.print("Connected to: ");
  Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println("Disconnected!");
  Serial.println("Advertising...");
}

void cccd_callback(BLECharacteristic& chr, uint16_t cccd_value)
{
    // Display the raw request packet
    Serial.print("CCCD Updated: ");
    //Serial.printBuffer(request->data, request->len);
    Serial.print(cccd_value);
    Serial.println("");

    // Check the characteristic this CCCD update is associated with in case
    // this handler is used for multiple CCCD records.
    if (chr.uuid == stepc.uuid) {
        if (chr.notifyEnabled()) {
            Serial.println("Running Speed and Cadence Step Count 'Notify' enabled");
        } else {
            Serial.println("Running Speed and Cadence Step Count 'Notify' disabled");
        }
    }
}

void loop()
{
  digitalToggle(LED_RED);

  // STEP COUNTING ALGORITHM
  // Increment step count whenever accelerometer exceeds threshold
  if ( analogRead(xpin) > 500 && analogRead(ypin) > 500 && analogRead(zpin) > 500 ) {
    steps++;
    Serial.print("Step count updated to: "); 
    Serial.println(steps); 
  }

  // Send step count to to app via BLE notify
  // TODO: Probably only need this to notify once every 5-10 seconds even if we want to check for steps more frequently on the device
  if ( Bluefruit.connected() ) {  // sensor connected
    // Note: We use .notify instead of .write!
    // If it is connected but CCCD is not enabled
    // The characteristic's value is still updated although notification is not sent
    if ( stepc.notify16(steps) ) {
      Serial.println("SUCCESS: Step count sent to app"); 
    } else {
      Serial.println("ERROR: Notify not set in the CCCD or not connected!");
    }
  }

  // Update once per 1s
  delay(1000);
}
