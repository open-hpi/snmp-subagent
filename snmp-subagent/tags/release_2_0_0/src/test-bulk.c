/*
 * 
  gcc  test-bulk.c  `net-snmp-config --netsnmp-libs` -o test -ggdb
  gcc  test-bulk.c  `net-snmp-config --netsnmp-libs --libdir --base-cflags  --cflags --libs --netsnmp-libs --ldflags --external-libs ` -lcrypto -o test -ggdb
 */
 
 /*

    1. use the EntryCount to calcuclate number of GETBULKS
    2. validate the count caluele the OID has changed to the next oid 
	    if not restart GOTO 1
	    if yes GETBULK until end.

!!!!!!!!!!!!!!THIS IS THE BEGINNING OF A NEW ENTRY !!!!!!!!!!!!!!!!!!
iso.3.6.1.3.90.1.4.1.2.0.2.2 = Gauge32: 2
David M Judkovics: iso.3.6.1.3.90.1.4.1.2
David M Judkovics: iso.3.6.1.3.90.1.4.1.1
Konrad Rzeszutek:  ::= {saHpiEntry 1}
Konrad Rzeszutek:   ::= {saHpiEntry 2}
Konrad Rzeszutek:   read_objid ("saHpiDomainID", anOID, &anOID_len);
Konrad Rzeszutek:  pdu->max_repetitions = 3;
Konrad Rzeszutek: .. do the get ...
Konrad Rzeszutek: .. and then ..
Konrad Rzeszutek:  read_objid ("saHpiResourceID", anOID, &anOID_len);
Konrad Rzeszutek: .. and so forth ..
Konrad Rzeszutek: memmove (anOID, vars->name,
                           vars->name_length * sizeof (oid));
                  anOID_len = vars->name_length;
Konrad Rzeszutek:  anOID_len = vars->name_length-3;
Konrad Rzeszutek:    pdu = snmp_pdu_create (SNMP_MSG_GETBULK);
      snmp_add_null_var (pdu, anOID, anOID_len);

*/

#include <stdio.h>
#include <time.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "/home/davej/dev/openhpi/openhpi/include/SaHpi.h"

#define SA_HPI_ENTRY 			"HPI-MIB::saHpiEntry"
//#define SA_HPI_ENTRY_COUNT      	".1.3.6.1.3.90.1.1.0"
#define SA_HPI_ENTRY_COUNT      	"HPI-MIB::saHpiEntryCount.0"
#define SA_HPI_RDR_COUNT      	".1.3.6.1.3.90.3.1.0"
//#define SA_HPI_RDR_COUNT      		"HPI-MIB::saHpiRdrCount.0"


#define SA_HPI_DOMAIN_ID				".1.3.6.1.3.90.1.4.1.1"
#define SA_HPI_ENTRY_ID					".1.3.6.1.3.90.1.4.1.2"
#define SA_HPI_RESOURCE_ID				".1.3.6.1.3.90.1.4.1.3"
#define SA_HPI_RESOURCE_ENTITY_PATH			".1.3.6.1.3.90.1.4.1.4"
#define SA_HPI_RESOURCE_CAPABILITIES			".1.3.6.1.3.90.1.4.1.5"
#define SA_HPI_RESOURCE_SEVERITY			".1.3.6.1.3.90.1.4.1.6"
#define SA_HPI_RESOURCE_INFO_RESOURCE_REV		".1.3.6.1.3.90.1.4.1.7"
#define SA_HPI_DOMAIN_RESOURCE_INFO_SPECIFIC_VER	".1.3.6.1.3.90.1.4.1.8"
#define SA_HPI_DOMAIN_RESOURCE_INFO_DEVICE_SUPPORT	".1.3.6.1.3.90.1.4.1.9"
#define SA_HPI_DOMAIN_RESOURCE_INFO_MANUFACTUER_ID	".1.3.6.1.3.90.1.4.1.10"
#define SA_HPI_DOMAIN_RESOURCE_INFO_PRODUCT_ID		".1.3.6.1.3.90.1.4.1.11"
#define SA_HPI_DOMAIN_RESOURCE_INFO_FIRMWARE_MAJOR_REV	".1.3.6.1.3.90.1.4.1.12"
#define SA_HPI_DOMAIN_RESOURCE_INFO_FIRMWARE_MINOR_REV	".1.3.6.1.3.90.1.4.1.13"
#define SA_HPI_DOMAIN_RESOURCE_INFO_AUX_FIRMWARE_REV	".1.3.6.1.3.90.1.4.1.14"
#define SA_HPI_DOMAIN_RESOURCE_TAG_TEXT_TYPE		".1.3.6.1.3.90.1.4.1.15"
#define SA_HPI_DOMAIN_RESOURCE_TAG_TEXT_LANGUAGE	".1.3.6.1.3.90.1.4.1.16"
#define SA_HPI_DOMAIN_RESOURCE_TAG			".1.3.6.1.3.90.1.4.1.17"
#define SA_HPI_DOMAIN_PARAM_CONTROL			".1.3.6.1.3.90.1.4.1.18"
#define SA_HPI_DOMAIN_CLEAR_EVENTS			".1.3.6.1.3.90.1.4.1.19"
#define SA_HPI_DOMAIN_EVENT_LOG_TIME			".1.3.6.1.3.90.1.4.1.20"
#define SA_HPI_DOMAIN_EVENT_LOG_STATE    		".1.3.6.1.3.90.1.4.1.21"

#define NUM_REPITIONS	240



#define MAX_ASN_STR_LEN 256

/* Place-holder for values set and returned by snmp */
struct snmp_value {
        u_char type;
        char string[MAX_ASN_STR_LEN];
        long integer;
};

int snmp_get( struct snmp_session *ss, const char *objid, struct snmp_value *value) 
{
        struct snmp_pdu *pdu;
        struct snmp_pdu *response;
        
        oid anOID[MAX_OID_LEN];
        size_t anOID_len = MAX_OID_LEN;
        struct variable_list *vars;
        int status;

	memset(value, 0, sizeof(*value));
        
        /*
         * Create the PDU for the data for our request.
         */
        pdu = snmp_pdu_create(SNMP_MSG_GET);
        read_objid(objid, anOID, &anOID_len);
        snmp_add_null_var(pdu, anOID, anOID_len);
        /*
         * Send the Request out.
         */
        status = snmp_synch_response(ss, pdu, &response);
        /*
         * Process the response.
         */
        if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
                vars = response->variables;
                value->type = vars->type;
                if (vars->next_variable != NULL) {
                        value->type = ASN_NULL;
                }/* If there are more values, set return type to null. */
                else if ( (vars->type == ASN_INTEGER) || (vars->type == ASN_COUNTER) )  {
                        value->integer = *(vars->val.integer);
                } else {
                        unsigned int str_len = vars->val_len;
                        strncpy(value->string,
                                (char *)vars->val.string,
                                MAX_ASN_STR_LEN);
                        if (str_len < MAX_ASN_STR_LEN) value->string[str_len] = '\0';
                        else value->string[MAX_ASN_STR_LEN-1] = '\0';
                }
        } else {
                value->type = (u_char)0x00; /* Set return type to 0 in case of error. */
                if (status == STAT_SUCCESS)
                        fprintf(stderr, "Error in packet %s\nReason: %s\n",
                                objid, snmp_errstring(response->errstat));
                else
                        snmp_sess_perror("snmpget", ss);
        }

        /* Clean up: free the response */
        if (response) snmp_free_pdu(response);

        return value->type? 0 : -1;
}

int snmp_get_bulk( struct snmp_session *ss, 
		   const char *bulk_objid, 
		   struct snmp_pdu *bulk_pdu, 
		   struct snmp_pdu **bulk_response )
{
	size_t anOID_len = MAX_OID_LEN;
	oid anOID[MAX_OID_LEN];
	int status;

	int count = 0;
	static c = 0;


	/* Create the PDU for theenrty_count data for our request. */
	read_objid(bulk_objid, anOID, &anOID_len);

	bulk_pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
 	
	bulk_pdu->non_repeaters = 0; 
	
	bulk_pdu->max_repetitions = NUM_REPITIONS;
	
	snmp_add_null_var(bulk_pdu, anOID, anOID_len);
	
	/* Send the Request out.*/
	status = snmp_synch_response(ss, bulk_pdu, bulk_response);

	return(status);

}


get_sahpi_table_entries( struct snmp_session *ss, 
		        const char *objid, 
		        struct snmp_value *value, 
		        int num_entries )
{
	struct snmp_pdu *pdu;
	struct snmp_pdu *response;
	int status;
	struct variable_list *vars;

	int i;
	int count=0;
	static c = 0;

	SaHpiRptEntryT *rpt_cache = NULL;
		    
	rpt_cache = malloc(num_entries * sizeof(*rpt_cache));
	memset( rpt_cache, 0, (num_entries * sizeof(*rpt_cache)) );

	status = snmp_get_bulk(ss, objid, pdu, &response);
	
	/* Process the response */
	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
	/* SUCCESS: Print the result variables */  

		/* Get the data from the response */
		vars = response->variables;
		
		/* SA_HPI_DOMAIN_ID */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].DomainId = *vars->val.integer;
			else
				printf("SA_HPI_DOMAIN_ID:something terrible has happened\n");
			vars = vars->next_variable;
		
		}						   
		
		
		/* SA_HPI_ENTRY_ID */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].EntryId = *vars->val.integer; 
			else
				printf("SA_HPI_ENTRY_ID:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		
		/* SA_HPI_RESOURCE_ID */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].ResourceId = *vars->val.integer; 
			else
				printf("SA_HPI_RESOURCE_ID:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_RESOURCE_ENTITY_PATH */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_OCTET_STR)  {
		//TODO       	if(string2entitypath(vars->val.string, &rpt_cache[i].ResourceEntity))
		//TODO			dbg("something terrible happened with SA_HPI_RESOURCE_ENTITY_PATH");
			}
			else
				printf("SA_HPI_RESOURCE_ENTITY_PATH:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_RESOURCE_CAPABILITIES */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].ResourceCapabilities = *vars->val.integer; 
			else
				printf("SA_HPI_RESOURCE_CAPABILITIES:something terrible has happened\n");
			vars = vars->next_variable;
		
		} 
		/* SA_HPI_RESOURCE_SEVERITY */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_INTEGER)
				rpt_cache[i].ResourceSeverity = *vars->val.integer; 
			else
				printf("SA_HPI_RESOURCE_SEVERITY:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_RESOURCE_INFO_RESOURCE_REV */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].ResourceInfo.ResourceRev = *vars->val.integer; 
			else
				printf("SA_HPI_RESOURCE_INFO_RESOURCE_REV:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_DOMAIN_RESOURCE_INFO_SPECIFIC_VER */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].ResourceInfo.SpecificVer = *vars->val.integer; 
			else
				printf("SA_HPI_DOMAIN_RESOURCE_INFO_SPECIFIC_VER:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_DOMAIN_RESOURCE_INFO_DEVICE_SUPPORT */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].ResourceInfo.DeviceSupport = *vars->val.integer; 
			else
				printf("SA_HPI_DOMAIN_RESOURCE_INFO_DEVICE_SUPPORT:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_DOMAIN_RESOURCE_INFO_MANUFACTUER_ID */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].ResourceInfo.ManufacturerId = *vars->val.integer; 
			else
				printf("SA_HPI_DOMAIN_RESOURCE_INFO_MANUFACTUER_ID:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_DOMAIN_RESOURCE_INFO_PRODUCT_ID */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].ResourceInfo.ProductId = *vars->val.integer; 
			else
				printf("SA_HPI_DOMAIN_RESOURCE_INFO_PRODUCT_ID:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_DOMAIN_RESOURCE_INFO_FIRMWARE_MAJOR_REV */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].ResourceInfo.FirmwareMajorRev = *vars->val.integer; 
			else
				printf("SA_HPI_DOMAIN_RESOURCE_INFO_FIRMWARE_MAJOR_REV:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_DOMAIN_RESOURCE_INFO_FIRMWARE_MINOR_REV */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].ResourceInfo.FirmwareMinorRev = *vars->val.integer; 
			else
				printf("SA_HPI_DOMAIN_RESOURCE_INFO_FIRMWARE_MINOR_REV:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_DOMAIN_RESOURCE_INFO_AUX_FIRMWARE_REV */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_UNSIGNED)
				rpt_cache[i].ResourceInfo.AuxFirmwareRev = *vars->val.integer; 
			else
				printf("SA_HPI_DOMAIN_RESOURCE_INFO_AUX_FIRMWARE_REV:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_DOMAIN_RESOURCE_TAG_TEXT_TYPE */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_INTEGER)
				rpt_cache[i].ResourceTag.DataType = *vars->val.integer; 
			else
				printf("SA_HPI_DOMAIN_RESOURCE_TAG_TEXT_TYPE:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_DOMAIN_RESOURCE_TAG_TEXT_LANGUAGE */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_INTEGER)
				rpt_cache[i].ResourceTag.Language = *vars->val.integer; 
			else
				printf("SA_HPI_DOMAIN_RESOURCE_TAG_TEXT_LANGUAGE:something terrible has happened\n");
			vars = vars->next_variable;
		
		}
		/* SA_HPI_DOMAIN_RESOURCE_TAG */
		for (i = 0; i < num_entries; i++) {
			if (vars->type == ASN_OCTET_STR) {
				memcpy(&rpt_cache[i].ResourceTag.Data, vars->val.string, vars->val_len);
				rpt_cache[i].ResourceTag.DataLength = vars->val_len;
			}
			else
				printf("SA_HPI_DOMAIN_RESOURCE_TAG:something terrible has happened\n");
			vars = vars->next_variable;
		
		}

		for(vars = response->variables; vars; vars = vars->next_variable) {

			c++;
			printf("\n**** oid count %d ******\n", c);

			if ((vars->type != SNMP_ENDOFMIBVIEW) &&
			    (vars->type != SNMP_NOSUCHOBJECT) &&
			    (vars->type != SNMP_NOSUCHINSTANCE)) {
				if ( vars->name[vars->name_length - 4] != 1 ) {
					// We only want the indexes, not the rest of the
					// column vales.
					printf("!! THIS IS THE BEGINNING OF A NEW ENTRY !!!\n");
//		        		break;
				}
				
				if (vars->type == ASN_UNSIGNED) {
					char *sp = (char *)malloc(1 + vars->val_len);
					memcpy(sp, vars->val.integer, vars->val_len);
					sp[vars->val_len] = '\0';
					printf("value #%d is an Interger: %d\n", count++, sp);
					free(sp);
				}

				if (vars->type == ASN_OCTET_STR) {
					char *sp = (char *)malloc(1 + vars->val_len);
					memcpy(sp, vars->val.string, vars->val_len);
					sp[vars->val_len] = '\0';
					printf("value #%d is a string: %s\n", count++, sp);
					free(sp);
				}

				print_variable(vars->name, vars->name_length, vars);  

			}  else 
				fprintf(stderr,"No idea.\n");
		} 
		/* FAILURE: print what went wrong! */
	} else { 
		if (status == STAT_SUCCESS)
			fprintf(stderr, "Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
		else
			snmp_sess_perror("snmpget", ss);
	}

	/* Clean up: free the response. */
	if (response)
		snmp_free_pdu(response);
}


get_sahpi_rdr_table( struct snmp_session *ss, 
		        const char *objid, 
		        struct snmp_value *value, 
		        int num_entries )
{
	struct snmp_pdu *pdu;
	struct snmp_pdu *response;
	int status;
	struct variable_list *vars;

	int i;
	int count=0;
	static c = 0;

	SaHpiRptEntryT *rpt_cache = NULL;
		    
	rpt_cache = malloc(num_entries * sizeof(*rpt_cache));
	memset( rpt_cache, 0, (num_entries * sizeof(*rpt_cache)) );

	status = snmp_get_bulk(ss, objid, pdu, &response);

	/* Process the response */
	if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
	/* SUCCESS: Print the result variables */  

		for(vars = response->variables; vars; vars = vars->next_variable) {
	
			c++;
			printf("\n**** oid count %d ******\n", c);
	
			if ((vars->type != SNMP_ENDOFMIBVIEW) &&
			    (vars->type != SNMP_NOSUCHOBJECT) &&
			    (vars->type != SNMP_NOSUCHINSTANCE)) {
				if ( vars->name[vars->name_length - 4] != 1 ) {
					// We only want the indexes, not the rest of the
					// column vales.
					printf("!! THIS IS THE BEGINNING OF A NEW ENTRY !!!\n");
	//		        		break;
				}
	
				if (vars->type == ASN_UNSIGNED) {
					char *sp = (char *)malloc(1 + vars->val_len);
					memcpy(sp, vars->val.integer, vars->val_len);
					sp[vars->val_len] = '\0';
					printf("value #%d is an Interger: %d\n", count++, sp);
					free(sp);
				}
	
				if (vars->type == ASN_OCTET_STR) {
					char *sp = (char *)malloc(1 + vars->val_len);
					memcpy(sp, vars->val.string, vars->val_len);
					sp[vars->val_len] = '\0';
					printf("value #%d is a string: %s\n", count++, sp);
					free(sp);
				}
	
				print_variable(vars->name, vars->name_length, vars);  
	
			}  else 
				fprintf(stderr,"No idea.\n");
		} 
	/* FAILURE: print what went wrong! */
	} else { 
		if (status == STAT_SUCCESS)
		fprintf(stderr, "Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
		else
			snmp_sess_perror("snmpget", ss);
	}


	/* Clean up: free the response. */
	if (response)
		snmp_free_pdu(response);

}

main (int argc, char **argv){

    struct snmp_session session, *ss;

    int rc = -1;
    int running = 1; 

    int entry_count=0;

    struct snmp_value value;


    /*
     * Initialize the SNMP library
     */
    init_snmp("snmpapp");

    /*
     * Initialize a "session" that defines who we're going to talk to
     */
    snmp_sess_init( &session );                   /* set up defaults */
    session.peername = strdup("localhost");

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


    /* Get the saHpiEntryCount */
    snmp_get(ss, SA_HPI_ENTRY_COUNT, &value);
    /* Get the saHpiEntries, these are the resources */
    get_sahpi_table_entries(ss, "HPI-MIB::saHpiEntry", &value, value.integer);

    /* Get the saHpiRdrCount */
    snmp_get(ss, SA_HPI_RDR_COUNT, &value);
    /* Get the saHpiEntries, these are the resources */
    get_sahpi_rdr_table(ss, "HPI-MIB::saHpiRdrEntry", &value, value.integer);
    
    snmp_close(ss);

    SOCK_CLEANUP;
    return (rc);
} /* main() */


