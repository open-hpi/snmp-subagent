/*
 * 
  gcc test-time.c  `net-snmp-config --netsnmp-libs` -o test -ggdb
  snmpwalk -v2c -c public localhost hpi | grep -i "Time" | awk ' { print $1 } ' > v
  for a in `cat v`
  do 
    ./test $a
  done
 */

#include <stdio.h>
#include <time.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
int
convert (long long hpi_time)
{
  time_t hpi_tt = 0;
  time_t local_tt = 0;
  struct tm *hpi_tm, *local_tm;
  char time_buf[100], time_buf2[100];
  int rc;

  hpi_tt = (hpi_time / 1000000000);

  hpi_tm = gmtime (&hpi_tt);
  strftime (time_buf, 100, "%c", hpi_tm);

  local_tt = time (&local_tt);
  local_tm = gmtime (&local_tt);
  strftime (time_buf2, 100, "%c", local_tm);


  fprintf (stderr, "Time from SNMP [%s]\n", time_buf);
  fprintf (stderr, "Local time:    [%s]\n", time_buf2);

  return (strcmp (time_buf, time_buf2));
}

main (int argc, char **argv)
{

  struct snmp_session session, *ss;
  struct snmp_pdu *pdu;
  struct snmp_pdu *response;
  long long time_is;
  oid anOID[MAX_OID_LEN];
  size_t anOID_len = MAX_OID_LEN;

  struct variable_list *vars;
  int status;
  int count = 1;
  int rc = -1;


  /*
   * Initialize the SNMP library
   */
  init_snmp ("snmpapp");

  /*
   * Initialize a "session" that defines who we're going to talk to
   */
  snmp_sess_init (&session);	/* set up defaults */
  session.peername = strdup ("127.0.0.1");

  /* set up the authentication parameters for talking to the server */


  /* set the SNMP version number */
  session.version = SNMP_VERSION_2c;

  /* set the SNMPv1 community name used for authentication */
  session.community = "public";
  session.community_len = strlen (session.community);


  /*
   * Open the session
   */
  SOCK_STARTUP;
  ss = snmp_open (&session);	/* establish the session */

  if (!ss)
    {
      snmp_perror ("ack");
      snmp_log (LOG_ERR, "something horrible happened!!!\n");
      exit (2);
    }

  /*
   * Create the PDU for the data for our request.
   *   1) We're going to GET the system.sysDescr.0 node.
   */
  pdu = snmp_pdu_create (SNMP_MSG_GET);
  if (argc < 2)
    read_objid (".1.3.6.1.3.90.1.3.0", anOID, &anOID_len);
  else
    {
      snmp_log (LOG_INFO, "GET for %s\n", argv[1]);
      read_objid (argv[1], anOID, &anOID_len);
    }
  snmp_add_null_var (pdu, anOID, anOID_len);

  /*
   * Send the Request out.
   */
  status = snmp_synch_response (ss, pdu, &response);

  /*
   * Process the response.
   */
  if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR)
    {
      /*
       * SUCCESS: Print the result variables
       */
      for (vars = response->variables; vars; vars = vars->next_variable)
	print_variable (vars->name, vars->name_length, vars);

      /* manipuate the information ourselves */
      for (vars = response->variables; vars; vars = vars->next_variable)
	{
	  if (vars->type == ASN_COUNTER64)
	    {

	      time_is = (*vars->val.counter64).high;
	      time_is = time_is << 32;
	      time_is += (*vars->val.counter64).low;

	      rc = convert (time_is);
	      if (rc != 0)
		fprintf (stderr, "+++ test failed\n");
	      else
		fprintf (stderr, "---- test ok\n");
	    }
	}
      /*
       * FAILURE: print what went wrong!
       */
    }
  else
    {
      if (status == STAT_SUCCESS)
	fprintf (stderr, "Error in packet\nReason: %s\n",
		 snmp_errstring (response->errstat));
      else
	snmp_sess_perror ("snmpget", ss);

    }
  /*
   * Clean up:
   *  1) free the response.
   *  2) close the session.
   */
  if (response)
    snmp_free_pdu (response);
  snmp_close (ss);

  SOCK_CLEANUP;
  return (rc);
}				/* main() */
