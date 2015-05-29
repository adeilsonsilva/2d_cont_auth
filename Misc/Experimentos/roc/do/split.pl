#!/usr/bin/perl

use strict;
use warnings;

my @values;
my $line;

open (FILE, '<', $ARGV[0]) or die "Erro!";

while($line = <FILE>){
	@values = split(' ', $line);
	print "$values[1]\n";
}
