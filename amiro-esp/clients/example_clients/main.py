from amiro_remote.client import AmiroRemoteClient
from amiro_remote.amiro import Topic, TopicData
from time import sleep


if __name__ == "__main__":
    amr = AmiroRemoteClient("129.70.148.80", 1234, topics=[Topic.FloorAmbient]) 
    
    with amr: 
        floor_values: TopicData
        while True:
            floor_values = amr.latest.get(Topic.FloorAmbient)
            if floor_values is None:
                print(floor_values)
            else:
                color = [1,2,2,3,3,0,0,1,1,2,1,3,4,5,0,1,2,3,4,1]
                remote_values = [1,2,3,4]
                if floor_values.values[0] < 500:
                    color = [2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2]
                    remote_values = [8,9,10,11]
                amr.set_light(color)
                amr.set_topic(Topic.Remote, remote_values)
                print("Floor Ambient Values:")
                print("0: ", floor_values.values[0])
                print("1: ", floor_values.values[1])
                print("2: ", floor_values.values[2])
                print("3: ", floor_values.values[3])
                print("")
            sleep(1)
