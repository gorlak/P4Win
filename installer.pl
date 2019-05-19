#!/usr/bin/perl -w
use strict;
use Data::Dumper;
use File::Basename;
use File::Copy;
use File::Path;
use Getopt::Long;


#
# globals
#

my $g_ISCCFilePath        = File::Spec->catfile( 'Dependencies', 'InnoSetup', 'ISCC.exe' );
my $g_ISSFilePath         = 'installer.iss';
my $g_OutputBaseFilename  = 'P4WinSetup';
my $g_OutputDir           = 'Bin\\Release';
my $g_VersionFilePath     = 'Source\\gui\\resource.h';
my $g_OutputFilePath      = File::Spec->catfile( $g_OutputDir, "$g_OutputBaseFilename.exe" );


#
# extract version
#

my $g_MajorVersion = 0;
my $g_MinorVersion = 0;
my $g_PatchVersion = 0;

open IN, "<$g_VersionFilePath" or die "could not read $g_VersionFilePath: $!";
my @VersionFileContent = <IN>;
close IN or die "could not close $g_VersionFilePath: $!";
chomp @VersionFileContent;

foreach my $line ( @VersionFileContent )
{
  if ( $line =~ /VERSION_MAJOR\s+(\d+)/i )
  {
    $g_MajorVersion = $1;
  }
  elsif ( $line =~ /VERSION_MINOR\s+(\d+)/i )
  {
    $g_MinorVersion = $1;
  }
  elsif ( $line =~ /VERSION_PATCH\s+(\d+)/i )
  {
    $g_PatchVersion = $1;
  }
}
my $g_PrintVersion = "${g_MajorVersion}.${g_MinorVersion}.${g_PatchVersion}";


#
# build installer
#

print( "\n o Building installer for $g_PrintVersion...\n" );

if ( -e $g_OutputFilePath )
{
  system( "del /Q /F \"$g_OutputFilePath\"" );
}

mkpath( $g_OutputDir );

system( "\"$g_ISCCFilePath\" \"$g_ISSFilePath\" /q /o\"$g_OutputDir\" /f\"$g_OutputBaseFilename\" /d\"_AppVersionMajor=$g_MajorVersion\" /d\"_AppVersionMinor=$g_MinorVersion\" /d\"_AppVersionPatch=$g_PatchVersion\"" );

exit $? >> 8;