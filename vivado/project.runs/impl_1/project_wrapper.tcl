proc start_step { step } {
  set stopFile ".stop.rst"
  if {[file isfile .stop.rst]} {
    puts ""
    puts "*** Halting run - EA reset detected ***"
    puts ""
    puts ""
    return -code error
  }
  set beginFile ".$step.begin.rst"
  set platform "$::tcl_platform(platform)"
  set user "$::tcl_platform(user)"
  set pid [pid]
  set host ""
  if { [string equal $platform unix] } {
    if { [info exist ::env(HOSTNAME)] } {
      set host $::env(HOSTNAME)
    }
  } else {
    if { [info exist ::env(COMPUTERNAME)] } {
      set host $::env(COMPUTERNAME)
    }
  }
  set ch [open $beginFile w]
  puts $ch "<?xml version=\"1.0\"?>"
  puts $ch "<ProcessHandle Version=\"1\" Minor=\"0\">"
  puts $ch "    <Process Command=\".planAhead.\" Owner=\"$user\" Host=\"$host\" Pid=\"$pid\">"
  puts $ch "    </Process>"
  puts $ch "</ProcessHandle>"
  close $ch
}

proc end_step { step } {
  set endFile ".$step.end.rst"
  set ch [open $endFile w]
  close $ch
}

proc step_failed { step } {
  set endFile ".$step.error.rst"
  set ch [open $endFile w]
  close $ch
}

set_msg_config -id {HDL 9-1061} -limit 100000
set_msg_config -id {HDL 9-1654} -limit 100000

start_step init_design
set rc [catch {
  create_msg_db init_design.pb
  create_project -in_memory -part xc7z020clg484-1
  set_property board_part em.avnet.com:zed:part0:1.3 [current_project]
  set_property design_mode GateLvl [current_fileset]
  set_param project.singleFileAddWarning.threshold 0
  set_property webtalk.parent_dir /home/INTRA/chakal/workspace/project/vivado/project.cache/wt [current_project]
  set_property parent.project_path /home/INTRA/chakal/workspace/project/vivado/project.xpr [current_project]
  set_property ip_repo_paths {
  /home/INTRA/chakal/workspace/project/vivado/project.cache/ip
  /home/INTRA/chakal/workspace/project/ip_repo/SoC_Design/HDL/Audio_Mixer
  /home/INTRA/chakal/workspace/project/ip_repo/SoC_Design/IPs/OLED/ZedBoard_OLED_1.0
  /home/INTRA/chakal/workspace/project/ip_repo/zedboard_audio
  /home/INTRA/chakal/workspace/project/ip_repo/ip_repo_vivado/FILTER_IIR_1.0
  /home/INTRA/chakal/workspace/project/ip_repo/ip_repo_vivado/Volume_Pregain_1.0
  /home/INTRA/chakal/workspace/project/ip_repo/MY_AXI_AUDIO_1.0
} [current_project]
  set_property ip_output_repo /home/INTRA/chakal/workspace/project/vivado/project.cache/ip [current_project]
  set_property XPM_LIBRARIES {XPM_CDC XPM_MEMORY} [current_project]
  add_files -quiet /home/INTRA/chakal/workspace/project/vivado/project.runs/synth_1/project_wrapper.dcp
  read_xdc -ref project_processing_system7_0_0 -cells inst /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_processing_system7_0_0/project_processing_system7_0_0.xdc
  set_property processing_order EARLY [get_files /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_processing_system7_0_0/project_processing_system7_0_0.xdc]
  read_xdc -prop_thru_buffers -ref project_rst_processing_system7_0_100M_0 -cells U0 /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_rst_processing_system7_0_100M_0/project_rst_processing_system7_0_100M_0_board.xdc
  set_property processing_order EARLY [get_files /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_rst_processing_system7_0_100M_0/project_rst_processing_system7_0_100M_0_board.xdc]
  read_xdc -ref project_rst_processing_system7_0_100M_0 -cells U0 /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_rst_processing_system7_0_100M_0/project_rst_processing_system7_0_100M_0.xdc
  set_property processing_order EARLY [get_files /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_rst_processing_system7_0_100M_0/project_rst_processing_system7_0_100M_0.xdc]
  read_xdc -prop_thru_buffers -ref project_axi_gpio_0_0 -cells U0 /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_axi_gpio_0_0_1/project_axi_gpio_0_0_board.xdc
  set_property processing_order EARLY [get_files /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_axi_gpio_0_0_1/project_axi_gpio_0_0_board.xdc]
  read_xdc -ref project_axi_gpio_0_0 -cells U0 /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_axi_gpio_0_0_1/project_axi_gpio_0_0.xdc
  set_property processing_order EARLY [get_files /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_axi_gpio_0_0_1/project_axi_gpio_0_0.xdc]
  read_xdc -prop_thru_buffers -ref project_axi_gpio_1_0 -cells U0 /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_axi_gpio_1_0/project_axi_gpio_1_0_board.xdc
  set_property processing_order EARLY [get_files /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_axi_gpio_1_0/project_axi_gpio_1_0_board.xdc]
  read_xdc -ref project_axi_gpio_1_0 -cells U0 /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_axi_gpio_1_0/project_axi_gpio_1_0.xdc
  set_property processing_order EARLY [get_files /home/INTRA/chakal/workspace/project/vivado/project.srcs/sources_1/bd/project/ip/project_axi_gpio_1_0/project_axi_gpio_1_0.xdc]
  read_xdc /home/INTRA/chakal/workspace/project/ip_repo/zedboard_audio/constraints/zed_audio.xdc
  link_design -top project_wrapper -part xc7z020clg484-1
  write_hwdef -file project_wrapper.hwdef
  close_msg_db -file init_design.pb
} RESULT]
if {$rc} {
  step_failed init_design
  return -code error $RESULT
} else {
  end_step init_design
}

start_step opt_design
set rc [catch {
  create_msg_db opt_design.pb
  opt_design 
  write_checkpoint -force project_wrapper_opt.dcp
  report_drc -file project_wrapper_drc_opted.rpt
  close_msg_db -file opt_design.pb
} RESULT]
if {$rc} {
  step_failed opt_design
  return -code error $RESULT
} else {
  end_step opt_design
}

start_step place_design
set rc [catch {
  create_msg_db place_design.pb
  implement_debug_core 
  place_design 
  write_checkpoint -force project_wrapper_placed.dcp
  report_io -file project_wrapper_io_placed.rpt
  report_utilization -file project_wrapper_utilization_placed.rpt -pb project_wrapper_utilization_placed.pb
  report_control_sets -verbose -file project_wrapper_control_sets_placed.rpt
  close_msg_db -file place_design.pb
} RESULT]
if {$rc} {
  step_failed place_design
  return -code error $RESULT
} else {
  end_step place_design
}

start_step route_design
set rc [catch {
  create_msg_db route_design.pb
  route_design 
  write_checkpoint -force project_wrapper_routed.dcp
  report_drc -file project_wrapper_drc_routed.rpt -pb project_wrapper_drc_routed.pb
  report_timing_summary -warn_on_violation -max_paths 10 -file project_wrapper_timing_summary_routed.rpt -rpx project_wrapper_timing_summary_routed.rpx
  report_power -file project_wrapper_power_routed.rpt -pb project_wrapper_power_summary_routed.pb -rpx project_wrapper_power_routed.rpx
  report_route_status -file project_wrapper_route_status.rpt -pb project_wrapper_route_status.pb
  report_clock_utilization -file project_wrapper_clock_utilization_routed.rpt
  close_msg_db -file route_design.pb
} RESULT]
if {$rc} {
  step_failed route_design
  return -code error $RESULT
} else {
  end_step route_design
}

start_step write_bitstream
set rc [catch {
  create_msg_db write_bitstream.pb
  catch { write_mem_info -force project_wrapper.mmi }
  write_bitstream -force project_wrapper.bit 
  catch { write_sysdef -hwdef project_wrapper.hwdef -bitfile project_wrapper.bit -meminfo project_wrapper.mmi -file project_wrapper.sysdef }
  catch {write_debug_probes -quiet -force debug_nets}
  close_msg_db -file write_bitstream.pb
} RESULT]
if {$rc} {
  step_failed write_bitstream
  return -code error $RESULT
} else {
  end_step write_bitstream
}

