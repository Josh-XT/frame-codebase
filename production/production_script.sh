#!/bin/bash

stty -echoctl

echo "Frame programming script"
echo "-----------------------"

while :
do

    echo ""
    read -p "Press Enter key to start, or Ctrl-C to quit"
    echo "" >> log.txt

    # Automatically assign port depending if MacOS or Linux
    if [ "`uname`" = Darwin ]; then
        PORT=`ls /dev/cu.usbmodem*1 | grep "cu."`
    else
        PORT=/dev/ttyACM0
    fi

    # Create timestamp
    NOW=`date -u +'%d/%m/%Y - %H:%M:%S'`

    # Unlock chip
    echo "$NOW - Unlocking chip" | tee -a log.txt
    arm-none-eabi-gdb \
        -nx \
        --batch-silent \
        -ex "target extended-remote ${PORT}" \
        -ex "monitor swd_scan" \
        -ex "attach 1" \
        -ex "monitor erase_mass" \
        2> /dev/null

    # Erase chip (same thing as before, but here we do want to catch the error)
    echo "$NOW - Erasing chip" | tee -a log.txt
    arm-none-eabi-gdb \
        -nx \
        --batch-silent \
        -ex "target extended-remote ${PORT}" \
        -ex "monitor swd_scan" \
        -ex "attach 1" \
        -ex "monitor erase_mass" \
        2> /dev/null
    
    # If successful, continue otherwise throw and error and return to top of loop
    if [ $? -eq 0 ]; then

        # Get and print device ID
        echo -n "$NOW - " | tee -a log.txt
        arm-none-eabi-gdb \
            -nx \
            --batch-silent \
            -ex "target extended-remote ${PORT}" \
            -ex "monitor swd_scan" \
            -ex "attach 1" \
            -ex "set logging file /dev/stdout" \
            -ex "set logging enabled on" \
            -ex "monitor read deviceid" \
            -ex "set logging enabled off" \
            2> /dev/null \
            | tee -a log.txt

        # Get and print device address
        echo -n "$NOW - " | tee -a log.txt
        arm-none-eabi-gdb \
            -nx \
            --batch-silent \
            -ex "target extended-remote ${PORT}" \
            -ex "monitor swd_scan" \
            -ex "attach 1" \
            -ex "set logging file /dev/stdout" \
            -ex "set logging enabled on" \
            -ex "monitor read deviceaddr" \
            -ex "set logging enabled off" \
            2> /dev/null \
            | tee -a log.txt

        # Program sections
        echo "$NOW - Programming chip. Please wait"
        arm-none-eabi-gdb \
            -nx \
            --batch-silent \
            -ex "target extended-remote ${PORT}" \
            -ex 'monitor swd_scan' \
            -ex 'attach 1' \
            -ex 'load' \
            -ex 'compare-sections' \
            -ex 'kill' \
            frame-firmware-v*.hex \
            2> /dev/null

        # If successful, start the camera focusing script otherwise throw error
        if [ $? -eq 0 ]; then
            echo "$NOW - Programmed successfully" | tee -a log.txt

            # Run the camera focusing script
            python3 focus_camera_script.py 2> /dev/null

            # Clear the download counter and test display
            if [ $? -eq 0 ]; then

                echo -e -n $"\r\033[2K"
                echo "$NOW - Camera focused" | tee -a log.txt
                python3 test_display_script.py 2> /dev/null
                echo -n "                        Press y if display is working, otherwise n"
                read -s -n1 input

                # If okay, test the microphone
                if [ $input == "y" ]; then
                    echo -e -n $"\r\033[2K"
                    echo "$NOW - Display okay" | tee -a log.txt

                    python3 test_microphone_script.py #2> /dev/null
                    echo -e -n $"\r\033[2K"
                    echo -n "                        Press y if microphone is working, otherwise n"
                    read -s -n1 input

                    # Done
                    if [ $input == "y" ]; then
                        echo -e -n $"\r\033[2K"
                        echo "$NOW - Microphone okay" | tee -a log.txt
                        echo "$NOW - Done" | tee -a log.txt
                    else
                        echo -e -n $"\r\033[2K"
                        echo "$NOW - Error: Microphone not working" | tee -a log.txt
                    fi

                else
                    echo -e -n $"\r\033[2K"
                    echo "$NOW - Error: Display not working" | tee -a log.txt
                fi

            else
                echo -e -n $"\r\033[2K"
                echo "$NOW - Error: Could not connect to start focusing" | tee -a log.txt
            fi

        else
            echo "$NOW - Error: Chip could not be programmed" | tee -a log.txt
        fi

    else
        echo "$NOW - Error: Chip not found" | tee -a log.txt
    fi

done