#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

bool DEBUG = true;

// 
// Hardware configuration
// 
int close_pin = A0;
int open_pin = A1;
int is_closed_pin = A2;  // unused
int is_open_pin = A3;  // unused

/* Door states:
 * 0 = error, probably communication blackout
 * 1 = close
 * 2 = open
 * 3 = noop (do nothing, all ok)
 */
unsigned long door_action = 3;

int reset_pin = 7;

int led = 8;  // onboard led 13 is used by RF24 for clock o_O
bool led_state = HIGH;
unsigned long led_time_ok = 1000;
unsigned long led_time_blackout = 50;
unsigned long led_timer = led_time_blackout;
unsigned long led_timer_start = millis();
unsigned long radio_led_blackout_treshold = 1000;

unsigned long wireless_interval = 200;
unsigned long last_wireless_millis = 0;
unsigned long radio_wd_time = 4000;

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9, 10);

// 
// Topology
// 
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xE8E8F0F0E1LL };

// 
// Role management
// 
typedef enum { role_ping_out = 1, role_pong_back } role_e;
role_e role = role_ping_out;


void setup()
{
    // reset hack
    digitalWrite(reset_pin, HIGH);
    pinMode(reset_pin, OUTPUT);

    pinMode(led, OUTPUT);
    digitalWrite(led, led_state);

    pinMode(close_pin, INPUT_PULLUP);
    digitalWrite(close_pin, HIGH);
    pinMode(open_pin, INPUT_PULLUP);
    digitalWrite(open_pin, HIGH);
    pinMode(is_closed_pin, INPUT_PULLUP);
    digitalWrite(is_closed_pin, HIGH);
    pinMode(is_open_pin, INPUT_PULLUP);
    digitalWrite(is_open_pin, HIGH);

    // 
    // Setup and configure rf radio
    // 
    radio.begin();

    // optionally, increase the delay between retries & # of retries
    radio.setRetries(15, 15);

    // optionally, reduce the payload size.  seems to
    // improve reliability
    radio.setPayloadSize(8);

    if ( role == role_ping_out )
    {
        radio.openWritingPipe(pipes[0]);
        radio.openReadingPipe(1, pipes[1]);
    }
    else
    {
        radio.openWritingPipe(pipes[1]);
        radio.openReadingPipe(1, pipes[0]);
    }

    if (DEBUG)
    {
        Serial.begin(115200);
        Serial.println("BEGIN");
    }
}

/*
 * Positive logic.
 * Also guard against disconnected C1 Arduino (all command pins will be HIGH due to internal pullup).
 */
bool readCommandPin(int pin)
{
    if (digitalRead(open_pin) && digitalRead(close_pin))
    {
        return 0;
    }
    return digitalRead(pin);
}

void loop()
{
    door_action_loop();
    radio_loop();
    led_loop();
}

void door_action_loop()
{
    if (readCommandPin(open_pin))
    {
        door_action = 1;
    }
    else if (readCommandPin(close_pin))
    {
        door_action = 2;
    }
    else
    {
        door_action = 3;
    }
}

void radio_loop()
{
    if (millis() - last_wireless_millis < wireless_interval) return;

    // First, stop listening so we can talk.
    radio.stopListening();

    if (DEBUG)
    {
        Serial.print("Now sending:");
        Serial.println(door_action);
    }

    bool ok = radio.write(&door_action, sizeof(unsigned long));
    if (DEBUG)
    {
        if (ok)
        {
            Serial.println("OK...");
        }
        else
        {
            Serial.println("Failed!");
        }
    }

    // Now, continue listening
    radio.startListening();

    if (!ok) return;  // if send failed, bail out

    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while (!radio.available() && !timeout)
    {
        if (millis() - started_waiting_at > 250)
        {
            timeout = true;
        }
    }

    if (timeout)
    {
        if (DEBUG) Serial.println("Failed, response timed out.\n\r");
    }
    else
    {
        // Grab the response and compare
        unsigned long got_response;
        radio.read(&got_response, sizeof(unsigned long));

        if (DEBUG)
        {
            Serial.print("Got response:");
            Serial.println(got_response);
        }

        if (got_response == door_action)
        {
            // wireless ok
            last_wireless_millis = millis();
        }
    }

    // Reset if communication blackout
    if (millis() - last_wireless_millis > radio_wd_time)
    {
        if (DEBUG) Serial.println("RESET");
        delay(100);
        digitalWrite(reset_pin, LOW);  // Pulling the RESET pin LOW triggers the reset.
    }
}

void led_loop()
{
    if ((last_wireless_millis == 0) || (millis() - last_wireless_millis > radio_led_blackout_treshold))
    {
        led_timer = led_time_blackout;
    }
    else
    {
        led_timer = led_time_ok;
    }
    if (millis() - led_timer_start > led_timer)
    {
        led_state = ! led_state;
        led_timer_start = millis();
        digitalWrite(led, led_state);
    }
}
