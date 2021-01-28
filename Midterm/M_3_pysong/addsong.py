import serial
import time

song = [
  261, 330, 392, 522, 440, 522, 392, 
  
  349, 392, 330, 261, 294, 294, 261,

  392, 392, 349, 349, 330, 392, 294,

  392, 392, 349, 349, 330, 392, 294,

  261, 330, 392, 522, 440, 522, 392, 
  
  349, 392, 330, 261, 294, 294, 261]

noteLength = [
  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2,

  1, 1, 1, 1, 1, 1, 2]


# send to K66F

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)

print("Sending signal ...")


s.write(bytes("SpringGod\n", 'UTF-8'))

for i in song:
    s.write(bytes("%d\n" % i , 'UTF-8'))
for i in noteLength:
    s.write(bytes("%d\n" % i , 'UTF-8'))
    
s.close()

print("Signal sended")