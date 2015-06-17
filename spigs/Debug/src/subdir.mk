################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/PigsDAQ.cpp \
../src/PigsDAQ_test.cpp \
../src/spigs.cpp 

OBJS += \
./src/PigsDAQ.o \
./src/PigsDAQ_test.o \
./src/spigs.o 

CPP_DEPS += \
./src/PigsDAQ.d \
./src/PigsDAQ_test.d \
./src/spigs.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/root -I"/home/o/workspace/git/DAQ-DT5781/spigs/src" -I/usr/local/include/CAEN -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


