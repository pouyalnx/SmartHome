import socket
import threading
import json
import os
import sys
import time
import signal
from sh_log import log
#######################################################################################################################################





#######################################################################################################################################
def safe_file_write(name,text):
    
    pass


def send_signal(name,sig):
    with open('pid.json') as f:
        pids=json.load(f)
    if name in pids.keys():
        os.kill(pids[name],sig)
        return 0
    return -1


def handler(sc,addr):
    with open("cm_passwd.json") as f:
        passwd=json.load(f)
    while 1:
        try:
            data=sc.recv(1024).decode('ascii')
            if(data==''):
                sc.close()
                break
            name,key,port=data.split()
            print(name,key,port)
            if name in passwd.keys():  
                if passwd[name]==key:
                    
                    with open('address.json') as f:
                        address=json.load(f)
                    address[name]=(addr[0],int(port))
                    ##we need use safe lock
                    with open('address.json','w') as f:
                        json.dump(address,f)
                    
                    
                    ###here first must give one of free socket                    
                    log("cm loggin req accpected "+addr[0]+' '+port+' '+data,level='warning')
                    try:
                        send_signal('dm',signal.SIGIO)
                    except:
                        log("unable to send signal ",level='error')
                    sc.close()
                    break
                else:
                    log("cm loggin req wrong password "+addr[0]+' '+data,level='warning')
            else:
                log("cm logging req undefined device "+addr[0]+' '+data,level='warning')
        except:
            sc.close()
            break

########################################################################################################################################
with open('config.json') as f:
    config=json.load(f)

a={}
with open('address.json','w') as f:
    json.dump(a,f)

main_socket=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
main_socket.bind(tuple(config['cm_addr']))
main_socket.listen(config['cm_limit'])




while 1:
    try:
        s,a=main_socket.accept()
        threading.Thread(target=handler,args=(s,a)).start()
    except:
        log("cm main socket closed",level='error')
        main_socket.close()
        break