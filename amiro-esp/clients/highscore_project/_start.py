import sys
import threading
import highscore_ui
from highscore_esp_connection import ESP_AMiRo


# CAN-Bus Topic des AMiRos
# TOPICID_HIGHSCORE = 2
# (Stand 14.04.22)
topicid_highscore = 2

# Default AMiRo IP
amiro_ip = '129.70.148.10'





class ESP_Thread(threading.Thread):
    def __init__(self, esp):
        super().__init__()
        self.esp = esp

    def run(self):
        pass
        # self.esp.start()

    def stop(self):
        self.loop.call_soon_threadsafe(self.loop.stop)


if __name__ == '__main__':
    """Threads initiieren, GUI starten.
    ESP Verbindung trennen nach schließen des Fensters über 'X'."""

    esp = ESP_AMiRo(amiro_ip, topicid_highscore)
    threadESP = ESP_Thread(esp)
    threadESP.start()
    

    gui = highscore_ui.HighscoreApp(esp)
    gui.run()

    esp.disconnect()
    sys.exit()
