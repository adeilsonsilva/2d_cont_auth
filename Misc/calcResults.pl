#!/usr/bin/perl

use strict;
use warnings;

my $line;
my $frames = 0;
my $user = 0;
my $intruder = 0;
my @users;

open(FILE, '<', "results.txt");

while($line = <FILE>) {
	$frames++;
	@users = $line =~ /USER/gcimx;
	$user += scalar @users;
}

my $porcUser = $user/$frames;
my $porcIntr = 1 - $porcUser;
print "Total de Frames: $frames\n";
print "Porcentagem de usuario: " . $porcUser*100 . "%\n";
print "Porcentagem de intrusos: " . $porcIntr*100 . "%\n";
