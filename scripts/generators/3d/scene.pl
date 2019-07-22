#!/usr/bin/perl

use strict;
use Math::Trig;

my $segments = 2048;
my $circles = 4;
my $index = 1;

print "vn 0 0 1\n";

for (my $p=0; $p<$circles; $p++) {
    printf "\no Circle %d\n", $p;
    
    my ($angle, $step) = (0, 2.0 * pi / $segments);
    for (my $i=0; $i<$segments; $i++) {
        printf("v %.5f %.5f %.5f\n", ($p+1) * cos($angle), ($p+1)*sin($angle), $p);
        $angle += $step;
    }
    
    printf("f");
    for (my $i=0; $i<$segments; $i++) {
        printf(" %d//1", $i + $index);
    }
    printf("\n\n");
    
    $index += $segments;
}
