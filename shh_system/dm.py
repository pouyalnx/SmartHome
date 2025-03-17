import os
import syslog
import time
import pathlib
import signal
import socket
import threading
import select
import json
import random
import logging

######################################################################################
io={}
io_changed=False
def io_update(signalnumber,frame):
    name_list=[]
    global io_changed
    with open('address.json') as f:
        for name,address in json.load(f).items():
            name_list.append(name)
            if not(name in io.keys()):
                io[name]=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
                try:
                    io[name].connect(tuple(address))
                except:
                    io.pop(name)
                io_changed=True
    
    for i in io.keys():
        if not(i in name_list):
            io[i].close()
            io.pop(i)
            io_changed=True
    
signal.signal(signal.SIGIO,io_update)
#######################################################################################
pids={}
def pid_update(signalnumber,frame):
    with open('pid.json') as f:
        pids=json.load(f)
    pids['dm']=os.getpid()
    with open('pid.json','w') as f:
        json.dump(pids,f)

signal.signal(signal.SIGUSR1,io_update)        
#######################################################################################

device_mem_cache={}
pid_counter=1
def conv_req_to_airreq(reqs):
    global pid_counter
    
    [uid,addr,cmd,arg]=reqs

    air_msg_pid=pid_counter
    pid_counter=pid_counter+1

    if not(addr in device_mem_cache):
        try:
            with open('devices/'+addr+'/config.json') as f:
                device_mem_cache[addr]=json.load(f)
        except:
            return (0,"","cant find device")

    try:
        air_msg_addr=device_mem_cache[addr]['id']
        air_msg_pipe=device_mem_cache[addr]['pipe']
        air_msg_cmd=device_mem_cache[addr]['cmd'][cmd]
        return (str(air_msg_pid),air_msg_pipe,str(air_msg_pid)+':'+air_msg_addr+':'+air_msg_cmd+':'+arg+';')
    except:
        return (0,"","device information invalid")
    

def conv_airreq_to_req(airreq):
    return airreq.split(':')


#######################################################################################


pending_messages={} #pid->uid


def air_transfer(uid,pid,msg,pipe,timeout):
    if not(pipe in io.keys()):
        return (0,"io not avalible now")
    pending_messages[pid]=uid
    print("pending messange",pending_messages)
    try:
        io[pipe].send(msg.encode())
        ##wait for ret msg
        with user_list[uid][1]:
            for i in range(timeout):
                user_list[uid][1].wait(1)
#                print('timeout',i)
                for cnt in range(len(user_list[uid][2])):
#                    print("cnt",cnt)
                    if user_list[uid][2][cnt][0]==pid:
                        msg=user_list[uid][2][cnt][1]
                        user_list[uid][2].pop(cnt)
                        return (pid,msg)
        return (0,"time out") 
    except:
        return (0,"pipe broken")



def air_listen_thread():
    global io
    global io_changed
    poll=select.poll()
    poll_timeout=1000

    for i,j in io.items():
        poll.register(j.fileno(),select.POLLIN)

    while(1):
        file_req=poll.poll(poll_timeout)
        for i in file_req:
            if i[1] & select.POLLIN:
                file_not_find=True
                for name,sock in io.items():
                    if sock.fileno()==i[0]:
                        msg=sock.recv(1024).decode('ascii')
                        if msg=='':
                            print("lost this connectors")
                            poll.unregister(sock.fileno())
                        try:
                            for i in msg.split(';'):
                                print(i)
                                if i!='' and i!='\n':
                                    pid=conv_airreq_to_req(i)[0]
                                    print("resived pid",pid)
                                    if pid in pending_messages.keys():
                                        user_list[pending_messages[pid]][2].append([pid,i])
                                        with user_list[pending_messages[pid]][1]:
                                            user_list[pending_messages[pid]][1].notify()
                                    else:
                                        print("unknown user msg or delected user ->",i)
                                    #    logging.info("unknown user msg or delected user ->",i)
                        except:
                            print("error in data format->",msg)
                        #    logging.info("error in data format->",msg)
                        file_not_find=False
                        break
                if file_not_find==True:
                    io_changed=True

        if io_changed:
            poll=select.poll()
            for i,j in io.items():
                poll.register(j.fileno(),select.POLLIN)
            io_changed=False





#######################################################################################

uid_list=[]
user_list={}

def unpacking_msg(msg):
    msg_part=msg.split(':')
    if len(msg_part)%4!=0:
        return None
    return msg_part


def user_send(sc,data):
    sc.send(data.encode())

def user_res(sc):
    return sc.recv(1024).decode('ascii')

def user_manager(sc,address,uid,lock):
    try:
        user_send(sc,uid+'\n')
        while(1):
            send_buffer=''
            user_data=user_res(sc)
            if user_data=='':
                print("connection lost")
                user_list.pop(uid)
                sc.close()
                break

            for i in user_data.split(';'):
                if i!='' and i!='\n':
                    #print(i)
                    reqs=unpacking_msg(i)
                    #print(reqs)
                    if reqs==None:
                        send_buffer+=i+"+packing_error;"
                    else:
                        req_id=reqs[0].split('-')
                        #print(req_id)
                        if len(req_id)==2 and req_id[0]==uid:
                            #pid=req_id[1] #not application yet
                            (air_pid,air_pip,air_msg)=conv_req_to_airreq(reqs)
                            if air_pid!=0:
                            #    print(air_pid,air_pip,air_msg)
                                pid_stat,msg=air_transfer(uid,air_pid,air_msg,air_pip,20)
                                if pid_stat==0:
                                    send_buffer+=reqs[0]+':+'+msg+';'
                                else:
                                    send_buffer+=reqs[0]+':+'+msg.split(':')[-1]+';'
                            else:
                                send_buffer+=reqs[0]+':+'+air_msg+';'
                        else:   
                            send_buffer+=i+"+wrong uid;"
            user_send(sc,send_buffer)
    except:
        print("pipe broken")
        user_list.pop(uid)
        sc.close()
        return      

#######################################################################################
def get_uid(lent):
    if lent<=0:
        return None
    st=str(random.random()).split('.')[1]
    if len(st)>lent:
        return st[0:lent-1]
    else:
        return st.zfill(lent)
#######################################################################################
with open('config.json') as f:
    config=json.load(f)

main_socket=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
main_socket.bind(tuple(config['dm_addr']))
main_socket.listen(config['dm_limit'])


io_update(0,0)
threading.Thread(target=air_listen_thread).start()

pid_update(0,0)


while 1:
    try:
        sc,addr=main_socket.accept()
        uid=get_uid(config['uid_len'])
        while(uid in uid_list):
            uid=get_uid(config['uid_len'])
        lob=threading.Condition()
        ut=threading.Thread(target=user_manager,args=(sc,addr,uid,lob))
        uid_list.append(uid)
        user_list[uid]=[sc,lob,[]]
        ut.start()
    except:
        logging.info("pip or threading mechanism has broken {}".format(time.ctime()))
        break

main_socket.close()