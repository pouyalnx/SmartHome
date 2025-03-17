import socket


class demon:
    def __init__(self):
        self.soc=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.pidcounter=0

    def connect(self,ip,port):
        self.addr=(ip,port)
        self.soc.connect(self.addr)
        self.uid=self.soc.recv(1024).decode('ascii').split()[0]
        if self.uid=='':
            self.soc.close()
        return self.uid

    def close(self):
        self.close()
    #################################################################    
    def rec(self):
        return self.soc.recv(1024).decode('ascii')

    def snd(self,data):
        self.soc.send(data.encode())
    #################################################################

    def create_msg(self,addr,cmd,value):
        self.pidcounter+=1
        return self.uid+'-'+str(self.pidcounter)+':'+addr+':'+cmd+':'+str(value)+';'

    def send_req(self,addr,cmd,value):
        msg=self.create_msg(addr,cmd,value)
        print(msg)
        self.snd(msg)
        resp=self.rec()
        if resp=='':
            return None #pipe maybe broken
        return resp.split(':')[-1]

    def Set(self,addr,value):
        return self.send_req(addr,"set",value)
        

    def Get(self,addr):
        return self.send_req(addr,"get",'')