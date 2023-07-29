import time
from socket import *


IP = "localhost"
PORT = 3333
flag = ""


for i in range(0,6):
    s = socket()
    s.connect((IP, PORT))
    s.recv(2048)
    s.send(("HCAMP{"+flag+"}").encode("utf-8"))
    stime = time.time()
    s.recv(2048)
    basetime = time.time() - stime
    print("\nBasetime :", basetime)
    s.close()
    for j in range(0,10):
        s = socket()    
        s.connect((IP, PORT))
        s.recv(2048)
        flag_form = 'HCAMP{'+flag
        send_flag = flag_form+(str(j))+"}"
        stime = time.time()
        s.send(send_flag.encode("utf-8"))
        s.recv(2048)
        dtime = time.time() - stime
        print(send_flag, dtime)
        if dtime > basetime+0.5:
            flag = flag + str(j)
            s.close()
            break
        s.close()        

    