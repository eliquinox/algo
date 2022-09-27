# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/eliquinox/code/fpga/Vitis-2021/echo-server-app_system/_ide/scripts/debugger_echo-server-app-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/eliquinox/code/fpga/Vitis-2021/echo-server-app_system/_ide/scripts/debugger_echo-server-app-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -filter {jtag_cable_name =~ "Digilent Arty A7-100T 210319B57CCDA" && level==0 && jtag_device_ctx=="jsn-Arty A7-100T-210319B57CCDA-13631093-0"}
fpga -file /home/eliquinox/code/fpga/Vitis-2021/echo-server-app/_ide/bitstream/download.bit
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
loadhw -hw /home/eliquinox/code/fpga/Vitis-2021/microblaze-ethernet/export/microblaze-ethernet/hw/top_wrapper.xsa -regs
configparams mdm-detect-bscan-mask 2
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
rst -system
after 3000
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
dow /home/eliquinox/code/fpga/Vitis-2021/echo-server-app/Debug/echo-server-app.elf
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
con
