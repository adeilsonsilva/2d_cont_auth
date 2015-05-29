#!/usr/bin/perl
use strict;
my %result;
my @data;
my %datas;
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
    my ($score, $degree) = split(/\s+/, $line);
    #my $degree = $line;
    $datas{$score}++;
};

# Get sorted degrees for count-up iteration
@degrees = sort { $a <=> $b } keys %datas;

# Iterate degrees, adding each one's system count to result for this degree
# and all higher degrees
#for (my $i = 0; $i < scalar(@degrees); $i++) {
#    my $degree = $degrees[$i];
#    my $count = $datas{$degree};
#    for (my $j = $i; $j < scalar(@degrees); $j++) {
#        $result{$degrees[$j]} += $count;
#    };
#};

# Output result counts
foreach my $degree (sort { $a <=> $b } keys %datas) {
    my $string = sprintf("%.4f %.2f\n", $datas{$degree}/$ARGV[1], $degree);
    print $string;
};
