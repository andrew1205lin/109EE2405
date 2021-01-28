# Part. 1
#=======================================
# Import module
#  csv -- fileIO operation
import csv
#=======================================

# Part. 2
#=======================================
# Read cwb weather data
cwb_filename = '107061127.csv'
data = []
header = []
with open(cwb_filename) as csvfile:
   mycsv = csv.DictReader(csvfile)
   header = mycsv.fieldnames
   for row in mycsv:
      data.append(row)
#=======================================

# Part. 3
#=======================================
# Analyze data depend on your group and store it to target_data like:
# Retrive all data points which station id is "C0X260" as a list.
# target_data = list(filter(lambda item: item['station_id']  == 'C0X260', data))


valid_data = list(filter(
    lambda item:
        (item['TEMP'] != '-99.000') & ((item['station_id']  =='C0A880') | (item['station_id']  =='C0F9A0' ) |(item['station_id']  =='C0G640') |(item['station_id']  =='C0R190')| (item['station_id']  =='C0X260'))
    , data))



station = []           #創建一個新的list記錄有哪些station的資料
temp = []
for i in valid_data:
    if(i['station_id'] not in station):    #如果station內尚未紀錄此測站，則記錄他並在temp留下他的溫度
        station.append(i['station_id']),
        temp.append(i['TEMP'])            
    elif(i['TEMP']>=temp[station.index(i['station_id'])]):        #如果已經有此測站，則比較temp中的值，並留下較高者
        temp[station.index(i['station_id'])]=i['TEMP']
   
for i in ['C0G640', 'C0A880', 'C0X260', 'C0R190', 'C0F9A0']:
    if(i not in station):        #如果測站沒有資料則溫度=None
        station.append(i),
        temp.append("None")
   
   
   
   
target_data=[]
for i,j in zip(station,temp):    #兩個變數在跑，用zip打包
    target_data.append([i,j])

def takeSecond(elem):
    return elem[0]

target_data.sort(key=takeSecond) #進入target_data的每個element，以elem[0](測站名)為標準做排序。
#=======================================

# Part. 4
#=======================================
# Print result
print(target_data)


#========================================