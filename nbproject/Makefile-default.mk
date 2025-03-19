#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ESD.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/ESD.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=config_memory.c configuration_bits.c crc.c fonts.c i2c2.c interrupts.c keypad.c lcd.c ltc2943.c main.c queue.c tmr2.c traps.c uart1_queued.c uc1701x.c coords.c mc24aa512_i2c2.c geofence.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/config_memory.o ${OBJECTDIR}/configuration_bits.o ${OBJECTDIR}/crc.o ${OBJECTDIR}/fonts.o ${OBJECTDIR}/i2c2.o ${OBJECTDIR}/interrupts.o ${OBJECTDIR}/keypad.o ${OBJECTDIR}/lcd.o ${OBJECTDIR}/ltc2943.o ${OBJECTDIR}/main.o ${OBJECTDIR}/queue.o ${OBJECTDIR}/tmr2.o ${OBJECTDIR}/traps.o ${OBJECTDIR}/uart1_queued.o ${OBJECTDIR}/uc1701x.o ${OBJECTDIR}/coords.o ${OBJECTDIR}/mc24aa512_i2c2.o ${OBJECTDIR}/geofence.o
POSSIBLE_DEPFILES=${OBJECTDIR}/config_memory.o.d ${OBJECTDIR}/configuration_bits.o.d ${OBJECTDIR}/crc.o.d ${OBJECTDIR}/fonts.o.d ${OBJECTDIR}/i2c2.o.d ${OBJECTDIR}/interrupts.o.d ${OBJECTDIR}/keypad.o.d ${OBJECTDIR}/lcd.o.d ${OBJECTDIR}/ltc2943.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/queue.o.d ${OBJECTDIR}/tmr2.o.d ${OBJECTDIR}/traps.o.d ${OBJECTDIR}/uart1_queued.o.d ${OBJECTDIR}/uc1701x.o.d ${OBJECTDIR}/coords.o.d ${OBJECTDIR}/mc24aa512_i2c2.o.d ${OBJECTDIR}/geofence.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/config_memory.o ${OBJECTDIR}/configuration_bits.o ${OBJECTDIR}/crc.o ${OBJECTDIR}/fonts.o ${OBJECTDIR}/i2c2.o ${OBJECTDIR}/interrupts.o ${OBJECTDIR}/keypad.o ${OBJECTDIR}/lcd.o ${OBJECTDIR}/ltc2943.o ${OBJECTDIR}/main.o ${OBJECTDIR}/queue.o ${OBJECTDIR}/tmr2.o ${OBJECTDIR}/traps.o ${OBJECTDIR}/uart1_queued.o ${OBJECTDIR}/uc1701x.o ${OBJECTDIR}/coords.o ${OBJECTDIR}/mc24aa512_i2c2.o ${OBJECTDIR}/geofence.o

# Source Files
SOURCEFILES=config_memory.c configuration_bits.c crc.c fonts.c i2c2.c interrupts.c keypad.c lcd.c ltc2943.c main.c queue.c tmr2.c traps.c uart1_queued.c uc1701x.c coords.c mc24aa512_i2c2.c geofence.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/ESD.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ128GA202
MP_LINKER_FILE_OPTION=,--script=p24FJ128GA202.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/config_memory.o: config_memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/config_memory.o.d 
	@${RM} ${OBJECTDIR}/config_memory.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  config_memory.c  -o ${OBJECTDIR}/config_memory.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/config_memory.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/config_memory.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/configuration_bits.o: configuration_bits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/configuration_bits.o.d 
	@${RM} ${OBJECTDIR}/configuration_bits.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  configuration_bits.c  -o ${OBJECTDIR}/configuration_bits.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/configuration_bits.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/configuration_bits.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/crc.o: crc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/crc.o.d 
	@${RM} ${OBJECTDIR}/crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  crc.c  -o ${OBJECTDIR}/crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/crc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/crc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/fonts.o: fonts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/fonts.o.d 
	@${RM} ${OBJECTDIR}/fonts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  fonts.c  -o ${OBJECTDIR}/fonts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/fonts.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/fonts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/i2c2.o: i2c2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/i2c2.o.d 
	@${RM} ${OBJECTDIR}/i2c2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  i2c2.c  -o ${OBJECTDIR}/i2c2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/i2c2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/i2c2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/interrupts.o: interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interrupts.o.d 
	@${RM} ${OBJECTDIR}/interrupts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupts.c  -o ${OBJECTDIR}/interrupts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interrupts.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/interrupts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/keypad.o: keypad.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/keypad.o.d 
	@${RM} ${OBJECTDIR}/keypad.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  keypad.c  -o ${OBJECTDIR}/keypad.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/keypad.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/keypad.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/lcd.o: lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/lcd.o.d 
	@${RM} ${OBJECTDIR}/lcd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  lcd.c  -o ${OBJECTDIR}/lcd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/lcd.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/lcd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/ltc2943.o: ltc2943.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ltc2943.o.d 
	@${RM} ${OBJECTDIR}/ltc2943.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ltc2943.c  -o ${OBJECTDIR}/ltc2943.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ltc2943.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/ltc2943.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/queue.o: queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/queue.o.d 
	@${RM} ${OBJECTDIR}/queue.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  queue.c  -o ${OBJECTDIR}/queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/queue.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/queue.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/tmr2.o: tmr2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/tmr2.o.d 
	@${RM} ${OBJECTDIR}/tmr2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  tmr2.c  -o ${OBJECTDIR}/tmr2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/tmr2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/tmr2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/traps.o: traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/traps.o.d 
	@${RM} ${OBJECTDIR}/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  traps.c  -o ${OBJECTDIR}/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/traps.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/uart1_queued.o: uart1_queued.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart1_queued.o.d 
	@${RM} ${OBJECTDIR}/uart1_queued.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uart1_queued.c  -o ${OBJECTDIR}/uart1_queued.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/uart1_queued.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/uart1_queued.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/uc1701x.o: uc1701x.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uc1701x.o.d 
	@${RM} ${OBJECTDIR}/uc1701x.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uc1701x.c  -o ${OBJECTDIR}/uc1701x.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/uc1701x.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/uc1701x.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/coords.o: coords.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/coords.o.d 
	@${RM} ${OBJECTDIR}/coords.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  coords.c  -o ${OBJECTDIR}/coords.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/coords.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/coords.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mc24aa512_i2c2.o: mc24aa512_i2c2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/mc24aa512_i2c2.o.d 
	@${RM} ${OBJECTDIR}/mc24aa512_i2c2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mc24aa512_i2c2.c  -o ${OBJECTDIR}/mc24aa512_i2c2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mc24aa512_i2c2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/mc24aa512_i2c2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/geofence.o: geofence.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/geofence.o.d 
	@${RM} ${OBJECTDIR}/geofence.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  geofence.c  -o ${OBJECTDIR}/geofence.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/geofence.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/geofence.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/config_memory.o: config_memory.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/config_memory.o.d 
	@${RM} ${OBJECTDIR}/config_memory.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  config_memory.c  -o ${OBJECTDIR}/config_memory.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/config_memory.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/config_memory.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/configuration_bits.o: configuration_bits.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/configuration_bits.o.d 
	@${RM} ${OBJECTDIR}/configuration_bits.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  configuration_bits.c  -o ${OBJECTDIR}/configuration_bits.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/configuration_bits.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/configuration_bits.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/crc.o: crc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/crc.o.d 
	@${RM} ${OBJECTDIR}/crc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  crc.c  -o ${OBJECTDIR}/crc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/crc.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/crc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/fonts.o: fonts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/fonts.o.d 
	@${RM} ${OBJECTDIR}/fonts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  fonts.c  -o ${OBJECTDIR}/fonts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/fonts.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/fonts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/i2c2.o: i2c2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/i2c2.o.d 
	@${RM} ${OBJECTDIR}/i2c2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  i2c2.c  -o ${OBJECTDIR}/i2c2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/i2c2.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/i2c2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/interrupts.o: interrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/interrupts.o.d 
	@${RM} ${OBJECTDIR}/interrupts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  interrupts.c  -o ${OBJECTDIR}/interrupts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/interrupts.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/interrupts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/keypad.o: keypad.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/keypad.o.d 
	@${RM} ${OBJECTDIR}/keypad.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  keypad.c  -o ${OBJECTDIR}/keypad.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/keypad.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/keypad.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/lcd.o: lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/lcd.o.d 
	@${RM} ${OBJECTDIR}/lcd.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  lcd.c  -o ${OBJECTDIR}/lcd.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/lcd.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/lcd.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/ltc2943.o: ltc2943.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ltc2943.o.d 
	@${RM} ${OBJECTDIR}/ltc2943.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ltc2943.c  -o ${OBJECTDIR}/ltc2943.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ltc2943.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/ltc2943.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  main.c  -o ${OBJECTDIR}/main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/queue.o: queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/queue.o.d 
	@${RM} ${OBJECTDIR}/queue.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  queue.c  -o ${OBJECTDIR}/queue.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/queue.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/queue.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/tmr2.o: tmr2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/tmr2.o.d 
	@${RM} ${OBJECTDIR}/tmr2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  tmr2.c  -o ${OBJECTDIR}/tmr2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/tmr2.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/tmr2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/traps.o: traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/traps.o.d 
	@${RM} ${OBJECTDIR}/traps.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  traps.c  -o ${OBJECTDIR}/traps.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/traps.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/traps.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/uart1_queued.o: uart1_queued.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uart1_queued.o.d 
	@${RM} ${OBJECTDIR}/uart1_queued.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uart1_queued.c  -o ${OBJECTDIR}/uart1_queued.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/uart1_queued.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/uart1_queued.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/uc1701x.o: uc1701x.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/uc1701x.o.d 
	@${RM} ${OBJECTDIR}/uc1701x.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  uc1701x.c  -o ${OBJECTDIR}/uc1701x.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/uc1701x.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/uc1701x.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/coords.o: coords.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/coords.o.d 
	@${RM} ${OBJECTDIR}/coords.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  coords.c  -o ${OBJECTDIR}/coords.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/coords.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/coords.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/mc24aa512_i2c2.o: mc24aa512_i2c2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/mc24aa512_i2c2.o.d 
	@${RM} ${OBJECTDIR}/mc24aa512_i2c2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  mc24aa512_i2c2.c  -o ${OBJECTDIR}/mc24aa512_i2c2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/mc24aa512_i2c2.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/mc24aa512_i2c2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/geofence.o: geofence.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/geofence.o.d 
	@${RM} ${OBJECTDIR}/geofence.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  geofence.c  -o ${OBJECTDIR}/geofence.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/geofence.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mfillupper=255 -msmall-code -mconst-in-code -O0 -funroll-loops -fomit-frame-pointer -mcci -msmart-io=1 -Werror -Wall -msfr-warn=off -mno-override-inline 
	@${FIXDEPS} "${OBJECTDIR}/geofence.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/ESD.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/ESD.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_ICD3=1  -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)   -mreserve=data@0x800:0x81B -mreserve=data@0x81C:0x81D -mreserve=data@0x81E:0x81F -mreserve=data@0x820:0x821 -mreserve=data@0x822:0x823 -mreserve=data@0x824:0x827 -mreserve=data@0x82A:0x84F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_ICD3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/ESD.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/ESD.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/ESD.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
