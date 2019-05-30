#!/usr/bin/python

 #/* -*-  Mode: Python; python-file-style: "gnu"; indent-tabs-mode:nil; -*- */
 #
 # Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 #
 # This program is free software; you can redistribute it and/or modify
 # it under the terms of the GNU General Public License version 2 as
 # published by the Free Software Foundation;
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program; if not, write to the Free Software
 # Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import sys


firstWrite = True

class Line:
   def __init__ (self):
       self.time = 0
       self.cellId = 0
       self.IMSI = 0
       self.RNTI = 0
       self.frame = 0
       self.sframe = 0
       self.scPrdStartFr = 0
       self.scPrdStartSf = 0
       self.resPscch = 0
       self.sizeTb = 0
       self.pscchRbLen = 0
       self.pscchStartRb = 0
       self.hopping = 0
       self.hoppingInfo = 0
       self.psschRbLen = 0
       self.psschStartRb = 0
       self.iTrp = 0
       self.mcs = 0
       self.groupDstId = 0
       self.dropped = 0
       self.pssch_fr2 = 0
       self.pssch_sf2 = 0

   def initialize (self, textLine):
       words = textLine.split("\t")
       assert (len(words) == 20)

       self.time = words[0]
       self.cellId = words[1]
       self.IMSI = words[2]
       self.RNTI = words[3]
       self.frame = words[4]
       self.sframe = words[5]
       self.scPrdStartFr = words[6]
       self.scPrdStartSf = words[7]
       self.resPscch = words[8]
       self.sizeTb = words[9]
       self.pscchRbLen = words[10]
       self.pscchStartRb = words[11]
       self.hopping = words[12]
       self.hoppingInfo = words[13]
       self.psschRbLen = words[14]
       self.psschStartRb = words[15]
       self.iTrp = words[16]
       self.mcs = words[17]
       self.groupDstId = words[18]
       self.dropped = words[19][0]


   def merge (self, otherLine):
       self.pssch_fr2 = otherLine.frame
       self.pssch_sf2 = otherLine.sframe

   def print (self, input):
       global firstWrite
       if  firstWrite == True:
           f = open(input + "/" + 'Wns32017SlCchMacStats.txt', "w")
           f.write("time\tcellId\tIMSI\tRNTI\tpscchFr1\tpscchSf1\tscPrdStartFr\tscPrdStartSf\tresPsch\tsizeTb\tpscchRbLen"
                    "\tpscchStartRb\thopping\thoppingInfo\tpsschRbLen\tpsschStartRb\tiTrp\tmcs\tl1GroupDsId\tdropped\tpscchFr2\tpscchSf2\n")

           f.write('%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' %
                   (self.time, self.cellId, self.IMSI, self.RNTI, self.frame, self.sframe,
                    self.scPrdStartFr, self.scPrdStartSf, self.resPscch, self.sizeTb,
                    self.pscchRbLen, self.pscchStartRb, self.hopping, self.hoppingInfo,
                    self.psschRbLen, self.psschStartRb, self.iTrp, self.mcs, self.groupDstId,
                    self.dropped, self.pssch_fr2, self.pssch_sf2))
           firstWrite = False
           f.close()
       else:
           f = open(input + "/" + 'Wns32017SlCchMacStats.txt', "a")
           f.write('%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' %
                   (self.time, self.cellId, self.IMSI, self.RNTI, self.frame, self.sframe,
                    self.scPrdStartFr, self.scPrdStartSf, self.resPscch, self.sizeTb,
                    self.pscchRbLen, self.pscchStartRb, self.hopping, self.hoppingInfo,
                    self.psschRbLen, self.psschStartRb, self.iTrp, self.mcs, self.groupDstId,
                    self.dropped, self.pssch_fr2, self.pssch_sf2))
           f.close()

def cantor(x1, x2):
   return (((x1 + x2) * (x1 + x2 + 1)) / 2) + x2;

def main(input):
    f = open(input + "/SlCchMacStats.txt", "r")
    lines = f.readlines()
    storage = { }
    for textLine in lines:
        if textLine[0] == "%":
            continue

        l = Line()
        l.initialize(textLine)
        key = cantor(int(l.IMSI), int(l.scPrdStartFr))
        if key in storage:
            storage[key][0].merge(l)
            storage[key][1] += 1
            assert (storage[key][1] <= 2)
        else:
            storage[key] = [0] * 2
            storage[key][0] = l
            storage[key][1] = 1

#    for key, value in storage.items():
#        if (value[1] == 1):
#            print ("EHI THIS LINE WAS NOT MERGED")
#        value[0].print() 

    for key, value in sorted(storage.items()):
        value[0].print(input)

if __name__== "__main__":
 if len(sys.argv) > 1:
     main(sys.argv[1])
 else:
    print("Error: path to the SlCchMacStats.txt file not provided")
    exit(1)
