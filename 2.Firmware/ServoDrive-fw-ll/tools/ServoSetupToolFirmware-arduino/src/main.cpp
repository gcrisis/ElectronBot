#include <Arduino.h>
#include <SPI.h>
#include <stdio.h>
#include <string.h>
#include <Adafruit_I2CDevice.h>
#define I2C_ADDRESS 0x00
#define SET_SLAVE_ID 0x01
#define CTRL_SERVO   0x02
uint8_t i2cTxData[8];
uint8_t i2cRxData[8];
uint8_t uart_tx_buff[16];     //实际最大发送8个，但是定义成8个回传会卡死。
uint8_t uart_rx_buff[8];

uint8_t recv_ok=1;
uint8_t slave_id=0;
uint8_t trans_lens = 0;

Adafruit_I2CDevice i2c_dev = Adafruit_I2CDevice(I2C_ADDRESS);
void enable_servo(uint8_t en)
{
    i2cTxData[0] = 0xff;
    i2cTxData[1] = en;
    i2c_dev.write_then_read(i2cTxData,5,i2cRxData,5,true);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  if (!i2c_dev.begin()) {
    // Serial.print("Did not find device at 0x");
    // Serial.println(i2c_dev.address(), HEX);
    while (1);
  }
//   Serial.print("Device found on address 0x");
//   Serial.println(i2c_dev.address(), HEX);
 // enable_servo(1);
  Serial.flush();
}

void loop() {
  // put your main code here, to run repeatedly:
//   digitalWrite(13,HIGH);
//   delay(1000);
//     digitalWrite(13,LOW);
//   delay(1000);
    if(Serial.available()>0)
    {
        if(Serial.peek() == 240)    //arduino打开串口后会收到这个奇怪的数据，不知道为啥，这里就简单粗暴的读走丢掉了
        {
            Serial.read();
            digitalWrite(13,HIGH);
        }
        else
        {
            Serial.readBytes(uart_rx_buff,6);
            // Serial.println(uart_rx_buff[0]);
            // Serial.println(uart_rx_buff[1]);
            // Serial.println(uart_rx_buff[2]);
            // Serial.println(uart_rx_buff[3]);
            // Serial.println(uart_rx_buff[4]);
            // Serial.println(uart_rx_buff[5]);
            recv_ok =1;
            switch(uart_rx_buff[0])
            {
                case SET_SLAVE_ID:
                    slave_id = uart_rx_buff[1];
                    uart_tx_buff[0]=SET_SLAVE_ID;
                    uart_tx_buff[1]='s';
                    trans_lens = 2;
                break;                
                case CTRL_SERVO:
                    switch (uart_rx_buff[1])
                    {
                    case 0x01:  //设置角度
                    break;
                    case 0x02:  //设置速度
                    break;
                    case 0x03:  //设置转矩
                    break;
                    case 0x11:  //读取角度
                    break;
                    case 0x12:  //读取速度
                    break;
                    case 0x21:  //设置id
                    break;
                    case 0x22:  //设置kp
                    break;
                    case 0x23:  //设置ki
                    break;
                    case 0x24:  //设置kv
                    break;
                    case 0x25:  //设置kd
                    break;
                    case 0x26:  //设置转矩并保存
                    break;
                    case 0x27:  //初始角度
                    break;
                    case 0xff:  //使能/失能
                    break;        
                    default:
                        recv_ok =0;
                        break;
                    }  
                    memcpy(i2cTxData,uart_rx_buff+1,5) ;
                    uart_tx_buff[0]=CTRL_SERVO;    
                    uart_tx_buff[1]='f';    
                                    
                    if(recv_ok&&(i2c_dev.write_then_read(i2cTxData,5,i2cRxData,5,true)==true))
                    {                    
                        uart_tx_buff[1]='s';                    
                        if(uart_rx_buff[1]==0x11)
                        {                   
                            double valF = *((double*) (i2cRxData + 1));
                            dtostrf(valF,6,2,(char*)(uart_tx_buff+2));   //0.00-180.00理论上最多6位，但实际上还要加一个\0结尾。
                            trans_lens = strlen((char *)uart_tx_buff);
                        }
                        else
                        {
                            trans_lens = 2;
                        }
                    }
                    else{
                        trans_lens = 2;
                    }                
                break;
                default:
                    uart_tx_buff[0]=uart_rx_buff[0];
                    uart_tx_buff[1]='e';
                    trans_lens = 2;
                break;
            }
            Serial.write(uart_tx_buff,trans_lens);
        }
        
    }

    
}