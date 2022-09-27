#!/bin/sh

# 
# Vivado(TM)
# runme.sh: a Vivado-generated Runs Script for UNIX
# Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
# 

if [ -z "$PATH" ]; then
  PATH=/home/eliquinox/Xilinx-2021/Vitis/2021.1/bin:/home/eliquinox/Xilinx-2021/Vivado/2021.1/ids_lite/ISE/bin/lin64:/home/eliquinox/Xilinx-2021/Vivado/2021.1/bin
else
  PATH=/home/eliquinox/Xilinx-2021/Vitis/2021.1/bin:/home/eliquinox/Xilinx-2021/Vivado/2021.1/ids_lite/ISE/bin/lin64:/home/eliquinox/Xilinx-2021/Vivado/2021.1/bin:$PATH
fi
export PATH

if [ -z "$LD_LIBRARY_PATH" ]; then
  LD_LIBRARY_PATH=
else
  LD_LIBRARY_PATH=:$LD_LIBRARY_PATH
fi
export LD_LIBRARY_PATH

HD_PWD='/home/eliquinox/code/fpga/microblaze-ethernet/microblaze-ethernet.runs/top_auto_pc_0_synth_1'
cd "$HD_PWD"

HD_LOG=runme.log
/bin/touch $HD_LOG

ISEStep="./ISEWrap.sh"
EAStep()
{
     $ISEStep $HD_LOG "$@" >> $HD_LOG 2>&1
     if [ $? -ne 0 ]
     then
         exit
     fi
}

EAStep vivado -log top_auto_pc_0.vds -m64 -product Vivado -mode batch -messageDb vivado.pb -notrace -source top_auto_pc_0.tcl
