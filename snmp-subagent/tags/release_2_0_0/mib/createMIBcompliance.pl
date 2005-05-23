#!/usr/bin/perl

use strict;

# change accordingly to your desires.

my $OUTPUT = "file.mib";
my $INPUT  = "hpi-mib.mib";
my $MIB_COMPLIANCE = "hpiMIBCompliances";
my $TABLE_PREFIX = "saHpi";

# 
# Do not change anything below here
my %sequence_items = ();
my %sequence_root = ();
my @sequences = ();
my $sequence_size = 0;
my @objects = ();
my @groups = ();
my @notifications = ();
my $notifications_size = 0;
my $group_size = 0;
my $objects_size = 0;

&parseMIB($INPUT);
&statistics();
&writeOut($OUTPUT);

sub parseMIB() {

  my $input = shift;
  my $object = "";
  my $state = 0;
  my @temp = ();
  my $sequence = "";
  open (INPUTFILE, $input) or die $!;

  while (<INPUTFILE>) {

    #print "$_\n";
    if (/NOTIFICATION-TYPE/i && $state == 0)
    {
      s/NOTIFICATION-TYPE//i;
      s/\s//g;
      $notifications[$notifications_size] = $_;
      $notifications_size++;
      next;
    }

    if (/OBJECT-TYPE/i && !(/,/) && $state ==0) {
      s/OBJECT-TYPE//i;
      s/\s//g;
      chomp($_);
      $objects[$objects_size] = $_;
      $objects_size++;
      $state = 1;    
    }

    # Should be SYNTAX
    if ($state == 1 && /SYNTAX/i) {
      # Remove junk
      s/SYNTAX//i;
      if (/SEQUENCE/i) {	
	s/SEQUENCE//i;
	s/OF//i;
	s/\s//g;
	$sequences[$sequence_size] = $objects[$objects_size-1];
	$sequence_root{$objects[$objects_size-1]}=$_;
	$sequence_size++;
	#print "Table :",$sequence_root{$objects[$objects_size-1]}, "\n";
      }
      $state = 0;
    }
    # Also search for SEQUENCE
    if (/SEQUENCE/i && $state == 0) {
      $state = 2;
      s/SEQUENCE//i;
      s/::=//;
      s/{//;
      s/\s//g;
      $sequence = $_;
      $sequence_items{$sequence}="";
      $state = 4;
      next;
    }
    if ($state == 4) {
      # The end of the SEQUENCE
      if (/}/)
	{
	  $state = 0;
	 # print $sequence_items{$sequence};
	}
      else 
	{
	  $_ =~ /\s*(\w*)\s*/;
	  $_ = $1;
	  s/\s//g;
	  $sequence_items{$sequence}=$sequence_items{$sequence}." ".$_;
	}
    }
  
  }
  close (INPUTFILE);
}

sub writeOut() {

  my $file = shift;

  my $item = "";
  my $temp_size= "";
  my @temp = ();

  open OUTPUT,">$file" or die $!;
  # Use this for sorted in alphabet 
  #foreach $item (sort keys (%sequence_root))
  # or use this for the natural order in the MIB.
  for (my $a =0; $a < $sequence_size; $a++)
    {
      $item = $sequences[$a];
      $_ = $item;
      s/Table/Group/g;
      $groups[$group_size] = $_;
      $group_size++;
      print OUTPUT ($_," OBJECT-GROUP\n");
      print OUTPUT ("\tOBJECTS {\n");
      @temp = split(/ /, $sequence_items{$sequence_root{$item}});
      $temp_size = @temp;
      for (my $j=0; $j < @temp; $j++) {
	{
	  if ($temp[$j] =~ /\w/) {
	    print OUTPUT ("\t\t",$temp[$j]);
	    if ($j+1 >= @temp)
	      {
		print OUTPUT ("\n");
	      }
	    else
	      {
		print OUTPUT (",\n");
	      }
	  }
	}
      }
      print OUTPUT ("\t}\n");
      print OUTPUT ("\tSTATUS\tcurrent\n");
      print OUTPUT ("\tDESCRIPTION\n");
      s/Group//g;
      print OUTPUT ("\t\t\"",$_," table.\"\n");
      print OUTPUT ("\t::= { ",$MIB_COMPLIANCE," ",$group_size,"}\n");
      print OUTPUT ("\n");
    }

  #
  # Print out the NOTIFICATION group
  #
  print OUTPUT ("\n",$TABLE_PREFIX,"NotificationGroup NOTIFICATION-GROUP\n");
  print OUTPUT ("\tNOTIFICATIONS {\n");
  for (my $z = 0; $z < $notifications_size; $z++)
    {
      if ($notifications[$z] =~ /\w/) 
	{
	  print OUTPUT ("\t\t",$notifications[$z]);
	  if ($z +1 >= $notifications_size)
	    {
	      print OUTPUT ("\n");
	    }
	  else
	    {
	      print OUTPUT (",\n");
	      }
	}
    }
  print OUTPUT ("\t}\n");
  print OUTPUT ("\tSTATUS\tcurrent\n");
  print OUTPUT ("\tDESCRIPTION\n");
  print OUTPUT ("\t\t\"Notification group.\"\n");
  print OUTPUT ("\t::= { ",$MIB_COMPLIANCE," ",$group_size+1,"}\n");
  print OUTPUT ("\n");

  # Last part, MODULE-COMPLIANCE
  print OUTPUT ("\n",$TABLE_PREFIX,"MIBCompliance MODULE-COMPLIANCE\n");
  print OUTPUT ("\tSTATUS\tcurrent\n");
  print OUTPUT ("\tDESCRIPTION\n");
  print OUTPUT ("\t\"The compliance statement for SMIv2 compliance of platform MIB.\"\n");
  print OUTPUT ("\tMODULE\n");
  print OUTPUT ("\tMANDATORY-GROUPS {\n");
  
  for (my $i = 0; $i < $group_size; $i++)
    {
      print OUTPUT ("\t\t",$groups[$i]);
      if ($i+1 >= $group_size) 
	{
	  print OUTPUT ("\n");
	}
      else
	{
	  print OUTPUT (",\n");
	  }
    }
  print OUTPUT ("\t}\n");
  print OUTPUT ("\t::= { ",$MIB_COMPLIANCE," ",$group_size+2,"}\n");
  close (OUTPUT);
}
sub statistics() {

  my $item = "";
  print $objects_size, " objects\n";
  print $sequence_size," tables\n";
#  foreach $item (sort keys (%sequence_root))
 #   {
 #    print $item,"\n";
#$sequence_root{$item},"\n";
  #  }
#  for (my $i=0; $i< $sequence_size; $i++) {
 #   print "\t ",$sequences[$i],"\n";
 # }

}
