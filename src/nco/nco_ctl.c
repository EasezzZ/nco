/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_ctl.c,v 1.50 2004-06-18 23:56:45 zender Exp $ */

/* Purpose: Program flow control functions */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_ctl.h" /* Program flow control functions */

void
nco_exit /* [fnc] Wrapper for exit() */
(int rcd) /* I [enm] Return code */
{
  /* Purpose: Wrapper for exit() */

  exit(rcd);
} /* nco_exit() */

void 
nco_exit_gracefully(void) /* [fnc] Clean up timers, file descriptors, memory, then exit */
{
  char *time_bfr_end;
  time_t time_crr_time_t;
  
  /* End clock */ 
  time_crr_time_t=time((time_t *)NULL);
  time_bfr_end=ctime(&time_crr_time_t); time_bfr_end=time_bfr_end; /* Avoid compiler warning until variable is used for something */
  /*  (void)fprintf(stderr,"\tend = %s\n",time_bfr_end);*/

  (void)fclose(stderr);
  (void)fclose(stdin);
  (void)fclose(stdout);

  nco_exit(EXIT_SUCCESS);
} /* end nco_exit_gracefully() */

bool /* [flg] Program is multi-file operator */
nco_is_mlt_fl_opr /* [fnc] Query whether program is multi-file operator */
(const int prg_id) /* [enm] Program ID */
{
  /* Purpose: Is this a multi-file operator?
     Such operators attempt to read input filenames from stdin when input filenames are not positional arguments */
  switch(prg_id){
  case ncea:
  case ncecat: 
  case ncra:
  case ncrcat: 
    return True;
    break;
  case ncap: 
  case ncatted: 
  case ncbo: 
  case ncflint: 
  case ncks: 
  default:
    return False;
    break;
  } /* end switch */
} /* end nco_is_mlt_fl_opr() */

bool /* [flg] Program does arithmetic */
nco_is_rth_opr /* [fnc] Query whether program does arithmetic */
(const int prg_id) /* [enm] Program ID */
{
  /* Purpose: Does operator do arithmetic?
     Consequences are operator specific
     Currently, arithmetic operators automatically unpack variables by default 
     Non-arithmetic operators do not unpack variables */
  switch(prg_id){
  case ncap: 
  case ncbo:
  case ncea:
  case ncflint:
  case ncra:
  case ncwa:
    return True;
    break;
  case ncatted: 
  case ncecat: 
  case ncks: 
  case ncrcat: 
  default:
    return False;
    break;
  } /* end switch */
} /* end nco_is_rth_opr() */

void
nco_lbr_vrs_prn(void) /* [fnc] Print netCDF library version */
{
  /* Purpose: Print netCDF library version */

  char *cmp_dat_sng;
  char *dlr_ptr;
  char *lbr_sng;
  char *lbr_vrs_sng;
  char *of_ptr;

  size_t cmp_dat_sng_lng;
  size_t lbr_vrs_sng_lng;

  /* As of netCDF 3.4, nc_inq_libvers() returns strings like "3.4 of May 16 1998 14:06:16 $" */  
  lbr_sng=(char *)strdup(nc_inq_libvers());
  of_ptr=strstr(lbr_sng," of ");
  if(of_ptr == NULL)(void)fprintf(stderr,"%s: WARNING nco_lbr_vrs_prn() reports of_ptr == NULL\n",prg_nm_get());
  lbr_vrs_sng_lng=(size_t)(of_ptr-lbr_sng);
  lbr_vrs_sng=(char *)nco_malloc(lbr_vrs_sng_lng+1);
  strncpy(lbr_vrs_sng,lbr_sng,lbr_vrs_sng_lng);
  lbr_vrs_sng[lbr_vrs_sng_lng]='\0'; /* NUL-terminate */

  dlr_ptr=strstr(lbr_sng," $");
  if(dlr_ptr == NULL)(void)fprintf(stderr,"%s: WARNING nco_lbr_vrs_prn() reports dlr_ptr == NULL\n",prg_nm_get());
  cmp_dat_sng_lng=(size_t)(dlr_ptr-of_ptr-4); /* 4 is the length of " of " */
  cmp_dat_sng=(char *)nco_malloc(cmp_dat_sng_lng+1ul);
  strncpy(cmp_dat_sng,of_ptr+4,cmp_dat_sng_lng); /* 4 is the length of " of " */
  cmp_dat_sng[cmp_dat_sng_lng]='\0'; /* NUL-terminate */

  (void)fprintf(stderr,"Linked to netCDF library version %s, compiled %s\n",lbr_vrs_sng,cmp_dat_sng);
  (void)fprintf(stdout,"Homepage URL: http://nco.sf.net\n");
  (void)fprintf(stdout,"User's Guide: http://nco.sf.net/nco.html\n");
  /* TKN2YESNO is insufficient when TKN is undefined
     Full macro language like M4 might be useful here, though probably too much trouble */
#define TKN2YESNO(x) ((x+0) ? ("No"):("Yes"))
  /* Configuration option tokens must be consistent among configure.in, bld/Makefile, and nco_ctl.c
     Arrange tokens alphabetically by first word in English text description */
  (void)fprintf(stderr,"Configuration Option:\tActive?\tMeaning or Reference:\nDebugging: Custom\t%s\tPedantic, bounds checking (slowest execution)\nDebugging: Symbols\t%s\tProduce symbols for debuggers (e.g., dbx, gdb)\nDODS/OpenDAP clients\t%s\thttp://nco.sf.net/nco.html#DODS\nInternationalization\t%s\thttp://nco.sf.net/nco.html#i18n (pre-alpha)\nOpenMP Multi-threading\t%s\thttp://nco.sf.net/nco.html#omp (alpha testing)\nOptimization: run-time\t%s\tFastest execution possible (slowest compilation)\nShared libraries built\t%s\tSmall, dynamically linked executables\nStatic libraries built\t%s\tLarge executables with private namespaces\nUDUnits conversions\t%s\thttp://nco.sf.net/nco.html#UDUnits\nWildcarding (regex)\t%s\thttp://nco.sf.net/nco.html#rx\n%s",
#if defined(ENABLE_DEBUG_CUSTOM) && (ENABLE_DEBUG_CUSTOM)
		"Yes",
#else /* !ENABLE_DEBUG_CUSTOM */
		"No",
#endif /* !ENABLE_DEBUG_CUSTOM */
#if defined(ENABLE_DEBUG_SYMBOLS) && (ENABLE_DEBUG_SYMBOLS)
		"Yes",
#else /* !ENABLE_DEBUG_SYMBOLS */
		"No",
#endif /* !ENABLE_DEBUG_SYMBOLS */
#if defined(ENABLE_DODS) && (ENABLE_DODS)
		"Yes",
#else /* !ENABLE_DODS */
		"No",
#endif /* !ENABLE_DODS */
#if defined(I18N) && (I18N)
		"Yes",
#else /* !I18N */
		"No",
#endif /* !I18N */
#if defined(_OPENMP) && (_OPENMP)
		"Yes",
#else /* !_OPENMP */
		"No",
#endif /* !_OPENMP */
#if defined(ENABLE_OPTIMIZE_CUSTOM) && (ENABLE_OPTIMIZE_CUSTOM)
		"Yes",
#else /* !ENABLE_OPTIMIZE_CUSTOM */
		"No",
#endif /* !ENABLE_OPTIMIZE_CUSTOM */
#if defined(ENABLE_SHARED) && (ENABLE_SHARED)
		"Yes",
#else /* !ENABLE_SHARED */
		"No",
#endif /* !ENABLE_SHARED */
#if defined(ENABLE_STATIC) && (ENABLE_STATIC)
		"Yes",
#else /* !ENABLE_STATIC */
		"No",
#endif /* !ENABLE_STATIC */
#if defined(ENABLE_UDUNITS) && (ENABLE_UDUNITS)
		"Yes",
#else /* !ENABLE_UDUNITS */
		"No",
#endif /* !ENABLE_UDUNITS */
#if defined(NCO_HAVE_REGEX_FUNCTIONALITY) && (NCO_HAVE_REGEX_FUNCTIONALITY)
		"Yes",
#else /* !NCO_HAVE_REGEX_FUNCTIONALITY */
		"No",
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */
		""); /* End of print statement marker */

  (void)fprintf(stderr,"\n%s",nco_nmn_get());

  lbr_vrs_sng=(char *)nco_free(lbr_vrs_sng);
  lbr_sng=(char *)nco_free(lbr_sng);
  cmp_dat_sng=(char *)nco_free(cmp_dat_sng);
} /* end nco_lbr_vrs_prn() */

const char * /* O [sng] Mnemonic that describes current NCO version */
nco_nmn_get(void) /* [fnc] Return mnemonic that describes current NCO version */
{ 
  /* Purpose: Return mnemonic describing current NCO version */
  return "NCO save you time? Want to help NCO get funded to implement new and frequenctly requested features? Peruse NCO's first proposal for Institutional funding:\nhttp://dust.ess.uci.edu/prp/prp_sei/prp_sei.pdf\nEndorse the proposal by sending e-mail to surname@uci.edu (surname=zender) with Subject: \"NSF SEI(GEO) NCO/SDO Proposal Endorsement\". Put (at least) your Name, Title, and Institutional affiliation in the body. Add any additional comments. It is never too late to endorse this proposal, as it may be recycled at the next opportunity. The information in your endorsement will be used to create supplementary documents to NSF and NASA for future NCO proposal advocacy.\n";
} /* end nco_nmn_get() */

char * /* O [sng] nm_in stripped of any path (i.e., program name stub) */ 
prg_prs /* [fnc] Strip program name to stub and return program ID */
(const char * const nm_in, /* I [sng] Name of program, i.e., argv[0] (may include path prefix) */
 int * const prg_lcl) /* O [enm] Enumerated number corresponding to nm_in */
{
  /* Purpose: Set program name and enum */

  char *nm_out;

  /* Get program name (use strrchr() first in case nm_in contains a path)
     fxm: Put initial nm_out pointer in list of malloc()'s to be freed
     Then free this memory in nco_exit() */
  nm_out=(char *)strdup(nm_in);
  if(strrchr(nm_out,'/') != NULL) nm_out=strrchr(nm_out,'/')+1;

  /* Skip possible libtool prefix */
  if(!strncmp(nm_out,"lt-",3)){nm_out+=3;}

  /* Classify calling program */
  if(!strcmp(nm_out,"ncra")){*prg_lcl=ncra;}
  else if(!strcmp(nm_out,"ncap")){*prg_lcl=ncap;}
  else if(!strcmp(nm_out,"ncea")){*prg_lcl=ncea;}
  else if(!strcmp(nm_out,"ncbo")){*prg_lcl=ncbo;}
  /* Synonyms for ncbo */
  else if(!strcmp(nm_out,"ncadd")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out,"ncdiff")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out,"ncsub")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out,"ncsubtract")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out,"ncmult")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out,"ncmultiply")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out,"ncdivide")){*prg_lcl=ncbo;}
  /* End synonyms for ncbo */
  else if(!strcmp(nm_out,"ncflint")){*prg_lcl=ncflint;}
  else if(!strcmp(nm_out,"ncwa")){*prg_lcl=ncwa;}
  else if(!strcmp(nm_out,"ncrcat")){*prg_lcl=ncrcat;}
  else if(!strcmp(nm_out,"ncecat")){*prg_lcl=ncecat;}
  else if(!strcmp(nm_out,"ncks")){*prg_lcl=ncks;}
  else if(!strcmp(nm_out,"ncrename")){*prg_lcl=ncrename;}
  else if(!strcmp(nm_out,"ncatted")){*prg_lcl=ncatted;}
  else{
    (void)fprintf(stdout,"%s: ERROR executable name %s not registered in prg_prs()\n",nm_out,nm_out);
    nco_exit(EXIT_FAILURE);
  } /* end else */

  return nm_out;

} /* end prg_prs() */

void 
nco_usg_prn(void)
{
  /* Purpose: Print correct command-line usage (currently to stdout) */

  char *opt_sng=NULL_CEWI;

  int prg_lcl;

  prg_lcl=prg_get();

  switch(prg_lcl){
  case ncra:
  case ncea:
    opt_sng=(char *)strdup("[-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-l path] [-n ...] [-O] [-o out.nc] [-p path] [-R] [-r] [-v ...] [-x] [-y op_typ] in.nc [...] [out.nc]\n");
    break;
  case ncrcat:
  case ncecat:
    opt_sng=(char *)strdup("[-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-l path] [-n ...] [-O] [-o out.nc] [-p path] [-R] [-r] [-v ...] [-x] in.nc [...] [out.nc]\n");
    break;
  case ncbo:
    opt_sng=(char *)strdup("[-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-l path] [-n ...] [-O] [-o out.nc] [-p path] [-R] [-r] [-v ...] [-x] [-y op_typ] in_1.nc in_2.nc [out.nc]\n");
    break;
  case ncflint:
    opt_sng=(char *)strdup("[-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-i var,val] [-l path] [-n ...] [-O] [-o out.nc] [-p path] [-R] [-r] [-v ...] [-x] [-w wgt_1[,wgt_2]] in_1.nc in_2.nc [out.nc]\n");
    break;
  case ncwa:
    opt_sng=(char *)strdup("[-A] [-a ...] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-I] [-l path] [-m mask] [-M val] [-N] [-O] [-o out.nc] [-p path] [-R] [-r] [-t condition] [-v ...] [-w wgt] [-x] [-y op_typ] in.nc [out.nc]\n");
    break;
  case ncap:
    opt_sng=(char *)strdup("[-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-f] [-h] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] [-s algebra] [-S fl.nco] [-v] in.nc [out.nc]\n");
    break;
  case ncks:
    opt_sng=(char *)strdup("[-A] [-a] [-B] [-b fl_bnr] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-l path] [-m] [-M] [-O] [-o out.nc] [-p path] [-q] [-R] [-r] [-s format] [-u] [-v ...] [-x] in.nc [[out.nc]]\n");
    break;
  case ncatted:
    opt_sng=(char *)strdup("[-a ...] [-D dbg_lvl] [-h] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] in.nc [[out.nc]]\n");
    break;
  case ncrename:
    opt_sng=(char *)strdup("[-a ...] [-D dbg_lvl] [-d ...] [-h] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] [-v ...] in.nc [[out.nc]]\n");
    break;
  } /* end switch */
  
  /* Public service announcements */
  (void)fprintf(stdout,"NCO homepage at http://nco.sf.net has complete online User's Guide\n");
  (void)fprintf(stdout,"Post questions, suggestions, patches at http://sf.net/projects/nco\n");

  /* We now have command-specific command line option string */
  (void)fprintf(stdout,"%s %s\n",prg_nm_get(),opt_sng);

  if(strstr(opt_sng,"-A")) (void)fprintf(stdout,"-A, --apn, --append\tAppend to existing output file, if any\n");
  if(strstr(opt_sng,"-a")){
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-a, --attribute old_att,new_att Attribute's old and new names\n");
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-a, --avg, --average avg_dim1[,avg_dim2[...]] Averaging dimensions\n");
    if(prg_lcl == ncks) (void)fprintf(stdout,"-a, --abc, --alphabetize\tDisable alphabetization of extracted variables\n");
    if(prg_lcl == ncatted) (void)fprintf(stdout,"-a, --attribute att_nm,var_nm,mode,att_typ,att_val Attribute specification:\n\t\t\tmode = a,c,d,m,o and att_typ = f,d,l,s,c,b\n");
  } /* end if */
  if(strstr(opt_sng,"-B")) (void)fprintf(stdout,"-B, --bnr, --binary\tWrite data to unformatted binary file\n");
  if(strstr(opt_sng,"-b")) (void)fprintf(stdout,"-b, --fl_bnr, --binary-file fl_bnr\tUnformatted binary file to write\n");
  if(strstr(opt_sng,"-c")) (void)fprintf(stdout,"-c, --crd, --coords\tCoordinate variables will all be processed\n");
  if(strstr(opt_sng,"-C")) (void)fprintf(stdout,"-C, --nocoords\t\tAssociated coordinate variables should not be processed\n");
  if(strstr(opt_sng,"-D")) (void)fprintf(stdout,"-D, --dbg_lvl, --debug-level dbg_lvl\tDebugging level\n");
  if(strstr(opt_sng,"-d")){
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-d, --dmn, --dimension old_dim,new_dim Dimension's old and new names\n");
    else if(prg_lcl == ncks) (void)fprintf(stdout,"-d, --dmn, --dimension dim,[min][,[max]][,[stride]] Dimension's limits and stride in hyperslab\n");
    else if(prg_lcl == ncra || prg_lcl == ncrcat) (void)fprintf(stdout,"-d, --dmn, --dimension dim,[min][,[max]][,[stride]] Dimension's limits (any dimension) and stride (record dimension only) in hyperslab\n");
    else (void)fprintf(stdout,"-d, --dmn, --dimension dim,[min][,[max]] Dimension's limits in hyperslab\n");
  } /* end if -d */
  if(strstr(opt_sng,"-F")) (void)fprintf(stdout,"-F, --ftn, --fortran\tFortran indexing conventions (1-based) for I/O\n");
  if(strstr(opt_sng,"-f")) (void)fprintf(stdout,"-f, --fnc_tbl, --prn_fnc_tbl\tPrint function table\n");
  if(strstr(opt_sng,"-H")){
    if(prg_lcl == ncks) (void)fprintf(stdout,"-H, --prn, --print\tPrint data\n");
    if(nco_is_mlt_fl_opr(prg_lcl)) (void)fprintf(stdout,"-H, --fl_lst_in, --file_list\tDo not create \"input_file_list\" global attribute\n");
  } /* end if -H */
  if(strstr(opt_sng,"-h")) (void)fprintf(stdout,"-h, --hst, --history\tDo not append to \"history\" global attribute\n");
  if(strstr(opt_sng,"-i")) (void)fprintf(stdout,"-i, --ntp, --interpolate var,val\tInterpolant and value\n");
  if(strstr(opt_sng,"-I")) (void)fprintf(stdout,"-I, --wgt_msk_crd_var \tDo not weight or mask coordinate variables\n");
  if(strstr(opt_sng,"-l")) (void)fprintf(stdout,"-l, --lcl, --local path\tLocal storage path for remotely-retrieved files\n");
  if(strstr(opt_sng,"-M")){
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-M, --msk_val, --mask-value, --mask_value mask_val\tMasking value (default is 1.0)\n");
    if(prg_lcl == ncks) (void)fprintf(stdout,"-M, --Mtd, --Metadata\tPrint global metadata\n");
  } /* end if */
  if(strstr(opt_sng,"-m")){
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-m, --msk_nm, --msk_var, --mask-variable, --mask_variable mask_var\tMasking variable name\n");
    if(prg_lcl == ncks) (void)fprintf(stdout,"-m, --mtd, --metadata\tPrint variable metadata\n");
  } /* end if */
  if(strstr(opt_sng,"-N")) (void)fprintf(stdout,"-N, --nmr, --numerator\tNo normalization\n");
  if(strstr(opt_sng,"-n")){
    /*    if(prg_lcl == ncwa) (void)fprintf(stdout,"-n\t\tNormalize by tally but not weight\n");*/
    if(prg_lcl != ncwa) (void)fprintf(stdout,"-n, --nintap nbr_files,[nbr_numeric_chars[,increment]] NINTAP-style abbreviation of file list\n");
  } /* end if */
  if(strstr(opt_sng,"-o")) (void)fprintf(stdout,"-o, --output, --fl_out fl_out\tOutput file name (or use last positional argument)\n");
  if(strstr(opt_sng,"-O")) (void)fprintf(stdout,"-O, --ovr, --overwrite\tOverwrite existing output file, if any\n");
  if(strstr(opt_sng,"-p")) (void)fprintf(stdout,"-p, --pth, --path path\tPath prefix for all input filenames\n");
  if(strstr(opt_sng,"-q")) (void)fprintf(stdout,"-q, --quiet\t\tToggle printing of dimension indices and coordinate values\n");
  if(strstr(opt_sng,"-R")) (void)fprintf(stdout,"-R, --rtn, --retain\tRetain remotely-retrieved files after use\n");
  if(strstr(opt_sng,"-r")) (void)fprintf(stdout,"-r, --revision, --vrs, --version\tProgram version and copyright notice\n");
  if(strstr(opt_sng,"-s")){
    if(prg_lcl != ncap) (void)fprintf(stdout,"-s, --sng, --string, --fmt, --format format\tString format for text output\n");
    if(prg_lcl == ncap) (void)fprintf(stdout,"-s, --spt, --script algebra\tAlgebraic command defining single output variable\n");
  } /* end if */
  if(strstr(opt_sng,"-S")) (void)fprintf(stdout,"-S, --fl_spt, --script-file fl.nco\tScript file containing multiple algebraic commands\n");
  if(strstr(opt_sng,"-t")) (void)fprintf(stdout,"-t, --truth_condition, --msk_cmp_typ, --op_rlt condition\tTruth condition for masking: eq,ne,ge,le,gt,lt\n");
  if(strstr(opt_sng,"-u")) (void)fprintf(stdout,"-u, --units\t\tUnits of variables, if any, will be printed\n");
  if(strstr(opt_sng,"-v")){
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-v, --variable old_var,new_var Variable's old and new names\n");
	if(prg_lcl == ncap) (void)fprintf(stdout,"-v, --variable \t\tOutput file includes ONLY user-defined variables\n");
    if(prg_lcl != ncrename && prg_lcl != ncap) (void)fprintf(stdout,"-v, --variable var1[,var2[...]] Variable(s) to process (regular expressions supported)\n");
  } /* end if */
  /*  if(strstr(opt_sng,"-W")) (void)fprintf(stdout,"-W\t\tNormalize by weight but not tally\n");*/
  if(strstr(opt_sng,"-w")){
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-w, --wgt_var, --weight wgt\tWeighting variable name\n");
    if(prg_lcl == ncflint) (void)fprintf(stdout,"-w, --wgt_var, --weight wgt_1[,wgt_2] Weight(s) of file(s)\n");
  } /* end if */
  if(strstr(opt_sng,"-x")) (void)fprintf(stdout,"-x, --xcl, --exclude\tExtract all variables EXCEPT those specified with -v\n");
  if(strstr(opt_sng,"-y")){
    if(prg_lcl == ncbo)(void)fprintf(stdout,"-y, --op_typ, --operation op_typ\tBinary arithmetic operation: add,sbt,mlt,dvd (+,-,*,/)\n");
    if(prg_lcl == ncra || prg_lcl == ncea || prg_lcl == ncwa)(void)fprintf(stdout,"-y, --op_typ, --operation op_typ\tArithmetic operation: avg,min,max,ttl,sqravg,avgsqr,sqrt,rms,rmssdn\n");
  }
  if(strstr(opt_sng,"in.nc")) (void)fprintf(stdout,"in.nc\t\t\tInput file name(s)\n");
  if(strstr(opt_sng,"[out.nc]")) (void)fprintf(stdout,"[out.nc]\t\tOutput file name (or use -o switch)\n");
/*  if(strstr(opt_sng,"-")) (void)fprintf(stdout,"-\n");*/

  /* Free the space holding the string */
  opt_sng=(char *)nco_free(opt_sng);

} /* end nco_usg_prn() */

