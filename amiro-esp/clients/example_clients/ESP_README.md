# ESP Breakout Board for the AMiRo

## General information
The ESP is a breakout board on top of the AMiRo. This breakout board is connected tho the internal CAN (Controller Area Network) of the AMiRo and to a local area network via wifi. All messages on the bus, including sensor values are streamed to the local area network and can be read out wireless with a corresponding client. Certainly it is also possible to send data from another device connected to the same network to the AMiRo resp. the internal CAN bus, so that the programming cable is no longer required for data transmission.
Currently, the ESP module is connected to the internal CITEC network, which is only available at Netboot machines.

## Important Notes
Please keep the following notes in mind whenever you work with the ESPs:
- Never remove the ESPs from the AMiRo
- The ESPs are connected to the CITEC network. To read/send data you must be in the same network.

## Download Miniconda
Since this package is developed with **Python 3.8** and on the Netboot machines only **Python 3.5** is available, you have to install a newer version of python. For this purpose, the easiest way is to use **Miniconda** and install it in your home directory. <br>
Follow the instructions given [here](https://docs.conda.io/projects/conda/en/latest/user-guide/install/linux.html). 
This involves the steps _Downloading_, _Verifying_ and _Installing_. 

For more information about **Miniconda**, have a look [here](https://docs.conda.io/projects/conda/en/latest/user-guide/concepts/index.html).



## Connect a Netboot machine to the ESP
To connect from a netboot machine to the ESP, the IP address of the ESP module must be identified. For this purpose, connect the ESP module via USB with the Netboot machine. Open gtkterm (or any other serial terminal), select the correct USB port (Configuration -> Port) and baudrate (115200) and reboot the ESP (_EN-Button_). The reboot should be visible by various outputs in the serial terminal and finally the IP address should be printed. 

## Execute a small example
In your AMiRo-Apps directory check out the [ASE_SoSe22](https://gitlab.ub.uni-bielefeld.de/AMiRo/AMiRo-Apps/-/tree/ASE_SoSe22/) branch. After the project initialization, go into configurations/AMiRoDefault and flash the AMiRo. Sometimes before flashing you need to do _make clean_. <br>
Before executing this example, you need to install the amiro-remote package, located in Clients/. 
Open the main.py file out of this directory in a text editor of your choice and change the IP address according to the one you found out above. Then when the AMiRo is running, you execute this python script the Floor Ambient values of the AMiRo are printed and the AMiRo lights up. Also some ints are send to the AMiRo which are printed on the DiWheelDrive.

