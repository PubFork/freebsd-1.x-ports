
#
# rmservice -- script for removing (unadvertising) a currently 
#           -- advertised service
#

###########################################################################

dpcapLoad;

set servicesHost [lindex $dpcap(services) 0];
set servicesPort [lindex $dpcap(services) 1];

###########################################################################

if {$argc != 1} {
  puts stdout "usage : rmservice service";
  exit;
}

if {[catch {dp_MakeRPCClient $servicesHost $servicesPort} ns] == 0} {
    catch {dp_RPC $ns UnadvertiseService [lindex $argv 0]}
    dp_CloseRPC $ns
}
exit
