#define DATA_PIN   10
#define CLOCK_PIN  9
#define PLOAD_PIN  8
#define INT_PIN    2

#define BYTES_VAL_T unsigned int

#define PULSE_WIDTH_USEC   5
#define POLL_DELAY_MSEC    1
#define DATA_WIDTH         8

BYTES_VAL_T pinValues;
BYTES_VAL_T oldPinValues;

void setup() {
  Serial.begin(9600);

  /* Initialize our digital pins...
  */
  pinMode(PLOAD_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, INPUT);

  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(PLOAD_PIN, HIGH);

  /* Read in and display the pin states at startup.
  */
  pinValues = read_shift_regs();
  display_pin_values();
  oldPinValues = pinValues;
}

BYTES_VAL_T read_shift_regs() {
  long bitVal;
  BYTES_VAL_T bytesVal = 0;

  /* Trigger a parallel Load to latch the state of the data lines,
  */
  digitalWrite(PLOAD_PIN, LOW);
  delayMicroseconds(PULSE_WIDTH_USEC);
  digitalWrite(PLOAD_PIN, HIGH);

  /* Loop to read each bit value from the serial out line
     of the SN74HC165N.
  */
  for (int i = 0; i < DATA_WIDTH; i++)
  {
    bitVal = digitalRead(DATA_PIN);

    /* Set the corresponding bit in bytesVal.
    */
    bytesVal |= (bitVal << ((DATA_WIDTH - 1) - i));

    /* Pulse the Clock (rising edge shifts the next bit).
    */
    digitalWrite(CLOCK_PIN, HIGH);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(CLOCK_PIN, LOW);
  }

  return (bytesVal);
}

void display_pin_values() {
  Serial.print("Pin States:\r\n");

  for (int i = 0; i < DATA_WIDTH; i++) {
    Serial.print("  Pin-");
    Serial.print(i);
    Serial.print(": ");

    if ((pinValues >> i) & 1) {
      Serial.print("HIGH");
    } else {
      Serial.print("LOW");
    }
    Serial.print("\r\n");
  }
  Serial.print("\r\n");
}

void loop() {
  /* Read the state of all zones.
  */
  pinValues = read_shift_regs();

  /* If there was a chage in state, display which ones changed.
  */
  if (pinValues != oldPinValues) {
    Serial.print("*Pin value change detected*\r\n");
    display_pin_values();
    oldPinValues = pinValues;
  }

  delay(POLL_DELAY_MSEC);
}
