/*
 * 
  gcc  test-walk.c  `net-snmp-config --netsnmp-libs` -o test -ggdb
 */

#include <stdio.h>
#include <time.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#define TABLE "HPI-MIB::saHpiEntry"

main (int argc, char **argv){

    struct snmp_session session, *ss;
    struct snmp_pdu *pdu;
    struct snmp_pdu *response;
    oid anOID[MAX_OID_LEN];
    size_t anOID_len = MAX_OID_LEN;

    struct variable_list *vars;
    int status;
    int count=0;
    int rc = -1;
    int running = 1;
	
    /*
     * Initialize the SNMP library
     */
    init_snmp("snmpapp");

    /*
     * Initialize a "session" that defines who we're going to talk to
     */
    snmp_sess_init( &session );                   /* set up defaults */
    session.peername = strdup("ran151");

    /* set up the authentication parameters for talking to the server */

 
    /* set the SNMP version number */
    session.version = SNMP_VERSION_2c;

    /* set the SNMPv1 community name used for authentication */
    session.community = "public";
    session.community_len = strlen(session.community);


    /*
     * Open the session
     */
    SOCK_STARTUP;
    ss = snmp_open(&session);                     /* establish the session */

    if (!ss) {
        snmp_perror("ack");
        snmp_log(LOG_ERR, "something horrible happened!!!\n");
        exit(2);
    }
    /*
     * Create the PDU for the data for our request.
     */
    read_objid("HPI-MIB::saHpiEntry", anOID, &anOID_len);
    while (running) {
       pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
       snmp_add_null_var(pdu, anOID, anOID_len);
  
    /*
     * Send the Request out.
     */
       status = snmp_synch_response(ss, pdu, &response);

    /*
     * Process the response.
     */
      if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
      /*
       * SUCCESS: Print the result variables
       */
      for(vars = response->variables; vars; vars = vars->next_variable) {
	if ((vars->type != SNMP_ENDOFMIBVIEW) &&
	    (vars->type != SNMP_NOSUCHOBJECT) &&
	    (vars->type != SNMP_NOSUCHINSTANCE)) {
		//
		// The index is a three tuple. The number pre-index is the
		// column. We are going to extract the column number and see if
		// it has increased. If it has, that means we are on the next row
		// -- which is not something we want to do.
		if (vars->name[vars->name_length-4] != 1) {
			running = 0;
			continue;
		}

		// Or another way - if we have the count, we cna check by that.
		/*
		count++;		
		if (count > 3) {
			running = 0;
			continue;
		}*/
        	print_variable(vars->name, vars->name_length, vars);
		/*
		 * 10:26:54.726810 morbo.val.pok.ibm.com.33372 > ran151.val.pok.ibm.com.snmp:  GetNextRequest(28)  X:90.1.4.1 (DF)
		 * 10:26:54.729151 ran151.val.pok.ibm.com.snmp > morbo.val.pok.ibm.com.33372:  GetResponse(33)  X:90.1.4.1.1.0.1.1=0 (DF)
		 */
		// We take the response OID - 90.1.4.1.1.0.1.1 and that will be the next 
		//  OID we wil lask for.
		memmove(anOID, vars->name, vars->name_length * sizeof(oid));
		anOID_len = vars->name_length;	
        } 
	  else 	 // Quit hte loop.
	running = 0;
      } 
      /*
       * FAILURE: print what went wrong!
       */
    } else { 
      if (status == STAT_SUCCESS)
        fprintf(stderr, "Error in packet\nReason: %s\n",
                snmp_errstring(response->errstat));
      else
        snmp_sess_perror("snmpget", ss);

    }
    /*
     * Clean up:
     *  1) free the response.
     *  2) close the session.
     */
      if (response)
        snmp_free_pdu(response);

    }

    snmp_close(ss);

    SOCK_CLEANUP;
    return (rc);
} /* main() */
