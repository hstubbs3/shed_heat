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
	LOG = open(f"{filename}.log","wt",encoding="Latin1")
	CSV = open(f"{filename}.csv","wt",encoding="Latin1")

else :
	ser = sys.stdin


counterLabel=tk.Label(root, text="...",fg='#0f8',bg='#000',font=daFontHeader)
#counterLabel.pack(side="top")

master_frame = tk.Frame(root)
master_frame.configure(bg="black")
master_frame.pack()
frame_left = tk.Frame(master_frame)
frame_left.configure(bg="black")
frame_left.pack(side="left")
frame_right = tk.Frame(master_frame)
frame_right.configure(bg="black")
frame_right.pack(side="left")

loop_counter = 100
main_counter = 0 

temps = { 
	"SHED" : "222",
	"HEATSINK": "222",
	"SAND":"222",
	"RESERVOIR":"222",
	"ROOF":"222",
	"AMBIENT":"222",
	"FOOTER":"222"
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
for lineNum in range(7):
	lineFrame=tk.Frame(frame_right)
	lines.append([tk.Label(frame_left, text=f"{lineNum}",fg='#0f8',bg='#000',font=daFontHeader),
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
				elif CSV:
					if not CSV_Headers:
						if output.startswith("CSV_HEADERS"):
							CSV_Headers = output
							CSV.write(output)
					elif output.startswith("CSV_VALUES"):
						CSV.write(output)
						with open("/var/www/html/shed_current.csv.txt","wt",encoding="Latin-1") as FILE:
							FILE.write(CSV_Headers)
							FILE.write(output)
			#else: print("no output this loop")


	if loop_counter == 0 :
		main_counter +=1
		if main_counter > 99: main_counter=0
		loop_counter=10
		counterLabel.configure(text=f'{main_counter}')
		dateTimeStampVar.set(datetime.datetime.now().strftime("%Y/%m/%d %H:%M:%S"))
		lineNum=0
		for (k,v) in temps.items():
			lines[lineNum][0].configure(text=f"{k} ")
			lines[lineNum][2].configure(text=f"{v} °F")
			lineNum +=1 
	root.after(1, logloop)



root.after(1, logloop)
root.mainloop()