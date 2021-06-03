#!/bin/perl

# my @SD_arr = qw(2 3 4);
my @SD_arr = qw(1);
my $BR_BL = 1164;
my @BR_EL_arr = qw(63 115 242 483);
my $NO_TILE = 24;
my $tileID;
my $cmd;
my $rootDir = "6f_2x2";
my $size;
my $BR_EL;

foreach $SD (@SD_arr){
	print "$SD\n";
	# Generate folder
	# $cmd = "mkdir ${rootDir}/${SD}s";
	# print "$cmd\n";
	# system $cmd;

	# Base layer
	# $cmd = "mkdir ${rootDir}/${SD}s/Face0";
	# print "$cmd\n";
	# system $cmd;

	$size = $BR_BL * $SD * 125; 
	$cmd = "dd if=/dev/zero of=${rootDir}/${SD}s/Face0/${BR_BL} bs=${size} count=1";
	print "$cmd\n";
	system $cmd;

	# Enhance layer
	for($tileID = 1; $tileID <= $NO_TILE; $tileID ++){
		# $cmd = "mkdir ${rootDir}/${SD}s/Face${tileID}";
		# print "$cmd\n";
		# system $cmd;
		foreach $BR_EL (@BR_EL_arr){
			$size = $BR_EL * $SD * 125;
			$cmd = "dd if=/dev/zero of=${rootDir}/${SD}s/Face${tileID}/${BR_EL} bs=${size} count=1";
			print "$cmd\n";
			system $cmd;
		}
	}
}