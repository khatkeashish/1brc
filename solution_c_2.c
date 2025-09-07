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
    double min;
    double max;
    int    counter;
    double sum;
} CityResult;  // mean computed at dump-time


/* --- Helper: initialize a fresh CityResult --- */
static inline CityResult cityresult_new(void) {
    CityResult cr;
    cr.min  = DBL_MAX;
    cr.max  = -DBL_MAX;
    cr.counter = 0;
    cr.sum  = 0.0;
    return cr;
}


/* ---------- Hash map (string -> CityResult) ---------- */
typedef struct Entry {
    uint64_t hash;
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
static inline uint64_t hash_str(const char *s) {
    uint64_t h = 1469598103934665603ull;
    while (*s) { h ^= (unsigned char)*s++; h *= 1099511628211ull; }
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
            size_t idx = (size_t)(e->hash & (new_cap - 1));
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
    if (!fp) { perror("fopen"); return 1; }

    static char outbuf[1<<20];
    setvbuf(fp, outbuf, _IOFBF, sizeof(outbuf));

    for (size_t i = 0; i < m->capacity; ++i) {
        for (Entry *e = m->buckets[i]; e; e = e->next) {
            double mean = (e->value.counter ? e->value.sum / (double)e->value.counter : 0.0);
            // fprintf is fine with big buffer; if you want, use snprintf + fwrite
            fprintf(fp, "%s;%.6f;%.6f;%.6f\n", e->key, mean, e->value.min, e->value.max);
        }
    }

    fclose(fp);
    return 0;
}


// returns pointer to value in map; inserts a zeroed CityResult if absent
CityResult* citymap_upsert(CityMap *m, const char *key, uint64_t h) {
    if ((m->size * LOAD_FACTOR_DEN) >= (m->capacity * LOAD_FACTOR_NUM)) {
        citymap_rehash(m, m->capacity * 2);
    }
    size_t idx = (size_t)(h & (m->capacity - 1));
    for (Entry *e = m->buckets[idx]; e; e = e->next) {
        if (e->hash == h && strcmp(e->key, key) == 0) {
            return &e->value;
        }
    }
    Entry *ne = (Entry*)malloc(sizeof(Entry));
    if (!ne) { perror("malloc"); exit(1); }
    ne->hash = h;
    ne->key  = strdup_safe(key);
    ne->value.min = 0.0;  // will be set on first insert below
    ne->value.max = 0.0;
    ne->value.counter = 0;
    ne->value.sum = 0.0;
    ne->next = m->buckets[idx];
    m->buckets[idx] = ne;
    m->size++;
    return &ne->value;
}


/* --- Core: read file, split on ';', update hashmap --- */
int process_file(const char *path, CityMap *map) {
    FILE *fp = fopen(path, "r");
    if (!fp) { perror("fopen"); return 1; }

    // Big buffered I/O
    static char inbuf[1<<20];  // 1 MB
    setvbuf(fp, inbuf, _IOFBF, sizeof(inbuf));

    char *line = NULL;
    size_t cap = 0;
    ssize_t len;

    while ((len = getline(&line, &cap, fp)) != -1) {
        if (len == 0) continue;

        // Trim trailing '\n' or '\r\n'
        while (len && (line[len-1] == '\n' || line[len-1] == '\r')) { line[--len] = '\0'; }
        if (len == 0) continue;

        // Find first ';'
        char *sep = memchr(line, ';', (size_t)len);
        if (!sep) continue;

        *sep = '\0';
        const char *city = line;
        const char *value_str = sep + 1;

        char *endp = NULL;
        double v = strtod(value_str, &endp);
        if (endp == value_str) continue;  // parse error

        // upsert once
        uint64_t h = hash_str(city);
        CityResult *cr = citymap_upsert(map, city, h);

        // first sample init (faster, no DBL_MAX compares)
        if (cr->counter == 0) {
            cr->min = v;
            cr->max = v;
            cr->sum = v;
            cr->counter = 1;
        } else {
            if (v < cr->min) cr->min = v;
            if (v > cr->max) cr->max = v;
            cr->sum += v;
            cr->counter += 1;
        }
    }

    free(line);
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
