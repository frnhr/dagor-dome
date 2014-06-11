#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

boolean DEBUG = true;

//
// Hardware configuration
//
int close_pin = 5;
int open_pin = 4;
int close_pin2 = 3;
int open_pin2 = 2;
int is_closed_pin = 1; //unused
int is_open_pin = 0; //unused

int led = 8;
boolean led_state = 1;
unsigned long led_timer = 300;
unsigned long led_time_1 = 50000;
unsigned long led_time_2 = 1000;
unsigned long led_timer_12 = 200;
unsigned long led_time = led_time_1;
unsigned long last_radio_send_millis = 0;
unsigned long radio_wd_time = 4000;

unsigned long wireless_interval = 200;
unsigned long last_wireless_millis = 0;

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
RF24 radio(9,10);

//
// Topology
//
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xE8E8F0F0E1LL };

//
// Role management
//
typedef enum { role_ping_out = 1, role_pong_back } role_e;
role_e role = role_ping_out;


void setup() {
  //reset hack
  digitalWrite(7, HIGH);
  pinMode(7,OUTPUT);
    
  pinMode(led, OUTPUT);     
  digitalWrite(led, led_state);
  
  pinMode(close_pin, INPUT_PULLUP);
  pinMode(open_pin, INPUT_PULLUP);
  pinMode(close_pin, INPUT_PULLUP);
  pinMode(open_pin, INPUT_PULLUP);
  
  //
  // Setup and configure rf radio
  //
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(8);

  if ( role == role_ping_out ) {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1, pipes[1]);
  } else {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1, pipes[0]);
  }
  
  if(DEBUG) {
    Serial.begin(115200);
    Serial.println("BEGIN");
  }
}

void loop() {
  unsigned long door_action = 3;
  if(digitalRead(open_pin) == LOW || digitalRead(open_pin2) == LOW) {
    door_action = 1;
  } else if(digitalRead(close_pin) == LOW || digitalRead(close_pin2) == LOW) {
    door_action = 2;
  }
  
  //
  // Ping out role.  Repeatedly send the current time
  //
  if (millis() - last_wireless_millis > wireless_interval) {
    last_wireless_millis = millis();
    
    // First, stop listening so we can talk.
    radio.stopListening();
  
    // Take the time, and send it.  This will block until complete
    if(DEBUG) {
      Serial.print("Now sending:");
      Serial.println(door_action);
    }
    
    boolean ok = radio.write(&door_action, sizeof(unsigned long));
    if(DEBUG) {
      if (ok) {
        Serial.println("OK...");
      } else {
        Serial.println("Failed!");
      }
    }
  
    // Now, continue listening
    radio.startListening();
  
    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    boolean timeout = false;
    while(!radio.available() && !timeout) {
      if(millis() - started_waiting_at > 250) {
        timeout = true;
      }
    }
  
    boolean ping_ok = false;
    if (timeout && DEBUG) {
      printf("Failed, response timed out.\n\r");
    } else {
      // Grab the response and compare
      unsigned long got_response;
      radio.read(&got_response, sizeof(unsigned long));

      if(DEBUG) {
        Serial.print("Got response:");
        Serial.println(got_response);
      }
        
      if(got_response == door_action) ping_ok = true;
    }
  
    // signal error and reset door_action
    if (ping_ok) {
      last_radio_send_millis = millis();
      digitalWrite(led, LOW);
      door_action = 3;
    } else {
      digitalWrite(led, HIGH);
    }
    
    if (millis() - last_radio_send_millis > radio_wd_time) {
      // reset!
      if(DEBUG) Serial.println("RESET!");
      delay(100);
      digitalWrite(7, LOW); //Pulling the RESET pin LOW triggers the reset. 
    }
  }
}
