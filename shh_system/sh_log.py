import time

def log(msg,level='info',date=time.ctime(),file='log.txt'):
    with open(file,'a') as f:
        f.write(level+'-'+date+'-'+msg+'\n')
