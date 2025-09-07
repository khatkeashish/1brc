#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <float.h>

#define INITIAL_CAPACITY  1024
#define LOAD_FACTOR_NUM   3
#define LOAD_FACTOR_DEN   4

/* ---------- CityResult ---------- */
typedef struct {
    double mean;
    double min;
    double max;
    int    counter;
    double sum;   // now double
} CityResult;

/* --- Helper: initialize a fresh CityResult --- */
static inline CityResult cityresult_new(void) {
    CityResult cr;
    cr.mean = 0.0;
    cr.min  = DBL_MAX;
    cr.max  = -DBL_MAX;
    cr.counter = 0;
    cr.sum  = 0.0;
    return cr;
}


/* ---------- Hash map (string -> CityResult) ---------- */
typedef struct Entry {
    char *key;
    CityResult value;
    struct Entry *next;
} Entry;

typedef struct {
    Entry **buckets;
    size_t capacity;
    size_t size;
} CityMap;


/* ---------- Hash function ---------- */
static uint64_t hash_str(const char *s) {
    uint64_t h = 1469598103934665603ull;
    while (*s) {
        h ^= (unsigned char)(*s++);
        h *= 1099511628211ull;
    }
    return h;
}

/* ---------- Utilities ---------- */
static char *strdup_safe(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = (char*)malloc(n);
    if (!p) { perror("malloc"); exit(1); }
    memcpy(p, s, n);
    return p;
}

/* ---------- Map core ---------- */
CityMap citymap_new(void) {
    CityMap m;
    m.capacity = INITIAL_CAPACITY;
    m.size = 0;
    m.buckets = (Entry**)calloc(m.capacity, sizeof(Entry*));
    if (!m.buckets) { perror("calloc"); exit(1); }
    return m;
}

static void citymap_free_entries(Entry *e) {
    while (e) {
        Entry *n = e->next;
        free(e->key);
        free(e);
        e = n;
    }
}

void citymap_free(CityMap *m) {
    for (size_t i = 0; i < m->capacity; ++i) {
        citymap_free_entries(m->buckets[i]);
    }
    free(m->buckets);
    m->buckets = NULL;
    m->capacity = 0;
    m->size = 0;
}

static void citymap_rehash(CityMap *m, size_t new_cap) {
    Entry **new_buckets = (Entry**)calloc(new_cap, sizeof(Entry*));
    if (!new_buckets) { perror("calloc"); exit(1); }
    for (size_t i = 0; i < m->capacity; ++i) {
        Entry *e = m->buckets[i];
        while (e) {
            Entry *next = e->next;
            size_t idx = (size_t)(hash_str(e->key) & (new_cap - 1));
            e->next = new_buckets[idx];
            new_buckets[idx] = e;
            e = next;
        }
    }
    free(m->buckets);
    m->buckets = new_buckets;
    m->capacity = new_cap;
}

CityResult* citymap_put(CityMap *m, const char *key, CityResult value) {
    if ((m->size * LOAD_FACTOR_DEN) >= (m->capacity * LOAD_FACTOR_NUM)) {
        citymap_rehash(m, m->capacity * 2);
    }
    size_t idx = (size_t)(hash_str(key) & (m->capacity - 1));
    Entry *e = m->buckets[idx];
    for (; e; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            e->value = value;
            return &e->value;
        }
    }
    Entry *ne = (Entry*)malloc(sizeof(Entry));
    if (!ne) { perror("malloc"); exit(1); }
    ne->key = strdup_safe(key);
    ne->value = value;
    ne->next = m->buckets[idx];
    m->buckets[idx] = ne;
    m->size++;
    return &ne->value;
}

CityResult* citymap_get(CityMap *m, const char *key) {
    size_t idx = (size_t)(hash_str(key) & (m->capacity - 1));
    for (Entry *e = m->buckets[idx]; e; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            return &e->value;
        }
    }
    return NULL;
}

/* Write the results to a file in "city;mean;min;max" format */
int citymap_dump(CityMap *m, const char *outpath) {
    FILE *fp = fopen(outpath, "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    for (size_t i = 0; i < m->capacity; ++i) {
        for (Entry *e = m->buckets[i]; e; e = e->next) {
            fprintf(fp, "%s;%.6f;%.6f;%.6f\n",
                    e->key,
                    e->value.mean,
                    e->value.min,
                    e->value.max);
        }
    }

    fclose(fp);
    return 0;
}

/* --- Core: read file, split on ';', update hashmap --- */
int process_file(const char *path, CityMap *map) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    char line[4096];
    while (fgets(line, sizeof(line), fp)) {
        // strip trailing newline
        size_t n = strcspn(line, "\r\n");
        line[n] = '\0';

        // find the first ';'
        char *sep = strchr(line, ';');
        if (!sep) {
            // malformed line, skip
            continue;
        }

        // split into city and value_str
        *sep = '\0';
        const char *city = line;
        const char *value_str = sep + 1;

        // parse value_str -> double
        char *endp = NULL;
        double v = strtod(value_str, &endp);
        if (endp == value_str) {
            // parse error, skip the line
            continue;
        }

        // lookup or insert CityResult
        CityResult *cr = citymap_get(map, city);
        if (!cr) {
            // create & insert initialized record
            CityResult init = cityresult_new();
            cr = citymap_put(map, city, init);  // returns pointer to stored value
        }

        // update min/max/sum/counter/mean
        if (v < cr->min) cr->min = v;
        if (v > cr->max) cr->max = v;
        cr->sum += v;
        cr->counter += 1;
        cr->mean = cr->sum / (double)cr->counter;
    }

    fclose(fp);
    return 0;
}


int main(void) {
    CityMap map = citymap_new();

    if (process_file("test_sample.txt", &map) != 0) {
        fprintf(stderr, "Failed to process file\n");
        citymap_free(&map);
        return 1;
    }

    if (citymap_dump(&map, "test_sample_results_calculated.txt") != 0) {
        fprintf(stderr, "Failed to write output\n");
    }

    citymap_free(&map);
    return 0;
}
