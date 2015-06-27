################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/PigsDAQ.cpp \
../src/PigsDAQ_test.cpp \
../src/PigsDict.cpp \
../src/PigsEvent.cpp \
../src/PigsGUI.cpp \
../src/PigsGUI_test.cpp \
../src/PigsStorage.cpp \
../src/spigs.cpp 

C_UPPER_SRCS += \
../src/guitest.C 

OBJS += \
./src/PigsDAQ.o \
./src/PigsDAQ_test.o \
./src/PigsDict.o \
./src/PigsEvent.o \
./src/PigsGUI.o \
./src/PigsGUI_test.o \
./src/PigsStorage.o \
./src/guitest.o \
./src/spigs.o 

CPP_DEPS += \
./src/PigsDAQ.d \
./src/PigsDAQ_test.d \
./src/PigsDict.d \
./src/PigsEvent.d \
./src/PigsGUI.d \
./src/PigsGUI_test.d \
./src/PigsStorage.d \
./src/spigs.d 

C_UPPER_DEPS += \
./src/guitest.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -fPIC -I/usr/include/root -I"/home/rc/git/DAQ-DT5781/spigs/src" -I/usr/local/include/CAEN -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.C
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -fPIC -I/usr/include/root -I"/home/rc/git/DAQ-DT5781/spigs/src" -I/usr/local/include/CAEN -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


