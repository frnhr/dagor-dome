void calibration_start()
{
    //reset variables in case that calibration is running after another calibration
    //be sure to run from home position in that case
    calibration.in_progress = true;
    calibration.cycles = 0L;
    volatile MehanicalCombination screws1 = {false, false, false, 0, 0};
    volatile MehanicalCombination screws2 = {false, false, false, 0, 0};
    volatile MehanicalCombination screws3 = {false, false, false, 0, 0};
    volatile MehanicalCombination screws4 = {false, false, false, 0, 0};
    volatile MehanicalCombination current_screws = {false, false, false, 0, 0};

    dome.route = 1; //rotate down

    if (DEBUG)
    {
        Serial.println("Start of calibration");//DEBUG
    }
}

void detect_screws(double diff)
{
    if (DEBUG)
    {
        Serial.print("Detected:\t");
        Serial.print(current_screws.one);
        Serial.print("\t");
        Serial.print(current_screws.two);
        Serial.print("\t");
        Serial.print(current_screws.three);
        Serial.print("\n");
    }

    if (screws1.one == false && screws1.two == false && screws1.three == false)
    {
        screws1.cycles = calibration.cycles - diff;
        screws1.one = current_screws.one;
        screws1.two = current_screws.two;
        screws1.three = current_screws.three;
    }
    else if (screws2.one == false && screws2.two == false && screws2.three == false)
    {
        screws2.cycles = calibration.cycles - diff;
        screws2.one = current_screws.one;
        screws2.two = current_screws.two;
        screws2.three = current_screws.three;
    }
    else if (screws3.one == false && screws3.two == false && screws3.three == false)
    {
        screws3.cycles = calibration.cycles - diff;
        screws3.one = current_screws.one;
        screws3.two = current_screws.two;
        screws3.three = current_screws.three;
    }
    else if (screws4.one == false && screws4.two == false && screws4.three == false)
    {
        screws4.cycles = calibration.cycles - diff;
        screws4.one = current_screws.one;
        screws4.two = current_screws.two;
        screws4.three = current_screws.three;
    }
    else
    {
        dome.route = 0;  //stop motors

        dome.cycles_for_degree = (abs(calibration.cycles - screws1.cycles) - diff) / 360;

        //azimuth of each screw
        screws1.azimuth = dome.home_azimuth + (screws1.cycles / dome.cycles_for_degree);
        if (screws1.azimuth >= 360)
        {
            screws1.azimuth -= 360;
        }
        else if (screws1.azimuth < 0)
        {
            screws1.azimuth += 360;
        }

        screws2.azimuth = dome.home_azimuth + (screws2.cycles / dome.cycles_for_degree);
        if (screws2.azimuth >= 360)
        {
            screws2.azimuth -= 360;
        }
        else if (screws2.azimuth < 0)
        {
            screws2.azimuth += 360;
        }

        screws3.azimuth = dome.home_azimuth + (screws3.cycles / dome.cycles_for_degree);
        if (screws3.azimuth >= 360)
        {
            screws3.azimuth -= 360;
        }
        else if (screws3.azimuth < 0)
        {
            screws3.azimuth += 360;
        }

        screws4.azimuth = dome.home_azimuth + (screws4.cycles / dome.cycles_for_degree);
        if (screws4.azimuth >= 360)
        {
            screws4.azimuth -= 360;
        }
        else if (screws4.azimuth < 0)
        {
            screws4.azimuth += 360;
        }

        if (DEBUG)
        {
            Serial.print("azimuth1: ");
            Serial.println(screws1.azimuth);
            Serial.print("azimuth2: ");
            Serial.println(screws2.azimuth);
            Serial.print("azimuth3: ");
            Serial.println(screws3.azimuth);
            Serial.print("azimuth4: ");
            Serial.println(screws4.azimuth);
        }

        dome.azimuth = screws1.azimuth + (diff / dome.cycles_for_degree);

        calibration.drift_start_azimuth = dome.azimuth;
        calibration.drift_start_time = millis();
        calibration.in_progress = false;
        calibration.measuring_drift = true;
    }
}
