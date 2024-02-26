import socket
import matplotlib.pyplot as plt
import math
import numpy as np
import threading
import sys

def send_data():
    print("thread Started")

    global ended
    while True:
        if ended:
            break
        userInput = input()
        if (userInput.lower() == "q" or userInput.lower() == "quit"):
            ended = True
            break
        inputArray = [char for char in userInput]
        for char in userInput:
            tcpipClient.send(char.encode())

if __name__ == "__main__":
    ended = False
    # Set up TCP connection
    tcpipClient = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcpipClient.connect(('192.168.1.1', 288))
    print("connected")
    # Define the maximum degrees and cm values for the radial graph
    maxDegrees = 360 # Maximum degrees for the radial graph
    maxCm = 100 # Maximum cm value for the radial graph

    # Set up figure for the radial graph
    fig = plt.figure()
    plt.ion()
    ax = fig.add_subplot(111)
    botPoint, = ax.plot([0], [0], 'r.') # Plot initial point at (0, 0)
    botDirectionPoint, = ax.plot([0], [18], 'g.')
    ax.plot([300], [300], 'w.')
    ax.plot([300], [-300], 'w.')
    ax.plot([-300], [300], 'w.')
    ax.plot([-300], [-300], 'w.')
    ax.set_title('map') # Set title for the plot

    print("Initialized")

    tcpCapture = threading.Thread(target=send_data)
    tcpCapture.daemon = True
    tcpCapture.start()

    try:
        while True:
            if ended:
                break
            # Continuously receive and plot data from TCP stream
            dataRaw = tcpipClient.recv(1024)
            strings = dataRaw.decode().split('\x00')[:-1]
            # print(strings)
            for string in strings:
                # print(string)
                if ended:
                    break
                if len(string) < 2:
                    print(string)
                elif string[0] == 'g':
                    if string[1] == 'm': # Check if it's plotting bot movement
                        x, y, angle = map(float, string[2:].strip('()\n').split(', ')) # Extract x and y
                        dx = x + (15 * math.cos(math.radians(angle)))
                        dy = y + (15 * math.sin(math.radians(angle)))
                        botPoint.remove() # Remove previous bot point
                        botDirectionPoint.remove()
                        botPoint, = ax.plot(x, y, 'r.') # Add new bot point (red dot)
                        botDirectionPoint, = ax.plot(dx, dy, 'g.') # (green dot)
                        ax.axis('equal')
                        plt.pause(0.002) # Pause to allow plot to update
                    elif string[1] == 't': # Check if it's plotting tape
                        x, y, angle = map(float, string[2:].strip('()\n').split(', ')) # Extract x and y
                        x = x + (15 * math.cos(math.radians(angle)))
                        y = y + (15 * math.sin(math.radians(angle)))
                        ax.plot(x, y, 'y.') # (yellow dot)
                        ax.axis('equal')
                        plt.pause(0.001) # Pause to allow plot to update
                    elif string[1] == 'h': # Check if it's plotting hole
                        x, y, angle = map(float, string[2:].strip('()\n').split(', ')) # Extract x and y
                        x = x + (15 * math.cos(math.radians(angle)))
                        y = y + (15 * math.sin(math.radians(angle)))
                        ax.plot(x, y, 'k.') # (black dot)
                        ax.axis('equal')
                        plt.pause(0.001) # Pause to allow plot to update
                    elif string[1] == 'b': # Check if bumped into something
                        x, y, angle = map(float, string[2:].strip('()\n').split(', ')) # Extract x and y
                        x = x + (15 * math.cos(math.radians(angle)))
                        y = y + (15 * math.sin(math.radians(angle)))
                        ax.plot(x, y, 'mD') # (magenta diamond)
                        ax.axis('equal')
                        plt.pause(0.001) # Pause to allow plot to update
                    else:  
                        x, y, angle = map(float, string[2:].strip('()\n').split(', ')) # Extract x and y
                        x = x + (15 * math.cos(math.radians(angle)))
                        y = y + (15 * math.sin(math.radians(angle)))
                        ax.plot(x, y, 'b.') # (blue dot)
                        ax.axis('equal')
                        plt.pause(0.001) # Pause to allow plot to update
                elif string[0] == 'p':
                    print(string[1:])
    except:
        raise
    finally:
        ended = True
        print("finally")

plt.close('all')
tcpipClient.close()
sys.exit()