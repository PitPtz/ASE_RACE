import socket
import sys
import argparse
import struct


class ESP_AMiRo:

    def __init__(self, amiro_ip, topicid_highscore) -> None:
        self.amiro_ip = amiro_ip
        self.topicid_highscore = topicid_highscore
        socket.setdefaulttimeout(5)

    def connect(self):
        self.connect_to_amiro(self.amiro_ip, 1)


    def disconnect(self):
        self.connect_to_amiro(self.amiro_ip, 0)


    def got_weighting(self, value):
        '''Switch between weighting_data and score_data.
        value: 0 -> weighting_data, 1 -> score_data'''

        if (value == 0):

            judgment = self.connect_to_amiro(self.amiro_ip, 2)

            if (judgment[0] == 1):
                return judgment
            elif (judgment[0] == 2):
                return judgment
            else:
                print("Bitte Erneut Starten: Keine Gewichtung vom Amiro erfahren")
                return 5

        if (value == 1):
            score = self.connect_to_amiro(self.amiro_ip, 2)
            return score

    
    def connect_to_amiro(self,amiro_ip,status):
        '''Connection to ESP -> Wait for return. Else: Tries to restart
        Status: 0 -> Disconnect,  1 -> ConnectionTest,  >1 -> Weighting_data or Score_data'''

        if (status == 0):
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print("Disconnect")
            sock.close()
        else:
            parser = argparse.ArgumentParser(description='Recv CAN Messages over TCP')
            parser.add_argument('--hostname', help='AMiRo Hostname [amiro1, amiro2]')
            parser.add_argument('--ip', default=amiro_ip, help='AMiRo IP')
            parser.add_argument('--port', default=1234, help='tcp port')

            args = parser.parse_args()

            if args.ip and args.hostname:
                print("Use hostname or ip, not both")

            if not args.ip and not args.hostname:
                print("You need to specify an ip or hostname")
                sys.exit(-1)

            if args.hostname:
                args['ip'] = socket.gethostbyname(args.hostname)
                print('IP: %s' % args['ip'])
                
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server_address = (args.ip, args.port)
            print('connecting to {} port {}'.format(*server_address))
            sock.connect(server_address)
            print("connected")
            
            if (status == 1):
                sock.close()
                return True
            try:
                while True:
                    data = sock.recv(20) #CAN struct size is 20 ,see twai_message_t (esp-idf)
                    msg = struct.unpack('IIBBBBBBBBBxxx',data) #Flags, Identifier, Length, Data[8], 3 bytes pad to 32bit?

                    #Decode ESP Message-Identifier
                    identifier = msg[1]
                    real_id = int(bin(identifier)[3:19], 2)
                    counter = int(bin(identifier)[20:], 2) #counter = 1 -> data, counter = 0 -> timestamp

                    msg_list = list(msg)
                    msg_list[1] = real_id
                    msg_list.insert(2, counter)

                    #Filter for relevant data
                    if (real_id == self.topicid_highscore and counter == 1):

                        # check if weighting data
                        if (msg_list[4] == 1 or msg_list[4] == 2):
                            
                            if(msg_list[4] == 1):
                                print("LineFollowing")
                            else:
                                print("WallFollowing")
                            
                            print("Speed_Weighting: " +  str(msg_list[6]/100))
                            print("Accuracy_Weighting: " +  str(msg_list[8]/100))
                            print("Efficiency_Weighting: " +  str(msg_list[10]/100))
                            
                            #weighting_data
                            return(msg_list[4],msg_list[6]/100,msg_list[8]/100,msg_list[10]/100)

                        # check if cheat or error
                        elif (msg_list[4] == 0 and msg_list[6] == 0 and msg_list[8] == 0 and msg_list[10] == 0):
                            print("Cheat or unexpected Error occurred! Lap does not count")
                            return 7

                        # must be score data
                        else:
                            if(msg_list[5] > 0):
                                part1 = msg_list[5]
                                part2 = msg_list[4]
                                together = "{:>08b}{:>08b}".format(part1, part2)
                                EndScore = int(together, 2)
                            else:
                                EndScore = msg_list[4]

                            if(msg_list[7] > 0):
                                part1 = msg_list[7]
                                part2 = msg_list[6]
                                together = "{:>08b}{:>08b}".format(part1, part2)
                                SpeedScore = int(together, 2)
                            else:
                                SpeedScore = msg_list[6]

                            if(msg_list[9] > 0):
                                part1 = msg_list[9]
                                part2 = msg_list[8]
                                together = "{:>08b}{:>08b}".format(part1, part2)
                                AccuracyScore = (int(together, 2))
                            else:
                                AccuracyScore = msg_list[8]
                            
                            if(msg_list[11] > 0):
                                part1 = msg_list[11]
                                part2 = msg_list[10]
                                together = "{:>08b}{:>08b}".format(part1, part2)
                                EfficiencyScore = int(together, 2)
                            else:
                                EfficiencyScore = msg_list[10]
                            
                            print("EndScore: " + str(EndScore))
                            print("SpeedScore: " +  str(SpeedScore))
                            print("AccuracyScore: " +  str(AccuracyScore))
                            print("EfficiencyScore: " +  str(EfficiencyScore))

                            #score_data
                            return(EndScore,SpeedScore,AccuracyScore,EfficiencyScore)

            except:
                sock.close()
                print("Restart")
                try:
                    self.connect_to_amiro(self.amiro_ip,2)
                except:
                    return False
                    
            finally:
                sock.close()