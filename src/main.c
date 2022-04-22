#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <search.h>
#include <time.h>
#include <unistd.h>

static char charlist[] = "abcdefghijklmnopqrstuvwxyz-0123456789";
const int max_domain_length = 32;
const unsigned int max_domain_entry = 1000 * 10000;
const char *domain_txt = "./domains.txt";

bool search(char *sir) {
    ENTRY item;
    ENTRY *found_item;
    item.key = sir;
    if ((found_item = hsearch(item, FIND)) != NULL) {
        printf("Found %s, target is %s\n", sir, (char *)found_item->data);
        return true;
    }
    return false;
}

bool add(const char *sir, const char *data) {
    ENTRY item;
    ENTRY *ep;
    item.key = (char *)sir;
    item.data = (char *)data;
    ep = hsearch(item, ENTER);
    if (ep == NULL) {
        fprintf(stderr, "Add entry failed\n");
        return false;
    }
    return true;
}

const char *random_domain() {
    int domain_length = rand() % (max_domain_length-4-4-1);
    if(domain_length < 2) {
        domain_length = 2;
    }
    char *domain = calloc(4 + max_domain_length + 4 + 1, sizeof(char));
    char *prefix = "www.";
    
    int r = rand() % (100);
    char *suffix = r  < 67 ? ".com" : r < 34 ? ".net" : ".org";
    
    int i=0;
    for(; i<4; i++) {
        domain[i] = prefix[i];
    }
    
	for(; i<domain_length+4; i++)
	{
        domain[i] = charlist[rand() % (sizeof(charlist) - 1)];
	}
    
    int s=0;
	for(; i<domain_length+4+4; i++) {
        domain[i] = suffix[s++];
    }
    
    domain[i+1] = '\0';
    return domain;
}

int main() {
    srand((unsigned int)(time(NULL))); // not a good RNG, may use mt19937-64.c ?
    // http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/VERSIONS/C-LANG/mt19937-64.c

    hcreate(max_domain_entry);
    struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    FILE *fp = fopen(domain_txt, "w+");
    if(!fp) {
        printf("Failed to open the file\n");
    }

    for(int i=0; i<max_domain_entry; i++) {
	    const char *domain = random_domain();
        const char *target = random_domain();
        add(domain, target);
        
        fprintf(fp, "%s %s\n", domain, target);
    }
    clock_gettime(CLOCK_MONOTONIC, &tend);
    printf("Elapsed %.5f seconds to generate %d domains\n",
        ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec),
        max_domain_entry);

    clock_gettime(CLOCK_MONOTONIC, &tstart);
    if(!search("www.v5.com")) {
        printf("Not found\n");
    }
    clock_gettime(CLOCK_MONOTONIC, &tend);
    printf("Elapsed %.5f seconds to search\n", ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));

    fflush(fp);
    fclose(fp);
    hdestroy();
    return 0;
}
