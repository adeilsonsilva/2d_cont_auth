#!/usr/bin/perl
use strict;
my %result;
my @data;
my %data;
my @degrees;
my $infile = $ARGV[0] || die "Usage: $0 <file>\n";

chomp($infile);
# Read source data from input file
open IN, '<', $infile
    or die "Couldn't open data file: $!\n";
while (my $line = <IN>) { chomp $line; push @data, $line; };
close IN;

# Convert data lines to hash
foreach my $line (@data) {
    #my ($count, $degree) = split(/\s+/, $line);
    my $degree = $line;
    $data{$degree}++;
};
print "aaaaa\n";
# Get sorted degrees for count-up iteration
@degrees = sort { $a <=> $b } keys %data;

# Iterate degrees, adding each one's system count to result for this degree
# and all higher degrees
for (my $i = 0; $i < scalar(@degrees); $i++) {
    my $degree = $degrees[$i];
    my $count = $data{$degree};
    for (my $j = $i; $j < scalar(@degrees); $j++) {
        $result{$degrees[$j]} += $count;
    };
};

# Output result counts
foreach my $degree (sort { $a <=> $b } keys %result) {
    print "$result{$degree} $degree\n";
};