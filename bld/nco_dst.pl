#!/usr/bin/perl
				
# Purpose: Perform NCO distributions

# Usage:
# Export tagged, public versions

# $HOME/nc/nco/bld/nco_dst.pl --dbg=2 --bld --cln nco1_1_45
# $HOME/nc/nco/bld/nco_dst.pl --dbg=2 --cln --nst_all nco1_1_45
# $HOME/nc/nco/bld/nco_dst.pl --dbg=2 --cln --acd_cnt nco1_1_45
# $HOME/nc/nco/bld/nco_dst.pl --dbg=2 --cln --acd_prs nco1_1_45
# $HOME/nc/nco/bld/nco_dst.pl --dbg=2 --cln --cgd_cnt nco1_1_45
# $HOME/nc/nco/bld/nco_dst.pl --dbg=2 --cln --cray_prs nco1_1_45
# $HOME/nc/nco/bld/nco_dst.pl --dbg=2 --cln --blk_cnt nco1_1_45
# $HOME/nc/nco/bld/nco_dst.pl --dbg=2 --cln --dat_cnt nco1_1_45
# $HOME/nc/nco/bld/nco_dst.pl --dbg=2 --cln --ute_prs nco1_1_45

# Export daily snapshot
# $HOME/nc/nco/bld/nco_dst.pl --dbg=2 
# $HOME/nc/nco/bld/nco_dst.pl --dbg=1 --cln --nst

# Machines requiring interactive builds
# cd $HOME/nc/nco;cvs update;cd bld;make;make tst

BEGIN{
    unshift @INC,$ENV{'HOME'}.'/perl'; # Location of csz.pl and DBG.pm HaS98 p. 170
} # end BEGIN

my $CVS_Header='$Header: /data/zender/nco_20150216/nco/bld/nco_dst.pl,v 1.44 2000-01-28 01:15:03 zender Exp $';

# Specify modules
use strict; # Protect all namespaces
use Getopt::Long; # GNU-style getopt
use File::Basename; # For parsing filenames

# Personal modules
use DBG; # Debugging constants
require 'csz.pl'; # Contains date_time()

# Set output flushing to help debugging on hard crashes 
# These options update the filehandle after every output statement
select((select(STDOUT),$|=1)[0]); # Camel book, p. 110
select((select(STDERR),$|=1)[0]); # Camel book, p. 110

# Timing information
my ($lcl_date_time,$srt_usr_tm,$srt_sys_tm,$srt_child_usr_tm,$srt_child_sys_tm);
&time_srt($lcl_date_time,$srt_usr_tm,$srt_sys_tm,$srt_child_usr_tm,$srt_child_sys_tm);
printf STDOUT ("Start user time %f\n",$srt_usr_tm);

# Declare local variables
my ($idx,$rcd);
my ($prg_nm,$prg_dsc,$prg_vrs,$prg_date);
my ($pth_in,$fl_sfx);

my ($dst_vrs,$dst_fl);
my ($nco_vrs,$nco_vrs_mjr,$nco_vrs_mnr,$nco_vrs_pch);
my ($dly_snp);
my ($mk_cmd,$tar_cmd,$rmt_mch);

# Set defaults 
my $False=0;
my $True=1;

my $CVSROOT='/home/zender/cvs';
my $CVS_Date='$Date: 2000-01-28 01:15:03 $';
my $CVS_Id='$Id: nco_dst.pl,v 1.44 2000-01-28 01:15:03 zender Exp $';
my $CVS_Revision='$Revision: 1.44 $';
my $PVM_ARCH=$ENV{'PVM_ARCH'};
my $bld=$False; # Option bld; Whether to rebuild netCDF distribution
my $data_nm=$ENV{'DATA'};
my $dst_pth='/data/zender'; # Where the distribution will be exported and built
my $main_trunk_tag='nco';
my $nco_sng='nco';
my $usr_nm=$ENV{'USER'};
my $vrs_tag='';
my $www_drc='/web/web-data/cms/nco'; # WWW directory for package

# Set defaults for command line arguments
my $cln=$True; # GNU standard Makefile option `clean'
my $dbg_lvl=0;
my $dst_cln=$False; # GNU standard Makefile option `distclean'
my $nst_all=$False; # Option nst_all; Install version on all machines
my $acd_cnt=$False; # Option acd_cnt; Install version in acd contrib
my $acd_prs=$False; # Option acd_prs; Install version in acd personal
my $blk_cnt=$False; # Option blk_cnt; Install version in blackforest contrib
my $dat_cnt=$False; # Option dat_cnt; Install version in dataproc contrib
my $ute_prs=$False; # Option ute_prs; Install version in ute personal
my $cgd_cnt=$False; # Option cgd_cnt; Install version in CGD contrib
my $cgd_prs=$False; # Option cgd_prs; Install version in CGD personal
my $cray_prs=$False; # Option cray_prs; Install version in Cray personal

# Derived fields
if($PVM_ARCH =~ m/SUN/){ # See Camel p. 81 for =~ and m//
    $tar_cmd='gtar';
    $mk_cmd='make';
}elsif($PVM_ARCH =~ m/CRAY/){
    $tar_cmd='tar';
    $mk_cmd='gnumake';
}else{
    $tar_cmd='tar';
    $mk_cmd='make';
} # endelse
if($dst_pth eq '/home/zender'){die "$prg_nm: ERROR \$dst_pth eq $dst_pth";} # This would be disasterous

$prg_dsc='NCO distribution maker'; # Program description
($prg_nm,$prg_vrs)=$CVS_Id =~ /: (.+).pl,v ([\d.]+)/; # Program name and version
$prg_vrs.='*' if length('$Locker:  $ ') > 12; # Tack '*' if it is not checked in into CVS.
($prg_nm,$pth_in,$fl_sfx)=fileparse($0,''); # $0 is program name Camel p. 136
if(length($CVS_Date) > 6){($prg_date)=unpack '@7 a19',$CVS_Date;}else{$prg_date='Unknown';}

# Parse command line arguments: '!' means Boolean, '|' is OR, '=' specifies required argument: 'i' is integer, 'f' is float, 's' is string
$rcd=GetOptions( # man Getopt::GetoptLong
		'acd_cnt!' => \$acd_cnt,
		'acd_prs!' => \$acd_prs,
		'bld!' => \$bld,
		'blk_cnt!' => \$blk_cnt,
		'cgd_cnt!' => \$cgd_cnt,
		'cgd_prs!' => \$cgd_prs,
		'clean!' => \$cln,
		'cln!' => \$cln,
		'cray_prs!' => \$cray_prs,
		'dat_cnt!' => \$dat_cnt,
		'dbg_lvl=i' => \$dbg_lvl,
		'distclean!' => \$dst_cln,
		'dst_cln!' => \$dst_cln,
		'nst_all!' => \$nst_all,
		'ute_prs!' => \$ute_prs,
		 ); # end GetOptions arguments

# Parse positional arguments, if present
if($#ARGV > 0){die "$prg_nm: ERROR Called with $#ARGV+1 positional arguments, need no more than 1\n";}
elsif($#ARGV == 0){$vrs_tag=$ARGV[0];} # Version name is first positional argument, if present. 

if($nst_all){
    $bld=$True;
    $cgd_prs=$True;
    $cgd_cnt=$True;
    $acd_prs=$True;
    $acd_cnt=$True;
    $ute_prs=$True;
    $blk_cnt=$True;
    $dat_cnt=$True;
    $cray_prs=$True;
} # endif

# Print initialization state
if($dbg_lvl >= 1){print ("$prg_nm: $prg_dsc, version $prg_vrs of $prg_date\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$vrs_tag = $vrs_tag\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$dbg_lvl = $dbg_lvl\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$bld = $bld\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$cln = $cln\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$dst_cln = $dst_cln\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$nst_all = $nst_all\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$mk_cmd = $mk_cmd\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$tar_cmd = $tar_cmd\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$acd_cnt = $acd_cnt\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$acd_prs = $acd_prs\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$cgd_cnt = $cgd_cnt\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$cgd_prs = $cgd_prs\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$cray_prs = $cray_prs\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$blk_cnt = $blk_cnt\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$dat_cnt = $dat_cnt\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$ute_prs = $ute_prs\n");} # endif dbg

if($vrs_tag eq $main_trunk_tag || $vrs_tag eq ''){$dly_snp=$True;}else{$dly_snp=$False;}
# NCO is distributed using the `cvs export' command, so the 
# version tag to be distributed must be supplied to this script. 
if($dly_snp){
# The version tag is blank or of the form `nco'
    $nco_vrs=&YYYYMMDD();
    $dst_vrs=$nco_sng.'-'.$nco_vrs;
}else{
# The version tag is of the form `nco1_2_0'
    my ($tag_sng);
    my ($nco_psn);

    $tag_sng=$vrs_tag;
    $tag_sng=~s/_/./g; # Use =~ to bind $tag_sng to s/// Camel p. 81
    $nco_vrs=substr($tag_sng,length($nco_sng),length($tag_sng)-length($nco_sng));
    ($nco_vrs_mjr,$nco_vrs_mnr,$nco_vrs_pch)=split(/\./,$nco_vrs);
    $dst_vrs=$nco_sng.'-'.$nco_vrs;
    if($nco_vrs_mjr < 1 || $nco_vrs_mjr > 2){die "$prg_nm: ERROR $nco_vrs_mjr < 1 || $nco_vrs_mjr > 2"};
} # endelse
$dst_fl=$dst_vrs.'.tar.gz';

if($dbg_lvl >= 1){		 
    print STDOUT "$prg_nm: Version to release: $vrs_tag\n";
    print STDOUT "$prg_nm: Distribution version: $dst_vrs\n";
    print STDOUT "$prg_nm: Distribution file: $dst_fl\n";
    print STDOUT "$prg_nm: NCO version: $nco_vrs\n";
    print STDOUT "$prg_nm: NCO major version: $nco_vrs_mjr\n";
    print STDOUT "$prg_nm: NCO minor version: $nco_vrs_mnr\n";
    print STDOUT "$prg_nm: NCO patch version: $nco_vrs_pch\n";
} # end if dbg 

# Build distribution from scratch
if($bld){
    &cmd_prc("/bin/rm -r -f $dst_pth/$dst_vrs"); # Remove contents of current directory, if any
# NB: NCO code currently assumes -kkv and -r will fail otherwise
    if($dly_snp){
	&cmd_prc("cvs -d $CVSROOT export -kkv -D \"1 second ago\" -d $dst_pth/$dst_vrs nco"); # Export
    }else{
	&cmd_prc("cvs -d $CVSROOT export -kkv -r $vrs_tag -d $dst_pth/$dst_vrs nco"); # Export
    } # endelse
    &cmd_prc("printf $dst_vrs > $dst_pth/$dst_vrs/doc/VERSION"); # Stamp version in VERSION file
    
# Make sure documentation files are up to date
    my $bld_pth=$dst_pth.'/'."$dst_vrs".'/bld';
    chdir $bld_pth or die "$prg_nm: ERROR unable to chdir to $bld_pth: $!\n"; # $! is the system error sng
    &cmd_prc("$mk_cmd doc"); 
    &cmd_prc("$mk_cmd clean"); 
    
# Set up FTP server
    chdir $dst_pth or die "$prg_nm: ERROR unable to chdir to $dst_pth: $!\n"; # $! is the system error sng
    &cmd_prc("$tar_cmd -cvzf $dst_fl ./$dst_vrs"); # Create gzipped tarfile
    &cmd_prc("rsh ftp.cgd.ucar.edu /bin/rm -f /ftp/pub/zender/nco/$dst_fl"); # Remove any distribution with same name
    if($dly_snp){&cmd_prc("rsh ftp.cgd.ucar.edu /bin/rm -r -f /ftp/pub/zender/nco/nco-????????.tar.gz");} # Remove previous daily snapshots from FTP server
    &cmd_prc("rcp $dst_fl ftp.cgd.ucar.edu:/ftp/pub/zender/nco"); # Copy local tarfile to FTP server
    
# Full release procedure (public releases only) includes update Web pages
    if(!$dly_snp){
	&cmd_prc("rsh ftp.cgd.ucar.edu /bin/rm -f /ftp/pub/zender/nco/nco.tar.gz");
	&cmd_prc("rsh ftp.cgd.ucar.edu \"cd /ftp/pub/zender/nco; ln -s $dst_fl nco.tar.gz\"");
	&cmd_prc("/bin/cp -f $dst_pth/$dst_vrs/doc/index.shtml $www_drc/index.shtml");
	&cmd_prc("/bin/cp -f $dst_pth/$dst_vrs/doc/nco_news.shtml $www_drc/nco_news.shtml");
	&cmd_prc("/bin/cp -f $dst_pth/$dst_vrs/doc/nco.ps $www_drc/nco.ps");
	&cmd_prc("gzip --force $www_drc/nco.ps");
	&cmd_prc("/bin/cp -f $dst_pth/$dst_vrs/doc/README $www_drc/README");
#    &cmd_prc("/bin/cp -f $dst_pth/$dst_vrs/doc/INSTALL $www_drc/INSTALL");
	&cmd_prc("/bin/cp -f $dst_pth/$dst_vrs/doc/VERSION $www_drc/VERSION");
	&cmd_prc("/bin/cp -f $dst_pth/$dst_vrs/doc/ChangeLog $www_drc/ChangeLog");
    } # endif 
    
# Housekeeping
    if($cln){&cmd_prc("/bin/rm $dst_pth/$dst_fl");} # Remove local tarfile
    if($dst_cln){&cmd_prc("/bin/rm -r $dst_pth/$dst_vrs");} # Remove local distribution

# Sanity check
    &cmd_prc("rsh ftp.cgd.ucar.edu ls -l /ftp/pub/zender/nco");
} # endif bld

if($acd_prs){
     $rmt_mch='dust.acd.ucar.edu';
     print STDOUT "\n$prg_nm: Updating private NCO on $rmt_mch...\n";
     &cmd_prc("rsh $rmt_mch \"cd ~/nc/nco;cvs update\"");
     &cmd_prc("rsh $rmt_mch \"cd ~/nc/nco/bld;make cln all tst\"");
# Unfortunately, sudo does not work at all with rsh
#    &cmd_prc("rsh $rmt_mch \"sudo cp /gs/zender/bin/LINUX/nc* /usr/local/bin\"");
     print STDOUT "$prg_nm: Done updating private NCO binaries on $rmt_mch\n\n";
} # endif acd_prs

if($acd_cnt){
    $rmt_mch='garcia.acd.ucar.edu';
    print STDOUT "\n$prg_nm: Updating private NCO on $rmt_mch...\n";
    &cmd_prc("rsh $rmt_mch \"cd ~/nc/nco;/local/bin/cvs update\"");
    &cmd_prc("rsh $rmt_mch \"cd ~/nc/nco/bld;/local/bin/gmake cln all tst\"");
# Unfortunately, sudo does not work at all with rsh
#    &cmd_prc("rsh $rmt_mch \"sudo cp /a1/zender/bin/ALPHA/nc* /usr/local/bin\"");
    print STDOUT "$prg_nm: Done updating private NCO binaries on $rmt_mch\n\n";
} # endif acd_cnt

if($cgd_cnt){
    $rmt_mch='sanitas.cgd.ucar.edu';
#    rsh $rmt_mch 'printf $PVM_ARCH'
    print STDOUT "\n$prg_nm: Updating contrib NCO on $rmt_mch...\n";
    &cmd_prc("rsh $rmt_mch \"/bin/rm -r -f /usr/tmp/zender/nco*\"");
    &cmd_prc("rsh $rmt_mch \"mkdir -p /usr/tmp/zender/$dst_vrs/obj\"");
    &cmd_prc("rcp -p ftp.cgd.ucar.edu:/ftp/pub/zender/nco/nco.tar.gz $rmt_mch:/usr/tmp/zender");
    &cmd_prc("rsh $rmt_mch \"cd /usr/tmp/zender;gtar -xvzf nco.tar.gz;rm -f nco.tar.gz\"");
    &cmd_prc("rsh $rmt_mch \"cd /usr/tmp/zender/$dst_vrs/bld; setenv MY_BIN_DIR /contrib/nco-1.1/bin; setenv MY_LIB_DIR /contrib/nco-1.1/lib; setenv MY_OBJ_DIR /usr/tmp/zender/$dst_vrs/obj; gmake cln all test\"");
    print STDOUT "$prg_nm: Done updating contrib NCO on $rmt_mch\n\n";
} # endif cgd_cnt

if($dat_cnt){
    $rmt_mch='dataproc.ucar.edu';
#    rsh $rmt_mch 'printf $PVM_ARCH'
    print STDOUT "\n$prg_nm: Updating contrib NCO on $rmt_mch...\n";
    &cmd_prc("rsh $rmt_mch \"/bin/rm -r -f /usr/tmp/zender/nco*\"");
    &cmd_prc("rsh $rmt_mch \"mkdir -p /usr/tmp/zender/$dst_vrs/obj\"");
    &cmd_prc("rcp -p ftp.cgd.ucar.edu:/ftp/pub/zender/nco/nco.tar.gz $rmt_mch:/usr/tmp/zender");
    &cmd_prc("rsh $rmt_mch \"cd /usr/tmp/zender;tar -xvzf nco.tar.gz;rm -f nco.tar.gz\"");
    &cmd_prc("rsh $rmt_mch \"cd /usr/tmp/zender/$dst_vrs/bld; setenv MY_BIN_DIR /contrib/nco-1.1/bin; setenv MY_LIB_DIR /contrib/nco-1.1/lib; setenv MY_OBJ_DIR /usr/tmp/zender/$dst_vrs/obj; gmake cln all tst\"");
    print STDOUT "$prg_nm: Done updating contrib NCO on $rmt_mch\n\n";
} # endif dat_cnt

if($blk_cnt){
    $rmt_mch='blackforest.ucar.edu';
#    rsh $rmt_mch 'printf $PVM_ARCH'
    print STDOUT "\n$prg_nm: Updating contrib NCO on $rmt_mch...\n";
    &cmd_prc("rsh $rmt_mch \"/bin/rm -r -f /usr/tmp/zender/nco*\"");
    &cmd_prc("rsh $rmt_mch \"mkdir -p /usr/tmp/zender/$dst_vrs/obj\"");
    &cmd_prc("rcp -p ftp.cgd.ucar.edu:/ftp/pub/zender/nco/nco.tar.gz $rmt_mch:/usr/tmp/zender");
    &cmd_prc("rsh $rmt_mch \"cd /usr/tmp/zender;tar -xvzf nco.tar.gz;rm -f nco.tar.gz\"");
    &cmd_prc("rsh $rmt_mch \"cd /usr/tmp/zender/$dst_vrs/bld; setenv MY_BIN_DIR /home/blackforest/zender/bin/AIX; setenv MY_LIB_DIR /home/blackforest/zender/lib/AIX; setenv MY_OBJ_DIR /home/blackforest/zender/obj/AIX; setenv NETCDF_INC /usr/local/include; setenv NETCDF_LIB /usr/local/lib32/r4i4; gmake cln all tst\"");
    print STDOUT "$prg_nm: Done updating contrib NCO on $rmt_mch\n\n";
} # endif blk_cnt

if($ute_prs){
    $rmt_mch='utefe.ucar.edu'; # utefe and ute are cross-mounted, utefe is for interactive logins
#    rsh $rmt_mch 'printf $PVM_ARCH'
    print STDOUT "\n$prg_nm: Updating personal NCO on $rmt_mch...\n";
    &cmd_prc("rsh $rmt_mch \"/bin/rm -r -f /usr/tmp/zender/nco*\"");
    &cmd_prc("rsh $rmt_mch \"mkdir -p /usr/tmp/zender/$dst_vrs/obj\"");
    &cmd_prc("rcp -p ftp.cgd.ucar.edu:/ftp/pub/zender/nco/nco.tar.gz $rmt_mch:/usr/tmp/zender");
    &cmd_prc("rsh $rmt_mch \"cd /usr/tmp/zender;tar -xvzf nco.tar.gz;rm -f nco.tar.gz\"");
    &cmd_prc("rsh $rmt_mch \"cd /usr/tmp/zender/$dst_vrs/bld; setenv MY_BIN_DIR /home/ute/zender/bin/SGIMP64/bin; setenv MY_LIB_DIR /home/ute/zender/bin/SGIMP64/lib; setenv MY_OBJ_DIR /usr/tmp/zender/$dst_vrs/obj; gmake cln all tst\"");
    print STDOUT "$prg_nm: Done updating contrib NCO on $rmt_mch\n\n";
} # endif ute_prs

if($cray_prs){
    $rmt_mch='ouray.ucar.edu';
    print STDOUT "\n$prg_nm: Updating private NCO on $rmt_mch...\n";
    &cmd_prc("rsh $rmt_mch \"/bin/rm -r -f /usr/tmp/zender/nco*\"");
    &cmd_prc("rsh $rmt_mch \"mkdir -p /usr/tmp/zender/$dst_vrs/obj\"");
    &cmd_prc("rcp -p ftp.cgd.ucar.edu:/ftp/pub/zender/nco/nco.tar.gz $rmt_mch:/usr/tmp/zender");
    &cmd_prc("rsh $rmt_mch \"cd /usr/tmp/zender;gunzip nco.tar.gz;tar -xvf nco.tar;rm -f nco.tar*\"");
    &cmd_prc("rsh $rmt_mch \"cd /usr/tmp/zender/$dst_vrs/bld; setenv MY_BIN_DIR /home/ouray0/zender/bin/CRAY; setenv MY_LIB_DIR /usr/tmp/zender/$dst_vrs/lib; setenv MY_OBJ_DIR /usr/tmp/zender/$dst_vrs/obj; gnumake cln all tst\"");
    print STDOUT "$prg_nm: Done updating contrib NCO on $rmt_mch\n\n";
} # endif cray_prs
