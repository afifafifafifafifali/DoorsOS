#ifndef FILEMETA_H
#define FILEMETA_H

#ifndef FILE_NAME
#define FILE_NAME        "unknown"
#endif

#ifndef CODE_QUALITY
#define CODE_QUALITY "Unknown - To be Reviewed"
#endif

#ifndef FILE_VERSION
#define FILE_VERSION     "0.0"
#endif

#ifndef FILE_DESCRIPTION
#define FILE_DESCRIPTION "no description"
#endif

#ifndef FILE_AUTHOR
#define FILE_AUTHOR      "unknown"
#endif

#ifndef FILE_LAST_UPDATED_DATE
#define FILE_LAST_UPDATED_DATE "dd/mm/yyyy"
#endif
typedef struct {
    const char* name;
    const char* version;
    const char* description;
    const char* author;
    const char* code_quality;
    const char* file_last_upd_d;
} file_metadata_t;

/* Unique symbol using __COUNTER__ */
#define CONCAT2(x,y) x##y
#define CONCAT(x,y) CONCAT2(x,y)

static const file_metadata_t
__attribute__((used, section(".filemeta")))
CONCAT(__file_metadata_, __COUNTER__) = {
    FILE_NAME,
    FILE_VERSION,
    FILE_DESCRIPTION,
    FILE_AUTHOR,
    CODE_QUALITY,
    FILE_LAST_UPDATED_DATE

};

#endif