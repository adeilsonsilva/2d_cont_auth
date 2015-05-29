#!/usr/bin/perl

use strict;
use warnings;

my $line;

open(FILE, '<', "Score.txt") or die "Could not open file $!";
open(SAFE, '>', "Safes.txt") or die "Could not open file $!";
open(NONSAFE, '>', "Nonsafes.txt") or die "Could not open file $!";

while($line = <FILE>){
    chomp($line);
    my @type = $line =~ /.*P/gcimx;
    my @numbers = $type[0] =~ /\s.*\s/gcimx;
    my $number = $numbers[0];
    $number =~ s/\s+//g;
    if($type[0] =~ /^Safe/){
        print SAFE "$number\n";
    }else{
        print NONSAFE "$number\n";
    }
}

close(FILE);
close(SAFE);
close(NONSAFE);
