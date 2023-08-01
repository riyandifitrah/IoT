import time
import pytz
from datetime import datetime
import cv2
import os
import time
import serial 
import mysql.connector
import cv2
import os
import time

video = cv2.VideoCapture(0);
video.set(cv2.CAP_PROP_FRAME_WIDTH, 640)   # 3
video.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)  # 4

PATH_MODEL=r"C:\Users\riyan\Documents\Qt Design\Haarcascade and Tensor\model\model.xml"
face_cascade = cv2.CascadeClassifier(r"C:\Users\riyan\Documents\Qt Design\Haarcascade and Tensor\haarcascade_frontalface_alt.xml")
faceRecognition= cv2.face.LBPHFaceRecognizer_create()
faceRecognition.read(PATH_MODEL)
id=0
label=['Sigit','Mahasiswa','Tidak Dikenal']
minWidth=0.1
minHeight=0.1

#establish connection to MySQL. You'll have to change this for your database.
dbConn=mysql.connector.connect(host="localhost",user="root",passwd="",database="rfid")
if(dbConn.connect):
    print("berhasil konek database",dbConn)
else:
    print("error")
#open a cursor to the database
cursor = dbConn.cursor()
# device = serial.Serial('COM4')
IST = pytz.timezone('Asia/Jakarta')
date_str=datetime.now(IST)
time_str=date_str.strftime('%H-,-%M=-,-%S')
date=date_str.strftime('%Y,%m,%d')
# print(time_str)
# print(date)

try: 
  arduino = serial.Serial('COM10', 9600) 
  print("mencoba..", arduino)

  try:
          data=arduino.readline()
          print(data)
          pieces=data.split(b"\n")
  except:
          print("respon lambat")
except: 
  print("gagal terhubung USB atau USB salah port") 
while True:
    ret, frame = video.read()
    gray_img = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    wajah = face_cascade.detectMultiScale(gray_img,scaleFactor = 1.2,minNeighbors = 5, minSize=(round(minHeight),round(minHeight)))
    
    for (x,y, w, h) in wajah:
        font = cv2.FONT_HERSHEY_SIMPLEX
        id, confidence=faceRecognition.predict(gray_img[y:y+h,x:x+w])#confidence 0 artinya cocok atau sempurna
        print(confidence)
        
                # if confidence>=20:
                #     nameID=label[0]
                #     confidenceTxt="{0}%".format(round(100-confidence))
                # # elif confidence<=40:
                # #     nameID=label[1]
                # #     confidenceTxt="{0}%".format(round(100-confidence))
                # else:
                #     nameID=label[1]
                #     confidenceTxt="{0}%".format(round(100-confidence))
        if confidence>=80:
          nameID=label[0]
          confidenceTxt="{0}%".format(round(confidence))

          cv2.putText(frame,str(nameID),(x+5,y-5),font,1,(0,0,255),2)
          cv2.putText(frame,str(confidenceTxt),(x+y,y-5),font,1,(0,0,255),2)
                
                # cv2.putText(frame,'Wajah',(x,y-4), font, 1.5, (0,0,255), 2, cv2.LINE_AA)
          cv2.rectangle(frame,(x,y),          # posisi kotak
                    (x+w, y+h),     # posisi kotak
                    (0, 255, 0),    # warna kotak RGB
                    3               # lebar garis kotak
                )
          if confidence>=90:
                print("tempel kartu lagi")
                time.sleep(1)
                try:
                    data=arduino.readline()
                    print(data)
                    pieces=data.split(b",")
                    try:
                        import time
                        import datetime
                        ts = time.time()
                        timestamp = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
                        # cv2.putText(frame, 'Muka Terdeteksi', (50,50), cv2.FONT_HERSHEY_SIMPLEX, 
                        # 1, (255,0,0), 2, cv2.LINE_AA)
                        cursor=dbConn.cursor()
                        cursor.execute("INSERT INTO tbabsen (id,nokartu,nama,time) VALUES (NULL,%s,%s,%s)", (pieces[0],label[0],timestamp))
                        dbConn.commit()
                        print("berhasil masukan data")
                        cursor.close()
                        for i in range(1):
  # printing numbers
                            print(i)
  # adding 2 seconds time delay
                            time.sleep(2)
                        
                    except:
                        print ("failed to insert data")
                    finally:
                        cursor.close()
                except:
                  print ("Processing")
        elif confidence>=70:
            nameID=label[1]
            confidenceTxt="{0}%".format(round(confidence))
        else:
            nameID=label[2]
            confidenceTxt="{0}%".format(round(confidence))
          # Executing the SQL command
          # Commit your changes in the database
    cv2.imshow("OpenCV",frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    # time.sleep(1)
    
    # try:
    #   if(confidence>=80):
    
    # except:
    #     print("gagal")
dbConn.close()
video.release()
cv2.destroyAllWindows()