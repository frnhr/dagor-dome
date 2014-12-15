#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

bool DEBUG = true;

// 
// Hardware configuration
// 
int rele_open = 5;
int rele_close = 6;
int reset_pin = 7;

/* Door states:
 * 0 = error, probably communication blackout
 * 1 = close
 * 2 = open
 * 3 = noop (do nothing, all ok)
 */
unsigned long current_door_state = 3;
unsigned long last_door_state = 3;
unsigned long commanded_door_state = 3;

unsigned long min_state_timeout = 0;
unsigned long min_state_start_time = 600;
unsigned long min_state_stop_time = 600;
unsigned long noop_keep_state_timeout = 0;
unsigned long noop_keep_state_time = 15000;

unsigned long last_radio_recieve_millis = 0;
unsigned long radio_wd_time = 4000;

int led = 8;  // onboard led 13 is used by RF24 for clock o_O
bool led_state = HIGH;
unsigned long led_time_ok = 1000;
unsigned long led_time_blackout = 50;
unsigned long led_timer = led_time_blackout;
unsigned long led_timer_start = millis();
unsigned long radio_led_blackout_treshold = 1000;

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9, 10);

// 
// Topology
// 
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xE8E8F0F0E1LL };


void setup(void)
{
    // reset hack
    digitalWrite(reset_pin, HIGH);
    pinMode(reset_pin, OUTPUT);

    // releys
    pinMode(rele_open, OUTPUT);
    pinMode(rele_close, OUTPUT);
    digitalWrite(rele_open, LOW);
    digitalWrite(rele_close, LOW);

    // status LED
    pinMode(led, OUTPUT);


    // Setup and configure rf radio
    radio.begin();
    radio.setRetries(15, 15);  // optionally, increase the delay between retries & # of retries
    radio.setPayloadSize(8);  // optionally, reduce the payload size. seems to improve reliability
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1, pipes[0]);

    if (DEBUG)
    {
        Serial.begin(115200);
        Serial.println("BEGIN");
    }
    radio.startListening();
}


void loop(void)
{
    radio_loop();
    door_loop();
    led_loop();
}


void door_loop()
{

    if (min_state_timeout > millis())
    {
        Serial.println("min_state_timeout in effect");
        return;

    }
    else
    {

        unsigned long next_door_state = current_door_state;

        // the logic if stopped
        if (current_door_state == 3)                                       // currently STOPPED
        {
            if (commanded_door_state != 3)                                       // and commanded to MOVE
            {
                next_door_state = commanded_door_state;
                min_state_timeout = millis() +  min_state_start_time;
            }
        }

        // the logic if moving
        if (current_door_state != 3)                                       // currently MOVING
        {
            if (commanded_door_state == 3)                                       // and commanded to STOP
            {
                next_door_state = 3;
                min_state_timeout = millis() + min_state_stop_time;
            }
            if (commanded_door_state != current_door_state)                      // and commanded to CHANGE DIRECTION
            {
                next_door_state = 3;
                min_state_timeout = millis() + min_state_stop_time;
            }
        }

        current_door_state = next_door_state;
    }


    // @TODO look at is_open and is_closed and oply fire relays if needed
    if (current_door_state == 1)
    {
        digitalWrite(rele_open, LOW);
        digitalWrite(rele_close, HIGH);
    }
    else if (current_door_state == 2)
    {
        digitalWrite(rele_close, LOW);
        digitalWrite(rele_open, HIGH);
    }
    else if (current_door_state == 3)
    {
        digitalWrite(rele_close, LOW);
        digitalWrite(rele_open, LOW);
    }
}


/*
 * Handles radio communication with C2,
 * i.e. sets "commanded_door_state" value.
 * Resets itself in case of radio blackout.
 */
void radio_loop()
{
    // Receive each packet, dump it out, and send it back
    if (radio.available())
    {
        // Dump the payloads until we've gotten everything
        unsigned long door_action;
        bool done = false;

        while (!done)
        {
            // Fetch the payload, and see if this was the last one.
            done = radio.read(&door_action, sizeof(unsigned long));

            if (DEBUG)
            {
                Serial.print("Got payload:");
                Serial.println(door_action);
            }

            // Delay just a little bit to let the other unit
            // make the transition to receiver
            delay(20);
        }

        // First, stop listening so we can talk
        radio.stopListening();

        // Send the final one back.
        radio.write(&door_action, sizeof(unsigned long));

        // Now, resume listening so we catch the next packets.
        radio.startListening();

        // Set global variables
        commanded_door_state = door_action;
        last_radio_recieve_millis = millis();
    }

    // Reset if communication blackout
    if (millis() - last_radio_recieve_millis > radio_wd_time)
    {
        if (DEBUG) Serial.println("RESET");
        delay(100);
        digitalWrite(reset_pin, LOW);  // Pulling the RESET pin LOW triggers the reset.
    }

}

void led_loop()
{
    if ((last_radio_recieve_millis == 0) || (millis() - last_radio_recieve_millis > radio_led_blackout_treshold))
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
    delay(100);
}
