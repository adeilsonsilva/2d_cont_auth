#!/usr/bin/perl

use strict;
use warnings;

my $line;
my $frames = 1;
my @Safes;
my $safe;

my $filename = $ARGV[0];
my $treshold = $ARGV[1];
chomp($filename);
chomp($treshold);
open(FILE, '<', $filename.".txt");
my @out = split('_', $filename);
open(OUT_ONE, '>', $out[0]);
open(OUT_TWO, '>', $out[1]);
open(FILEOUT, '>', $filename);

while($line = <FILE>) {
	chomp($line);
	$safe = substr($line, -5);
	if($frames <= $treshold) {
		print OUT_ONE "$frames $safe\n";
	} else{
		print OUT_TWO "$frames $safe\n";
	}
	print FILEOUT "$frames $safe\n";
	$frames++;
}

close(FILE);
close(OUT_ONE);
close(OUT_TWO);
close(FILEOUT);