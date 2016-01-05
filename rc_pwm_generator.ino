
#include <Servo.h>

#define CHANNEL_MAX  2000
#define CHANNEL_MIN 1000
#define CHANNEL_NUMBER 3
#define CHANNEL_MID 1500

#define INPUT_FREQUENCY 5

#define DO_SERIAL false

const byte channel_pin[] = {A1, A0, A2};
int channel_input[] = {0, 0, 0};
int channel_output[] = {1500, 1500, 1500, 1300};
int channel_center[] = {0, 0, 0};
int channel_min[] = {-400, -400, -400};
int channel_max[] = {400, 400, 400};

const byte analog_channels[] = {0, 1};
const byte digital_channels[] = {2};

const byte pmw_pin[] = {3, 5, 6};

Servo servos[CHANNEL_NUMBER];

void setup() {

  if (DO_SERIAL) {
    Serial.begin(56700);
  }
 
  for (int i = 0; i < sizeof(digital_channels); i++) {
    pinMode(channel_pin[2], INPUT_PULLUP);
  }

  for (int i = 0; i < sizeof(analog_channels); i++) {
    channel_center[i] = analogRead(channel_pin[i]);
  }

  for (int i = 0; i < CHANNEL_NUMBER; i++) {
    servos[i].attach(pmw_pin[i], 1000, 2000);
    servos[i].writeMicroseconds(CHANNEL_MID);
  }

}

void process_analog_channel(int channel) {
  channel_input[channel] = analogRead(channel_pin[channel]);

  int diff = channel_center[channel] - channel_input[channel];

  if (diff > channel_max[channel]) {
    channel_max[channel] = diff;
  }

  if (diff < channel_min[channel]) {
    channel_min[channel] = diff;
  }

  if (diff > 0) {
    channel_output[channel] = map(diff, 0, channel_max[channel], CHANNEL_MID, CHANNEL_MAX);
  } else {
    channel_output[channel] = map(diff, channel_min[channel], 0, CHANNEL_MIN, CHANNEL_MID);
  }
  
}

void loop() {
  
  channel_input[2] = digitalRead(channel_pin[2]);
  channel_output[2] = map(channel_input[2], 0, 1, CHANNEL_MAX, CHANNEL_MIN);

  for (int i = 0; i < sizeof(analog_channels); i++) {
    process_analog_channel(i);
  }

  /*
   * Write channels to PWM pins
   */
  for (int i = 0; i < CHANNEL_NUMBER; i++) {
    servos[i].writeMicroseconds(channel_output[i]);
  }

  if (DO_SERIAL) {
    for (int i = 0; i < CHANNEL_NUMBER; i++) {
      if (i > 0) {
        Serial.print(" | ");
      }
      Serial.print(channel_output[i]); 
    }
    Serial.println();
  }

  delay(1000 / INPUT_FREQUENCY);
}
