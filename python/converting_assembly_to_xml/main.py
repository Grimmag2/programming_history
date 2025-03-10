import argparse
import sys

def main():
    finText = readInputFile("input.txt")
    fout = open("output.xml","w")
    
    printHeader(fout)
    printProgram(finText, fout)

    fout.close()




def readInputFile(inputFile):
    """
    Opens file specified by given argument.
    Splits input file to rows.
    Removes those, that include only whitespaces and/or comment.
    ---------------------------------------
    inputFile       File/pathway to a file

    return          List of rows from file

    exit(11)        File does not exists or script does not have permission
    """

    fin = open(inputFile,"r")
    finTextBase = fin.read()
    finTextUnedited = finTextBase.split("\n")
    finTextEdited = ""

    for row in finTextUnedited:
        if row.isspace():            # remove row of whitespaces
            continue
        elif row == "":              # remove empty row
            continue
        elif row.strip()[0] == "#":  # remove row with only comment
            continue
        finTextEdited += row
        finTextEdited += "\n"

    finTextEdited = finTextEdited.split("\n")
    fin.close()

    return finTextEdited



def printHeader(fout):
    """
    Writes default xml header to output file.
    ---------------------------------------
    fout       File to write to
    """
    fout.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")



def printProgram(finText, fout):
    """
    Starts converting input file in IPPcode24 to XML.
    Writes <program> to output file.
    ---------------------------------------
    finText     List of rows from input text
    fout        File to write to

    exit(21)    Input file missing ".IPPcode24" header
    """

    if finText[0].strip().lower() != ".ippcode24":
        print("21",file=sys.stderr)
        fout.close()
        exit()
    fout.write("<program language=\"IPPcode24\">\n")
    printAssembly(finText, fout)
    fout.write("</program>\n")



def printAssembly(finText, fout):
    """
    Converts rows of instructions of input file in IPPcode24 to XML.
    Writes to output file.
    ---------------------------------------
    finText     List of rows from input text
    fout        File to write to
    """

    instNum = 1             # number of converted instructions
    for row in finText[1:]:
        wordNum = 0         # number of converted words of each row
        argNum = 1          # number of converted arguments of each instruction
        instPrinted = False # <instruction> printed for a specific row

        for word in row.split():
            if "#" in word: # ends on comment
                break
            if wordNum == 0: # first word on row == name of instruction
                printInstruction(word, instNum, fout)
                instNum += 1
                instPrinted = True
            else:
                printArg(word,argNum,fout)
                argNum += 1

            wordNum += 1

        if instPrinted:
            fout.write("\t</instruction>\n")
    
    

def printInstruction(instruction, instNum, fout):
    """
    Writes beggining of <instruction> with correct order and opcode.
    ---------------------------------------
    instruction     Instruction to be converted
    instNum         Global number of instruction
    fout            File to write to

    exit(22)        Instruction does not exist
    """
    
    instList = ["MOVE", "CREATEFRAME", "PUSHFRAME", "POPFRAME",
                "DEFVAR", "CALL", "RETURN", "PUSHS", "POPS",
                "ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ",
                "AND", "OR", "NOT", "INT2CHAR", "STRI2INT",
                "READ", "WRITE", "CONCAT", "STRLEN", "GETCHAR",
                "SETCHAR", "TYPE", "LABEL", "JUMP", "JUMPIFEQ",
                "JUMPIFNEQ", "EXIT", "DPRINT", "BREAK"]
    if instruction.upper() not in instList:
        print("22",file=sys.stderr)
        fout.close()
        exit()
    fout.write("\t<instruction order=\""+ str(instNum) +"\" opcode=\""+ instruction +"\">\n")



def printArg(argument, argNum, fout):
    """
    Converts argument to XML <arg> with correct type and text
    ---------------------------------------
    argument    Argument to be converted
    argNum      Number of argument of currently converted instruction
    fout        File to write to

    exit(23)    Syntax error of input code
    """
    #TODO check syntax of argument
    #TODO determine type of argument

    argType = ""
    argumentSplit = ["",argument]

    if "@" in argument: # other than labels, all arguments must have '@'
        argumentSplit = argument.split("@", 1)
        
        varPrefix = ["GF","LF","TF"]
        otherPrefix = ["int","bool","string","nil"]
        if argumentSplit[0] in varPrefix:
            argType = "var"
            argumentSplit[1] = argumentSplit[0] + "@" + argumentSplit[1]
        elif argumentSplit[0] in otherPrefix:
            argType = argumentSplit[0]
            if argType == "bool":
                argumentSplit[1] = argumentSplit[1].lower()
            if argType == "string":
                argumentSplit[1] = convertString(argumentSplit[1])
        else:
            print("23",file=sys.stderr)
            fout.close()
            exit()
    else:
        argType = "label"

    fout.write("\t\t<arg" + str(argNum) + " type=\"" + argType + "\">")
    fout.write(argumentSplit[1])
    fout.write("</arg" + str(argNum) + ">\n")



def convertString(myString):
    """
    Converts given string in IPPcode24 to XML notation.
    ---------------------------------------
    myString    input string
    
    return      converted string
    """
    myString = myString.replace("<","&#60")
    myString = myString.replace(">","&#62")
    myString = myString.replace("&","&#38")
    myString = myString.replace("'","&#39")
    myString = myString.replace("\"","&#34")
    return myString

if __name__ == "__main__":
    main()