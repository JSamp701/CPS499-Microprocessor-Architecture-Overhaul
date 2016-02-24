import os
import sys

sourcefile = open(sys.argv[1], "r")

outfile = open("out-" + sys.argv[1], "w")

start = True

section = []

for line in sourcefile:
	if start:
		section.append(line)
		start = False
	elif line[0] != "\t":
		#output all the html
		date = section[1][1:-1:]
		title = section[0][:-1:]
		outfile.write('<tr><td class="auto-style1">' + date + '</td>')
		outfile.write('<td><ul><li>' + title + '<ul>')
		for strng in section[2::]:
			outfile.write("<li>" + strng[1::] + "</li>")
		outfile.write('</ul></li></ul></td></tr>')
		#remake section
		section = []
		section.append(line)
	else: #not the start of a new section, so append the line to it's section
		section.append(line)

#output the last of the html
outfile.write('<tr><td class="auto-style1">TBD</td>')
outfile.write('<td><ul><li>' + section[0][:-1:] + '<ul>')
for strng in section[1::]:
	outfile.write("<li>" + strng + "</li>")
outfile.write('</ul></li></ul></td></tr>')


outfile.close()