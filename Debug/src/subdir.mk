################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ant.cpp \
../src/cvrp.cpp \
../src/metachallenge.cpp \
../src/sa.cpp \
../src/steiner.cpp \
../src/steiner_solution.cpp \
../src/util.cpp 

OBJS += \
./src/ant.o \
./src/cvrp.o \
./src/metachallenge.o \
./src/sa.o \
./src/steiner.o \
./src/steiner_solution.o \
./src/util.o 

CPP_DEPS += \
./src/ant.d \
./src/cvrp.d \
./src/metachallenge.d \
./src/sa.d \
./src/steiner.d \
./src/steiner_solution.d \
./src/util.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/opt/boost-1.35/include/boost-1_35 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


