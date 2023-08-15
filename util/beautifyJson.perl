#!/bin/perl -0

while(<>) {
    my @array = split(/(\{[^{}]+\})/, $_);
    for(my $a = 1; $a < scalar(@array); $a += 2) {
        $array[$a] =~ s!^\s+!!mg;
        $array[$a] =~ s![\r\n]+! !g;
    }
    print join "", @array;
}
