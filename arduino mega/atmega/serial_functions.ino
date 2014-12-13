void serial_loop()
{
    while (Serial.available() > 0)
    {
        char incomingByte = (char)Serial.read();

        if (incomingByte == '\n')
        {
            serial_comm.reading_complete = true;
        }
        else
        {
            serial_comm.data_received += incomingByte;
        }
    }

    if (serial_comm.reading_complete)
    {
        if (serial_comm.data_received == "status")
        {
            Serial.println("ok 3");
            Serial.println(dome.azimuth);
            Serial.println(dome.route);
            Serial.println(calibration.in_progress ? 0 : (dome.cycles_for_degree == 0 ? -1 : 1));
        }
        else if (serial_comm.data_received == "dg")   //get azimuth
        {
            if (dome.cycles_for_degree == 0)
            {
                Serial.println("error 1");
                Serial.println("ec");
            }
            else
            {
                Serial.println("ok 1");
                Serial.println(dome.azimuth);
            }
        }
        else if (serial_comm.data_received.substring(0, 2) == "ds")   //go to defined azimuth
        {
            if (dome.cycles_for_degree == 0)
            {
                Serial.println("error 1");
                Serial.println("ec");
            }
            else
            {
                Serial.println("ok 1");
                String data = serial_comm.data_received.substring(2);
                char buf[data.length() + 1];
                data.toCharArray(buf, data.length() + 1);
                double value = atof(buf);
                Serial.println(value);
                motor_goto(value);
            }
        }
        else if (serial_comm.data_received == "dp")   //park dome
        {
            if (dome.cycles_for_degree == 0)
            {
                Serial.println("error 1");
                Serial.println("ec");
            }
            else
            {
                Serial.println("ok 0");
                motor_goto(dome.home_azimuth);
            }
        }
        else if (serial_comm.data_received == "do")   //open door
        {
            Serial.println("ok 0");
            door_open();
        }
        else if (serial_comm.data_received == "dc")   //close door
        {
            Serial.println("ok 0");
            door_close();
        }
        else if (serial_comm.data_received == "hg")   //get home
        {
            Serial.println("ok 1");
            Serial.println(dome.home_azimuth);
        }
        else if (serial_comm.data_received == "hs")  //set home at current position
        {
            if (dome.cycles_for_degree == 0)
            {
                Serial.println("error 1");
                Serial.println("ec");
            }
            else
            {
                Serial.println("ok 0");
                EEPROM_write_home(dome.azimuth);
            }
        }
        else if (serial_comm.data_received == "cs")    //calibration start
        {
            Serial.println("ok 0");
            calibration_start();
        }
        else if (serial_comm.data_received == "stop")    //stop rotation manually
        {
            Serial.println("ok 0");
            dome.route = 0;
        }
        else if (serial_comm.data_received == "up")    //move up manually
        {
            Serial.println("ok 0");
            dome.route = -1;
        }
        else if (serial_comm.data_received == "down")    //move down manually
        {
            Serial.println("ok 0");
            dome.route = 1;
        }
        else if (serial_comm.data_received.substring(0, 5) == "force")    //force current azimuth to be replaced with new one (useful for correcting home)
        {
            Serial.println("ok 0");
            String data = serial_comm.data_received.substring(5);
            char buf[data.length() + 1];
            data.toCharArray(buf, data.length() + 1);
            double value = atof(buf);
            dome.azimuth = value;
        }
        else {
            Serial.println("error 1");
            Serial.print("unknown command: ");
            Serial.println(serial_comm.data_received);
        }

        serial_comm.reading_complete = false;
        serial_comm.data_received = "";
    }
}
