import Live_view_DSLR
import Live_view_FLIR
import math
from pathlib import Path
import serial
import time
import os


class CoordXY:

    def __init__(self, port, baud_rate):
        self.port = port
        self.baud_rate = baud_rate

        # make sure the 'COM#' is set according the Windows Device Manager
        self.ser = serial.Serial(self.port, self.baud_rate, timeout=None)
        time.sleep(2)

        # calling the class for the first time is meant to aid in finding attached cameras.
        # the name of the connected camera is then stored in DSLR.camera_model
        self.FLIR = Live_view_FLIR.customFLIR()
        """
        self.DSLR = Live_view_DSLR.customDSLR()
        if self.DSLR.camera_model is None:
            # exit program when no camera is connected
            exit()
        """
        # if a camera is connected, it needs to be initialised before use
        self.FLIR.initialise_camera()

    def home_all(self):
        self.ser.write(b"HOME X\n")
        for i in range(3):
            line = self.ser.readline()
            if line:
                string = line.decode()
                print(string)
        self.ser.write(b"HOME Y\n")

        for i in range(3):
            line = self.ser.readline()
            if line:
                string = line.decode()
                print(string)
        self.ser.write(b"HOME Z\n")
        for i in range(3):
            line = self.ser.readline()
            if line:
                string = line.decode()
                print(string)

        self.ser.write(b"X 550 Y 550 Z 400 CP 0 CY 0\n")
        for i in range(3):
            line = self.ser.readline()
            if line:
                string = line.decode()
                print(string)

        self.ser.write(b"HOME CP                       \n")
        for i in range(3):
            line = self.ser.readline()
            if line:
                string = line.decode()
                print(string)

        self.ser.write(b"HOME CY                       \n")
        for i in range(3):
            line = self.ser.readline()
            if line:
                string = line.decode()
                print(string)

        self.ser.write(b"X 550 Y 800 Z 1000 CP 520 CY 220\n")
        for i in range(3):
            line = self.ser.readline()
            if line:
                string = line.decode()
                print(string)

        print("\n HOMED ALL AXES!\n")

    def run_example_capture(self, steps=180, output_folder=str(Path.cwd())):
        for i in range(steps):
            val = 2 * i / (steps / math.pi)
            x_pos = int(math.sin(val) * 500 + 550)
            y_pos = int(math.sin(-val * 2) * 500 + 550)
            z_pos = int(math.cos(val) * 1000 + 1000)
            cp_pos = int(math.sin(val + 0.5 * math.pi) * 80 + 510)
            cy_pos = int(-math.sin(val) * 100 + 220)

            command = "X " + str(x_pos) + \
                      " Y " + str(y_pos) + \
                      " Z " + str(z_pos) + \
                      " CP " + str(cp_pos) + \
                      " CY " + str(cy_pos) + "                              \n"
            self.ser.write(command.encode(encoding='UTF-8'))

            for j in range(3):
                line = self.ser.readline()
                if line:
                    string = line.decode()
                    print(string)

            time.sleep(0.3)
            self.FLIR.capture_image(output_folder + "\\image_coordxy_" + str(i) + ".jpg")


    def close(self):
        self.ser.close()


if __name__ == '__main__':
    # initialise camera robot
    coordxy = CoordXY(port="COM8", baud_rate=9600)

    coordxy.FLIR.set_gain(gain=2)
    coordxy.FLIR.set_gamma(gamma=0.6)
    coordxy.FLIR.set_white_balance(red=1.58, blue=1.79)
    coordxy.FLIR.configure_exposure(exposure_time_to_set=60000)

    """
    # example DSLR settings
    iso = "500"
    aperture = "3.5"
    shutterspeed = "1/8"
    whitebalance = "Fluorescent"
    compression = "JPEG (BASIC)"
    
    coordxy.DSLR.set_iso(iso)
    coordxy.DSLR.set_aperture(aperture)
    coordxy.DSLR.set_shutterspeed(shutterspeed)
    coordxy.DSLR.set_whitebalance(whitebalance)
    coordxy.DSLR.set_compression(compression)
    """

    # home all axes
    coordxy.home_all()

    # run example capture process over set number of increments and save out captured images
    out_dir = "example_images"
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    coordxy.run_example_capture(steps=180, output_folder=out_dir)

    # de-initialise camera robot by closing the serial connection
    coordxy.close()
