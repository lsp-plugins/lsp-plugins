#!/usr/bin/perl

use strict;

my $steps = 16;
my $planes = 2;
my $index = 1;

print "vn 0 0 1\n";

print "\no Triangle\n";
print "v -1 -0.75 -1\n";
print "v 1 -0.75 -1\n";
print "v 0 1 -1\n";
print "f 1//1 2//1 3//1\n";
$index += 3;

for (my $p=0; $p<$planes; $p++) {
    printf "\no Plane %d\n", $p;
    
    for (my $y=0; $y<=$steps; $y++) {
        for (my $x=0; $x<=$steps; $x++) {
            printf("v %.2f %.2f -%d\n", ($x-8)*0.25, ($y-8)*0.25, ($p+2));
        }
    }

    for (my $y=0; $y<$steps; $y++) {
        for (my $x=0; $x<$steps; $x++) {
            my ($p0, $p1) = ($x + $y*($steps + 1) + $index, $x + 1 + $y*($steps + 1) + $index);
            my ($p2, $p3) = ($p0 + $steps + 1, $p1 + $steps + 1);
        
            printf "f %d//1 %d//1 %d//1\n", $p1, $p0, $p2;
            printf "f %d//1 %d//1 %d//1\n", $p1, $p2, $p3;
        }
    }
    
    $index += ($steps + 1)*($steps + 1);
}
