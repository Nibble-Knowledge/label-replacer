INF BASE_ADDRESS

loop:
	LOD KEYB
	STR CHIP_SELECT
	try: LOD READOUT
	STR STATUS_BUS
	LOD STATUS_BUS
	STR XIN1
	LOD READOUT
	STR XIN2
	LOD RET[0]
	STR XORRET[1]
	LOD RET[1]
	STR XORRET[2]
	LOD RET[2]
	STR XORRET[3]
	LOD RET[3]
	STR XORRET[4]
	LOD zero
	JMP XOR
	RETURN_1: JMP try
	
	LOD DATA_BUS
	STR CURNIB
	STR XIN1
	LOD READLOW
	STR STATUS_BUS
	LOD BREAK
	STR XIN2
	LOD RET2[0]
	STR XORRET[1]
	LOD RET2[1]
	STR XORRET[2]
	LOD RET2[2]
	STR XORRET[3]
	LOD RET2[3]
	STR XORRET[4]
	LOD zero
	STR CURRENT
	LOD zero
	JMP XOR
	RETURN_2: JMP AUDIOOUT

	LOD CURNIB
	STR XIN1
	LOD MAKES[0]
	STR XIN2
	LOD RET3[0]
	STR XORRET[1]
	LOD RET3[1]
	STR XORRET[2]
	LOD RET3[2]
	STR XORRET[3]
	LOD RET3[3]
	STR XORRET[4]
	LOD zero
	JMP XOR
	RETURN_3: JMP ONEFIRST
	LOD zero
	JMP NOTONEFIRST
	
	ONEFIRST:
		LOD KEYB
		STR CHIP_SELECT
		tryone: LOD READOUT
		STR STATUS_BUS
		LOD STATUS_BUS
		STR XIN1
		LOD READOUT
		STR XIN2
		LOD RET4[0]
		STR XORRET[1]
		LOD RET4[1]
		STR XORRET[2]
		LOD RET4[2]
		STR XORRET[3]
		LOD RET4[3]
		STR XORRET[4]
		LOD zero
		JMP XOR
		RETURN_4: JMP tryone
	
		LOD DATA_BUS
		STR XIN1
		LOD READLOW
		STR STATUS_BUS
		LOD MAKES[1]
		STR XIN2
		LOD RET5[0]
		STR XORRET[1]
		LOD RET5[1]
		STR XORRET[2]
		LOD RET5[2]
		STR XORRET[3]
		LOD RET5[3]
		STR XORRET[4]
		LOD n1
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_5: JMP AUDIOOUT
		LOD zero
		JMP loop
		
	NOTONEFIRST:
		LOD CURNIB
		STR XIN1
		LOD MAKED[0]
		STR XIN2
		LOD RET6[0]
		STR XORRET[1]
		LOD RET6[1]
		STR XORRET[2]
		LOD RET6[2]
		STR XORRET[3]
		LOD RET6[3]
		STR XORRET[4]
		LOD zero
		JMP XOR
		RETURN_6: JMP TWOFIRST
		LOD zero
		JMP NOTTWOFIRST
	
	TWOFIRST:
		LOD KEYB
		STR CHIP_SELECT
		trytwo: LOD READOUT
		STR STATUS_BUS
		LOD STATUS_BUS
		STR XIN1
		LOD READOUT
		STR XIN2
		LOD RET9[0]
		STR XORRET[1]
		LOD RET9[1]
		STR XORRET[2]
		LOD RET9[2]
		STR XORRET[3]
		LOD RET9[3]
		STR XORRET[4]
		LOD zero
		JMP XOR
		RETURN_9: JMP trytwo
		
		LOD DATA_BUS
		STR XIN1
		LOD READLOW
		STR STATUS_BUS
		LOD MAKED[1]
		STR XIN2
		LOD RET10[0]
		STR XORRET[1]
		LOD RET10[1]
		STR XORRET[2]
		LOD RET10[2]
		STR XORRET[3]
		LOD RET10[3]
		STR XORRET[4]
		LOD n2
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_10: JMP AUDIOOUT
		
		LOD MAKEE[1]
		STR XIN2
		LOD RET11[0]
		STR XORRET[1]
		LOD RET11[1]
		STR XORRET[2]
		LOD RET11[2]
		STR XORRET[3]
		LOD RET11[3]
		STR XORRET[4]
		LOD n10
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_11: JMP AUDIOOUT
		
		LOD MAKEE[1]
		STR XIN2
		LOD RET12[0]
		STR XORRET[1]
		LOD RET12[1]
		STR XORRET[2]
		LOD RET12[2]
		STR XORRET[3]
		LOD RET12[3]
		STR XORRET[4]
		LOD n10
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_12: JMP AUDIOOUT
		
		LOD MAKEF[1]
		STR XIN2
		LOD RET13[0]
		STR XORRET[1]
		LOD RET13[1]
		STR XORRET[2]
		LOD RET13[2]
		STR XORRET[3]
		LOD RET13[3]
		STR XORRET[4]
		LOD n3
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_13: JMP AUDIOOUT
		
		LOD MAKET[1]
		STR XIN2
		LOD RET14[0]
		STR XORRET[1]
		LOD RET14[1]
		STR XORRET[2]
		LOD RET14[2]
		STR XORRET[3]
		LOD RET14[3]
		STR XORRET[4]
		LOD n12
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_14: JMP AUDIOOUT
		
		LOD MAKER[1]
		STR XIN2
		LOD RET15[0]
		STR XORRET[1]
		LOD RET15[1]
		STR XORRET[2]
		LOD RET15[2]
		STR XORRET[3]
		LOD RET15[3]
		STR XORRET[4]
		LOD n11
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_15: JMP AUDIOOUT
		
		LOD zero
		JMP loop
		
	NOTTWOFIRST:
		LOD CURNIB
		STR XIN1
		LOD MAKEH[0]
		STR XIN2
		LOD RET7[0]
		STR XORRET[1]
		LOD RET7[1]
		STR XORRET[2]
		LOD RET7[2]
		STR XORRET[3]
		LOD RET7[3]
		STR XORRET[4]
		LOD zero
		JMP XOR
		RETURN_7: JMP THREEFIRST
		LOD zero
		JMP NOTTHREEFIRST
	
	THREEFIRST:
		LOD KEYB
		STR CHIP_SELECT
		trythree: LOD READOUT
		STR STATUS_BUS
		LOD STATUS_BUS
		STR XIN1
		LOD READOUT
		STR XIN2
		LOD RET16[0]
		STR XORRET[1]
		LOD RET16[1]
		STR XORRET[2]
		LOD RET16[2]
		STR XORRET[3]
		LOD RET16[3]
		STR XORRET[4]
		LOD zero
		JMP XOR
		RETURN_16: JMP trythree
		
		LOD DATA_BUS
		STR XIN1
		LOD READLOW
		STR STATUS_BUS
		LOD MAKEH[1]
		STR XIN2
		LOD RET17[0]
		STR XORRET[1]
		LOD RET17[1]
		STR XORRET[2]
		LOD RET17[2]
		STR XORRET[3]
		LOD RET17[3]
		STR XORRET[4]
		LOD n6
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_17: JMP AUDIOOUT

		LOD MAKEG[1]
		STR XIN2
		LOD RET18[0]
		STR XORRET[1]
		LOD RET18[1]
		STR XORRET[2]
		LOD RET18[2]
		STR XORRET[3]
		LOD RET18[3]
		STR XORRET[4]
		LOD n5
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_18: JMP AUDIOOUT
		
		LOD MAKEY[1]
		STR XIN2
		LOD RET19[0]
		STR XORRET[1]
		LOD RET19[1]
		STR XORRET[2]
		LOD RET19[2]
		STR XORRET[3]
		LOD RET19[3]
		STR XORRET[4]
		LOD n14
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_19: JMP AUDIOOUT
		
		LOD MAKEJ[1]
		STR XIN2
		LOD RET20[0]
		STR XORRET[1]
		LOD RET20[1]
		STR XORRET[2]
		LOD RET20[2]
		STR XORRET[3]
		LOD RET20[3]
		STR XORRET[4]
		LOD n7
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_20: JMP AUDIOOUT
		
		LOD MAKEU[1]
		STR XIN2
		LOD RET21[0]
		STR XORRET[1]
		LOD RET21[1]
		STR XORRET[2]
		LOD RET21[2]
		STR XORRET[3]
		LOD RET21[3]
		STR XORRET[4]
		LOD n14
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_21: JMP AUDIOOUT
		
		LOD zero
		JMP loop
		
	NOTTHREEFIRST:
		LOD CURNIB
		STR XIN1
		LOD MAKEK[0]
		STR XIN2
		LOD RET8[0]
		STR XORRET[1]
		LOD RET8[1]
		STR XORRET[2]
		LOD RET8[2]
		STR XORRET[3]
		LOD RET8[3]
		STR XORRET[4]
		LOD zero
		JMP XOR
		RETURN_8: JMP FOURFIRST
		LOD zero
		JMP NOTFOURFIRST
	
	FOURFIRST:	
		LOD KEYB
		STR CHIP_SELECT
		tryfour: LOD READOUT
		STR STATUS_BUS
		LOD STATUS_BUS
		STR XIN1
		LOD READOUT
		STR XIN2
		LOD RET22[0]
		STR XORRET[1]
		LOD RET22[1]
		STR XORRET[2]
		LOD RET22[2]
		STR XORRET[3]
		LOD RET22[3]
		STR XORRET[4]
		LOD zero
		JMP XOR
		RETURN_22: JMP tryfour
		
		LOD DATA_BUS
		STR XIN1
		LOD READLOW
		STR STATUS_BUS
		LOD MAKEK[1]
		STR XIN2
		LOD RET23[0]
		STR XORRET[1]
		LOD RET23[1]
		STR XORRET[2]
		LOD RET23[2]
		STR XORRET[3]
		LOD RET23[3]
		STR XORRET[4]
		LOD n8
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_23: JMP AUDIOOUT
		
		LOD MAKEI[1]
		STR XIN2
		LOD RET24[0]
		STR XORRET[1]
		LOD RET24[1]
		STR XORRET[2]
		LOD RET24[2]
		STR XORRET[3]
		LOD RET24[3]
		STR XORRET[4]
		LOD n15
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_24: JMP AUDIOOUT
		
		LOD MAKEL[1]
		STR XIN2
		LOD RET25[0]
		STR XORRET[1]
		LOD RET25[1]
		STR XORRET[2]
		LOD RET25[2]
		STR XORRET[3]
		LOD RET25[3]
		STR XORRET[4]
		LOD n9
		STR CURRENT
		LOD zero
		JMP XOR
		RETURN_25: JMP AUDIOOUT
		
		LOD zero
		JMP loop	
		
	NOTFOURFIRST:
		LOD zero
		JMP loop
		
	AUDIOOUT:
	LOD AUDIO
	STR CHIP_SELECT
	LOD WRITEOUT
	STR STATUS_BUS
	NOP
	LOD CURRENT
	STR DATA_BUS
	LOD zero
	JMP loop

LOD zero
JMP loop
XOR:
	STR XIN1
	NND XIN2
	STR XININT1
	NND XIN1
	STR XININT2
	LOD XININT1
	NND XIN2
	NND XININT2
XORRET: JMP loop

RET: .data 4 RETURN_1
RET2: .data 4 RETURN_2
RET3: .data 4 RETURN_3
RET4: .data 4 RETURN_4
RET5: .data 4 RETURN_5
RET6: .data 4 RETURN_6
RET7: .data 4 RETURN_7
RET8: .data 4 RETURN_8
RET9: .data 4 RETURN_9
RET10: .data 4 RETURN_10
RET11: .data 4 RETURN_11
RET12: .data 4 RETURN_12
RET13: .data 4 RETURN_13
RET14: .data 4 RETURN_14
RET15: .data 4 RETURN_15
RET16: .data 4 RETURN_16
RET17: .data 4 RETURN_17
RET18: .data 4 RETURN_18
RET19: .data 4 RETURN_19
RET20: .data 4 RETURN_20
RET21: .data 4 RETURN_21
RET22: .data 4 RETURN_22
RET23: .data 4 RETURN_23
RET24: .data 4 RETURN_24
RET25: .data 4 RETURN_25

zero: .data 1 0x0
n1: .data 1 0x1
n2: .data 1 0x2
n3: .data 1 0x3
n4: .data 1 0x4
n5: .data 1 0x5
n6: .data 1 0x6
n7: .data 1 0x7
n8: .data 1 0x8
n9: .data 1 0x9
n10: .data 1 0xA
n11: .data 1 0xB
n12: .data 1 0xC
n13: .data 1 0xD
n14: .data 1 0xE
n15: .data 1 0xF

XIN1: .data 1 0x0
XIN2: .data 1 0x0
XININT1: .data 1 0x0
XININT2: .data 1 0x0

KEYB: .data 1 0b0010
AUDIO: .data 1 0b0001
READOUT: .data 1 0b0101
WRITEOUT: .data 1 0b1001
READLOW: .data 1 0b0001
READY: .data 1 0b0111

CURRENT: .data 1 0x0
CURNIB: .data 1 0x0

MAKES: .data 2 0x1B

MAKED: .data 2 0x23
MAKEE: .data 2 0x24
MAKEF: .data 2 0x2B
MAKET: .data 2 0x2C
MAKER: .data 2 0x2D

MAKEH: .data 2 0x33
MAKEG: .data 2 0x34
MAKEY: .data 2 0x35
MAKEJ: .data 2 0x3B
MAKEU: .data 2 0x3C

MAKEK: .data 2 0x42
MAKEI: .data 2 0x43
MAKEL: .data 2 0x4B

BREAK: .data 1 0xF

