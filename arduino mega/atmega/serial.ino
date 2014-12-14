

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
        //status buffer:
        if (serial_comm.data_received == "status")
        {
            Serial.println("ok 3");
            Serial.println(status_buffer.current_azimuth);
            Serial.println(status_buffer.rotation);
            Serial.println(status_buffer.calibration);
        }
        else if (serial_comm.data_received == "hg")   //get home
        {
            Serial.println("ok 1");
            Serial.println(settings.home_azimuth);
        }
        else if (serial_comm.data_received == "dg" && calibration_check())   //get azimuth
        {
            Serial.println("ok 1");
            Serial.println(status_buffer.current_azimuth);
        }
        
        //input buffer:
        else if (serial_comm.data_received.substring(0, 2) == "ds" && calibration_check())   //go to defined azimuth
        {
            Serial.println("ok 0");
            String data = serial_comm.data_received.substring(2);
            char buf[data.length() + 1];
            data.toCharArray(buf, data.length() + 1);
            double value = atof(buf);
            
            input_buffer.target_azimuth = value;
        }
        else if (serial_comm.data_received == "dp" && calibration_check())   //park dome
        {
            Serial.println("ok 0");
            input_buffer.target_azimuth = settings.home_azimuth;
        }
        else if (serial_comm.data_received == "do")   //open door
        {
            Serial.println("ok 0");
            input_buffer.doors = OPEN;
        }
          else if (serial_comm.data_received == "dc")   //close door
        {
            Serial.println("ok 0");
            input_buffer.doors = CLOSE;
        }
        else if (serial_comm.data_received == "dt")   //stop door
        {
            Serial.println("ok 0");
            input_buffer.doors = DOORS_STOP;
        }
        else if (serial_comm.data_received == "hs" && calibration_check())  //set home at current position
        {
            Serial.println("ok 0");
            EEPROM_write_home(status_buffer.current_azimuth);
        }
        else if (serial_comm.data_received == "cs")    //calibration start
        {
            Serial.println("ok 0");
            input_buffer.start_calibration = true;
        }
        else if (serial_comm.data_received == "stop")    //stop rotation manually
        {
            Serial.println("ok 0");
            input_buffer.stop = true;
        }
        else if (serial_comm.data_received == "up")    //move up manually
        {
            Serial.println("ok 0");
            input_buffer.direction = UP;
        }
        else if (serial_comm.data_received == "down")    //move down manually
        {
            Serial.println("ok 0");
            input_buffer.direction = DOWN;
        }
        else if (serial_comm.data_received.substring(0, 5) == "force")    //force current azimuth to be replaced with new one (useful for correcting home)
        {
            Serial.println("ok 0");
            String data = serial_comm.data_received.substring(5);
            char buf[data.length() + 1];
            data.toCharArray(buf, data.length() + 1);
            double value = atof(buf);
            status_buffer.current_azimuth = value;
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

boolean calibration_check() {
    if (status_buffer.calibration != CALIBRATION_DONE)
    {
        Serial.println("error 1");
        Serial.println("ec");
        return false;
    }
    return true;
}