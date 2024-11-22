#!/usr/bin/python3

import serial, string, time, sys, select
import tkinter as tk 
import datetime

root = tk.Tk()
root.title("HFS3001 SHED HVAC Logger")
root.geometry("800x300")
root.configure(bg="black")

daFontHeader = ("monospace", 20, "bold") 

dateTimeStampVar=tk.StringVar()
dateTimeStampVar.set("This is supposed to be the date time...")
dateTimeLabel=tk.Label(root, textvariable=dateTimeStampVar,fg='#0f8',bg='#000')
dateTimeLabel.pack()
dateTimeLabel.configure(font = daFontHeader) 

def _check_stdin():
	return select.select([sys.stdin], [], [], 0)[0]

def win_check_stdin():
	return msvcrt.getch()

try:
	import msvcrt
	check_stdin = win_check_stdin
except ImportError:
	check_stdin = _check_stdin


ser = False
LOG = False
CSV = False
CSV_Headers = False
if len(sys.argv) < 2:
	#stdin is a terminal, not likely to be a pipe..
	try:
		ser = serial.Serial('/dev/ttyACM0', 9600, 8, 'N', 1, timeout=1)
	except:
		ser = serial.Serial('/dev/ttyACM1', 9600, 8, 'N', 1, timeout=1)
	filename = f"shed_heater-{datetime.datetime.now().strftime('%Y-%m-%d--%H-%M-%S')}"
	LOG = open(f"shed.log","at",encoding="Latin1")
	CSV = open(f"shed.csv","at",encoding="Latin1")

else :
	ser = sys.stdin

csv_lines = []

try:
	with open("shed.csv","r") as FILE:
		csv_lines = FILE.readlines()[-24*60:]
		if csv_lines[0].startswith("time"): csv_lines.pop(0)
except Exception as e:
	print("oops opening csv to read...")
	print(e)


def add_csv_line(csv_line):
	global csv_lines
	global CSV_Headers
	csv_lines.append(csv_line) #remove that first bit...
	if len(csv_lines) >= 24*60: #keep only last 24 hours ...
		csv_lines.pop(0) # Keep it Simple Stupid!

	with open("shed24.csv","wt",encoding="Latin-1") as FILE:
		FILE.write(CSV_Headers) 
		for line in csv_lines:
			FILE.write(line)

	with open("shed12.csv","wt",encoding="Latin-1") as FILE:
		FILE.write(CSV_Headers)
		for line in csv_lines[-12*60:]:
			FILE.write(line)

	with open("shed6.csv","wt",encoding="Latin-1") as FILE:
		FILE.write(CSV_Headers)
		for line in csv_lines[-6*60:]:
			FILE.write(line)

	with open("shed3.csv","wt",encoding="Latin-1") as FILE:
		FILE.write(CSV_Headers)
		for line in csv_lines[-3*60:]:
			FILE.write(line)



counterLabel=tk.Label(root, text="...",fg='#0f8',bg='#000',font=daFontHeader)
#counterLabel.pack(side="top")

watts_label = tk.Label(root,text="Watt you looking at?",fg='#0f8',bg='#000',font=daFontHeader)
watts_label.pack(side="top")
light_label = tk.Label(root,text="How enlightening!",fg='#0f8',bg='#000',font=daFontHeader)
light_label.pack(side="top")
master_frame = tk.Frame(root)
master_frame.configure(bg="black")
master_frame.pack()
left_columns = tk.Frame(master_frame)
left_columns.configure(bg="black")
left_columns.pack(side="left")

l_frame_left = tk.Frame(left_columns)
l_frame_left.configure(bg="black")
l_frame_left.pack(side="left")
l_frame_right = tk.Frame(left_columns)
l_frame_right.configure(bg="black")
l_frame_right.pack(side="left")

right_columns = tk.Frame(master_frame)
right_columns.configure(bg="black")
right_columns.pack(side="right")

r_frame_left = tk.Frame(right_columns)
r_frame_left.configure(bg="black")
r_frame_left.pack(side="left")
r_frame_right = tk.Frame(right_columns)
r_frame_right.configure(bg="black")
r_frame_right.pack(side="left")


loop_counter = 100
main_counter = 0 

temps = { 
	"SHED" : "222",
	"SANDA": "222",
	"SANDB":"222",
	"RESERVOIR":"222",
	"ROOF":"222",
	"AMBIENT":"222",
	"FOOTWARM":"222",
	"HEATERA":"222",
	"HEATERB":"222",
	"FLOOR":"222"
}

blahBitmap = tk.BitmapImage(data="""
#define im_width 8
#define im_height 12
static char im_bits[] = {
0x10,
0x10, 
0x10, 
0x10, 
0x10, 
0x10, 
0x10, 
0x10, 
0x10, 
0x10, 
0x10, 
0x10, 
};
""",foreground="#0f8", background="black")

#need to make this bigger, like 3x ... probably create something for scaling ?
bitmap_font = {
	'2': tk.BitmapImage(data="""
#define im_width 8
#define im_height 12
static char im_bits[] = {
0x00, 
0x3C, 
0x58, 
0x60, 
0x60, 
0x58, 
0x58, 
0x06, 
0x06,
0x1A, 
0x3C, 
0x00, 
};
""",foreground="#0f8", background="black"), #OK but why is this like backward? lol...

	'3': tk.BitmapImage(data="""
#define im_width 8
#define im_height 12
static char im_bits[] = {
0x00, 
0x3C, 
0x58, 
0x60, 
0x60, 
0x58, 
0x58, 
0x60, 
0x60,
0x58, 
0x3C, 
0x00, 
};
""",foreground="#0f8", background="black"),

	'5': tk.BitmapImage(data="""
#define im_width 8
#define im_height 12
static char im_bits[] = {
0x00, 
0x3C, 
0x1A, 
0x06, 
0x06, 
0x1A, 
0x58, 
0x60, 
0x60,
0x58, 
0x3C, 
0x00, 
};
""",foreground="#0f8", background="black"),
	
}


lines = [ ]
for lineNum in range(5):
	lineFrame=tk.Frame(l_frame_right)
	lines.append([tk.Label(l_frame_left, text=f"{lineNum}",fg='#0f8',bg='#000',font=daFontHeader),
		lineFrame,
		tk.Label(lineFrame, text=f"{lineNum}",fg='#0f8',bg='#000',font=daFontHeader),
		tk.Label(lineFrame,image=blahBitmap,fg='#0f8',bg='#000'),
		tk.Label(lineFrame,image=blahBitmap,fg='#0f8',bg='#000'),
		tk.Label(lineFrame,image=bitmap_font['2'],fg='#0f8',bg='#000')])
	lines[-1][0].pack(anchor="e")
	lines[-1][1].pack(anchor="e")
	lines[-1][2].pack(side="left")
	lines[-1][3].pack(side="left")
	lines[-1][4].pack(side="left")
	lines[-1][5].pack(side="left")

for lineNum in range(5,10,1):
	lineFrame=tk.Frame(r_frame_right)
	lines.append([tk.Label(r_frame_left, text=f"{lineNum}",fg='#0f8',bg='#000',font=daFontHeader),
		lineFrame,
		tk.Label(lineFrame, text=f"{lineNum}",fg='#0f8',bg='#000',font=daFontHeader),
		tk.Label(lineFrame,image=blahBitmap,fg='#0f8',bg='#000'),
		tk.Label(lineFrame,image=blahBitmap,fg='#0f8',bg='#000'),
		tk.Label(lineFrame,image=bitmap_font['2'],fg='#0f8',bg='#000')])
	lines[-1][0].pack(anchor="e")
	lines[-1][1].pack(anchor="e")
	lines[-1][2].pack(side="left")
	lines[-1][3].pack(side="left")
	lines[-1][4].pack(side="left")
	lines[-1][5].pack(side="left")

do_temps = False 

def logloop():
	global loop_counter;
	global main_counter
	global temps;
	global lines;
	global ser;
	global LOG; 
	global do_temps;
	global CSV;
	global CSV_Headers;
	loop_counter -= 1 ;

	if ser :
			output = False
			if LOG:
				output = ser.readline().decode("Latin-1")
			elif check_stdin():
				output = ser.readline()
			if output:
				print(output,end="")
				if LOG: LOG.write(output)

				if do_temps:
					if "DONE CHECK ALL DHT" in output or "evaluateRule" in output or "done rules..." in output:
						do_temps = False
					else :
						splits = output.split(' ')
						try:
							temps[splits[0]]=splits[2]
						except:
							do_temps = False
				elif output.startswith("CHECK ALL DHT"):
					do_temps = True
				elif "last_min_watts:" in output:
					watts_label.configure(text=" ".join(output[:-1].split(" ")[-5:]))
				elif "Light Level:" in output:
					light_label.configure(text=output[:-1])
				elif CSV:
					if not CSV_Headers:
						if output.startswith("CSV_HEADERS"):
							CSV_Headers = output[12:] # CSV_HEADERS,time,"SHED",
							if len(csv_lines) == 0:
								CSV.write(CSV_Headers)
					elif output.startswith("CSV_VALUES"):
						add_csv_line(output[12:])
						CSV.write(output[12:])
						with open("/var/www/html/shed_current.csv.txt","wt",encoding="Latin-1") as FILE:
							FILE.write(CSV_Headers)
							FILE.write(output[12:])
			#else: print("no output this loop")


	if loop_counter == 0 :
		loop_counter=10
		dateTimeStampVar.set(datetime.datetime.now().strftime("%Y/%m/%d %H:%M:%S"))
		lineNum=0
		for (k,v) in temps.items():
			print(f"window - setting {lineNum} to {k} {v}")
			lines[lineNum][0].configure(text=f"{k} ")
			lines[lineNum][2].configure(text=f"{v} °F")
			lineNum +=1 
	root.after(1, logloop)



root.after(1, logloop)
root.mainloop()