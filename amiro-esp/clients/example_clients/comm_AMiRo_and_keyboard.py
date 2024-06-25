#
# Example communication between AMiRo and Keyboard via Wi-Fi (ESP)
#

import threading
import tkinter as tk
import sys
import time
from amiro_remote.client import AmiroRemoteClient
from amiro_remote.amiro import LightRingColor


class CustomAMiRoThread(threading.Thread):
    def __init__(self, root):
        threading.Thread.__init__(self)
        self.root = root
        self.direction = None
        self.stop_flag = threading.Event()
        self.amiro = AmiroRemoteClient("129.70.148.56", 1234)  

        # connect to the Wi-Fi
        self.amiro.connect()
        time.sleep(2)
        try:
            if not self.amiro.is_connected():
                raise ConnectionRefusedError(
                    "Wi-Fi connection of AMiRo cannot be established!")
            else:
                print(" Connection is established for the AMiRo's ESP!")
                print(" Use 'ARROW' keys on your keyboard to talk to the AMiRo, \n and 'q' key to quit from the thread & exit!")
                self.amiro.set_motor(0, 0)
                time.sleep(1)
                self.amiro.set_light(LightRingColor.Off)

        except ConnectionRefusedError as e:
            print(" Connection Refused Err: ", e)
            print(" Please check the IP address and port no. of your AMiRo!")       
            sys.exit(0)
        except Exception as e:
            print(" Exception Err: ", e)
            self.amiro.disconnect()
            sys.exit()

    def run(self):
        while not self.stop_flag.is_set():
            try:

                if self.direction == "up":                    
                    self.amiro.set_light(LightRingColor.Green)  # Green
                    self.amiro.set_motor(0.3, 0)
                    self.direction=None   # to reduce the unnecessarily repeated msgs' transmission rate

                elif self.direction == "down":                    
                    self.amiro.set_light(LightRingColor.Red)  # red
                    self.amiro.set_motor(0, 0)
                    self.direction=None

                elif self.direction == "left":                    
                    self.amiro.set_light(LightRingColor.Blue)  # Blue
                    self.amiro.set_motor(0.05, 0.5)
                    self.direction=None

                elif self.direction == "right":                    
                    self.amiro.set_light(LightRingColor.Blue)  # Blue
                    self.amiro.set_motor(0.05, -0.5)
                    self.direction=None

                else:
                    # simply wait for the command from Keyboard 
                    time.sleep(0.5)

                # Check the Wi-Fi connection status (e.g. due to AMiRo turn off or battery empty)
                if not self.amiro.is_connected():
                    raise ConnectionAbortedError(
                        "The Wi-Fi connection on AMiRo is lost!")

            except KeyboardInterrupt:
                print("\nKeyboard Interrupt!")
                time.sleep(1)
                self.amiro.set_light(LightRingColor.Off)
                time.sleep(1)
                self.amiro.set_motor(0, 0)
                self.amiro.disconnect()
                sys.exit(0)

            except ConnectionAbortedError as e:
                print(" Connection Abort Err: ", e)
                self.amiro.set_motor(0, 0)
                time.sleep(1)
                self.amiro.set_light(LightRingColor.Off)                 
                sys.exit(0)           

            except Exception as e:
                print(" Exception Err: ", e)
                time.sleep(1)
                self.amiro.set_light(LightRingColor.Off)
                time.sleep(1)
                self.amiro.set_motor(0, 0)
                self.amiro.disconnect()
                sys.exit(0)

            self.stop_flag.wait(0.1)
            

    def stop(self):
        self.stop_flag.set()
        # time.sleep(1)
        self.amiro.set_light(LightRingColor.Off)
        time.sleep(1)
        self.amiro.set_motor(0, 0)
        sys.exit(0)


class App(tk.Tk):
    def __init__(self):
        tk.Tk.__init__(self)
        self.lock = threading.Lock()
        self.label = tk.Label(self, text=" Press the 'ARROW' keys to talk to the AMiRo,\n\n'q' key to quit from the thread & exit!\n")
        self.label.pack()
        self.button = tk.Button(
            self, text="Start the AMiRo Thread", command=self.start_custom_amiro_thread)
        self.button.pack()
        self.bind("<Up>", self.move_up)
        self.bind("<Down>", self.move_down)
        self.bind("<Left>", self.move_left)
        self.bind("<Right>", self.move_right)
        self.bind("<q>", self.stop_custom_amiro_thread)

    def start_custom_amiro_thread(self):
        thread = CustomAMiRoThread(self)
        thread.start()
        self.thread = thread

    def move_up(self, event):
        with self.lock:
            self.thread.direction = "up"

    def move_down(self, event):
        with self.lock:
            self.thread.direction = "down"

    def move_left(self, event):
        with self.lock:
            self.thread.direction = "left"

    def move_right(self, event):
        with self.lock:
            self.thread.direction = "right"

    def stop_custom_amiro_thread(self, event):
        with self.lock:
            self.thread.stop()


if __name__ == '__main__':
    app = App()

    try:
        app.mainloop()

    except KeyboardInterrupt:
        print("Keyboard Interrupt")
        sys.exit(0)
    except Exception as e:
        print("Exception Err:", e)
        sys.exit(0)
