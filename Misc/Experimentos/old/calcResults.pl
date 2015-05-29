#!/usr/bin/perl

use strict;
use warnings;

my $line;
my $frames = 0;
my @Safes;
my $safe;

open(FILE, '<', "results.txt");

while($line = <FILE>) {
	chomp($line);
	$frames++;
	$safe = substr($line, -5);
	print "$safe\n";
}
