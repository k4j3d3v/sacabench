#include <stdlib.h>
#include <stdio.h>
#include <likwid.h>

int setup_likwid(int** cpus)
{
        // Load the topology module and print some values.
        int err = topology_init();
        if (err < 0)
        {
            printf("Failed to initialize LIKWID's topology module\n");
            return 1;
        }
        // CpuInfo_t contains global information like name, CPU family, ...
        CpuInfo_t info = get_cpuInfo();
        // CpuTopology_t contains information about the topology of the CPUs.
        CpuTopology_t topo = get_cpuTopology();
        // Create affinity domains. Commonly only needed when reading Uncore counters
        //affinity_init();
    
        printf("Likwid example on a %s with %d CPUs\n", info->name, topo->numHWThreads);
    
        *cpus = (int*)malloc(topo->numHWThreads * sizeof(int));

        if (!*cpus)
            return 1;
    
        for (int i=0;i<topo->numHWThreads;i++)
        {
            (*cpus)[i] = topo->threadPool[i].apicId;
        }
        // Must be called before perfmon_init() but only if you want to use another
        // access mode as the pre-configured one. For direct access (0) you have to
        // be root.
        //accessClient_setaccessmode(0);
    
        // Initialize the perfmon module.
        err = perfmon_init(topo->numHWThreads, *cpus);
        if (err < 0)
        {
            printf("Failed to initialize LIKWID's performance monitoring module\n");
            topology_finalize();
            return 1;
        }
        return 0;
}
int setup_event_set(char *estr, int &gid)
{


    // Add eventset string to the perfmon module.
    gid = perfmon_addEventSet(estr);
    if (gid < 0)
    {
        printf("Failed to add event string %s to LIKWID's performance monitoring module\n", estr);
        perfmon_finalize();
        topology_finalize();
        return 1;
    }
    
    // Setup the eventset identified by group ID (gid).
    int err = perfmon_setupCounters(gid);
    if (err < 0)
    {
        printf("Failed to setup group %d in LIKWID's performance monitoring module\n", gid);
        perfmon_finalize();
        topology_finalize();
        return 1;
    }
    return 0;
}
int start_counters(int gid)
{
        // Start all counters in the previously set up event set.
        int err = perfmon_startCounters();
        if (err < 0)
        {
            printf("Failed to start counters for group %d for thread %d\n",gid, (-1*err)-1);
            perfmon_finalize();
            topology_finalize();
            return 1;
        }
        return 0;
}
int read_counters(int gid){
       // Read and record current event counts.
       int err = perfmon_readCounters();
       if (err < 0)
       {
           printf("Failed to read counters for group %d for thread %d\n",gid, (-1*err)-1);
           perfmon_finalize();
           topology_finalize();
           return 1;
       }
       return 0;
}

void print_events(int n, int *cpus, int gid, char * enames[])
{

    CpuTopology_t topo = get_cpuTopology();

    for (int j=0; j<n; j++)
    {
       // for (int i = 0;i < topo->numHWThreads; i++)
        for (int i = 0;i < 1; i++)
        {
            double result = perfmon_getLastResult(gid, j, i);
            printf("- event set %s at CPU %d: %f\n", enames[j], cpus[i], result);
        }
    }
}
int stop_counters(int gid)
{
    // Stop all counters in the currently-active event set.
    int err = perfmon_stopCounters();
    if (err < 0)
    {
        printf("Failed to stop counters for group %d for thread %d\n",gid, (-1*err)-1);
        perfmon_finalize();
        topology_finalize();
        return 1;
    }
    return 0;

}
void  print_total_meauserements(int n, int *cpus, int gid, char * enames[])
{
    CpuTopology_t topo = get_cpuTopology();

    // Print the result of every thread/CPU for all events in estr, counting since counters first started.
    printf("Total sum measurements:\n");
    for (int j=0; j<n; j++)
    {
        for (int i = 0;i < 1; i++)
        {
            double result = perfmon_getResult(gid, j, i);
            printf("- event set %s at CPU %d: %f\n", enames[j], cpus[i], result);
        }
    }
}
void finalize_all()
{
        // Uninitialize the perfmon module.
        perfmon_finalize();
        //affinity_finalize();
        // Uninitialize the topology module.
        topology_finalize();
}