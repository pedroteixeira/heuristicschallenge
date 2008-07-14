################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ant.cpp \
../src/cvrp.cpp \
../src/metachallenge.cpp \
../src/steiner.cpp \
../src/util.cpp 

OBJS += \
./src/ant.o \
./src/cvrp.o \
./src/metachallenge.o \
./src/steiner.o \
./src/util.o 

CPP_DEPS += \
./src/ant.d \
./src/cvrp.d \
./src/metachallenge.d \
./src/steiner.d \
./src/util.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


