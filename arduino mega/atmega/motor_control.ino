double cycles_at_last_hit = 0;
long current_cycles = 0;
long soft_start_down = 0, soft_start_up = 0; //0 while unpressed, millis() after controller is pressed
long soft_end = 0;

void motor_loop()
{
    if (calibration.in_progress)
    {
        current_cycles = calibration.cycles;
    }
    else
    {
        current_cycles = encoder.cycle;
    }

    if (analogRead(switch_one) == 0)
    {
        update_switch1();
    }
    if (analogRead(switch_two) == 0)
    {
        update_switch2();
    }
    if (analogRead(switch_three) == 0)
    {
        update_switch3();
    }


    if ((current_screws.one || current_screws.two ||  current_screws.three) && abs(current_cycles - cycles_at_last_hit) > 128) //if we hit screws, correct azimuth
    {
        finish_screws();
    }
}



void update_switch1()
{
    current_screws.one = true;
    cycles_at_last_hit = current_cycles;
}

void update_switch2()
{
    current_screws.two = true;
    cycles_at_last_hit = current_cycles;
}

void update_switch3()
{
    current_screws.three = true;
    cycles_at_last_hit = current_cycles;
}

void finish_screws()
{
    double diff = current_cycles - cycles_at_last_hit;

    if (calibration.in_progress)
    {
        detect_screws(diff);
    }
    else
    {
        if (DEBUG)
        {
            Serial.print("Before correction: ");
            Serial.println(dome.azimuth);
        }

        //detect which group of screws is hit and correct azimuth to it's azimuth:
        if (current_screws.one == screws1.one && current_screws.two == screws1.two && current_screws.three == screws1.three)
        {
            if (DEBUG)
            {
                Serial.println("Detected screws1");
            }
            dome.azimuth = screws1.azimuth + diff / dome.cycles_for_degree;
        }
        else if (current_screws.one == screws2.one && current_screws.two == screws2.two && current_screws.three == screws2.three)
        {
            if (DEBUG)
            {
                Serial.println("Detected screws2");
            }
            dome.azimuth = screws2.azimuth + diff / dome.cycles_for_degree;
        }
        else if (current_screws.one == screws3.one && current_screws.two == screws3.two && current_screws.three == screws3.three)
        {
            if (DEBUG)
            {
                Serial.println("Detected screws3");
            }
            dome.azimuth = screws3.azimuth + diff / dome.cycles_for_degree;
        }
        else if (current_screws.one == screws4.one && current_screws.two == screws4.two && current_screws.three == screws4.three)
        {
            if (DEBUG)
            {
                Serial.println("Detected screws4");
            }
            dome.azimuth = screws4.azimuth + diff / dome.cycles_for_degree;
        }
        else
        {
            if (DEBUG)
            {
                Serial.print("Error, no screws detected on azimuth: ");
                Serial.println(dome.azimuth);
            }
        }

        if (DEBUG)
        {
            Serial.print("After correction: ");
            Serial.println(dome.azimuth);
        }
    }

    current_screws.one = false;
    current_screws.two = false;
    current_screws.three = false;
}
