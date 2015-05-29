#!/usr/bin/perl

use strict;
use warnings;

my $line;
my $filename = "Score.txt";
my @text;

open(SCORE, '<', $filename);
open(SAFE, '>', "safes.txt");
open(NONSAFE, '>', "non-safes.txt");

while ($line = <SCORE>) {
	#print $line;
	chomp($line);
	@text = $line =~ /.*\sP/gcimx;
	my @numbers = $text[0] =~ /\s.+\s/g;
	my $number = $numbers[0];
	$number =~ s/\s//gx;
	#print "$number\n";
	if($text[0] =~ /Non-/g){
		print NONSAFE "$number\n";
	} else {
		print SAFE "$number\n";
	}
}
close(SCORE);
close(SAFE);
close(NONSAFE);