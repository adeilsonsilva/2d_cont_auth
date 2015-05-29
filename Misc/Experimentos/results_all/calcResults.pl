#!/usr/bin/perl

use strict;
use warnings;

my $line;
my $frames = 0;
my @Safes;
my $safe;
my @score;
my @confidence;

open(FILE, '<', $ARGV[0]);

while($line = <FILE>) {
	chomp($line);
	$frames++;
	$safe = substr($line, -5);
    @confidence = $line =~ /Confidence:\s.*\s\|/gcimx;
    @score = $confidence[0] =~ /\s.*\s/gcimx;
    my $Score = $score[0];
    $Score =~ s/\s//gcimx;
	print "$Score $safe\n";
}
