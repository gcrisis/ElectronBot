'''
由于读取角度是单独的线程，所以有可能与其他事件产生冲突，比如二者同时写串口，然后其中一个线程把所有数据都读走，
而另一个线程进入了无限等待中，解决办法是操作串口加锁，这应该是标准做法。

'''

from asyncore import read
from base64 import encodebytes
from binhex import HexBin
from cProfile import label
from cgitb import text
from fileinput import close
from operator import truediv
from quopri import encodestring
import sched
from sqlite3 import SQLITE_DELETE
import struct
from tabnanny import check
from telnetlib import NOP
import tkinter as tk
from turtle import width
from tkinter import CENTER, BooleanVar, StringVar, Variable, ttk
from typing_extensions import IntVar
from h11 import SWITCHED_PROTOCOL
import serial
import serial.tools.list_ports
import time
import threading


servo_config = {
    'master_addr':0,
    'enabled':0,
    'servo_id':0,
    'set_angle':0,
    'read_angle':0,
    'kp':0,
    'ki':0,
    'kv':0,
    'kd':0,
    'init_angle':0,
    'torque_limit':0
}

stop = False
def btncom_callback():  
    global stop
    global checkbox_var
    if(ser.isOpen() == True):                
        button_com.config(text='打开')    
        if(servo_config['enabled'] is 1):
            stop = True
            d =bytes.fromhex('02ff00000000')  
            ser_write_read(d) 
            checkbox_var.set('0')
            entry_addr['state']='normal'
            entry_setid['state']='normal'
            servo_config['enabled']=0
        ser.close()    
        entry_com['state'] ='normal'
        info_label.config(textvariable=StringVar(value='串口关闭'))
        t=threading.Timer(1,period_run)
        t.setDaemon(True)
        t.start()
        print('关闭')
    else:
        ser.baudrate = 115200
        ser.port = entry_com.get()[0:4]
        ser.open()
        # time.sleep(2.5)     #如果使用arduino，需要这里延迟一段时间，不知道为啥arduino会重启。
        d =bytes.fromhex('010000000000')
        read = ser_write_read(d)
        if read is False:
            ser.close()
            t=threading.Timer(1,period_run)
            t.setDaemon(True)
            t.start()
            info_label.config(textvariable=StringVar(value='端口错误'))
            return
        entry_com['state'] = 'disable'
        info_label.config(textvariable=StringVar(value='串口打开'))
        button_com.config(text='关闭')        
        print('打开')     

def btn_setid_callback():
    if(ser.is_open):
        d =bytes.fromhex('0221')  
        a = int(entry_setid.get()) 
        servo_config['servo_id']=a
        read = ser_write_read(d+a.to_bytes(4,'little'))
        if(read is False):
            info_label.config(textvariable=StringVar(value='串口通信失败'))
        elif(chr(read[1]) is 's'):
            d =bytes.fromhex('01')
            a = int(entry_setid.get()) 
            servo_config['master_add']=a
            ser_write_read(d+a.to_bytes(5,'little'))
            entry_setid.config(foreground="green")
            if(entry_addr.get() is not '0'):
                entry_addr.config(textvariable = tk.StringVar(value = entry_setid.get()),foreground="green")
            info_label.config(textvariable=StringVar(value='舵机ID：'+entry_setid.get()))
        else:
            entry_setid.config(foreground="red")
            info_label.config(textvariable=StringVar(value='操作舵机失败，舵机未连接或地址错误'))
        print("setid"+entry_setid.get())
    else:
        info_label.config(textvariable=StringVar(value='串口未打开'))

def checkbox_callback():  
    if ser.is_open:
        global stop
        global recv_data
        stop = False
        d =bytes.fromhex('02ff')  
        a = int(checkbox_var.get()) 
        read = ser_write_read(d+a.to_bytes(4,'little'))
        if(chr(read[1]) is 's'):
            if(a is 1):
                d =bytes.fromhex('021100000000')     
                read = ser_write_read(d)     
                slide.set(float(read[2:]))          
                recv_data = threading.Thread(target=thread_recv)
                recv_data.setDaemon(True)
                recv_data.start()
                servo_config['enabled']=1
                entry_addr['state']='disable'
                entry_setid['state']='disable'
                info_label.config(textvariable=StringVar(value='舵机使能'))
            else:
                stop = True
                servo_config['enabled']=0
                entry_addr['state']='normal'
                entry_setid['state']='normal'
                info_label.config(textvariable=StringVar(value='舵机禁用'))
        else:
            checkbox_var.set('0')
            info_label.config(textvariable=StringVar(value='操作舵机失败，舵机未连接或地址错误'))
        print("enable"+checkbox_var.get())
    else:
        info_label.config(textvariable=StringVar(value='串口未打开'))
        checkbox_var.set('0')

def entry_edit_callback(entry):
    entry.config(foreground="black")
    return True


def enter_callback(entry,cmd):
    if ser.is_open:
        if(entry is entry_addr):
            d =bytes.fromhex('01')
            a = int(entry.get()) 
            ser_write_read(d+a.to_bytes(5,'little'))
            info_label.config(textvariable=StringVar(value='I2C地址：'+entry.get()))
            entry.config(foreground="green")
        elif(entry is entry_slide):
            slide.set(entry.get())
        else:
            d =bytes.fromhex('02'+cmd)  
            a = float(entry.get())         
            # ser.write(d+struct.pack('<f',a))
            read = ser_write_read(d+struct.pack('<f',a))   
            if chr(read[1]) is 's':
                info_label.config(textvariable=StringVar(value='设置成功'))
                entry.config(foreground="green")
            else:
                entry.config(foreground="red")
                info_label.config(textvariable=StringVar(value='操作舵机失败，舵机未连接或地址错误'))       
    else:
        info_label.config(textvariable=StringVar(value='串口未打开'))

def slide_callback(val):
    if ser.is_open:
        if(int(checkbox_var.get())):
            d =bytes.fromhex('0201')  
            a = float(val) 
            ser_write_read(d+struct.pack('<f',a))
            entry_slide.config(textvariable = tk.StringVar(value = val),foreground="green")
        else:
            info_label.config(textvariable=StringVar(value='舵机禁用'))
    else:
        info_label.config(textvariable=StringVar(value='串口未打开'))
 
#######串口##################
mutex = threading.Lock()
def ser_write_read(data):
    global ser
    timeout = 100
    try:
        mutex.acquire(blocking=True)
        if ser.writable:
            ser.write(data)
        # else:
        #     mutex.release()
        #     return False
        while ser.in_waiting is 0:
            timeout -= 1
            time.sleep(0.01)
            if(timeout == 0):
                print("ser read timeout")
                mutex.release()
                return False
            else:
                continue 
        read = ser.read_all()
        mutex.release()
        return read 
    except:
        info_label.config(textvariable=StringVar(value='串口未打开'))

def thread_recv():
    global ser 
    global stop
    d =bytes.fromhex('021100000000')
    while stop is False:       
        read = ser_write_read(d)
        if(read is False):
             info_label.config(textvariable=StringVar(value='串口通信失败'))
        elif(chr(read[1]) is 'f'):
            info_label.config(textvariable=StringVar(value='读取角度失败'))
            checkbox_var.set('0')
            entry_addr['state']='normal'
            break        
        else:               
            entry_real.config(textvariable=tk.StringVar(value = read[2:]))                
        time.sleep(0.05) 

'''
发现串口设备
'''
def period_run(): 
    global port_list   
    global ser
    if ser.is_open is False:
        port_list = list(serial.tools.list_ports.comports())
        entry_com.config(values=port_list)
        t=threading.Timer(1,period_run)
        t.setDaemon(True)
        t.start()

t=threading.Timer(1,period_run)
t.setDaemon(True)
t.start()

port_list = list(serial.tools.list_ports.comports())

ser = serial.Serial()
recv_data = threading.Thread(target=thread_recv)
##########调度器#########


###############窗口##############
def close_window():
    global stop         
    if(ser.is_open):
        d =bytes.fromhex('010000000000')  
        ser_write_read(d)    
        if(servo_config['enabled'] is 1):
            d =bytes.fromhex('02ff00000000')  
            ser_write_read(d)  
        ser.close()
    root.destroy()

root = tk.Tk()
win_width = root.winfo_screenwidth()
win_height = root.winfo_screenheight()
root.title('ServoSetupTool-科G栈')
x=win_width/2-185
y=win_height/2-150
root.geometry("370x300+%d+%d" %(x,y))
root.resizable(False,False)
root.protocol('WM_DELETE_WINDOW',close_window)

#################地址#########################################
labelframe1 = tk.LabelFrame(root,height='100',width='100',text='地址')
labelframe1.place(relx=0.05,rely=0.02,relwidth=0.9,relheight=0.25)

label_addr=tk.Label(labelframe1,text='I2C地址：')
label_addr.place(relx=0.03,rely=0.16)

entry_addr = tk.Entry(labelframe1,validate = "key",validatecommand=(lambda:entry_edit_callback(entry_addr)))
entry_addr.config(textvariable=StringVar(value="0"))
entry_addr.place(relx=0.23,rely=0.16,relwidth=0.1)
entry_addr.bind("<Return>",lambda _:enter_callback(entry_addr,''))

checkbox_var = tk.StringVar(value='0')
checkbox = tk.Checkbutton(labelframe1,text='Enabled',variable=checkbox_var,command=checkbox_callback)
checkbox.place(relx=0.37,rely=0.15)

button_setid = tk.Button(labelframe1,text="SetID",command=btn_setid_callback)
button_setid.place(relx=0.63,rely=0.13,relwidth=0.2,relheight=0.5)

entry_setid = tk.Entry(labelframe1,validate = "key",validatecommand=(lambda:entry_edit_callback(entry_setid)))
entry_setid.config(textvariable=StringVar(value="2"))
entry_setid.place(relx=0.85,rely=0.16,relwidth=0.1)

###################角度###################################
labelframe2 = tk.LabelFrame(root,height='100',width='100',text='角度')
labelframe2.place(relx=0.05,rely=0.27,relwidth=0.9,relheight=0.25)

slide = tk.Scale(labelframe2,orient='horizontal',from_=0,to=180,showvalue=False,)
slide.set(90)
slide.config(command=slide_callback)
slide.place(relx=0.03,rely=0.16,relwidth=0.6)

entry_slide = tk.Entry(labelframe2,validate = "key",validatecommand=(lambda:entry_edit_callback(entry_slide)))
entry_slide.place(relx=0.63,rely=0.16,relwidth=0.08)
entry_slide.config(textvariable=StringVar(value="90"))
entry_slide.bind("<Return>",lambda _:enter_callback(entry_slide,'01'))

label_real=tk.Label(labelframe2,text='Real:')
label_real.place(relx=0.72,rely=0.16)

entry_real = tk.Entry(labelframe2)
entry_real.place(relx=0.82,rely=0.16,relwidth=0.13)
###########参数#####################################
labelframe3 = tk.LabelFrame(root,height='100',width='100',text='参数')
labelframe3.place(relx=0.05,rely=0.52,relwidth=0.9,relheight=0.25)

label_kp=tk.Label(labelframe3,text='Kp:')
label_kp.grid(row=0,padx=5)

entry_kp = tk.Entry(labelframe3,width=7,validate = "key",validatecommand=(lambda:entry_edit_callback(entry_kp)))
#entry1.place(relx=0.85,rely=0.16,relwidth=0.1)
entry_kp.grid(row=0,column=1)
entry_kp.config(textvariable=StringVar(value="10"))
entry_kp.bind("<Return>",lambda _:enter_callback(entry_kp,'22'))

label_ki=tk.Label(labelframe3,text='Ki:')
label_ki.grid(row=0,column=2)

entry_ki = tk.Entry(labelframe3,width=7,validate = "key",validatecommand=(lambda:entry_edit_callback(entry_ki)))
entry_ki.grid(row=0,column=3,)
entry_ki.config(textvariable=StringVar(value="0"))
entry_ki.bind("<Return>",lambda _:enter_callback(entry_ki,'23'))

label_kv=tk.Label(labelframe3,text='Kv:')
label_kv.grid(row=0,column=4)

entry_kv = tk.Entry(labelframe3,width=7,validate = "key",validatecommand=(lambda:entry_edit_callback(entry_kv)))
entry_kv.grid(row=0,column=5)
entry_kv.config(textvariable=StringVar(value="0"))
entry_kv.bind("<Return>",lambda _:enter_callback(entry_kv,'24'))

label_kd=tk.Label(labelframe3,text='Kd:')
label_kd.grid(row=0,column=6)

entry_kd = tk.Entry(labelframe3,width=7,validate = "key",validatecommand=(lambda:entry_edit_callback(entry_kd)))
entry_kd.grid(row=0,column=7)
entry_kd.config(textvariable=StringVar(value="50"))
entry_kd.bind("<Return>",lambda _:enter_callback(entry_kd,'25'))

label_initangle=tk.Label(labelframe3,text='初始角度:')
label_initangle.grid(row=1,columnspan=3)

entry_initangle = tk.Entry(labelframe3,width=7,validate = "key",validatecommand=(lambda:entry_edit_callback(entry_initangle)))
entry_initangle.grid(row=1,column=3)
entry_initangle.config(textvariable=StringVar(value="90"))
entry_initangle.bind("<Return>",lambda _:enter_callback(entry_initangle,'27'))

label_torqlimit=tk.Label(labelframe3,text='力矩限制:')
label_torqlimit.grid(row=1,column=4,columnspan=3)

entry_torqlimit = tk.Entry(labelframe3,width=7,validate = "key",validatecommand=(lambda:entry_edit_callback(entry_torqlimit)))
entry_torqlimit.grid(row=1,column=7)
entry_torqlimit.config(textvariable=StringVar(value="0.5"))
entry_torqlimit.bind("<Return>",lambda _:enter_callback(entry_torqlimit,'26'))

##########################################

label_com=tk.Label(root,text='端口:')
label_com.place(relx=0.05,rely=0.80)

entry_com = ttk.Combobox(root,values=port_list)
entry_com.place(relx=0.15,rely=0.80,relwidth=0.5)
if(len(port_list)==0):
    pass
elif len(port_list)==1:
    entry_com.set(port_list[0])
else:
    entry_com.set(port_list[1])

button_com = tk.Button(root,text='打开',command=btncom_callback)
button_com.place(relx=0.75,rely=0.80,relwidth=0.2)

info_label = tk.Entry(root,textvariable=StringVar(value='串口关闭'))
info_label['state'] = 'disable'
info_label.place(relx=0.05,rely=0.9,relwidth=0.6)
root.mainloop()

