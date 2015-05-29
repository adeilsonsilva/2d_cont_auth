#!/usr/bin/perl

use strict;
use warnings;

my $line;

open (FILE, '<', $ARGV[0]) or die "Erro!";

while($line = <FILE>){
    print "Non-Safe $line";
}

close(FILE);
