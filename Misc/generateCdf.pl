#!/usr/bin/perl

use strict;
use warnings;

use constant { true => 1, false => 0 };

my $filename = $ARGV[0];
open(FILE, '<', $filename.".txt") or die "Could not open file '$filename' $!";

sub isCounted {
	my @array = @{$_[0]};
	my $search = $_[1];

	foreach my $x (@array) {
		if ($x == $search) {
			return true;
		}
	}
	return false;
}