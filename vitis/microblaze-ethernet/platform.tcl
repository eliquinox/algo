# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/eliquinox/code/fpga/Vitis-2021/microblaze-ethernet/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/eliquinox/code/fpga/Vitis-2021/microblaze-ethernet/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {microblaze-ethernet}\
-hw {/home/eliquinox/code/fpga/microblaze-ethernet/top_wrapper.xsa}\
-proc {microblaze_0} -os {standalone} -out {/home/eliquinox/code/fpga/Vitis-2021}

platform write
platform generate -domains 
platform active {microblaze-ethernet}
bsp reload
bsp setlib -name lwip211 -ver 1.5
bsp config phy_link_speed "CONFIG_LINKSPEED100"
bsp write
bsp reload
catch {bsp regenerate}
platform generate
platform generate
platform generate -domains standalone_domain 
