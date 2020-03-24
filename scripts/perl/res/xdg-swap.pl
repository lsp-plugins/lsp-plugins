#!/usr/bin/perl

use strict;

opendir DIRHDL, $ARGV[0];
my @files = grep /\.desktop$/, readdir DIRHDL;
closedir DIRHDL;

foreach my $file (@files) {
    open FD, '<', $file;
    my @lines = <FD>;
    close FD;
    
    open FD, '>', $file;
    foreach my $line(@lines) {
        chomp $line;
        if ($line =~ /^Name(\[.*\])?=(.*)$/o) {
            print FD "GenericName$1=$2\n";
        }
        elsif ($line =~ /^GenericName(\[.*\])?=(.*)$/o) {
            print FD "Name$1=$2\n";
        }
        else {
            print FD "$line\n";
        }
    }
    close FD;
}
