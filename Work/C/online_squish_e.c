
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libpq-fe.h>
#include <meos.h>
/* The expandable functions are in the internal MEOS API */
#include <meos_internal.h>

/* Number of instants to send in batch to the database */
#define NO_INSTANTS_BATCH 1000
/* Number of instants to keep when restarting a sequence, should keep at least one */
#define NO_INSTANTS_KEEP 2
/* Maximum length in characters of a header record in the input CSV file */
#define MAX_LENGTH_HEADER 1024
/* Maximum length in characters of a point in the input data */
#define MAX_LENGTH_POINT 64
/* Maximum number of trips */
#define MAX_TRIPS 5

typedef struct
{
  Timestamp T;
  long int MMSI;
  double Latitude;
  double Longitude;
  double SOG;
} AIS_record;



typedef struct
{
  long int MMSI;   /* Identifier of the trip */
  TSequence *trip; /* Latest observations of the trip */
} trip_record;
typedef struct PriorityQueueElem
{
  void * point;
  double priority;
  int index; //reference his own index
} PriorityQueueElem;

typedef void* IDict;

typedef struct PriorityQueue
{
  PriorityQueueElem **arr;
  IDict dict;
  size_t size;
  size_t capacity;
} PriorityQueue;

struct PriorityQueue *
create_PriorityQueue(int capacity);
size_t size_queue(const PriorityQueue *Q);
void minHeapify(PriorityQueue* Q, int index);
PriorityQueueElem *remove_min(PriorityQueue *Q);
struct PriorityQueueElem *
replace_elem(void *p_i,double priority ,PriorityQueue *Q);
void insertHelper(PriorityQueue* Q, int index);
void
push(PriorityQueueElem * insert,PriorityQueue *Q);
void
set_priority_queue(void *p_i,double priority ,PriorityQueue *Q);
void destroy_Queue(struct PriorityQueue *Q);
typedef struct PointDict
{
    void *key;
    void *value;
} PointDict;


typedef void* Dict;

typedef struct PriorityDict
{
  void * key;
  double priority;
} PriorityDict;


typedef void* PDict;

typedef struct
{
  uint32_t i;
  long int MMSI;

  size_t beta;
  Dict succ;
  Dict pred;
  PDict  *p;
  PriorityQueue *Q;

  TInstant * p_i;
  TInstant * p_j;
  TInstant * point;

} squish_variables;

void
init_squish_variables(squish_variables *sq);
void
free_squish_variables(squish_variables *sq);
void
sq_iteration(TInstant * point,squish_variables *sq ,const double lambda, bool syncdist,
                                                      uint32_t minpts);

TSequence *
construct_simplify_path(squish_variables *sq,
bool lower_inc,bool upper_inc, interpType interp, bool normalize);


/**
 * @brief Function that sends a SQL query to the database
 * @returns 0 if OK -1 on error
 */
static int
exec_sql(PGconn *conn, const char *sql, ExecStatusType status)
{
  int result = 0;
  PGresult *res = PQexec(conn, sql);
  if (PQresultStatus(res) != status)
  {
    fprintf(stderr, "SQL command failed:\n%s %s", sql, PQerrorMessage(conn));
    result = -1;
  }
  PQclear(res);
  return result;
}

/* Main program */
int
main(int argc, char **argv)
{
  const char *conninfo;
  PGconn *conn;
  int res_sql;
  AIS_record rec;
  int no_records = 0;
  int no_nulls = 0;
  char point_buffer[MAX_LENGTH_POINT];
  char text_buffer[MAX_LENGTH_HEADER];
  /* Allocate space to build the trips */
  trip_record trips[MAX_TRIPS] = {0};
  squish_variables variables[MAX_TRIPS] = {0};
  uint32_t i_; size_t beta;
  double lambda = 1;
  bool syncdist = 1;





  /* Number of ships */
  int no_ships = 0;
  /* Number of writes */
  int no_writes = 0;
  /* Iterator variables */
  int i, j;
  /* Exit value initialized to 1 (i.e., error) to quickly exit upon error */
  int exit_value = 1;

  /* Get start time */
  clock_t t;
  t = clock();

  /***************************************************************************
   * Section 1: Connexion to the database
   ***************************************************************************/

  /*
   * If the user supplies a parameter on the command line, use it as the
   * conninfo string; otherwise default to setting dbname=postgres and using
   * environment variables or defaults for all other connection parameters.
   */
  if (argc > 1)
    conninfo = argv[1];
  else
    conninfo = "host=localhost user=postgres dbname=brussels password=1234";

  /* Make a connection to the database */
  conn = PQconnectdb(conninfo);

  /* Check to see that the backend connection was successfully made */
  if (PQstatus(conn) != CONNECTION_OK)
  {
    fprintf(stderr, "%s", PQerrorMessage(conn));
    goto cleanup;
  }

  /* Set always-secure search path, so malicious users can't take control. */
  res_sql = exec_sql(conn, "SELECT pg_catalog.set_config('search_path', '', false)",
      PGRES_TUPLES_OK);
  if (res_sql < 0)
    goto cleanup;

  /* Create the table that will hold the data*/
  printf("Creating the table AISTripsSQ in the database\n");
  res_sql = exec_sql(conn, "DROP TABLE IF EXISTS public.AISTripsSQ;",
      PGRES_COMMAND_OK);
  if (res_sql < 0)
    goto cleanup;
  res_sql = exec_sql(conn, "CREATE TABLE public.AISTripsSQ("
    "MMSI integer,lambda float,buffer_size int, trip public.tgeompoint,PRIMARY KEY (MMSI, lambda,buffer_size));", PGRES_COMMAND_OK);
  if (res_sql < 0)
    goto cleanup;

  /***************************************************************************
   * Section 2: Initialize MEOS and open the input AIS file
   ***************************************************************************/

  /* Initialize MEOS */
  meos_initialize(NULL, NULL);

  /* You may substitute the full file path in the first argument of fopen */
  FILE *file = fopen("../../Data/ais_instants.csv", "r");

  if (! file)
  {
    printf("Error opening input file\n");
    goto cleanup;
  }

  /***************************************************************************
   * Section 3: Read input file line by line and append each observation as a
   * temporal point in MEOS
   ***************************************************************************/

  printf("Accumulating %d instants before sending them to the database\n"
    "(one '*' marker every database update)\n", NO_INSTANTS_BATCH);

  /* Read the first line of the file with the headers */
  fscanf(file, "%1023s\n", text_buffer);


  printf("BEGIN LOOP ///\n");
  /* Continue reading the file */
  do
  {

    //printf("BEGIN / Iteration\n");
    int read = fscanf(file, "%32[^,],%ld,%lf,%lf,%lf\n",
      text_buffer, &rec.MMSI, &rec.Latitude, &rec.Longitude, &rec.SOG);
    /* Transform the string representing the timestamp into a timestamp value */
    rec.T = pg_timestamp_in(text_buffer, -1);

    if (read == 5)
      no_records++;

    if (read != 5 && ! feof(file))
    {
      printf("Record with missing values ignored\n");
      no_nulls++;
    }

    if (ferror(file))
    {
      printf("Error reading input file\n");
      fclose(file);
      goto cleanup;
    }

    /* Find the place to store the new instant */
    j = -1;
    for (i = 0; i < MAX_TRIPS; i++)
    {
      if (variables[i].MMSI == rec.MMSI)
      {
        j = i;
        break;
      }
    }
    if (j < 0)
    {
      j = no_ships++;
      if (j == MAX_TRIPS)
      {
        printf("The maximum number of ships in the input file is bigger than %d",
          MAX_TRIPS);
        fclose(file);
        goto cleanup;
      }
      variables[j].MMSI = rec.MMSI;
      init_squish_variables(& variables[j]);

    }

    /*
     * Append the latest observation to the corresponding ship.
     * In the input file it is assumed that
     * - The coordinates are given in the WGS84 geographic coordinate system
     * - The timestamps are given in GMT time zone
     */
    char *t_out = pg_timestamp_out(rec.T);
    sprintf(point_buffer, "SRID=4326;Point(%lf %lf)@%s+00", rec.Longitude,
      rec.Latitude, t_out);


    /* Send the trip to the database when its size reaches the maximum size */
    // This code caused a segmentation fault search for it
    if (variables[j].Q && size_queue(variables[j].Q) == NO_INSTANTS_BATCH)
    {
      /* Construct the query to be sent to the database */
        printf("BEGIN WRITING / i_ %i for beta %i MMSI %i / SIZE QUEUE %i \n",variables[j].i,variables[j].beta,variables[j].MMSI,size_queue(variables[j].Q));
      TSequence * trip = construct_simplify_path(&variables[j],true,true, LINEAR, false);
      char *temp_out = tsequence_out(trip, 15);
      char *query_buffer = malloc(sizeof(char) * (strlen(temp_out) + 256));
      sprintf(query_buffer, "INSERT INTO public.AISTripsSQ(MMSI,lambda,buffer_size, trip) "
        "VALUES (%ld,%f,%i, '%s') ON CONFLICT (MMSI,lambda,buffer_size) DO "
        "UPDATE SET trip = public.update(AISTripsSQ.trip, EXCLUDED.trip, true);",
        variables[j].MMSI,lambda,NO_INSTANTS_BATCH , temp_out);

      res_sql = exec_sql(conn, query_buffer, PGRES_COMMAND_OK);
      if (res_sql < 0)
        goto cleanup;

      /* Free memory */
      free(temp_out);
      free(query_buffer);
      TInstant *point = variables[j].p_i;
      TInstant *point_j = variables[j].p_j;
      free_squish_variables(&variables[j]);
      init_squish_variables(&variables[j]);
        //printf("END WRITING / i_ %i for beta %i MMSI %i / SIZE QUEUE %i \n",variables[j].i,variables[j].beta,variables[j].MMSI,size_queue(variables[j].Q));
      sq_iteration( point_j ,&variables[j], lambda,  syncdist,NO_INSTANTS_KEEP);
      sq_iteration( point ,&variables[j], lambda,  syncdist,NO_INSTANTS_KEEP);

      no_writes++;
      printf("*");
      fflush(stdout);
      /* Restart the sequence by only keeping the last instants */

      //tsequence_restart(trips[j].trip, NO_INSTANTS_KEEP);

    }

    /* Append the last observation */
    TInstant *inst = (TInstant *) tgeogpoint_in(point_buffer);
    sq_iteration( inst ,&variables[j], lambda,  syncdist,NO_INSTANTS_KEEP);

    //printf("END / Iteration %i for MMSI %i \n",variables[j].i,variables[j].MMSI);


  } while (! feof(file));
  /* Close the file */
  fclose(file);


    for (j = 0; j < MAX_TRIPS; j++)
    {
        if (variables[j].Q && size_queue(variables[j].Q) > 0){
            /* Construct the query to be sent to the database */
              TSequence * trip = construct_simplify_path(&variables[j],true,true, LINEAR, false);
              char *temp_out = tsequence_out(trip, 15);
              char *query_buffer = malloc(sizeof(char) * (strlen(temp_out) + 256));

              sprintf(query_buffer, "INSERT INTO public.AISTripsSQ(MMSI,lambda,buffer_size, trip) "
                "VALUES (%ld,%f,%i, '%s') ON CONFLICT (MMSI,lambda,buffer_size) DO "
                "UPDATE SET trip = public.update(AISTripsSQ.trip, EXCLUDED.trip, true);",
                variables[j].MMSI,lambda,NO_INSTANTS_BATCH , temp_out);

              res_sql = exec_sql(conn, query_buffer, PGRES_COMMAND_OK);
              if (res_sql < 0)
                goto cleanup;

              /* Free memory */
              free(temp_out);
              free(query_buffer);
              free_squish_variables(&variables[j]);
              no_writes++;
              printf("*");
              fflush(stdout);

        }
    }

  printf("\n%d records read\n%d incomplete records ignored\n"
    "%d writes to the database\n", no_records, no_nulls, no_writes);



  sprintf(text_buffer,
    "SELECT MMSI, public.numInstants(trip) FROM public.AISTripsSQ;");
  PGresult *res = PQexec(conn, text_buffer);
  if (PQresultStatus(res) != PGRES_TUPLES_OK)
  {
    fprintf(stderr, "SQL command failed:\n%s %s", text_buffer,
      PQerrorMessage(conn));
    PQclear(res);
    goto cleanup;
  }

  int numrows = PQntuples(res);
  printf("Result of the query '%s'\n\n", text_buffer);
  printf("   mmsi    | numinstants\n");
  printf("-----------+-------------\n");
  for (i = 0; i < numrows; i++)
    printf(" %s |     %s\n", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));

  /* State that the program executed successfully */
  exit_value = 0;

  /* Calculate the elapsed time */
  t = clock() - t;
  double time_taken = ((double) t) / CLOCKS_PER_SEC;
  printf("The program took %f seconds to execute\n", time_taken);

/* Clean up */
cleanup:

 /* Free memory */
  for (i = 0; i < no_ships; i++)
    free(trips[i].trip);

  /* Finalize MEOS */
  meos_finalize();

  /* Close the connection to the database and cleanup */
  PQfinish(conn);

  return exit_value;
}
