#!/usr/bin/perl
# Copyright (C) 2023. Roger Doss.
#
#	Permission is hereby granted, free of charge, to any person obtaining a copy
#	of this software and associated documentation files (the "Software"), to deal
#	in the Software without restriction, including without limitation the rights
#	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#	copies of the Software, and to permit persons to whom the Software is
#	furnished to do so, subject to the following conditions:
#	
#	The above copyright notice and this permission notice shall be included in all
#	copies or substantial portions of the Software.
#	
#	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#	SOFTWARE.
#
use strict;
use warnings;

my $trials = 128;
if(defined ($ARGV[0])) {
  $trials = int($ARGV[0]);
}

print "running with $trials trials\n";

my $proc = "./one_way";

if(defined ($ARGV[1])) {
  $proc = $ARGV[1];
}

print "running with $proc proc\n";

my $filename = "dump.dat";

my %hash = ();

my $all_uniq=1;
for (my $i = 0; $i <= $trials; $i++) {
  print "trial [$i]\r";
  my $text = genText();
  chomp($text);
  if(length($text) <= 0) {
    $text = genText();
    chomp($text);
  }
  #print "text {$text}\n";
  #print "filename {$filename}\n";
  writeFile($filename, $text);
  my $result=`$proc $filename`;
  chomp($result);
  my @result=split / /,$result;
  if(exists $hash{$result[0]}) {
    if($hash{$result[0]} ne $text) {
      $hash{$result[0]} .= ",$text";
      print "collision found hash=[$result] val=[$hash{$result[0]}]\n";
      $all_uniq = 0;
      last;
    }
  } else {
    $hash{$result[0]} .= "$text";
  }
}

if($all_uniq == 0) {
  print "found collisions\n";
}

### methods ###
sub genText {
  my $text = "";
  my $r    = rand(65536);
  if($r <= 0) { $r *= -1; $r++; }
  my $size = int($r % 4099);
  #print "genText size [$size]\n";
  for(my $i = 0; $i < $size; $i++) {
    $r = rand(26);
    $text .= (97 + $r); # Add ascii 'a'.
  }
  return $text;
}

sub writeFile {
  my $filename = shift;
  my $text     = shift;
  open(FH, '>', $filename) or die $!;
  print FH $text;
  close(FH);
}
