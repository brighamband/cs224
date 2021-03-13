#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

const int MAX_MEM_SIZE = (1 << 13);

void fetchStage(int *icode, int *ifun, int *rA, int *rB, wordType *valC, wordType *valP) {
    wordType pc = getPC();
    byteType byte = getByteFromMemory(pc);
    
    *icode = (byte >> 4) & 0xf;
    *ifun = byte & 0xf;

    if (*icode == HALT) {
        *valP = pc + 1;
    }

    if (*icode == NOP) {
        *valP = pc + 1;
    }

    if (*icode == IRMOVQ || *icode == RMMOVQ) {
        byte = getByteFromMemory(pc + 1);
        *rA = (byte >> 4) & 0xf;
        *rB = byte & 0xf;
        *valC = getWordFromMemory(pc + 2);
        *valP = pc + 10;
    }

    if (*icode == RRMOVQ || *icode == OPQ || *icode == PUSHQ || *icode == POPQ) {
        byte = getByteFromMemory(pc + 1);
        *rA = (byte >> 4) & 0xf;
        *rB = byte & 0xf;
        *valP = pc + 2;
    }

    if (*icode == JXX || *icode == CALL) {
        *valC = getWordFromMemory(pc + 1);
        *valP = pc + 9;
    }
}

void decodeStage(int icode, int rA, int rB, wordType *valA, wordType *valB) {
    if (icode == RRMOVQ) {
        *valA = getRegister(rA);
    }

    if (icode == RMMOVQ || icode == OPQ) {
        *valA = getRegister(rA);
        *valB = getRegister(rB);
    }

    if (icode == CALL) {
        *valB = getRegister(RSP);
    }

    if (icode == RET || icode == POPQ) {
        *valA = getRegister(RSP);
        *valB = getRegister(RSP);
    }

    if (icode == PUSHQ) {
        *valA = getRegister(rA);
        *valB = getRegister(RSP);
    }
}

void executeStage(int icode, int ifun, wordType valA, wordType valB, wordType valC, wordType *valE, bool *Cnd) {
    if (icode == IRMOVQ) {
        *valE = 0 + valC;
    }

    if (icode == RRMOVQ) {
        *valE = 0 + valA;
    }

    if (icode == RMMOVQ) {
        *valE = valB + valC;
    }

    if (icode == OPQ) {
		bool sf = FALSE;
		bool zf = FALSE;
		bool of = FALSE;

        // Perform operation
        if (ifun == ADD) {
            *valE = valB + valA;

            if ((valA > 0 && valB > 0 && *valE < 0) || (valA < 0 && valB < 0 && *valE > 0)) {	// Update overflow flag
                of = TRUE;
            }
        }

        if (ifun == SUB) {
            *valE = valB - valA;

            if ((valA > 0 && valB < 0 && *valE < 0) || (valA < 0 && valB > 0 && *valE > 0)) {	// Update overflow flag
                of = TRUE;
            }
        }

        if (ifun == AND) {
            *valE = valB & valA;
        }

        if (ifun == XOR) {
            *valE = valB ^ valA;
        }
            
        // Update sign flag
        if (*valE < 0) {
            sf = TRUE;
        }

        // Update zero flag
        if (*valE == 0) {
            zf = TRUE;
        }
        
        setFlags(sf, zf, of);
    }

    if (icode == JXX) {
        *Cnd = Cond(ifun);
    }

    if (icode == CALL) {
        *valE = valB - 8;
    }

    if (icode == RET || icode == POPQ) {
        *valE = valB + 8;
    }

    if (icode == PUSHQ) {
        *valE = valB - 8;
    }
}

void memoryStage(int icode, wordType valA, wordType valP, wordType valE, wordType *valM) {
    if (icode == RMMOVQ || icode == PUSHQ) {
        setWordInMemory(valE, valA);
    }

    if (icode == CALL) {
        setWordInMemory(valE, valP);
    }

    if (icode == RET || icode == POPQ) {
        *valM = getWordFromMemory(valA);
    }
}

void writebackStage(int icode, wordType rA, wordType rB, wordType valE, wordType valM) {
    if (icode == IRMOVQ || icode == RRMOVQ || icode == OPQ) {
        setRegister(rB, valE);
    }

    if (icode == CALL || icode == RET || icode == PUSHQ) {
        setRegister(RSP, valE);
    }

    if (icode == POPQ) {
        setRegister(RSP, valE);
        setRegister(rA, valM);
    }
}

void pcUpdateStage(int icode, wordType valC, wordType valP, bool Cnd, wordType valM) {
    if (icode == HALT) {
        setPC(valP);
        setStatus(STAT_HLT);
    }
    
    if (icode == NOP || icode == IRMOVQ || icode == RRMOVQ || icode == RMMOVQ || icode == OPQ || icode == PUSHQ || icode == POPQ) {
        setPC(valP);
    }

    if (icode == JXX) {
        if (Cnd) {
            setPC(valC);
        } else {
            setPC(valP);
        }
    }

    if (icode == CALL) {
        setPC(valC);
    }

    if (icode == RET) {
        setPC(valM);
    }
}

void stepMachine(int stepMode) {
    /* FETCH STAGE */
    int icode = 0, ifun = 0;
    int rA = 0, rB = 0;
    wordType valC = 0;
    wordType valP = 0;
	
    /* DECODE STAGE */
    wordType valA = 0;
    wordType valB = 0;

    /* EXECUTE STAGE */
    wordType valE = 0;
    bool Cnd = 0;

    /* MEMORY STAGE */
    wordType valM = 0;

    fetchStage(&icode, &ifun, &rA, &rB, &valC, &valP);
    applyStageStepMode(stepMode, "Fetch", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

    decodeStage(icode, rA, rB, &valA, &valB);
    applyStageStepMode(stepMode, "Decode", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

    executeStage(icode, ifun, valA, valB, valC, &valE, &Cnd);
    applyStageStepMode(stepMode, "Execute", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

    memoryStage(icode, valA, valP, valE, &valM);
    applyStageStepMode(stepMode, "Memory", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

    writebackStage(icode, rA, rB, valE, valM);
    applyStageStepMode(stepMode, "Writeback", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

    pcUpdateStage(icode, valC, valP, Cnd, valM);
    applyStageStepMode(stepMode, "PC update", icode, ifun, rA, rB, valC, valP, valA, valB, valE, Cnd, valM);

    incrementCycleCounter();
}

/** 
 * main
 * */
int main(int argc, char **argv) {
    int stepMode = 0;
    FILE *input = parseCommandLine(argc, argv, &stepMode);

    initializeMemory(MAX_MEM_SIZE);
    initializeRegisters();
    loadMemory(input);

    applyStepMode(stepMode);
    while (getStatus() != STAT_HLT) {
        stepMachine(stepMode);
        applyStepMode(stepMode);
#ifdef DEBUG
        printMachineState();
        printf("\n");
#endif
    }
    printMachineState();
    return 0;
}
