#!/usr/bin/perl

use strict;
#use warnings;

use constant { true => 1, false => 0 };

our @array;
my @lines;
my $line;
my $num;
my $filename = $ARGV[0];
my $lineCount = 0;
my $count = 0;
open(FILE, '<', $filename.".txt") or die "Could not open file '$filename' $!";
while($line = <FILE>){
	chomp($line);
	push (@lines, $line);
	$lineCount++;
}
close(FILE);

open(FILE, '<', $filename.".txt") or die "Erro!";

while($line = <FILE>){
	chomp($line);
	if(isCounted($line)){
		next;	
	}else{
		$count = 0;
		foreach $num (@lines){
			if($num == $line){
				$count++;	
			}	
		}	
		push(@array, $num);
		my $out = sprintf "%d  %.2f\n", $count, $line;
		print $out;
	}
}

close(FILE);

sub isCounted {
	#my @array = @{$_[0]};
	my $search = $_[0];

	foreach my $x (@array) {
		if ($x == $search) {
			return true;
		}
	}
	return false;
}
