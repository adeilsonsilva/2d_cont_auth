#!/usr/bin/perl

use strict;
use warnings;

my $lines;
my $l = 0;
my $file = $ARGV[0];

chomp($file);

open FILE, $file;

while($lines = <FILE>){
	$l++;	
}

print "$l\n";
