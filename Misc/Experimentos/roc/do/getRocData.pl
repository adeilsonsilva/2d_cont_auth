#!/usr/bin/perl

use strict;
use warnings;

use constant {
	SAFES_TOTAL => 8573,
	NONSAFES_TOTAL => 17122
};

my %safes;
my %nonsafes;
my $limiar;
my $countVP;
my $countFP;
my $taxaVP;
my $taxaFP;


open(SAFES, '<', "safes.txt");
open(NONSAFES, '<', "nonsafes.txt");
open(ROC, '>', "roc.txt");
open(LIMIAR, '>', "limiares.txt");

while(my $line = <SAFES>){ 
	chomp $line;
	$safes{$line}++;
	#push @safes, $line;
}
while(my $line = <NONSAFES>){ 
	chomp $line; 
	$nonsafes{$line}++;
	#push @nonsafes, $line;
}

close(SAFES);
close(NONSAFES);

for($limiar = 0.0; $limiar < 1.05; $limiar += 0.05){
	$countVP = 0; 
	$countFP = 0;
	foreach my $valor (keys %safes){
		$countVP += $valor >= $limiar ? $safes{$valor} : 0;	
		print "$valor $limiar $countVP\n";
	}

	foreach my $valor (keys %nonsafes){
		$countFP += $valor >= $limiar ? $nonsafes{$valor} : 0;	
	}

	$taxaVP = $countVP/SAFES_TOTAL;
	$taxaFP = $countFP/NONSAFES_TOTAL;

	my $Out = sprintf("%.5f %.5f\n", $taxaFP, $taxaVP);
	print ROC $Out;
	$Out = sprintf("%.2f\n", $limiar);
	print LIMIAR $Out;
}


